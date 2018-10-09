/*
 * Copyright (C) 2010-2012 Alibaba Group Holding Limited
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

#include "util.h"
#include "asp_parser.h"
#include "sessionid.h"
#include "firmware.h"


typedef struct {
    int is_post_form;
    int disable_auth;
    ngx_hash_t                          types;
    ngx_array_t                        *types_keys;
    ngx_http_complex_value_t           *variable;
} ngx_http_wive_loc_conf_t;


static char *ngx_http_post_form_init(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static char *ngx_http_disable_auth_init(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static char *ngx_http_wive(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static void *ngx_http_wive_create_loc_conf(ngx_conf_t *cf);
static char *ngx_http_wive_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child);
static ngx_int_t ngx_http_wive_init(ngx_conf_t *cf);

static ngx_command_t  ngx_http_wive_commands[] = {

    { ngx_string("asp_post_form"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,
      ngx_http_post_form_init,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },

    { ngx_string("disable_auth"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,
      ngx_http_disable_auth_init,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },


    { ngx_string("wive"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_http_wive,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },

    { ngx_string("wive_types"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_1MORE,
      ngx_http_types_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_wive_loc_conf_t, types_keys),
      &ngx_http_html_default_types[0] },

      ngx_null_command
};


static ngx_http_module_t  ngx_http_wive_module_ctx = {
    NULL,                               /* proconfiguration */
    ngx_http_wive_init,        /* postconfiguration */

    NULL,                               /* create main configuration */
    NULL,                               /* init main configuration */

    NULL,                               /* create server configuration */
    NULL,                               /* merge server configuration */

    ngx_http_wive_create_loc_conf,    /* create location configuration */
    ngx_http_wive_merge_loc_conf      /* merge location configuration */
};


ngx_module_t  ngx_http_wive_module = {
    NGX_MODULE_V1,
    &ngx_http_wive_module_ctx, /* module context */
    ngx_http_wive_commands,    /* module directives */
    NGX_HTTP_MODULE,                    /* module type */
    NULL,                               /* init master */
    NULL,                               /* init module */
    NULL,                               /* init process */
    NULL,                               /* init thread */
    NULL,                               /* exit thread */
    NULL,                               /* exit process */
    NULL,                               /* exit master */
    NGX_MODULE_V1_PADDING
};

//DEPRECATED FUNCS
//static ngx_http_output_header_filter_pt ngx_http_next_header_filter;
//static ngx_http_output_body_filter_pt   ngx_http_next_body_filter;

static ngx_int_t ngx_http_asp_handler(ngx_http_request_t *r);
static ngx_int_t ngx_http_asp_form_handler(ngx_http_request_t *r);
static void ngx_http_mymodule_body_handler ( ngx_http_request_t *r );
ngx_int_t  ngx_http_mymodule_phase_handler ( ngx_http_request_t *r );



ngx_int_t  ngx_http_mymodule_phase_handler ( ngx_http_request_t *r ) 
{
    ngx_int_t rc = NGX_OK;

    if(r->request_body == NULL) { 
     // set the handler before POST request received
        rc = ngx_http_read_client_request_body(r,  ngx_http_mymodule_body_handler);

        if (rc >= NGX_HTTP_SPECIAL_RESPONSE) {
            return rc;
        }

        return NGX_DONE;
    }

    // Still receiving body?
     if(r->request_body->rest) {
        return NGX_DONE;
    }

    return rc;
}

static char* split_http_val(char* str, char separator)
{
    char* ptr = strchr(str, separator);
    if (ptr == NULL) return NULL;
    *ptr = '\0';
    return ptr+1;
}

static int http_arg_decode(char* arg, int bufsize)
{
    char* out = (char*) calloc(bufsize, sizeof(char));
    char* out_ptr = out;
    char* in_ptr;
    char hex_str[3] = {0};

    char* hex_alpha = "0123456789ABCDEFabcdef";

    int state = 0;

    if (out == NULL) return 1;

    for (in_ptr = arg; *in_ptr != '\0'; in_ptr++)
    {
        char c = *in_ptr;

        switch (state) 
        {
        case 0:
            if (c == '%')
            {
                state = 1;
            }

            if (c == '+') c = ' ';

            *out_ptr = c;
            out_ptr++;
        break;

        case 1:
            if (strchr(hex_alpha, c) != NULL)
            {
                hex_str[0] = c;
                *out_ptr = c;
                out_ptr++;
                state = 2;
            }
            else
            {
                *out_ptr = c;
                state = 0;
            }
        break;

        case 2:
            if (strchr(hex_alpha, c) != NULL)
            {
                hex_str[1] = c;
                int number = (int)strtol(hex_str, NULL, 16);

                out_ptr -= 2;
                out_ptr[0] = (char)number;
                out_ptr[1] = '\0';
                out_ptr++;
                state = 0;
            }
            else
            {
                *out_ptr = c;
                out_ptr++;
                state = 0;
            }
        break;
        }
    }

    memcpy(arg, out, bufsize);
    free(out);

    return 0;
}

