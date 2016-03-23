/*
 * value.c -- Generic type (holds all types)
 *
 * Copyright (c) GoAhead Software Inc., 1995-2000. All Rights Reserved.
 *
 * $Id: value.c,v 1.3 2007-02-01 07:41:01 winfred Exp $
 */

/******************************** Description *********************************/

/*
 *	This module provides a generic type that can hold all possible types.
 *	It is designed to provide maximum effeciency.
 */

/********************************* Includes ***********************************/
#include	"uemf.h"
/*********************************** Code *************************************/
/*
 *	Initialize a integer value.
 */

value_t valueInteger(long value)
{
	value_t	v;

	memset(&v, 0x0, sizeof(v));
	v.valid = 1;
	v.type = tinteger;
	v.value.tinteger = value;
	return v;
}

/******************************************************************************/
/*
 *	Initialize a string value.
 */

value_t valueString(char_t* value, int flags)
{
	value_t	v;

	memset(&v, 0x0, sizeof(v));
	v.valid = 1;
	v.type = tstring;
	if (flags & VALUE_ALLOCATE) {
		v.allocated = 1;
		v.value.tstring = bstrdup(B_L, value);
	} else {
		v.allocated = 0;
		v.value.tstring = value;
	}
	return v;
}

/******************************************************************************/
/*
 *	Free any storage allocated for a value.
 */

void valueFree(value_t* v)
{
	if (v->valid && v->allocated && v->type == tstring &&
			v->value.tstring != NULL) {
		bfree(B_L, v->value.tstring);
	}
	v->type = tundefined;
	v->valid = 0;
	v->allocated = 0;
}
