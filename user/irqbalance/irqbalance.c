/* 
 * Copyright (C) 2006, Intel Corporation
 * Copyright (C) 2012, Neil Horman <nhorman@tuxdriver.com> 
 * 
 * This file is part of irqbalance
 *
 * This program file is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; version 2 of the License.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program in a file named COPYING; if not, write to the 
 * Free Software Foundation, Inc., 
 * 51 Franklin Street, Fifth Floor, 
 * Boston, MA 02110-1301 USA
 */
#include "config.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <sys/time.h>
#include <syslog.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#ifdef HAVE_GETOPT_LONG 
#include <getopt.h>
#endif

#ifdef HAVE_LIBCAP_NG
#include <cap-ng.h>
#endif
#include "irqbalance.h"

volatile int keep_going = 1;
int socket_fd;
int one_shot_mode;
int debug_mode;
int foreground_mode;
int numa_avail;
int journal_logging = 0;
int need_rescan;
unsigned int log_mask = TO_ALL;
const char *log_indent;
unsigned long power_thresh = ULONG_MAX;
unsigned long deepest_cache = 2;
unsigned long long cycle_count = 0;
char *pidfile = NULL;
char *banscript = NULL;
char *polscript = NULL;
long HZ;
int sleep_interval = SLEEP_INTERVAL;
GMainLoop *main_loop;

char *banned_cpumask_from_ui = NULL;

static void sleep_approx(int seconds)
{
	struct timespec ts;
	struct timeval tv;
	gettimeofday(&tv, NULL);
	ts.tv_sec = seconds;
	ts.tv_nsec = -tv.tv_usec*1000;
	while (ts.tv_nsec < 0) {
		ts.tv_sec--;
		ts.tv_nsec += 1000000000;
	}
	nanosleep(&ts, NULL);
}

#ifdef HAVE_GETOPT_LONG
struct option lopts[] = {
	{"oneshot", 0, NULL, 'o'},
	{"debug", 0, NULL, 'd'},
	{"foreground", 0, NULL, 'f'},
	{"hintpolicy", 1, NULL, 'h'},
	{"powerthresh", 1, NULL, 'p'},
	{"banirq", 1 , NULL, 'i'},
	{"banscript", 1, NULL, 'b'},
	{"deepestcache", 1, NULL, 'c'},
	{"policyscript", 1, NULL, 'l'},
	{"pid", 1, NULL, 's'},
	{"journal", 0, NULL, 'j'},
	{"banmod", 1 , NULL, 'm'},
	{"interval", 1 , NULL, 't'},
	{0, 0, 0, 0}
};

static void usage(void)
{
	log(TO_CONSOLE, LOG_INFO, "irqbalance [--oneshot | -o] [--debug | -d] [--foreground | -f] [--journal | -j] [--hintpolicy= | -h [exact|subset|ignore]]\n");
	log(TO_CONSOLE, LOG_INFO, "	[--powerthresh= | -p <off> | <n>] [--banirq= | -i <n>] [--banmod= | -m <module>] [--policyscript= | -l <script>]\n");
	log(TO_CONSOLE, LOG_INFO, "	[--pid= | -s <file>] [--deepestcache= | -c <n>] [--interval= | -t <n>]\n");
}

static void parse_command_line(int argc, char **argv)
{
	int opt;
	int longind;
	unsigned long val;

	while ((opt = getopt_long(argc, argv,
		"odfji:p:s:c:b:l:m:t:",
		lopts, &longind)) != -1) {

		switch(opt) {
			case '?':
				usage();
				exit(1);
				break;
			case 'b':
#ifndef INCLUDE_BANSCRIPT
				/*
				 * Banscript is no longer supported unless
				 * explicitly enabled
				 */
				log(TO_CONSOLE, LOG_INFO, "--banscript is not supported on this version of irqbalance, please use --policyscript\n");
				usage();
				exit(1);
#endif
				banscript = strdup(optarg);
				break;
			case 'c':
				deepest_cache = strtoul(optarg, NULL, 10);
				if (deepest_cache == ULONG_MAX || deepest_cache < 1) {
					usage();
					exit(1);
				}
				break;
			case 'd':
				debug_mode=1;
				foreground_mode=1;
				break;
			case 'f':
				foreground_mode=1;
				break;
			case 'i':
				val = strtoull(optarg, NULL, 10);
				if (val == ULONG_MAX) {
					usage();
					exit(1);
				}
				add_cl_banned_irq((int)val);
				break;
			case 'l':
				polscript = strdup(optarg);
				break;
			case 'm':
				add_cl_banned_module(optarg);
				break;
			case 'p':
				if (!strncmp(optarg, "off", strlen(optarg)))
					power_thresh = ULONG_MAX;
				else {
					power_thresh = strtoull(optarg, NULL, 10);
					if (power_thresh == ULONG_MAX) {
						usage();
						exit(1);
					}
				}
				break;
			case 'o':
				one_shot_mode=1;
				break;
			case 's':
				pidfile = optarg;
				break;
			case 'j':
				journal_logging=1;
				foreground_mode=1;
				break;
			case 't':
				sleep_interval = strtol(optarg, NULL, 10);
				if (sleep_interval < 1) {
					usage();
					exit(1);
				}
				break;
		}
	}
}
#endif

