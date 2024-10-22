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

#ifndef __CWMP_PRIVATE_H__
#define __CWMP_PRIVATE_H__

#include <cwmp/cwmp.h>
#include <cwmp/buffer.h>
#include <cwmp/pool.h>
#include <cwmp/http.h>


struct  device_id_st
{
    char *manufactorer;
    char *name;
    char *oui;
    char *model_name;
    char *product_class;
    char *serial_number;
} ;


struct parameter_value_node_st
{
    const char *name;
    const char *type;
    char *  value;
    int     value_length;
};


struct parameter_st
{
    const char * name;
    const char * value;
    unsigned int value_length;

    int type;
    int fault_code;

};

struct parameter_list_st
{
    int count;
    int size;
    parameter_t ** parameters;
};



struct header_st
{
    char *  id;
    int     hold_requests;
    int     no_more_requests;
};


struct key_value_t
{
    char *name;
    char *value;
};



struct http_parser_t
{
    http_header_t *     header[MAX_HEADERS];
    int                 count;

};


struct http_socket_t
{
    int sockdes;
    int type;
    int protocol;
    http_sockaddr_t * addr;
    ssize_t send_timeout;
    ssize_t recv_timeout;

    int option;
    http_write_callback_pt write_callback;
    void *  write_calldata;

    int error_code;

    int use_ssl;

#ifdef USE_CWMP_OPENSSL
    SSL * ssl;
#endif


    pool_t * pool;

    struct http_statistics stat;

};



#endif

