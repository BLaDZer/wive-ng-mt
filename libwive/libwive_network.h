#ifndef _WIVE_NETWORK_H
#define _WIVE_NETWORK_H 	1

#ifdef CONFIG_IPV6
#define _PATH_PROCNET_IPV6		"/proc/net/if_inet6"

#define SIXRD_SIG			"6rd"
#define SIX2FOUR_SIG			"sit"

#define IPV6_ADDR_GLOBAL        	0x0000U
#define IPV6_ADDR_LOOPBACK      	0x0010U
#define IPV6_ADDR_LINKLOCAL     	0x0020U
#define IPV6_ADDR_SITELOCAL     	0x0040U
#define IPV6_ADDR_COMPATv4      	0x0080U
#endif

int getIfIsReady(const char *ifname);
int getIfIp(const char *ifname, char *if_addr);
int getIfMac(const char *ifname, char *if_hw);
int getIfNetmask(const char *ifname, char *if_net);

#ifdef CONFIG_IPV6
int getIfIPv6(const char *ifname, char *if_addr, char *netmask);
#endif

void arplookup(char *ip, char *arp);
#endif
