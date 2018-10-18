#ifndef __ASP_MOD_HELPERS_H__
#define __ASP_MOD_HELPERS_H__

#include "util.h"

ngx_array_t* asp_func_array;
ngx_array_t* asp_form_array;

typedef struct parameter_fetch_t
{
	char *web_param;
	char *nvram_param;
	int is_switch;
	char *dfl_param;
} parameter_fetch_t;


typedef char char_t;

typedef int (*asp_func_proto)(webs_t *wp, char** params, int nparams);
typedef void (*asp_form_proto)(webs_t* wp, char_t *path, char_t *query);

typedef struct asp_func_t{
    char* name;
    asp_func_proto ptr;
    enum UserRole perm;
} asp_func_t;

typedef struct asp_form_t{
    char* name;
    asp_form_proto ptr;
    enum UserRole perm;
} asp_form_t;




typedef struct {
    int         code;   /* HTTP error code */
    char_t      *msg;   /* HTTP error message */
} websErrorType;


#define WEBS_CACHE_CONTROL_STRING	T("Cache-Control: no-store, no-cache, must-revalidate, post-check=0, pre-check=0\r\nPragma: no-cache\r\n")

//----------------------------------------------
// System defines

#define LED_ON				1
#define LED_OFF				0

#define IPT_SHORT_ACCOUNT

#define _PATH_IPT_ACCOUNTING_FILE	"/proc/net/ipt_account/mynetwork"

#define RULE_MODE_DISABLE		0
#define RULE_MODE_DROP			1
#define RULE_MODE_ACCEPT		2

#define TXBYTE				0
#define TXPACKET			1
#define RXBYTE				2
#define RXPACKET			3

#define DMZ_CHAIN			"DMZ"

// Optimizer macros
#define CHK_IF_DIGIT(var, val)  \
	((var != NULL) && (var[0] == (val + '0')) && (var[1] == '\0'))

#define CHK_GET_DIGIT(var) \
	(((var[0] >= '0') && (var[1] <= '9') && (var[1] == '\0')) ? (var[0] - '0') : -1)

#define CHK_IF_CHECKED(var)  \
	(((var != NULL) && ((var[0] == 'o') || (var[0] == 'O')) && ((var[1] == 'n') || (var[1] == 'N')) && (var[2] == '\0')) ? 1 : 0)

#define CHK_IF_SET(var) \
	((var != NULL) && (var[0] != '\0'))

#define CHK_IF_EMPTYSTR(var) \
	((var == NULL) || (var[0] == '\0'))


/* STF means "Save To Flash" ...*/
#define STF(nvram, index, flash_key)    STFs(nvram, index, #flash_key, flash_key)

/* LFF means "Load From Flash" ...*/
#define LFF(result, nvram, x, n)								\
				do{	char tmp[128];						\
					if(! ( x  = nvram_get(nvram, #x)) )			\
						tmp[0] = '\0';					\
					else {							\
						if( getNthValueSafe(n, x, ';', tmp, 128) != -1)	\
							strncat(result, tmp, 4096);		\
					}							\
				}								\
				while(0)


#define LFW(x, y) do{ if(! ( x = websGetVar(wp, T(#y), T("")))) return;	}while(0)

char_t 	*websGetVar(webs_t* wp, char_t *var, char_t *def);
void setupParameters(webs_t* wp, parameter_fetch_t *fetch, int transaction);

int aspDefineFunc(char* func_name, asp_func_proto func_ptr, enum UserPermission perm);
int websFormDefine(char* func_name, asp_form_proto form_ptr, enum UserPermission perm);

void outputTimerForReload(webs_t* wp, long delay);

int websAddHeader(webs_t* wp, char* key, char* value);

int websDone(webs_t* wp, int code);
void websSetContentType(webs_t* wp, char* content_type);
int websRedirect(webs_t* wp, char_t *url);

void websLongWrite(webs_t* wp, char *lonstr);
void websError(webs_t* wp, int code, char_t *fmt, ...);

void websHeader(webs_t* wp);
void websFooter(webs_t* wp);

int getBasicAuthCredentials(webs_t* wp, char** username, char** password);

#endif // __ASP_MOD_HELPERS_H__