/*
 *  This implementation is for rtGSW Switch.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include "linux/autoconf.h"
#include "igmpproxy.h"
#include "ra_esw_reg.h"
#include "ra_ioctl.h"

#define END_OF_MAC_TABLE		0xFFFFFFFF
#define ITERATIONTIMEOUT		7000

/* ioctl commands */
#define RAETH_ESW_REG_READ		0x89F1
#define RAETH_ESW_REG_WRITE		0x89F2

/* GSW embedded ethernet switch registers */
#define REG_ESW_VLAN_MEMB_BASE		0x70
#define REG_ESW_TABLE_SEARCH		0x24
#define REG_ESW_TABLE_STATUS0		0x28
#define REG_ESW_TABLE_STATUS1		0x2C
#define REG_ESW_TABLE_STATUS2		0x30
#define REG_ESW_WT_MAC_AD0		0x34
#define REG_ESW_WT_MAC_AD1		0x38
#define REG_ESW_WT_MAC_AD2		0x3C

#define LAN_VLAN_ID			1
#define WAN_VLAN_ID			2

extern uint32_t WanPort;
struct mac_table{
	unsigned int		mac1;		// 4 byte mac address
	unsigned short int	mac2;		// 2 byte mac address
	unsigned char		vid;
	unsigned char		port_map;
};

// function prototype
static inline int reg_read(int offset, int *value);
static inline int reg_write(int offset, int value);

// global variables.
static struct mac_table 	internal_mac_table[2048];
static int 			esw_fd = -1;
static struct ifreq		ifr;
static esw_reg			reg;
#if defined(CONFIG_RALINK_MT7621) || defined (CONFIG_P5_RGMII_TO_MT7530_MODE)
ra_mii_ioctl_data mii;
#endif

#if !defined (CONFIG_RALINK_MT7621) && !defined (CONFIG_P5_RGMII_TO_MT7530_MODE)
static unsigned int rareg(int mode, unsigned int addr, long long int new_value)
{
	int fd;
	unsigned int round;
	void *start;
	volatile unsigned int *v_addr;
	unsigned int rc;

	fd = open("/dev/mem", O_RDWR | O_SYNC );
	if ( fd < 0 ) { 
		printf("open file /dev/mem error. %s\n", strerror(errno));
		exit(-1);
	}

	// round addr to PAGE_SIZE
	round = addr;								// keep old value
	addr = (addr / PAGE_SIZE) * PAGE_SIZE;
	round = round - addr;

	start = mmap(0, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr);
	if(	(int)start == -1 ){
		printf("mmap() failed at phsical address:%d %s\n", addr, strerror(errno));
		close(fd);
		exit(-1);
	}
	//printf("mmap() starts at 0x%08x successfuly\n", (unsigned int) start);

	v_addr = (void *)start + round;
	addr = addr + round;
	printf("0x%08x: 0x%08x\n", addr, *v_addr);

	if(mode == WRITEMODE){
		*v_addr = new_value;
		usleep(WRITE_DELAY * 1000);
		printf("0x%08x: 0x%08x\n", addr, *v_addr);
	}

	rc = *v_addr;
	munmap(start, PAGE_SIZE);
	close(fd);
	return rc;
}
#endif

void dump_table(void)
{
	int i=0;
	unsigned int mac1;
	char show_buf[128];
	while (i < 2048){
	mac1 = internal_mac_table[i].mac1;
	    if (mac1 != END_OF_MAC_TABLE){
		sprintf(show_buf, "%08x%04x, %08x %08x,\n", internal_mac_table[i].mac1, internal_mac_table[i].mac2, internal_mac_table[i].port_map, internal_mac_table[i].vid);
		printf("%s\n", show_buf);
		i++;
	    }else
		return;
	}
}

