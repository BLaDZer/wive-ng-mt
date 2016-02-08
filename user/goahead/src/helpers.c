#include "helpers.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>

#include	"utils.h"
#include	"internet.h"

// Sizes table
const char *normalSizes[] =
{
	"B",
	"kB",
	"MB",
	"GB",
	"TB",
	"PB",
	"HB",
	"ZB"
};

// Normalize size
const char *normalizeSize(long long *size)
{
	const char **result = normalSizes;
	while ((*size)>(128*1024))
	{
		*size /= 1024;
		result++;
	}
	return *result;
}

// Read unsigned number
// Returns -1 on error
long readUnsigned(const char *str)
{
	long result = 0;
	while ((*str)!='\0')
	{
		char ch = *(str++);
		if ((ch<'0') || (ch>'9'))
			return -1;
		result = (result*10) + (ch-'0');
	}
	return result;
}

void setupParameters(webs_t wp, const parameter_fetch_t *fetch, int transaction)
{
	if (transaction)
		nvram_init(RT2860_NVRAM);

	while (fetch->web_param != NULL)
	{
		// Get variable
		char_t *str = websGetVar(wp, (char_t *)fetch->web_param, (char_t *)fetch->dfl_param);
		if (fetch->is_switch) // Check if need update a switch
		{
			if (strcmp(str, "on") != 0)
				str = "off";
			if (fetch->is_switch == 2)
				str = (strcmp(str, "on") == 0) ? "1" : "0";
		}

		if (nvram_bufset(RT2860_NVRAM, (char_t *)fetch->nvram_param, (void *)str)!=0) //!!!
			syslog(LOG_ERR, "set %s nvram error, %s", fetch->nvram_param, __FUNCTION__);
#ifdef PRINT_DEBUG
		printf("%s value : %s\n", fetch->nvram_param, str);
#endif
		fetch++;
	}

	if (transaction)
	{
		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}
}

char *catIndex(char *buf, const char *ptr, int index)
{
	char *p = buf;
	while ((*ptr) != '\0')
		*(p++) = *(ptr++);
	*(p++) = (char)(index + '0');
	*p = '\0';

	return buf;
}

// String splitting tools
int initSplitter(string_split_t *buf)
{
	buf->buf = (char *)malloc(SPLITTER_BUFFER_QUANTITY);
	if (buf->buf == NULL)
		return errno;
	buf->items = (char **)malloc(SPLITTER_TOKEN_QUANTITY * sizeof(char *));
	if (buf->items == NULL)
	{
		free(buf->buf);
		buf->buf = NULL;
		return errno;
	}

	buf->buf_size = SPLITTER_BUFFER_QUANTITY;
	buf->pointers = SPLITTER_TOKEN_QUANTITY;
	buf->found    = 0;

	return 0;
}

int splitString(string_split_t *buf, const char *string, char splitter)
{
	// First reset
	buf->found = 0;
	if (string == NULL)
		return 0;

	// Calculate character buffer size
	size_t size = strlen(string) + 1;

	// Check if need to realloc buffer
	if (buf->buf_size < size)
	{
		free(buf->buf);
		size_t amount = size + (SPLITTER_BUFFER_QUANTITY - size%SPLITTER_BUFFER_QUANTITY);
		buf->buf = (char *)malloc(amount);
		if (buf->buf == NULL)
			return errno;
		buf->buf_size = amount;
	}

	// copy string
	memcpy(buf->buf, string, size);

	// calculate splitters
	buf->found = 1;
	char *p = buf->buf;
	while (*p != '\0')
	{
		if (*(p++) == splitter)
			buf->found++;
	}

	// Check if need to realloc pointers
	if (buf->pointers < buf->found)
	{
		free(buf->items);
		size_t amount = buf->found + (SPLITTER_TOKEN_QUANTITY - buf->found%SPLITTER_TOKEN_QUANTITY);
		buf->items = (char **)malloc(amount * sizeof(char *));
		if (buf->items == NULL)
			return errno;
		buf->pointers = amount;
	}

	// Make split
	p = buf->buf;
	char **it = buf->items;
	*(it++) = p; // Assign first token

	while (*p != '\0')
	{
		if (*p == splitter)
		{
			*(p++) = '\0';
			*(it++) = p;
		}
		else
			p++;
	}

	return 0;
}

int freeSplitter(string_split_t *buf)
{
	if (buf->buf != NULL)
	{
		free(buf->buf);
		buf->buf = NULL;
	}
	if (buf->items != NULL)
	{
		free(buf->items);
		buf->items = NULL;
	}
	return 0;
}

void STFs(int nvram, int index, char *flash_key, char *value)
{
	char *result;
	char *tmp = nvram_get(nvram, flash_key);
	if(!tmp)
		tmp = "";
	result = setNthValue(index, tmp, value);
	nvram_bufset(nvram, flash_key, result);
	return ;
}
