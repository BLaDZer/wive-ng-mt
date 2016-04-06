#include <linux/autoconf.h>

#if defined(CONFIG_RALINK_RT3052)
#define PROCREG_GMAC	"/proc/rt3052/gmac"
#elif defined(CONFIG_RALINK_RT3352)
#define PROCREG_GMAC	"/proc/rt3352/gmac"
#elif defined(CONFIG_RALINK_RT5350)
#define PROCREG_GMAC	"/proc/rt5350/gmac"
#elif defined(CONFIG_RALINK_RT3883)
#define PROCREG_GMAC	"/proc/rt3883/gmac"
#elif defined(CONFIG_RALINK_MT7620)
#define PROCREG_GMAC	"/proc/mt7620/gmac"
#elif defined(CONFIG_RALINK_MT7621)
#define PROCREG_GMAC	"/proc/mt7621/gmac"
#elif defined(CONFIG_RALINK_MT7628)
#define PROCREG_GMAC	"/proc/mt7628/gmac"
#else
#define PROCREG_GMAC	"/proc/mt7620/gmac"
#endif

#define PAGE_SIZE		0x1000  /* 4096 */
#define OTHER_INTERFACE		7	/* port 7  (wifi)  */

/* delay mac table deletion */
#define DELETED			1
#define ZEROED			2

#define ADDENTRY		1
#define DELENTRY		2

struct group_member
{
	unsigned long 		ip_addr;
	unsigned char		a0;		// class D, the 1st byte.
	char 				port_num;
	char				has_report;	// 0 or 1
	struct group_member *next;
};

struct group
{
	unsigned char	a1;			// class D, the 2nd byte
	unsigned char	a2;			// class D, the 3rd byte
	unsigned char	a3;			// class D, the 4th byte
	unsigned char	port_map;	// port map
	struct group_member	*members;
	struct group	*next;
};

typedef u_int8_t   uint8;
typedef u_int16_t  uint16;
typedef u_int32_t  uint32;

#define READMODE        0x0
#define WRITEMODE       0x1
#define WRITE_DELAY     150                     /* ms */

/* global vars */
extern uint32_t WanPort;
extern int auto_lan_snooping;
#ifdef WIFI_IGMPSNOOP_SUPPORT
extern int auto_wifi_snooping;
#endif

void sigUSR1Handler(int unused);
void dump_table(void);

void rt_init(void);
void rt_fini(void);
void rt_switch_init(void);
void rt_switch_fini(void);
#ifdef WIFI_IGMPSNOOP_SUPPORT
void rtwifi_enable(void);
void rtwifi_disable(void);
#endif
void sweap_no_report_members(void);

void insert_multicast_ip(uint32 m_ip_addr, uint32 u_ip_addr);
void remove_member(uint32 m_ip_addr, uint32 u_ip_addr);
void remove_multicast_ip(uint32 m_ip_addr);
void clear_all_entries_report(void);

int addRTWiFiIntf(char *wifi);
int portLookUpByMac(char *mac);
