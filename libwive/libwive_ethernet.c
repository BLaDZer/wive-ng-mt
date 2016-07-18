/*
 EULA:

 WIVE FIRMWARE IS FREE FOR THE NON-COMMERCIAL USE ONLY.

 Conditions of commercial use non GPL (or other not viral license) components are discussed individually.
 For a more detailed discussion please mail me.
 News and changes in Russian: http://wive-ng.sf.net
 Bug report please send to support[at]nag.ru or sfstudio[at]mail.ru
*/

#include "libwive.h"

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
#endif
