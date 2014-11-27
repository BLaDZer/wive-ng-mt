#ifndef _PROCPS_H
#define _PROCPS_H

#include <sys/types.h>

#define CMDLINE_EXT             0x100
#define CMDLINE_AEXT            0x10

typedef struct cmdline_t
{
	pid_t   pid;    // process ID
	size_t  cap;    // buffer capacity in bytes
	size_t  size;   // buffer size in bytes
	char   *buffer; // buffer
	size_t  acap;   // argv capacity in elements

	int     argc;   // arguments count
	char  **argv;   // pointer to arguments
	
	int     dynamic; // dynamic flag
	struct cmdline_t *next; // pointer to next cmd line
} cmdline_t;

extern void              procps_init(cmdline_t *src);
extern void              procps_free(cmdline_t *src);
extern int               procps_read_args(pid_t procnum, cmdline_t *pcmdline);
extern int               procps_find(const char *procname, size_t elems, pid_t *pids);
extern int               procps_count(const char *procname);
extern cmdline_t        *procps_list();
#endif /* _PROCPS_H */
