/*
 * 	ralink_esw.c -- Ralink RT3052 Embedded Fast-Ethernet Switch MIB Support
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
*/

#include	<sys/param.h>
#include	<sys/types.h>
#include	<sys/time.h>
#include	<stdio.h>
#include	<netdb.h>
#include	<unistd.h>
#include 	<sys/file.h>

#include	"ctypes.h"
#include	"local.h"
#include	"debug.h"
#include	"miv.h"
#include	"asn.h"
#include	"mix.h"
#include	"ralink_esw.h"

//static	CUnslType	eswSnmp;

static AsnIdType eswRetrieve(CIntfType item)
{
	AsnIdType asnresult;
	unsigned long result = 0;
	struct port_counts pcs;
	
	if (portscounts(&pcs) != 0)
	{
		printf("eswRetrieve() error retrieving counters!\n");
		return 0;
	}

	int esw_count = item-1;

	if (esw_count >= RALINKESW_PORT0TXCNT)
	{
	    result = pcs.tx_count[esw_count-RALINKESW_PORT0TXCNT];
	}
	else
	{
	    result = pcs.rx_count[esw_count-RALINKESW_PORT0RXCNT];
	}

	asnresult = asnUnsl (asnClassApplication, (AsnTagType) 1, result);
	return (asnresult);
}

static	MixStatusType	eswRelease(MixCookieType cookie)
{
	cookie = cookie;
	return (smpErrorGeneric);
}

static	MixStatusType	eswCreate(MixCookieType cookie, MixNamePtrType name, MixLengthType namelen, AsnIdType asn)
{
	cookie = cookie;
	name = name;
	namelen = namelen;
	asn = asn;
	return (smpErrorGeneric);
}

static	MixStatusType	eswDestroy(MixCookieType cookie, MixNamePtrType name, MixLengthType namelen)
{
	cookie = cookie;
	name = name;
	namelen = namelen;
	return (smpErrorGeneric);
}

static	AsnIdType	eswNext(MixCookieType cookie, MixNamePtrType name, MixLengthPtrType namelenp)
{
	CIntfType		item;

	cookie = cookie;
	if (*namelenp == (MixLengthType) 0) {
		*namelenp = (MixLengthType) 2;
		*name++ = (MixNameType) 1;
		*name = (MixNameType) 0;
		return (eswRetrieve ((CIntfType) 1));
	} else if (*namelenp == (MixLengthType) 1) {
		if ((item = (CIntfType) *name) <= (CIntfType) (ESW_MAXTYPE+1)) {
			*namelenp = (MixLengthType) 2;
			*(++name) = (MixNameType) 0;
			return (eswRetrieve (item));
		} else {
			return ((AsnIdType) 0);
		}	/* if - else  */
	}	/* if- else - if */
	else if ((item = (CIntfType) *name) < (CIntfType) (ESW_MAXTYPE+1)) {
		*namelenp = (MixLengthType) 2;
		*name++ = (MixNameType) (++item);

		return (eswRetrieve (item));
	} else {
		return ((AsnIdType) 0);
	}	/* if -else if -else */
}

static	AsnIdType  eswGet(MixCookieType cookie, MixNamePtrType name, MixLengthType namelen)
{
	CIntfType		item;

	cookie = cookie;
	if ((namelen != (MixLengthType) 2) || ((item = (CIntfType) *name) < (CIntfType) 1) || (item > (CIntfType) (ESW_MAXTYPE+1)) || (*(name + 1) != (MixNameType) 0)) {
			return ((AsnIdType) 0);
	} else {
		return (eswRetrieve (item));
	}
}

static	MixStatusType	eswSet(MixCookieType cookie, MixNamePtrType name, MixLengthType namelen, AsnIdType asn)
{
	cookie = cookie;
	name = name;
	namelen = namelen;
	asn = asn;
	return (smpErrorReadOnly);
}

static MixOpsType	ralink_eswOps = {
	eswRelease,
	eswCreate,
	eswDestroy,
	eswNext,
	eswGet,
	eswSet
};

CVoidType ralink_esw_init(void)
{
//	int esw_count;
//	unsigned long	result;
//	struct port_counts pcs;
/*	
	if (portscounts(&pcs) != 0)
	{
		printf("ralink_esw_init() error retrieving counters! \n");
		return 0;
	}

	result = 0;

	for (esw_count = RALINKESW_PORT0RXCNT; esw_count <= RALINKESW_PORT5TXCNT; esw_count++)
	{
	    if (esw_count >= RALINKESW_PORT0TXCNT)
	    {
		    result = pcs.tx_count[esw_count-RALINK_PORT0TXCNT];
	    }
	    else
	    {
		    result = pcs.rx_count[esw_count-RALINK_PORT0RXCNT];
	    }
*/
//	     eswSnmp = (CUnslType)result;
	(void) misExport ((MixNamePtrType) ESW_OID, (MixLengthType) 6, &ralink_eswOps, (MixCookieType) 0);
//	} /* for */


}