#define MULTIPART_BOUNDARY_PREFIX "multipart/form-data; boundary="

static int 
parse_multipart_args(ngx_http_request_t *r, ngx_str_t headers, ngx_str_t body, ngx_array_t* http_args)
{
    unsigned int i;
    char* line = (char*)headers.data;
    char* next_line = NULL;

    char* url = ngx_to_cstring(r->pool, r->uri);
    ELOG_DEBUG(r->connection->log, 0, "ASP FORM BODY SIZE %i \n", body.len);

// parse multipart-form part headers
    while (line != NULL)
    {
        next_line = cstring_split_inplace(line, "\r\n");

        ELOG_DEBUG(r->connection->log, 0, "multipart header line: |%.100s|\n", line);
        ngx_array_t* arr = cstring_split(r->pool, line, "; ");

        if (arr == NULL || arr->nelts < 1) // something went wrong
        {
            line = next_line;
            continue;
        }

        char** args = (char**)arr->elts;
        ELOG_DEBUG(r->connection->log, 0, "multipart args arr nelts: %i\n", arr->nelts);

        for (i=0;i<arr->nelts;i++)
        {
            if (strstr(args[i],"name=\"") == args[i])
            {
                char* varname = cstring_copy(r->pool, args[i]+6);
                varname = cstring_trim_right(r->pool, varname, '"');

                keyval_t* keyval = ngx_array_push(http_args);
                keyval->key = varname;
                keyval->val = (char*)body.data;
                ELOG_DEBUG(r->connection->log, 0, "multipart arg %i: %s\n", i, varname);

                break;
            }
        }
/*
        if (strcmp(args[0], "Content-Disposition: form-data"  ) == 0)
        {
            if (strcmp(args[1], "name=\"filename\"") == 0 && strcmp(url, "/cgi-bin/upload.cgi") == 0)
            {
                FILE *f=fopen("/tmp/download.bin","wb");
                fwrite((char*)body.data, 1, body.len, f);
                fclose(f);
            }
        }
*/

/*
        for (i=0;i<arr->nelts;i++)
        {
            ELOG_DEBUG(r->connection->log, 0, "IIII header line arg[%i]: |%.100s|\n", i, ((char**)arr->elts)[i]);
        }
*/
        line = next_line;
    }

    return 0;
}

