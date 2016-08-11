/*
 * Codes at here are heavily taken from upload.cgi.c which is for large file uploading , but
 * in fact "upload_settings" only need few memory(~16k) so it is not necessary to follow
 * upload.cgi.c at all.
 *
 * YYHuang@Ralink TODO: code size.
 *
 */

#ifndef _UPLOAD_F_H_
#define _UPLOAD_F_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/reboot.h>
#include <sys/reboot.h>

#include <linux/autoconf.h>  				/* kernel config		*/
#include "../../../tools/mkimage/include/image.h"	/* For Uboot image header format */

/* for calculate max image size */
#include "../../../linux/drivers/mtd/ralink/ralink-flash.h"

#define RFC_ERROR 		"RFC1867 ...."
#define MEM_SIZE        	1024
#define MEM_HALF        	512
#define MIN_FIRMWARE_SIZE       2097152 		/* minium firmware size(2MB) */

int firmware_upgrade(char* filename);

/*
void *memmem(const void *buf, size_t buf_len, const void *byte_line, size_t byte_line_len)
{
	unsigned char *bl = (unsigned char *)byte_line;
	unsigned char *bf = (unsigned char *)buf;
	unsigned char *p  = bf;

	while (byte_line_len <= (buf_len - (p - bf)))
	{
		unsigned int b = *bl & 0xff;
		if ((p = (unsigned char *) memchr(p, b, buf_len - (p - bf))) != NULL)
		{
			if ((memcmp(p, byte_line, byte_line_len)) == 0)
				return p;
			else
				p++;
		}
		else
			break;
	}
	return NULL;
}

int findStrInFile(char *filename, int offset, unsigned char *str, int str_len)
{
	int pos = 0, rc;
	FILE *fp;
	unsigned char mem[MEM_SIZE];

	if(str_len > MEM_HALF)
		return -1;
	if(offset <0)
		return -1;

	fp = fopen(filename, "rb");
	if(!fp)
		return -1;

	rewind(fp);
	fseek(fp, offset + pos, SEEK_SET);
	rc = fread(mem, 1, MEM_SIZE, fp);
	while(rc)
	{
		unsigned char *mem_offset;
		mem_offset = (unsigned char*)memmem(mem, rc, str, str_len);
		if (mem_offset)
		{
			fclose(fp);	//found it
			return (mem_offset - mem) + pos + offset;
		}

		if (rc == MEM_SIZE)
			pos += MEM_HALF;	// 8
		else
			break;

		rewind(fp);
		fseek(fp, offset+pos, SEEK_SET);
		rc = fread(mem, 1, MEM_SIZE, fp);
	}

	fclose(fp);
	return -1;
}
*/
/*
 *  ps. callee must free memory...
 */
/*
void *getMemInFile(char *filename, int offset, int len)
{
	void *result;
	FILE *fp;
	if ((fp = fopen(filename, "r")) == NULL )
		return NULL;

	fseek(fp, offset, SEEK_SET);
	result = malloc(sizeof(unsigned char) * len );

	if(!result)
		return NULL;

	if (fread(result, 1, len, fp) != len)
	{
		free(result);
		return NULL;
	}

	return result;
}

static void html_header()
{
	fprintf
	(
		stdout,
		"Server: %s\n"
		"Pragma: no-cache\n"
		"Content-type: text/html\n",
		getenv("SERVER_SOFTWARE")
	);

	fprintf
	(
		stdout,
		"\n<html>\n<head>\n"
		"<title>Import Settings</title>\n"
		"<link rel=\"stylesheet\" href=\"/style/normal_ws.css\" type=\"text/css\">\n"
		"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
	);

	char data[2048];

	// Copy /js/ajax.js to stdout
	FILE *fd = fopen("/web/js/ajax.js", "r");
	if (fd != NULL)
	{
		size_t count;
		fprintf(stdout, "<script type=\"text/javascript\">\n");
		fprintf(stdout, "// Here is script copied from file /js/ajax.js\n");

		while ((count = fread(data, sizeof(char), sizeof(data)/sizeof(char), fd)) > 0)
			fwrite(data, sizeof(char), count, stdout);

		fclose(fd);
	}
	else
		fprintf(stdout, "<script type=\"text/javascript\" src=\"/js/ajax.js\">\n");

	// Output end of javascript
	fprintf
	(
		stdout,
		"</script>\n"
		"</head>\n"
		"<body>\n"
		"<h1>Update Settings</h1>\n"
	);
}

static void html_success(int timeout)
{
	fprintf
	(
		stdout,
		"<p>Done</p>\n"
		"<script language=\"JavaScript\" type=\"text/javascript\">\n"
		"ajaxReloadDelayedPage(%d000);\n"
		"</script>"
		"</body></html>\n\n",
		timeout
	);
	// Output success message
	fflush(stdout);
	fclose(stdout);
}

static void html_error(const char *s)
{
	fprintf
	(
		stdout,
		"<p>%s</p>\n"
		"<script language=\"JavaScript\" type=\"text/javascript\">\n"
		"alert('%s');\n"
		"ajaxReloadDelayedPage(0);\n"
		"</script>\n"
		"</body></html>\n\n",
		s, s
	);
	// Output error message
	fflush(stdout);
	fclose(stdout);
}
*/
//------------------------------------------------------------------------------
// Multipart/form-data parser

