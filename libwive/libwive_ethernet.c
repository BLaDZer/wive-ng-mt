/*
 EULA:

 WIVE FIRMWARE IS FREE FOR THE NON-COMMERCIAL USE ONLY.

 Conditions of commercial use non GPL (or other not viral license) components are discussed individually.
 For a more detailed discussion please mail me.
 News and changes in Russian: http://wive-ng.sf.net
 Bug report please send to support[at]nag.ru or sfstudio[at]mail.ru
*/

#include "libwive.h"

void portscounts(struct port_counts *st)
{
#ifdef CONFIG_RAETH_SNMPD
	char buf[256];
	FILE *fp;
#endif
	st->rx_count[0] = st->rx_count[1] = st->rx_count[2] = st->rx_count[3] = st->rx_count[4] = st->rx_count[5] = 0;
	st->tx_count[0] = st->tx_count[1] = st->tx_count[2] = st->tx_count[3] = st->tx_count[4] = st->tx_count[5] = 0;

#ifdef CONFIG_RAETH_SNMPD
	fp = fopen(PROCREG_SNMP, "r");
	if (fp == NULL) {
		syslog(LOG_ERR, "no snmp, %s", __FUNCTION__);
		return;
	}

	// skip 32bit counters
	fgets(buf, sizeof(buf), fp);
	fgets(buf, sizeof(buf), fp);

	// get rx 64 bit counter
	if((fgets(buf, sizeof(buf), fp) == NULL) ||
	    (sscanf(buf, "rx64 counters: %llu %llu %llu %llu %llu %llu\n", &st->rx_count[0], &st->rx_count[1], &st->rx_count[2], &st->rx_count[3], &st->rx_count[4], &st->rx_count[5]) != 6)) {
		syslog(LOG_ERR, "rx64 format string error, %s", __FUNCTION__);
		fclose(fp);
		return;
	}

	// get tx 64 bit counter
	if((fgets(buf, sizeof(buf), fp) == NULL ) ||
	    (sscanf(buf, "tx64 counters: %llu %llu %llu %llu %llu %llu\n", &st->tx_count[0], &st->tx_count[1], &st->tx_count[2], &st->tx_count[3], &st->tx_count[4], &st->tx_count[5]) != 6)) {
		syslog(LOG_ERR, "tx64 format string error, %s", __FUNCTION__);
		fclose(fp);
		return;
	}
	fclose(fp);
#endif
}

#if defined(CONFIG_ETHTOOL)
/*
 * description: get link info from ethtool (return defaults values in error path for compat with webswrite)
 */
int linkspeed(const char *ifname, int sd) {
	struct ethtool_cmd ecmd = { .cmd = ETHTOOL_GSET, };
	int iocret, speed = SPEED_10;
	struct ifreq ifr;

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1);
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';
	ifr.ifr_data = (caddr_t)&ecmd;
	if((iocret = ioctl(sd,SIOCETHTOOL,&ifr)) == 0)
		speed = ecmd.speed;
	else
		syslog(LOG_ERR, "ioctl error, %s", __FUNCTION__);

	/* validate nic link speed */
	switch (speed) {
	    case SPEED_10:
	    case SPEED_100:
#ifdef CONFIG_RALINK_MT7621
	    case SPEED_1000:
#endif

	    return speed;
	}

	return SPEED_10;
}

int linkduplex(const char *ifname, int sd) {
	struct ethtool_cmd ecmd = { .cmd = ETHTOOL_GSET, };
	int iocret, duplex = DUPLEX_HALF;
	struct ifreq ifr;

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1);
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';
	ifr.ifr_data = (caddr_t)&ecmd;
	if((iocret = ioctl(sd,SIOCETHTOOL,&ifr)) == 0)
		duplex = ecmd.duplex;
	else
		syslog(LOG_ERR, "ioctl error, %s", __FUNCTION__);

	/* validate duplex mode */
	switch (duplex) {
	    case DUPLEX_HALF:
	    case DUPLEX_FULL:

	    return duplex;
	}

	return DUPLEX_HALF;
}

int linkstatus(const char *ifname, int sd) {
	struct ethtool_value ethval = { .cmd = ETHTOOL_GLINK, };
	int iocret, ret = 0;
	struct ifreq ifr;

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1);
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';
	ifr.ifr_data = (caddr_t)&ethval;
	if((iocret = ioctl(sd,SIOCETHTOOL,&ifr)) == 0)
		ret = ethval.data;
	else
		syslog(LOG_ERR, "ioctl error, %s", __FUNCTION__);


	/* validate link status */
	if (ret)
	    return 1;
	else
	    return 0;
}

void portstatus(struct port_status *st, int portnm)
{
	int sd = -1;
	FILE *proc_file;

	st->portnum = portnm;
	st->link = st->speed = st->duplex = 0;

	if (getIfIsReady(IOCTL_IF) != 1) {
	    syslog(LOG_ERR, "ioctl iface down, %s", __FUNCTION__);
	    return;
	}

	/* switch phy to needed port */
	proc_file = fopen(PROCREG_GMAC, "w");
	if (!proc_file) {
	    syslog(LOG_ERR, "no proc, %s", __FUNCTION__);
	    return;
	}
	fprintf(proc_file, "%d\n", st->portnum);
	fclose(proc_file);

	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sd < 0) {
	    syslog(LOG_ERR, "error ethtool socket open, %s", __FUNCTION__);
	    return;
	}

	st->link = linkstatus(IOCTL_IF, sd);
	st->speed = linkspeed(IOCTL_IF, sd);
	st->duplex = linkduplex(IOCTL_IF, sd);

	close(sd);
}
#endif
