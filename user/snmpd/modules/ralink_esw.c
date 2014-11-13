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


static	CUnslType	eswSnmp;

/* need rewritw to 64bit support */
struct ralink_esw_mib
{
	/* rx counters */
	unsigned long	port0_rx_counter;	/* ralink esw mib - ralinksnmp 1 */
	unsigned long	port1_rx_counter;	/* ralink esw mib - ralinksnmp 2 */
	unsigned long	port2_rx_counter;	/* ralink esw mib - ralinksnmp 3 */
	unsigned long	port3_rx_counter;	/* ralink esw mib - ralinksnmp 4 */
	unsigned long	port4_rx_counter;	/* ralink esw mib - ralinksnmp 5 */
	unsigned long	port5_rx_counter;	/* ralink esw mib - ralinksnmp 6 */

	/* tx counters */
	unsigned long	port0_tx_counter;	/* ralink esw mib - ralinksnmp 7 */
	unsigned long	port1_tx_counter;	/* ralink esw mib - ralinksnmp 8 */
	unsigned long	port2_tx_counter;	/* ralink esw mib - ralinksnmp 9 */
	unsigned long	port3_tx_counter;	/* ralink esw mib - ralinksnmp 10 */
	unsigned long	port4_tx_counter;	/* ralink esw mib - ralinksnmp 11 */
	unsigned long	port5_tx_counter;	/* ralink esw mib - ralinksnmp 12 */
};

static AsnIdType eswRetrieve(CIntfType item)
{
	AsnIdType 	asnresult;
	unsigned long 	result=0;
	struct ralink_esw_mib esw_stat;
	FILE 		*in;
	char		line[1024];

	in = fopen(PROCREG_SNMP, "r");

	if (!in)
	{
		printf("eswRetrieve() error opening proc file, please insmod reath module!\n");
		return 0;
	}

	while ( line == fgets(line, 1024, in)) {
		if ( 6 == sscanf(line, "rx counters: %lu %lu %lu %lu %lu %lu\n", &esw_stat.port0_rx_counter, &esw_stat.port1_rx_counter, &esw_stat.port2_rx_counter, &esw_stat.port3_rx_counter, &esw_stat.port4_rx_counter, &esw_stat.port5_rx_counter))
			continue;
		if ( 6 == sscanf(line, "tx counters: %lu %lu %lu %lu %lu %lu\n", &esw_stat.port0_tx_counter, &esw_stat.port1_tx_counter, &esw_stat.port2_tx_counter, &esw_stat.port3_tx_counter, &esw_stat.port4_tx_counter, &esw_stat.port5_tx_counter))
			break;
	}
	fclose(in);

	switch ( item-1 ) {
		case RALINKESW_PORT0RXCNT:
			result = esw_stat.port0_rx_counter;
			break;
		case RALINKESW_PORT1RXCNT:
			result = esw_stat.port1_rx_counter;
			break;
		case RALINKESW_PORT2RXCNT:
			result = esw_stat.port2_rx_counter;
			break;
		case RALINKESW_PORT3RXCNT:
			result = esw_stat.port3_rx_counter;
			break;
		case RALINKESW_PORT4RXCNT:
			result = esw_stat.port4_rx_counter;
			break;
		case RALINKESW_PORT5RXCNT:
			result = esw_stat.port5_rx_counter;
			break;
		case RALINKESW_PORT0TXCNT:
			result = esw_stat.port0_tx_counter;
			break;
		case RALINKESW_PORT1TXCNT:
			result = esw_stat.port1_tx_counter;
			break;
		case RALINKESW_PORT2TXCNT:
			result = esw_stat.port2_tx_counter;
			break;
		case RALINKESW_PORT3TXCNT:
			result = esw_stat.port3_tx_counter;
			break;
		case RALINKESW_PORT4TXCNT:
			result = esw_stat.port4_tx_counter;
			break;
		case RALINKESW_PORT5TXCNT:
			result = esw_stat.port5_tx_counter;
			break;
		default:
			break;
	}	/* switch-case */


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
	int esw_count;
	unsigned long	result;
	FILE	*in;
	struct 		ralink_esw_mib esw_stat;
	char		line[1024];

	in = fopen(PROCREG_SNMP,"r");

	if(!in) {
		printf("proc open failed!\n");
		return;
	}

	while ( line == fgets(line, 1024, in)) {
		if ( 6 == sscanf(line, "rx counters: %lu %lu %lu %lu %lu %lu\n", &esw_stat.port0_rx_counter, &esw_stat.port1_rx_counter, &esw_stat.port2_rx_counter, &esw_stat.port3_rx_counter, &esw_stat.port4_rx_counter, &esw_stat.port5_rx_counter))
			continue;
		if ( 6 == sscanf(line, "tx counters: %lu %lu %lu %lu %lu %lu\n", &esw_stat.port0_tx_counter, &esw_stat.port1_tx_counter, &esw_stat.port2_tx_counter, &esw_stat.port3_tx_counter, &esw_stat.port4_tx_counter, &esw_stat.port5_tx_counter))
			break;
	}
	fclose(in);

	for (esw_count = 0; esw_count <= RALINKESW_PORT5TXCNT; esw_count++)
	{
		switch(esw_count) {
			case RALINKESW_PORT0RXCNT:
				result = esw_stat.port0_rx_counter;
				break;
			case RALINKESW_PORT1RXCNT:
				result = esw_stat.port1_rx_counter;
				break;
			case RALINKESW_PORT2RXCNT:
				result = esw_stat.port2_rx_counter;
				break;
			case RALINKESW_PORT3RXCNT:
				result = esw_stat.port3_rx_counter;
				break;
			case RALINKESW_PORT4RXCNT:
				result = esw_stat.port4_rx_counter;
				break;
			case RALINKESW_PORT5RXCNT:
				result = esw_stat.port5_rx_counter;
				break;
			case RALINKESW_PORT0TXCNT:
				result = esw_stat.port0_tx_counter;
				break;
			case RALINKESW_PORT1TXCNT:
				result = esw_stat.port1_tx_counter;
				break;
			case RALINKESW_PORT2TXCNT:
				result = esw_stat.port2_tx_counter;
				break;
			case RALINKESW_PORT3TXCNT:
				result = esw_stat.port3_tx_counter;
				break;
			case RALINKESW_PORT4TXCNT:
				result = esw_stat.port4_tx_counter;
				break;
			case RALINKESW_PORT5TXCNT:
				result = esw_stat.port5_tx_counter;
				break;
			default:
				break;
		}	/* switch-case */
			 eswSnmp = (CUnslType)result;
			 (void) misExport ((MixNamePtrType) ESW_OID,
			 	(MixLengthType) 6, &ralink_eswOps, (MixCookieType) 0);
	}	/* for */
}