// Multipart/form-data parameter
/*
typedef struct upload_parameter_t
{
	char *content_type; // Content type
	char *field_name; // field name
	char *value; // field value
	long start_pos; // start position of parameter
	long end_pos; // end position of parameter

	struct upload_parameter_t *next;
} upload_parameter_t;

int get_content_separator(char *separator, int limit, long *length)
{
	// Get content type
	char *content_type = (char *)getenv("CONTENT_TYPE");
	char *content_len  = (char *)getenv("CONTENT_LENGTH");
	if ((content_type == NULL) || (content_len == NULL))
	{
		content_type = (char *)getenv("HTTP_CONTENT_TYPE");
		content_len  = (char *)getenv("HTT_CONTENT_LENGTH");
	}
	if ((content_type == NULL) || (content_len == NULL))
		return -1;

	if (strncasecmp(content_type, "multipart/form-data;", strlen("multipart/form-data;")) != 0)
		return -1;

	// Now parse boundary
	content_type = strstr(content_type, "boundary=");
	if (content_type == NULL)
		return -1;

	// Find boundary
	content_type += strlen("boundary=");
	while ((content_type[0] == '"') && (content_type[0]==' '))
		content_type++;

	// Get boundary
	while (1)
	{
		// Protect from buffer overflow
		if ((--limit)<=0)
			return -1;

		if ((content_type[0] == ' ') || (content_type[0] == '\0') || (content_type[0] == '"'))
			break;

		*(separator++) = *(content_type++);
	}

	// Terminating character
	*separator = '\0';
	*length = atol(content_len);

	return 0;
}

#define BUF_SIZE 4096
#define MAX_SEPARATOR_LEN 128

static int search_data(FILE *fd, long start, long *found_offset, const void *buffer, int len)
{
	char buf[BUF_SIZE];
	const char *data = (const char *)buffer;
	int offset = sizeof(buf);     // buffer offset
	int found  = 0;               // number of chars matched
	int read   = 0;               // buffer size
	long flpos = 0;               // file last read position
	long lmatch= start;           // position of first character in match sequence

	if (fseek(fd, start, SEEK_SET)<0)
		return -1;

	while (found < len)
	{
		// Check if buffer is empty
		if (offset >= read)
		{
			flpos = ftell(fd);
			read  = fread(buf, 1, sizeof(buf), fd);

			if (read <= 0) // Nothnig to read?
				return EOF;

			offset = 0;
		}

		// Check if characters match
		if (data[found] != buf[offset])
		{
			found = 0;
			if (lmatch < flpos) // Check if buffer is out of range
			{
				// Mark bufer re-read & change position
				if (fseek(fd, ++lmatch, SEEK_SET)<0)
					return -1;
				offset = sizeof(buf);
			}
			else
				offset = (++lmatch) - flpos; // Seek to next character
		}
		else
		{
			if (found == 0)
				lmatch = flpos + offset; // Remember last found character file offset
			// Characters matched
			found++;
			offset++;
		}
	}

	*found_offset = lmatch;

	return 0;
}

static int search_text(FILE *fd, long start, long *found_offset, const char *data)
{
	return search_data(fd, start, found_offset, data, strlen(data));
}

typedef enum rd_params_t
{
	INIT,
	READ_HEADER,
	READ_CONTENT
} rd_params_t;

typedef struct buffer_t
{
	char *data;
	size_t size;
} buffer_t;

static void init_buffer(buffer_t *buf, size_t initial)
{
	buf->size = initial;
	buf->data = (char *)malloc(initial);
}

static int read_buffer(FILE *fd, long start, size_t count, buffer_t *buf)
{
	// Check size
	if (buf->size < (count+1))
	{
		size_t new_size = ((count+1)+0x80)&(~0x7f);
		char *ptr = (char *)realloc(buf->data, new_size);
		if (ptr == NULL)
			return -1;
		buf->data = ptr;
		buf->size = new_size;
	}

	// Seek to specified position data
	if (fseek(fd, start, SEEK_SET)<0)
		return -2;

	// Now read data
	int read = fread(buf->data, 1, count, fd);
	if (read < 0)
		return read;

	// Append buffer with '\0' character
	buf->data[read] = '\0';
	return read;
}

static void release_buffer(buffer_t *buf)
{
	free(buf->data);
}

static void reset_parameter(upload_parameter_t *param)
{
	param->content_type = NULL;
	param->field_name = NULL;
	param->value = NULL;
	param->start_pos = 0;
	param->end_pos = 0;
	param->next = NULL;
}

static void release_parameters(upload_parameter_t *list)
{
	while (list != NULL)
	{
		upload_parameter_t *data = list;
		list = list->next;

		// Release data
		if (data->content_type != NULL)
			free(data->content_type);
		if (data->field_name != NULL)
			free(data->field_name);
		if (data->value != NULL)
			free(data->value);

		// And release list pointer
		free(data);
	}
}

upload_parameter_t *find_parameter(upload_parameter_t *list, const char *name)
{
	while (list != NULL)
	{
		if (list->field_name != NULL)
		{
			if (strcmp(list->field_name, name) == 0)
				return list;
		}

		list = list->next;
	}

	return NULL;
}

const char* headers[] =
{
	"Content-Disposition: ",
	"Content-Type: "
};
const char* valid_content_type[]=
{
	"application/mac-binary",
	"application/macbinary",
	"application/octet-stream",
	"application/x-binary",
	"application/x-macbinary",
	NULL
};

int check_binary_content_type(const char *content_type)
{
	const char **ptr = valid_content_type;
	while (*ptr != NULL)
	{
		// Check content type validity
		if (strcasecmp(*ptr, content_type)==0)
			return 1;

		ptr++;
	}

	return 0;
}
*/