static void sync_internal_mac_table(void *argu)
{
	unsigned int value, value1, mac2, i = 0;

	reg_write(REG_ESW_WT_MAC_ATC, 0x8004);
	usleep(ITERATIONTIMEOUT);
	while( i < 0x7fe) {
		reg_read(REG_ESW_WT_MAC_ATC, &value);
		if ((value & (0x1 << 13)) && (((value >> 15) &0x1) == 0)) { //search_rdy and Address Table is not busy
			reg_read(REG_ESW_TABLE_ATRD, &value1);
			if ((value1 & 0xff000000) == 0) {
				my_log(LOG_WARNING, 0, "*** rtGSW: found an unused entry (age = 3'b000), stop check!");
				reg_write(REG_ESW_WT_MAC_ATC, 0x8005); //search for next address
				break;
			}

			// read mac1
			reg_read(REG_ESW_TABLE_TSRA1, &(internal_mac_table[i].mac1));

			// read mac2
			reg_read(REG_ESW_TABLE_TSRA2, &mac2);
			internal_mac_table[i].mac2 = (mac2 >> 16);
			internal_mac_table[i].vid = (mac2 & 0xfff);  //vid
			internal_mac_table[i].port_map = (value1 & 0x0007f0) >> 4 ;

			if (value & 0x4000) {
				my_log(LOG_WARNING, 0, "*** rtGSW: end of table. %d", i);
				my_log(LOG_INFO, 0, "sync table at_table_end 1");
				internal_mac_table[i+1].mac1 = END_OF_MAC_TABLE;
				return;
			}
			reg_write(REG_ESW_WT_MAC_ATC, 0x8005); //search for next address
			usleep(ITERATIONTIMEOUT);
			i++;
		}else if (value & 0x4000) { //at_table_end
			my_log(LOG_DEBUG, 0, "*** rtGSW: found the last entry (not ready). %d", i);
			my_log(LOG_INFO, 0, "sync table at_table_end");
			internal_mac_table[i].mac1 = END_OF_MAC_TABLE;
			return;
		} else
			usleep(ITERATIONTIMEOUT);
	}
	internal_mac_table[i].mac1 = END_OF_MAC_TABLE;
	return;
}

void rt_switch_fini(void)
{
#if !defined (CONFIG_RALINK_MT7621) && !defined (CONFIG_P5_RGMII_TO_MT7530_MODE)
	unsigned int value;
	/* IGMP forward to cpu */
	value = rareg(READMODE, 0x1011001c, 0);
	value = value & 0xffff9ff9;
	rareg(WRITEMODE, 0x1011001c, value);

#endif
	if(esw_fd >= 0)
		close(esw_fd);
}

void rt_switch_init(void)
{
#if !defined (CONFIG_RALINK_MT7621) && !defined (CONFIG_P5_RGMII_TO_MT7530_MODE)
	unsigned int value;
	/* to check default IGMP flooding rule IGMP report forward to cpu/query: default policy */
	value = rareg(READMODE, 0x1011001c, 0);
	value = value | 0x00006000;
	rareg(WRITEMODE, 0x1011001c, value);

#endif
	esw_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (esw_fd < 0) {
		perror("socket");
		exit(-1);
	}

	strncpy(ifr.ifr_name, "eth2", 5);
	ifr.ifr_data = (caddr_t)&reg;

	sync_internal_mac_table(NULL);
}

#if defined(CONFIG_RALINK_MT7621) || defined (CONFIG_P5_RGMII_TO_MT7530_MODE)
int reg_read(int offset, int *value)
{
	strncpy(ifr.ifr_name, "eth2", 5);
	ifr.ifr_data = (caddr_t)&mii;

	mii.phy_id = 0x1f;
	mii.reg_num = offset;

	if (-1 == ioctl(esw_fd, RAETH_MII_READ, &ifr)) {
		perror("ioctl");
		close(esw_fd);
		exit(0);
	}
	*value = mii.val_out;
	return 0;
}

