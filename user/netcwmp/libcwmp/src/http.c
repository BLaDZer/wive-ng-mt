/* vim: set et: */
/************************************************************************
 * Id: http.c                                                           *
 *                                                                      *
 * TR069 Project:  A TR069 library in C                                 *
 * Copyright (C) 2013-2014  netcwmp.netcwmp group                                *
 *                                                                      *
 *                                                                      *
 * Email: netcwmp ( & ) gmail dot com                                *
 *                                                                      *
 ***********************************************************************/

#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

#include "cwmp/http.h"
#include "cwmp/log.h"
#include "cwmp_private.h"
#include <cwmp/md5.h>



struct http_sockaddr_t
{
    struct sockaddr_in sin4;

#if HAVE_IPV6
    /** IPv6 sockaddr structure */
    struct sockaddr_in6 sin6;
#endif
};


int http_calc_digest_response(const char *method, const char * user, const char * pwd, http_digest_auth_t *digest);



char * http_get_variable(http_parser_t * parser, const char *name)
{
    int i;
    for (i=0; i<parser->count; i++)
    {
        if (TRstrcasecmp(parser->header[i]->name, name) == 0)
        {
            return parser->header[i]->value;
        }
    }

    return NULL;

}

void http_set_variable(http_parser_t *parser, const char *name, const char *value, pool_t * pool)
{
    key_value_t *var;

    cwmp_log_trace("%s(parser=%p, name=\"%s\", value=\"%s\", pool=%p)",
            __func__, (void*)parser, name, value, (void*)pool);

    if (name == NULL || value == NULL)
        return;


    var = (key_value_t *)pool_pcalloc(pool, sizeof(key_value_t));
    if (var == NULL)
    {
        return;
    }

    var->name = pool_pstrdup_lower(pool, name);
    var->value = pool_pstrdup(pool, value);
    if (parser->count >= MAX_HEADERS)
    {
        return;
    }
    parser->header[parser->count++] = var;
}


int http_connect(http_socket_t * sock, const char * url)
{
    return 0;
}

int http_dest_create(http_dest_t ** dest, const char * url, pool_t * pool)
{
    http_dest_t * d = (http_dest_t*)pool_pcalloc(pool, sizeof(http_dest_t));
//    cwmp_uint32_t length = TRstrlen(url);
    http_parse_url(d, url);
    d->url = pool_pstrdup(pool, url);
//	d->url = (char *)pool_pcalloc(pool, length+1);
//	strncpy(d->url, url, length);
    cwmp_log_debug("dest create url is %s", d->url);
    *dest = d;
    return CWMP_OK;
}

void http_sockaddr_set(http_sockaddr_t * addr, int family, int port, const char * host)
{
    addr->sin4.sin_family = family;

    if (family == AF_INET)
    {

    }

    if (port)
    {
        addr->sin4.sin_port = htons((unsigned short)port);
    }

    if (host)
    {
        //inet_aton(host, &addr->sin4.sin_addr);
        addr->sin4.sin_addr.s_addr = inet_addr(host);
    }
    else
    {
       // addr->sin4.sin_addr.s_addr = INADDR_ANY;
    }
}


int http_socket_calloc(http_socket_t **news, pool_t * pool)
{
    cwmp_log_trace("%s(news=%p, pool=%p)", __func__, (void*)news, (void*)pool);
    (*news) = (http_socket_t *)pool_pcalloc(pool, sizeof(http_socket_t));

    if ((*news) == NULL)
    {
        cwmp_log_error("socket create pool pcalloc null.");
        return CWMP_ERROR;
    }

    (*news)->addr = (http_sockaddr_t*)pool_pcalloc(pool, sizeof(http_sockaddr_t));
    if ((*news)->addr == NULL)
    {
        (*news) = NULL;
        cwmp_log_error("http_sockaddr_t  pool pcalloc  null.");
        return CWMP_ERROR;
    }
    (*news)->sockdes = -1;
    (*news)->recv_timeout = -1;
    (*news)->send_timeout = -1;
    (*news)->pool = pool;


    pool_cleanup_add(pool, (pool_cleanup_handler)http_socket_close, (*news));
    return CWMP_OK;
}


int http_socket_create(http_socket_t **news, int family, int type, int protocol, pool_t * pool)
{
    int stat;
    stat = http_socket_calloc(news, pool);
    if (stat == CWMP_ERROR)
    {
        return CWMP_ERROR;
    }


    (*news)->sockdes = socket(family, type, protocol);

#if HAVE_IPV6
    if ((*news)->sockdes == -1)
    {
        family = AF_INET;
        (*news)->sockdes = socket(family, type, protocol);
    }
#endif

    if ((*news)->sockdes == -1)
    {
        cwmp_log_error("sockdes is -1.");
        return - errno;
    }

    (*news)->type = type;
    (*news)->protocol = protocol;
    http_sockaddr_set((*news)->addr,family, 0, NULL);
    (*news)->recv_timeout = -1;
    (*news)->send_timeout = -1;

    return CWMP_OK;
}

int http_socket_server (http_socket_t **news, int port, int backlog, int timeout, pool_t * pool)
{
    int i;
    http_socket_t * sock;
    int rc;

    rc = http_socket_create(&sock, AF_INET, SOCK_STREAM, 0, pool);
    if (rc != CWMP_OK)
    {
        cwmp_log_error("http_socket_create faild. %s", strerror(errno));
        return CWMP_ERROR;
    }
    i = 1;
    if (setsockopt (sock->sockdes, SOL_SOCKET, SO_REUSEADDR, (void *)&i, sizeof i) == -1)
    {
        cwmp_log_error ("http_socket_server: setsockopt SO_REUSEADDR: %sock", strerror (errno));
    }

    http_sockaddr_set(sock->addr, AF_INET, port, NULL);

    if (bind (sock->sockdes, (struct sockaddr *)&sock->addr->sin4, sizeof (struct sockaddr)) == -1)
    {
        http_socket_close (sock);
        return CWMP_ERROR;
    }

    if (listen (sock->sockdes, (unsigned)backlog) == -1)
    {
        http_socket_close (sock);
        return CWMP_ERROR;
    }

    *news = sock;

    return CWMP_OK;


}

void
saddr_char(char *str, size_t size, sa_family_t family, struct sockaddr *sa)
{
    char xhost[40];
    unsigned short port = 0;
    switch(family) {
    case AF_INET:
        inet_ntop(AF_INET, &((struct sockaddr_in*)sa)->sin_addr,
                xhost, sizeof(xhost));
        port = ntohs(((struct sockaddr_in*)sa)->sin_port);
        if (port) {
            snprintf(str, size, "%s:%u", xhost, (unsigned short)port);
        } else {
            snprintf(str, size, "%s", xhost);
        }
        break;
    case AF_INET6:
        inet_ntop(AF_INET6, &((struct sockaddr_in6*)sa)->sin6_addr,
                xhost, sizeof(xhost));
        port = ntohs(((struct sockaddr_in6*)sa)->sin6_port);
        if (port) {
            snprintf(str, size, "[%s]:%u", xhost, port);
        } else {
            snprintf(str, size, "%s", xhost);
        }
        break;
    default:
        snprintf(str, size, "[unknown fa: %d]", family);
        break;
    }
}

/* substract tv2 from tv1, result in tv1
 * if tv2 > tv1 result is tv1.tv_sec = 0, tv1.tv_usec = 0
 */
static void
timeval_substract_tv(struct timeval *tv1, const struct timeval *tv2)
{
    if (tv2->tv_usec > tv1->tv_usec) {
        if (tv1->tv_sec >= 1) {
            tv1->tv_sec--;
            tv1->tv_usec += 1000000;
        }
    }
    if (tv2->tv_sec > tv1->tv_sec || tv2->tv_usec > tv1->tv_usec) {
        tv1->tv_sec = 0u;
        tv1->tv_usec = 0u;
    } else {
        tv1->tv_sec -= tv2->tv_sec;
        tv1->tv_usec -= tv2->tv_usec;
    }
}

static bool
timeval_is_null(struct timeval *tv)
{
    if (!tv->tv_sec && !tv->tv_usec)
        return true;
    return false;
}

