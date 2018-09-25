/*
**  igmpproxy - IGMP proxy based multicast router
**  Copyright (C) 2005 Johnny Egeland <johnny@rlo.org>
**
**  This program is free software; you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation; either version 2 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
**
**----------------------------------------------------------------------------
**
**  This software is derived work from the following software. The original
**  source code has been modified from it's original state by the author
**  of igmpproxy.
**
**  smcroute 0.92 - Copyright (C) 2001 Carsten Schill <carsten@cschill.de>
**  - Licensed under the GNU General Public License, version 2
**
**  mrouted 3.9-beta3 - COPYRIGHT 1989 by The Board of Trustees of
**  Leland Stanford Junior University.
**  - Original license can be found in the Stanford.txt file.
**
*/
/**
*   igmpproxy.c - The main file for the IGMP proxy application.
*
*   February 2005 - Johnny Egeland
*/

#include "igmpproxy.h"
#include <sys/sysinfo.h>

static const char Usage[] =
"Usage: igmpproxy [-h] [-d] [-v [-v]] <configfile>\n"
"\n"
"   -h   Display this help screen\n"
"   -d   Run in debug mode. Output all messages on stderr\n"
#ifdef RALINK_ESW_SUPPORT
"   --------------igmp_snooping_config------------------\n"
"   -w	 Wan at port 0/4. Only in switch management mode\n"
"   -n	 Force igmp_snooping disable (default auto)	\n"
#ifdef WIFI_IGMPSNOOP_SUPPORT
"   -a	 Auto enable wifi M2U by client request		\n"
#endif
"   ----------------------------------------------------\n"
#endif
"   -v   Be verbose. Give twice to see even debug messages.\n"
"\n"
PACKAGE_STRING "\n"
;

// Local function Prototypes
static void signalHandler(int);
int     igmpProxyInit();
void    igmpProxyCleanUp();
void    igmpProxyRun();

#ifdef RALINK_ESW_SUPPORT
/* wan port select */
uint32_t WanPort = 0x1;		/* 0000001 */
int auto_lan_snooping = 1;
#ifdef WIFI_IGMPSNOOP_SUPPORT
int auto_wifi_snooping = 0;
#endif
#endif

// Global vars...
static int sighandled = 0;
#define	GOT_SIGINT	0x01
#define	GOT_SIGHUP	0x02
#define	GOT_SIGUSR1	0x04
#define	GOT_SIGUSR2	0x08

// The upstream VIF index
int         upStreamVif;

/**
*   Program main method. Is invoked when the program is started
*   on commandline. The number of commandline arguments, and a
*   pointer to the arguments are recieved on the line...
*/
int main( int ArgCn, char *ArgVc[] ) {

    int c = 0;

    while(1) {
#ifdef RALINK_ESW_SUPPORT
	/* check esw exist */
	FILE *fp = fopen(PROCREG_GMAC, "r");
	if(!fp) {
	    fprintf(stderr, "igmpproxy: proc switch managment file not exist, disable LAN snooping.\n");
	    auto_lan_snooping = 0; /* disable if proc file not exist */
	} else
    	    fclose(fp);

	// set default wan port position (0000001)
	WanPort = 0x1;

	// Parse the commandline options and setup basic settings..
	c = getopt(ArgCn, ArgVc, "dwnvha");
#else
	c = getopt(ArgCn, ArgVc, "dvh");
#endif
	if (c == -1)
	    break;
        switch (c) {
        case 'd':
            Log2Stderr = true;
            break;
#ifdef RALINK_ESW_SUPPORT
        case 'w':
	    WanPort = 0x10; /* 0010000 */
            break;
        case 'n':
	    auto_lan_snooping = 0;
            break;
#ifdef WIFI_IGMPSNOOP_SUPPORT
        case 'a':
	    auto_wifi_snooping = 1;
            break;
#endif
#endif
        case 'v':
            LogLevel++;
            break;
        case 'h':
            fputs(Usage, stderr);
            exit(0);
            break;
        default:
            exit(1);
            break;
        }
    }

    if (optind != ArgCn - 1) {
	fprintf(stderr, "You must specify the configuration file.\n");
	exit(1);
    }
    char *configFilePath = ArgVc[optind];

    // Chech that we are root
    if (geteuid() != 0) {
       fprintf(stderr, "igmpproxy: must be root\n");
       exit(1);
    }

    openlog("igmpproxy", LOG_PID, LOG_USER);

    // Write debug notice with file path...
    my_log(LOG_DEBUG, 0, "Searching for config file at '%s'" , configFilePath);

#ifdef WIFI_IGMPSNOOP_SUPPORT
    // Register WLAN interface
    if (addRTWiFiIntf("ra0") == -1)
        my_log(LOG_WARNING, 0, "Uncorrect wifi interface name.");
#endif
#ifdef RALINK_ESW_SUPPORT
    if(!auto_lan_snooping) {
	my_log(LOG_INFO, 0, "Force LAN igmp_snooping disable.");
    } else {
	my_log(LOG_INFO, 0, "Auto mode LAN igmp_snooping.");
	rt_init();
    }
#endif

    do {

        // Loads the config file...
        if( ! loadConfig( configFilePath ) ) {
            my_log(LOG_ERR, 0, "Unable to load config file...");
            break;
        }

        // Initializes the deamon.
        if ( !igmpProxyInit() ) {
            my_log(LOG_ERR, 0, "Unable to initialize IGMPproxy.");
            break;
        }

	if ( !Log2Stderr ) {
            if ( daemon(1, 0) < 0)
		my_log( LOG_ERR, errno, "failed to detach daemon" );
	}

        // Go to the main loop.
        igmpProxyRun();

        // Clean up
        igmpProxyCleanUp();

    } while ( false );

#ifdef RALINK_ESW_SUPPORT
    if (auto_lan_snooping)
        rt_fini();
#endif

    // Inform that we are exiting.
    my_log(LOG_INFO, 0, "Shutdown complete....");

    exit(0);
}

