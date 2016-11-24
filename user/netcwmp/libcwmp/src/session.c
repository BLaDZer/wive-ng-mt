/* vim: set et: */
/************************************************************************
 * Id: session.c                                                        *
 *                                                                      *
 * TR069 Project:  A TR069 library in C                                 *
 * Copyright (C) 2013-2014 netcwmp group                                *
 *                                                                      *
 *                                                                      *
 * Email: netcwmp ( & ) gmail dot com                                *
 *                                                                      *
 ***********************************************************************/


#include "cwmp/session.h"
#include "cwmp/cfg.h"
#include "cwmp/log.h"
#include "cwmp/cwmp.h"
#include "cwmp_private.h"



static cwmp_uint32_t g_cwmp_session_sequence = 0;
static char g_cwmp_session_sequence_buffer[64];

//static parameter_node_t * g_cwmp_session_root_parameters = NULL;



#define ParameterFormatEnd  ".%s"
#define ParameterFormat1    "%s"
#define ParameterFormat2 ParameterFormat1 ParameterFormatEnd
#define ParameterFormat3 ParameterFormat2 ParameterFormatEnd
#define ParameterFormat4 ParameterFormat3 ParameterFormatEnd
#define ParameterFormat5 ParameterFormat4 ParameterFormatEnd
#define ParameterFormat6 ParameterFormat5 ParameterFormatEnd
#define ParameterFormat7 ParameterFormat6 ParameterFormatEnd
#define ParameterFormat8 ParameterFormat7 ParameterFormatEnd
#define ParameterFormat9 ParameterFormat8 ParameterFormatEnd
#define ParameterFormat10 ParameterFormat9 ParameterFormatEnd
#define ParameterFormat11 ParameterFormat10 ParameterFormatEnd
#define ParameterFormat12 ParameterFormat11 ParameterFormatEnd
#define ParameterFormat13 ParameterFormat12 ParameterFormatEnd
#define ParameterFormat14 ParameterFormat13 ParameterFormatEnd
#define ParameterFormat15 ParameterFormat14 ParameterFormatEnd

#define CWMP_PARAMETER_FORMATS_MAX 15

static char * cwmp_parameter_formats_string[] =
{
    ParameterFormat1,
    ParameterFormat2,
    ParameterFormat3,
    ParameterFormat4,
    ParameterFormat5,
    ParameterFormat6,
    ParameterFormat7,
    ParameterFormat8,
    ParameterFormat9,
    ParameterFormat10,
    ParameterFormat11,
    ParameterFormat12,
    ParameterFormat13,
    ParameterFormat14,
    ParameterFormat15
};




static char * rpc_methods[] =
{
    "GetRPCMethods",
    "SetParameterValues",
    "GetParameterValues",
    "GetParameterNames",
    "SetParameterAttributes",
    "GetParameterAttributes",
//    "AddObject",
//    "DeleteObject",
    "Download",
    "Upload",
    "Reboot",
    "FactoryReset",
    "Inform"
};



char * cwmp_data_append_parameter_name(pool_t * pool, int count, ...)
{

    char buffer[1024] = {0};
    char * p = NULL;
    char * format;
    va_list ap;
    if (count >0 && count <= CWMP_PARAMETER_FORMATS_MAX)
    {
        format = cwmp_parameter_formats_string[count-1];

        va_start(ap, count);
        vsprintf(buffer, format, ap);
        va_end(ap);

        p = pool_pcalloc(pool, strlen(buffer)+1);
        strcpy(p, buffer);
    }
    return p;
}

int cwmp_data_sprintf_parameter_name(char * buffer, int count, ...)
{
    int rc = 0;
    char * format;
    va_list ap;
    if (count >0 && count <= CWMP_PARAMETER_FORMATS_MAX)
    {
        format = cwmp_parameter_formats_string[count-1];


        va_start(ap, count);
        rc = vsprintf(buffer, format, ap);
        va_end(ap);
        buffer[rc] = 0;

    }
    return rc;
}

#if 0

char * cwmp_data_append_parameter_name(pool_t * pool, const char * format, ...)
{

    char buffer[1024] = {0};
    char * p;
    va_list ap;
    va_start(ap, format);
    vsprintf(buffer, format, ap);
    va_end(ap);

    p = pool_pcalloc(pool, strlen(buffer)+1);
    strcpy(p, buffer);

    return p;
}

