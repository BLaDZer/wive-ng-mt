/*
 ***************************************************************************
 * MediaTek Inc. 
 *
 * All rights reserved. source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of MediaTek. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of MediaTek, Inc. is obtained.
 ***************************************************************************

    Module Name:
    	main.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#include "bndstrg.h"

int DebugLevel = DEBUG_ERROR;

extern struct bndstrg_event_ops bndstrg_event_ops;

static void usage()
{

	printf("-d <bndstrg debug level> 0..4\n");
}

static void process_options(int argc, char *argv[])
{
	int c;
	int debug = DEBUG_ERROR;

	while ((c = getopt(argc, argv, "hd:")) != -1) {
		switch (c) {
		    case 'd':
			debug = atoi(optarg);
			if (debug >= 0 && debug <= DEBUG_INFO) {
				DebugLevel = debug;
			} else {
				printf("-d option does not have this debug_level %d, must be 0..4 range.\n", debug);
				usage();
				exit(0);
			}
			break;
		    case 'h':
			usage();
			exit(0);
		    default:
			usage();
			exit(0);
		}
	}
}

int main(int argc, char *argv[])
{

	struct bndstrg bndstrg;
	pid_t child_pid;

#ifdef SYSLOG
	openlog("bndstrg", LOG_PID|LOG_NDELAY, LOG_DAEMON);
#endif

	/* options processing */
	process_options(argc, argv);

	child_pid = fork();

	if (child_pid == 0) {
		int ret = 0;
		DBGPRINT(DEBUG_OFF, "Initialize bndstrg\n");
		ret = bndstrg_init(&bndstrg, &bndstrg_event_ops, 0, 0, 2);

		if (ret)
			goto error;

		bndstrg_run(&bndstrg);

	} else
		goto error;
#if 0
error0:
	bndstrg_deinit(&hs);
#endif
error:
	return -1;
}
