/*
 EULA:

 WIVE FIRMWARE IS FREE FOR THE NON-COMMERCIAL USE ONLY.

 Conditions of commercial use non GPL (or other not viral license) components are discussed individually.
 For a more detailed discussion please mail me.
 News and changes in Russian: http://wive-ng.sf.net
 Bug report please send to support[at]nag.ru or sfstudio[at]mail.ru
*/

#include "libwive.h"

/*
 * arguments: ifname - interface name
 * description: return 1 if interface is up and ready
 *              return 0 if interface is down or no carrier
 *              return -1 if error
 */
int getIfIsReady(const char *ifname)
{
	struct ifreq ifr;
	int skfd;

	skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (skfd == -1) {
		syslog(LOG_ERR, "open socket failed, %s", __FUNCTION__);
		return -1;
	}

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1);
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';
	if (ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0) {
		close(skfd);
		syslog(LOG_ERR, "ioctl call failed, %s", __FUNCTION__);
		return -1;
	}
	close(skfd);

	if ((ifr.ifr_flags & IFF_UP) && (ifr.ifr_flags & IFF_RUNNING))
		return 1;
	else
		return 0;
}

/*
 * arguments: ifname  - interface name
 *            if_addr - a 16-byte buffer to store ip address
 * description: fetch ip address, netmask associated to given interface name
 */
int getIfIp(const char *ifname, char *if_addr)
{
	struct ifreq ifr;
	int skfd = 0;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		syslog(LOG_ERR, "open socket failed, %s", __FUNCTION__);
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1);
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';
	ifr.ifr_addr.sa_family = AF_INET;
	if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0) {
		close(skfd);
		syslog(LOG_ERR, "ioctl call failed, %s", __FUNCTION__);
		return -1;
	}
	strcpy(if_addr, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

	close(skfd);
	return 0;
}

/*
 * arguments: ifname  - interface name
 *            if_addr - a 18-byte buffer to store mac address
 * description: fetch mac address according to given interface name
 */
int getIfMac(const char *ifname, char *if_hw)
{
	struct ifreq ifr;
	char *ptr;
	int skfd;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		syslog(LOG_ERR, "open socket failed, %s", __FUNCTION__);
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1);
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';
	ifr.ifr_addr.sa_family = AF_INET;
	if(ioctl(skfd, SIOCGIFHWADDR, &ifr) < 0) {
		close(skfd);
		syslog(LOG_ERR, "ioctl call failed, %s", __FUNCTION__);
		return -1;
	}

	ptr = (char *)&ifr.ifr_addr.sa_data;
	sprintf(if_hw, "%02X:%02X:%02X:%02X:%02X:%02X",
			(ptr[0] & 0377), (ptr[1] & 0377), (ptr[2] & 0377),
			(ptr[3] & 0377), (ptr[4] & 0377), (ptr[5] & 0377));

	close(skfd);
	return 0;
}

/*
 * arguments: ifname - interface name
 *            if_net - a 16-byte buffer to store subnet mask
 * description: fetch subnet mask associated to given interface name
 */
int getIfNetmask(const char *ifname, char *if_net)
{
	struct ifreq ifr;
	int skfd = 0;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		syslog(LOG_ERR, "open socket failed, %s", __FUNCTION__);
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1);
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';
	ifr.ifr_addr.sa_family = AF_INET;
	if (ioctl(skfd, SIOCGIFNETMASK, &ifr) < 0) {
		close(skfd);
		syslog(LOG_ERR, "ioctl call failed, %s", __FUNCTION__);
		return -1;
	}
	strcpy(if_net, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
	close(skfd);
	return 0;
}

void arplookup(char *ip, char *arp)
{
    char buf[256];
    FILE *fp = fopen("/proc/net/arp", "r");
    if(!fp) {
	syslog(LOG_ERR, "no proc , %s", __FUNCTION__);
        return;
    }

    strcpy(arp, "00:00:00:00:00:00");
    while((fgets(buf, sizeof(buf), fp)) != NULL) {
        char ip_entry[32], hw_type[8],flags[8], hw_address[32];
        sscanf(buf, "%s %s %s %s", ip_entry, hw_type, flags, hw_address);
        if(!strcmp(ip, ip_entry)) {
            strcpy(arp, hw_address);
            break;
        }
    }

    fclose(fp);
    return;
}
