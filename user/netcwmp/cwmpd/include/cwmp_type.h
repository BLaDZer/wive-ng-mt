/************************************************************************
 *                                                                      *
 * Netcwmp/Opencwmp Project                                             *
 * A software client for enabling TR-069 in embedded devices (CPE).     *
 *                                                                      *
 * Copyright (C) 2013-2014  netcwmp.netcwmp group                         *
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
 * Copyright 2013-2014           Mr.x() <netcwmp@gmail.com>          *
 *                                                                      *
 ***********************************************************************/

#ifndef __CWMP_TYPE_H__
#define __CWMP_TYPE_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

#ifdef WIN32

# include <winsock2.h>
# include <windows.h>
# include <process.h>

#else

#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <bits/pthreadtypes.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <netdb.h>
#include <time.h>
#include <getopt.h>
#include <syslog.h>
#include <signal.h>
#include <ctype.h>
//#include <mcheck.h> //mtrace including header file

#endif

#include <cwmp/types.h>

/*!
* @file Common Platform Runtime Library
* @author
* @brief
*/

#ifdef WIN32
typedef  int cwmp_sig_atomic_t ;
#else
typedef sig_atomic_t cwmp_sig_atomic_t ;
#endif

typedef unsigned int                    cwmp_tls_key_t;

typedef struct cwmp_process_t           cwmp_process_t;

typedef struct cwmp_signal_t            cwmp_signal_t;
typedef struct cwmp_channel_t           cwmp_channel_t;

#ifdef WIN32
#define sleep(x)   Sleep(1000 * (x))
#include <signal.h>
#endif

#define cwmp_bzero(b, len)              (void)memset(b, 0, len)
#ifdef WIN32
#define cwmp_socket_nonblocking(s)      0
#else
#define cwmp_socket_nonblocking(s)      fcntl(s, F_SETFL, fcntl(s, F_GETFL) | O_NONBLOCK)
#endif
#define cwmp_value_helper(n)   #n
#define cwmp_value(n)                   cwmp_value_helper(n)


#endif
