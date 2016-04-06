/************************************************************************
 * Id: log.c                                                            *
 *                                                                      *
 * TR069 Project:  A TR069 library in C                                 *
 * Copyright (C) 2013-2014  netcwmp.netcwmp group                                *
 *                                                                      *
 *                                                                      *
 * Email: netcwmp ( & ) gmail dot com                                *
 *                                                                      *
 ***********************************************************************/
 
 
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <syslog.h>
#include "cwmp/log.h"

struct cwmp_log_t
{
    FILE * file;
    int level;
    char * name;
};

static cwmp_log_t 	g_cwmp_log_file;
static cwmp_log_t*	g_ot_log_file_ptr = NULL;



int cwmp_loglevel_to_syslog_level(int level) {
    switch (level)
    {
	case CWMP_LOG_CRIT: return LOG_CRIT;
	case CWMP_LOG_ERROR: return LOG_ERR;
	case CWMP_LOG_WARN:  return LOG_WARNING;
	case CWMP_LOG_NOTICE:return LOG_NOTICE;
	case CWMP_LOG_INFO:  return LOG_INFO;
	case CWMP_LOG_TRACE: return LOG_DEBUG;
	case CWMP_LOG_DEBUG: return LOG_DEBUG;
	default: return LOG_DEBUG;
    }
}

int cwmp_log_init(const char * filename, int level)
{
    setlogmask (LOG_UPTO (cwmp_loglevel_to_syslog_level(level)));

    openlog ("cwmpd", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_DAEMON);

    g_cwmp_log_file.file = NULL;
    g_cwmp_log_file.name = NULL;
    if (filename)
    {
        g_cwmp_log_file.file = fopen(filename,"a+");
        g_cwmp_log_file.name = strdup(filename);
    }

    if (g_cwmp_log_file.file == NULL)
    {
        g_cwmp_log_file.file = stdout;
    }

    g_cwmp_log_file.level = level;

    g_ot_log_file_ptr = &g_cwmp_log_file;

    return 0;
}

void cwmp_log_fini()
{
    free(g_cwmp_log_file.name);

    if ((g_cwmp_log_file.file != stdout) && (g_cwmp_log_file.file != NULL))
    {
        fclose(g_cwmp_log_file.file);
    }

}

void cwmp_log_write(int level, cwmp_log_t * log, const char * fmt, va_list ap)
{
    if (g_ot_log_file_ptr == NULL) return; /* Uninitialized logger! */
    vsyslog(cwmp_loglevel_to_syslog_level(level), fmt, ap);

    cwmp_log_t * logger = log;
    if (logger == NULL)
    {
        logger = g_ot_log_file_ptr;
    }

    if (logger->level >= level)
    {
        vfprintf(logger->file, fmt, ap);
        fprintf(logger->file, "\n");

        fflush(logger->file);
    }



}

void cwmp_log_tracer(int level, cwmp_log_t * log,const char * fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    cwmp_log_write(level, log, fmt, ap);
    va_end(ap);
}

void cwmp_log_debug(const char * fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    cwmp_log_write(CWMP_LOG_DEBUG, NULL, fmt, ap);
    va_end(ap);
}

void cwmp_log_info(const char * fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    cwmp_log_write(CWMP_LOG_INFO, NULL, fmt, ap);
    va_end(ap);
}

void cwmp_log_warn(const char * fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    cwmp_log_write(CWMP_LOG_WARN, NULL, fmt, ap);
    va_end(ap);
}




void cwmp_log_error(const char * fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    cwmp_log_write(CWMP_LOG_ERROR, NULL, fmt, ap);
    va_end(ap);
}

void cwmp_log_alert(const char * fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    cwmp_log_write(CWMP_LOG_ALERT, NULL, fmt, ap);
    va_end(ap);
}

void cwmp_log_critical(const char * fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    cwmp_log_write(CWMP_LOG_CRIT, NULL, fmt, ap);
    va_end(ap);
}

