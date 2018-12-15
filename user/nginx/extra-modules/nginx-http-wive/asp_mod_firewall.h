/*
 *	firewall.h -- Firewall Configuration Header 
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: firewall.h,v 1.5 2008-04-14 09:26:00 yy Exp $
 */

#ifndef __ASP_MOD_FIREWALL_H__
#define __ASP_MOD_FIREWALL_H__

#include "asp_mod_helpers.h"

void asp_mod_firewall_init();
void firewall_rebuild(int flushconntrack);
#endif //__ASP_MOD_FIREWALL_H__
