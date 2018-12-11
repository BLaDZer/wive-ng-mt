#include "asp_mod_helpers.h"

/*
    Standard HTTP error codes
 */
websErrorType websErrors[] = {
    { 200, T("OK") },
    { 201, T("Created") },
    { 204, T("No Content") },
    { 205, T("Reset Content") },
    { 206, T("Partial Content") },
    { 301, T("Redirect") },
    { 302, T("Redirect") },
    { 304, T("Not Modified") },
    { 400, T("Bad Request") },
    { 401, T("Unauthorized") },
    { 402, T("Payment required") },
    { 403, T("Forbidden") },
    { 404, T("Not Found") },
    { 405, T("Access Denied") },
    { 406, T("Not Acceptable") },
    { 408, T("Request Timeout") },
    { 413, T("Request too large") },
    { 500, T("Internal Server Error") },
    { 501, T("Not Implemented") },
    { 503, T("Service Unavailable") },
    { 0, NULL }
};

/******************************************************************************/
/*
 *	Return the error message for a given code
 */
char_t *websErrorMsg(int code)
{
    websErrorType *ep;

    if (code < 0)
    {
        return websErrorMsg(500);
    }
//    a_assert(code >= 0);

    for (ep = websErrors; ep->code; ep++) {
        if (code == ep->code) {
            return ep->msg;
        }
    }
    return websErrorMsg(500); /* if not find - use internal server error */
}


void websError(webs_t* wp, int code, char_t *fmt, ...)
{
    va_list	args;
    char_t	*msg, *userMsg;/*, *buf;*/
//    char_t*     safeUrl = NULL;

//    char* url = ngx_to_cstring(wp->pool, wp->request->uri);

    if (fmt == NULL)
    {
        syslog(LOG_ERR, "fmt is NULL , %s", __FUNCTION__);
        return;
    }

//    websStats.errors++;

   /* remove any dangerous characters in the url, and replace the string in the 
    * wp structure. The webs_t cleanup code will free this memory for us.
    */
    // FIXME: make url safe
//    safeUrl = websSafeUrl(wp->url);
//    wp->url = safeUrl;

    userMsg = (char*)ngx_pcalloc(wp->pool, 1024);
    if (userMsg == NULL) {
        syslog(LOG_ERR, "error buffer allocation , %s", __FUNCTION__);
        return;
    }

    va_start(args, fmt);
    vsnprintf(userMsg, 1024, fmt, args);
    userMsg[1023] = '\0';

    va_end(args);
//    safeMsg = websSafeUrl(userMsg);
//    bfree(B_L, userMsg);
//    userMsg = safeMsg;
//    safeMsg  = NULL;


	msg = T("<html><head><title>Document Error: %s</title></head>\r\n\
		<body><h2>Access Error: %s</h2>\r\n\
		<p>%s</p></body></html>\r\n");
/*
 *	Ensure we have plenty of room
 */

//	buf = NULL;
//        buf = (char*)ngx_pcalloc(wp->pool, 1024);
//        snprintf(buf, 1024, msg, websErrorMsg(code), websErrorMsg(code), userMsg);

        // body cleanup
        ngx_array_destroy(wp->out);
        wp->out = ngx_array_create(wp->pool, 128, sizeof(char));

        websSetContentType(wp, "text/html");
        websWrite(wp, msg, websErrorMsg(code), websErrorMsg(code), userMsg);
//	fmtAlloc(&buf, WEBS_BUFSIZE, msg, websErrorMsg(code), websErrorMsg(code), userMsg);

//	if (buf == NULL) {
//		syslog(LOG_ERR, "error buffer allocation , %s", __FUNCTION__);
//                ngx_pfree(wp->pool, userMsg);
//		return;
//	}

//	websResponse(wp, code, buf, NULL);

        websDone(wp, code);

//        ngx_pfree(wp->pool, buf);
        ngx_pfree(wp->pool, userMsg);
//	bfree(B_L, buf);
//	bfree(B_L, userMsg);
}



void websLongWrite(webs_t* wp, char *lonstr)
{
    char tmp[513] = {0};
    int len = strlen(lonstr);
    char *end = lonstr + len;

    while(lonstr < end){
        strncpy(tmp, lonstr, 512);
        websWrite(wp, T("%s"), tmp);
        lonstr += 512;
    }
    return;
}

void websSetContentType(webs_t* wp, char* content_type)
{
    wp->request->headers_out.content_type = cstring_to_ngx(wp->pool, content_type);
}



/*
 * description: parse va and do system
 */
