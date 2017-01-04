
#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <curses.h>
#include <ncurses.h>
#include "irqbalance-ui.h"
#include "ui.h"
#include "helpers.h"


int irqbalance_pid = -1;
GList *tree = NULL;
setup_t setup;
GMainLoop *main_loop;
int is_tree = 1;

struct msghdr * create_credentials_msg()
{
	struct ucred *credentials = malloc(sizeof(struct ucred));
	credentials->pid = getpid();
	credentials->uid = geteuid();
	credentials->gid = getegid();

	struct msghdr *msg = malloc(sizeof(struct msghdr));
	memset(msg, 0, sizeof(struct msghdr));
	msg->msg_iovlen = 1;
	msg->msg_control = malloc(CMSG_SPACE(sizeof(struct ucred)));
	msg->msg_controllen = CMSG_SPACE(sizeof(struct ucred));

	struct cmsghdr *cmsg = CMSG_FIRSTHDR(msg);
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_CREDENTIALS;
	cmsg->cmsg_len = CMSG_LEN(sizeof(struct ucred));
	memcpy(CMSG_DATA(cmsg), credentials, sizeof(struct ucred));

	return msg;
}

int init_connection()
{
	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(struct sockaddr_un));

	int socket_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
	if(socket_fd < 0) {
		perror("Error opening socket");
		return 0;
	}
	addr.sun_family = AF_UNIX;
	char socket_name[64];
	snprintf(socket_name, 64, "%s%d.sock", SOCKET_PATH, irqbalance_pid);
	strncpy(addr.sun_path + 1, socket_name, strlen(socket_name));

	if(connect(socket_fd, (struct sockaddr *)&addr,
				sizeof(sa_family_t) + strlen(socket_name) + 1) < 0) {
		return 0;
	}

	return socket_fd;
}

void send_settings(char *data)
{
	/* Send "settings sleep X" to set sleep interval, "settings ban
	 * irqs X Y..." to ban IRQs from balancing,
	 * "settings cpus <banned_list>" to setup which CPUs are forbidden
	 * to handle IRQs
	 */
	int socket_fd = init_connection();
	if(!socket_fd) {
		return;
	}

	struct msghdr *msg = create_credentials_msg();
	struct iovec iov;
	iov.iov_base = (void *) data;
	iov.iov_len = strlen(data);
	msg->msg_iov = &iov;
	sendmsg(socket_fd, msg, 0);

	close(socket_fd);
}

char * get_data(char *string)
{
	/* Send "setup" to get sleep interval, banned IRQs and banned CPUs,
	 * "stats" to get CPU tree statistics
	 */
	int socket_fd = init_connection();
	if(!socket_fd) {
		return NULL;
	}

	struct msghdr *msg = create_credentials_msg();
	struct iovec iov;
	iov.iov_base = (void *) string;
	iov.iov_len = strlen(string);
	msg->msg_iov = &iov;
	sendmsg(socket_fd, msg, 0);

	char *data = malloc(2048 * sizeof(char));
	int len = recv(socket_fd, data, 2048, 0);
	close(socket_fd);
	data[len] = '\0';
	return data;
}

void parse_setup(char *setup_data)
{
	char *token, *ptr;
	if((setup_data == NULL) || (strlen(setup_data) == 0)) return;
	char copy[strlen(setup_data) + 1];
	strncpy(copy, setup_data, strlen(setup_data) + 1);
	setup.banned_irqs = NULL;
	setup.banned_cpus = NULL;
	token = strtok_r(copy, " ", &ptr);
	if(strncmp(token, "SLEEP", strlen("SLEEP"))) goto out;
	setup.sleep = strtol(strtok_r(ptr, " ", &ptr), NULL, 10);
	token = strtok_r(ptr, " ", &ptr);
	/* Parse banned IRQ data */
	while(!strncmp(token, "IRQ", strlen("IRQ"))) {
		irq_t *new_irq = malloc(sizeof(irq_t));
		new_irq->vector = strtol(strtok_r(ptr, " ", &ptr), NULL, 10);
		token = strtok_r(ptr, " ", &ptr);
		if(strncmp(token, "LOAD", strlen("LOAD"))) goto out;
		new_irq->load = strtol(strtok_r(ptr, " ", &ptr), NULL, 10);
		token = strtok_r(ptr, " ", &ptr);
		if(strncmp(token, "DIFF", strlen("DIFF"))) goto out;
		new_irq->diff = strtol(strtok_r(ptr, " ", &ptr), NULL, 10);
		token = strtok_r(ptr, " ", &ptr);
		if(strncmp(token, "CLASS", strlen("CLASS"))) goto out;
		new_irq->class = strtol(strtok_r(ptr, " ", &ptr), NULL, 10);
		new_irq->is_banned = 1;
		new_irq->assigned_to = NULL;
		setup.banned_irqs = g_list_append(setup.banned_irqs, new_irq);
		token = strtok_r(ptr, " ", &ptr);
	}

	if(strncmp(token, "BANNED", strlen("BANNED"))) goto out;
	token = strtok_r(ptr, " ", &ptr);
	for(int i = strlen(token) - 1; i >= 0; i--) {
		char *map = hex_to_bitmap(token[i]);
		for(int j = 3; j >= 0; j--) {
			if(map[j] == '1') {
				uint64_t *banned_cpu = malloc(sizeof(uint64_t));
				*banned_cpu = (4 * (strlen(token) - (i + 1)) + (4 - (j + 1)));
				setup.banned_cpus = g_list_append(setup.banned_cpus,
								banned_cpu);
			}
		}
	}
	return;

out: {
	/* Invalid data presented */
	char invalid_data[128];
	snprintf(invalid_data, 128, "Invalid data sent. Unexpected token: %s\n",
				token);
	printf("%s\n", invalid_data);
	g_list_free(tree);
	exit(1);
}
}