static int parse_multipart(ngx_http_request_t *r, char* body, unsigned int body_len, ngx_array_t* http_args)
{
    ELOG_DEBUG(r->connection->log, 0, "ASP FORM PARSE MULTIPART start \n");

    ngx_str_t body_str;
    ngx_str_t sep_str;

    ngx_str_t type_prefix = ngx_string(MULTIPART_BOUNDARY_PREFIX);

    body_str.data = (unsigned char*)body;
    body_str.len = body_len;

    if (r->headers_in.content_type == NULL)
    {
        return 1;
    }

    ngx_str_t content_type = r->headers_in.content_type->value;

//ASP FORM SEP multipart/form-data; boundary=----WebKitFormBoundarygDuHoeTV4B2TUAEV

    if (n_strstr(content_type, type_prefix).data == NULL)
    {
        return 1;
    }

    ELOG_DEBUG(r->connection->log, 0, "ASP FORM FULLBODY %.100s \n", body);

//    char* sep_uniq = ngx_to_cstring(r->pool, n_skipchar(content_type, type_prefix.len));
    ngx_str_t sep_uniq = n_skipchar(content_type, type_prefix.len);

//    char* separator = ngx_pcalloc(r->pool, strlen(sep_uniq)+1+6);
    char* separator = ngx_pcalloc(r->pool, sep_uniq.len+1+6);

    sep_str.data = (unsigned char*)separator;
    sprintf(separator,"--%.*s", (int)sep_uniq.len, sep_uniq.data);
    sep_str.len = sep_uniq.len+2; //strlen(separator);

    if (n_strstr(body_str, sep_str).data == body_str.data)
    {
        body_str = n_skipchar(body_str, sep_str.len);
//        body += strlen(separator);
    }

    sprintf(separator,"\r\n--%.*s", (int)sep_uniq.len, sep_uniq.data);
    sep_str.len = sep_uniq.len+4; //strlen(separator);

    ELOG_DEBUG(r->connection->log, 0, "ASP FORM SEP %s\n", separator);

    ngx_str_t token;
    token.data = (unsigned char*)body;
    token.len = body_len;

    ngx_str_t next_token;
    next_token.data = NULL;
    next_token.len = 0;

    while (token.data != NULL)
    {
        ELOG_DEBUG(r->connection->log, 0, "NEXT_TOKEN: %s\n", token.data);

        next_token = ngx_split_inplace(token, sep_str);
        ELOG_DEBUG(r->connection->log, 0, "TOKEN A");

        if (token.len >= 2)
        {
            if (token.data[0] == '\r' && token.data[1] == '\n')
            {
                token = n_skipchar(token, 2);
            }
            else
            if (token.data[0] == '-' && token.data[1] == '-' && token.len >= 4) // --\r\n
            {
                token = n_skipchar(token, 4);
            }
        }


        ngx_str_t empty_line = ngx_string("\r\n\r\n");
        ELOG_DEBUG(r->connection->log, 0, "TOKEN 1: %s\n", token.data);
        ngx_str_t headers_end = n_strstr(token, empty_line);
        ELOG_DEBUG(r->connection->log, 0, "TOKEN 1B");

// headers: |Content-Disposition: form-data; name="datafile"; filename="wive.bin"
// Content-Type: application/octet-stream|

        if (headers_end.data != NULL)
        {
            headers_end.data[0] = '\0';

            ngx_str_t body_data = n_skipchar(headers_end, 4);

            ELOG_DEBUG(r->connection->log, 0, "parse_multipart headers: |%.100s|\n", token.data);
            ELOG_DEBUG(r->connection->log, 0, "parse_multipart body: |%.100s|\n", body_data.data);

            int len = 0;
            if (next_token.data != NULL)
            {
                len = next_token.data-body_data.data-sep_str.len;
            }
            else
            {
                len = body_str.len-(body_data.data-body_str.data);
            }

            if (len >= 0)
            {
                body_data.len = len;
            }

            ELOG_DEBUG(r->connection->log, 0, "PARSE_MULTIPART_ARGS started\n");
            parse_multipart_args(r, token, body_data, http_args);
            ELOG_DEBUG(r->connection->log, 0, "PARSE_MULTIPART_ARGS ended\n");
        }

        token = next_token;
    }

    ELOG_DEBUG(r->connection->log, 0, "ASP FORM PARSE MULTIPART end \n");
    return 0;

}

static void parse_http_args2(ngx_http_request_t *r, char* line, ngx_array_t* http_args)
{
    FUNCTION_TRACE(r->connection->log);
    unsigned int i;
    ngx_array_t* args;

    args = cstring_split(r->pool, line, "&");
    if (args == NULL)
    {
        return;
    }

    char** elts_ptr = (char**)args->elts;

    for (i=0;i<args->nelts;i++)
    {
        char* arg = elts_ptr[i];
        char* arg_val = cstring_split_inplace(arg, "=");
        http_arg_decode(arg, strlen(arg)+1);

        if (arg_val != NULL)
        {
            http_arg_decode(arg_val, strlen(arg_val)+1);
//            ELOG_DEBUG(r->connection->log, 0, "ASP FORM PARSE HTTP ARG %s = %s \n", arg, arg_val);
        }
        else
        {
//            ELOG_DEBUG(r->connection->log, 0, "ASP FORM PARSE HTTP ARG %s = NULL \n", arg);
        }

        keyval_t* keyval = ngx_array_push(http_args);
        keyval->key = arg;
        keyval->val = arg_val;
    }


    ELOG_DEBUG(r->connection->log, 0, "ASP FORM PARSE HTTP ARGS 2 end \n");
}

// DEPRECATED http args parser
/*
static void parse_http_args(ngx_http_request_t *r, char* line, ngx_array_t* http_args)
{
    char* ptr = line;
    char* arg_ptr = line;
    ELOG_DEBUG(r->connection->log, 0, "ASP FORM PARSE HTTP ARGS 1 start \n");
*/
/*
    ngx_hash_t       * args_hashtable;
    ngx_hash_keys_arrays_t  args_hashkeys;
    ngx_hash_init_t  hash;

    args_hashtable = (ngx_hash_t*) ngx_pcalloc(r->pool, sizeof(ngx_hash_t));

    hash.hash = args_hashtable;
    hash.key = ngx_hash_key;
    hash.max_size = 512;
    hash.bucket_size = ngx_align(64, ngx_cacheline_size);
    hash.name = "args_hashkeys";
    hash.pool = r->pool;
    hash.temp_pool = NULL;

    args_hashkeys.pool = r->pool;
    args_hashkeys.temp_pool = NULL;
    
    ngx_hash_keys_array_init(&args_hashkeys, NGX_HASH_SMALL);
*/

