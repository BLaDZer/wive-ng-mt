/***************************************************************************
 *   Copyright (C) 2006 by Kozlov D.   *
 *   xeb@mail.ru   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <net/route.h>
#include <linux/if_pppox.h>

#include "pppd.h"
#include "fsm.h"
#include "lcp.h"
#include "ipcp.h"
#include "ccp.h"
#include "util.h"
#include "pathnames.h"
#include "pptp_callmgr.h"
#include "inststr.h"

#define PPTPC_VERSION "0.8.4"
#define sin_addr(s) (((struct sockaddr_in *)(s))->sin_addr)

extern char** environ;

char pppd_version[] = VERSION;

char *pptp_server = NULL;
char *pptp_client = NULL;
char *pptp_phone = NULL;
int pptp_sock=-1;
int log_level = 0;
struct rtentry rt;
struct in_addr localbind = { .s_addr = INADDR_NONE };

static int callmgr_sock;
static int pptp_fd;
int call_ID;

static int open_callmgr(int call_id,struct in_addr inetaddr, char *phonenr,int window);
static void launch_callmgr(int call_is,struct in_addr inetaddr, char *phonenr,int window);
static int get_call_id(int sock, pid_t gre, pid_t pppd, u_int16_t *peer_call_id);

static int route_add(const struct in_addr inetaddr, struct rtentry *rt);

static option_t Options[] =
{
    { "pptp_server", o_string, &pptp_server,
      "PPTP Server" },
    { "pptp_client", o_string, &pptp_client,
      "PPTP Client" },
    { "pptp_sock",o_int, &pptp_sock,
      "PPTP socket" },
    { "pptp_phone", o_string, &pptp_phone,
      "PPTP Phone number" },
    { "loglevel", o_int, &log_level,
      "debugging level (0=low, 1=default, 2=high)"},
    { NULL }
};

static int pptp_connect(void);
static void pptp_disconnect(void);

struct channel pptp_channel = {
    options: Options,
    check_options: NULL,
    connect: &pptp_connect,
    disconnect: &pptp_disconnect,
    establish_ppp: &generic_establish_ppp,
    disestablish_ppp: &generic_disestablish_ppp,
    close: NULL,
    cleanup: NULL
};

static int pptp_start_server(void)
{
	pptp_fd=pptp_sock;
	sprintf(ppp_devnam,"pptp (%s)",pptp_client);

	return pptp_fd;
}
static int pptp_start_client(void)
{
	socklen_t len;
	struct sockaddr_pppox src_addr,dst_addr;
	struct hostent *hostinfo;

	hostinfo=gethostbyname(pptp_server);
	if (!hostinfo)
	{
		error("PPTP: Unknown host %s\n", pptp_server);
		return -1;
	}
	dst_addr.sa_addr.pptp.sin_addr=*(struct in_addr*)hostinfo->h_addr;

 	memset(&rt, 0, sizeof(rt));
 	route_add(dst_addr.sa_addr.pptp.sin_addr, &rt);

	{
		int sock;
		struct sockaddr_in addr;
		len=sizeof(addr);
		addr.sin_addr=dst_addr.sa_addr.pptp.sin_addr;
		addr.sin_family=AF_INET;
		addr.sin_port=htons(1700);
		sock=socket(AF_INET,SOCK_DGRAM,0);
		if (connect(sock,(struct sockaddr*)&addr,sizeof(addr)))
		{
			close(sock);
			error("PPTP: connect failed (%s). Wait 15 seconds and try again.\n",strerror(errno));
			sleep(15);
			return -1;
		}
		getsockname(sock,(struct sockaddr*)&addr,&len);
		src_addr.sa_addr.pptp.sin_addr=addr.sin_addr;
		close(sock);
	}

	src_addr.sa_family=AF_PPPOX;
	src_addr.sa_protocol=PX_PROTO_PPTP;
	src_addr.sa_addr.pptp.call_id=0;

	dst_addr.sa_family=AF_PPPOX;
	dst_addr.sa_protocol=PX_PROTO_PPTP;
	dst_addr.sa_addr.pptp.call_id=0;

	pptp_fd=socket(AF_PPPOX,SOCK_STREAM,PX_PROTO_PPTP);
	if (pptp_fd<0)
	{
		error("PPTP: failed to create PPTP socket (%s)\n",strerror(errno));
		return -1;
	}
	if (bind(pptp_fd,(struct sockaddr*)&src_addr,sizeof(src_addr)))
	{
		close(pptp_fd);
		error("PPTP: failed to bind PPTP socket (%s)\n",strerror(errno));
		return -1;
	}
	len=sizeof(src_addr);
	getsockname(pptp_fd,(struct sockaddr*)&src_addr,&len);
	call_ID=src_addr.sa_addr.pptp.call_id;

        /*
         * Open connection to call manager (Launch call manager if necessary.)
         */
	callmgr_sock = -1;
	do {
		if (callmgr_sock >= 0)
    			close(callmgr_sock);
		callmgr_sock = open_callmgr(src_addr.sa_addr.pptp.call_id, dst_addr.sa_addr.pptp.sin_addr, pptp_phone, 50);
		if (callmgr_sock < 0)
		{
			close(pptp_fd);
			return -1;
		}
	/* Exchange PIDs, get call ID */
	} while (get_call_id(callmgr_sock, getpid(), getpid(), &dst_addr.sa_addr.pptp.call_id) < 0);

	if (connect(pptp_fd,(struct sockaddr*)&dst_addr,sizeof(dst_addr)))
	{
		close(callmgr_sock);
		close(pptp_fd);
		error("PPTP: failed to connect PPTP socket (%s)\n",strerror(errno));
		return -1;
	}

	sprintf(ppp_devnam,"pptp (%s)", inet_ntoa(dst_addr.sa_addr.pptp.sin_addr));

	return pptp_fd;
}
static int pptp_connect(void)
{
	if ((!pptp_server && !pptp_client) || (pptp_server && pptp_client))
	{
		fatal("PPTP: unknown mode (you must specify pptp_server or pptp_client option)");
		return -1;
	}

	if (pptp_server) return pptp_start_client();
	return pptp_start_server();
}

