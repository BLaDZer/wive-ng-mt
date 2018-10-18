#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <ngx_hash.h>
#include <ngx_md5.h>

#include <syslog.h>

#include "sdk_version.h"
#include "config/autoconf.h"			//user config
#include "user/busybox/include/autoconf.h"	//busybox config
#ifndef EXTERNAL_BUILD
#include <linux/autoconf.h>			//kernel config
#endif

#define	T(s) 			s

// comment out to build with native libs
#include "external.h"

#ifdef EXTERNAL_BUILD
#include "stub/libnvram.h"
#include "stub/libwive.h"
#else
#include <libnvram.h>
#include <libwive.h>
#endif

enum UserRole {
    DENY = 0,
    USER = 1,
    ADMIN = 2
};

enum UserRole get_user_role(char* username);


ngx_array_t* asp_nvram_acl_array;

#define outWrite(...) arrsWrite(wp->out, __VA_ARGS__)
#define websWrite(x,...) arrsWrite(x->out, __VA_ARGS__)

#define UNUSED(x) (void)(x)

#define FUNCTION_TRACE(x) ngx_log_error(NGX_LOG_DEBUG, x, 0, ": %s(%s:%i)\n", __func__, __FILE__, __LINE__)

#define ELOG_DEBUG(...) ngx_log_error(NGX_LOG_DEBUG, __VA_ARGS__)
#define ELOG_INFO(...) ngx_log_error(NGX_LOG_INFO, __VA_ARGS__)
#define ELOG_NOTICE(...) ngx_log_error(NGX_LOG_NOTICE, __VA_ARGS__)
#define ELOG_WARN(...) ngx_log_error(NGX_LOG_WARN, __VA_ARGS__)

#define ELOG_ERR(...) ngx_log_error(NGX_LOG_ERR, __VA_ARGS__)
#define ELOG_CRIT(...) ngx_log_error(NGX_LOG_CRIT, __VA_ARGS__)
#define ELOG_ALERT(...) ngx_log_error(NGX_LOG_ALERT, __VA_ARGS__)
#define ELOG_EMERG(...) ngx_log_error(NGX_LOG_EMERG, __VA_ARGS__)

#define TRUE 1
#define FALSE 0


enum UserPermission {
  NOBODY = 0,
  USER_R = 1,
  USER_W = 2,
  USER_X = 4,
  ADMIN_R = 8,
  ADMIN_W = 16,
  ADMIN_X = 32,
  USER_RW = 1 | 2,
  USER_RWX = 1 | 2 | 4,
  ADMIN_RW = 8 | 16,
  ADMIN_RWX = 8 | 16 | 32,
  EVERYONE = 1 | 2 | 4 | 8 | 16 | 32
};

enum ResponseSentAction {
    DO_NOTHING = 0,
    DO_REBOOT = 1,
    DO_RECONFIGURE = 2,
    DO_RECONFIGURE_AND_SAMBA_RESTART = 3,
    DO_RESTART_MISC = 4
};

typedef struct keyval_t {
    char* key;
    char* val;
} keyval_t;


typedef struct webs_t {
    ngx_http_request_t* request;
    ngx_pool_t* pool;   // request mempool
    ngx_array_t* out;   // outbut buffer, array of char
    ngx_log_t* log;     // request connection log
    ngx_array_t* args;  // http GET/POST arguments (check when it populates!), array of keyval_t

    struct auth_session_t* auth_session;
    enum ResponseSentAction on_response_ok; // do action if response was successful
} webs_t;

typedef struct asp_nvram_acl_t{
    char* name;
    int flags;
} asp_nvram_acl_t;

//char PASS_HASH_SALT[128];

#ifdef NGX_HTTP_SSL
void sha256(char *string, char outputBuffer[65]);
#endif
void md5(char *string, char outputBuffer[65]);

//ngx_array_t* get_passwd_users(ngx_pool_t* pool);
int check_shadow_pass(char* username, char* password);
char* crypt_pass(char* username, char* password);
int doSystem(char *fmt, ...);

ngx_str_t n_base64_decode(ngx_pool_t* pool, ngx_str_t str);

ngx_str_t  n_skipchar (ngx_str_t str, int n);
int        n_strcmp   (ngx_str_t str1, ngx_str_t str2);
ngx_str_t  n_strstr   (ngx_str_t str1, ngx_str_t str2);

ngx_str_t  n_strstr2   (ngx_http_request_t *r, ngx_str_t str1, ngx_str_t str2);

ngx_str_t n_escape_html(ngx_pool_t* pool, ngx_str_t str);
ngx_str_t n_unescape_uri(ngx_pool_t* pool, ngx_str_t str, ngx_uint_t type);

ngx_array_t* ngx_string_split(ngx_pool_t* pool, ngx_str_t str, ngx_str_t sep);
ngx_str_t ngx_split_inplace(ngx_str_t str, ngx_str_t sep);
ngx_str_t ngx_split_inplace2(ngx_http_request_t *r, ngx_str_t str, ngx_str_t sep);

char *ngx_array_to_cstring(ngx_pool_t* pool, ngx_array_t *arr);


char* cstring_trim_left(ngx_pool_t* pool, char* str, char symbol);
char* cstring_trim_right(ngx_pool_t* pool, char* str, char symbol);
char* cstring_trim(ngx_pool_t* pool, char* str, char symbol);

char* cstring_copy(ngx_pool_t* pool, char* str);

ngx_str_t cstring_to_ngx(ngx_pool_t* pool, char* str);
char* ngx_to_cstring(ngx_pool_t* pool, ngx_str_t str);

ngx_array_t* cstring_split(ngx_pool_t* pool, char* str, char* sep);
char* cstring_split_inplace(char* str, char* sep);

int arrsWrite(ngx_array_t *arr, char* fmt, ...);

int nvram_acl_check(webs_t* wp, char* key, int write);

int ngx_nvram_bufset(webs_t* wp, char *key, char *value);
int ngx_nvram_set(webs_t* wp, char *key, char *value);

char* ngx_nvram_get(webs_t* wp, char* key);
int ngx_nvram_get_int(webs_t* wp, char* key, int def);

char* getCookie(ngx_http_request_t *r, char* name);

int import_rwfs(char *filename, int offset, int len);
int import_settings(char *filename, int offset, int len);

//char* T(char* str);


#endif // __UTIL_H__