int cwmp_data_sprintf_parameter_name(char * buffer, const char * format, ...)
{
    int rc;
    va_list ap;
    va_start(ap, format);
    rc = vsprintf(buffer, format, ap);
    va_end(ap);
    buffer[rc] = 0;

    return rc;
}



#endif



char * cwmp_data_get_parameter_value(cwmp_t * cwmp, parameter_node_t * root, const char * name, pool_t * pool)
{
    parameter_node_t * node;
    char * value = NULL;
    int rc;

    /*
    cwmp_log_trace("%s(cwmp=%p, root=%p, name=\"%s\", pool=%p)",
            __func__, (void*)cwmp, (void*)root, name, (void*)pool)
    */

    node = cwmp_get_parameter_node(root, name);
    if (!node)
        return NULL;

    rc = cwmp_get_parameter_node_value(cwmp, node, name, &value, pool);
    if(rc == 0) {
        return value;
    } else {
        return node->value;
    }

}

int cwmp_data_set_parameter_value(cwmp_t * cwmp, parameter_node_t * root, const char * name, const char * value, int value_length, pool_t * pool)
{
    parameter_node_t * node;

    node = cwmp_get_parameter_node(root, name);
    if (!node)
        return CWMP_ERROR;
    return cwmp_set_parameter_node_value(cwmp, node, name, value, value_length);
}

char * cwmp_session_get_sequence(pool_t * pool)
 {
    g_cwmp_session_sequence++;
    TRsnprintf(g_cwmp_session_sequence_buffer, 63, "%u", g_cwmp_session_sequence);
    return g_cwmp_session_sequence_buffer;
}

