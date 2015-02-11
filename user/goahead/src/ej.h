/*
 *	ej.h -- Ejscript(TM) header
 *
 * Copyright (c) GoAhead Software Inc., 1992-2000. All Rights Reserved.
 *
 *	See the file "license.txt" for information on usage and redistribution
 *
 * $Id: ej.h,v 1.3 2007-02-01 07:41:01 winfred Exp $
 */

#ifndef _h_EJ
#define _h_EJ 1

/******************************** Description *********************************/

/*
 *	GoAhead Ejscript(TM) header. This defines the Ejscript API and internal
 *	structures.
 */

/********************************* Includes ***********************************/
#include	"uemf.h"
/******************************** Prototypes **********************************/

extern int  ejArgs(int argc, char_t **argv, char_t *fmt, ...);
extern int  ejOpenEngine(sym_fd_t variables, sym_fd_t functions);
extern int  ejSetGlobalFunction(int eid, char_t *name, int (*fn)(int eid, void *handle, int argc, char_t **argv));
extern int  ejGetVar(int eid, char_t *var, char_t **value);
extern void ejSetResult(int eid, char_t *s);
extern void ejCloseEngine(int eid);
extern void ejSetVar(int eid, char_t *var, char_t *value);
extern char_t *ejEval(int eid, char_t *script, char_t **emsg);
#endif /* _h_EJ */
