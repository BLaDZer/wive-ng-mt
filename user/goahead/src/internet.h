/*
 *	internet.h -- Internet Configuration Header
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: internet.h,v 1.8 2008-03-17 07:47:16 yy Exp $
 */

#ifdef CONFIG_RAETH_GMAC2		/* dual phy/rgmii mode */
#define WAN_DEF "eth3"
#else
#define WAN_DEF "eth2.2"		/* internal ralink esw with vlan parted mode */
#endif

#define BR_SIG	"br"
#define ETH_SIG	"eth"
#define VPN_SIG	"ppp"
#define VPN_DEF "ppp0"

#define IPT_SHORT_ACCOUNT

#define _PATH_PROCNET_DEV		"/proc/net/dev"
#define _PATH_IPT_ACCOUNTING_FILE	"/proc/net/ipt_account/mynetwork"

#define _PATH_PPP_ROUTES		"/etc/routes_ppp_replace"
#define _PATH_LANWAN_ROUTES		"/etc/routes_replace"

#define _PATH_PFW			"/etc"
#define _PATH_PFW_FILE			_PATH_PFW "/portforward"
#define _PATH_PFW_FILE_VPN		_PATH_PFW "/portforward_vpn"
#define _PATH_MACIP_FILE		_PATH_PFW "/macipfilter"
#define _PATH_WEBS_FILE			_PATH_PFW "/websfilter"

#ifdef CONFIG_IPV6
#define _PATH_PROCNET_IPV6		"/proc/net/if_inet6"

#define SIXRD_SIG	"6rd"
#define SIX2FOUR_SIG	"sit"

#define IPV6_ADDR_GLOBAL        0x0000U
#define IPV6_ADDR_LOOPBACK      0x0010U
#define IPV6_ADDR_LINKLOCAL     0x0020U
#define IPV6_ADDR_SITELOCAL     0x0040U
#define IPV6_ADDR_COMPATv4      0x0080U
#endif

void formDefineInternet(void);
void initInternet(void);
char* getLanIfName(void);
char* getWanIfName(void);
int getIfIp(char *ifname, char *if_addr);
int getIfMac(char *ifname, char *if_hw);
