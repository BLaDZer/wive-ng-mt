/*
 *	firewall.h -- Firewall Configuration Header 
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: firewall.h,v 1.5 2008-04-14 09:26:00 yy Exp $
 */


#define PROTO_UNKNOWN		0
#define PROTO_TCP		1
#define PROTO_UDP		2
#define PROTO_TCP_UDP		3
#define PROTO_ICMP		4
#define PROTO_NONE		5

#define RULE_MODE_DISABLE	0
#define RULE_MODE_DROP		1
#define RULE_MODE_ACCEPT	2

#define ACTION_DROP		0
#define ACTION_ACCEPT		1

#define DMZ_CHAIN			"DMZ"

#define WEB_FILTER_CHAIN		"web_filter"
#define IPPORT_FILTER_CHAIN		"macipport_filter"

#define PORT_FORWARD_PRE_CHAIN		"port_forward_pre"
#define PORT_FORWARD_POST_CHAIN		"port_forward_post"

#define PORT_FORWARD_PRE_CHAIN_VPN	"port_forward_pre_vpn"
#define PORT_FORWARD_POST_CHAIN_VPN	"port_forward_post_vpn"

void formDefineFirewall(void);
void firewall_rebuild(void);
void firewall_rebuild_etc(void);