/*int doSystem(char_t *fmt, ...)
{
    va_list vargs;
    char_t *cmd = NULL;
    int	rc = 0;

    va_start(vargs, fmt);
    if (fmtValloc(&cmd, WEBS_BUFSIZE, fmt, vargs) >= WEBS_BUFSIZE) {
        syslog(LOG_ERR, "lost data, buffer overflow , %s", __FUNCTION__);
        va_end(vargs);
        return -1;
    }

    if (cmd == NULL) {
        syslog(LOG_ERR, "error buffer allocation , %s", __FUNCTION__);
        va_end(vargs);
        return -1;
    }

    va_end(vargs);

    rc = system(cmd);
    bfree(B_L, cmd);

    //
    // The system command itself failed
    //
    if (rc == -1)
        syslog(LOG_ERR, "error system() call , %s", __FUNCTION__);

    return rc;
}
*/

char_t 	*websGetVar(webs_t* wp, char_t *var, char_t *def)
{
    if (wp->args == NULL) return def; // not populated

    unsigned int i;

    keyval_t* elems = wp->args->elts;

    for (i=0;i<wp->args->nelts;i++)
    {
        keyval_t ptr = elems[i];

        if (strcmp(ptr.key, var) == 0)
        {
            return ptr.val;
        }
    }

    return def;
}

void setupParameters(webs_t* wp, parameter_fetch_t *fetch, int transaction)
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

		if (ngx_nvram_bufset(wp, fetch->nvram_param, (void *)str)!=0) //!!!
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

int aspDefineFunc(char* func_name, asp_func_proto func_ptr, enum UserPermission perm)
{
    asp_func_t* arr_ptr = (asp_func_t*)ngx_array_push(asp_func_array);
    if (arr_ptr == NULL) return NGX_ERROR;

    arr_ptr->name = func_name;
    arr_ptr->ptr = func_ptr;
    arr_ptr->perm = perm;

    return NGX_OK;
}

int websFormDefine(char* func_name, asp_form_proto form_ptr, enum UserPermission perm)
{
    asp_form_t* arr_ptr = (asp_form_t*)ngx_array_push(asp_form_array);
    if (arr_ptr == NULL) return NGX_ERROR;

    arr_ptr->name = func_name;
    arr_ptr->ptr = form_ptr;
    arr_ptr->perm = perm;

    return NGX_OK;
}


void outputTimerForReload(webs_t* wp, long delay)
{
        outWrite("<script language=\"JavaScript\" type=\"text/javascript\">\n"
                    "ajaxReloadDelayedPage(%ld);\n"
                    "</script>",
                    delay);

        websDone(wp, 200);
}


int websDone(webs_t* wp, int code)
{
    wp->request->headers_out.status = code;
    return 0;
}


int websAddHeader(webs_t* wp, char* key, char* value)
{
        ngx_http_request_t* r = wp->request;
        ngx_list_t* header_list = &r->headers_out.headers;

        if (header_list == NULL)
        {
            return NGX_ERROR;
        }

        ngx_table_elt_t* elt = ngx_list_push(header_list);

        if (elt == NULL) {
            return NGX_ERROR;
        }

//        ngx_str_set(elt->key, key);
        elt->key = cstring_to_ngx(wp->pool, key);
        elt->value = cstring_to_ngx(wp->pool, value);
        elt->hash = 1;

        elt->lowcase_key = ngx_pnalloc(wp->pool,elt->key.len);
        if (elt->lowcase_key == NULL) {
            return NGX_ERROR;
        }

        ngx_strlow(elt->lowcase_key, elt->key.data, elt->key.len);

    return NGX_OK;
}

static int redirect_inner(webs_t* wp, char_t *url)
{
        ngx_http_request_t* r = wp->request;
        ngx_http_clear_location(r);

        r->headers_out.location = ngx_list_push(&r->headers_out.headers);

        if (r->headers_out.location == NULL) {
            return NGX_ERROR;
        }

        r->headers_out.location->hash = 1;
        ngx_str_set(&r->headers_out.location->key, "Location");

/*        int len = strlen(url)+1;
        p = ngx_pnalloc(r->pool, len);

        if (p == NULL) {
            ngx_http_clear_location(r);
            return NGX_ERROR;
        }
*/
        r->headers_out.location->value = cstring_to_ngx(wp->pool, url);

        websDone(wp, 302);

        return NGX_OK;
}