int cwmp_session_get_localip(char *hostip)
{
#ifdef WIN32
    /*    struct sockaddr addr;
    	SOCKET fd;
    	char local_ip_addr[20] = {0};
    	int len = sizeof(addr);
        ZeroMemory( &addr, sizeof(addr) );


    	if(!hostip)
                return -1;

    	if((fd=socket(AF_INET,SOCK_DGRAM,0))>=0)
        {
    		if( getsockname( fd, &addr, &len ) )
    		{
    			len = WSAGetLastError();
    		}

    		TRsnprintf(local_ip_addr, 20, "%s", inet_ntoa( ((struct sockaddr_in*)&addr)->sin_addr ));
    		TRstrcpy(hostip, local_ip_addr);
    	}
    */

    char hostname[256];

    struct hostent* pHostent;

    struct sockaddr_in sa;

    struct hostent he;
    int i;

    int res = gethostname(hostname, sizeof(hostname));
    if (res != 0) {
        cwmp_log_error("%u", WSAGetLastError());
        return -1;
    }
    cwmp_log_debug("hostname=%s", hostname);
    ////////////////
    // ������������ȡ������Ϣ.
    //


    pHostent = gethostbyname(hostname);


    if (pHostent==NULL) {
        cwmp_log_error("%u", WSAGetLastError());
        return -1;
    }
    //////////////////
    // ���򷵻ص�hostent��Ϣ.
    //

    he = *pHostent;


    cwmp_log_debug("name=%s\naliases=%s\naddrtype=%d\nlength=%d",
                   he.h_name, he.h_aliases, he.h_addrtype, he.h_length);


    for (i=0; he.h_addr_list[i]; i++)
    {
        memcpy ( &sa.sin_addr.s_addr, he.h_addr_list[i],he.h_length);
        // ����������IP��ַ.
        cwmp_log_debug("Address: %s", inet_ntoa(sa.sin_addr)); // ��ʾ��ַ��
        TRsnprintf(hostip, 20, "%s", inet_ntoa(sa.sin_addr));
        break;
    }






#else
    register int fd,intrface,retn=0;
    struct ifreq buf[32];
    struct ifconf ifc;
    char domain_host[100] = {0};
    char local_ip_addr[20] = {0};
//    char local_mac[20] = {0};
    //Get Domain Name --------------------------------------------------
    if (!hostip)
        return -1;
    if (getdomainname(&domain_host[0], 100) != 0)
    {
        return -1;
    }
    //------------------------------------------------------------------
    //Get IP Address & Mac Address ----------------------------------------
    if ((fd=socket(AF_INET,SOCK_DGRAM,0))>=0)
    {
        ifc.ifc_len=sizeof buf;
        ifc.ifc_buf=(caddr_t)buf;
        if (!ioctl(fd,SIOCGIFCONF,(char*)&ifc))
        {
            intrface=ifc.ifc_len/sizeof(struct ifreq);
            while (intrface-->0)
            {
                if (!(ioctl(fd,SIOCGIFFLAGS,(char*)&buf[intrface])))
                {
                    if (buf[intrface].ifr_flags&IFF_PROMISC)
                    {
                        retn++;
                    }
                }
                //Get IP Address
                if (!(ioctl(fd,SIOCGIFADDR,(char*)&buf[intrface])))
                {
                    sprintf(local_ip_addr, "%s", inet_ntoa(((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr));
                }
                //Get Hardware Address
#if 0
                if (!(ioctl(fd,SIOCGIFHWADDR,(char*)&buf[intrface])))
                {


		    sprintf(local_mac,"%02x:%02x:%02x:%02x:%02x:%02x",
                            (unsigned char)buf[intrface].ifr_hwaddr.sa_data[0],
                            (unsigned char)buf[intrface].ifr_hwaddr.sa_data[1],
                            (unsigned char)buf[intrface].ifr_hwaddr.sa_data[2],
                            (unsigned char)buf[intrface].ifr_hwaddr.sa_data[3],
                            (unsigned char)buf[intrface].ifr_hwaddr.sa_data[4],
                            (unsigned char)buf[intrface].ifr_hwaddr.sa_data[5]);

                    break;
                }
#endif
            }//While
        }
    }
    if ( fd > 0 )
    {
        close(fd);
    }

    strcpy(hostip, local_ip_addr);
#endif

    return CWMP_OK;
}

cwmp_session_t * cwmp_session_create(cwmp_t * cwmp)
{


    pool_t * pool = pool_create(POOL_MIN_SIZE);
    cwmp_session_t * session = pool_pcalloc(pool, sizeof(cwmp_session_t));
    session->env = pool_pcalloc(pool, sizeof(env_t));
    session->env->cwmp = cwmp;
    session->cwmp = cwmp;
    cwmp_chunk_create( &session->writers, pool);
    cwmp_chunk_create(&session->readers, pool);

    session->pool = pool;
    session->status = 0;
    session->newdata = 0;
    session->envpool = NULL;
    session->connpool = NULL;

    session->resend_counter = 0u;

    session->root = cwmp->root;
    session->retry_count = 0;

    return session;
}

void cwmp_session_free(cwmp_session_t * session)
{
    pool_t * pool = session->pool;

    if (session->envpool)
    {
        pool_destroy(session->envpool);
        session->envpool = NULL;
    }
    if (session->connpool)
    {
        pool_destroy(session->connpool);
        session->connpool = NULL;
    }
    pool_destroy(pool);

}

int cwmp_session_close(cwmp_session_t * session)
{
    pool_destroy(session->envpool);
    pool_destroy(session->connpool);
    session->envpool = NULL;
    session->connpool = NULL;
    return 0;
}

int cwmp_session_open(cwmp_session_t * session)
{

    pool_t *envpool = pool_create(POOL_MIN_SIZE);

    session->connpool = pool_create(POOL_MIN_SIZE);
    if (!session->connpool)
    {
        cwmp_log_error("session init: create connection pool null.");
        return CWMP_ERROR;
    }
    session->envpool = envpool;
    session->env->pool = envpool;


    //pool_cleanup_add(envpool, cwmp_chunk_clear, session->writers);
    //pool_cleanup_add(envpool, cwmp_chunk_clear, session->readers);

    return CWMP_OK;
}

static size_t cwmp_session_write_callback(char *data, size_t size, size_t nmemb, void * calldata)
{
    cwmp_session_t * session = (cwmp_session_t *)calldata;

    cwmp_chunk_write_string(session->readers, data, size * nmemb, session->envpool);

    return size * nmemb;
}

int cwmp_session_connect(cwmp_session_t * session, const char * url)
{

    http_dest_t *  dest;
    int rv;

    http_dest_create(&dest, url, session->connpool);
    session->dest = dest;
    rv = cwmp_session_create_connection(session);
    if(rv != CWMP_OK)
    {
        return rv;
    }
    cwmp_session_set_headers(session, 0);

    return CWMP_OK;
}

int cwmp_session_set_auth(cwmp_session_t * session, const char * user, const char * pwd)
{
    cwmp_log_trace("%s(session=%p, user=\"%s\", pwd=\"%s\")",
            __func__, (void*)session, user, pwd);
    if (*user && *pwd) {
        TRstrncpy(session->dest->auth.username, user, URL_USER_LEN);
        TRstrncpy(session->dest->auth.password, pwd, URL_PWD_LEN);
    }

    return CWMP_OK;
}


int cwmp_session_set_headers(cwmp_session_t * session, int postempty)
{

    return 0;
}


int cwmp_session_create_connection(cwmp_session_t * session)
{

//    cwmp_t * cwmp = session->cwmp;
    http_socket_t * sock;
    int use_ssl = 0;
    http_dest_t *  dest = session->dest;
    int timeout = -1;

	cwmp_log_trace("%s(session=%p)", __func__, (void*)session);

    if(dest)
    {
        if(strncmp(dest->scheme, "https", 5) == 0)
        {
#ifdef USE_CWMP_OPENSSL
            use_ssl = 1;
#else
            cwmp_log_alert("Ignoring https scheme, force http");
#endif
        }
    }
    cwmp_log_debug("session connect using ssl: %s", use_ssl==1?"yes":"no");

    int rc = http_socket_create(&sock, AF_INET, SOCK_STREAM, 0, session->connpool);
    if (rc != CWMP_OK)
    {
        cwmp_log_error("ERROR: session connect - create socket error.");
        return rc;
    }

    cwmp_log_debug("dest host: %s, dest port: %d", session->dest->host, session->dest->port);
    if ((timeout = cwmp_conf_get_int("cwmpd:session_connect_timeout")) >= -1) {
        http_socket_set_recvtimeout(sock, timeout);
        http_socket_set_sendtimeout(sock, timeout);
    } else {
        http_socket_set_recvtimeout(sock, -1);
        http_socket_set_sendtimeout(sock, -1);
    }

    rc = http_socket_connect(sock, session->dest->host, session->dest->port);
    if(rc != CWMP_OK)
    {
        cwmp_log_alert("connect to ACS faild. Host is %s:%d.", session->dest->host, session->dest->port);
        return rc;
    }

    if(use_ssl)
    {
#ifdef USE_CWMP_OPENSSL
        SSL *ssl = openssl_connect(session->cwmp->ssl_ctx, sock->sockdes);
        if(ssl)
        {
           sock->ssl = ssl;
           sock->use_ssl = 1;
        }
#endif
    }
    if ((timeout = cwmp_conf_get_int("cwmpd:session_response_timeout")) >= -1) {
        http_socket_set_recvtimeout(sock, timeout);
        http_socket_set_sendtimeout(sock, timeout);
    } else {
        http_socket_set_recvtimeout(sock, -1);
        http_socket_set_sendtimeout(sock, -1);
    }

    http_socket_set_writefunction(sock, cwmp_session_write_callback, session);

    session->sock = sock;

    return CWMP_OK;
}

header_t * cwmp_session_create_header(cwmp_session_t * session, pool_t * pool)
{

    header_t * header;
    FUNCTION_TRACE();

    header = pool_palloc(pool, sizeof(header_t));
    header->hold_requests = 0;
    header->id = cwmp_session_get_sequence(pool);
    header->no_more_requests = 0;

    strncpy(session->id, header->id, 128);

    return header;
}

device_id_t * cwmp_session_create_inform_device(cwmp_session_t * session, pool_t * pool)
{
    device_id_t * device;
    char * name;
//    char * value;

    FUNCTION_TRACE();


    device = pool_palloc(pool, sizeof(device_id_t));
/*
    device->manufactorer = session->cwmp->cpe_mf;  //cwmp_get_parameter_value(InternetGatewayDeviceModule, DeviceInfoModule,ManufacturerModule);
    device->oui          = session->cwmp->cpe_oui; //cwmp_get_parameter_value(InternetGatewayDeviceModule, DeviceInfoModule, ManufacturerOUIModule);
    device->product_class = session->cwmp->cpe_pc; //cwmp_get_parameter_value(InternetGatewayDeviceModule, DeviceInfoModule, ProductClassModule);
    device->serial_number = session->cwmp->cpe_sn; //cwmp_get_parameter_value(InternetGatewayDeviceModule, DeviceInfoModule, SerialNumberModule);
    device->device_log = session->cwmp->cpe_mf; //cwmp_get_parameter_value(InternetGatewayDeviceModule, DeviceInfoModule, SerialNumberModule);
*/

    name    = CWMP_APPEND_PARAMETER_NAME(pool, 3, InternetGatewayDeviceModule, DeviceInfoModule,ManufacturerModule);
    device->manufactorer = cwmp_data_get_parameter_value(session->cwmp, session->root, name, pool);

    name    = CWMP_APPEND_PARAMETER_NAME(pool, 3, InternetGatewayDeviceModule, DeviceInfoModule,ManufacturerOUIModule);
    device->oui = cwmp_data_get_parameter_value(session->cwmp, session->root, name, pool);

    name    = CWMP_APPEND_PARAMETER_NAME(pool, 3, InternetGatewayDeviceModule, DeviceInfoModule,ProductClassModule);
    device->product_class = cwmp_data_get_parameter_value(session->cwmp, session->root, name, pool);

    name    = CWMP_APPEND_PARAMETER_NAME(pool, 3, InternetGatewayDeviceModule, DeviceInfoModule,SerialNumberModule);
    device->serial_number = cwmp_data_get_parameter_value(session->cwmp, session->root, name, pool);

    device->name = session->cwmp->cpe_name;

    return device;
}

parameter_list_t * cwmp_session_create_inform_parameters(cwmp_session_t * session, pool_t * pool)
{
    parameter_list_t * pl;

    parameter_t * parameter;

    //parameter_node_t * parameterNode;
    char * name;
    char * value;

    cwmp_log_trace("%s(session=%p, pool=%p)", __func__, (void*)session, (void*)pool);

    pl = cwmp_create_parameter_list(session->env);

    name    = CWMP_APPEND_PARAMETER_NAME(pool, 2, InternetGatewayDeviceModule, DeviceSummaryModule);
    value   = cwmp_data_get_parameter_value(session->cwmp, session->root, name, pool);
    parameter = cwmp_create_parameter(session->env,  name, value, 0, TYPE_STRING);
    cwmp_add_parameter_to_list(session->env,  pl, parameter);

    name    = CWMP_APPEND_PARAMETER_NAME(pool, 3, InternetGatewayDeviceModule, DeviceInfoModule, SpecVersionModule);
    value   = cwmp_data_get_parameter_value(session->cwmp, session->root, name, pool);
    parameter = cwmp_create_parameter(session->env,  name, value, 0, TYPE_STRING);
    cwmp_add_parameter_to_list(session->env,  pl, parameter);

    name    = CWMP_APPEND_PARAMETER_NAME(pool, 3, InternetGatewayDeviceModule, ManagementServerModule, ParameterKeyModule);
    value   = cwmp_data_get_parameter_value(session->cwmp, session->root, name, pool);
    parameter = cwmp_create_parameter(session->env,  name, value, 0, TYPE_STRING);
    cwmp_add_parameter_to_list(session->env,  pl, parameter);

    return pl;
}

event_list_t * cwmp_session_create_inform_events(cwmp_session_t * session, pool_t * pool)
{
    event_list_t * el;
    event_code_t * ev;
//    int i=0;

    FUNCTION_TRACE();

    el = cwmp_create_event_list(session->env, INFORM_MAX);

    if (el->count == 0)
    {
        ev = cwmp_create_event_code(session->env);
        ev->event = 1;
        ev->code = CWMP_INFORM_EVENT_CODE_1;
        el->events[el->count++] = ev;
    }

    return el;
}



datatime_t *cwmp_session_create_inform_datetimes(cwmp_session_t * session, pool_t * pool)
{
    struct tm t;
    time_t tn;
    datatime_t *now;

    //FUNCTION_TRACE();
    tn = time(NULL);
#ifdef WIN32
    cwmp_log_debug("inform datatime");
    //localtime_s(&t, &tn);
    memset(&t, 0, sizeof(struct tm));
#else
    t = *localtime(&tn);
#endif

    now = pool_palloc(pool, sizeof(datatime_t));
    now->year = t.tm_year + 1900;
    now->month = t.tm_mon + 1;
    now->day = t.tm_mday;
    now->hour = t.tm_hour;
    now->min = t.tm_min;
    now->sec = t.tm_sec;

    return now;
}

xmldoc_t *  cwmp_session_create_inform_message(cwmp_session_t * session, event_list_t * evtlist,  pool_t * pool)
{

    header_t * header;
    device_id_t * device;
//    event_list_t * el;
    datatime_t *now;
    parameter_list_t * pl;

    FUNCTION_TRACE();

    header = cwmp_session_create_header(session, pool);
    device  = cwmp_session_create_inform_device(session, pool);
    pl      = cwmp_session_create_inform_parameters(session, pool);

    now     = cwmp_session_create_inform_datetimes(session, pool);

    return  cwmp_create_inform_message(session->env, header, device, evtlist, now, 1, session->retry_count, pl, session->root);
}

xmldoc_t *  cwmp_session_create_transfercomplete_message(cwmp_session_t * session, event_code_t * evcode,  pool_t * pool)
{

    header_t * header;
//    device_id_t * device;
//    event_list_t * el;
//    datatime_t *now;
//    parameter_list_t * pl;

    FUNCTION_TRACE();

    header = cwmp_session_create_header(session, pool);

    return  cwmp_create_transfercomplete_message(session->env, header, evcode);

}

xmldoc_t *  cwmp_session_create_getrpcmethods_response_message(cwmp_session_t * session, xmldoc_t * doc, pool_t * pool)
{
    header_t * header;
    int rv;
    FUNCTION_TRACE();
    rv = cwmp_parse_header_node(cwmp_get_header_node(doc), &header, pool);
    if (rv != CWMP_OK) {
        cwmp_log_debug("no header node");
    }
    return cwmp_create_getrpcmethods_response_message(session->env, header, rpc_methods, sizeof(rpc_methods)/sizeof(rpc_methods[0]));
}

xmldoc_t *  cwmp_session_create_getparameternames_response_message(cwmp_session_t * session, xmldoc_t * doc, pool_t * pool)
{
    header_t * header;
    int rv;
    char * path;
    unsigned int next_level;
    unsigned int next_subset;
    parameter_node_t * node;
    fault_code_t fault;

    cwmp_log_trace("%s(session=%p, doc=%p, pool=%p)",
            __func__, (void*)session, (void*)doc, (void*)pool);

    rv = cwmp_parse_header_node(cwmp_get_header_node(doc), &header, pool);
    if (rv != CWMP_OK) {
        cwmp_log_debug("no header node");
    }

    rv = cwmp_parse_getparameternames_message(session->env, doc, &path, &next_level, &fault);

    if (path[strlen(path)-1] == '.')
    {
        next_subset = CWMP_YES;
    }
    else
    {
        next_subset = CWMP_NO;
    }

    node = cwmp_get_parameter_path_node(session->root, path);


    return cwmp_create_getparameternames_response_message(session->env, header, path, node, next_subset, next_level);
}


xmldoc_t *  cwmp_session_create_getparametervalues_response_message(cwmp_session_t * session, xmldoc_t * doc, pool_t * pool)
{
    header_t * header;
    int rv;
    parameter_list_t * pl;
    fault_code_t fault;

    cwmp_log_trace("%s(session=%p, doc=%p, pool=%p)",
            __func__, (void*)session, (void*)doc, (void*)pool);

    rv = cwmp_parse_header_node(cwmp_get_header_node(doc), &header, pool);
    if (rv != CWMP_OK) {
        cwmp_log_debug("no header node");
    }

    rv = cwmp_parse_getparametervalues_message(session->env, doc, session->root, &pl, &fault);



    return cwmp_create_getparametervalues_response_message(session->env, header, pl);
}

xmldoc_t *  cwmp_session_create_setparametervalues_response_message(cwmp_session_t * session, xmldoc_t * doc, pool_t * pool)
{
    header_t * header;
    int rv;
    parameter_list_t * pl;
    fault_code_t fault;


//    parameter_t * parameter;
//    char * name;
//    char * value;


    cwmp_log_trace("%s(session=%p, doc=%p, pool=%p)",
            __func__, (void*)session, (void*)doc, (void*)pool);
    rv = cwmp_parse_header_node(cwmp_get_header_node(doc), &header, pool);
    if (rv != CWMP_OK) {
        cwmp_log_debug("no header node");
    }

    rv = cwmp_parse_setparametervalues_message(session->env, doc, session->root, &pl, &fault);


    if(rv != CWMP_OK)
    {
        return cwmp_create_faultcode_setparametervalues_response_message(session->env, header, pl, &fault);
    }

    xmldoc_t * resdoc = cwmp_create_setparametervalues_response_message(session->env, header, 0, pl);
    //FIXME: D-Link refresh
    //session->cwmp->new_request = CWMP_YES;

    return resdoc;
}

xmldoc_t *  cwmp_session_create_setparameterattributes_response_message(cwmp_session_t * session, xmldoc_t * doc, pool_t * pool)
{
    header_t * header;
    int rv;
    parameter_list_t * pl = NULL;
    fault_code_t fault;

//    parameter_t * parameter;
//    char * name;
//    char * value;

    // FIXME: STUB!!!

    cwmp_log_trace("%s(session=%p, doc=%p, pool=%p)",
            __func__, (void*)session, (void*)doc, (void*)pool);

    rv = cwmp_parse_header_node(cwmp_get_header_node(doc), &header, pool);
    if (rv != CWMP_OK) {
        cwmp_log_debug("no header node");
    }

    rv = cwmp_parse_setparameterattributes_message(session->env, doc, session->root, &pl, &fault);


/*    if(rv != CWMP_OK)
    {
	//FIXME STUB!
//        return cwmp_create_faultcode_setparametervalues_response_message(session->env, header, pl, &fault);
	return resdoc;
    }
*/
    xmldoc_t * resdoc = cwmp_create_setparameterattributes_response_message(session->env, header, 0, pl);
    //FIXME: D-Link refresh
    //session->cwmp->new_request = CWMP_YES;

    return resdoc;
}




xmldoc_t *  cwmp_session_create_download_response_message(cwmp_session_t * session, xmldoc_t * doc, pool_t * pool)
{
    header_t * header;
    int rv;
//    char * key;
    fault_code_t fault;
    FUNCTION_TRACE();
    rv = cwmp_parse_header_node(cwmp_get_header_node(doc), &header, pool);
    if (rv != CWMP_OK) {
        cwmp_log_debug("no header node");
    }

    download_arg_t * dlarg;

    rv = cwmp_parse_download_message(session->env, doc, &dlarg, &fault);

    //add download arg to taskqueue
    //begin download process

   if(rv == CWMP_OK)
    {
	download_arg_t * newdlarg = cwmp_clone_download_arg(dlarg);
	if(newdlarg != NULL)
	{
		cwmp_t * cwmp = session->cwmp;

		queue_push(cwmp->queue, newdlarg, TASK_DOWNLOAD_TAG);

		cwmp_log_debug("push new download task to queue! url: %s ", newdlarg->url);
	}
    }

   int status = 1;

   return cwmp_create_download_response_message(session->env, header, status);
}

xmldoc_t *  cwmp_session_create_upload_response_message(cwmp_session_t * session, xmldoc_t * doc, pool_t * pool)
{
    header_t * header;
    int rv;
//    char * key;
    fault_code_t fault;
    FUNCTION_TRACE();
    rv = cwmp_parse_header_node(cwmp_get_header_node(doc), &header, pool);
    if (rv != CWMP_OK) {
        cwmp_log_debug("no header node");
    }

    upload_arg_t * uparg;

    rv = cwmp_parse_upload_message(session->env, doc, &uparg, &fault);

   if(rv == CWMP_OK)
    {
	upload_arg_t * newularg = cwmp_clone_upload_arg(uparg);
	if(newularg)
	{
		cwmp_t * cwmp = session->cwmp;
		queue_push(cwmp->queue, newularg, TASK_UPLOAD_TAG);
		cwmp_log_debug("push new upload task to queue! url: %s ", newularg->url);
	}
    }

    int status = 1;
    return cwmp_create_upload_response_message(session->env, header, status);
}

xmldoc_t *  cwmp_session_create_addobject_response_message(cwmp_session_t * session, xmldoc_t * doc, pool_t * pool)
{
    header_t * header;
    int rv;
    int instances, status;
    fault_code_t fault;
    FUNCTION_TRACE();
    rv = cwmp_parse_header_node(cwmp_get_header_node(doc), &header, pool);
    if (rv != CWMP_OK) {
        cwmp_log_debug("no header node");
    }
    rv = cwmp_parse_addobject_message(session->env, doc, session->root, &instances, &status,  &fault);
    if(rv != CWMP_OK)
    {
	return cwmp_create_faultcode_response_message(session->env, header, &fault);
    }

    return cwmp_create_addobject_response_message(session->env, header, instances, status);
}

xmldoc_t *  cwmp_session_create_deleteobject_response_message(cwmp_session_t * session, xmldoc_t * doc, pool_t * pool)
{
    header_t * header;
    int rv, status;
    fault_code_t fault;
    FUNCTION_TRACE();
    rv = cwmp_parse_header_node(cwmp_get_header_node(doc), &header, pool);
    if (rv != CWMP_OK) {
        cwmp_log_debug("no header node");
    }

    rv = cwmp_parse_deleteobject_message(session->env, doc, session->root, &status, &fault);

    if(rv != CWMP_OK)
    {
	return cwmp_create_faultcode_response_message(session->env, header, &fault);
    }


    return cwmp_create_deleteobject_response_message(session->env, header, status);
}


xmldoc_t *  cwmp_session_create_reboot_response_message(cwmp_session_t * session, xmldoc_t * doc, pool_t * pool)
{
    header_t * header;
    int rv;
    char * key;
    fault_code_t fault;

    cwmp_log_trace("%s(session=%p, doc=%p, pool=%p)",
            __func__, (void*)session, (void*)doc, (void*)pool);

    rv = cwmp_parse_header_node(cwmp_get_header_node(doc), &header, pool);
    if (rv != CWMP_OK)
    {
        cwmp_log_debug("no header node");
    }

    rv = cwmp_parse_reboot_message(session->env, doc, &key, &fault);

    cwmp_t * cwmp = session->cwmp;
    queue_push(cwmp->queue, NULL, TASK_REBOOT_TAG);

    return cwmp_create_reboot_response_message(session->env, header);
}

xmldoc_t *  cwmp_session_create_factoryreset_response_message(cwmp_session_t * session, xmldoc_t * doc, pool_t * pool)
{
    header_t * header;
    int rv;
//    char * key;
//    fault_code_t fault;
    FUNCTION_TRACE();
    rv = cwmp_parse_header_node(cwmp_get_header_node(doc), &header, pool);
    if (rv != CWMP_OK) {
        cwmp_log_debug("no header node");
    }

    cwmp_t * cwmp = session->cwmp;
    queue_push(cwmp->queue, NULL, TASK_FACTORYRESET_TAG);

    return cwmp_create_factoryreset_response_message(session->env, header);
}

int cwmp_session_send_request(cwmp_session_t * session)
{
    int rv;
    http_request_t * request;
    cwmp_log_trace("%s(session=%p)", __func__, (void*)session);

    http_request_create(&request, session->envpool);
    request->dest = session->dest;

    rv = http_post(session->sock, request, session->writers, session->envpool);

    if (rv <= 0)
    {
        return CWMP_ERROR;
    }
    else
    {
        return CWMP_OK;
    }
}

int cwmp_session_recv_response(cwmp_session_t * session)
{
    int respcode;
    http_response_t * response;
    char * auth;
    char * cookie;
    //select session->sock
    //if have new data, then read it

    cwmp_log_trace("%s(session=%p)", __func__, (void*)session);

    http_response_create(&response, session->envpool);
    response->readers = session->readers;
    respcode= http_read_response(session->sock, response, session->envpool);

    session->last_code = response->status;

    if(respcode != HTTP_200 && respcode != HTTP_204)
    {
        if(response->status == 401 ||response->status == 407)
        {
            auth = http_get_variable(response->parser, "WWW-Authenticate");
            if(auth)
            {
                http_parse_digest_auth(auth, &session->dest->auth, session->dest->uri);
            }
        } else {
            cwmp_log_error("http read response failed. return code is %d", response->status);
        }
    }

    if(session->last_method == CWMP_INFORM_METHOD)
    {
        cookie = http_get_variable(response->parser, "Set-Cookie");
        if(cookie)
        {
            http_parse_cookie(cookie, session->dest->cookie);
        }
    }

    return respcode;
}