static void
timeval_calc_timeout(const struct timeval *begin, unsigned segments, int timeout, struct timeval *result)
{
    struct timeval tv_timeout = { .tv_sec = timeout, .tv_usec = 0 };
    struct timeval ctv = {};
    useconds_t usec = 0;
    gettimeofday(&ctv, NULL);

    memset(result, 0u, sizeof(*result));

    timeval_substract_tv(&ctv, begin);
    if (timeval_is_null(&ctv)) {
        return;
    }

    timeval_substract_tv(&tv_timeout, &ctv);
    if (segments) {
        /* get part of global timeout */
        usec = tv_timeout.tv_sec * 1000000 + tv_timeout.tv_usec;
        usec = usec / segments;
        tv_timeout.tv_sec = usec / 1000000;
        tv_timeout.tv_usec = usec % 1000000;
    }
    memcpy(result, &tv_timeout, sizeof(*result));
}

int http_socket_connect(http_socket_t * sock, const char * host, int port)
{
    struct addrinfo hints = {.ai_family = AF_UNSPEC, .ai_socktype = SOCK_STREAM};
    struct addrinfo *result = NULL;
    struct addrinfo *res = NULL;
    char nport[16] = {};
    int rval = 0;
    struct timeval tv = {};
    struct timeval tvr = {};
    struct timeval tvs = {};
    unsigned segments = 0u;
    unsigned no = 0u;
    cwmp_log_trace("%s(sock=%p, host=\"%s\", port=%d)",
            __func__, (void*)sock, host, port);

    cwmp_log_info("resolving destination %s:%d", host, port);

    if (sock->sockdes != 0 && sock->sockdes != -1) {
        /* sucks functions:
         * http_socket_create()
         * http_sockaddr_set()
        */
        close(sock->sockdes);
        sock->sockdes = -1;
    }
    memset(&sock->stat, 0, sizeof(sock->stat));
    gettimeofday(&sock->stat.ns_query, NULL);
    snprintf(nport, sizeof(nport), "%d", port);
    rval = getaddrinfo(host, nport, &hints, &result);
    if (rval != 0) {
        if (rval == EAI_SYSTEM) {
            cwmp_log_info("getaddrinfo(): %s", strerror(errno));
        } else {
            cwmp_log_info("getaddrinfo(): %s", gai_strerror(rval));
        }
        return CWMP_ERROR;
    } else if (!result) {
        cwmp_log_info("address not resolved");
        return CWMP_ERROR;
    }

    gettimeofday(&sock->stat.tcp_connect, NULL);
    /* calc */
    for (res = result; res; res = res->ai_next) {
        segments++;
    }
    /* try connect */
    for (res = result, no = 1u; res; no++, res = res->ai_next) {
        char xaddr[96] = {};
        sock->sockdes = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        saddr_char(xaddr, sizeof(xaddr),\
                res->ai_family, (struct sockaddr*)res->ai_addr);

        if (sock->sockdes == -1) {
            cwmp_log_info("socket(): %s", strerror(errno));
            goto gai_error;
        }

        /* setup options */
        if (sock->recv_timeout > 0) {
            timeval_calc_timeout(&sock->stat.tcp_connect, segments, sock->recv_timeout, &tvr);
            if (timeval_is_null(&tvr)) {
                cwmp_log_error("connect: recv timeout reached, value=%d", sock->recv_timeout);
                goto timeout;
            }
            setsockopt(sock->sockdes, SOL_SOCKET, SO_RCVTIMEO, &tvr, sizeof(tvr));
        }
        if (sock->send_timeout > 0) {
            timeval_calc_timeout(&sock->stat.tcp_connect, segments, sock->send_timeout, &tvs);
            if (timeval_is_null(&tvs)) {
                cwmp_log_error("connect: send timeout reached, value=%d", sock->send_timeout);
                goto timeout;
            }
            setsockopt(sock->sockdes, SOL_SOCKET, SO_SNDTIMEO, &tvs, sizeof(tvs));
        }

        cwmp_log_info(
                "connecting to address: %s (%u/%u, timeouts=["
                    "recv: %"PRIdPTR".%.06"PRIdPTR", "
                    "send: %"PRIdPTR".%.06"PRIdPTR"])",
                xaddr, no, segments,
                tvr.tv_sec, tvr.tv_usec,
                tvs.tv_sec, tvs.tv_usec
                );

        /* connect */
        if (connect(sock->sockdes, res->ai_addr, res->ai_addrlen) == -1) {
            if (errno == EINPROGRESS) {
                cwmp_log_info("connect(): timeout reached, value=%d",
                        (sock->send_timeout > sock->recv_timeout ?
                            sock->recv_timeout : sock->send_timeout));
            } else {
                cwmp_log_info("connect(): %s", strerror(errno));
            }
            close(sock->sockdes);
            sock->sockdes = -1;
            continue;
        }
        gettimeofday(&sock->stat.tcp_response, NULL);
        memcpy(&tv, &sock->stat.tcp_response, sizeof(tv));
        timeval_substract_tv(&tv, &sock->stat.tcp_connect);
        if (sock->recv_timeout != -1 || sock->send_timeout != -1) {
            timeval_calc_timeout(&sock->stat.tcp_connect, 0u, sock->send_timeout, &tvs);
            timeval_calc_timeout(&sock->stat.tcp_connect, 0u, sock->recv_timeout, &tvr);
            cwmp_log_info(
                    "connected to: %s:%d (%s), reaming timeouts=["
                    "recv: %"PRIdPTR".%.06"PRIdPTR", "
                    "send: %"PRIdPTR".%.06"PRIdPTR"], time=%"PRIdPTR".%.06"PRIdPTR,
                    host, port, xaddr,
                    tvr.tv_sec, tvr.tv_usec,
                    tvs.tv_sec, tvs.tv_usec,
                    tv.tv_sec, tv.tv_usec);
        } else {
            cwmp_log_info("connected to: %s:%d (%s), time=%"PRIdPTR".%.06"PRIdPTR,
                    host, port, xaddr, tv.tv_sec, tv.tv_usec);
        }

        break;
    }

    freeaddrinfo(result);
    if (sock->sockdes == -1) {
        return CWMP_ERROR;
    }
    return CWMP_OK;

gai_error:
    freeaddrinfo(result);
    return CWMP_ERROR;
timeout:
    freeaddrinfo(result);
    return CWMP_TIMEOUT;
}

int http_socket_accept(http_socket_t *sock, http_socket_t ** news)
{
    struct sockaddr addr;
    size_t len;
    pool_t * pool;
    int rc, s;
    char xaddr[96] = {};
    cwmp_log_trace("%s(sock=%p, news=%p)", __func__, (void*)sock, (void*)news);

    len = sizeof addr;
    s = accept (sock->sockdes, &addr, &len);
    if (s == -1)
    {
        return CWMP_ERROR;
    }

    saddr_char(xaddr, sizeof(xaddr), addr.sa_family, &addr);
    cwmp_log_info("accepted connection from: %s", xaddr);

    pool = pool_create(POOL_DEFAULT_SIZE);
    rc = http_socket_calloc(news, pool);
    if (rc != CWMP_OK)
    {
        return CWMP_ERROR;
    }
    (*news)->sockdes = s;
    memcpy(&(*news)->addr->sin4, &addr, sizeof(struct sockaddr_in));


    return CWMP_OK;

}



void http_socket_close(http_socket_t * sock)
{
    cwmp_log_trace("%s(sock=%p)", __func__, (void*)sock);
    if (sock)
    {
        if (sock->sockdes != -1)
        {
#ifdef WIN32
            closesocket(sock->sockdes);
#else
            close(sock->sockdes);
#endif
            sock->sockdes = -1;
        }

    }
}

void http_socket_destroy(http_socket_t * sock)
{
    pool_t * pool;
    pool = sock->pool;

    pool_destroy(pool);

}

int http_socket_get_fd(http_socket_t * sock)
{
    if (sock)
        return sock->sockdes;
    else
        return -1;
}

