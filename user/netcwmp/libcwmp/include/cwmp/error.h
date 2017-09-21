/************************************************************************
 *                                                                      *
 * Netcwmp Project                                                      *
 *                                                                      *
 * A software client for enabling TR-069 in embedded devices (CPE).     *
 *                                                                      *
 * Copyright (C) 2013-2014  netcwmp.netcwmp group                         *
 *                                                                      *
 * Copyright 2013-2014           Mr.x() <netcwmp@gmail.com>          *
 *                                                                      *
 ***********************************************************************/

#ifndef __CWMPERROR_H__
#define __CWMPERROR_H__


typedef enum { CWMP_OK=0, CWMP_ERROR=-1, CWMP_SOCKCLOSEERR=-2, CWMP_BADFUNCARG=-3, CWMP_ENOMEM=-4, CWMP_COULDNOT_CONNECT=-5, CWMP_TIMEOUT=-6 } cwmp_errcode_t;


#define CWMP_TRUE			1
#define CWMP_FALSE			0

#define CWMP_YES			1
#define CWMP_NO				0

#endif