/*
 * This builds our object tree.  The Heirarchy is pretty straightforward
 * At the top are numa_nodes
 * All CPU packages belong to a single numa_node
 * All Cache domains belong to a CPU package
 * All CPU cores belong to a cache domain
 *
 * Objects are built in that order (top down)
 *
 * Object workload is the aggregate sum of the
 * workload of the objects below it
 */
static void build_object_tree(void)
{
	build_numa_node_list();
	parse_cpu_tree();
	rebuild_irq_db();
}

static void free_object_tree(void)
{
	free_numa_node_list();
	clear_cpu_tree();
	free_irq_db();
}

static void dump_object_tree(void)
{
	for_each_object(numa_nodes, dump_numa_node_info, NULL);
}

static void force_rebalance_irq(struct irq_info *info, void *data __attribute__((unused)))
{
	if (info->level == BALANCE_NONE)
		return;

	if (info->assigned_obj == NULL)
		rebalance_irq_list = g_list_append(rebalance_irq_list, info);
	else
		migrate_irq(&info->assigned_obj->interrupts, &rebalance_irq_list, info);

	info->assigned_obj = NULL;
}

gboolean handler(gpointer data __attribute__((unused)))
{
	keep_going = 0;
	g_main_loop_quit(main_loop);
	return TRUE;
}

gboolean force_rescan(gpointer data __attribute__((unused)))
{
	if (cycle_count)
		need_rescan = 1;
	return TRUE;
}

gboolean scan(gpointer data)
{
	log(TO_CONSOLE, LOG_INFO, "\n\n\n-----------------------------------------------------------------------------\n");
	clear_work_stats();
	parse_proc_interrupts();
	parse_proc_stat();


	/* cope with cpu hotplug -- detected during /proc/interrupts parsing */
	if (need_rescan) {
		need_rescan = 0;
		cycle_count = 0;
		log(TO_CONSOLE, LOG_INFO, "Rescanning cpu topology \n");
		clear_work_stats();

		free_object_tree();
		build_object_tree();
		for_each_irq(NULL, force_rebalance_irq, NULL);
		parse_proc_interrupts();
		parse_proc_stat();
		sleep_approx(sleep_interval);
		clear_work_stats();
		parse_proc_interrupts();
		parse_proc_stat();
	} 

	if (cycle_count)	
		update_migration_status();

	calculate_placement();
	activate_mappings();

	if (debug_mode)
		dump_tree();
	if (one_shot_mode)
		keep_going = 0;
	cycle_count++;

	if (data != &sleep_interval) {
		data = &sleep_interval;
		g_timeout_add_seconds(sleep_interval, scan, data);
		return FALSE;
	}

	if (keep_going)
		return TRUE;
	else
		return FALSE;
}

void get_irq_data(struct irq_info *irq, void *data)
{
	sprintf(data + strlen(data),
			"IRQ %d LOAD %lu DIFF %lu CLASS %d ", irq->irq, irq->load,
			(irq->irq_count - irq->last_irq_count), irq->class);
}

void get_object_stat(struct topo_obj *object, void *data)
{
	char irq_data[1024] = "\0";

	if (g_list_length(object->interrupts) > 0) {
		for_each_irq(object->interrupts, get_irq_data, irq_data);
	}
	sprintf(data + strlen(data), "TYPE %d NUMBER %d LOAD %lu SAVE_MODE %d %s",
			object->obj_type, object->number, object->load,
			object->powersave_mode, irq_data);
	if (object->obj_type != OBJ_TYPE_CPU) {
		for_each_object(object->children, get_object_stat, data);
	}
}

