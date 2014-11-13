/*
 *	firewall.h -- Firewall Configuration Header 
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: firewall.h,v 1.5 2008-04-14 09:26:00 yy Exp $
 */


#define PROTO_UNKNOWN	0
#define PROTO_TCP		1
#define PROTO_UDP		2
#define PROTO_TCP_UDP	3
#define PROTO_ICMP		4
#define PROTO_NONE		5


#define RULE_MODE_DISABLE	0
#define RULE_MODE_DROP		1
#define RULE_MODE_ACCEPT	2

#define ACTION_DROP		0
#define ACTION_ACCEPT	1

#define WEB_FILTER_CHAIN	"web_filter"
#define WEB_FILTER_PRE_CHAIN	"web_filter_preroute"
#define IPPORT_FILTER_CHAIN	"macipport_filter"
#define IPPORT_NAT_FILTER_CHAIN	"macipport_filter_preroute"

#define DMZ_CHAIN			"DMZ"

#define PORT_FORWARD_PRE_CHAIN	"port_forward_pre" //for clean forward
#define PORT_FORWARD_POST_CHAIN	"port_forward_post" //for reverse nat

#define PORT_FORWARD_PRE_CHAIN_VPN	"port_forward_pre_vpn" //for clean forward
#define PORT_FORWARD_POST_CHAIN_VPN	"port_forward_post_vpn" //for reverse nat


#define MAGIC_NUM			"IPCHAINS"

#define HTML_NO_FIREWALL_UNDER_BRIDGE_MODE	\
"<img src=\"/graphics/warning.gif\"><font color=#ff0000>&nbsp; &nbsp;Warning: The current operation mode is \"Bridge mode\" and these settings may not be functional.</font>"

void formDefineFirewall(void);
void firewall_rebuild_etc(void);
