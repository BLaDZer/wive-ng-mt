/*
 * form.c -- Form processing (in-memory CGI) for the GoAhead Web server
 *
 * Copyright (c) GoAhead Software Inc., 1995-2000. All Rights Reserved.
 *
 * See the file "license.txt" for usage and redistribution license requirements
 *
 * $Id: form.c,v 1.4.34.1 2010-02-03 11:11:48 chhung Exp $
 */

/********************************** Description *******************************/

/*
 *	This module implements the /goform handler. It emulates CGI processing
 *	but performs this in-process and not as an external process. This enables
 *	a very high performance implementation with easy parsing and decoding 
 *	of query strings and posted data.
 */

/*********************************** Includes *********************************/

#include	"wsIntrn.h"

/************************************ Locals **********************************/

static sym_fd_t	formSymtab = -1;			/* Symbol table for form handlers */

/************************************* Code ***********************************/
/*
 *	Process a form request. Returns 1 always to indicate it handled the URL
 */

int websFormHandler(webs_t wp, char_t *urlPrefix, char_t *webDir, int arg, 
	char_t *url, char_t *path, char_t *query)
{
	sym_t		*sp;
	char_t		formBuf[FNAMESIZE];
	char_t		*cp, *formName;
	int			(*fn)(void *sock, char_t *path, char_t *args);

	a_assert(websValid(wp));
	a_assert(url && *url);
	a_assert(path && *path == '/');

	websStats.formHits++;

/*
 *	Extract the form name
 */
	gstrncpy(formBuf, path, TSZ(formBuf));
	if ((formName = gstrchr(&formBuf[1], '/')) == NULL) {
		websError(wp, 200, T("Missing form name"));
		return 1;
	}
	formName++;
	if ((cp = gstrchr(formName, '/')) != NULL) {
		*cp = '\0';
	}

/*
 *	Lookup the C form function first and then try tcl (no javascript support 
 *	yet).
 */
	sp = symLookup(formSymtab, formName);
	if (sp == NULL) {
		websError(wp, 404, T("Form %s is not defined"), formName);
	} else {
		fn = (int (*)(void *, char_t *, char_t *)) sp->content.value.integer;
		a_assert(fn);
		if (fn) {
/*
 *			For good practice, forms must call websDone()
 */
			(*fn)((void*) wp, formName, query);
		}
	}
	return 1;
}

/******************************************************************************/
/*
 *	Define a form function in the "form" map space.
 */

int websFormDefine(char_t *name, void (*fn)(webs_t wp, char_t *path, 
	char_t *query))
{
	a_assert(name && *name);
	a_assert(fn);

	if (fn == NULL) {
		return -1;
	}

	symEnter(formSymtab, name, valueInteger((int) fn), (int) NULL);
	return 0;
}

/******************************************************************************/
/*
 *	Open the symbol table for forms.
 */

void websFormOpen()
{
	formSymtab = symOpen(WEBS_SYM_INIT);
}

/******************************************************************************/
/*
 *	Close the symbol table for forms.
 */

void websFormClose()
{
	if (formSymtab != -1) {
		symClose(formSymtab);
		formSymtab = -1;
	}
}

/******************************************************************************/
/*
 *	Write a webs header. This is a convenience routine to write a common
 *	header for a form back to the browser.
 */

void websHeader(webs_t wp)
{
	a_assert(websValid(wp));

	websWrite(wp, T("HTTP/1.0 200 OK\n"));
	websWrite(wp, WEBS_CACHE_CONTROL_STRING);
/*
 *	By license terms the following line of code must not be modified
 */
	websWrite(wp, T("Server: %s\r\n"), WEBS_NAME);
	websWrite(wp, T("Content-Type: text/html\n"));
	websWrite(wp, T("\n"));
	websWrite(wp, T("<html>\n<head>\n"));
	websWrite(wp, T("<title>My Title</title>\n"));
	websWrite(wp, T("<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">\n"));
	char_t line[256];
	FILE *fd;
	// Copy /style/normal_ws.css
	if ((fd = fopen("/web/style/normal_ws.css", "r")) != NULL) {
		websWrite(wp, T("<style type=\"text/css\">\n"));
		while (fgets(line, 255, fd)) {
			if (line == NULL)
			    continue;
			websWrite(wp, T("%s\n"), line);
		}
		websWrite(wp, T("</style>\n"));
	} else {
		websWrite(wp, T("<link rel=\"stylesheet\" href=\"/style/normal_ws.css\" type=\"text/css\">\n"));
	}
	fclose(fd);
	// Copy /js/ajax.js
	if ((fd = fopen("/web/js/ajax.js", "r")) != NULL) {
		websWrite(wp, T("<script type=\"text/javascript\">\n"));
		websWrite(wp, T("// Here is script copied from file /js/ajax.js\n"));
		while (fgets(line, 255, fd)) {
			if (line == NULL)
			    continue;
			websWrite(wp, T("%s\n"), line);
		}
	} else {
		websWrite(wp, T("<script type=\"text/javascript\" src=\"/js/ajax.js\">\n"));
	}
	fclose(fd);
	websWrite(wp, T("</script>\n"));
	websWrite(wp, T("</head>\n<body>\n"));
}

/******************************************************************************/
/*
 *	Write a webs footer
 */

void websFooter(webs_t wp)
{
	a_assert(websValid(wp));

	websWrite(wp, T("</body>\n</html>\n"));
}