/*
int read_parameters(FILE *fd, const char *separator, upload_parameter_t **result_params)
{
	char start[MAX_SEPARATOR_LEN+8], end[MAX_SEPARATOR_LEN+8], middle[MAX_SEPARATOR_LEN+8];
	rd_params_t state = INIT;
	long position = 0;
	upload_parameter_t param;
	buffer_t buf;
	upload_parameter_t *list = NULL;

	if (strlen(separator) > MAX_SEPARATOR_LEN)
		return -1;

	strcpy(start, "--");
	strcat(start, separator);
	strcat(start, "\r\n"); // -- + separator + \r\n
	strcpy(middle, "\r\n--");
	strcat(middle, separator);
	strcat(middle, "\r\n"); // \r\n-- + separator + \r\n
	strcpy(end, "\r\n--");
	strcat(end, separator);
	strcat(end, "--"); // \r\n-- + separator + --

	init_buffer(&buf, 0x100);

	while (1)
	{
		if (state == INIT) // Find start
		{
			long start_pos = 0;
			int result = search_text(fd, position, &start_pos, start);
			if (result < 0)
			{
				release_parameters(list);
				release_buffer(&buf);
				return -1;
			}

			state = READ_HEADER; // Parse parameter
			position = start_pos + strlen(start);
			reset_parameter(&param);
		}
		else if (state == READ_HEADER) // Parse headers
		{
			long start_pos = 0;
			int result = search_text(fd, position, &start_pos, "\r\n");
			if (result < 0)
			{
				release_parameters(list);
				release_buffer(&buf);
				return -2;
			}

			// Analyze size
			if (start_pos != position)
			{
				if ((start_pos - position) > 0x10000)
				{
					release_parameters(list);
					release_buffer(&buf);
					return -1;
				}

				result = read_buffer(fd, position, start_pos-position, &buf);
				if (result < 0)
				{
					release_parameters(list);
					release_buffer(&buf);
					return result;
				}

				// Now analyze header
				if (strncasecmp(buf.data, headers[0], strlen(headers[0]))==0) // Content-disposition
				{
					// Check that it's correct
					if (strstr(buf.data, "form-data;") == NULL)
					{
						release_parameters(list);
						release_buffer(&buf);
						return -3;
					}
					// Get content name
					const char *ptr = strstr(buf.data, " name=");
					if (ptr == NULL)
					{
						release_parameters(list);
						release_buffer(&buf);
						return -4;
					}
					ptr += strlen(" name=");
					// Skip spaces & quotes
					while (((*ptr) != '\0') && (*ptr == '"'))
						++ptr;
					if (*ptr == '\0')
					{
						release_parameters(list);
						release_buffer(&buf);
						return -5;
					}
					const char *ptre = ptr+1;
					while (((*ptre) != '\0') && (*ptre != '"'))
						++ptre;
					if (*ptre == '\0')
					{
						release_parameters(list);
						release_buffer(&buf);
						return -6;
					}
					// Check element name length
					size_t len = ptre-ptr;
					param.field_name = (char *)malloc(len+1);
					if (param.field_name == NULL)
					{
						release_parameters(list);
						release_buffer(&buf);
						return -7;
					}

					// Now extract element name
					memcpy(param.field_name, ptr, len);
					param.field_name[len] = '\0';
				}
				else if (strncasecmp(buf.data, headers[1], strlen(headers[1]))==0) // Content-type
				{
					const char *ptr = &buf.data[strlen(headers[1])];
					param.content_type = (char *)malloc(strlen(ptr)+1);
					if (param.content_type == NULL)
					{
						release_parameters(list);
						release_buffer(&buf);
						return -7;
					}

					strcpy(param.content_type, ptr);
				}

				// Move pointer
				position = start_pos;
			}
			else
				state = READ_CONTENT;

			position += strlen("\r\n");
		}
		else if (state == READ_CONTENT)
		{
			long start_pos = 0;
			int final = 0;

			// Search separator or tail
			int result = search_text(fd, position, &start_pos, middle);
			if (result < 0)
			{
				result = search_text(fd, position, &start_pos, end);
				if (result < 0)
				{
					release_parameters(list);
					release_buffer(&buf);
					return result;
				}
				final = 1;
			}

			// Found separator position, look content-type
			if (param.content_type == NULL) // No content type, read param to memory
			{
				result = read_buffer(fd, position, start_pos-position, &buf);
				if (result < 0)
				{
					release_parameters(list);
					release_buffer(&buf);
					return result;
				}

				param.value = (char *)malloc(strlen(buf.data)+1);
				if (param.value == NULL)
				{
					release_parameters(list);
					release_buffer(&buf);
					return -7;
				}

				strcpy(param.value, buf.data);
			}
			else
			{
				param.start_pos = position;
				param.end_pos   = start_pos;
			}

			// Copy parameter to list
			upload_parameter_t *p_result = (upload_parameter_t *)malloc(sizeof(upload_parameter_t));
			if (p_result == NULL)
			{
				release_parameters(list);
				release_buffer(&buf);
				return -1;
			}

			// Store parameter in list
			memcpy(p_result, &param, sizeof(upload_parameter_t));
			p_result->next = list;
			list = p_result;

			if (final)
			{
				release_buffer(&buf);
				*result_params = p_result;
				return 0;
			}

			// Reset parameter
			position = start_pos + strlen(middle);
			reset_parameter(&param);
			state = READ_HEADER;
		}
	}
}
*/

#endif