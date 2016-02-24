#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "procps.h"
#include "uemf.h"

// Read unsigned number
// Returns -1 on error
static long readUnsigned(const char *str)
{
	long result = 0;
	while ((*str)!='\0')
	{
		char ch = *(str++);
		if ((ch<'0') || (ch>'9'))
			return -1;
		result = (result*10) + (ch-'0');
	}
	return result;
}

// Initialize command-line buffer
void procps_init(cmdline_t *src)
{
	src->pid        = -1;
	src->cap        = CMDLINE_EXT;
	src->size       = 0;
	src->buffer     = (char *)balloc(B_L, CMDLINE_EXT);
	src->acap       = CMDLINE_AEXT;
	src->argc       = 0;
	src->argv       = (char **)balloc(B_L, CMDLINE_AEXT * sizeof(char *));
	src->dynamic    = 0;
	src->next       = NULL;
}

// Finalize command-line buffer
void procps_free(cmdline_t *src)
{
	while (src != NULL)
	{
		cmdline_t *next = src->next;

		if (src->argv!=NULL)
			bfreeSafe(B_L, src->argv);
		if (src->buffer!=NULL)
			bfreeSafe(B_L, src->buffer);
		if (src->dynamic)
			bfreeSafe(B_L, src);
		src = next;
	}
}

// Read process arguments
int procps_read_args(pid_t procnum, cmdline_t *pcmdline)
{
	char cmdline[64];
	size_t i;
	sprintf(cmdline, "/proc/%ld/cmdline", (long int)procnum);

	FILE *fd = fopen(cmdline, "r");
	if (fd!=NULL)
	{
		// First read fully file
		size_t buf_pos = 0;

		while (!feof(fd))
		{
			size_t can_read = pcmdline->cap - buf_pos;
			if (can_read==0) // Need to exend buffer ?
			{
				size_t newcap   = pcmdline->cap + CMDLINE_EXT;

				char *newbuf    = (char *)brealloc(B_L, pcmdline->buffer, newcap);
				if (newbuf == NULL) // cmdline_free has to free resouce
				{
					fclose(fd); // close file
					return -errno;
				}

				// Remember new buffer state
				pcmdline->buffer= newbuf;
				pcmdline->cap   = newcap;
				can_read        = CMDLINE_EXT;
			}

			// Now read data to buffer
			size_t read = fread(&pcmdline->buffer[buf_pos], 1, can_read, fd);
			if (read>0)
				buf_pos        += read;
			if (read<can_read) // File was read fully ?
				break;
		}

		pcmdline->size = buf_pos; // Store last success buffer position in cmd line state

		// Now we know size of read buffer, have to calculate number of arguments
		size_t argc = 0;
		for (i=0; i<buf_pos; i++)
			if (pcmdline->buffer[i] == '\0')
				argc++;

		// Number of arguments was calculated, check if need to resize
		if (argc >= pcmdline->acap)
		{
			size_t newcap = argc + CMDLINE_AEXT - (argc % CMDLINE_AEXT); // Calculate new capacity

			char **new_argv = (char **)brealloc(B_L, pcmdline->argv, (newcap*sizeof(char *))); // resize
			if (new_argv == NULL) // cmdline_free has to free resouce
			{
				fclose(fd);
				return -errno;
			}

			pcmdline->argv  = new_argv;
			pcmdline->acap  = newcap;
		}

		// Now split arguments into separate pointers
		char *cptr = pcmdline->buffer;
		for (i=0; i<argc; )
		{
			if (cptr[0] != '\0')
			{
				pcmdline->argv[i] = cptr;
				cptr = cptr + strlen(cptr)+1; // Move to next string argument
				i++;
			}
			else
				argc--;
		}

		// Store new number of arguments
		pcmdline->argc = argc;
		pcmdline->pid  = procnum;

		fclose(fd);
		return 0;
	}

	return -errno;
}

// Find process containing string
int procps_find(const char *procname, size_t elems, pid_t *pids)
{
	struct dirent *dent;
	cmdline_t cmd_line;

	// Try to open /proc
	unsigned int result = 0;
	DIR *proc = opendir("/proc");

	if (proc!=NULL)
	{
		// Initialize process command line buffer
		procps_init(&cmd_line);

		while ((dent=readdir(proc))!=NULL)
		{
			long procnum = readUnsigned(dent->d_name);
			if (procnum>=0) // Valid unsigned ?
			{
				int code = procps_read_args(procnum, &cmd_line);
				if (code<0)
				{
					closedir(proc);
					return code;
				}

				size_t plen = strlen(procname);

				// Now search all arguments
				for (code=0; code<cmd_line.argc; code++)
				{
					char *subs = strstr(cmd_line.argv[code], procname);
					if (subs != NULL)
					{
						// Check that it is a beginning of word
						if ((subs == cmd_line.argv[code]) ||
							(subs[-1]==' ') || (subs[-1]=='/')) 
						{
							// Check that it is an end of word
							if ((subs[plen] == '\0') || (subs[plen]==' ') || (subs[plen]=='/'))
							{
								if (result<elems)
									pids[result] = procnum;
								result++;
								break;
							}
						}
					}
				}
			}
		}

		procps_free(&cmd_line);

		closedir(proc);
	}
	return result;
}

// Find process containing string
cmdline_t *procps_list()
{
	struct dirent *dent;
	cmdline_t *result = NULL;

	// Try to open /proc
	DIR *proc = opendir("/proc");

	if (proc != NULL)
	{
		cmdline_t *cmdline = NULL;

		// Initialize process command line buffer
		while ((dent = readdir(proc)) != NULL)
		{
			// Allocate new entry if needed
			if (cmdline == NULL)
			{
				cmdline = (cmdline_t *)balloc(B_L, sizeof(cmdline_t));
				if (cmdline == NULL) // Allocation failed?
				{
					// Free previously allocated data & return
					procps_free(result);
					closedir(proc);
					return NULL;
				}
				else
				{
					procps_init(cmdline);
					cmdline->dynamic = 1;
				}
			}

			// Read process number
			long procnum = readUnsigned(dent->d_name);
			if (procnum >= 0) // Valid unsigned ?
			{
				int code = procps_read_args(procnum, cmdline);
				if (code == 0) // All seems to be OK?
				{
					// Add to list
					cmdline->next   = result;
					result          = cmdline;
					cmdline         = NULL;
				}
			}
		}

		// Free allocated cmdline if exists
		if (cmdline != NULL)
			procps_free(cmdline);

		closedir(proc);
	}

	return result;
}

// Calculate process
int procps_count(const char *procname)
{
	return procps_find(procname, 0, NULL);
}
