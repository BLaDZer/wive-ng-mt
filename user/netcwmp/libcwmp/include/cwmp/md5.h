/* MD5.H - header file for MD5C.C */


/*  Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
    rights reserved.

    License to copy and use this software is granted provided that it
    is identified as the &quot;RSA Data Security, Inc. MD5 Message-Digest
    Algorithm&quot; in all material mentioning or referencing this software
    or this function.

    License is also granted to make and use derivative works provided
    that such works are identified as &quot;derived from the RSA Data
    Security, Inc. MD5 Message-Digest Algorithm&quot; in all material
    mentioning or referencing the derived work.

    RSA Data Security, Inc. makes no representations concerning either
    the merchantability of this software or the suitability of this
    software for any particular purpose. It is provided &quot;as is&quot;
    without express or implied warranty of any kind.

    These notices must be retained in any copies of any part of this
    documentation and/or software.
*/
#ifndef __MD5_H__
#define __MD5_H__

#ifndef PROTOTYPES
#define PROTOTYPES 1
#endif

/* POINTER defines a generic pointer type */
typedef unsigned char *POINTER;

/* UINT2 defines a two byte word */
typedef unsigned short int UINT2;

/* UINT4 defines a four byte word */
typedef unsigned long int UINT4;

/* PROTO_LIST is defined depending on how PROTOTYPES is defined above.
   If using PROTOTYPES, then PROTO_LIST returns the list, otherwise it
   returns an empty list.
 */

#if PROTOTYPES
#define PROTO_LIST(list) list
#else
#define PROTO_LIST(list) ()
#endif

#define HASHLEN     16
#define HASHHEXLEN  32


/* MD5 context. */
typedef struct
{
    UINT4 state[4];                                   /* state (ABCD) */
    UINT4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
    unsigned char buffer[64];                         /* input buffer */

} MD5_CTX;

void MD5Init PROTO_LIST ((MD5_CTX *));
void MD5Update PROTO_LIST ((MD5_CTX *, unsigned char *, unsigned int));
void MD5Final PROTO_LIST ((unsigned char [16], MD5_CTX *));


#endif
