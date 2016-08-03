#ifndef _WIVE_ETHERNET_H
#define _WIVE_ETHERNET_H 	1

#if defined (CONFIG_RALINK_MT7620)
#define PROCREG_DIR			"mt7620"
#elif defined (CONFIG_RALINK_MT7621)
#define PROCREG_DIR			"mt7621"
#elif defined (CONFIG_RALINK_MT7628)
#define PROCREG_DIR			"mt7628"
#else
#define PROCREG_DIR			"mt7620"
#endif

#ifdef CONFIG_RAETH_SNMPD
#define PROCREG_SNMP			("/proc/" PROCREG_DIR "/snmp")
#endif
#define PROCREG_GMAC		("/proc/" PROCREG_DIR "/gmac")
#define IOCTL_IF "eth2"

struct port_status {
	int portnum;	// port number
	int link;	// link status
	int speed;	// link speed
	int duplex;	// link duplex
};

int linkspeed(const char *ifname, int sd);
int linkduplex(const char *ifname, int sd);
int linkstatus(const char *ifname, int sd);
void portstatus(struct port_status *st, int portnm);
#endif
