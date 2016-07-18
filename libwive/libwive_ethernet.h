#ifndef _WIVE_ETHERNET_H
#define _WIVE_ETHERNET_H 	1

int linkspeed(const char *ifname, int sd);
int linkduplex(const char *ifname, int sd);
int linkstatus(const char *ifname, int sd);

#endif