/* create VIFs for all IP, non-loop interfaces */
void igmpCreateVIFs() {

    unsigned Ix;
    struct IfDesc *Dp;
    int vifcount = 0;
    upStreamVif = -1;

    my_log(LOG_DEBUG, 0, "Create VIFs for all interfaces");

    for ( Ix = 0; (Dp = getIfByIx(Ix)); Ix++ ) {
	my_log(LOG_DEBUG, 0, "getIf by Ix[%d]", Ix);
    	    if ( Dp->InAdr.s_addr && ! (Dp->Flags & IFF_LOOPBACK) && Dp->state != IF_STATE_DISABLED ){
                if(Dp->state == IF_STATE_UPSTREAM) {
		    my_log(LOG_DEBUG, 0, "Dp state is UPSTREAM ViF %d", Ix);
                    if(upStreamVif == -1) {
                        upStreamVif = Ix;
                    } else {
                        my_log(LOG_WARNING, 0, "Vif #%d was already upstream. Cannot set VIF #%d as upstream as well (skipping this VIF).", upStreamVif, Ix);
                        Dp->state = IF_STATE_DISABLED;
		    }
                }
    	     }
	if (Dp->state != IF_STATE_DISABLED) {
    	    addVIF( Dp );
    	    vifcount++;
	}
     }
    // If there is only one VIF, or no defined upstream VIF, we send an error.
    if(vifcount < 2 || upStreamVif < 0)
	my_log(LOG_WARNING, 0, "Warning, No upstream interface assigned.");
}

/**
*   Handles the initial startup of the daemon.
*/
int igmpProxyInit() {
    struct sigaction sa;
    int Err;


    sa.sa_handler = signalHandler;
    sa.sa_flags = 0;    /* Interrupt system calls */
    sigemptyset(&sa.sa_mask);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);

#ifdef RALINK_ESW_SUPPORT
    sa.sa_handler = sigUSR1Handler;
    sa.sa_flags = 0;    /* Interrupt system calls */
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR1, &sa, NULL);
#endif

    // Loads configuration for Physical interfaces...
    buildIfVc();

    // Configures IF states and settings
    configureVifs();

    switch ( Err = enableMRouter() ) {
	case 0: break;
	case EADDRINUSE: my_log( LOG_ERR, EADDRINUSE, "MC-Router API already in use" ); break;
	default: my_log( LOG_ERR, Err, "MRT_INIT failed" );
    }

    //Create ViFs
    igmpCreateVIFs();
    // Initialize IGMP
    initIgmp();
    // Initialize Routing table
    initRouteTable();
    // Initialize timer
    callout_init();

    return 1;
}

