/*
 * 
 * Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 */
#ifndef	RALINK_ESW_MIB
#define	RALINK_ESW_MIB

#include <linux/autoconf.h>
#include <libwive.h>

CVoidType ralink_esw_init(void);

#define ESW_MAXTYPE	11
#define ESW_OID		"\53\6\1\4\1\1107\1\1\1\1"

#define	RALINKESW_PORT0RXCNT	0
#define	RALINKESW_PORT1RXCNT	1
#define	RALINKESW_PORT2RXCNT	2
#define	RALINKESW_PORT3RXCNT	3
#define	RALINKESW_PORT4RXCNT	4
#define	RALINKESW_PORT5RXCNT	5

#define	RALINKESW_PORT0TXCNT	6
#define	RALINKESW_PORT1TXCNT	7
#define	RALINKESW_PORT2TXCNT	8
#define	RALINKESW_PORT3TXCNT	9
#define	RALINKESW_PORT4TXCNT	10
#define	RALINKESW_PORT5TXCNT	11
#endif