pool_t * http_socket_get_pool(http_socket_t * sock)
{
    if(sock && sock->pool)
    {
        return sock->pool;
    }
    else
    {
        return NULL;
    }
}


int http_socket_read (http_socket_t * sock, char *buf, int bufsize)
{
    int res = 0;

    if(sock->use_ssl)
    {

#ifdef USE_CWMP_OPENSSL
        do
        {
            res = SSL_read(sock->ssl, buf, bufsize);
        }
        while (res == -1 && errno == EINTR);
#endif
    }
    else
    {
        do
        {
            res = recv (sock->sockdes, buf, bufsize, 0);
        }
        while (res == -1 && errno == EINTR);

    if (res == -1 && errno != 0) {
            cwmp_log_error("http_socket_read ERRNO: %d, res %d, buf %d, bufsize %d", errno, res, buf, bufsize);
    }


//        cwmp_log_error("http_socket_read ERRNO: %d, res %d, buf %d, bufsize %d", errno, res, buf, bufsize);
/*    if (res == 1) {
        buf[1] = 0;
            cwmp_log_error("%s", buf);
    }
    else
    {
            cwmp_log_error("http_socket_read ERRNO: %d, res %d, buf %d, bufsize %d", errno, res, buf, bufsize);
    }
*/
    }

    if (res > 0) {
        sock->stat.bytes_rx += res;
        if(!sock->stat.transmission_rx.tv_sec) {
            gettimeofday(&sock->stat.transmission_rx, NULL);
        }
    }

    return res;
}

int http_socket_write (http_socket_t * sock, const char *buf, int bufsize)
{
    int res = 0;
    if(sock->use_ssl)
    {
        cwmp_log_debug("http socket ssl write buffer: %s, length: %d", buf, bufsize);
#ifdef USE_CWMP_OPENSSL
        do
        {

            res = SSL_write (sock->ssl, buf, bufsize);
        }
        while (res == -1 && errno == EINTR);
#endif
        return res;
    }
    else
    {
        cwmp_log_debug("http socket write buffer fd:%d, length:%d,  [\n%s\n]", sock->sockdes, bufsize, buf);
        do
        {

            res = send (sock->sockdes, buf, bufsize, 0);
        }
        while (res == -1 && errno == EINTR);
        return res;

    }
}

void http_socket_set_sendtimeout(http_socket_t * sock, int timeout)
{
    struct timeval to;
    to.tv_sec = timeout;
    to.tv_usec = 0;
    cwmp_log_trace("%s(sock=%p, timeout=%d)", __func__, (void*)sock, timeout);
    sock->send_timeout = timeout;
    if (sock->sockdes == -1)
        return;
    setsockopt(sock->sockdes, SOL_SOCKET, SO_SNDTIMEO,
               (char *) &to,
               sizeof(to));
}

void http_socket_set_recvtimeout(http_socket_t * sock, int timeout)
{
    struct timeval to;
    to.tv_sec = timeout;
    to.tv_usec = 0;
    cwmp_log_trace("%s(sock=%p, timeout=%d)", __func__, (void*)sock, timeout);
    sock->recv_timeout = timeout;
    if (sock->sockdes == -1)
        return;
    setsockopt(sock->sockdes, SOL_SOCKET, SO_RCVTIMEO,
               (char *) &to,
               sizeof(to));
}

int http_socket_set_writefunction(http_socket_t * sock, http_write_callback_pt callback, void * calldata)
{
    if(!sock)
    {
        return CWMP_ERROR;
    }
    sock->write_callback = callback;
    sock->write_calldata = calldata;
    return CWMP_OK;
}


int http_request_create(http_request_t ** request , pool_t * pool)
{
    http_request_t * req;
    cwmp_log_trace("%s(request=%p, pool=%p)",
            __func__, (void*)request, (void*)pool);
    req = (http_request_t*)pool_pcalloc(pool, sizeof(http_request_t));
    req->parser = (http_parser_t*)pool_pcalloc(pool, sizeof(http_parser_t));

    *request = req;

    return CWMP_OK;
}

int http_response_create(http_response_t ** response, pool_t * pool)
{
    http_response_t * res;
    cwmp_log_trace("%s(response=%p, pool=%p)",
            __func__, (void*)response, (void*)pool);
    res = (http_response_t*)pool_pcalloc(pool, sizeof(http_response_t));
    res->parser = (http_parser_t*)pool_pcalloc(pool, sizeof(http_parser_t));

    *response = res;

    return CWMP_OK;
}


int http_parse_cookie(const char * cookie, char * dest_cookie)
{
//    char data[MIN_BUFFER_LEN+1] = {0};
    char * s ;
//    char buffer[128];
//    char * end;

    FUNCTION_TRACE();

    if (!cookie)
        return CWMP_ERROR;

    for (s =  (char*)cookie; isspace(*s); s++);


    TRstrncpy(dest_cookie, s, MIN_BUFFER_LEN);

    return CWMP_OK;

}


void http_parse_key_value(char ** from, char *to, int len, int shift)
{
    int n;
    char fmt[20];
    char *p = *from + shift;

    *from = p;

    if (*p == '"')//notice that '"' is not two " ,but ' and " and ',Jeff Sun - Jul.24.2005
    {
        TRsnprintf(fmt, sizeof(fmt), "%%%d[^\"]%%n", len - 1);
        p++;
    }
    else
    {
        TRsnprintf(fmt, sizeof(fmt), "%%%d[^ \t,]%%n", len - 1);
    }

    if (sscanf(p, fmt, to, &n))
    {
        p += n;
        *from = p;
    }
}




int http_parse_url(http_dest_t * dest, const char * url)
{
    char *d;
    const char *p, *q;
    const char * uri;
    int i;

    /* allocate struct url */
    //char urlbuf[1024] = {0};
    //strncpy(urlbuf, url, strlen(url));
    cwmp_log_trace("%s(dst=%p, url=\"%s\")",
            __func__, (void*)dest, url);

    uri = url;
    /* scheme name */
    if ((p = strstr(url, ":/")))
    {
        TRsnprintf(dest->scheme, URL_SCHEME_LEN+1,
                   "%.*s", (int)(p - uri), uri);
        uri = ++p;
        /*
         * Only one slash: no host, leave slash as part of document
         * Two slashes: host follows, strip slashes
         */
        if (uri[1] == '/')
            uri = (p += 2);
    }
    else
    {
        p = uri;
        *dest->scheme = '\0';
        /* default scheme: http */
        TRstrncpy(dest->scheme, "http", sizeof(dest->scheme));
    }
    if (!*uri || *uri == '/' || *uri == '.')
        goto nohost;

    p = strpbrk(uri, "/@");
    if (p && *p == '@')
    {
        /* username */
        for (q = uri, i = 0; (*q != ':') && (*q != '@'); q++)
            if (i < URL_USER_LEN)
            {
                dest->auth.username[i++] = *q;
            }

        /* password */
        if (*q == ':')
            for (q++, i = 0; (*q != ':') && (*q != '@'); q++)
                if (i < URL_PWD_LEN)
                {
                    dest->auth.password[i++] = *q;
                }

        p++;
    }
    else
    {
        p = uri;
    }

    /* hostname */
#ifdef INET6
    if (*p == '[' && (q = strchr(p + 1, ']')) != NULL &&
            (*++q == '\0' || *q == '/' || *q == ':'))
    {
        if ((i = q - p - 2) > MAX_HOST_NAME_LEN)
            i = MAX_HOST_NAME_LEN;
        strncpy(dest->host, ++p, i);

        p = q;
    }
    else
#endif
        memset(dest->host, 0, MAX_HOST_NAME_LEN+1);
    for (i = 0; *p && (*p != '/') && (*p != ':'); p++)
        if (i < MAX_HOST_NAME_LEN)
        {
            dest->host[i++] = *p;
        }


    /* port */
    if(TRstrcasecmp(dest->scheme, "https") == 0)
    {
#ifdef USE_CWMP_OPENSSL
        dest->port = 443;
#else
        dest->port = 80;
#endif
    }
    else
    {
        dest->port = 80;
    }
    if (*p == ':')
    {
        dest->port = 0;
        for (q = ++p; *q && (*q != '/'); q++)
            if (isdigit(*q))
                dest->port = dest->port * 10 + (*q - '0');
            else
            {
                /* invalid port */
                goto outoff;
            }
        p = q;
    }

nohost:
    /* document */
    if (!*p)
        p = "/";

    if (TRstrcasecmp(dest->scheme, "http") == 0 ||
            TRstrcasecmp(dest->scheme, "https") == 0)
    {
        const char hexnums[] = "0123456789abcdef";
        d = dest->uri;
        while (*p != '\0')
        {
            if (!isspace(*p))
            {
                *d++ = *p++;
            }
            else
            {
                *d++ = '%';
                *d++ = hexnums[((unsigned int)*p) >> 4];
                *d++ = hexnums[((unsigned int)*p) & 0xf];
                p++;
            }
        }
        *d = '\0';
    }
    else
    {
        //strncpy(d, p, MAX_URI_LEN);
    }

    cwmp_log_debug(
        "scheme:   [%s]\n"
        "user:     [%s]\n"
        "password: [%s]\n"
        "host:     [%s]\n"
        "port:     [%d]\n"
        "uri: [%s]\n",
        dest->scheme, dest->auth.username, dest->auth.password,
        dest->host, dest->port, dest->uri);

    return CWMP_OK;

outoff:
    cwmp_log_error("parse url error.");
    return CWMP_ERROR;
}



