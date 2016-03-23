/* vi: set sw=4 ts=4 sts=4: */
/*
 * main.c -- Main program for the GoAhead WebServer (LINUX version)
 *
 * Copyright (c) GoAhead Software Inc., 1995-2000. All Rights Reserved.
 *
 * See the file "license.txt" for usage and redistribution license requirements
 *
 * $Id: goahead.c,v 1.100.2.4 2009-04-08 08:52:59 chhung Exp $
 */

/******************************** Description *********************************/

/*
 *	Main program for for the GoAhead WebServer. This is a demonstration
 *	main program to initialize and configure the web server.
 */

/********************************* Includes ***********************************/

#include	"utils.h"
#include	"helpers.h"
#include	"uemf.h"
#include	"wsIntrn.h"
#include	"internet.h"
#include	"services.h"
#include	"firewall.h"
#include	"management.h"
#include	"station.h"
#include	"usb.h"

#ifdef WEBS_SSL_SUPPORT
#include	"websSSL.h"
#endif

#ifdef USER_MANAGEMENT_SUPPORT
#include	"um.h"
void	formDefineUserMgmt(void);
#endif

/*********************************** Locals ***********************************/
static char_t	*rootWeb = T("/tmp/web");		/* Root web directory */
static char_t	*password = T("");			/* Security password */
static char_t	*gopid = T("/var/run/goahead.pid");	/* pid file */
static int	port = 80;				/* Server port */
static int	retries = 5;				/* Server port retries */
static int	finished = 0;				/* Finished flag */
static int	firstboot = 1;				/* First boot flag */

#ifdef B_STATS
static void printMemStats(int handle, char_t *fmt, ...);
static void memLeaks();
#endif

/****************************** Forward Declarations **************************/
extern void defaultErrorHandler(int etype, char_t *msg);
extern void defaultTraceHandler(int level, char_t *buf);
extern void formDefineWireless(void);

/******************************************************************************/
/*
 *	Write pid to the pid file
 */
static int writeGoPid(void)
{
	FILE *fp;

	fp = fopen(gopid, "w+");
	if (NULL == fp) {
		syslog(LOG_ERR, "cannot open pid file, %s", __FUNCTION__);
		return (-1);
	}
	fprintf(fp, "%d", getpid());
	fclose(fp);

	return 0;
}

/******************************************************************************/
/*
 *	Home page handler
 */
static int websHomePageHandler(webs_t wp, char_t *urlPrefix, char_t *webDir,
	int arg, char_t *url, char_t *path, char_t *query)
{
	/*
	 *	If the empty or "/" URL is invoked, redirect default URLs to the home page
	 */
	if (*url == '\0' || strcmp(url, T("/")) == 0) {
		websRedirect(wp, T("home.asp"));
		return 1;
	}
	return 0;
}

/******************************************************************************/
/*
 *	Initialize the web server.
 */