int websRedirect(webs_t* wp, char_t *url)
{
    ngx_http_core_srv_conf_t  *cscf;
    char_t *urlbuf, *redirectFmt;
    char* websHostUrl = NULL;

    ngx_table_elt_t* hostTable = wp->request->headers_in.host;
    if (hostTable != NULL)
    {
        websHostUrl = ngx_pcalloc(wp->pool, hostTable->value.len+1);
        memcpy(websHostUrl, hostTable->value.data, hostTable->value.len);
    }
    else
    {
        cscf = ngx_http_get_module_srv_conf(wp->request, ngx_http_core_module);

        websHostUrl = ngx_pcalloc(wp->pool, cscf->server_name.len+1);
        memcpy(websHostUrl, cscf->server_name.data, cscf->server_name.len);
    }

//   websStats.redirects++;
    urlbuf = NULL;

    if (!url || !url[0]) {
        syslog(LOG_ERR, "redirect url isn't set - PLS fix it, %s", __FUNCTION__);
        return NGX_ERROR;
    }

/*
 *      Some browsers require a http://host qualified URL for redirection
 */
    if (strstr(url, T("http://")) == NULL) {
        if (*url == '/') {
            url++;
        }

        redirectFmt = T("http://%s/%s");

#ifdef NGX_HTTP_SSL
        if (wp->request->connection->ssl != 0) {
            redirectFmt = T("https://%s/%s");
        }
#endif

        urlbuf = ngx_pcalloc(wp->pool, 4096 + 80);
        snprintf(urlbuf, 4096+80-1, redirectFmt, websGetVar(wp, T("HTTP_HOST"), websHostUrl), url);

        url = urlbuf;
    }

/*
 * Add human readable message for completeness. Should not be required.
 */
    websSetContentType(wp, "text/html");
    outWrite("<html><head></head><body>\r\n\
        This document has moved to a new <a href=\"%s\">location</a>.\r\n\
        Please update your documents to reflect the new location.\r\n\
        </body></html>\r\n", url);

//      websResponse(wp, 302, msgbuf, url);

    if (redirect_inner(wp, url) != NGX_OK)
    {
        websDone(wp, NGX_HTTP_INTERNAL_SERVER_ERROR);
        return NGX_ERROR;
    }

    websDone(wp, 302);

    return NGX_OK;
}

void websHeader(webs_t* wp)
{
    websSetContentType(wp, "text/html");

//	a_assert(websValid(wp));

	websWrite(wp, T("<html>\n<head>\r\n"));
	websWrite(wp, T("<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">\r\n"));
	char_t line[256];
	FILE *fd;
	// Copy /style/normal_ws.css
	if ((fd = fopen("/web/style/normal_ws.css", "r")) != NULL) {
		websWrite(wp, T("<style type=\"text/css\">\r\n"));
		while ((fgets(line, sizeof(line), fd)) != NULL)
			websWrite(wp, T("%s\n"), line);
		websWrite(wp, T("</style>\r\n"));
		fclose(fd);
	} else {
		websWrite(wp, T("<link rel=\"stylesheet\" href=\"/style/normal_ws.css\" type=\"text/css\">\n"));
	}
	// Copy /js/ajax.js
	if ((fd = fopen("/web/js/ajax.js", "r")) != NULL) {
		websWrite(wp, T("<script type=\"text/javascript\">\n"));
		websWrite(wp, T("// Here is script copied from file /js/ajax.js\n"));
		while ((fgets(line, sizeof(line), fd)) != NULL)
			websWrite(wp, T("%s\n"), line);
		fclose(fd);
	} else {
		websWrite(wp, T("<script type=\"text/javascript\" src=\"/js/ajax.js\">\n"));
	}
	websWrite(wp, T("</script>\r\n"));
	websWrite(wp, T("</head>\n<body>\r\n"));
}

/******************************************************************************/
/*
 *	Write a webs footer
 */

void websFooter(webs_t* wp)
{
//	a_assert(websValid(wp));

	websWrite(wp, "</body>\n</html>\n");
}

/*
int getBasicAuthCredentials(webs_t* wp, char** username, char** password)
{
    ngx_str_t prefix = ngx_string("Basic ");
    ngx_table_elt_t* auth_el = wp->request->headers_in.authorization;
    ngx_str_t auth_line;

    if (auth_el == NULL)
    {
        return 1;
    }

    auth_line = auth_el->value;

    ngx_str_t pos = n_strstr(auth_line, prefix);

    if (auth_line.data == NULL )
    {
        return 2;
    }

    if (pos.data != auth_line.data)
    {
        return 3;
    }

    auth_line = n_skipchar(auth_line, 6);

    auth_line = n_base64_decode(wp->pool, auth_line);
    if (auth_line.len == 0)
    {
        return 4;
    }

    ngx_str_t sep = ngx_string(":");
    ngx_str_t ngx_password = ngx_split_inplace(auth_line, sep);


    if (username == NULL || password == NULL)
    {
        return 5;
    }

    *username = ngx_to_cstring(wp->pool, auth_line);
    *password = ngx_to_cstring(wp->pool, ngx_password);

    return 0;
}
*/