static void pptp_disconnect(void)
{
	if (pptp_server) 
	    close(callmgr_sock);

	close(pptp_fd);
}

static int open_callmgr(int call_id,struct in_addr inetaddr, char *phonenr,int window)
{
    /* Try to open unix domain socket to call manager. */
    struct sockaddr_un where;
    const int NUM_TRIES = 3;
    int i, fd;
    pid_t pid;
    int status;
    /* Open socket */
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        fatal("Could not create unix domain socket: %s", strerror(errno));
    }
    /* Make address */
    callmgr_name_unixsock(&where, inetaddr, localbind);
    for (i = 0; i < NUM_TRIES; i++)
    {
        if (connect(fd, (struct sockaddr *) &where, sizeof(where)) < 0)
        {
            /* couldn't connect.  We'll have to launch this guy. */

	    unlink (where.sun_path); /* FIXME: potential race condition */

            /* fork and launch call manager process */
            switch (pid = fork())
            {
                case -1: /* failure */
                    fatal("fork() to launch call manager failed.");
                case 0: /* child */
                {
                    close (fd);
		    close(pptp_fd);
                    launch_callmgr(call_id,inetaddr, phonenr,window);
                }
                default: /* parent */
                    waitpid(pid, &status, 0);
		    if (WIFEXITED(status))
			status = WEXITSTATUS(status);
			if (status != 0)
		    {
			close(fd);
			error("Call manager exited with error %d", status);
			return -1;
		    }
                    break;
            }
            sleep(1);
        }
        else return fd;
    }
    close(fd);
    error("Could not launch call manager after %d tries.", i);
    return -1;   /* make gcc happy */
}

/*** call the call manager main ***********************************************/
static void launch_callmgr(int call_id, struct in_addr inetaddr, char *phonenr, int window)
{
	char win[10];
	char call[10];
	char *my_argv[9] = { "pptp", inet_ntoa(inetaddr), "--call_id", call, "--phone", phonenr, "--window", win, NULL };
	char buf[128];
	int argc = 0;
	char **argv = environ;

	sprintf(win, "%u", window);
	sprintf(call, "%u", call_id);
	snprintf(buf, sizeof(buf), "pptp: call manager for %s", my_argv[1]);

	if (argv && *argv)
		argv--;
	if (argv && *argv == NULL && progname)
	do {
		argv--;
		argc++;
	} while (argv && *argv && *argv > progname);
	if (argv && *argv == progname)
		inststr(argc, argv, environ, buf);

	exit(callmgr_main(8, my_argv, environ));
}