GList * concat_child_lists(cpu_node_t *node)
{
	GList *new = NULL;
	GList *child_entry = g_list_first(node->children);
	do {
		cpu_node_t *child = (cpu_node_t *)child_entry->data;
		GList *cpu_entry = g_list_first(child->cpu_list);
		do {
			uint64_t *cpu = (uint64_t *)cpu_entry->data;
			new = g_list_append(new, cpu);
			cpu_entry = g_list_next(cpu_entry);
		} while(cpu_entry != NULL);
		child_entry = g_list_next(child_entry);
	} while(child_entry != NULL);

	return new;
}

void copy_cpu_list_to_irq(irq_t *irq, void *data)
{
	irq->assigned_to = g_list_copy((GList *)data);
	irq->assigned_to = g_list_sort(irq->assigned_to, sort_ints);
}

void assign_cpu_lists(cpu_node_t *node, void *data __attribute__((unused)))
{
	if(g_list_length(node->children) > 0) {
		for_each_node(node->children, assign_cpu_lists, NULL);
		node->cpu_list = concat_child_lists(node);
	} else {
		node->cpu_list = g_list_append(node->cpu_list, &(node->number));
	}

	for_each_irq(node->irqs, copy_cpu_list_to_irq, node->cpu_list);
}

void assign_cpu_mask(cpu_node_t *node, void *data __attribute__((unused)))
{
	char *mask = malloc(16 * sizeof(char));
	mask[0] = '\0';
	unsigned int sum = 0;
	GList *list_entry = g_list_first(node->cpu_list);
	do {
		int *cpu = list_entry->data;
		sum += 1 << (*cpu);
		list_entry = g_list_next(list_entry);
	} while(list_entry != NULL);
	snprintf(mask, 15, "0x%x", sum);
	node->cpu_mask = mask;

	if(g_list_length(node->children) > 0) {
		for_each_node(node->children, assign_cpu_mask, NULL);
	}
}

