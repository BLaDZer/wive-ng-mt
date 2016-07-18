#ifndef _WIVE_NETWORK_H
#define _WIVE_NETWORK_H 	1

int getIfIp(const char *ifname, char *if_addr);
int getIfMac(const char *ifname, char *if_hw);
int getIfNetmask(const char *ifname, char *if_net);

void arplookup(char *ip, char *arp);
#endif