int reg_write(int offset, int value)
{
	strncpy(ifr.ifr_name, "eth2", 5);
	ifr.ifr_data = (caddr_t)&mii;

	mii.phy_id = 0x1f;
	mii.reg_num = offset;
	mii.val_in = value;

	if (-1 == ioctl(esw_fd, RAETH_MII_WRITE, &ifr)) {
		perror("ioctl");
		close(esw_fd);
		exit(0);
	}
	return 0;
}
#else
static inline int reg_read(int offset, int *value)
{
    reg.off = offset;
    if (-1 == ioctl(esw_fd, RAETH_ESW_REG_READ, &ifr)) {
        perror("ioctl");
        close(esw_fd);
        exit(0);
    }
    *value = reg.val;
    return 0;
}

static inline int reg_write(int offset, int value)
{
    reg.off = offset;
    reg.val = value;
    if (-1 == ioctl(esw_fd, RAETH_ESW_REG_WRITE, &ifr)) {
        perror("ioctl");
        close(esw_fd);
        exit(0);
    }
    return 0;
}
#endif /* CONFIG_RALINK_MT7621 */

static inline void wait_switch_done(void)
{
	int i, value;

	for (i = 0; i < 20; i++) {
	    reg_read(REG_ESW_WT_MAC_ATC, &value);
	    if ((value & 0x8000) == 0 ){ //mac address busy
		my_log(LOG_INFO, 0, "mac table IO done.\n");
		break;
	    }
	    usleep(1000);
	}
	if (i == 20)
		my_log(LOG_WARNING, 0, "*** rtGSW: timeout.");
}

void updateMacTable(struct group *entry, int delay_delete)
{
	int value = 0, value1 = 0;
	char wholestr[13];
	char tmpstr[9];

        my_log(LOG_INFO, 0, "updateMacTable: delay_delete is %d", delay_delete);
	sprintf(wholestr, "%s%02x%02x%02x", "01005e", entry->a1, entry->a2, entry->a3);

	strncpy(tmpstr, wholestr, 8);
	tmpstr[8] = '\0';

	value = strtoul(tmpstr, NULL, 16);

	reg_write(REG_ESW_WT_MAC_ATA1, value);
	my_log(LOG_INFO, 0, "REG_ESW_WT_MAC_ATA1 is 0x%x",value);


	strncpy(tmpstr, &wholestr[8], 4);
	tmpstr[4] = '\0';

	value = strtoul(tmpstr, NULL, 16);
	value = (value << 16);
	value |= (1 << 15);//IVL=1

#if defined(CONFIG_RALINK_MT7621) && defined (CONFIG_RAETH_GMAC2)
#elif defined(CONFIG_GE2_INTERNAL_GMAC_P5) || defined(CONFIG_P4_RGMII_TO_MT7530_GMAC_P5)
#elif defined(CONFIG_P5_RGMII_TO_MT7530_MODE)
#else
	value |= ((LAN_VLAN_ID) << 0); //LAN ID ==1
#endif
	reg_write(REG_ESW_WT_MAC_ATA2, value);
	my_log(LOG_INFO, 0, "REG_ESW_WT_MAC_ATA2 is 0x%x",value);
	value1 = value; //save for later usage

	value = 0;
	if(entry->port_map || delay_delete == ZEROED){
		/*
		 * force all mulicast addresses to bind with CPU.
		 */
		value |= (0x1 << 10);//port 6 cpu port
		/*
		 * fill the port map
		 */
		my_log(LOG_INFO, 0, "entry->port_map is 0x%x", entry->port_map);
		value |= (entry->port_map & (0x7f)) << 4;

                value |= (0xff << 24); //w_age_field
		value |= (0x3<< 2); //static

		reg_write(REG_ESW_WT_MAC_ATWD, value);
		value = 0x8001;  //w_mac_cmd
	        reg_write(REG_ESW_WT_MAC_ATC, value);

		wait_switch_done();

		/*
		 * new an additional entry for IGMP Inquery/Report on WAN.
		 */
		if(WanPort){
		        value = value1;
			value = (value & 0xffffff00);
#if defined(CONFIG_RALINK_MT7621) && defined (CONFIG_RAETH_GMAC2)
#elif defined(CONFIG_GE2_INTERNAL_GMAC_P5) || defined(CONFIG_P4_RGMII_TO_MT7530_GMAC_P5)
#elif defined(CONFIG_P5_RGMII_TO_MT7530_MODE)
#else
			value |= ((WAN_VLAN_ID) << 0); //WAN ID ==2
#endif
			reg_write(REG_ESW_WT_MAC_ATA2, value);
			my_log(LOG_INFO, 0, "WAN REG_ESW_WT_MAC_ATA2 is 0x%x",value);

			value1 = (WanPort << 4);
#if defined(CONFIG_GE2_INTERNAL_GMAC_P5) || defined(CONFIG_P4_RGMII_TO_MT7530_GMAC_P5)
			value1 |= (0x1 << 9);//port 5 cpu port
#else
			value1 |= (0x1 << 10);//port 6 cpu port
#endif
			value1 |= (0xff << 24); //w_age_field
			value1 |= (0x3<< 2); //static

			reg_write(REG_ESW_WT_MAC_ATWD, value1);

			value1 = 0x8001;  //w_mac_cmd
			reg_write(REG_ESW_WT_MAC_ATC, value1);

			wait_switch_done();
			my_log(LOG_INFO, 0, "for wan port is done");
		}
	}else{
			/*
			 * delete the entry
			 */
			value = 0; //STATUS=0, delete mac
			reg_write(REG_ESW_WT_MAC_ATWD, value);

			value = 0x8001;  //w_mac_cmd
			reg_write(REG_ESW_WT_MAC_ATC, value);
			wait_switch_done();

			/*
			 * delete the additional entry on WAN.
			 */
			value = value1;
			value = (value & 0xffffff00);
			value |= (2 << 0); //WAN ID ==2

			reg_write(REG_ESW_WT_MAC_ATA2, value);
			my_log(LOG_INFO, 0, "REG_ESW_WT_MAC_ATA2 is 0x%x",value);

			value = 0; //STATUS=0, delete mac
			reg_write(REG_ESW_WT_MAC_ATWD, value);

			value = 0x8001;  //w_mac_cmd
			reg_write(REG_ESW_WT_MAC_ATC, value);
			wait_switch_done();
	}
}

