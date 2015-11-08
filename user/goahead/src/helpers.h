#ifndef HELPERS_H_
#define HELPERS_H_

#include <sys/types.h>
#include "webs.h"

#define SPLITTER_BUFFER_QUANTITY        1024
#define SPLITTER_TOKEN_QUANTITY         32

//----------------------------------------------
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
				do{		char tmp[128];					\
				    if(! ( x  = nvram_get(nvram, #x)) )				\
					tmp[0] = '\0';						\
				    else{							\
					if( getNthValueSafe(n, x, ';', tmp, 128) != -1){	\
					    gstrncat(result, tmp, 4096);			\
					}							\
				    }gstrncat(result, "\r", 4096);				\
				}while(0)

/* Load from Web */
#define LFW(x, y) do{ if(! ( x = websGetVar(wp, T(#y), T("")))) return;	}while(0)

//----------------------------------------------

/* Special functions */
typedef struct replacement_t
{
	const char *key;
	const char *value;
} replacement_t;

typedef struct parameter_fetch_t
{
	const char *web_param;
	const char *nvram_param;
	int is_switch;
	const char *dfl_param;
} parameter_fetch_t;

typedef struct string_split_t
{
	size_t  buf_size;
	size_t  pointers;
	size_t  found;
	char   *buf;
	char  **items;
} string_split_t;

extern const char *normalizeSize(long long *size);
extern long readUnsigned(const char *str);

// Set-up parameters in NVRAM
extern void setupParameters(webs_t wp, const parameter_fetch_t *fetch, int transaction);
extern char *catIndex(char *buf, const char *ptr, int index);

// String splitting tools
extern int initSplitter(string_split_t *buf);
extern int splitString(string_split_t *buf, const char *string, char splitter);
extern int freeSplitter(string_split_t *buf);

extern void STFs(int nvram, int index, char *flash_key, char *value);
#endif /* HELPERS_H_ */