/*
    while (1)
    {
        char c = ptr[0];
        if (c == '&' || c == '\0')
        {
            *ptr = '\0';

//            printf("arg: %s \n", arg_ptr);
            char* arg_name = cstring_copy(r->pool, arg_ptr);
            char* arg_val = NULL;
            if (arg_name)
            {
//                arg_val = split_http_val(arg_name, '=');
                arg_val = cstring_split_inplace(arg_name, "=");

                http_arg_decode(arg_name, strlen(arg_name)+1);

                if (arg_val != NULL)
                {
                    http_arg_decode(arg_val, strlen(arg_val)+1);
                    ptr = arg_val + strlen(arg_val);
                    ELOG_DEBUG(r->connection->log, 0, "ASP FORM PARSE HTTP ARG %s = %s \n", arg_name, arg_val);

                }
                else
                {
                    ELOG_DEBUG(r->connection->log, 0, "ASP FORM PARSE HTTP ARG %s = NULL \n", arg_name);
                }

//                ngx_str_t key = cstring_to_ngx(r->pool, arg_name);
//                ngx_hash_add_key(&args_hashkeys, &key, (void*)arg_val, 0);//NGX_HASH_READONLY_KEY);

                keyval_t* keyval = ngx_array_push(http_args);
                keyval->key = arg_name;
                keyval->val = arg_val;
            }

            arg_ptr = ptr + 1;
            if (c == '\0') break;
        }
        ptr++;
    }
*/
//TODO: hash tables
/*    if (ngx_hash_init(&hash, args_hashkeys.keys.elts, args_hashkeys.keys.nelts) != NGX_OK)
    {
        ELOG_CRIT(r->connection->log, 0, "Unable to init http arguments hashtable!");
        return NULL;
    }

    return args_hashtable;*/
//    ELOG_DEBUG(r->connection->log, 0, "ASP FORM PARSE HTTP ARGS 1 end \n");
//}

static int check_auth(webs_t* wp)
{
    ngx_http_wive_loc_conf_t  *lcf;
    lcf = ngx_http_get_module_loc_conf(wp->request, ngx_http_wive_module);
    if (lcf != NULL && lcf->disable_auth == 1)
    {
        wp->auth_session = (auth_session_t*)ngx_pcalloc(wp->pool, sizeof(struct auth_session_t));
        //wp->auth_session->username = "noauth";
        strcpy(wp->auth_session->username, "noauth");
        wp->auth_session->role = USER;
        return 0; // skip authorization
    }

    char* sessionid = getCookie(wp->request, "sessionid");

    if (!sessionid)
    {
        ELOG_DEBUG(wp->log, 0, "??? No sessionid \n");

        goto unauthorized;
    }

    ELOG_DEBUG(wp->log, 0, "??? my id: %s \n", sessionid);
    unsigned int i;

    char* desired_addr = ngx_to_cstring(wp->pool, wp->request->connection->addr_text);
    ELOG_DEBUG(wp->log, 0, "??? Desired address: %s \n", desired_addr);

    ELOG_DEBUG(wp->log, 0, "??? Session count: %d \n", auth_sessions->nelts);
    struct auth_session_t* session = getSessionById(sessionid);

    if (session == NULL)
    {
        ELOG_DEBUG(wp->log, 0, "??? Session is NULL! \n");
        goto unauthorized;
    }

    if (strcmp(session->sessionid, sessionid) == 0 && session->role != DENY)
    {
        ELOG_DEBUG(wp->log, 0, "??? sessionid equals! \n");
        if (time(NULL) < (session->start_time + AUTH_SESSION_MAX_LENGTH))
        {
            wp->auth_session = session;
            ELOG_DEBUG(wp->log, 0, "??? Got Session %d! \n", wp->auth_session->role);
            return 0;
        }
    }

unauthorized:
    wp->auth_session = NULL;
    ELOG_DEBUG(wp->log, 0, "??? Unauthorized! \n");
    return 1;
}