int portLookUpByMac(char *mac)
{
	unsigned long long int  mac1, mac2;
	unsigned int i = 0, mac_iter;
	char mac_entry1[16], mac_entry2[16];

	sync_internal_mac_table(NULL);

	memset(mac_entry1, 0, sizeof(mac_entry1));
	memset(mac_entry2, 0, sizeof(mac_entry2));

	strncpy(mac_entry1, mac, 8);
	strncpy(mac_entry2, &mac[8], 4);
	mac1 = strtoll(mac_entry1, 0, 16);
	mac2 = strtol(mac_entry2, 0, 16);

	mac_iter = internal_mac_table[i].mac1;
	while(i < 0x7fe && mac_iter != END_OF_MAC_TABLE) {
		if(internal_mac_table[i].vid != LAN_VLAN_ID)
			goto next_entry;
		if(	internal_mac_table[i].mac1 == mac1 &&
			internal_mac_table[i].mac2 == mac2){
			switch( internal_mac_table[i].port_map ){
			case 0x1:
				return 0;
			case 0x2:
				return 1;
			case 0x4:
				return 2;
			case 0x8:
				return 3;
			case 0x10:
				return 4;
			case 0x40:	/* CPU Only */
				break;
			default:
				my_log(LOG_WARNING, 0, "No/Multi ports found:%x", internal_mac_table[i].port_map);
				return -1;
			}
		}
next_entry:
		i++;
		mac_iter = internal_mac_table[i].mac1;
	}
	return -1;
}
