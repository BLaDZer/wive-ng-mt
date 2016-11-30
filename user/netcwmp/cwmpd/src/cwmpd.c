/* vim: set et: */
/************************************************************************
 *                                                                      *
 * Netcwmp/Opencwmp Project                                             *
 * A software client for enabling TR-069 in embedded devices (CPE).     *
 *                                                                      *
 * Copyright (C) 2013-2014  netcwmp.netcwmp group                            *
 *                                                                      *
 * This program is free software; you can redistribute it and/or        *
 * modify it under the terms of the GNU General Public License          *
 * as published by the Free Software Foundation; either version 2       *
 * of the License, or (at your option) any later version.               *
 *                                                                      *
 * This program is distributed in the hope that it will be useful,      *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 * GNU General Public License for more details.                         *
 *                                                                      *
 * You should have received a copy of the GNU Lesser General Public     *
 * License along with this library; if not, write to the                *
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,          *
 * Boston, MA  02111-1307 USA                                           *
 *                                                                      *
 * Copyright 2013-2014  Mr.x(Mr.x) <netcwmp@gmail.com>          *
 *                                                                      *
 ***********************************************************************/

#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include "cwmpd.h"
#include "modules/data_model.h"


#define CWMP_VALUE_UNSET -1

int              cwmp_argc;
char           **cwmp_argv;

static pool_t * cwmp_global_pool;

void cwmp_daemon()
{
    //daemon(0, 1);
}

void cwmp_getopt(int argc, char **argv)
{
}
/*
static int cwmp_save_argv( int argc, char *const *argv)
{
    cwmp_argv = (char **) argv;
    cwmp_argc = argc;

    return 0;
}*/

int cwmp_set_var(cwmp_t * cwmp)
{
    cwmp_log_trace("%s(cwmp=%p)", __func__, (void*)cwmp);

    cwmp_bzero(cwmp, sizeof(cwmp_t));
    pool_t * pool = pool_create(POOL_DEFAULT_SIZE);
    cwmp->pool = pool;


    cwmp_event_init(cwmp);
    cwmp_event_time_init(cwmp, NULL);

    cwmp->queue = queue_create(pool);

    return CWMP_OK;
}

#ifdef USE_CWMP_OPENSSL
void cwmp_init_ssl(cwmp_t * cwmp)
{
    char * cafile = cwmp_conf_pool_get(cwmp_global_pool, "cwmp:ca_file");
    char * capasswd = cwmp_conf_pool_get(cwmp_global_pool, "cwmp:ca_password");
    cwmp->ssl_ctx = openssl_initialize_ctx(cafile, capasswd);
}
#endif

static void signal_ignore_handler(int sig)
{
}

int main(int argc, char **argv)
{
    int level = CWMP_LOG_INFO;
    cwmp_t * cwmp;
    time_t seed = time(NULL);
    srand((unsigned int)seed);

    cwmp_log_init(NULL, level);
    int cwmp_enable = 0;

#ifdef WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    cwmp_global_pool = pool_create(POOL_DEFAULT_SIZE);
    cwmp = pool_pcalloc(cwmp_global_pool, sizeof(cwmp_t));

    cwmp_conf_open("/etc/cwmp.conf");

    char* loglevel = cwmp_conf_pool_get(cwmp_global_pool,"cwmpd:log_level");
    char* log_filename = cwmp_conf_pool_get(cwmp_global_pool, "cwmpd:log_filename");

    if (loglevel != NULL)
    {
        if (strcasecmp(loglevel,"crit") == 0) level = CWMP_LOG_CRIT;
        else if (strcasecmp(loglevel,"error") == 0) level = CWMP_LOG_ERROR;
        else if (strcasecmp(loglevel,"warn") == 0) level = CWMP_LOG_WARN;
        else if (strcasecmp(loglevel,"info") == 0) level = CWMP_LOG_INFO;
        else if (strcasecmp(loglevel,"debug") == 0) level = CWMP_LOG_DEBUG;
        else if (strcasecmp(loglevel,"trace") == 0) level = CWMP_LOG_TRACE;
    }

    cwmp_log_set(log_filename, level);
    cwmp_log_debug("DEBUG: current log level is \"%s\" (%i)", loglevel, level);

    cwmp_enable=cwmp_conf_get_int_def("cwmp:enable", 0);
    if(!cwmp_enable)
    {
        exit(-1);
    }

    cwmp_getopt(argc, argv);
    cwmp_set_var(cwmp);
    cwmp_daemon();
    cwmp_conf_init(cwmp);

    cwmp_conf_set("env:DEVNAME", DEVNAME);
    cwmp_conf_set("env:VERSIONPKG", VERSIONPKG);

    /* setup signals */
    signal(SIGPIPE, signal_ignore_handler);
#ifdef USE_CWMP_OPENSSL
    cwmp_init_ssl(cwmp);
#endif

    char model_filename[255] = "/etc/device.xml";
    char* acs_type = cwmp_conf_pool_get(cwmp_global_pool,"cwmp:acs_type");
    if (acs_type != NULL)
    {
        sprintf(model_filename, "/etc/device_%s.xml",acs_type);
    }

    cwmp_model_load(cwmp, model_filename);
    cwmp_process_start_master(cwmp);

    return 0;
}