/*** exchange data with the call manager  *************************************/
/* XXX need better error checking XXX */
static int get_call_id(int sock, pid_t gre, pid_t pppd,
		u_int16_t *peer_call_id)
{
    u_int16_t m_call_id, m_peer_call_id;
    /* write pid's to socket */
    /* don't bother with network byte order, because pid's are meaningless
     * outside the local host.
     */
    int rc;
    rc = write(sock, &gre, sizeof(gre));
    if (rc != sizeof(gre))
        return -1;
    rc = write(sock, &pppd, sizeof(pppd));
    if (rc != sizeof(pppd))
        return -1;
    rc = read(sock,  &m_call_id, sizeof(m_call_id));
    if (rc != sizeof(m_call_id))
        return -1;
    rc = read(sock,  &m_peer_call_id, sizeof(m_peer_call_id));
    if (rc != sizeof(m_peer_call_id))
        return -1;
    /*
     * XXX FIXME ... DO ERROR CHECKING & TIME-OUTS XXX
     * (Rhialto: I am assuming for now that timeouts are not relevant
     * here, because the read and write calls would return -1 (fail) when
     * the peer goes away during the process. We know it is (or was)
     * running because the connect() call succeeded.)
     * (James: on the other hand, if the route to the peer goes away, we
     * wouldn't get told by read() or write() for quite some time.)
     */
    *peer_call_id = m_peer_call_id;
    return 0;
}

void plugin_init(void)
{
#if defined(__linux__)
    extern int new_style_driver;	/* From sys-linux.c */
    if (!ppp_available() && !new_style_driver)
	fatal("Kernel doesn't support ppp_generic - needed for PPTP");
#else
	fatal("No PPTP support on this OS");
#endif
    add_options(Options);

    info("PPTP plugin version %s compiled for pppd-%s\n", PPTPC_VERSION, VERSION);

    the_channel = &pptp_channel;
    modem = 0;
}

static int
route_ctrl(int ctrl, struct rtentry *rt)
{
	int s;

	/* Open a raw socket to the kernel */
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ||	ioctl(s, ctrl, rt) < 0)
	        log("route_ctrl: %s", strerror(errno));
	else errno = 0;

	close(s);
	return errno;
}

static int
route_add(const struct in_addr inetaddr, struct rtentry *rt)
{
	char buf[256], dev[64];
	int metric, flags;
	u_int32_t dest, mask;

	FILE *f = fopen("/proc/net/route", "r");
	if (f == NULL) {
	        warn("/proc/net/route: %s", strerror(errno));
		return -1;
	}

	while (fgets(buf, sizeof(buf), f)) 
	{
		if (sscanf(buf, "%63s %x %x %X %*s %*s %d %x", dev, &dest,
		    	&sin_addr(&rt->rt_gateway).s_addr, &flags, &metric, &mask) != 6)
			continue;
		if ((flags & RTF_UP) == RTF_UP && (inetaddr.s_addr & mask) == dest &&
			(dest || strncmp(dev, "ppp", 3)) /* avoid default via pppX to avoid on-demand loops*/)
		{
			rt->rt_metric = metric;
			rt->rt_gateway.sa_family = AF_INET;
			break;
		}
	}

	fclose(f);

	/* check for no route */
	if (rt->rt_gateway.sa_family != AF_INET) 
	{
		/* warn("route_add: no route to host"); */
		return -1;
	}

	/* check for existing route to this host, 
	add if missing based on the existing routes */
	if (flags & RTF_HOST) {
		/* warn("route_add: not adding existing route"); */
		return -1;
	}

	sin_addr(&rt->rt_dst) = inetaddr;
	rt->rt_dst.sa_family = AF_INET;

	sin_addr(&rt->rt_genmask).s_addr = INADDR_BROADCAST;
	rt->rt_genmask.sa_family = AF_INET;

	rt->rt_flags = RTF_UP | RTF_HOST;
	if (flags & RTF_GATEWAY)
		rt->rt_flags |= RTF_GATEWAY;

	rt->rt_metric++;
	rt->rt_dev = strdup(dev);

	if (!rt->rt_dev)
	{
	        warn("route_add: no memory");
		return -1;
	}

	if (!route_ctrl(SIOCADDRT, rt))
		return 0;

	free(rt->rt_dev), rt->rt_dev = NULL;

	return -1;
}