void parse_into_tree(char *data)
{
	if((data == NULL) || (strlen(data) == 0)) return;
	char *token, *ptr;
	cpu_node_t *parent = NULL;
	char copy[strlen(data) + 1];
	strncpy(copy, data, strlen(data) + 1);
	tree = NULL;

	token = strtok_r(copy, " ", &ptr);
	while(token != NULL) {
		/* Parse node data */
		if(strncmp(token, "TYPE", strlen("TYPE"))) goto out;
		cpu_node_t *new = malloc(sizeof(cpu_node_t));
		new->irqs = NULL;
		new->children = NULL;
		new->cpu_list = NULL;
		new->cpu_mask = NULL;
		new->type = strtol(strtok_r(ptr, " ", &ptr), NULL, 10);
		if(new->type == OBJ_TYPE_NODE) {
			parent = NULL;
		} else if(new->type >= parent->type) {
			parent = parent->parent;
		}
		token = strtok_r(ptr, " ", &ptr);
		if(strncmp(token, "NUMBER", strlen("NUMBER"))) goto out;
		new->number = strtol(strtok_r(ptr, " ", &ptr), NULL, 10);
		token = strtok_r(ptr, " ", &ptr);
		if(strncmp(token, "LOAD", strlen("LOAD"))) goto out;
		new->load = strtol(strtok_r(ptr, " ", &ptr), NULL, 10);
		token = strtok_r(ptr, " ", &ptr);
		if(strncmp(token, "SAVE_MODE", strlen("SAVE_MODE"))) goto out;
		new->is_powersave = strtol(strtok_r(ptr, " ", &ptr), NULL, 10);
		token = strtok_r(ptr, " ", &ptr);

		/* Parse assigned IRQ data */
		while((token != NULL) && (!strncmp(token, "IRQ", strlen("IRQ")))) {
			irq_t *new_irq = malloc(sizeof(irq_t));
			new_irq->vector = strtol(strtok_r(ptr, " ", &ptr), NULL, 10);
			token = strtok_r(ptr, " ", &ptr);
			if(strncmp(token, "LOAD", strlen("LOAD"))) goto out;
			new_irq->load = strtol(strtok_r(ptr, " ", &ptr), NULL, 10);
			token = strtok_r(ptr, " ", &ptr);
			if(strncmp(token, "DIFF", strlen("DIFF"))) goto out;
			new_irq->diff = strtol(strtok_r(ptr, " ", &ptr), NULL, 10);
			token = strtok_r(ptr, " ", &ptr);
			if(strncmp(token, "CLASS", strlen("CLASS"))) goto out;
			new_irq->class = strtol(strtok_r(ptr, " ", &ptr), NULL, 10);
			new_irq->is_banned = 0;
			new->irqs = g_list_append(new->irqs, new_irq);
			token = strtok_r(ptr, " ", &ptr);
		}

		if((token == NULL) || (strncmp(token, "IRQ", strlen("IRQ")))) {
			new->parent = parent;
			if(parent == NULL) {
				tree = g_list_append(tree, new);
			} else {
				parent->children = g_list_append(parent->children, new);
			}
			if(new->type != OBJ_TYPE_CPU) {
				parent = new;
			}
		}
	}

	for_each_node(tree, assign_cpu_lists, NULL);
	for_each_node(tree, assign_cpu_mask, NULL);
	return;

out: {
	/* Invalid data presented */
	char invalid_data[128];
	snprintf(invalid_data, 128, "Invalid data sent. Unexpected token: %s\n",
				token);
	printf("%s\n", invalid_data);
	g_list_free(tree);
	exit(1);
}
}

gboolean rescan_tree(gpointer data __attribute__((unused)))
{
	char *setup_data = get_data(SETUP);
	parse_setup(setup_data);
	char *irqbalance_data = get_data(STATS);
	parse_into_tree(irqbalance_data);
	if(is_tree) {
		display_tree();
	}

	return TRUE;
}

gboolean key_loop(gpointer data __attribute__((unused)))
{
	int c = getch();
	switch(c) {
	case 'q':
		close_window(0);
		break;
	case KEY_F(3):
		is_tree = 1;
		display_tree();
		break;
	case KEY_F(4):
		is_tree = 0;
		settings();
		break;
	case KEY_F(5):
		is_tree = 0;
		setup_irqs();
		break;
	default:
		break;
	}
	return TRUE;
}

int main(int argc, char **argv)
{
	if(getuid() != 0) {
		printf("This program needs to be executed with root priviledges\n");
		return EACCES;
	}
	if(argc > 1) {
		/* PID of irqbalance specified */
		irqbalance_pid = strtol(argv[1], NULL, 10);
		if(!irqbalance_pid) {
			printf("PID must be a number\n");
			return EINVAL;
		}
	} else {
		/* We need to find irqbalance's PID */
		DIR *dir = opendir("/proc");
		if(dir) {
			struct dirent *entry;
			char cmdfile[64];
			char cmdstring[256];
			cmdstring[255] = '\0';
			do {
				entry = readdir(dir);
				if(entry) {
					snprintf(cmdfile, 64, "/proc/%s/cmdline", entry->d_name);
					FILE *f = fopen(cmdfile, "r");
					if(f == NULL) {
						continue;
					}
					fgets(cmdstring, 255, f);
					if((strstr(cmdstring, "irqbalance") != NULL) &&
							(strstr(cmdstring, "irqbalance-ui") == NULL)) {
						irqbalance_pid = strtol(entry->d_name, NULL, 10);
					}
					fclose(f);
				}
			} while((entry) && (irqbalance_pid == -1));
		}
		if(irqbalance_pid == -1) {
			printf("Unable to determine irqbalance PID\n");
			return EINVAL;
		}
	}

	init();

	main_loop = g_main_loop_new(NULL, FALSE);
	g_timeout_add_seconds(5, rescan_tree, NULL);
	g_timeout_add_seconds(1, key_loop, NULL);
	g_main_loop_run(main_loop);


	g_main_loop_quit(main_loop);
	close_window(0);
}
