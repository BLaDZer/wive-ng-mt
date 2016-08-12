/*
 EULA:

 WIVE FIRMWARE IS FREE FOR THE NON-COMMERCIAL USE ONLY.

 Conditions of commercial use non GPL (or other not viral license) components are discussed individually.
 For a more detailed discussion please mail me.
 News and changes in Russian: http://wive-ng.sf.net
 Bug report please send to support[at]nag.ru or sfstudio[at]mail.ru
*/

#include "libwive.h"

/* nicscounts - Get software network interface statistics list
 *
 * arg: (out) elem_count - number of list elements
 * return: nic_counts struct array
 */
struct nic_counts* nicscounts(int *elem_count) {
	char buf[512];
	FILE *fp = fopen(_PATH_PROCNET_DEV, "r");
	int inum = 0;
	(*elem_count) = 0;

	if (fp == NULL)
	{
		syslog(LOG_ERR, "no proc, %s", __FUNCTION__);
		return NULL;
	}

	int res_arr_capacity = 32;
	struct nic_counts* pst = calloc(res_arr_capacity,sizeof(struct nic_counts));

	if (pst == NULL)
	{
    		syslog(LOG_ERR, "Memory allocation error, %s", __FUNCTION__);
		fclose(fp);
		return NULL;
	}

	// skip headers
	fgets(buf, sizeof(buf), fp);
	fgets(buf, sizeof(buf), fp);

	while (fgets(buf, sizeof(buf), fp) != NULL)
	{
		char *semiColon;
		struct nic_counts* nc = &pst[inum];
		nc->is_available = 0;

		// find : , extract ifname, move pointer to next block semicolon
		semiColon = strchr(buf, ':');
		if (semiColon == NULL || (*semiColon++ = 0, sscanf(buf, "%s", nc->ifname) != 1)) {
			syslog(LOG_ERR, "wrong format string in /proc/net/dev, %s", __FUNCTION__);
			continue;
		}

		// not correct parse
		if(nc->ifname[0]=='\0')
			continue;

		// long ifname
		if(strlen(nc->ifname) > IFNAMSIZ)
			continue;

		// filter 'lo' interface
		if (strcmp(nc->ifname, "lo") == 0)
			continue;

		// check that interface is up
		if (getIfIsReady(nc->ifname) != 1)
			continue;

		// extract scale and print statistic
		if (sscanf(semiColon, "%llu%llu%llu%llu%llu%llu%llu%*d%llu%llu%llu%llu%llu%llu%llu",
			   &nc->rx_bytes, &nc->rx_packets, &nc->rx_errs, &nc->rx_drops, &nc->rx_fifo, &nc->rx_frame, &nc->rx_multi,
			    &nc->tx_bytes, &nc->tx_packets, &nc->tx_errs, &nc->tx_drops, &nc->tx_fifo, &nc->tx_colls, &nc->tx_carrier) != 14) {
			// not extracted - print n/a
			nc->is_available = 0;
		} else {
			nc->is_available = 1;
		}

		inum++;
		if (inum == res_arr_capacity)
		{
		    res_arr_capacity += 32;
		    void* nnc = realloc(pst,res_arr_capacity*sizeof(struct nic_counts));
		    if (nnc == NULL) {
	    		syslog(LOG_ERR, "Memory allocation error, %s", __FUNCTION__);
			fclose(fp);
			return pst;
		    }
		    pst = (struct nic_counts*)nnc;
		    
		}

		pst[inum].ifname[0] = '\0';
	}

	fclose(fp);

	(*elem_count) = inum;
	return pst;

}


/* portscounts - Get hardware network port statistics
 *
 * arg: (out) st - output port_counts structure pointer
 * return: 0 = OK, -1 = SNMPD unavailable, > 0 = error
 */
int portscounts(struct port_counts *st)
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
		return 1;
	}

	// skip 32bit counters
	fgets(buf, sizeof(buf), fp);
	fgets(buf, sizeof(buf), fp);

	// get rx 64 bit counter
	if((fgets(buf, sizeof(buf), fp) == NULL) ||
	    (sscanf(buf, "rx64 counters: %llu %llu %llu %llu %llu %llu\n", &st->rx_count[0], &st->rx_count[1], &st->rx_count[2], &st->rx_count[3], &st->rx_count[4], &st->rx_count[5]) != 6)) {
		syslog(LOG_ERR, "rx64 format string error, %s", __FUNCTION__);
		fclose(fp);
		return 2;
	}

	// get tx 64 bit counter
	if((fgets(buf, sizeof(buf), fp) == NULL ) ||
	    (sscanf(buf, "tx64 counters: %llu %llu %llu %llu %llu %llu\n", &st->tx_count[0], &st->tx_count[1], &st->tx_count[2], &st->tx_count[3], &st->tx_count[4], &st->tx_count[5]) != 6)) {
		syslog(LOG_ERR, "tx64 format string error, %s", __FUNCTION__);
		fclose(fp);
		return 3;
	}
	fclose(fp);
	return 0;
#endif
	return -1;
}

#if defined(CONFIG_ETHTOOL)


/* linkspeed - get link speed info from ethtool
 *
 * arg: ifname - interface to examine
 * arg: sd - ioctl socket descriptor
 * return: actual link speed or SPEED_10 in case of error for compat with webswrite
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

/* linkduplex - get link duplex state from ethtool
 *
 * arg: ifname - interface to examine
 * arg: sd - ioctl socket descriptor
 * return: actual duplex state or DUPLEX_HALF in case of error for compat with webswrite
 */
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

/* linkstatus - get link status from ethtool
 *
 * arg: ifname - interface to examine
 * arg: sd - ioctl socket descriptor
 * return: 1 = available, 0 = inavailable
 */
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

/* portstatus - get port status from ethtool
 *
 * arg: (out) st - port_status structure
 * arg: portnm - port number
 * return: 0 = OK
 */
int portstatus(struct port_status *st, int portnm)
{
	int sd = -1;
	FILE *proc_file;

	st->portnum = portnm;
	st->link = st->speed = st->duplex = 0;

	if (getIfIsReady(IOCTL_IF) != 1) {
	    syslog(LOG_ERR, "ioctl iface down, %s", __FUNCTION__);
	    return 1;
	}

	/* switch phy to needed port */
	proc_file = fopen(PROCREG_GMAC, "w");
	if (!proc_file) {
	    syslog(LOG_ERR, "no proc, %s", __FUNCTION__);
	    return 2;
	}
	fprintf(proc_file, "%d\n", st->portnum);
	fclose(proc_file);

	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sd < 0) {
	    syslog(LOG_ERR, "error ethtool socket open, %s", __FUNCTION__);
	    return 3;
	}

	st->link = linkstatus(IOCTL_IF, sd);
	st->speed = linkspeed(IOCTL_IF, sd);
	st->duplex = linkduplex(IOCTL_IF, sd);

	close(sd);

	return 0;
}
#endif