gboolean sock_handle(gint fd, GIOCondition condition, gpointer user_data __attribute__((unused)))
{
	char buff[500];
	int sock;
	int recv_size = 0;
	int valid_user = 0;

	struct iovec iov = { buff, 500 };
	struct msghdr msg = { NULL, 0, &iov, 1, NULL, 0, 0 };
	msg.msg_control = malloc(CMSG_SPACE(sizeof(struct ucred)));
	msg.msg_controllen = CMSG_SPACE(sizeof(struct ucred));

	struct cmsghdr *cmsg;

	if (condition == G_IO_IN) {
		sock = accept(fd, NULL, NULL);
		if (sock < 0) {
			log(TO_ALL, LOG_WARNING, "Connection couldn't be accepted.\n");
			return TRUE;
		}
		if ((recv_size = recvmsg(sock, &msg, 0)) < 0) {
			log(TO_ALL, LOG_WARNING, "Error while receiving data.\n");
			return TRUE;
		}
		cmsg = CMSG_FIRSTHDR(&msg);
		if ((cmsg->cmsg_level == SOL_SOCKET) &&
				(cmsg->cmsg_type == SCM_CREDENTIALS)) {
			struct ucred *credentials = (struct ucred *) CMSG_DATA(cmsg);
			if (!credentials->uid) {
				valid_user = 1;
			}
		}
		if (!valid_user) {
			log(TO_ALL, LOG_INFO, "Permission denied for user to connect to socket.\n");
			return TRUE;
		}

		if (!strncmp(buff, "stats", strlen("stats"))) {
			char stats[2048] = "\0";
			for_each_object(numa_nodes, get_object_stat, stats);
			send(sock, stats, strlen(stats), 0);
		}
		if (!strncmp(buff, "settings ", strlen("settings "))) {
			if (!(strncmp(buff + strlen("settings "), "sleep ",
							strlen("sleep ")))) {
				char *sleep_string = malloc(
						sizeof(char) * (recv_size - strlen("settings sleep ")));
				strncpy(sleep_string, buff + strlen("settings sleep "),
						recv_size - strlen("settings sleep "));
				int new_iterval = strtoul(sleep_string, NULL, 10);
				if (new_iterval >= 1) {
					sleep_interval = new_iterval;
				}
			} else if (!(strncmp(buff + strlen("settings "), "ban irqs ",
							strlen("ban irqs ")))) {
				char *end;
				char *irq_string = malloc(
						sizeof(char) * (recv_size - strlen("settings ban irqs ")));
				strncpy(irq_string, buff + strlen("settings ban irqs "),
						recv_size - strlen("settings ban irqs "));
				g_list_free_full(cl_banned_irqs, free);
				cl_banned_irqs = NULL;
				need_rescan = 1;
				if (!strncmp(irq_string, "NONE", strlen("NONE"))) {
					return TRUE;
				}
				int irq = strtoul(irq_string, &end, 10);
				do {
					add_cl_banned_irq(irq);
				} while((irq = strtoul(end, &end, 10)));
			} else if (!(strncmp(buff + strlen("settings "), "cpus ",
							strlen("cpus")))) {
				char *cpu_ban_string = malloc(
						sizeof(char) * (recv_size - strlen("settings cpus ")));
				strncpy(cpu_ban_string, buff + strlen("settings cpus "),
						recv_size - strlen("settings cpus "));
				banned_cpumask_from_ui = strtok(cpu_ban_string, " ");
				if (!strncmp(banned_cpumask_from_ui, "NULL", strlen("NULL"))) {
					banned_cpumask_from_ui = NULL;
				}
				need_rescan = 1;
			}
		}
		if (!strncmp(buff, "setup", strlen("setup"))) {
			char setup[2048] = "\0";
			snprintf(setup, 2048, "SLEEP %d ", sleep_interval);
			if(g_list_length(cl_banned_irqs) > 0) {
				for_each_irq(cl_banned_irqs, get_irq_data, setup);
			}
			char banned[512];
			cpumask_scnprintf(banned, 512, banned_cpus);
			snprintf(setup + strlen(setup), 2048 - strlen(setup),
					"BANNED %s", banned);
			send(sock, setup, strlen(setup), 0);
		}

		close(sock);
	}
	return TRUE;
}

int init_socket(char *socket_name)
{
	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(struct sockaddr_un));

	socket_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
	if (socket_fd < 0) {
		log(TO_ALL, LOG_WARNING, "Socket couldn't be created.\n");
		return 1;
	}

	addr.sun_family = AF_UNIX;
	addr.sun_path[0] = '\0';
	strncpy(addr.sun_path + 1, socket_name, strlen(socket_name));
	if (bind(socket_fd, (struct sockaddr *)&addr,
				sizeof(sa_family_t) + strlen(socket_name) + 1) < 0) {
		log(TO_ALL, LOG_WARNING, "Daemon couldn't be bound to the socket.\n");
		return 1;
	}
	int optval = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_PASSCRED, &optval, sizeof(optval)) < 0) {
		log(TO_ALL, LOG_WARNING, "Unable to set socket options.\n");
		return 1;
	}
	listen(socket_fd, 1);
	g_unix_fd_add(socket_fd, G_IO_IN, sock_handle, NULL);
	return 0;
}

