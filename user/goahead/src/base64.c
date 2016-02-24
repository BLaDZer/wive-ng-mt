/*
 * base64.c -- Base64 Mime encoding
 *
 * Copyright (c) GoAhead Software Inc., 1995-2000. All Rights Reserved.
 *
 * See the file "license.txt" for usage and redistribution license requirements
 *
 * $Id: base64.c,v 1.4 2009-10-15 09:05:28 winfred Exp $
 */

/******************************** Description *********************************/

/*
 *	The base64 command encodes and decodes a string in mime base64 format
 */

/********************************* Includes ***********************************/

#include	"wsIntrn.h"

/******************************** Local Data **********************************/
/*
 *	Mapping of ANSI chars to base64 Mime encoding alphabet (see below)
 */

static char_t	map64[] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
	-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, 
	-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};

/*********************************** Code *************************************/
/*
 *	Decode a buffer from "string" and into "outbuf"
 */

int websDecode64(char_t *outbuf, char_t *inpstring, int outlen)
{
	unsigned long	shiftbuf;
	char_t	*cp, *op;
	int c, i, j, shift;

	op = outbuf;
	*op = '\0';
	cp = inpstring;
	while (*cp && *cp != '=') {
/*
 *		Map 4 (6bit) input bytes and store in a single long (shiftbuf)
 */
		shiftbuf = 0;
		shift = 18;
		for (i = 0; i < 4 && *cp && *cp != '='; i++, cp++) {
			c = map64[*cp & 0xff];
			if (c == -1) {
				error(E_L, E_LOG, T("Bad inpstring: %s at %c index %d"), inpstring, c, i);
				return -1;
			}
			shiftbuf = shiftbuf | (c << shift);
			shift -= 6;
		}
/*
 *		Interpret as 3 normal 8 bit bytes (fill in reverse order).
 *		Check for potential buffer overflow before filling.
 */
		--i;
		if ((op + i) >= &outbuf[outlen]) {
			gstrcpy(outbuf, T("inpstring too big"));
			return -1;
		}
		for (j = 0; j < i; j++) {
			*op++ = (char_t) ((shiftbuf >> (8 * (2 - j))) & 0xff);
		}
		*op = '\0';
	}
	return 0;
}