static int initWebs(void)
{
	struct in_addr	intaddr;
	char			*lan_ip = nvram_get(RT2860_NVRAM, "lan_ipaddr");
	char			webdir[128];
	char			*cp;
	char_t			wbuf[128];
	int			web_port=80;

	/*
	 *	Initialize the socket subsystem
	 */
	socketOpen();

#ifdef USER_MANAGEMENT_SUPPORT
	/*
	 *	Initialize the User Management database
	 */
	char *admu = nvram_get(RT2860_NVRAM, "Login");
	char *admp = nvram_get(RT2860_NVRAM, "Password");
	umOpen();
	/* umRestore(T("umconfig.txt"));
	 * winfred: instead of using umconfig.txt, we create 'the one' adm defined in nvram
	 */
	umAddGroup(T("adm"), 0x07, AM_DIGEST, FALSE, FALSE);
	if (admu && strcmp(admu, "") && admp && strcmp(admp, "")) {
		umAddUser(admu, admp, T("adm"), FALSE, FALSE);
		umAddAccessLimit(T("/"), AM_DIGEST, FALSE, T("adm"));
	}
	else
		syslog(LOG_WARNING, "empty administrator account or password, %s", __FUNCTION__);
#endif

	/*
	 * get ip address from nvram configuration (we executed initInternet)
	 */
	if (NULL == lan_ip) {
		syslog(LOG_ERR, "cannot find lan_ip in NVRAM, %s", __FUNCTION__);
		return -1;
	}
	intaddr.s_addr = inet_addr(lan_ip);
	if (intaddr.s_addr == INADDR_NONE) {
		syslog(LOG_ERR, "failed to convert %s to binary ip data, %s", lan_ip, __FUNCTION__);
		return -1;
	}

	/*
	 *	Set rootWeb as the root web. Modify this to suit your needs
	 */
	sprintf(webdir, "%s", rootWeb);

	/*
	 *	Configure the web server options before opening the web server
	 */
	websSetDefaultDir(webdir);
	cp = inet_ntoa(intaddr);
	memcpy(wbuf, cp, min(strlen(cp) + 1, sizeof(wbuf)));
	websSetIpaddr(wbuf);
	/* use ip address (already in wbuf) as host */
	websSetHost(wbuf);

	/*
	 *	Configure the web server options before opening the web server
	 */
	websSetDefaultPage(T("default.asp"));
	websSetPassword(password);

	/*
	 *	Open the web server on the given port. If that port is taken, try
	 *	the next sequential port for up to "retries" attempts.
	 */
	web_port = atoi(nvram_get(RT2860_NVRAM, "RemoteManagementPort"));
	if (web_port && web_port != 80)
	    port=web_port;

	websOpenServer(port, retries);

	/*
	 * 	First create the URL handlers. Note: handlers are called in sorted order
	 *	with the longest path handler examined first. Here we define the security 
	 *	handler, forms handler and the default web page handler.
	 */
	websUrlHandlerDefine(T(""), NULL, 0, websSecurityHandler, WEBS_HANDLER_FIRST);
	websUrlHandlerDefine(T("/goform"), NULL, 0, websFormHandler, 0);
	websUrlHandlerDefine(T("/cgi-bin"), NULL, 0, websCgiHandler, 0);
	websUrlHandlerDefine(T(""), NULL, 0, websDefaultHandler, WEBS_HANDLER_LAST); 

	/*
	 *	Define our functions
	 */
	formDefineUtilities();
	formDefineInternet();
	formDefineServices();
#ifdef CONFIG_RALINKAPP_SWQOS
	formDefineQoS();
#endif
#ifdef CONFIG_USB
	formDefineUSB();
#endif
#ifdef CONFIG_USER_STORAGE
	formDefineSTORAGE();
#endif
	formDefineWireless();
#if defined(CONFIG_RT2860V2_STA) || defined(CONFIG_RT2860V2_STA_MODULE) || defined(CONFIG_MT76X2_STA) || defined(CONFIG_MT76X2_STA_MODULE)
	formDefineStation();
#endif
	formDefineFirewall();
	formDefineManagement();

	/*
	 *	Create a handler for the default home page
	 */
	websUrlHandlerDefine(T("/"), NULL, 0, websHomePageHandler, 0); 
	return 0;
}

/*********************************** Code *************************************/
/*
 *	Main -- entry point from LINUX
 */

static const char Usage[] =
"Usage: goahead [-h] [-f] [-r]\n"
"\n"
"   -h   Display this help screen\n"
"   -f   Start foreground\n"
"   -r   Force restore mode without full reinit\n"
"\n";

int main(int argcn, char *argvc[])
{
	int c, foreground=0, restore=0;

	while ((c = getopt(argcn, argvc, "frh")) != -1) {
    	    switch (c) {
    	    case 'f':
		foreground=1;
		break;
    	    case 'r':
		restore=1;
		break;
    	    case 'h':
        	fputs(Usage, stderr);
        	exit(0);
        	break;
    	    default:
        	break;
    	    }
	}

	openlog("goahead", LOG_PID|LOG_NDELAY, LOG_USER);

	/* Daemonize */
	if (!foreground) {
	    if (daemon(0,0)) {
		syslog(LOG_ERR, "cannot daemonize - exit!");
		return -1;
	    }
	}

	/* check pid file allready exist */
	if(!access(gopid, F_OK )){
		syslog(LOG_INFO, "pid file exist, remove old pidfile and start in restore mode.");
		firstboot = 0;
		unlink(gopid);
	}

	/* Write pidfile */
	if (writeGoPid() < 0) {
		syslog(LOG_ERR, "cannot write pid - exit!");
		return -1;
	}

	/* ignore some signals */
	signal(SIGHUP,  SIG_IGN);
	signal(SIGUSR1, SIG_IGN);
	signal(SIGUSR2, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);

	/*
	 *	Initialize the memory allocator. Allow use of malloc and start
	 *	with a 256K heap.  For each page request approx 64KB is allocated.
	 *	256KB allows for several concurrent page requests.  If more space
	 *	is required, malloc will be used for the overflow.
	 */
	bopen(NULL, (256 * 1024), B_USE_MALLOC);

	/* Initialize the web server */
	if (initWebs() < 0) {
		/* Clean-up and exit */
		syslog(LOG_ERR, "web pages not exists - exit!");
		return -1;
	} else {
#ifdef WEBS_SSL_SUPPORT
	    websSSLOpen();
#endif
    	    /* Start needed services */
	    if (firstboot && !restore) {
		initInternet();
#ifdef CONFIG_USB
		/* Rescan usb devices after start */
		doSystem("service hotplug rescan");
#endif
		ledAlways(CONFIG_RALINK_GPIO_SYS_LED, LED_ON);		//Turn on power LED
	    }
	    syslog(LOG_INFO, "version %s started", WEBS_VERSION);
	}

	/*
	 *	Basic event loop. SocketReady returns true when a socket is ready for
	 *	service. SocketSelect will block until an event occurs. SocketProcess
	 *	will actually do the servicing.
	 */
	while (!finished) {
		if (socketReady(-1) || socketSelect(-1, 1000)) {
			socketProcess(-1);
		}
		websCgiCleanup();
		emfSchedProcess();
	}

#ifdef WEBS_SSL_SUPPORT
	websSSLClose();
#endif
#ifdef USER_MANAGEMENT_SUPPORT
	umClose();
#endif
	/*
	 *	Close the socket module, report memory leaks and close the memory allocator
	 */
	websCloseServer();
	socketClose();
#ifdef B_STATS
	memLeaks();
#endif
	ledAlways(CONFIG_RALINK_GPIO_SYS_LED, LED_OFF);			//Turn off power LED

	bclose();

	return 0;
}