static void ngx_http_mymodule_body_handler ( ngx_http_request_t *r ) 
{
    ngx_chain_t                 *cl;
    ngx_chain_t                out_chain;
    u_char                    *formName, *ptr;
    ngx_buf_t                 *b;

    unsigned char* url = ngx_pcalloc(r->pool, r->uri.len+1);
    if (url == NULL) return;// NGX_HTTP_INTERNAL_SERVER_ERROR;

    memcpy(url, r->uri.data, r->uri.len);
    if (url[0] == '\0') return;// NGX_HTTP_INTERNAL_SERVER_ERROR;

    formName = url;

    for(ptr=url; *ptr != '\0'; ptr++)
    {
        if (*ptr == '/') formName = ptr;
    }

    formName++;

    ngx_int_t rc = ngx_http_mymodule_phase_handler ( r ); //  call phase handler

    if (rc >= NGX_HTTP_SPECIAL_RESPONSE) {
        ngx_http_finalize_request(r,0); // finalize on success
    }

    ELOG_DEBUG(r->connection->log, 0, "ASP FORM BODY HANDLER \n");

    if (r->request_body == NULL) {
        ELOG_CRIT(r->connection->log, 0, "Empty request body! \n");
//        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
/*
    if (r->request_body->buf == NULL) {
        ELOG_CRIT(r->connection->log, 0, "Empty request buf! \n");
    }

    if (r->request_body->bufs == NULL) {
        ELOG_CRIT(r->connection->log, 0, "Empty request bufs! \n");
//        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

*/

    ngx_array_t* http_args = ngx_array_create(r->pool, 16, sizeof(keyval_t));
    ngx_array_t* body_data = ngx_array_create(r->pool, 1024, sizeof(char));

    for (cl = r->request_body->bufs; cl;) {
        int buf_len = cl->buf->last-cl->buf->pos;
        ELOG_DEBUG(r->connection->log, 0, "ASP FORM BUF SIZE %p \n", buf_len);
        if (buf_len < 1) continue;

        char* ptr = ngx_array_push_n(body_data, buf_len);
        memcpy(ptr, cl->buf->pos, buf_len);

        cl = cl->next;
    }

    unsigned char* argdata = NULL;

    if (body_data != NULL && body_data->nelts > 0)
    {
        int errcode = parse_multipart(r, (char*)body_data->elts, body_data->nelts, http_args);

        if (errcode != 0) 
        {
            char* ptr = ngx_array_push(body_data);
            *ptr = '\0';
            argdata = (unsigned char*)body_data->elts;
            parse_http_args2(r, (char*)argdata, http_args);
        }
    }

    if (r->args.len > 0)
    {
        argdata = (unsigned char*) ngx_to_cstring(r->pool, r->args);

        if (argdata != NULL)
        {
            parse_http_args2(r, (char*)argdata, http_args);
        }
    }

    if (argdata == NULL)
    {
        argdata = ngx_pcalloc(r->pool, sizeof(char)); // set empty argdata
    }

    webs_t* wp = ngx_pcalloc(r->pool, sizeof(webs_t));
    if (!wp)
    {
        return;
    }

    wp->request = r;
    wp->pool = r->pool;
    wp->out = ngx_array_create(r->pool, 128, sizeof(char));
    wp->log = r->connection->log;
    wp->args = http_args;
    wp->on_response_ok = DO_NOTHING;

    if (check_auth(wp) == 1)
    {
        websError(wp, 403, "Permission denied!");
    }
    else
    {
        rc = asp_parser_parse_form(wp, (char*)formName, "", (char*)argdata);
    }

    b = ngx_calloc_buf(r->pool);
    if (b == NULL) {
        return;// NGX_HTTP_INTERNAL_SERVER_ERROR;
    }


//    if (ngx_http_set_content_type(r) != NGX_OK) {
//        return NGX_HTTP_INTERNAL_SERVER_ERROR;
//        return;
//    }

//    if (wp->out->nelts > 0)
//    {

        b->in_file = 0;
        b->memory = 1;

        b->last_buf = (r == r->main) ? 1: 0;
        b->last_in_chain = 1;
        b->start = wp->out->elts;
        b->end = b->start + wp->out->nelts;
        b->pos = b->start;
        b->last = b->end;

        out_chain.buf = b;
        out_chain.next = NULL;
/*    }
    else
    {
        out_chain.buf = NULL;
        out_chain.next = NULL;
    }
*/
    websAddHeader(wp, "Cache-Control", "no-cache, no-store, must-revalidate");

    if (wp->out->nelts == 0 && r->headers_out.status == NGX_HTTP_OK)
    {
        r->headers_out.status = NGX_HTTP_NO_CONTENT;
    }

//sending header
    rc = ngx_http_send_header(r);

    if (rc == NGX_ERROR || rc > NGX_OK) {
    //    return NGX_HTTP_INTERNAL_SERVER_ERROR;
        ELOG_ERR(r->connection->log, 0, "ASP FORM send header error %i \n", rc);
        return;
    }

// sending body
    if (wp->out->nelts > 0)
    {
        rc = ngx_http_output_filter(r, &out_chain);
    }
    ngx_http_finalize_request(r, rc);

    switch (wp->on_response_ok) {
        case DO_REBOOT:
            reboot_now(); break;
        case DO_RECONFIGURE:
        case DO_RECONFIGURE_AND_SAMBA_RESTART:
            doSystem("internet.sh &"); break;
        case DO_RESTART_MISC:
            doSystem("services_restart.sh misc &"); break;
    }
}


static ngx_int_t ngx_http_asp_form_handler(ngx_http_request_t *r)
{
    ngx_http_wive_loc_conf_t  *lcf;
//    size_t                     root;
//    ngx_str_t                  path;
//    u_char                    *last, *ptr;
    ngx_chain_t                 *cl;
    ngx_int_t                   rc;


    lcf = ngx_http_get_module_loc_conf(r, ngx_http_wive_module);
    if (lcf->is_post_form != 1)
    {
        return NGX_DECLINED;
    }

    if (!(r->method & (NGX_HTTP_GET|NGX_HTTP_POST))) 
    {
        return NGX_HTTP_NOT_ALLOWED;
    }

    ngx_str_t ctype = ngx_string("application/json");
    r->headers_out.content_type = ctype;
    r->headers_out.content_type_len = ctype.len;
    r->headers_out.status = 200;

    rc = ngx_http_read_client_request_body(r, ngx_http_mymodule_body_handler);
    return NGX_DONE;
}


static ngx_int_t ngx_http_asp_handler(ngx_http_request_t *r)
{
    u_char                    *last, *location;
    size_t                     root, len;
    ngx_str_t                  path;
    ngx_int_t                  rc;
    ngx_uint_t                 level;
    ngx_log_t                 *log;
    ngx_buf_t                 *b;
    ngx_chain_t                out_chain;
    ngx_open_file_info_t       of;
    ngx_http_core_loc_conf_t  *clcf;
    ngx_http_wive_loc_conf_t  *lcf;


    if (!(r->method & (NGX_HTTP_GET|NGX_HTTP_HEAD|NGX_HTTP_POST))) {
        return NGX_HTTP_NOT_ALLOWED;
    }

    if (r->uri.data[r->uri.len - 1] == '/') {
        return NGX_DECLINED;
    }

    log = r->connection->log;



    lcf = ngx_http_get_module_loc_conf(r, ngx_http_wive_module);
    if (lcf->variable == (ngx_http_complex_value_t *) -1
        || r->header_only
        || (r->method & NGX_HTTP_HEAD)
        || r != r->main
        || r->headers_out.status == NGX_HTTP_NO_CONTENT
/*        || ngx_http_test_content_type(r, &lcf->types) == NULL*/)
    {
        return NGX_DECLINED;
    }

    /*
     * ngx_http_map_uri_to_path() allocates memory for terminating '\0'
     * so we do not need to reserve memory for '/' for possible redirect
     */

    last = ngx_http_map_uri_to_path(r, &path, &root, 0);
    if (last == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }


    path.len = last - path.data;

    ngx_log_debug1(NGX_LOG_DEBUG_HTTP, log, 0,
                   "http filename: \"%s\"", path.data);

    clcf = ngx_http_get_module_loc_conf(r, ngx_http_core_module);

    ngx_memzero(&of, sizeof(ngx_open_file_info_t));

    of.read_ahead = clcf->read_ahead;
    of.directio = clcf->directio;
    of.valid = clcf->open_file_cache_valid;
    of.min_uses = clcf->open_file_cache_min_uses;
    of.errors = clcf->open_file_cache_errors;
    of.events = clcf->open_file_cache_events;

    if (ngx_http_set_disable_symlinks(r, clcf, &path, &of) != NGX_OK) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    if (ngx_open_cached_file(clcf->open_file_cache, &path, &of, r->pool)
        != NGX_OK)
    {
        switch (of.err) {

        case 0:
            return NGX_HTTP_INTERNAL_SERVER_ERROR;

        case NGX_ENOENT:
        case NGX_ENOTDIR:
        case NGX_ENAMETOOLONG:

            level = NGX_LOG_ERR;
            rc = NGX_HTTP_NOT_FOUND;
            break;

        case NGX_EACCES:
#if (NGX_HAVE_OPENAT)
        case NGX_EMLINK:
        case NGX_ELOOP:
#endif

            level = NGX_LOG_ERR;
            rc = NGX_HTTP_FORBIDDEN;
            break;

        default:

            level = NGX_LOG_CRIT;
            rc = NGX_HTTP_INTERNAL_SERVER_ERROR;
            break;
        }

        if (rc != NGX_HTTP_NOT_FOUND || clcf->log_not_found) {
            ngx_log_error(level, log, of.err,
                          "%s \"%s\" failed", of.failed, path.data);
        }

        return rc;
    }

    r->root_tested = !r->error_page;

    ngx_log_debug1(NGX_LOG_DEBUG_HTTP, log, 0, "http static fd: %d", of.fd);

    if (of.is_dir) {

        ngx_log_debug0(NGX_LOG_DEBUG_HTTP, log, 0, "http dir");

        ngx_http_clear_location(r);

        r->headers_out.location = ngx_list_push(&r->headers_out.headers);
        if (r->headers_out.location == NULL) {
            return NGX_HTTP_INTERNAL_SERVER_ERROR;
        }

        len = r->uri.len + 1;

        if (!clcf->alias && clcf->root_lengths == NULL && r->args.len == 0) {
            location = path.data + clcf->root.len;

            *last = '/';

        } else {
            if (r->args.len) {
                len += r->args.len + 1;
            }

            location = ngx_pnalloc(r->pool, len);
            if (location == NULL) {
                ngx_http_clear_location(r);
                return NGX_HTTP_INTERNAL_SERVER_ERROR;
            }

            last = ngx_copy(location, r->uri.data, r->uri.len);

            *last = '/';

            if (r->args.len) {
                *++last = '?';
                ngx_memcpy(++last, r->args.data, r->args.len);
            }
        }

        r->headers_out.location->hash = 1;
        ngx_str_set(&r->headers_out.location->key, "Location");
        r->headers_out.location->value.len = len;
        r->headers_out.location->value.data = location;

        return NGX_HTTP_MOVED_PERMANENTLY;
    }

#if !(NGX_WIN32) /* the not regular files are probably Unix specific */

    if (!of.is_file) {
        ELOG_CRIT(log, 0, "\"%s\" is not a regular file", path.data);

        return NGX_HTTP_NOT_FOUND;
    }

#endif

    if (r->method == NGX_HTTP_POST) {
        return NGX_HTTP_NOT_ALLOWED;
    }

    rc = ngx_http_discard_request_body(r);

    if (rc != NGX_OK) {
        return rc;
    }

    /* we need to allocate all before the header would be sent */

    b = ngx_calloc_buf(r->pool);
    if (b == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    b->file = NULL;

    int n_read = 0;
    unsigned char* data_buf = ngx_pcalloc(r->pool, of.size+1);

    if (of.size > 0) {
        ngx_file_t* file = ngx_pcalloc(r->pool, sizeof(ngx_file_t));

        file->fd = of.fd;
        file->name = path;
        file->log = log;
        file->directio = of.is_directio;

        n_read = ngx_read_file(file, data_buf, of.size, 0);

        if (n_read == NGX_ERROR) {
            ELOG_CRIT(r->connection->log, ngx_errno, "File read \"%s\" failed", file->name.data);
            return NGX_HTTP_INTERNAL_SERVER_ERROR;
        }
    }

    b->in_file = 0;
    b->memory = 1;

    b->last_buf = (r == r->main) ? 1: 0;
    b->last_in_chain = 1;
    b->start = data_buf;
    b->end = b->start + n_read;
    b->pos = b->start;
    b->last = b->end;

    out_chain.buf = b;
    out_chain.next = NULL;

    log->action = "sending response to client";

    r->headers_out.status = (n_read != 0) ? NGX_HTTP_OK : NGX_HTTP_NO_CONTENT;

//    r->headers_out.content_length_n = -1;
    r->headers_out.last_modified_time = of.mtime;

    if (ngx_http_set_etag(r) != NGX_OK) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    if (ngx_http_set_content_type(r) != NGX_OK) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    webs_t* wp = ngx_pcalloc(r->pool, sizeof(webs_t));
    wp->request = r;
    wp->pool = r->pool;
    wp->out = ngx_array_create(r->pool, 128, sizeof(char));
    wp->log = r->connection->log;
    wp->on_response_ok = DO_NOTHING;


    if (check_auth(wp) == 1)
    {
        if (r->headers_out.content_type.len >= sizeof("text/html") - 1 
            && ngx_strncasecmp(r->headers_out.content_type.data, (u_char *) "text/html", sizeof("text/html") - 1) == 0)
        {
            websRedirect(wp, "/login.asp");
        }
        else
        {
            websError(wp, 403, "Permission denied!");
        }
    }
    else
    {
        if (ngx_http_test_content_type(r, &lcf->types) == NULL)
        {
            return NGX_DECLINED;
        }

        int final_state = asp_state_machine(wp, &out_chain);
        if (final_state < 0)
        {
            ELOG_ERR(r->connection->log, 0, "ERROR! State machine final state: %i\n", final_state);
        }
    }

    asp_nvram_acl_t* acl_arr = (asp_nvram_acl_t*)asp_nvram_acl_array->elts;
    unsigned int n;
    for (n=0;n<asp_nvram_acl_array->nelts;n++)
    {
        asp_nvram_acl_t acl = acl_arr[n];
//        ELOG_DEBUG(r->connection->log, 0, "ACL %s=%i\n", acl.name, acl.flags);
    }

    b->start = wp->out->elts;
    b->end = (unsigned char*)(wp->out->elts) + wp->out->nelts;
    b->pos = b->start;
    b->last = b->end;


//    ELOG_DEBUG(r->connection->log, 0, "ASP HANDLER RES %p %p \n", b->pos, b->last);

    websAddHeader(wp, "Cache-Control", "no-cache, no-store, must-revalidate");

// sending header

    r->headers_out.content_length_n = wp->out->nelts; //out_chain.buf->end - out_chain.buf->start; // of.size;

    if (r != r->main && of.size == 0) {
        return ngx_http_send_header(r);
    }

    r->allow_ranges = 1;
//    ngx_http_clear_accept_ranges(r);

    rc = ngx_http_send_header(r);

    if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
        return rc;
    }

// sending body

    return ngx_http_output_filter(r, &out_chain);
}


static char *ngx_http_post_form_init(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_wive_loc_conf_t *flcf = conf;
    flcf->is_post_form = 1;

    return NGX_CONF_OK;
}

static char *ngx_http_disable_auth_init(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_wive_loc_conf_t *flcf = conf;
    flcf->disable_auth = 1;

    return NGX_CONF_OK;
}


static char *ngx_http_wive(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_wive_loc_conf_t *flcf = conf;

    ngx_str_t                    *value;
    ngx_http_complex_value_t    **cv;

    cv = &flcf->variable;

    if (*cv != NULL) {
        return "is duplicate";
    }

    value = cf->args->elts;

    if (value[1].len) {
        cmd->offset = offsetof(ngx_http_wive_loc_conf_t, variable);
        return ngx_http_set_complex_value_slot(cf, cmd, conf);
    }

    *cv = (ngx_http_complex_value_t *) -1;

    return NGX_OK;
}


static void *
ngx_http_wive_create_loc_conf(ngx_conf_t *cf)
{
    ngx_http_wive_loc_conf_t  *conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_wive_loc_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    /*
     * set by ngx_pcalloc():
     *
     *     conf->types = { NULL };
     *     conf->types_keys = NULL;
     *     conf->variable = NULL;
     */

    return conf;
}


static char *
ngx_http_wive_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_wive_loc_conf_t  *prev = parent;
    ngx_http_wive_loc_conf_t  *conf = child;

    if (ngx_http_merge_types(cf, &conf->types_keys, &conf->types,
                             &prev->types_keys,&prev->types,
                             ngx_http_html_default_types)
        != NGX_OK)
    {
       return NGX_CONF_ERROR;
    }

    if (conf->variable == NULL) {
        conf->variable = prev->variable;
    }

    if (conf->variable == NULL) {
        conf->variable = (ngx_http_complex_value_t *) -1;
    }

    return NGX_CONF_OK;
}


static ngx_int_t ngx_http_wive_init(ngx_conf_t *cf)
{
    int i;
    ngx_http_handler_pt        *h;
    ngx_http_core_main_conf_t  *cmcf;

    openlog("nginx-wive", LOG_PID, LOG_USER);

    cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

    h = ngx_array_push_n(&cmcf->phases[NGX_HTTP_CONTENT_PHASE].handlers, 2);
    if (h == NULL) {
        return NGX_ERROR;
    }

    *h = ngx_http_asp_handler;
    h++;
    *h = ngx_http_asp_form_handler;


//////////////////////////

    asp_parser_init(cf->pool);
#ifdef EXTERNAL_BUILD
    nvram_populate(cf->pool, "nvram.conf");
#endif


    srand(time(NULL));
    sessionid_init(cf->pool);

    ledAlways(CONFIG_RALINK_GPIO_SYS_LED, LED_ON); //Turn on power LED


// DEPRECATED FILTERS
//    ngx_http_next_body_filter = ngx_http_top_body_filter;
//    ngx_http_top_body_filter = ngx_http_wive_body_filter;

//    ngx_http_next_header_filter = ngx_http_top_header_filter;
//    ngx_http_top_header_filter = ngx_http_wive_header_filter;

    return NGX_OK;
}