static int http_split_headers(char *data, unsigned long len, char **line)
{
    int lines = 0;
    unsigned long i;

    //FUNCTION_TRACE();

    line[lines] = data;
    for (i = 0; i < len && lines < MAX_HEADERS; i++)
    {
        if (data[i] == '\r')
            data[i] = '\0';
        if (data[i] == '\n')
        {
            lines++;
            data[i] = '\0';
            if (lines >= MAX_HEADERS)
                return MAX_HEADERS;
            if (i + 1 < len)
            {
                if (data[i + 1] == '\n' || data[i + 1] == '\r')
                    break;
                line[lines] = &data[i + 1];
            }
        }
    }

    i++;
    while (i < len && data[i] == '\n') i++;

    return lines;
}





static void http_parse_headers(http_parser_t * parser, char **line, int lines, pool_t * pool)
{
    int i,l;
    int whitespace, slen;
    char *name = NULL;
    char *value = NULL;

    //FUNCTION_TRACE();

    /* parse the name: value lines. */
    for (l = 1; l < lines; l++)
    {

        whitespace = 0;
        name = line[l];
        value = NULL;
        slen = strlen(line[l]);
        for (i = 0; i < slen; i++)
        {
            if (line[l][i] == ':')
            {
                whitespace = 1;
                line[l][i] = '\0';
            }
            else
            {
                if (whitespace)
                {
                    whitespace = 0;
                    while (i < slen && line[l][i] == ' ')
                        i++;

                    if (i < slen)
                        value = &line[l][i];

                    break;
                }
            }
        }

        if (name != NULL && value != NULL)
        {
            http_set_variable(parser, name, value, pool);
            name = NULL;
            value = NULL;
        }
    }
}

int http_read_line(http_socket_t * sock, char * buffer, int max)
{
    char c;
    int readnum;

    int i=0;
    while (i < max)
    {
        readnum = http_socket_read(sock, &c, 1);

        if (readnum < 0)
        {
            cwmp_log_error("recv, CANNOT READ 1 char");
            if (errno == EAGAIN)
                return CWMP_TIMEOUT;
            return CWMP_ERROR;
        };

        //FIXME
        if (readnum == 0) break;


        buffer[i++]=c;

        if (c=='\r')  // GOT CR
        {
            if ( http_socket_read(sock, &c, 1) < 0 )
            {
                cwmp_log_error("http_read_line ERROR 2");
                if (errno == EAGAIN)
                    return CWMP_TIMEOUT;
                return CWMP_ERROR;
            };

            buffer[i++]=c;
            break ;
        }
        else if (c=='\n')
        {
            break;
        }
    }
    if (i >= max)
    {
        cwmp_log_error("http_read_line ERROR 1");
        return CWMP_ERROR;
    }

    buffer[i] = 0;
    return i;
}

int http_read_header(http_socket_t * sock, cwmp_chunk_t * header, pool_t * pool)
{
    char buffer[1024];
    int rc, bytes;

    cwmp_log_trace("%s(sock=%p, header=%p, pool=%p)",
            __func__, (void*)sock, (void*)header, (void*)pool);

    bytes = 0;
    for (;;)
    {
        rc = http_read_line(sock, buffer, 1023);
        if (rc < 0) return rc;
        if (rc == 0) break;

        buffer[rc] = 0;

        if (buffer[1] == '\0') {
            if (buffer[0] == '\n' || buffer[0] == '\r') {
                break;
            }
        } else {
            if (buffer[0] == '\r' && buffer[1] == '\n' && buffer[2] == '\0') {
                break;
            }
        }

        cwmp_chunk_write_string(header, buffer, rc, pool);
        bytes += rc;
        // if (buffer[0] == '\r' && buffer[1] == '\n')
        //cwmp_log_error("%i %i",buffer[0], buffer[1]);
    }

    cwmp_log_debug("http_read_header READ %i",bytes);
    return bytes;

}





int http_read_body(http_socket_t * sock, int max)//, cwmp_chunk_t * body, pool_t * pool)
{
    int bytes = 0;
    int len;
    char buffer[512];

    while (bytes < max)
    {

        if ( (len = http_socket_read(sock, buffer, 512)) < 0 )
        {
            cwmp_log_error("recv, CANNOT READ 512 chars");
            if (errno == EAGAIN)
                return CWMP_TIMEOUT;
            return CWMP_ERROR;
        }
        if (len <= 0)
        {
            //*body = 0;
            if (len == 0)
            {
                return bytes;
            }
            return -1;
        }

        //memcpy(b, buffer, len);

        if(sock->write_callback)
        {
            (*sock->write_callback)(buffer, 1, len, sock->write_calldata);
        }

        bytes += len;
    }



    return bytes;

}