int main(int argc, char** argv)
{
	sigset_t sigset, old_sigset;

	sigemptyset(&sigset);
	sigaddset(&sigset,SIGINT);
	sigaddset(&sigset,SIGHUP);
	sigaddset(&sigset,SIGTERM);
	sigaddset(&sigset,SIGUSR1);
	sigaddset(&sigset,SIGUSR2);
	sigprocmask(SIG_BLOCK, &sigset, &old_sigset);
#ifdef HAVE_GETOPT_LONG
	parse_command_line(argc, argv);
#else /* ! HAVE_GETOPT_LONG */
	if (argc>1 && strstr(argv[1],"--debug")) {
		debug_mode=1;
		foreground_mode=1;
	}
	if (argc>1 && strstr(argv[1],"--foreground"))
		foreground_mode=1;
	if (argc>1 && strstr(argv[1],"--oneshot"))
		one_shot_mode=1;
	if (argc>1 && strstr(argv[1],"--journal")) {
		journal_logging=1;
		foreground_mode=1;
	}
#endif /* HAVE_GETOPT_LONG */

	/*
 	 * Open the syslog connection
 	 */
	openlog(argv[0], 0, LOG_DAEMON);

	if (getenv("IRQBALANCE_ONESHOT")) 
		one_shot_mode=1;

	if (getenv("IRQBALANCE_DEBUG")) {
		debug_mode=1;
		foreground_mode=1;
	}

	/*
 	 * If we are't in debug mode, don't dump anything to the console
 	 * note that everything goes to the console before we check this
 	 */
	if (journal_logging)
		log_indent = "....";
	else
		log_indent = "    ";

	if (!debug_mode)
		log_mask &= ~TO_CONSOLE;

	if (numa_available() > -1) {
		numa_avail = 1;
	} else 
		log(TO_CONSOLE, LOG_INFO, "This machine seems not NUMA capable.\n");

	if (geteuid() != 0)
		log(TO_ALL, LOG_WARNING, "Irqbalance hasn't been executed under root privileges, thus it won't in fact balance interrupts.\n");

	if (banscript) {
		char *note = "Please note that --banscript is deprecated, please use --policyscript instead";
		log(TO_ALL, LOG_WARNING, "%s\n", note);
	}

	HZ = sysconf(_SC_CLK_TCK);
	if (HZ == -1) {
		log(TO_ALL, LOG_WARNING, "Unable to determin HZ defaulting to 100\n");
		HZ = 100;
	}

	build_object_tree();
	if (debug_mode)
		dump_object_tree();


	/* On single core UP systems irqbalance obviously has no work to do */
	if (core_count<2) {
		char *msg = "Balancing is ineffective on systems with a "
			    "single cpu.  Shutting down\n";

		log(TO_ALL, LOG_WARNING, "%s", msg);
		exit(EXIT_SUCCESS);
	}

	if (!foreground_mode) {
		int pidfd = -1;
		if (daemon(0,0))
			exit(EXIT_FAILURE);
		/* Write pidfile */
		if (pidfile && (pidfd = open(pidfile,
			O_WRONLY | O_CREAT | O_EXCL | O_TRUNC,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) >= 0) {
			char str[16];
			snprintf(str, sizeof(str), "%u\n", getpid());
			write(pidfd, str, strlen(str));
			close(pidfd);
		}
	}

	g_unix_signal_add(SIGINT, handler, NULL);
	g_unix_signal_add(SIGTERM, handler, NULL);
	g_unix_signal_add(SIGUSR1, handler, NULL);
	g_unix_signal_add(SIGUSR2, handler, NULL);
	g_unix_signal_add(SIGHUP, force_rescan, NULL);
	sigprocmask(SIG_SETMASK, &old_sigset, NULL);

#ifdef HAVE_LIBCAP_NG
	// Drop capabilities
	capng_clear(CAPNG_SELECT_BOTH);
	capng_lock();
	capng_apply(CAPNG_SELECT_BOTH);
#endif

	for_each_irq(NULL, force_rebalance_irq, NULL);

	parse_proc_interrupts();
	parse_proc_stat();

	char socket_name[64];
	snprintf(socket_name, 64, "%s%d.sock", SOCKET_PATH, getpid());

	if (init_socket(socket_name)) {
		return EXIT_FAILURE;
		} 
	main_loop = g_main_loop_new(NULL, FALSE);
	int *last_interval = &sleep_interval;
	g_timeout_add_seconds(sleep_interval, scan, last_interval);
	g_main_loop_run(main_loop);

	g_main_loop_quit(main_loop);

	free_object_tree();
	free_cl_opts();

	/* Remove pidfile */
	if (!foreground_mode && pidfile)
		unlink(pidfile);
	/* Remove socket */
	close(socket_fd);

	return EXIT_SUCCESS;
}