/******************************************************************************/
/*
 *	Default error handler.  The developer should insert code to handle
 *	error messages in the desired manner.
 */
void defaultErrorHandler(int etype, char_t *msg)
{
	write(1, msg, strlen(msg));
}

/******************************************************************************/
/*
 *	Trace log. Customize this function to log trace output
 */
void defaultTraceHandler(int level, char_t *buf)
{
	/*
	 *	The following code would write all trace regardless of level
	 *	to stdout.
	 */
	if (buf) {
		if (0 == level)
			write(1, buf, strlen(buf));
	}
}

/******************************************************************************/
/*
 *	Returns a pointer to an allocated qualified unique temporary file name.
 *	This filename must eventually be deleted with bfree();
 */
char_t *websGetCgiCommName(webs_t wp)
{
	char_t	*pname1, *pname2;

	pname1 = (char_t *)tempnam(T("/var"), T("cgi"));
	pname2 = bstrdup(B_L, pname1);
	free(pname1);

	return pname2;
}

/******************************************************************************/
/*
 *	Launch the CGI process and return a handle to it.
 */
int websLaunchCgiProc(char_t *cgiPath, char_t **argp, char_t **envp,
					  char_t *stdIn, char_t *stdOut)
{
	int	pid, fdin, fdout, hstdin, hstdout, rc;

	fdin = fdout = hstdin = hstdout = rc = -1; 
	if ((fdin = open(stdIn, O_RDWR | O_CREAT, 0666)) < 0 ||
		(fdout = open(stdOut, O_RDWR | O_CREAT, 0666)) < 0 ||
		(hstdin = dup(0)) == -1 ||
		(hstdout = dup(1)) == -1 ||
		dup2(fdin, 0) == -1 ||
		dup2(fdout, 1) == -1) {
		goto DONE;
	}

 	rc = pid = fork();
 	if (pid == 0) {
	    /*
	    * if pid == 0, then we are in the child process
	    */
	    if (execve(cgiPath, argp, envp) == -1)
		printf("content-type: text/html\n\n Execution of cgi process failed\n");

	    exit (0);
	}

DONE:
	if (hstdout >= 0) {
	    dup2(hstdout, 1);
	    close(hstdout);
	}
	if (hstdin >= 0) {
	    dup2(hstdin, 0);
	    close(hstdin);
	}
	if (fdout >= 0) {
	    close(fdout);
	}
	if (fdin >= 0) {
	    close(fdin);
	}
	return rc;
}

/******************************************************************************/
/*
 *	Check the CGI process.  Return 0 if it does not exist; non 0 if it does.
 */
int websCheckCgiProc(int handle, int *status)
{
	/*
	 *	Check to see if the CGI child process has terminated or not yet.
	 */
	if (waitpid(handle, status, WNOHANG) == handle) {
		return 0;
	} else {
		return 1;
	}
}

/******************************************************************************/
#ifdef B_STATS
static void memLeaks()
{
	int		fd;

	if ((fd = open(T("leak.txt"), O_CREAT | O_TRUNC | O_WRONLY, 0666)) >= 0) {
		bstats(fd, printMemStats);
		close(fd);
	}
}

/******************************************************************************/
/*
 *	Print memory usage / leaks
 */
static void printMemStats(int handle, char_t *fmt, ...)
{
	va_list		args;
	char_t		buf[256];

	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);
	write(handle, buf, strlen(buf));
}
#endif
