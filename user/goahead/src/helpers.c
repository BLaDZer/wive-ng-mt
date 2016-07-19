#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>

#include "helpers.h"

void setupParameters(webs_t wp, parameter_fetch_t *fetch, int transaction)
{
	if (transaction)
		nvram_init(RT2860_NVRAM);

	while (fetch->web_param != NULL)
	{
		// Get variable
		char_t *str = websGetVar(wp, fetch->web_param, fetch->dfl_param);
		if (fetch->is_switch) // Check if need update a switch
		{
			if (strcmp(str, "on") != 0)
				str = "off";
			if (fetch->is_switch == 2)
				str = (strcmp(str, "on") == 0) ? "1" : "0";
		}

		if (nvram_bufset(RT2860_NVRAM, fetch->nvram_param, (void *)str)!=0) //!!!
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

// String splitting tools
int initSplitter(string_split_t *buf)
{
	buf->buf = (char *)balloc(B_L, SPLITTER_BUFFER_QUANTITY);
	if (buf->buf == NULL)
		return errno;
	buf->items = (char **)balloc(B_L, SPLITTER_TOKEN_QUANTITY * sizeof(char *));
	if (buf->items == NULL)
	{
		bfree(B_L, buf->buf);
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
		bfree(B_L, buf->buf);
		size_t amount = size + (SPLITTER_BUFFER_QUANTITY - size%SPLITTER_BUFFER_QUANTITY);
		buf->buf = (char *)balloc(B_L, amount);
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
		bfree(B_L, buf->items);
		size_t amount = buf->found + (SPLITTER_TOKEN_QUANTITY - buf->found%SPLITTER_TOKEN_QUANTITY);
		buf->items = (char **)balloc(B_L, amount * sizeof(char *));
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
		bfree(B_L, buf->buf);
		buf->buf = NULL;
	}
	if (buf->items != NULL)
	{
		bfree(B_L, buf->items);
		buf->items = NULL;
	}
	return 0;
}

/*
 * description: parse va and do system
 */
int doSystem(char_t *fmt, ...)
{
	va_list	vargs;
	char_t	*cmd = NULL;
	int	rc = 0;

	va_start(vargs, fmt);
	if (fmtValloc(&cmd, WEBS_BUFSIZE, fmt, vargs) >= WEBS_BUFSIZE)
		syslog(LOG_ERR, "lost data, buffer overflow , %s", __FUNCTION__);

	va_end(vargs);

	if (cmd) {
		rc = system(cmd);
		bfree(B_L, cmd);
	}

	return rc;
}

void reboot_now(void)
{
	doSystem("(sleep 2 && /etc/scripts/reboot.sh) > /dev/null 2>&1 &");
}

void redirect_wholepage(webs_t wp, const char *url)
{
	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/html\n"));
	websWrite(wp, WEBS_CACHE_CONTROL_STRING);
	websWrite(wp, T("\n"));
	websWrite(wp, T("<html><head><script language=\"JavaScript\">"));
	websWrite(wp, T("parent.location.replace(\"%s\");"), url);
	websWrite(wp, T("</script></head></html>"));
}

void outputTimerForReload(webs_t wp, char_t *url, long delay)
{
	char lan_if_addr[32];
	const char *lan_if_ip;

	if (getIfIp(getLanIfName(), lan_if_addr) != -1)
		lan_if_ip = lan_if_addr;
	else
	{
		lan_if_ip = nvram_get(RT2860_NVRAM, "lan_ipaddr");
		if (lan_if_ip == NULL)
			lan_if_ip = "192.168.1.1";
	}

	char_t *http_port = nvram_get(RT2860_NVRAM, "RemoteManagementPort");

	websHeader(wp);
	if (strcmp(http_port, "80") == 0)
	{
		websWrite
		(
			wp,
			T(
				"<script language=\"JavaScript\" type=\"text/javascript\">\n"
				"ajaxReloadDelayedPage(%ld, \"http://%s%s\");\n"
				"</script>"),
			delay, lan_if_ip, url
		);
	} else {
		websWrite
		(
			wp,
			T(
				"<script language=\"JavaScript\" type=\"text/javascript\">\n"
				"ajaxReloadDelayedPage(%ld, \"http://%s:%s%s\");\n"
				"</script>"),
			delay, lan_if_ip, http_port, url
		);
	}
	websFooter(wp);
	websDone(wp, 200);
}
