#ifndef _WIVE_ETHERNET_H
#define _WIVE_ETHERNET_H 	1

#if defined(CONFIG_RALINK_RT3052)
#define PROCREG_DIR			"rt3052"
#elif defined(CONFIG_RALINK_RT3352)
#define PROCREG_DIR			"rt3352"
#elif defined (CONFIG_RALINK_RT5350)
#define PROCREG_DIR			"rt5350"
#elif defined(CONFIG_RALINK_RT3883)
#define PROCREG_DIR			"rt3883"
#elif defined (CONFIG_RALINK_MT7620)
#define PROCREG_DIR			"mt7620"
#elif defined (CONFIG_RALINK_MT7621)
#define PROCREG_DIR			"mt7621"
#elif defined (CONFIG_RALINK_MT7628)
#define PROCREG_DIR			"mt7628"
#else
#define PROCREG_DIR			"mt7620"
#endif

#define IOCTL_IF			"eth2"

#ifdef CONFIG_RAETH_SNMPD
#define PROCREG_SNMP			("/proc/" PROCREG_DIR "/snmp")
#endif

#define _PATH_PROCNET_DEV		"/proc/net/dev"

struct port_counts {
	unsigned long long rx_count[6];	// recive switch port count
	unsigned long long tx_count[6];	// transmit switch port count
};

struct nic_counts {
	char ifname[IFNAMSIZ];
	unsigned long long rx_bytes, 
	    rx_packets, 
    	    rx_errs, 
	    rx_drops, 
	    rx_fifo, 
	    rx_frame,
	    tx_bytes, 
	    tx_packets, 
	    tx_errs, 
	    tx_drops, 
	    tx_fifo, 
	    tx_colls, 
	    tx_carrier, 
	    rx_multi;

	int is_available;
};



int portscounts(struct port_counts *st);
struct nic_counts* nicscounts();


#if defined(CONFIG_ETHTOOL)
#define PROCREG_GMAC			("/proc/" PROCREG_DIR "/gmac")
struct port_status {
	int portnum;	// port number
	int link;	// link status
	int speed;	// link speed
	int duplex;	// link duplex
};

int linkspeed(const char *ifname, int sd);
int linkduplex(const char *ifname, int sd);
int linkstatus(const char *ifname, int sd);
int portstatus(struct port_status *st, int portnm);
#endif
#endif
