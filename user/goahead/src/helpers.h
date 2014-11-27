#ifndef HELPERS_H_
#define HELPERS_H_

#include <sys/types.h>
#include "webs.h"

#define HTML_BUFFER_QUANTITY            1024
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

//----------------------------------------------

/* Special functions */
typedef struct replacement_t
{
	const char *key;
	const char *value;
} replacement_t;

typedef struct html_buffer_t
{
	size_t  size;   // Buffer capacity
	size_t  c_pos;  // Current buffer position
	char   *data;   // Buffer data
} html_buffer_t;

typedef struct html_replacement_t
{
	char            token;
	const char     *replacement;
} html_replacement_t;

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

extern const char *replaceWords(const char *key, const replacement_t *table);
extern const char *normalizeSize(long long *size);
extern long readUnsigned(const char *str);

// HTML content encoding routines
extern int initHTMLBuffer(html_buffer_t *buf);
extern void resetHTMLBuffer(html_buffer_t *buf);
extern int encodeHTMLContent(const char *data, html_buffer_t *buf);
extern int freeHTMLBuffer(html_buffer_t *buf);

extern int checkFileExists(const char *argv);

// Set-up parameters in NVRAM
extern void setupParameters(webs_t wp, const parameter_fetch_t *fetch, int transaction);
extern char *catIndex(char *buf, const char *ptr, int index);

// Get parameter index
extern void fetchIndexedParam(const char *buf, int index, char *retbuf);

// String splitting tools
extern int initSplitter(string_split_t *buf);
extern int splitString(string_split_t *buf, const char *string, char splitter);
extern int freeSplitter(string_split_t *buf);
#endif /* HELPERS_H_ */