int http_read_request(http_socket_t * sock, http_request_t * request, pool_t * pool)
{
    int rc;
    cwmp_chunk_t * header;
    char *line[MAX_HEADERS]; /* limited to 64 lines, should be more than enough */

    int lines, len;
    size_t	bytes;
    char *req_type = NULL;
//    char *uri = NULL;
//    char *version = NULL;
    int whitespace, wheres, slen;
    int i;
    http_parser_t * parser;
    char data[2048];

    cwmp_log_debug("http_read_request");

    FUNCTION_TRACE();
    bytes = 0;
    parser = request->parser;
    cwmp_chunk_create(&header, pool);

    rc = http_read_header(sock, header, pool);
    if (rc <= 0)
    {
        if (rc != 0) cwmp_log_error("ERROR: http_read_request ERR %i",rc);
        return rc;
    }



    len = cwmp_chunk_copy(data, header, 2047);
    cwmp_log_debug("http read request: %s", data);
    bytes += len;
    lines = http_split_headers(data, len, line);


    wheres = 0;
    whitespace = 0;
    slen = strlen(line[0]);
    req_type = line[0];


    for (i = 0; i < slen; i++)
    {
        if (line[0][i] == ' ')
        {
            whitespace = 1;
            line[0][i] = '\0';
        }
        else
        {
            // we're just past the whitespace boundry
            if (whitespace)
            {
                whitespace = 0;
                wheres++;
/*                switch (wheres)
                {
                case 1:
                    uri = &line[0][i];
                    break;
                case 2:
                    version = &line[0][i];
                    break;
                }
*/
            }
        }
    }


    if (TRstrcasecmp("GET", req_type) == 0)
    {
        request->method = HTTP_GET;
    }
    else if (TRstrcasecmp("POST", req_type) == 0)
    {
        request->method = HTTP_POST;
    }
    else if (TRstrcasecmp("HEAD", req_type) == 0)
    {
        request->method = HTTP_HEAD;
    }
    else
    {
        request->method = HTTP_UNKNOWN;
    }


    http_parse_headers(parser, line, lines, pool);

    return bytes;

#if 0
    cwmp_chunk_t header;
    cwmp_chunk_t body;
    int rc;

    char *tmp;
    char *line[MAX_HEADERS]; /* limited to 32 lines, should be more than enough */
    int i;
    int lines;
    char *req_type = NULL;
    char *uri = NULL;
    char *version = NULL;
    int whitespace, where, slen;


    rc = http_read_header(sock, &header);
    if (rc <= 0)
    {
        return CWMP_ERROR;
    }

    lines = http_split_headers(data, len, line);

    where = 0;
    whitespace = 0;
    slen = strlen(line[0]);
    req_type = line[0];
    for (i = 0; i < slen; i++)
    {
        if (line[0][i] == ' ')
        {
            whitespace = 1;
            line[0][i] = '\0';
        }
        else
        {
            /* we're just past the whitespace boundry */
            if (whitespace)
            {
                whitespace = 0;
                where++;
                switch (where)
                {
                case 1:
                    uri = &line[0][i];
                    break;
                case 2:
                    version = &line[0][i];
                    break;
                }
            }
        }
    }

    http_parse_headers(request->parser,
#endif

}

#if 0
int http_parse_request(http_request_t * request, char *data, unsigned long len)
{
    char *line[MAX_HEADERS]; /* limited to 32 lines, should be more than enough */
    int i;
    char *req_type = NULL;
    char *uri = NULL;
    char *version = NULL;
    int whitespace, where, slen;

    if (data == NULL)
        return 0;

    return 1;

    /* make a local copy of the data, including 0 terminator */
    //data = (char *)malloc(len+1);
    //if (data == NULL) return 0;
    //memcpy(data, http_data, len);
    //data[len] = 0;


    /* parse the first line special
    ** the format is:
    ** REQ_TYPE URI VERSION
    ** eg:
    ** GET /index.html HTTP/1.0
    */
    where = 0;
    whitespace = 0;
    slen = strlen(line[0]);
    req_type = line[0];
    for (i = 0; i < slen; i++)
    {
        if (line[0][i] == ' ')
        {
            whitespace = 1;
            line[0][i] = '\0';
        }
        else
        {
            // we're just past the whitespace boundry
            if (whitespace)
            {
                whitespace = 0;
                where++;
                switch (where)
                {
                case 1:
                    uri = &line[0][i];
                    break;
                case 2:
                    version = &line[0][i];
                    break;
                }
            }
        }
    }

    int lines;
    lines = http_split_headers(data, len, line);

    if (strcasecmp("GET", req_type) == 0)
    {
        parser->req_type = httpp_req_get;
    }
    else if (strcasecmp("POST", req_type) == 0)
    {
        parser->req_type = httpp_req_post;
    }
    else if (strcasecmp("HEAD", req_type) == 0)
    {
        parser->req_type = httpp_req_head;
    }
    else if (strcasecmp("SOURCE", req_type) == 0)
    {
        parser->req_type = httpp_req_source;
    }
    else if (strcasecmp("PLAY", req_type) == 0)
    {
        parser->req_type = httpp_req_play;
    }
    else if (strcasecmp("STATS", req_type) == 0)
    {
        parser->req_type = httpp_req_stats;
    }
    else
    {
        parser->req_type = httpp_req_unknown;
    }

    if (uri != NULL && strlen(uri) > 0)
    {
        char *query;
        if ((query = strchr(uri, '?')) != NULL)
        {
            http_set_variable(parser, HTTPP_VAR_RAWURI, uri);
            *query = 0;
            query++;
            parse_query(parser, query);
        }

        parser->uri = strdup(uri);
    }
    else
    {
        free(data);
        return 0;
    }

    if ((version != NULL) && ((tmp = strchr(version, '/')) != NULL))
    {
        tmp[0] = '\0';
        if ((strlen(version) > 0) && (strlen(&tmp[1]) > 0))
        {
            http_set_variable(parser, HTTPP_VAR_PROTOCOL, version);
            http_set_variable(parser, HTTPP_VAR_VERSION, &tmp[1]);
        }
        else
        {
            free(data);
            return 0;
        }
    }
    else
    {
        free(data);
        return 0;
    }

    if (parser->req_type != httpp_req_none && parser->req_type != httpp_req_unknown)
    {
        switch (parser->req_type)
        {
        case httpp_req_get:
            http_set_variable(parser, HTTPP_VAR_REQ_TYPE, "GET");
            break;
        case httpp_req_post:
            http_set_variable(parser, HTTPP_VAR_REQ_TYPE, "POST");
            break;
        case httpp_req_head:
            http_set_variable(parser, HTTPP_VAR_REQ_TYPE, "HEAD");
            break;
        case httpp_req_source:
            http_set_variable(parser, HTTPP_VAR_REQ_TYPE, "SOURCE");
            break;
        case httpp_req_play:
            http_set_variable(parser, HTTPP_VAR_REQ_TYPE, "PLAY");
            break;
        case httpp_req_stats:
            http_set_variable(parser, HTTPP_VAR_REQ_TYPE, "STATS");
            break;
        default:
            break;
        }
    }
    else
    {
        free(data);
        return 0;
    }

    if (parser->uri != NULL)
    {
        http_set_variable(parser, HTTPP_VAR_URI, parser->uri);
    }
    else
    {
        free(data);
        return 0;
    }

    parse_headers(parser, line, lines);

    free(data);

    return 1;

}
#endif

int http_read_response(http_socket_t * sock, http_response_t * response, pool_t * pool)
{
    char *line[MAX_HEADERS];
    int lines, slen,i, whitespace=0, where=0,code;
    char *version=NULL, *resp_code=NULL, *message=NULL;

    cwmp_chunk_t *header;
    //cwmp_chunk_t body;
    int rc;
    size_t len;

    char * data;
    char * ctxlen;
    size_t cont_len;

    cwmp_log_trace("%s(sock=%p, response=%p, pool=%p)",
            __func__, (void*)sock, (void*)response, (void*)pool);

    cwmp_chunk_create(&header, pool);
    rc = http_read_header(sock, header, pool);
    if (rc <= 0)
    {
        cwmp_log_error("http_read_response ERROR 1");
        if (rc == CWMP_TIMEOUT)
            return rc;
        return CWMP_ERROR;
    }

    len = cwmp_chunk_length(header);

    data = pool_pcalloc(pool, len + 1);
    cwmp_chunk_copy(data,header,  len);

    data[len] = 0;

    cwmp_log_debug("http read header length: %d, [\n%s\n]", len, data);

    lines = http_split_headers(data, len, line);

    /* In this case, the first line contains:
     * VERSION RESPONSE_CODE MESSAGE, such as HTTP/1.0 200 OK
     */
    slen = strlen(line[0]);
    version = line[0];
    for (i=0; i < slen; i++)
    {
        if (line[0][i] == ' ')
        {
            line[0][i] = 0;
            whitespace = 1;
        }
        else if (whitespace)
        {
            whitespace = 0;
            where++;
            if (where == 1)
                resp_code = &line[0][i];
            else
            {
                message = &line[0][i];
                break;
            }
        }
    }

    if (version == NULL || resp_code == NULL || message == NULL)
    {
        cwmp_log_debug("http_read_response ERROR 2");
        return -2;
    }

    http_set_variable(response->parser, HTTPP_VAR_ERROR_CODE, resp_code, pool);
    code = TRatoi(resp_code);
    response->status = code;
    if (code < 200 || code >= 300)
    {
        http_set_variable(response->parser, HTTPP_VAR_ERROR_MESSAGE, message, pool);
    }

    //http_set_variable(response->parser, HTTPP_VAR_URI, uri);
    http_set_variable(response->parser, HTTPP_VAR_REQ_TYPE, "NONE", pool);

    http_parse_headers(response->parser, line, lines, pool);

    ctxlen = http_get_variable(response->parser, "Content-Length");
    cont_len = 0;
    if (ctxlen)
    {
        cont_len = TRatoi(ctxlen);
    }

    rc = http_read_body(sock, cont_len);//, &body, pool);
    if (rc < 0 || (code != 200 && code != 204))
    {
        cwmp_log_debug("http read response code is (%d)", code);
    }
    cwmp_log_debug("http_read_response OK");
    gettimeofday(&sock->stat.transmission_rx_end, NULL);
    return code;

}

//#define http_set_variable(header, name, value)  http_set_var( &header, name, value)

char * http_method(int method)
{
    switch (method)
    {
    case HTTP_POST:
        return "POST";
    case HTTP_PUT:
        return "PUT";
    default:

        return "GET";

    };

    return "GET";
}




/* calculate H(A1) as per spec */

void http_digest_calc_ha1(
        const char *pszAlg,
        const char *pszUserName,
        const char *pszRealm,
        const char *pszPassword,
        const char *pszNonce,
        const char *pszCNonce,
        char *SessionKey)
{
	bool md5sess = (TRstrcasecmp(pszAlg, "md5-sess") == 0);
    char HA1[HASHLEN] = {};

    cwmp_log_trace("%s(pszAlg=\"%s\", pszUserName=\"%s\", pszRealm=\"%s\", pszPassword=\"%s\", pszNonce=\"%s\", pszCNonce=\"%s\", SessionKey=\"%p\")",
            __func__, pszAlg, pszUserName, pszRealm, pszPassword, pszNonce, pszCNonce, (void*)SessionKey);

	MD5(HA1, pszUserName, ":", pszRealm, ":", pszPassword, NULL);

	if (md5sess && (!pszCNonce || !*pszCNonce)) {
		/* rfc2069: skip cnonce in H(A1) */
		cwmp_log_info("cnonce not given for md5-sess algorithm");
	} else if (md5sess) {
		MD5(HA1, HA1, ":", pszNonce, ":", pszCNonce, NULL);
	}

    convert_to_hex(HA1, SessionKey);
};


int http_check_digest_auth(const char * auth_realm, const char * auth, char * cpeuser, char * cpepwd)
{
	/* server-side call */
    http_digest_auth_t digest = {};

    char response[128] = {};

    cwmp_log_trace("%s(auth_realm=\"%s\", auth=\"%s\", cpeuser=\"%s\", cpepwd=\"%s\")",
            __func__, auth_realm, auth, cpeuser, cpepwd);

    if (!auth)
        return -1;

    http_parse_digest_auth(auth, &digest, NULL);

    if (TRstrcmp(cpeuser, digest.username) != 0) {
        cwmp_log_info("invalid CPE user: %s", digest.username);
        return -1;
    }

    if (TRstrcmp(digest.realm, auth_realm)) {
        cwmp_log_info("invalid CPE realm: %s", digest.realm);
        return -1;
    }

    /* copy ASC response */
    TRstrncpy(response, digest.response, sizeof(response));

    /* calc valid response */
    http_calc_digest_response("GET", digest.username, cpepwd, &digest);

    if (TRstrcasecmp(response, digest.response) == 0) {
		cwmp_log_info("[response: %s] auth on CPE ok", digest.response);
        return 0;
	} else {
        cwmp_log_error("[response: %s, expected: %s] auth on CPE fail",
                response, digest.response);
        return -1;
    }
}

int http_calc_digest_response(const char *method,
		const char * user, const char * pwd,
				http_digest_auth_t *digest)
{
    char ha1hex[HASHHEXLEN+1] = {};
    char ha2[HASHLEN] = {};
	char ha2hex[HASHHEXLEN+1] = {};
    char valid_response[HASHLEN] = {};

	cwmp_log_trace("%s(method=\"%s\", user=\"%s\", pwd=\"%s\", digest=%p)",
            __func__, method, user, pwd, (void*)digest);

    http_digest_calc_ha1("MD5",
			user, digest->realm, pwd, digest->nonce, digest->cnonce, ha1hex);

    MD5(ha2, method, ":", digest->uri, NULL);
	convert_to_hex(ha2, ha2hex);

	if (digest->rfc2617) {
		/* increment nonce-count if client (zero for server-side) */
		if (digest->nc) {
			TRsnprintf(digest->nc_hex, sizeof(digest->nc_hex),
					"%08"PRIxPTR, digest->nc);
			digest->nc++;
		}
		/* RFC 2617 method */
		MD5(valid_response,
				ha1hex, ":",
				digest->nonce, ":",
				digest->nc_hex, ":",
				digest->cnonce, ":",
				digest->qop, ":",
				ha2hex, NULL);
	} else {
		/* simple, RFC 2069 method */
		MD5(valid_response,
				ha1hex, ":",
				digest->nonce, ":",
				ha2hex, NULL);
	}
	convert_to_hex(valid_response, digest->response);

    return CWMP_OK;
}

int http_parse_digest_auth(const char * auth, http_digest_auth_t * digest_auth, const char *back_uri)
{
	/* client-side and server-side call */
    char data[512] = {0};
    char * s ;
    char buffer[128];
    char * end;

    char		user[sizeof(digest_auth->username)] = {};
    char		uri[256] = {};//uri[32768]
    char		nonce[33] = {};
    char		cnonce[33] = {};
    char        realm[128] = {};

    char		qop[16] = {};
    char		nc[16] = {};

    char		response[128] = {};
    char		opaque[128] = {};
	bool		rfc2617 = false;


    cwmp_log_trace("%s(auth=\"%s\", digest_auth=%p, back_uri=\"%s\")",
            __func__, auth, (void*)digest_auth, back_uri);

    if (!auth)
        return CWMP_ERROR;

    for (s =  (char*)auth; isspace(*s); s++);
    strncpy(data, s, 511);
    s = data;
    if (!strncasecmp(s, "Digest", 6)) {
        digest_auth->type = HTTP_DIGEST_AUTH;
        for (s += 6;  isspace(*s); s++);
    } else if (!strncasecmp(s, "Basic", 5)) {
        digest_auth->type = HTTP_BASIC_AUTH;
        for (s += 5;  isspace(*s); s++);
    } else {
        cwmp_log_error("%s: unknown auth string: %s", __func__, auth);
        return -1;
    }

    end = s + strlen(s);
    memset(buffer, 0, 128);
    while (s<end)
    {
        if (!strncmp(s, "username=", 9))
            http_parse_key_value(&s, user, sizeof(user), 9);
        else if (!strncmp(s, "realm=", 6))
            http_parse_key_value(&s, realm, sizeof(realm), 6);
        else if (! strncmp(s, "nonce=", 6))
            http_parse_key_value(&s, nonce, sizeof(nonce), 6);
        else if (! strncmp(s, "response=", 9))
            http_parse_key_value(&s, response, sizeof(response), 9);
        else if (! strncmp(s, "uri=", 4))
            http_parse_key_value(&s, uri, sizeof(uri), 4);
        else if (! strncmp(s, "qop=", 4)) {
			rfc2617 = true;
            http_parse_key_value(&s, qop, sizeof(qop), 4);
		} else if (! strncmp(s, "cnonce=", 7)) {
            http_parse_key_value(&s, cnonce, sizeof(cnonce), 7);
		} else if (! strncmp(s, "nc=", 3))
            http_parse_key_value(&s, nc, sizeof(nc), 3);
        else if (! strncmp(s, "domain=", 7))
            http_parse_key_value(&s, uri, sizeof(uri), 7);
		else if (! strncmp(s, "opaque=", 7))
			http_parse_key_value(&s, opaque, sizeof(opaque), 7);
        s ++;
    }

	if (!*cnonce) {
		string_randomize(cnonce, sizeof(cnonce) - 1);
	}

	digest_auth->rfc2617 = rfc2617;
    TRstrncpy(digest_auth->realm, realm, MIN_DEFAULT_LEN);
    TRstrncpy(digest_auth->nonce, nonce, MIN_DEFAULT_LEN);
	if (!*uri && back_uri) {
		*digest_auth->uri = '\0';
		TRstrncpy(digest_auth->uri, back_uri, MIN_DEFAULT_LEN * 4);
	} else
		TRstrncpy(digest_auth->uri, uri, MIN_DEFAULT_LEN*4);
    TRstrncpy(digest_auth->cnonce, cnonce, MIN_DEFAULT_LEN);
    TRstrncpy(digest_auth->qop, "auth", MIN_DEFAULT_LEN);
	if (!*nc) {
		digest_auth->nc = 1;
	} else {
		TRstrncpy(digest_auth->nc_hex, nc, MIN_DEFAULT_LEN);
        /* server-side flag: use only nc_hex value */
		digest_auth->nc = 0;
	}
    if (*response) {
        TRstrncpy(digest_auth->response, response, sizeof(digest_auth->response));
    }
	TRstrncpy(digest_auth->opaque, opaque, MIN_DEFAULT_LEN);

    if (*user) {
        TRstrncpy(digest_auth->username, user, sizeof(user));
    }

    cwmp_log_debug("user[%s], realm[%s], "
			"nonce[%s], response[%s], uri[%s], "
			"qop[%s], cnonce[%s], nc[%s:%"PRIuPTR"], opaque[%s]\n",
                  digest_auth->username,
				  digest_auth->realm,
				  digest_auth->nonce,
				  digest_auth->response,
				  digest_auth->uri,
				  digest_auth->qop,
				  digest_auth->cnonce,
				  digest_auth->nc_hex,
				  digest_auth->nc,
				  digest_auth->opaque
				  );

    return CWMP_OK;
}






int http_write_request(http_socket_t * sock, http_request_t * request, cwmp_chunk_t * chunk, size_t additional_len, pool_t * pool)
{
    char buffer[HTTP_DEFAULT_LEN+1];
    char * data;

	size_t len1 = 0u;
	size_t len2 = 0u;

    cwmp_log_trace("%s(sock=%p, request=%p, chunk=%p, additional_len=%"PRIuPTR", pool=%p)",
            __func__,
            (void*)sock, (void*)request, (void*)chunk, additional_len, (void*)pool);

    const char * header_fmt =
        "%s %s HTTP/1.1\r\n"
        "Host: %s:%d\r\n"
        "User-Agent: %s\r\n"
        "Accept: */*\r\n"
        "Content-Type: text/xml; charset=utf-8\r\n"
        ;

    http_dest_t * dest = request->dest;

    len2 = cwmp_chunk_length(chunk);

	/* formatting header */
    len1 = TRsnprintf(buffer, HTTP_DEFAULT_LEN, header_fmt,
                    http_method(request->method),
                    dest->uri,
                    dest->host,
                    dest->port,
                    "CPE Netcwmp Agent");

    if (len2 || additional_len) {
        len1 += snprintf(buffer + len1, sizeof(buffer) - len1,
                "Content-Length: %d\r\n", len2 + additional_len);
    } else {
        len1 += snprintf(buffer + len1, sizeof(buffer) - len1,
                "Content-Length: 0\r\n");
    }

	if(dest->auth.type == HTTP_DIGEST_AUTH)
	{
		http_calc_digest_response(http_method(request->method),
				dest->auth.username, dest->auth.password, &dest->auth);

		/* formatting authorization string */
		len1 += TRsnprintf(buffer + len1, sizeof(buffer) - len1,
				"Authorization: Digest "
				"username=\"%s\", realm=\"%s\", nonce=\"%s\", "
				"uri=\"%s\", response=\"%s\"",
				dest->auth.username, dest->auth.realm, dest->auth.nonce,
				dest->auth.uri, dest->auth.response
				);

		if (dest->auth.rfc2617) {
			len1 += TRsnprintf(buffer + len1, sizeof(buffer) - len1,
					", qop=%s, nc=%s, cnonce=\"%s\"",
					dest->auth.qop, dest->auth.nc_hex, dest->auth.cnonce);
		}

		if (dest->auth.opaque[0]) {
			len1 += TRsnprintf(buffer + len1, sizeof(buffer) - len1,
					", opaque=\"%s\"",
					dest->auth.opaque);
		}
		len1 += TRsnprintf(buffer + len1, sizeof(buffer) - len1, "\r\n");
	} else if (dest->auth.type == HTTP_BASIC_AUTH)
    {
        char basic[1024] = {};
        char *basic_64 = NULL;
        snprintf(basic, sizeof(basic), "%s:%s", dest->auth.username, dest->auth.password);
        if ((basic_64 = cwmp_base64_encode(basic)) != NULL) {
            len1 += snprintf(buffer + len1, sizeof(buffer) - len1,
                    "Authorization: Basic %s\r\n", basic_64);
            free(basic_64);
        }
    }

    if(dest->cookie[0] != '\0')
    {

        len1 += TRsnprintf(buffer + len1, sizeof(buffer) - len1, "Cookie: %s\r\n",
                    dest->cookie);
    }

    if (request->method == HTTP_PUT) {
        len1 += snprintf(buffer + len1, sizeof(buffer) - len1,
                "Expect: 100-continue\r\n");
    }

    len1 += TRsnprintf(buffer + len1, sizeof(buffer) - len1, "\r\n");

    if(len2 > 0)
    {
        data = (char *)pool_palloc(pool, len1 + len2 + 1);
        TRstrncpy(data, buffer, len1);
        cwmp_chunk_copy(data+len1, chunk, len2);
    }
    else
    {
        data = buffer;
    }
    gettimeofday(&sock->stat.request, NULL);
    return http_socket_write(sock, data, (int)len1 + len2);
}

int http_get(http_socket_t * sock, http_request_t * request, cwmp_chunk_t * data, pool_t * pool)
{
    request->method = HTTP_GET;


    return http_write_request(sock, request, data, 0u, pool);

}

int http_post(http_socket_t * sock, http_request_t * request, cwmp_chunk_t * data, pool_t * pool)
{
    request->method = HTTP_POST;


    return http_write_request(sock, request, data, 0u, pool);

}

size_t http_send_file_callback(char *data, size_t size, size_t nmemb, void * calldata)
{
	FILE * tf = (FILE*) calldata;

	return  fread(data, size, nmemb, tf);
}


size_t http_receive_file_callback(char *data, size_t size, size_t nmemb, void * calldata)
{
	FILE * tf = (FILE*) calldata;

	return  fwrite(data, size, nmemb, tf);
}

static size_t http_send_diagnostics_data(http_socket_t *sock, size_t size)
{
    /* magic string for minimize compression effect */
    const char source[] =
        "\x92\xcf\xce\xb3"
        "\x9d\x57\xd9\x14"
        "\xed\x8b\x14\xd0"
        "\xe3\x76\x43\xde"
        "\x07\x97\xae\x56";
    const size_t len = sizeof(source) - 1;
    ssize_t r = 0;

    register size_t i = 0u;
    register size_t si = 0u;

    while (i < size) {
        if ((si = (i + len)) > size) {
            si = si % size;
            if ((r = write(sock->sockdes, source, si)) != -1) {
                i += r;
            }
            break;
        } else {
            if ((r = write(sock->sockdes, source, len)) != -1) {
                i += r;
            } else {
                break;
            }
        }
    }
    return i;
}

int http_send_diagnostics(size_t size, const char *tourl, struct http_statistics *hs)
{
    int rc = 0;
    pool_t * pool = NULL;
    http_dest_t * dest = NULL;
    http_socket_t * sock = NULL;
    http_request_t * request = NULL;
    http_response_t * response = NULL;

    cwmp_log_trace("%s(size=%"PRIuPTR", tourl=\"%s\", hs=%p)",
            __func__, size, tourl, (void*)hs);

    /* prepare */
    pool = pool_create(POOL_DEFAULT_SIZE);
    http_dest_create(&dest, tourl, pool);
    rc = http_socket_create(&sock, AF_INET, SOCK_STREAM, 0, pool);
    if (rc != CWMP_OK)
    {
        cwmp_log_error("%s(): create socket failed", __func__);
        goto end;
    }

    rc = http_socket_connect(sock, dest->host, dest->port);
    if (rc != CWMP_OK)
    {
        cwmp_log_error("%s(): connect to %s:%d failed",
                __func__, dest->host, dest->port);
        goto end;
    }

    http_socket_set_recvtimeout(sock, 30);

    http_request_create(&request, pool);

    request->dest = dest;
    request->method = HTTP_PUT;

    http_response_create(&response, pool);
    /* send */
    http_write_request(sock, request, NULL, size, pool);

    rc = http_read_response(sock, response, pool);
    if (rc == 401) {
        /* re-request */
        char *auth = http_get_variable(response->parser, "WWW-Authenticate");
        if (!auth) {
            goto end;
        }
        http_parse_digest_auth(auth, &request->dest->auth, request->dest->uri);
        http_write_request(sock, request, NULL, size, pool);
        rc = http_read_response(sock, response, pool);
    }

    if (rc != HTTP_100) {
        goto end;
    }

    gettimeofday(&sock->stat.transmission_tx, NULL);
    http_send_diagnostics_data(sock, size);
    rc = http_read_response(sock, response, pool);
    gettimeofday(&sock->stat.transmission_tx_end, NULL);
    if (hs) {
        memcpy(hs, &sock->stat, sizeof(*hs));
    }
end:
    pool_destroy(pool);
    if (rc != HTTP_200)
        if (rc == CWMP_TIMEOUT)
            return CWMP_TIMEOUT;
        return CWMP_ERROR;
    return CWMP_OK;
}

int http_send_file_request(http_socket_t * sock , http_request_t * request, const char  * fromfile, pool_t * pool)
{
    char buffer[HTTP_DEFAULT_LEN+1];
//    char * data;
    size_t len1, len2, totallen;

    FUNCTION_TRACE();


    const char * header_fmt =
        "%s %s HTTP/1.1\r\n"
        "Authorization: Basic ZnRwdXNlcjpmdHB1c2Vy\r\n"
        "Host: %s:%d\r\n"
        // "User-Agent: %s\r\n"
        "Accept: */*\r\n"
        // "Content-Type: multipart/form-data\r\n"
	// "Connection: Keep-Alive\r\n"
        "Content-Length: %lu\r\n"
        "Expect: 100-continue\r\n"
	"\r\n"
        ;
//    const char * auth_fmt = "Authorization: Digest username=\"%s\", realm=\"%s\", nonce=\"%s\", uri=\"%s\", response=\"%s\"\r\n";
    //qop=%s, nc=%s, cnonce=\"%s\"

    http_dest_t * dest = request->dest;

/*    struct stat buf;
	if(stat(fromfile, &buf)<0)
	{
		len2 = 0;
	}
	else
	{
		 len2 = buf.st_size;
	}
*/
    FILE *tf = fopen(fromfile, "rb");

    if(!tf) {
        cwmp_log_error("http_send_file_request(): unable to open filename %s", fromfile);
        return CWMP_ERROR;
    }

    fseek(tf,0L,SEEK_END);
    len2 = ftell(tf);
    fseek(tf,0L,SEEK_SET);
    cwmp_log_info("http_send_file_request FILE LEN %lu",len2);


    len1 = TRsnprintf(buffer, HTTP_DEFAULT_LEN, header_fmt,
                    http_method(request->method),
                    dest->uri,
                    dest->host,
                    dest->port,
//                    "CPE Netcwmp Agent",
                    len2);

//    len1 += TRsnprintf(buffer + len1, HTTP_DEFAULT_LEN - len1, "\r\n");

    cwmp_log_debug("SEND: %d[\n%s\n]", len1, buffer);

    http_socket_write(sock, buffer, (int)len1);

   http_response_t * response;
   http_response_create(&response, pool);

    int rc = http_read_response(sock, response, pool);
    if(rc != HTTP_100)
    {
        cwmp_log_error("ERROR: http_send_file_request response code: %i",rc);
	if(tf != NULL)
	{
		fclose(tf);
	}
        if (rc == CWMP_TIMEOUT) {
            return CWMP_TIMEOUT;
        }
	return CWMP_ERROR;
    }


    totallen = len1;

    while(1)
    {
	len2 = fread(buffer, 1, HTTP_DEFAULT_LEN, tf);
	cwmp_log_debug("http_send_file_request TO SEND %lu",len2);
	if(len2 <= 0)
	{
		break;
	}
	buffer[len2] = '\0';
	len2 = http_socket_write(sock, buffer, (int)len2);
	cwmp_log_debug("http_send_file_request SENT %lu, ERRNO %i",len2, errno);
	if(len2 <= 0)
	{
		break;
	}
	totallen += len2;
    }

    if(tf != NULL)
    {
	fclose(tf);
    }


    cwmp_log_debug("INFO: http_send_file_request OK (len: %i)",totallen);
    return totallen;
}


int http_send_file(const char * fromfile, const char *tourl )
{
	pool_t * pool;
	http_dest_t *  dest;
	http_socket_t * sock;
	http_request_t * request;

	http_response_t * response;

        cwmp_log_info("INFO: http_send_file: from %s to %s",fromfile, tourl);


	pool = pool_create(POOL_DEFAULT_SIZE);
	http_dest_create(&dest, tourl, pool);

        int rc = http_socket_create(&sock, AF_INET, SOCK_STREAM, 0, pool);
        if (rc != CWMP_OK)
        {
            cwmp_log_error("http send file: create socket error.");
            goto out;
        }

	int one = 1;
	//FIXME: find a proper way to wait sock after write instead of TCP_NODELAY
	setsockopt (sock->sockdes, IPPROTO_TCP, TCP_NODELAY, (void *)&one, sizeof(one));

        rc = http_socket_connect(sock, dest->host, dest->port);
        if(rc != CWMP_OK)
        {
            cwmp_log_error("connect to host faild. Host is %s:%d.", dest->host, dest->port);
            goto out;
        }

        http_socket_set_recvtimeout(sock, 30);

	http_request_create(&request, pool);
	request->dest = dest;
        request->method = HTTP_PUT;

	rc = http_send_file_request(sock, request, fromfile, pool);
        if(rc <= 0)
        {
            cwmp_log_error("http get host faild. Host is %s:%d.", dest->host, dest->port);
            goto out;
        }

	sleep(1);
        http_response_create(&response, pool);
	rc = http_read_response(sock, response, pool);

out:

	close(sock->sockdes);//FIXME: check result

	pool_destroy(pool);

	if(rc != HTTP_200)
	{
    	        cwmp_log_error("http_send_file response code %i",rc);
        if (rc == CWMP_TIMEOUT)
            return CWMP_TIMEOUT;
		return CWMP_ERROR;
	}
	else
	{
    	        cwmp_log_debug("http_send_file OK");
		return CWMP_OK;
	}

}

int http_receive_file(const char *fromurl, const char * tofile, struct http_statistics *hs)
{

    pool_t * pool;
    http_dest_t *  dest;
    http_socket_t * sock;
    http_request_t * request;

    http_response_t * response;

    FILE * tf = NULL;

    cwmp_log_trace("%s(formurl=\"%s\", tofile=\"%s\")",
            __func__, fromurl, tofile);

    pool = pool_create(POOL_DEFAULT_SIZE);
    http_dest_create(&dest, fromurl, pool);

    int rc = http_socket_create(&sock, AF_INET, SOCK_STREAM, 0, pool);
    if (rc != CWMP_OK) {
        cwmp_log_error("http receive file: create socket error.");
        goto out;
    }

    rc = http_socket_connect(sock, dest->host, dest->port);
    if(rc != CWMP_OK) {
        cwmp_log_error("connect to host faild. Host is %s:%d.",
                dest->host, dest->port);
        goto out;
    }

    if (tofile) {
        tf = fopen(tofile, "wb+");
        if(!tf) {
            cwmp_log_error("Unable to create target file: %s", tofile);
            goto out;
        }
        http_socket_set_writefunction(sock, http_receive_file_callback, tf);
    }

    http_socket_set_recvtimeout(sock, 30);

    http_request_create(&request, pool);
    request->dest = dest;
    rc = http_get(sock, request, NULL, pool);
    if(rc <= 0) {
        cwmp_log_error("http_get failed. Host is %s:%d.", dest->host, dest->port);
        goto out;
    }


    http_response_create(&response, pool);

    rc = http_read_response(sock, response, pool);
    if (hs) {
        memcpy(hs, &sock->stat, sizeof(*hs));
    }
out:
    if(tf) {
        fclose(tf);
    }
    pool_destroy(pool);

    return rc;
}