/**
*   Clean up all on exit...
*/
void igmpProxyCleanUp() {

    my_log( LOG_DEBUG, 0, "clean handler called" );

    free_all_callouts();    // No more timeouts.
    clearAllRoutes();       // Remove all routes.
    disableMRouter();       // Disable the multirout API

}

/**
*   Main daemon loop.
*/
void igmpProxyRun() {
    // Get the config.
    //struct Config *config = getCommonConfig();
    // Set some needed values.
    register int recvlen;
    int     MaxFD, Rt, secs;
    fd_set  ReadFDS;
    socklen_t dummy = 0;
    struct  timespec  curtime, lasttime, difftime, tv;
    // The timeout is a pointer in order to set it to NULL if nessecary.
    struct  timespec  *timeout = &tv;

    // Initialize timer vars
    difftime.tv_nsec = 0;
    clock_gettime(CLOCK_MONOTONIC, &curtime);
    lasttime = curtime;

    // First thing we send a membership query in downstream VIF's...
    sendGeneralMembershipQuery();

    // Loop until the end...
    for (;;) {

        // Process signaling...
        if (sighandled) {
            if (sighandled & GOT_SIGINT) {
                sighandled &= ~GOT_SIGINT;
                my_log(LOG_NOTICE, 0, "Got a interupt signal. Exiting.");
                break;
	    }
        }

        // Prepare timeout...
        secs = timer_nextTimer();
        if(secs == -1) {
            timeout = NULL;
        } else {
            timeout->tv_nsec = 0;
	    timeout->tv_sec = (secs > INTERVAL_QUERY) ? INTERVAL_QUERY : secs; // limit max select timeout
        }

        // Prepare for select.
        MaxFD = MRouterFD;

        FD_ZERO( &ReadFDS );
        FD_SET( MRouterFD, &ReadFDS );

        // wait for input
        Rt = pselect( MaxFD +1, &ReadFDS, NULL, NULL, timeout, NULL );

        // log and ignore failures
        if( Rt < 0 ) {
            my_log( LOG_WARNING, errno, "select() failure" );
            continue;
        }
        else if( Rt > 0 ) {

            // Read IGMP request, and handle it...
            if( FD_ISSET( MRouterFD, &ReadFDS ) ) {
                recvlen = recvfrom(MRouterFD, recv_buf, RECV_BUF_SIZE, 0, NULL, &dummy);
                if (recvlen < 0) {
                    if (errno != EINTR)
			my_log(LOG_ERR, errno, "recvfrom");
                    continue;
                }
                acceptIgmp(recvlen);
            }
        }

        // At this point, we can handle timeouts...
        do {
            /*
             * If the select timed out, then there's no other
             * activity to account for and we don't need to
             * call clock_gettime.
             */
            if (Rt == 0) {
                curtime.tv_sec = lasttime.tv_sec + secs;
                curtime.tv_nsec = lasttime.tv_nsec;
                Rt = -1; /* don't do this next time through the loop */
            } else {
                clock_gettime(CLOCK_MONOTONIC, &curtime);
            }
	    difftime.tv_sec = curtime.tv_sec - lasttime.tv_sec;
            difftime.tv_nsec += curtime.tv_nsec - lasttime.tv_nsec;
            while (difftime.tv_nsec > 1000000000) {
                difftime.tv_sec++;
                difftime.tv_nsec -= 1000000000;
            }
            if (difftime.tv_nsec < 0) {
                difftime.tv_sec--;
                difftime.tv_nsec += 1000000000;
            }
            lasttime = curtime;
            if (secs == 0 || difftime.tv_sec > 0)
                age_callout_queue(difftime.tv_sec);
            secs = -1;
        } while (difftime.tv_sec > 0);

    }

}

/*
 * Signal handler.  Take note of the fact that the signal arrived
 * so that the main loop can take care of it.
 */
static void signalHandler(int sig) {
    switch (sig) {
    case SIGINT:
    case SIGTERM:
        sighandled |= GOT_SIGINT;
        break;
        /* XXX: Not in use.
        case SIGHUP:
            sighandled |= GOT_SIGHUP;
            break;
        case SIGUSR1:
            sighandled |= GOT_SIGUSR1;
            break;
        case SIGUSR2:
            sighandled |= GOT_SIGUSR2;
            break;
        */
    }
}
