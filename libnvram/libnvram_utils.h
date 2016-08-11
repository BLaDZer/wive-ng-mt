#ifndef _NVRAM_UTIL_H
#define _NVRAM_UTIL_H 	1

#include <linux/autoconf.h>

/* utils for goahead, cli, nvram, netcwmpd */

#define _PATH_PPP_ROUTES		"/etc/routes_ppp_replace"
#define _PATH_LANWAN_ROUTES		"/etc/routes_replace"

#define _PATH_PFW			"/etc"
#define _PATH_PFW_FILE			_PATH_PFW "/portforward"
#define _PATH_PFW_FILE_VPN		_PATH_PFW "/portforward_vpn"
#define _PATH_PFW_FILE_ACCESS		_PATH_PFW "/portforward_access"
#define _PATH_MACIP_FILE		_PATH_PFW "/macipfilter"
#define _PATH_WEBS_FILE			_PATH_PFW "/websfilter"

#define WEB_FILTER_CHAIN		"web_filter"
#define IPPORT_FILTER_CHAIN		"macipport_filter"

#define PORT_FORWARD_PRE_CHAIN		"port_forward_pre"
#define PORT_FORWARD_POST_CHAIN		"port_forward_post"

#define PORT_FORWARD_PRE_CHAIN_VPN	"port_forward_pre_vpn"
#define PORT_FORWARD_POST_CHAIN_VPN	"port_forward_post_vpn"

#define PROTO_UNKNOWN			0
#define PROTO_TCP			1
#define PROTO_UDP			2
#define PROTO_TCP_UDP			3
#define PROTO_ICMP			4
#define PROTO_NONE			5

#define ACTION_DROP			0
#define ACTION_ACCEPT			1

#define BR_SIG	"br"
#define ETH_SIG	"eth"
#define VPN_SIG	"ppp"
#define VPN_DEF "ppp0"

#ifdef CONFIG_RAETH_BOTH_GMAC		/* dual phy/rgmii mode */
#define WAN_DEF "eth3"
#else
#define WAN_DEF "eth2.2"		/* internal ralink esw with vlan parted mode */
#endif

char *strip_space(char *str);
int isMacValid(char *str);
int isIpValid(char *str);
int isIpNetmaskValid(char *s);

int getNums(char *value, char delimit);
int getNthValueSafe(int index, char *value, char delimit, char *result, int len);

int vpn_mode_enabled(void);
char* getLanIfName(void);
char* getWanIfName(void);
char *getLanWanNamebyIf(const char *ifname);
char* getPPPIfName(void);

void static_routing_rebuild_etc(void);
void firewall_rebuild_etc(void);
int gen_wifi_config(int mode, int genmode);

void STFs(int nvram, int index, char *flash_key, char *value);
char *getNthValue(int index, char *values);
char *setNthValue(int index, char *old_values, char *new_value);
int deleteNthValueMulti(int index[], int count, char *value, char delimit);

int nvram_load_default(void);
#endif
