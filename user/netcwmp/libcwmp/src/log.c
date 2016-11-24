/* vim: set et : */
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
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <inttypes.h>
#include <time.h>
#include <stdio.h>
#include "cwmp/log.h"

struct cwmp_log_t
{
    FILE * file;
    int level;
    char * name;
};

static cwmp_log_t 	g_cwmp_log_file;
static cwmp_log_t*	g_ot_log_file_ptr = NULL;

const char *
cwmp_loglevel_to_string(int level)
{
    switch(level) {
        case CWMP_LOG_CRIT:
            return "CRITICAL";
        case CWMP_LOG_ERROR:
            return "ERROR";
        case CWMP_LOG_WARN:
            return "WARNING";
        case CWMP_LOG_NOTICE:
            return "NOTICE";
        case CWMP_LOG_INFO:
            return "INFO";
        case CWMP_LOG_TRACE:
            return "TRACE";
        case CWMP_LOG_DEBUG:
            return "DEBUG";
        case CWMP_LOG_ALERT:
            return "ALERT";
        default:
            return "?";
    }
}

int cwmp_loglevel_to_syslog_level(int level) {
    switch (level) {
    case CWMP_LOG_CRIT: return LOG_CRIT;
    case CWMP_LOG_ERROR: return LOG_ERR;
    case CWMP_LOG_WARN:  return LOG_WARNING;
    case CWMP_LOG_NOTICE:return LOG_NOTICE;
    case CWMP_LOG_INFO:  return LOG_INFO;
    case CWMP_LOG_TRACE: return LOG_DEBUG;
    case CWMP_LOG_DEBUG: return LOG_DEBUG;
    case CWMP_LOG_ALERT: return LOG_ALERT;
    default: return LOG_DEBUG;
    }
}

void cwmp_log_set(const char * filename, int level)
{
    if (!g_ot_log_file_ptr) {
        return;
    }

    if (filename)
    {
        if (g_cwmp_log_file.name) {
            free(g_cwmp_log_file.name);
        }

        if (g_cwmp_log_file.file &&
                g_cwmp_log_file.file != stdout &&
                g_cwmp_log_file.file != stderr) {
            fclose(g_cwmp_log_file.file);
            g_cwmp_log_file.file = NULL;
        }

        if (!strcmp(filename, "stdout")) {
            g_cwmp_log_file.file = stdout;
        } else if (!strcmp(filename, "stderr")) {
            g_cwmp_log_file.file = stderr;
        } else if (!*filename) {
            /* null log */
        } else {
            g_cwmp_log_file.file = fopen(filename,"a+");
        }

        g_cwmp_log_file.name = strdup(filename);
        cwmp_log_info("Log set filename=%s", filename);
    }

    if (level) {
        cwmp_log_info("Log set level=%s", cwmp_loglevel_to_string(level));
        g_cwmp_log_file.level = level;
    }
}

int cwmp_log_init(const char * filename, int level)
{
    setlogmask (LOG_UPTO (cwmp_loglevel_to_syslog_level(level)));

    openlog ("cwmpd", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_DAEMON);

    g_ot_log_file_ptr = &g_cwmp_log_file;

    g_cwmp_log_file.file = NULL;
    g_cwmp_log_file.name = NULL;

    if (!filename) {
        filename = "stdout";
    }

    cwmp_log_set(filename, level);

    cwmp_log_info("Log started: level=%s, filename=%s",
            cwmp_loglevel_to_string(g_cwmp_log_file.level),
            g_cwmp_log_file.name);

    return 0;
}

void cwmp_log_fini()
{
    if (g_cwmp_log_file.name) {
        free(g_cwmp_log_file.name);
    }

    if ((g_cwmp_log_file.file != NULL) &&
           (g_cwmp_log_file.file != stdout) &&
           (g_cwmp_log_file.file != stderr))
    {
        fclose(g_cwmp_log_file.file);
    }

    memset(&g_cwmp_log_file, 0u, sizeof(g_cwmp_log_file));
}

void cwmp_log_write(int level, cwmp_log_t * log, const char * fmt, va_list ap)
{
    time_t t;
    struct tm *tm;
    char tm_str[24] = {};
    pid_t pid = syscall(SYS_gettid);
    if (g_ot_log_file_ptr == NULL) return; /* Uninitialized logger! */
    vsyslog(cwmp_loglevel_to_syslog_level(level), fmt, ap);

    cwmp_log_t * logger = log;
    if (logger == NULL)
    {
        logger = g_ot_log_file_ptr;
    }

    if (!logger->file)
        return;

    if (logger->level >= level)
    {
        t = time(NULL);
        tm = gmtime(&t);

        /* syslog-style time */
        strftime(tm_str, sizeof(tm_str), "%b %e %T", tm);
        fprintf(logger->file, "%s - -[%"PRIuPTR"]: %s: ",
                tm_str, (size_t)pid, cwmp_loglevel_to_string(level));
        vfprintf(logger->file, fmt, ap);
        fprintf(logger->file, "\n");

        fflush(logger->file);
    }
}

void cwmp_log_trace(const char * fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    cwmp_log_write(CWMP_LOG_TRACE, NULL, fmt, ap);
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

