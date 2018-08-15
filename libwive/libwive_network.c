/*
 EULA:

 WIVE FIRMWARE IS FREE FOR THE NON-COMMERCIAL USE ONLY.

 Conditions of commercial use non GPL (or other not viral license) components are discussed individually.
 For a more detailed discussion please mail me.
 News and changes in Russian: http://wive-ng.sf.net
 Bug report please send to support[at]nag.ru or sfstudio[at]mail.ru
*/

#include "libwive.h"

#define T(s) s

const vpn_status_t vpn_statuses[] =
{
    { "disabled",     0x808080 },
    { "offline",      0xff0000 },
    { "connecting",   0xff8000 },
    { "online",       0x00ff00 }
};

#ifdef CONFIG_USER_KABINET
/*
 * LANAUTH status
 */
const vpn_status_t lanauth_statuses[] =
{
    { "disabled",           0x808080 },
    { "not started",        0x808080 },
    { "offline",            0xff0000 },
    { "kabinet networks",   0x33bb33 },
    { "full access",        0x00ff00 }
};
#endif

uint64_t hton64(uint64_t v)
{
        return (((uint64_t)htonl(v)) << 32) | htonl(v >> 32);
}

int isMacValid(char *str)
{
	int i, len = strlen(str);
	if(len != 17)
		return 0;

	for(i=0; i<5; i++){
		if( (!isxdigit( str[i*3])) || (!isxdigit( str[i*3+1])) || (str[i*3+2] != ':') )
			return 0;
	}
	return (isxdigit(str[15]) && isxdigit(str[16])) ? 1: 0;
}

int isIpValid(char *str)
{
	struct in_addr addr;	// for examination

	if( (! strcmp(T("any"), str)) || (! strcmp(T("any/0"), str)))
		return 1;

	if(! (inet_aton(str, &addr))){
		syslog(LOG_ERR, "isIpValid(): %s is not a valid IP address, %s", str, __FUNCTION__);
		return 0;
	}
	return 1;
}

int isIpNetmaskValid(char *s)
{
	char str[32];
	char *slash;
	struct in_addr addr;    // for examination

	if (!s || !strlen(s))
		return 0;

	strncpy(str, s, sizeof(str));

	if( (!strcmp("any", str)) || (!strcmp("any/0", str)))
		return 1;

	if (!isAllNumAndSlash(str))
		return 0;

	if (!isOnlyOneSlash(str))
		return 0;

	slash = strchr(str, '/');
	if (slash)
	{
		int mask;

		*slash = '\0';
		slash++;
		if(!strlen(slash)){
			return 0;
		}

		if(!isNumOnly(slash)){
			return 0;
		}

		mask = atoi(slash);
		if(mask < 0 || mask > 32){
			return 0;
		}
	}

	if (! (inet_aton(str, &addr)))
	{
		syslog(LOG_ERR, "isIpNetmaskValid(): %s is not a valid IP address, %s", str, __FUNCTION__);
		return 0;
	}

	return 1;
}

/*
 * description: return LAN interface name
 */
char* getLanIfName(void)
{
	char *mode = nvram_get(RT2860_NVRAM, "OperationMode");
	FILE *fp;
	static char lan_if[16] = {0}; /* max 16 char in lan if name */

	/* set default */
	if (mode == NULL)
		return "br0";

	/* try read fron file exported from init.d */
	fp = fopen("/tmp/lan_if_name", "r");
	if (fp) {
	    /* get first lan_if in file */
	    while ((fgets(lan_if, sizeof(lan_if), fp)) != NULL) {
		if ((strstr(lan_if, ETH_SIG) != NULL) || (strstr(lan_if, BR_SIG) != NULL)) {
		    fclose(fp);
		    return strip_space(lan_if);
		}
	    }
	    fclose(fp);
	}

	/* in ethernet converter mode lan_if = eth2 */
	if (!strncmp(mode, "2", 2))
		return "eth2";

	return "br0";
}

/*
 * description: return WAN interface name depend by opmode
 */
char* getWanIfName(void)
{
	char *mode = nvram_get(RT2860_NVRAM, "OperationMode");
	char *apc_cli_mode = nvram_get(RT2860_NVRAM, "ApCliBridgeOnly");
	char *apc_cli_wanif = nvram_get(RT2860_NVRAM, "ApCliIfName");
	char *if_name = WAN_DEF;
	char wan_if[16]; /* max 16 char in wan if name */
	FILE *fp;

	/* try read fron file exported from init.d */
	fp = fopen("/tmp/wan_if_name", "r");
	if (fp) {
	    /* get first wan_if in file */
	    while ((fgets(wan_if, sizeof(wan_if), fp)) != NULL) {
		if ((strstr(wan_if, ETH_SIG) != NULL) || (strstr(wan_if, BR_SIG) != NULL)) {
		    fclose(fp);
		    return strip_space(wan_if);
		}
	    }
	    fclose(fp);
	}

	/* set default */
	if (mode == NULL)
		return WAN_DEF;

	if (!strncmp(mode, "0", 2))					/* bridge mode */
		if_name = "br0";
	else if (!strncmp(mode, "1", 2) || !strncmp(mode, "4", 2))	/* gw and chillispot mode */
		if_name = WAN_DEF;
	else if (!strncmp(mode, "2", 2))				/* ethernet converter mode */
		if_name = "ra0";
	else if (!strncmp(mode, "3", 2)) {				/* apcli mode */
		    if (!strncmp(apc_cli_mode, "1", 2)) {
			if_name = "br0";				/* Client-AP-Bridge */
		    } else {
			if_name = apc_cli_wanif;			/* Client-AP-Gateway 2.4Ghz/5GHz */
		    }
		}

	return if_name;
}

/*
 * description: return 1 if vpn enables or 0 if disabled
 */
int vpn_mode_enabled(void)
{
	char *cm = nvram_get(RT2860_NVRAM, "vpnEnabled");

        if (!strncmp(cm, "on", 3))
	    return 1;
        else
	    return 0;
}

/*
 * description: get the value "WAN" or "LAN" the interface is belong to.
 */
char *getLanWanNamebyIf(const char *ifname)
{
	if(!strcmp(ifname, getLanIfName()))
		return "LAN";
	if(!strcmp(ifname, getWanIfName()))
		return "WAN";
	if (vpn_mode_enabled() == 1 && strstr(ifname, getPPPIfName()) != NULL)
		return "VPN";
	if (nvram_get_int(RT2860_NVRAM, "l2tp_srv_enabled", 0) == 1) {
		if (strstr(ifname, "ppp") != NULL) {
			int i, j = 0;
			char iface_num[3];
			for (i = 0; i < strlen(ifname); i++)
				if (ifname[i] != 'p') {
					iface_num[j] = ifname[i];
					j++;
				}
			if (atoi(iface_num) >= 10)
				return "VPN SERVER";
		}
	}

	return "LAN";
}

/*
 * description: return vpn interface name or VPN_DEF as default
 */
char* getPPPIfName(void)
{
        FILE *fp;
	char ppp_if[16]; /* max 16 char in vpn if name */

	fp = fopen("/tmp/vpn_if_name", "r");
	if (fp) {
	    /* get first ppp_if in file */
	    while ((fgets(ppp_if, sizeof(ppp_if), fp)) != NULL) {
		if (strstr(ppp_if, VPN_SIG) != NULL) {
		    fclose(fp);
		    return strip_space(ppp_if);
		}
	    }
	    fclose(fp);
	}

	return VPN_DEF;
}


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
	strlcpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));

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
	int retcode = 0;


	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		syslog(LOG_ERR, "open socket failed, %s", __FUNCTION__);
		return -1;
	}

	memset(&ifr, 0, sizeof(ifr));
	strlcpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
	ifr.ifr_addr.sa_family = AF_INET;

	retcode = ioctl(skfd, SIOCGIFADDR, &ifr);
	if (retcode < 0) {
		close(skfd);
		syslog(LOG_ERR, "ioctl call failed, %s, %d", __FUNCTION__, retcode);
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
int getIfMac(const char *ifname, char *if_hw, char separator)
{
	struct ifreq ifr;
	char *ptr;
	int skfd;


	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		syslog(LOG_ERR, "open socket failed, %s", __FUNCTION__);
		return -1;
	}

	memset(&ifr, 0, sizeof(ifr));
	strlcpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
	ifr.ifr_addr.sa_family = AF_INET;
	if(ioctl(skfd, SIOCGIFHWADDR, &ifr) < 0) {
		close(skfd);
		syslog(LOG_ERR, "ioctl call failed, %s", __FUNCTION__);
		return -1;
	}

	ptr = (char *)&ifr.ifr_addr.sa_data;

	if (separator == '\0') {
		sprintf(if_hw, "%02X%02X%02X%02X%02X%02X",
			(ptr[0] & 0377), (ptr[1] & 0377), (ptr[2] & 0377),
			(ptr[3] & 0377), (ptr[4] & 0377), (ptr[5] & 0377));
	}
	else
	{
		sprintf(if_hw, "%02X%c%02X%c%02X%c%02X%c%02X%c%02X",
			(ptr[0] & 0377), separator, (ptr[1] & 0377), separator, (ptr[2] & 0377), separator,
			(ptr[3] & 0377), separator, (ptr[4] & 0377), separator, (ptr[5] & 0377));
	}

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

	memset(&ifr, 0, sizeof(ifr));
	strlcpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
	ifr.ifr_addr.sa_family = AF_INET;

	if (ioctl(skfd, SIOCGIFNETMASK, &ifr) < 0) {
		close(skfd);
		syslog(LOG_ERR, "ioctl call failed, %s (ifname=%s)", __FUNCTION__, ifname);
		return -1;
	}
	strcpy(if_net, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
	close(skfd);
	return 0;
}

#ifdef CONFIG_IPV6
/*
 * arguments: ifname  - interface name
 *            if_addr - buffer to store ip address
 *            netmask - buffer to store prefix
 * description: fetch ipv6 address
 */
int getIfIPv6(const char *ifname, char *if_addr, char *netmask)
{
	FILE *fp;
	unsigned char ipv6[16];
	int scope, prefix;
	char address[INET6_ADDRSTRLEN] = "";
	char dname[IFNAMSIZ] = "";


	if (NULL == (fp = fopen(_PATH_PROCNET_IPV6, "r"))) {
		syslog(LOG_ERR, "getIPv6IntAddr: open /proc/net/if_inet6 error, %s", __FUNCTION__);
		return -1;
	}

	while (19 == fscanf(fp, "%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx %*x %x %x %*x %s",
		&ipv6[0], &ipv6[1], &ipv6[2], &ipv6[3], &ipv6[4], &ipv6[5], &ipv6[6], &ipv6[7], &ipv6[8], &ipv6[9],
		&ipv6[10], &ipv6[11], &ipv6[12], &ipv6[13], &ipv6[14], &ipv6[15], &prefix, &scope, dname)) {
		if (strcmp(ifname, dname) != 0) {
			continue;
		}
		if (inet_ntop(AF_INET6, ipv6, address, sizeof(address)) == NULL) {
			continue;
		}
		if (scope == IPV6_ADDR_GLOBAL) {
			break;
		}
	}
	fclose(fp);

	if (strlen(address)<=0) {
		return -255;
	} else {
		strcpy(if_addr, address);
		sprintf(netmask, "%d", prefix);
		return 0;
	}
}

/* getIPv6IntIPAddr
 * arg: (out) address - char[INET6_ADDRSTRLEN] IP address buffer
 * arg: (out) mask    - char[16] mask buffer
 * description: get internal IPv6 address
 */
int getIPv6IntIPAddr(char* address, char* mask) {
	char lanif[IFNAMSIZ] = "";
	int opmode = nvram_get_int(RT2860_NVRAM, "IPv6OpMode", -1);

	if (opmode == 0)
		return 1;

	strcpy(lanif, getLanIfName());

	if (getIfIPv6(lanif, address, mask) != 0)
		return 2;

	return 0;
}

/* getIPv6ExtIPAddr
 * arg: (out) address - char[INET6_ADDRSTRLEN] IP address buffer
 * arg: (out) mask    - char[16] mask buffer
 * description: get external IPv6 address
 */
int getIPv6ExtIPAddr(char* address, char* mask) {
	char tmpif[IFNAMSIZ] = "";
	char wanif[IFNAMSIZ] = "";
	FILE *fp;

	int opmode = nvram_get_int(RT2860_NVRAM, "IPv6OpMode", -1);

	if (opmode == 0)
		return 1;

	if (NULL == (fp = fopen("/tmp/six_wan_if_name", "r"))) {

		switch (opmode)
		{
		    case 1:
			    if (vpn_mode_enabled() == 1)
				strcpy(wanif, getPPPIfName());
			    else
				strcpy(wanif, getWanIfName());
			break;
		    case 2:
			    strcpy(wanif, "6rd");
			break;
		    case 3:
			    strcpy(wanif, "sit0");
			break;
		}

	} else {
		while ((fgets(tmpif, sizeof(tmpif), fp)) != NULL) {
			if ((strstr(tmpif, ETH_SIG) != NULL) || (strstr(tmpif, BR_SIG) != NULL) ||
				(strstr(tmpif, SIXRD_SIG) != NULL) || (strstr(tmpif, SIX2FOUR_SIG) != NULL)) {
				strcpy(wanif, strip_space(tmpif));
				break;
			}
		}
		fclose(fp);
	}

	if (getIfIPv6(wanif, address, mask) != 0)
		return 2;

	return 0;
}

#endif


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

#ifdef CONFIG_USER_KABINET
/* getLANAUTHState - Get lanauth state
 *
 * return: actual lanauth state+1 or 0 if lanauth process not found
 */
static int getLANAUTHState()
{
	FILE *fp;
	char result[2];

	fp = popen("ps | grep lanaut[h] | sed 's|.*-A ||'", "r");
	if(!fp){
		syslog(LOG_ERR, "no ps util exist, %s", __FUNCTION__);
		return -1;
	}
	fgets(result, sizeof(result), fp);
	pclose(fp);

	if (result[0] == '0')
		return 1;
	else if (result[0] == '1')
		return 2;
	else if (result[0] == '2')
		return 3;
	else
		return 0;
}
#endif


/*
 * Get VPN status
 */
int getVPNStatusCode()
{
	int status = 0; // Status is 'disabled'
	char *vpn_enabled = nvram_get(RT2860_NVRAM, "vpnEnabled");

	if ((vpn_enabled==NULL) || (vpn_enabled[0]=='\0'))
		vpn_enabled = "off";

	// Do not perform other checks if VPN is turned off
	if (strcmp(vpn_enabled, "on")==0) {
#ifdef CONFIG_USER_KABINET
		int vpn_type = nvram_get_int(RT2860_NVRAM, "vpnType", -1);
		if (vpn_type == 3)
			status = (getLANAUTHState() + 1) % 5;
		else {
#endif
			// Status is at least 'offline' now
			status++;

			// Try to find pppd or xl2tpd
			int found = procps_count("pppd");
			if (found==0)
				found = procps_count("xl2tpd");

			if (found>0) {
				// Now status is at least 'connecting'
				status++;

				// Try to search for 'pppXX' device
				FILE * fd = fopen(_PATH_PROCNET_DEV, "r");

				if (fd != NULL) {
					int ppp_id;
					char line[256];

					// Read all ifaces and check match
					while ((fgets(line, sizeof(line), fd)) != NULL) {
						// Filter only 'pppXX'
						if (sscanf(line, " ppp%d", &ppp_id) == 1) {
							// Check if ppp interface has number at least 8
							if ((ppp_id >= 0) && (ppp_id <= 8)) {
								status++; // Status is set to 'connected'
								break; // Do not search more
							}
						}
					}

					fclose(fd);
				} else {
					syslog(LOG_WARNING, "cannot open %s (%s).\n", _PATH_PROCNET_DEV, strerror(errno));
					return -1;
				}
			} else if (found<0) {
				//syslog(LOG_WARNING, "cannot serach process 'pppd' or 'xl2tpd': %s\n", strerror(-found));
				return -2;
			}
#ifdef CONFIG_USER_KABINET
		}
#endif
	}

	return status;
}

/*
 * Get VPN status string
 */
const char* getVPNStatusStr()
{
    const vpn_status_t *st_table = vpn_statuses;
    int status = getVPNStatusCode();
#ifdef CONFIG_USER_KABINET
    int vpn_type = nvram_get_int(RT2860_NVRAM, "vpnType", -1);
    if (vpn_type == 3)
        st_table = lanauth_statuses;
#endif
    return st_table[status].statusStr;
}


/* (static) findRoutingRule - Find routing rule index inside rule list string and populate the fields
 *
 * arg: rrs           - semicolon-separated rule list string
 * arg: (out) buf     - the whole rule from rrs
 * arg: (out) dest    - destination address
 * arg: (out) netmask - 
 * arg: (out) gw      - gateway
 * arg: (out) iface   - interface name
 * return: rule index (0..N) or -1 if error
 */
static int findRoutingRule(char *rrs, char *buf, const char *dest, const char *netmask, const char* gw, const char *iface)
{
	char c_dest[32], c_netmask[32], c_iface[32], c_gw[32];
	int index = 0;

	while (getNthValueSafe(index++, rrs, ';', buf, 256) != -1)
	{
		if ((getNthValueSafe(0, buf, ',', c_dest, sizeof(c_dest)) == -1))
			continue;
		if ((getNthValueSafe(1, buf, ',', c_netmask, sizeof(c_netmask)) == -1))
			continue;
		if ((getNthValueSafe(2, buf, ',', c_gw, sizeof(c_gw)) == -1))
			continue;
		if ((getNthValueSafe(4, buf, ',', c_iface, sizeof(c_iface)) == -1))
			continue;

		if ((strcmp(dest, c_dest)==0) && (strcmp(netmask, c_netmask)==0) &&
			(strcmp(iface, c_iface)==0) && (strcmp(gw, c_gw)==0))
			return index-1;
	}

	return -1;
}

/* parseRoutingTable - Generate routing rule table
 *
 * arg: rules_str          - semicolon-separated rule list string from NVRAM
 * arg: (out) res_rule_num - output array length
 * return: RoutingRule struct array (don't forget to free it afterwards) or NULL
 */
struct RoutingRule* parseRoutingTable(char* rules_str, int *res_rule_num)
{
	char   buff[512];
	int    nl = 0, index;
	char   dest_str[32], gw_str[32], netmask_str[32];
	int   *running_rules = NULL;
	char   one_rule[256];
	unsigned long int d,g,m;
	int  rule_count;
	int rule_num;

	int res_arr_capacity = 32;
	struct RoutingRule *table;

	FILE *fp = fopen("/proc/net/route", "r");
	if (!fp)
		return NULL;

	table = calloc(res_arr_capacity,sizeof(struct RoutingRule));

	// Get routing table
	if (rules_str == NULL)
	    rules_str = "";
	rule_count = getNums(rules_str, ';');

	if (rule_count)
	{
		running_rules = calloc(1, sizeof(int) * rule_count);
		if (!running_rules)
		{
			free(table);
			fclose(fp);
			return NULL;
		}
	}

	rule_num = 0;

	// true_interface[0], destination[1], gateway[2], netmask[3], flags[4], ref[5], use[6],
	// metric[7], category[8], interface[9], idle[10], comment[11], new[12]
	while ((fgets(buff, sizeof(buff), fp)) != NULL)
	{

		if (nl > 0)
		{
			struct RoutingRule* rule = &table[rule_num];

			if (sscanf(buff, "%s%lx%lx%X%d%d%d%lx", rule->interface, &d, &g, &rule->flgs, &rule->ref, &rule->use, &rule->metric, &m) != 8)
			{
				syslog(LOG_ERR, "format error, %s", __FUNCTION__);
				free(running_rules);
				fclose(fp);
				return NULL;
			}
			rule->dest.s_addr = d;
			rule->gw.s_addr = g;
			rule->netmask.s_addr = m;

			if (!(rule->flgs & 0x1))	// skip not usable
				continue;

			strncpy(dest_str, inet_ntoa(rule->dest), sizeof(dest_str));
			strncpy(gw_str, inet_ntoa(rule->gw), sizeof(gw_str));
			strncpy(netmask_str, inet_ntoa(rule->netmask), sizeof(netmask_str));

			// check if internal routing rules
			strcpy(rule->comment, "");
			if ((index=findRoutingRule(rules_str, one_rule, dest_str, netmask_str, gw_str, rule->interface)) != -1)
			{
				if (index < rule_count)
				{
					running_rules[index] = 1;
					rule->internal_id = index;
				}
				else
					syslog(LOG_ERR, "fatal error in %s", __FUNCTION__);


				if (getNthValueSafe(3, one_rule, ',', rule->iftype, sizeof(rule->iftype)) == -1)
					strcpy(rule->iftype, "'LAN'");  // 9

				if (getNthValueSafe(6, one_rule, ',', rule->comment, sizeof(rule->comment)) == -1)
					rule->comment[0] = '\0';

				rule->category = 1;

			}
			else
			{
				rule->iftype[0] = '\0';
				rule->comment[0] = '\0';
//				rule->internal_id = -1;
				rule->category = 2;
			}


			rule_num++;

    			if (rule_num == res_arr_capacity)
			{
			    res_arr_capacity += 32;
			    void* nnc = realloc(table,res_arr_capacity*sizeof(struct RoutingRule));
			    if (nnc == NULL) {
	    			syslog(LOG_ERR, "Memory allocation error, %s", __FUNCTION__);
				break;
			    }

			    table = (struct RoutingRule*)nnc;		    
	    		}

			table[rule_num].interface[0] = '\0';

		}
		nl++;
	}

	fclose(fp);

	for (index=0; index < rule_count; index++)
	{

    		if (rule_num == res_arr_capacity)
		{
		    res_arr_capacity += 32;
		    void* nnc = realloc(table,res_arr_capacity*sizeof(struct RoutingRule));
		    if (nnc == NULL) {
			syslog(LOG_ERR, "Memory allocation error, %s", __FUNCTION__);
			break;
		    }

		    table = (struct RoutingRule*)nnc;
    		}

		struct RoutingRule rule;

		if (running_rules[index])
		{
			syslog(LOG_ERR, "%s: rule is already running!", __FUNCTION__);
			continue;
		}

		if(getNthValueSafe(index, rules_str, ';', one_rule, sizeof(one_rule)) == -1)
		{
			syslog(LOG_ERR, "%s: rule separation error!", __FUNCTION__);
			continue;
		}

		if(getNthValueSafe(0, one_rule, ',', dest_str, sizeof(dest_str)) == -1)
		{
			syslog(LOG_ERR, "%s: rule dest get error!", __FUNCTION__);
			continue;
		}

		if (inet_aton(dest_str, &(rule.dest)) == 0)
		{
			syslog(LOG_ERR, "%s: rule dest aton error!", __FUNCTION__);
			continue;
		}

		if(getNthValueSafe(1, one_rule, ',', netmask_str, sizeof(netmask_str)) == -1)
		{
			syslog(LOG_ERR, "%s: rule netmask get error!", __FUNCTION__);
			continue;
		}

		if (inet_aton(netmask_str, &(rule.netmask)) == 0)
		{
			syslog(LOG_ERR, "%s: rule netmask aton error!", __FUNCTION__);
			continue;
		}

		if(getNthValueSafe(2, one_rule, ',', gw_str, sizeof(gw_str)) == -1)
		{
			syslog(LOG_ERR, "%s: rule gw get error!", __FUNCTION__);
			continue;
		}

		if (inet_aton(gw_str, &(rule.gw)) == 0)
		{
			syslog(LOG_ERR, "%s: rule gw aton error!", __FUNCTION__);
			continue;
		}

		if(getNthValueSafe(3, one_rule, ',', rule.iftype, sizeof(rule.iftype)) == -1)
		{
			syslog(LOG_ERR, "%s: rule iftype get error!", __FUNCTION__);
			continue;
		}

		if(getNthValueSafe(4, one_rule, ',', rule.interface, sizeof(rule.interface)) == -1)
		{
			syslog(LOG_ERR, "%s: rule interface name get error!", __FUNCTION__);
			continue;
		}

		if(getNthValueSafe(6, one_rule, ',', rule.comment, sizeof(rule.comment)) == -1)
		{
			syslog(LOG_ERR, "%s: rule comment get error!", __FUNCTION__);
			continue;
		}

		rule.internal_id = index;
		rule.flgs = 0;
		rule.ref = 0;
		rule.use = 0;
		rule.metric = 0;
		rule.category = 3;

		nl++;

		memcpy(&(table[rule_num]), &rule, sizeof(struct RoutingRule));

		rule_num++;
		table[rule_num].interface[0] = '\0';
	}


	if (running_rules)
		free(running_rules);

	if (res_rule_num)
		(*res_rule_num) = rule_num;

	return table;
}

/* removeRoutingRuleNvram - Remove routing rule from NVRAM
 *
 * arg: iftype  - interface type: 'LAN','WAN','VPN' or 'Custom'
 * arg: dest    - destination ip address
 * arg: netmask -
 * arg: gw      - gateway
 * return: 0 = OK
 */
int removeRoutingRuleNvram(const char *iftype, const char *dest, const char *netmask, const char *gw)
{
	char rule[256];
	char c_iftype[32], c_dest[32], c_netmask[32], c_gw[32];
	char *old = nvram_get(RT2860_NVRAM, "RoutingRules");

	// Check that NVRAM contains variable
	if (old == NULL)
	    return 1;

	// Create temporary buffer
	char *buf = (char *)calloc(1, strlen(old) + sizeof(char)*2);
	if (buf == NULL)
	    return 2;

	char *p = buf;
	buf[0] = '\0';

	char *head = old;
	char *tail = strchr(head, ';');
	if (tail == NULL)
		tail = head + strlen(head);

	while (tail != head)
	{
		do
		{
			// Create record
			int count = tail-head;
			memcpy(rule, head, count);
			rule[count] = '\0';

			// Get destination
			if ((getNthValueSafe(0, rule, ',', c_dest, sizeof(c_dest)) == -1))
				continue;
			// Get netmask
			if ((getNthValueSafe(1, rule, ',', c_netmask, sizeof(c_netmask)) == -1))
				continue;
			// Get gateway
			if ((getNthValueSafe(2, rule, ',', c_gw, sizeof(c_gw)) == -1))
				continue;
			// Get interface
			if ((getNthValueSafe(3, rule, ',', c_iftype, sizeof(c_iftype)) == -1))
				continue;

			// Check if rule not matches
			if (!((strcmp(iftype, c_iftype)==0) && (strcmp(dest, c_dest)==0) &&
			    (strcmp(netmask, c_netmask)==0) && (strcmp(gw, c_gw)==0))) {
				// Store rule
				if (strlen(buf) > 0)
				    *(p++) = ';';

				strcpy(p, rule);
				p += strlen(rule);
			}
			// else skip adding rule
		} while (0);

		// Move pointer to next entry
		head = tail;
		if (head[0] == ';')
			head++;
		tail = strchr(head, ';');
		if (tail == NULL)
			tail = head + strlen(head);
	}
	// Write new buffer to NVRAM
	nvram_set(RT2860_NVRAM, "RoutingRules", buf);
	free(buf);

	return 0;
}

/* addRoutingRuleNvram - Add routing rule to NVRAM
 *
 * arg: iftype    - interface type: 'LAN','WAN','VPN' or 'Custom'
 * arg: dest      - destination ip address
 * arg: netmask   -
 * arg: gw        - gateway
 * arg: ifname    - interface name
 * arg: custom_if - interface name (when iftype == 'Custom')
 * arg: comment   - text comment describing the rule
 */
void addRoutingRuleNvram(const char *iftype, const char *dest, const char *netmask, const char *gw, const char *ifname, const char *custom_if, const char *comment)
{
	char cmd[256];

	char *rrs = nvram_get(RT2860_NVRAM, "RoutingRules");
	if (rrs == NULL)
		rrs = "";

	// Create new item
	sprintf(cmd, "%s,%s,%s,%s,%s,%s,%s", dest, netmask, gw, iftype, ifname, custom_if, comment);

	int len = strlen(rrs);
	char *tmp = (char *)calloc(1, len + strlen(cmd) + sizeof(char)*2);
	char *p = tmp;
	if (tmp == NULL)
		return;

	// Write previous rules if exist
	if (len > 0)
	{
		strcpy(tmp, rrs);
		p += len;
		*(p++) = ';';
	}

	strcpy(p, cmd);

	// Set NVRAM variable
	nvram_set(RT2860_NVRAM, "RoutingRules", tmp);

	// Clear resource
	free(tmp);
}

/* getDhcpClientList - Get DHCP client list
 *
 * arg: (out) rownum_out - number of output array entries
 * arg: (out) written_at - leases file timestamp
 * return: dyn_lease struct array (don't forget to free it afterwards) or NULL if error
 */
struct dyn_lease* getDhcpClientList(int *rownum_out, uint64_t *written_at)
{
	struct dyn_lease lease;
	FILE *fp;
	int leases_arr_capacity = 32;
	struct dyn_lease *arr;
	int64_t curr;
	int rownum;

	rownum = 0;
	/* if DHCP is disabled - just exit */
	int dhcpEnabled = nvram_get_int(RT2860_NVRAM, "dhcpEnabled", 0);
	if (!dhcpEnabled)
		return 0;

	/* refresh lease file */
	system("killall -q -SIGUSR1 udhcpd > /dev/null 2>&1");

	fp = fopen("/var/udhcpd.leases", "r");
	if (!fp)
		return 0;

	/* Read header of dhcpleases */
	if ( fread(written_at, 1, sizeof(int64_t), fp) != sizeof(int64_t) )
	{
		fclose(fp);
		return 0;
	}

	(*written_at) = ntoh64(*written_at);
	curr = time(NULL);
	if (curr < (*written_at))
		(*written_at) = curr; /* lease file from future! :) */

	arr = calloc(leases_arr_capacity, sizeof(struct dyn_lease));

	/* Output leases file */
	while (fread(&lease, 1, sizeof(struct dyn_lease), fp) == sizeof(struct dyn_lease))
	{
		memcpy(&(arr[rownum]),&lease,sizeof(struct dyn_lease));
		rownum++;

		if (rownum == leases_arr_capacity)
		{
		    leases_arr_capacity += 32;
		    void* rearr = realloc(arr, leases_arr_capacity*sizeof(struct dyn_lease));
		    if (rearr == NULL)
			break; /* Not enough memory to read one more block */

		    arr = (struct dyn_lease*)rearr;
		}
	}

	fclose(fp);

	*rownum_out = rownum;
	return arr;
}

/* getDNSAddressStr
 * arg: index         - dns index, 1 = Primary DNS, 2 = Secondary DNS
 * arg: (out) out_buf - output char[16] buffer
 * description: write DNS ip address accordingly
 */
int getDNSAddressStr(int index, char* out_buf)
{
        FILE *fp;
        char buf[80] = {0}, ns_str[11], dns[16] = {0};
        int idx = 0;

        if (index != 1 && index != 2)
        {
            return 1;
        }

        fp = fopen("/etc/resolv.conf", "r");

        if (fp == NULL)
        {
            return 2;
        }

        while ((fgets(buf, sizeof(buf), fp)) != NULL)
        {
            if (sscanf(buf, "%s %s", ns_str, dns) != 2)
                continue;

            if (strcasecmp(ns_str, "nameserver") != 0)
                continue;

            idx++;
            if (idx == index) {
                strlcpy(out_buf, dns, 16);
                break;
            }
        }

        fclose(fp);
        return 0;
}

/* getWANGateway
 * arg: (out) sgw - wan gateway, char[16] buffer
 * description: write WAN gateway interface name to buffer
 * return: 0 = OK
 */
int getWANGateway(char* sgw)
{
	char   buff[256];
	char   ifname[16];
	int    nl = 0 ;
	struct in_addr dest;
	struct in_addr gw;
	int    flgs, ref, use, metric;
	unsigned long int d,g,m;
	int    find_default_flag = 0;

	FILE *fp = fopen("/proc/net/route", "r");

	while ((fgets(buff, sizeof(buff), fp)) !=NULL) {
		if (nl) {
			int ifl = 0;
			while (buff[ifl]!=' ' && buff[ifl]!='\t' && buff[ifl]!='\0') {
				ifname[ifl] = buff[ifl];
				ifl++;
			}
			ifname[ifl] = 0;
			buff[ifl] = 0;    /* interface */
			if (sscanf(buff+ifl+1, "%lx%lx%X%d%d%d%lx",
						&d, &g, &flgs, &ref, &use, &metric, &m)!=7) {
				fclose(fp);
				return 1;
			}

			if (flgs&RTF_UP) {
				dest.s_addr = d;
				gw.s_addr   = g;
				strcpy(sgw, (gw.s_addr==0 ? ifname : inet_ntoa(gw)));

				if (dest.s_addr == 0) {
					find_default_flag = 1;
					break;
				}
			}
		}
		nl++;
	}
	fclose(fp);

	if (find_default_flag == 1)
		return 0;
	else
		return 2;
}

static int addRoutingRule(char *buf, const char *dest, const char *netmask, const char *gw, const char *true_if)
{
	char cmd[256];

	// action, destination
	sprintf(cmd, "ip route $1 %s", dest);

	// netmask
	if (strcmp(netmask, "255.255.255.255")!=0)
		sprintf(cmd, "%s/%s", cmd, netmask);

	// interface
	sprintf(cmd, "%s dev %s", cmd, true_if);

	// gateway
	if (strlen(gw) && strcmp(gw, "0.0.0.0") != 0)
		sprintf(cmd, "%s via %s", cmd, gw);

	if (buf != NULL)
	{
		strcpy(buf, cmd);
		return 1;
	}
	else
	{
		FILE *fp;
		strcat(cmd, " 2>&1 ");
		printf("%s\n", cmd);

		fp = popen(cmd, "r");
		if (!fp)
			return 0;

		fgets(cmd, sizeof(cmd), fp);

		pclose(fp);
		return strlen(cmd) == 0;
	}
}

static void rebuildVPNRoutes(char *src_rrs)
{
	// Check if we are not in bridge mode
	char *op_mode = nvram_get(RT2860_NVRAM, "OperationMode");
	if (strcmp(op_mode, "0") == 0)
		return;

	// Now generate file
	int nwritten=0, index=0;
	char one_rule[256], iface[32], dest[32], netmask[32], gw[32];

	char *rrs = (src_rrs != NULL) ? src_rrs : nvram_get(RT2860_NVRAM, "RoutingRules");
	if (rrs == NULL)
		return;

	FILE *fd = fopen(_PATH_PPP_ROUTES, "w");
	if (fd == NULL)
	{
		syslog(LOG_ERR, "open %s failed, %s", _PATH_PPP_ROUTES, __FUNCTION__);
		return;
	}

	fputs("#/bin/sh\n\n", fd);

	while (getNthValueSafe(index++, rrs, ';', one_rule, sizeof(one_rule)) != -1)
	{
		// Get & check interface
		if (getNthValueSafe(3, one_rule, ',', iface, sizeof(iface)) == -1)
			continue;
		if (strcmp(iface, "VPN") != 0)
			continue;

		// Get destination
		if (getNthValueSafe(0, one_rule, ',', dest, sizeof(dest)) == -1)
			continue;

		// Get netmask
		if (getNthValueSafe(1, one_rule, ',', netmask, sizeof(netmask)) == -1)
			continue;

		// Get gateway
		if (getNthValueSafe(2, one_rule, ',', gw, sizeof(gw)) == -1)
			continue;

		addRoutingRule(one_rule, dest, netmask, gw, "$2");
		fprintf(fd, "%s\n", one_rule);
		nwritten++;
	}

	fclose(fd);

	if (nwritten <= 0)
		unlink(_PATH_PPP_ROUTES);
	else
		chmod(_PATH_PPP_ROUTES, S_IXGRP | S_IXUSR | S_IRUSR | S_IWUSR | S_IRGRP);
}

static void rebuildLANWANRoutes(char *src_rrs)
{
	// Get work mode
	char *op_mode = nvram_get(RT2860_NVRAM, "OperationMode");
	int isBridgeMode = (strcmp(op_mode, "0") == 0) ? 1 : 0;

	// Now generate file
	int nwritten=0, index=0;
	char one_rule[256], iface[32], dest[32], netmask[32], gw[32], custom_iface[32];

	char *rrs = (src_rrs != NULL) ? src_rrs : nvram_get(RT2860_NVRAM, "RoutingRules");
	if (rrs == NULL)
		return;

	FILE *fd = fopen(_PATH_LANWAN_ROUTES, "w");
	if (fd == NULL)
	{
		syslog(LOG_ERR, "open %s failed, %s", _PATH_LANWAN_ROUTES, __FUNCTION__);
		return;
	}

	fputs("#/bin/sh\n\n", fd);

	while (getNthValueSafe(index++, rrs, ';', one_rule, sizeof(one_rule)) != -1)
	{
		// Get & check interface
		if (getNthValueSafe(3, one_rule, ',', iface, sizeof(iface)) == -1)
			continue;
		if (strcmp(iface, "VPN") == 0)
			continue;
		else if ((strcmp(iface, "WAN")==0) && isBridgeMode)
		{
			syslog(LOG_ERR, "Skip WAN routing rule in the non-Gateway mode: %s,%s", one_rule, __FUNCTION__);
			continue;
		}

		// Get destination
		if (getNthValueSafe(0, one_rule, ',', dest, sizeof(dest)) == -1)
			continue;

		// Get netmask
		if (getNthValueSafe(1, one_rule, ',', netmask, sizeof(netmask)) == -1)
			continue;

		// Get gateway
		if (getNthValueSafe(2, one_rule, ',', gw, sizeof(gw)) == -1)
			continue;

		if ((getNthValueSafe(5, one_rule, ',', custom_iface, sizeof(custom_iface)) == -1))
			continue;

		// Check interface
		if (strcmp(iface, "WAN")==0)
			strcpy(custom_iface, "$3");
		else if (strcmp(iface, "LAN")==0)
			strcpy(custom_iface, "$2");
		else if (strcmp(iface, "Custom")==0)
		{
			if (strlen(custom_iface)<=0)
				strcpy(custom_iface, "$3");
		}
		else
		{
			strcpy(iface, "LAN");
			strcpy(custom_iface, "$2");
		}

		addRoutingRule(one_rule, dest, netmask, gw, custom_iface);
		fprintf(fd, "%s\n", one_rule);
		nwritten++;
	}

	fclose(fd);

	if (nwritten <= 0)
		unlink(_PATH_LANWAN_ROUTES);
	else
		chmod(_PATH_LANWAN_ROUTES, S_IXGRP | S_IXUSR | S_IRUSR | S_IWUSR | S_IRGRP);
}

void static_routing_rebuild_etc(void)
{
	// Get routing rules
	char *rrs = nvram_get(RT2860_NVRAM, "RoutingRules");

	// Remove static routes scripts
	remove(_PATH_PPP_ROUTES);
	remove(_PATH_LANWAN_ROUTES);

	// And rebuild VPN routes
	rebuildLANWANRoutes(rrs);
	rebuildVPNRoutes(rrs);
}

/* parse_portforward_rule - Parse portforward rule into struct by its number
 *
 * arg: rulestr    - rules string to parse
 * arg: (out) rule - output rule structure
 * arg: rule_num   - index of rule to parse
 * return: 0 = OK
 */
int parse_portforward_rule(char* rulestr, struct PortForwardRule* rule, int rule_num)
{
	int prf_int, prt_int, rprf_int, rprt_int;
	char protostr[8];
	char nat_loopback_str[8];
	char rec[128];

	if (rulestr == NULL)
		return 1;
	if (strlen(rulestr) == 0)
		return 2;

	/* format is :
	 * [interface],[protocol],[src_port],[dst_port],[ip_address],[redirect_src_port],[redirect_dst_port],[nat_loopback],[comment];
	 */
	if (getNthValueSafe(rule_num, rulestr, ';', rec, sizeof(rec)) == -1)
		return 3;

	// get interface
	if ((getNthValueSafe(0, rec, ',', rule->interface, sizeof(rule->interface)) == -1))
		return 4;

	// get protocol
	if ((getNthValueSafe(1, rec, ',', protostr, sizeof(protostr)) == -1))
		return 5;

	rule->protocol = atoi(protostr);
	switch(rule->protocol)
	{
		case PROTO_TCP:
		case PROTO_UDP:
		case PROTO_TCP_UDP:
			break;
		default:
			return 6;
	}

	// get port range "from"
	if ((getNthValueSafe(2, rec, ',', rule->prf, sizeof(rule->prf)) == -1))
		return 7;

	if (strlen(rule->prf) > 0)
	{
		if ((prf_int = atoi(rule->prf)) == 0 || prf_int > 65535)
			return 8;
	}

	// get port range "to"
	if ((getNthValueSafe(3, rec, ',', rule->prt, sizeof(rule->prt)) == -1))
		return 9;

	if (strlen(rule->prt) > 0)
	{
		if ((prt_int = atoi(rule->prt)) > 65535)
			return 10;
	}

	// get ip address
	if ((getNthValueSafe(4, rec, ',', rule->ip_address, sizeof(rule->ip_address)) == -1))
		return 11;

	if (!isIpValid(rule->ip_address))
		return 12;

	// get forward port range "from"
	if ((getNthValueSafe(5, rec, ',', rule->rprf, sizeof(rule->rprf)) == -1))
		return 13;

	if (strlen(rule->rprf) > 0)
	{
		if ((rprf_int = atoi(rule->rprf)) == 0 || rprf_int > 65535)
			return 14;
	}

	// get port range "to"
	if ((getNthValueSafe(6, rec, ',', rule->rprt, sizeof(rule->rprt)) == -1))
		return 15;

	if (strlen(rule->rprt) > 0)
	{
		if ((rprt_int = atoi(rule->rprt)) > 65535)
			return 16;
	}

	// get Nat Loopback enable flag
	if ((getNthValueSafe(7, rec, ',', nat_loopback_str, sizeof(nat_loopback_str)) == -1))
		return 17;

	if (strlen(nat_loopback_str) > 0)
		rule->nat_loopback = atoi(nat_loopback_str);
	else
		rule->nat_loopback = 0;


	// Get comment
	if ((getNthValueSafe(8, rec, ',', rule->comment, sizeof(rule->comment)) == -1))
		return 18;

	return 0;
}

/* parse_portfiltering_rule - Parse portfiltering rule into struct by its number
 *
 * arg: rulestr    - rules string to parse
 * arg: (out) rule - output rule structure
 * arg: rule_num   - index of rule to parse
 * return: 0 = OK
 */
int parse_portfiltering_rule(char* rulestr, struct PortFilteringRule* rule, int rule_num)
{
	int sprf_int, sprt_int, dprf_int, dprt_int;
	char protostr[8];
	char rec[256];

	if (rulestr == NULL)
		return 1;

	if (getNthValueSafe(rule_num, rulestr, ';', rec, sizeof(rec)) == -1)
		return 2;

	// Get interface
	if ((getNthValueSafe(0, rec, ',', rule->interface, sizeof(rule->interface)) == -1))
		return 3;

	// get protocol
	if ((getNthValueSafe(1, rec, ',', protostr, sizeof(protostr)) == -1))
		return 4;

	rule->protocol = atoi(protostr);
	switch (rule->protocol)
	{
		case PROTO_TCP:
		case PROTO_UDP:
		case PROTO_NONE:
		case PROTO_ICMP:
			break;
		default:
			return 5;
	}

	// get mac address
	if ((getNthValueSafe(2, rec, ',', rule->mac_address, sizeof(rule->mac_address)) == -1))
		return 6;

	// get source ip
	if ((getNthValueSafe(3, rec, ',', rule->sip, sizeof(rule->sip)) == -1))
		return 7;

	if (!isIpNetmaskValid(rule->sip))
		rule->sip[0] = '\0';

	// get source ip mask
	if ((getNthValueSafe(4, rec, ',', rule->sim, sizeof(rule->sim)) == -1))
		return 8;

	if (!isIpNetmaskValid(rule->sim))
		rule->sim[0] = '\0';

	// get source port range "from"
	if ((getNthValueSafe(5, rec, ',', rule->sprf, sizeof(rule->sprf)) == -1))
		return 9;

	if ((sprf_int = atoi(rule->sprf)) > 65535)
		return 10;

	// get source port range "to"
	if ((getNthValueSafe(6, rec, ',', rule->sprt, sizeof(rule->sprt)) == -1))
		return 11;

	if ((sprt_int = atoi(rule->sprt)) > 65535)
		return 12;

	// get destination ip
	if ((getNthValueSafe(7, rec, ',', rule->dip, sizeof(rule->dip)) == -1))
		return 13;

	if (!isIpNetmaskValid(rule->dip))
		rule->dip[0] = '\0';

	// get destination ip mask
	if ((getNthValueSafe(8, rec, ',', rule->dim, sizeof(rule->dim)) == -1))
		return 14;

	if (!isIpNetmaskValid(rule->dim))
		rule->dim[0] = '\0';

	// get destination port range "from"
	if ((getNthValueSafe(9, rec, ',', rule->dprf, sizeof(rule->dprf)) == -1))
		return 15;

	if ((dprf_int = atoi(rule->dprf)) > 65535)
		return 16;

	// get destination port range "to"
	if ((getNthValueSafe(10, rec, ',', rule->dprt, sizeof(rule->dprt)) == -1))
		return 17;

	if ((dprt_int = atoi(rule->dprt)) > 65535)
		return 18;

	// get action / policy
	if ((getNthValueSafe(11, rec, ',', rule->action, sizeof(rule->action)) == -1))
		return 19;

	// get comment
	if ((getNthValueSafe(12, rec, ',', rule->comment, sizeof(rule->comment)) == -1))
		return 20;

	return 0;


}

static int checkNatLoopback(char *rule)
{
	char nat_loopback[8];
	char rec[256];
	int inat_loopback, i=0;

	while ( (getNthValueSafe(i++, rule, ';', rec, sizeof(rec)) != -1) )
	{
		// get Nat Loopback enable flag
		if ((getNthValueSafe(7, rec, ',', nat_loopback, sizeof(nat_loopback)) == -1))
			continue;

		if (strlen(nat_loopback) > 0)
			inat_loopback = atoi(nat_loopback);
		else
			inat_loopback = 0;

		if (inat_loopback)
			return 1;
	}

	return 0;
}

static void makeIPPortFilterRule(char *buf, int len, char *iface, char *mac_address,
	char *sip, char *sim, int sprf_int, int sprt_int,
	char *dip, char *dim, int dprf_int, int dprt_int,
	int proto, int action, const char *chain)
{
	int rc = 0;
	char *pos = buf;

	rc = snprintf(pos, len, "iptables -A %s ", chain);

	pos += rc;
	len -= rc;

	if (iface != NULL)
	{
		rc = snprintf(pos, len, "-i %s ", iface);
		pos += rc;
		len -= rc;
	}

	// write mac address
	if ((mac_address!=NULL) && (strlen(mac_address)>0))
	{
		rc = snprintf(pos, len, "-m mac --mac-source %s ", mac_address);
		pos += rc;
		len -= rc;
	}

	// write source ip
	if ((sip == NULL) || (strlen(sip)<=0))
		sip = "any/0";

	if ((sim==NULL) || (strlen(sim)==0) || (strcmp(sim, "255.255.255.255")==0))
		rc = snprintf(pos, len, "-s %s ", sip);
	else
		rc = snprintf(pos, len, "-s %s/%s ", sip, sim);

	pos += rc;
	len -= rc;

	// write destination ip
	if ((dip == NULL) || (strlen(dip)<=0))
		dip = "any/0";

	if ((dim==NULL) || (strlen(dim)==0) || (strcmp(dim, "255.255.255.255")==0))
		rc = snprintf(pos, len, "-d %s ", dip);
	else
		rc = snprintf(pos, len, "-d %s/%s ", dip, dim);

	pos += rc;
	len -= rc;

	// write protocol type
	if (proto == PROTO_ICMP)
	{
		rc = snprintf(pos, len, "-p icmp ");
		pos += rc;
		len -= rc;
	}
	else if ((proto == PROTO_TCP) || (proto == PROTO_UDP))
	{
		if (proto == PROTO_TCP)
			rc = snprintf(pos, len, "-p tcp ");
		else // UDP
			rc = snprintf(pos, len, "-p udp ");
		pos += rc;
		len -= rc;

		// write source port
		if (sprf_int>0)
		{
			if (sprt_int>0)
				rc = snprintf(pos, len, "--sport %d:%d ", sprf_int, sprt_int);
			else
				rc = snprintf(pos, len, "--sport %d ", sprf_int);
			pos += rc;
			len -= rc;
		}

		// write dest port
		if (dprf_int>0)
		{
			if (dprt_int>0)
				rc = snprintf(pos, len, "--dport %d:%d ", dprf_int, dprt_int);
			else
				rc = snprintf(pos, len, "--dport %d ", dprf_int);
			pos += rc;
			len -= rc;
		}
	}

	switch(action)
	{
		case ACTION_DROP:			// 1 == ENABLE--DROP mode
			rc = snprintf(pos, len, "-j DROP");
			break;
		case ACTION_ACCEPT:			// 2 == ENABLE--ACCEPT mode
			rc = snprintf(pos, len, "-j ACCEPT");
			break;
	}

	pos += rc;
	len -= rc;
	rc = snprintf(pos, len, "\n");
}

static void makePortForwardRule(char *buf, int len, char *wan_name, char *ip_address, int proto, int prf_int, int prt_int, int rprf_int, int rprt_int, int inat_loopback)
{
	int rc = 0;
	char *pos = buf;

	char *lan_ip = nvram_get(RT2860_NVRAM, "lan_ipaddr");
	char *lan_nm = nvram_get(RT2860_NVRAM, "lan_netmask");
	char *lan_if = getLanIfName();

	// Add nat loopback
	if (inat_loopback)
	{
		//DNAT
		rc = snprintf(pos, len, "iptables -t nat -A %s -i %s -d $1 ", PORT_FORWARD_PRE_CHAIN, lan_if);
		pos += rc;
		len -= rc;

		// write protocol type
		if (proto == PROTO_TCP)
			rc = snprintf(pos, len, "-p tcp ");
		else if (proto == PROTO_UDP)
			rc = snprintf(pos, len, "-p udp ");
		else if (proto == PROTO_TCP_UDP)
			rc = snprintf(pos, len, " ");
		pos += rc;
		len -= rc;

		// write dst port
		if (prf_int != 0)
		{
			rc = (prt_int != 0) ?
			snprintf(pos, len, "--dport %d:%d ", prf_int, prt_int) :
			snprintf(pos, len, "--dport %d ", prf_int);
			pos += rc;
			len -= rc;
		}
		rc = snprintf(pos, len, "-j DNAT --to-destination %s", ip_address);
		pos += rc;
		len -= rc;
		// write dst port
		if (rprf_int != 0)
		{
			rc = (rprt_int != 0) ?
				snprintf(pos, len, ":%d-%d ", rprf_int, rprt_int) :
				snprintf(pos, len, ":%d ", rprf_int);
			pos += rc;
			len -= rc;
		}
		rc = snprintf(pos, len, "\n");
		pos += rc;
		len -= rc;

		//MASQ
		rc = snprintf(pos, len, "iptables -t nat -A %s -s %s/%s -d %s ", PORT_FORWARD_POST_CHAIN, lan_ip, lan_nm, ip_address);
		pos += rc;
		len -= rc;

		// write protocol type
		if (proto == PROTO_TCP)
			rc = snprintf(pos, len, "-p tcp ");
		else if (proto == PROTO_UDP)
			rc = snprintf(pos, len, "-p udp ");
		else if (proto == PROTO_TCP_UDP)
			rc = snprintf(pos, len, " ");
		pos += rc;
		len -= rc;

		// write dst port
		if (rprf_int != 0)
		{
			rc = (prt_int != 0) ?
			snprintf(pos, len, "--dport %d:%d ", rprf_int, rprt_int) :
			snprintf(pos, len, "--dport %d ", rprf_int);
			pos += rc;
			len -= rc;
		} else {
			if (prf_int != 0)
			{
				rc = (prt_int != 0) ?
				snprintf(pos, len, "--dport %d:%d ", prf_int, prt_int) :
				snprintf(pos, len, "--dport %d ", prf_int);
				pos += rc;
				len -= rc;
			}
		}
		rc = snprintf(pos, len, "-j MASQUERADE\n");
		pos += rc;
		len -= rc;
	}

	// Add forwarding rule
	rc = snprintf(pos, len, "iptables -t nat -A %s -i %s ! -d %s/%s ", PORT_FORWARD_PRE_CHAIN, wan_name, lan_ip, lan_nm);
	pos += rc;
	len -= rc;

	// write protocol type
	if (proto == PROTO_TCP)
		rc = snprintf(pos, len, "-p tcp ");
	else if (proto == PROTO_UDP)
		rc = snprintf(pos, len, "-p udp ");
	else if (proto == PROTO_TCP_UDP)
		rc = snprintf(pos, len, " ");
	pos += rc;
	len -= rc;

	// write src port
	if (prf_int != 0)
	{
		rc = (prt_int != 0) ?
			snprintf(pos, len, "--dport %d:%d ", prf_int, prt_int) :
			snprintf(pos, len, "--dport %d ", prf_int);
		pos += rc;
		len -= rc;
	}

	// write remote ip
	rc = snprintf(pos, len, "-j DNAT --to-destination %s", ip_address);
	pos += rc;
	len -= rc;

	// write dst port
	if (rprf_int != 0)
	{
		rc = (rprt_int != 0) ?
			snprintf(pos, len, ":%d-%d ", rprf_int, rprt_int) :
			snprintf(pos, len, ":%d ", rprf_int);
		pos += rc;
		len -= rc;
	}

	rc = snprintf(pos, len, "\n");
}

static void makePortForwardRuleVPN(char *buf, int len, char *wan_name, char *ip_address, int proto, int prf_int, int prt_int, int rprf_int, int rprt_int, int inat_loopback)
{
	int rc = 0;
	char *pos = buf;

	char *lan_ip = nvram_get(RT2860_NVRAM, "lan_ipaddr");
	char *lan_nm = nvram_get(RT2860_NVRAM, "lan_netmask");
	char *lan_if = getLanIfName();

	// Add nat loopback
	if (inat_loopback)
	{
		//DNAT
		rc = snprintf(pos, len, "iptables -t nat -$1 %s -i %s -d $3 ", PORT_FORWARD_PRE_CHAIN_VPN, lan_if);
		pos += rc;
		len -= rc;

		// write protocol type
		if (proto == PROTO_TCP)
			rc = snprintf(pos, len, "-p tcp ");
		else if (proto == PROTO_UDP)
			rc = snprintf(pos, len, "-p udp ");
		else if (proto == PROTO_TCP_UDP)
			rc = snprintf(pos, len, " ");
		pos += rc;
		len -= rc;

		// write dst port
		if (prf_int != 0)
		{
			rc = (prt_int != 0) ?
			snprintf(pos, len, "--dport %d:%d ", prf_int, prt_int) :
			snprintf(pos, len, "--dport %d ", prf_int);
			pos += rc;
			len -= rc;
		}
		rc = snprintf(pos, len, "-j DNAT --to-destination %s", ip_address);
		pos += rc;
		len -= rc;
		// write dst port
		if (rprf_int != 0)
		{
			rc = (rprt_int != 0) ?
				snprintf(pos, len, ":%d-%d ", rprf_int, rprt_int) :
				snprintf(pos, len, ":%d ", rprf_int);
			pos += rc;
			len -= rc;
		}
		rc = snprintf(pos, len, "\n");
		pos += rc;
		len -= rc;



		//MASQ
		rc = snprintf(pos, len, "iptables -t nat -$1 %s -s %s/%s -d %s ", PORT_FORWARD_POST_CHAIN_VPN, lan_ip, lan_nm, ip_address);
		pos += rc;
		len -= rc;

		// write protocol type
		if (proto == PROTO_TCP)
			rc = snprintf(pos, len, "-p tcp ");
		else if (proto == PROTO_UDP)
			rc = snprintf(pos, len, "-p udp ");
		else if (proto == PROTO_TCP_UDP)
			rc = snprintf(pos, len, " ");
		pos += rc;
		len -= rc;

		// write dst port
		if (rprf_int != 0)
		{
			rc = (prt_int != 0) ?
			snprintf(pos, len, "--dport %d:%d ", rprf_int, rprt_int) :
			snprintf(pos, len, "--dport %d ", rprf_int);
			pos += rc;
			len -= rc;
		} else {
			if (prf_int != 0)
			{
				rc = (prt_int != 0) ?
				snprintf(pos, len, "--dport %d:%d ", prf_int, prt_int) :
				snprintf(pos, len, "--dport %d ", prf_int);
				pos += rc;
				len -= rc;
			}
		}
		rc = snprintf(pos, len, "-j MASQUERADE\n");
		pos += rc;
		len -= rc;
	}

	// Add forwarding rule
	rc = snprintf(pos, len, "iptables -t nat -$1 %s -i %s ! -d %s/%s ", PORT_FORWARD_PRE_CHAIN_VPN, wan_name, lan_ip, lan_nm);
	pos += rc;
	len -= rc;

	// write protocol type
	if (proto == PROTO_TCP)
		rc = snprintf(pos, len, "-p tcp ");
	else if (proto == PROTO_UDP)
		rc = snprintf(pos, len, "-p udp ");
	else if (proto == PROTO_TCP_UDP)
		rc = snprintf(pos, len, " ");
	pos += rc;
	len -= rc;

	// write src port
	if (prf_int != 0)
	{
		rc = (prt_int != 0) ?
			snprintf(pos, len, "--dport %d:%d ", prf_int, prt_int) :
			snprintf(pos, len, "--dport %d ", prf_int);
		pos += rc;
		len -= rc;
	}

	// write remote ip
	rc = snprintf(pos, len, "-j DNAT --to-destination %s", ip_address);
	pos += rc;
	len -= rc;

	// write dst port
	if (rprf_int != 0)
	{
		rc = (rprt_int != 0) ?
			snprintf(pos, len, ":%d-%d ", rprf_int, rprt_int) :
			snprintf(pos, len, ":%d ", rprf_int);
		pos += rc;
		len -= rc;
	}

	rc = snprintf(pos, len, "\n");
}

static void makePortForwardAccesRule(char *buf, int len, char *wan_name, char *ip_address, int proto, int prf_int, int prt_int, int rprf_int, int rprt_int, int inat_loopback)
{
	int rc = 0;
	char *pos = buf;
	char *lan_if = getLanIfName();

	// Add forwarding rule
	rc = snprintf(pos, len, "iptables -A FORWARD -i %s -o %s -d %s ", wan_name, lan_if, ip_address);

	pos += rc;
	len -= rc;

	// write protocol type
	if (proto == PROTO_TCP)
		rc = snprintf(pos, len, "-p tcp ");
	else if (proto == PROTO_UDP)
		rc = snprintf(pos, len, "-p udp ");
	else if (proto == PROTO_TCP_UDP)
		rc = snprintf(pos, len, " ");
	pos += rc;
	len -= rc;

	// write dst port
	if (rprf_int != 0)
	{
		rc = (prt_int != 0) ?
			snprintf(pos, len, "--dport %d:%d ", rprf_int, rprt_int) :
			snprintf(pos, len, "--dport %d ", rprf_int);
		pos += rc;
		len -= rc;
	}

	// write remote ip
	rc = snprintf(pos, len, "-j ACCEPT");
	pos += rc;
	len -= rc;

	rc = snprintf(pos, len, "\n");
}

static void iptablesIPPortFilterBuildScript(void)
{
	int i = 0, mode, sprf_int, sprt_int, proto, action, dprf_int, dprt_int;
	char rec[256];
	char cmd[1024];
	char sprf[8], sprt[8], protocol[8], iface[8];
	char dprf[8], dprt[8], wan_name[16];
	char mac_address[32], action_str[4];
	char sip[32], dip[32], sim[32], dim[32];
	char *c_if, *firewall_enable, *default_policy, *rule;
	int default_drop_flag;

	// Check that IP/port filter is enabled
	firewall_enable = nvram_get(RT2860_NVRAM, "IPPortFilterEnable");
	if (firewall_enable == NULL)
	{
		syslog(LOG_WARNING, "can't find \"IPPortFilterEnable\" in nvram, %s", __FUNCTION__);
		return;
	}
	mode = atoi(firewall_enable);
	if (mode == 0)
		return;

	rule = nvram_get(RT2860_NVRAM, "IPPortFilterRules");
	if (rule == NULL)
	{
		syslog(LOG_WARNING, "can't find \"IPPortFilterRules\" in nvram, %s", __FUNCTION__);
		return;
	}

	// add the default policy to the end of FORWARD chain
	default_policy = nvram_get(RT2860_NVRAM, "DefaultFirewallPolicy");
	if (default_policy == NULL)
		default_policy = "0";
	default_drop_flag = atoi(default_policy);

	// get wan name
	strncpy(wan_name, getWanIfName(), sizeof(wan_name)-1);

	//Generate portforward script file
	FILE *fd = fopen(_PATH_MACIP_FILE, "w");
	if (fd != NULL)
	{
		fputs("#!/bin/sh\n\n", fd);
		// make chain and jump to from forward
		fprintf(fd, "iptables -t filter -N %s\n", IPPORT_FILTER_CHAIN);
		fprintf(fd, "iptables -t filter -I FORWARD -j %s\n\n", IPPORT_FILTER_CHAIN);
		// accept related/established sessions by conntrack
		fprintf(fd, "iptables -t filter -A %s -m state --state ESTABLISHED,RELATED -j ACCEPT\n\n", IPPORT_FILTER_CHAIN);

		while ((getNthValueSafe(i++, rule, ';', rec, sizeof(rec)) != -1))
		{
			// Get interface
			if ((getNthValueSafe(0, rec, ',', iface, sizeof(iface)) == -1))
				continue;

			if (strcmp(iface, "LAN")==0)
				c_if = "br0";
			else if (strcmp(iface, "VPN")==0)
				c_if = "ppp+";
			else
				c_if = wan_name;

			// get protocol
			if ((getNthValueSafe(1, rec, ',', protocol, sizeof(protocol)) == -1))
				continue;
			proto = atoi(protocol);
			switch(proto)
			{
				case PROTO_TCP:
				case PROTO_UDP:
				case PROTO_NONE:
				case PROTO_ICMP:
					break;
				default:
					continue;
			}

			// get mac address
			if ((getNthValueSafe(2, rec, ',', mac_address, sizeof(mac_address)) == -1))
				continue;

			if (strlen(mac_address) > 0)
			{
				if (!isMacValid(mac_address))
					continue;
			}

			// get source ip
			if ((getNthValueSafe(3, rec, ',', sip, sizeof(sip)) == -1))
				continue;
			if (!isIpNetmaskValid(sip))
				sip[0] = '\0';

			// get source ip mask
			if ((getNthValueSafe(4, rec, ',', sim, sizeof(sim)) == -1))
				continue;
			if (!isIpNetmaskValid(sim))
				sim[0] = '\0';

			// get source port range "from"
			if ((getNthValueSafe(5, rec, ',', sprf, sizeof(sprf)) == -1))
				continue;
			if ((sprf_int = atoi(sprf)) > 65535)
				continue;

			// get source port range "to"
			if ((getNthValueSafe(6, rec, ',', sprt, sizeof(sprt)) == -1))
				continue;
			if ((sprt_int = atoi(sprt)) > 65535)
				continue;

			// get destination ip
			if ((getNthValueSafe(7, rec, ',', dip, sizeof(dip)) == -1))
				continue;
			if (!isIpNetmaskValid(dip))
				dip[0] = '\0';

			// get destination ip mask
			if ((getNthValueSafe(8, rec, ',', dim, sizeof(dim)) == -1))
				continue;
			if (!isIpNetmaskValid(dim))
				dim[0] = '\0';

			// get destination port range "from"
			if ((getNthValueSafe(9, rec, ',', dprf, sizeof(dprf)) == -1))
				continue;
			if ((dprf_int = atoi(dprf)) > 65535)
				continue;

			// get destination port range "to"
			if ((getNthValueSafe(10, rec, ',', dprt, sizeof(dprt)) == -1))
				continue;
			if ((dprt_int = atoi(dprt)) > 65535)
				continue;

			// get action / policy
			if ((getNthValueSafe(11, rec, ',', action_str, sizeof(action_str)) == -1))
				continue;

			action = atoi(action_str);

			// build rules
			makeIPPortFilterRule(
				cmd, sizeof(cmd), c_if, mac_address, sip, sim, sprf_int,
				sprt_int, dip, dim, dprf_int, dprt_int, proto, action, IPPORT_FILTER_CHAIN);
			// write to file
			fputs(cmd, fd);
		}

		// write default policy
		if (default_drop_flag)
		    snprintf(cmd, 1024, "iptables -t filter -A %s -j DROP\n\n", IPPORT_FILTER_CHAIN);
		else
		    snprintf(cmd, 1024, "iptables -t filter -A %s -j ACCEPT\n\n", IPPORT_FILTER_CHAIN);

		// write to file
		fputs(cmd, fd);

		//close file
		fclose(fd);
		chmod(_PATH_MACIP_FILE, S_IXGRP | S_IXUSR | S_IRUSR | S_IWUSR | S_IRGRP);
	}
}

static void iptablesIPPortFilterInputBuildScript(void)
{
	int i = 0, mode, sprf_int, sprt_int, proto, action, dprf_int, dprt_int;
	char rec[256];
	char cmd[1024];
	char sprf[8], sprt[8], protocol[8], iface[8];
	char dprf[8], dprt[8], wan_name[16];
	char mac_address[32], action_str[4];
	char sip[32], dip[32], sim[32], dim[32];
	char *c_if, *firewall_enable, *rule;

	// Check that IP/port filter is enabled
	firewall_enable = nvram_get(RT2860_NVRAM, "IPPortFilterInputEnable");
	if (firewall_enable == NULL)
	{
		syslog(LOG_WARNING, "can't find \"IPPortFilterInputEnable\" in nvram, %s", __FUNCTION__);
		return;
	}
	mode = atoi(firewall_enable);
	if (mode == 0)
		return;

	rule = nvram_get(RT2860_NVRAM, "IPPortFilterInputRules");
	if (rule == NULL)
	{
		syslog(LOG_WARNING, "can't find \"IPPortFilterInputRules\" in nvram, %s", __FUNCTION__);
		return;
	}

	// get wan name
	strncpy(wan_name, getWanIfName(), sizeof(wan_name)-1);

	//Generate input script file
	FILE *fd = fopen(_PATH_MACIPINPUT_FILE, "w");
	if (fd != NULL)
	{
		fputs("#!/bin/sh\n\n", fd);

		while ((getNthValueSafe(i++, rule, ';', rec, sizeof(rec)) != -1))
		{
			// Get interface
			if ((getNthValueSafe(0, rec, ',', iface, sizeof(iface)) == -1))
				continue;

			if (strcmp(iface, "LAN")==0)
				c_if = "br0";
			else if (strcmp(iface, "VPN")==0)
				c_if = "ppp+";
			else
				c_if = wan_name;

			// get protocol
			if ((getNthValueSafe(1, rec, ',', protocol, sizeof(protocol)) == -1))
				continue;
			proto = atoi(protocol);
			switch(proto)
			{
				case PROTO_TCP:
				case PROTO_UDP:
				case PROTO_NONE:
				case PROTO_ICMP:
					break;
				default:
					continue;
			}

			// get mac address
			if ((getNthValueSafe(2, rec, ',', mac_address, sizeof(mac_address)) == -1))
				continue;

			if (strlen(mac_address) > 0)
			{
				if (!isMacValid(mac_address))
					continue;
			}

			// get source ip
			if ((getNthValueSafe(3, rec, ',', sip, sizeof(sip)) == -1))
				continue;
			if (!isIpNetmaskValid(sip))
				sip[0] = '\0';

			// get source ip mask
			if ((getNthValueSafe(4, rec, ',', sim, sizeof(sim)) == -1))
				continue;
			if (!isIpNetmaskValid(sim))
				sim[0] = '\0';

			// get source port range "from"
			if ((getNthValueSafe(5, rec, ',', sprf, sizeof(sprf)) == -1))
				continue;
			if ((sprf_int = atoi(sprf)) > 65535)
				continue;

			// get source port range "to"
			if ((getNthValueSafe(6, rec, ',', sprt, sizeof(sprt)) == -1))
				continue;
			if ((sprt_int = atoi(sprt)) > 65535)
				continue;

			// get destination ip
			if ((getNthValueSafe(7, rec, ',', dip, sizeof(dip)) == -1))
				continue;
			if (!isIpNetmaskValid(dip))
				dip[0] = '\0';

			// get destination ip mask
			if ((getNthValueSafe(8, rec, ',', dim, sizeof(dim)) == -1))
				continue;
			if (!isIpNetmaskValid(dim))
				dim[0] = '\0';

			// get destination port range "from"
			if ((getNthValueSafe(9, rec, ',', dprf, sizeof(dprf)) == -1))
				continue;
			if ((dprf_int = atoi(dprf)) > 65535)
				continue;

			// get destination port range "to"
			if ((getNthValueSafe(10, rec, ',', dprt, sizeof(dprt)) == -1))
				continue;
			if ((dprt_int = atoi(dprt)) > 65535)
				continue;

			// get action / policy
			if ((getNthValueSafe(11, rec, ',', action_str, sizeof(action_str)) == -1))
				continue;

			action = atoi(action_str);

			// build rules
			makeIPPortFilterRule(
				cmd, sizeof(cmd), c_if, mac_address, sip, sim, sprf_int,
				sprt_int, dip, dim, dprf_int, dprt_int, proto, action, "servicelimit");
			// write to file
			fputs(cmd, fd);
		}
		// close file
		fclose(fd);
		chmod(_PATH_MACIPINPUT_FILE, S_IXGRP | S_IXUSR | S_IRUSR | S_IWUSR | S_IRGRP);
	}
}

static void iptablesPortForwardBuildScript(void)
{
	char rec[256];
	char cmd[1024];
	char wan_name[16];
	char ip_address[32], prf[8], prt[8], rprf[9], rprt[8], protocol[8], interface[8], nat_loopback[8];
	char *c_if, *a_if, *firewall_enable, *rule;

	int i=0, prf_int, prt_int, rprf_int, rprt_int, proto, inat_loopback, nat_loopback_on;
	FILE *fd, *fd_vpn, *fd_access;

	//Remove portforward script
	firewall_enable = nvram_get(RT2860_NVRAM, "PortForwardEnable");
	if (!firewall_enable)
	{
		syslog(LOG_WARNING, "can't find \"PortForwardEnable\" in flash, %s", __FUNCTION__);
		return;
	}

	if (atoi(firewall_enable))
	{
		rule = nvram_get(RT2860_NVRAM, "PortForwardRules");
		if(!rule)
		{
			syslog(LOG_WARNING, "can't find \"PortForwardRules\" in flash, %s", __FUNCTION__);
			return;
		}
	}
	else
		return;

	nat_loopback_on = checkNatLoopback(rule);

	// get wan name
	strncpy(wan_name, getWanIfName(), sizeof(wan_name)-1);

	// Generate portforward script file
	fd = fopen(_PATH_PFW_FILE, "w");
	if (fd == NULL)
		return;

	// Print header for WAN/LAN
	fputs("#!/bin/sh\n\n", fd);
	fprintf(fd,
		"iptables -t nat -N %s\n"
		"iptables -t nat -A PREROUTING -j %s\n\n",
		PORT_FORWARD_PRE_CHAIN, PORT_FORWARD_PRE_CHAIN);

	// Additional rules if port forwarding enabled
	if (nat_loopback_on)
		fprintf(fd,
			"iptables -t nat -N %s\n"
			"iptables -t nat -A POSTROUTING -j %s\n\n",
			PORT_FORWARD_POST_CHAIN, PORT_FORWARD_POST_CHAIN);

	chmod(_PATH_PFW_FILE, S_IXGRP | S_IXUSR | S_IRUSR | S_IWUSR | S_IRGRP);

	// Open file for VPN
	fd_vpn = fopen(_PATH_PFW_FILE_VPN, "w");
	if (fd_vpn == NULL)
	{
		fclose(fd);
		return;
	}

	// Print header for VPN
	fputs("#!/bin/sh\n\n", fd_vpn);
	fprintf(fd_vpn,
		"iptables -t nat -N %s\n"
		"iptables -t nat -A PREROUTING -j %s\n\n",
		PORT_FORWARD_PRE_CHAIN_VPN, PORT_FORWARD_PRE_CHAIN_VPN);

	// Additional rules if port forwarding enabled
	if (nat_loopback_on)
		fprintf(fd_vpn,
			"iptables -t nat -N %s\n"
			"iptables -t nat -A POSTROUTING -j %s\n\n",
			PORT_FORWARD_POST_CHAIN_VPN, PORT_FORWARD_POST_CHAIN_VPN);

	chmod(_PATH_PFW_FILE_VPN, S_IXGRP | S_IXUSR | S_IRUSR | S_IWUSR | S_IRGRP);

	// Open file for ACCESS
	fd_access = fopen(_PATH_PFW_FILE_ACCESS, "w");
	if (fd_access == NULL)
	{
		fclose(fd);
		fclose(fd_vpn);
		return;
	}

	// Print header for ACESS
	fputs("#!/bin/sh\n\n", fd_access);

	chmod(_PATH_PFW_FILE_ACCESS, S_IXGRP | S_IXUSR | S_IRUSR | S_IWUSR | S_IRGRP);

	// Now write all rules
	while((getNthValueSafe(i++, rule, ';', rec, sizeof(rec)) != -1))
	{
		// get interface
		if ((getNthValueSafe(0, rec, ',', interface, sizeof(interface)) == -1))
			continue;

		// get protocol
		if ((getNthValueSafe(1, rec, ',', protocol, sizeof(protocol)) == -1))
			continue;

		proto = atoi(protocol);
		switch(proto)
		{
			case PROTO_TCP:
			case PROTO_UDP:
			case PROTO_TCP_UDP:
				break;
			default:
				continue;
		}

		// get port range "from"
		if ((getNthValueSafe(2, rec, ',', prf, sizeof(prf)) == -1))
			continue;

		if (strlen(prf) > 0)
		{
			if ((prf_int = atoi(prf)) == 0 || prf_int > 65535)
				continue;
		}
		else
			prf_int = 0;

		// get port range "to"
		if ((getNthValueSafe(3, rec, ',', prt, sizeof(prt)) == -1))
			continue;

		if (strlen(prt) > 0)
		{
			if ((prt_int = atoi(prt)) > 65535)
				continue;
		}
		else
			prt_int = 0;

		// get ip address
		if ((getNthValueSafe(4, rec, ',', ip_address, sizeof(ip_address)) == -1))
			continue;

		if (!isIpValid(ip_address))
			continue;

		// get forward port range "from"
		if ((getNthValueSafe(5, rec, ',', rprf, sizeof(rprf)) == -1))
			continue;

		if (strlen(rprf) > 0)
		{
			rprf_int = atoi(rprf);
			if (rprf_int > 65535)
				continue;
		}
		else
			rprf_int = 0;

		// get port range "to"
		if ((getNthValueSafe(6, rec, ',', rprt, sizeof(rprt)) == -1))
			continue;

		if (strlen(rprt) > 0)
		{
			if ((rprt_int = atoi(rprt)) > 65535)
				continue;
		}
		else
			rprt_int = 0;

		// get Nat Loopback enable flag
		if ((getNthValueSafe(7, rec, ',', nat_loopback, sizeof(nat_loopback)) == -1))
			continue;

		if (strlen(nat_loopback) > 0)
			inat_loopback = atoi(nat_loopback);
		else
			inat_loopback = 0;

		int is_vpn = 0;
		// Patch interface
		if (strcmp(interface, "LAN")==0)
			a_if = c_if = "br0";
		else if (strcmp(interface, "VPN")==0)
		{
			c_if = "$2";
			a_if = "ppp+";
			is_vpn = 1;
		}
		else
		{
			a_if = c_if = wan_name;
		}

		switch(proto)
		{
			case PROTO_TCP:
			case PROTO_UDP:
				if (is_vpn)
				{
					makePortForwardRuleVPN(cmd, sizeof(cmd), c_if, ip_address, proto, prf_int, prt_int, rprf_int, rprt_int, inat_loopback);
					fputs(cmd, fd_vpn);
				}
				else
				{
					makePortForwardRule(cmd, sizeof(cmd), c_if, ip_address, proto, prf_int, prt_int, rprf_int, rprt_int, inat_loopback);
					fputs(cmd, fd);
				}
				makePortForwardAccesRule(cmd, sizeof(cmd), a_if, ip_address, proto, prf_int, prt_int, rprf_int, rprt_int, inat_loopback);
				fputs(cmd, fd_access);
				break;
			case PROTO_TCP_UDP:
				if (is_vpn)
				{
					makePortForwardRuleVPN(cmd, sizeof(cmd), c_if, ip_address, PROTO_TCP, prf_int, prt_int, rprf_int, rprt_int, inat_loopback);
					fputs(cmd, fd_vpn);
					makePortForwardRuleVPN(cmd, sizeof(cmd), c_if, ip_address, PROTO_UDP, prf_int, prt_int, rprf_int, rprt_int, inat_loopback);
					fputs(cmd, fd_vpn);
				}
				else
				{
					makePortForwardRule(cmd, sizeof(cmd), c_if, ip_address, PROTO_TCP, prf_int, prt_int, rprf_int, rprt_int, inat_loopback);
					fputs(cmd, fd);
					makePortForwardRule(cmd, sizeof(cmd), c_if, ip_address, PROTO_UDP, prf_int, prt_int, rprf_int, rprt_int, inat_loopback);
					fputs(cmd, fd);
				}
				makePortForwardAccesRule(cmd, sizeof(cmd), a_if, ip_address, PROTO_TCP, prf_int, prt_int, rprf_int, rprt_int, inat_loopback);
				fputs(cmd, fd_access);
				makePortForwardAccesRule(cmd, sizeof(cmd), a_if, ip_address, PROTO_UDP, prf_int, prt_int, rprf_int, rprt_int, inat_loopback);
				fputs(cmd, fd_access);
				break;
		}
	}

	// Close files
	fclose(fd_access);
	fclose(fd_vpn);
	fclose(fd);
}

/* Same as the file "linux/netfilter_ipv4/ipt_webstr.h" */
#define BLK_JAVA                0x01
#define BLK_ACTIVE              0x02
#define BLK_COOKIE              0x04
#define BLK_PROXY               0x08
static void iptablesWebsFilterBuildScript(void)
{
	int i, content_filter = 0;
	char entry[256]; //need long buffer for utf domain name encoding support
	char *proxy		= nvram_get(RT2860_NVRAM, "websFilterProxy");
	char *java		= nvram_get(RT2860_NVRAM, "websFilterJava");
	char *activex		= nvram_get(RT2860_NVRAM, "websFilterActivex");
	char *cookies		= nvram_get(RT2860_NVRAM, "websFilterCookies");
	char *url_filter	= nvram_get(RT2860_NVRAM, "websURLFilters");
	char *host_filter	= nvram_get(RT2860_NVRAM, "websHostFilters");

	if ((url_filter && strlen(url_filter) && getRuleNums(url_filter)) ||
		(host_filter && strlen(host_filter) && getRuleNums(host_filter)) ||
			atoi(proxy) || atoi(java) || atoi(activex) || atoi(cookies))
	{
		// Content filter
		if(atoi(java))
			content_filter += BLK_JAVA;
		if(atoi(activex))
			content_filter += BLK_ACTIVE;
		if(atoi(cookies))
			content_filter += BLK_COOKIE;
		if(atoi(proxy))
			content_filter += BLK_PROXY;

		//Generate portforward script file
		FILE *fd = fopen(_PATH_WEBS_FILE, "w");

		if (fd != NULL)
		{
			fputs("#!/bin/sh\n\n", fd);
			fprintf(fd, "iptables -t filter -N %s\n", WEB_FILTER_CHAIN);
			fprintf(fd, "iptables -t filter -A FORWARD -j %s\n", WEB_FILTER_CHAIN);

			if (content_filter)
			{
				// Why only 3 ports are inspected?(This idea is from CyberTAN source code)
				// TODO: use layer7 to inspect HTTP
				fprintf(fd, "iptables -A %s -p tcp -m tcp --dport 80   -m webstr --content %d -j REJECT --reject-with tcp-reset\n", WEB_FILTER_CHAIN, content_filter);
				fprintf(fd, "iptables -A %s -p tcp -m tcp --dport 3128 -m webstr --content %d -j REJECT --reject-with tcp-reset\n", WEB_FILTER_CHAIN, content_filter);
				fprintf(fd, "iptables -A %s -p tcp -m tcp --dport 8080 -m webstr --content %d -j REJECT --reject-with tcp-reset\n", WEB_FILTER_CHAIN, content_filter);
			}

			// URL filter
			i=0;
			while ((i < getRuleNums(url_filter)) && (getNthValueSafe(i, url_filter, ';', entry, sizeof(entry)) != -1))
			{
				if (strlen(entry))
				{
					if (!strncasecmp(entry, "http://", strlen("http://")))
						strcpy(entry, entry + strlen("http://"));

					fprintf(fd, "iptables -A %s -p tcp -m tcp -m webstr --url  %s -j REJECT --reject-with tcp-reset\n", WEB_FILTER_CHAIN, entry);
				}
				i++;
			}

			// HOST(Keyword) filter
			i=0;
			while ((i < getRuleNums(host_filter)) && (getNthValueSafe(i, host_filter, ';', entry, sizeof(entry)) != -1))
			{
				if (strlen(entry))
				{
					fprintf(fd, "iptables -A %s -p tcp -m tcp -m webstr --host %s -j REJECT --reject-with tcp-reset\n", WEB_FILTER_CHAIN, entry);
				}
				i++;
			}

			//closefile
			fclose(fd);
			chmod(_PATH_WEBS_FILE, S_IXGRP | S_IXUSR | S_IRUSR | S_IWUSR | S_IRGRP);
		}
	}
}

void firewall_rebuild_etc(void)
{
	// Rebuild firewall scripts in etc

	// Port forwarding
	char *pfw_enable = nvram_get(RT2860_NVRAM, "PortForwardEnable");
	if (pfw_enable == NULL)
		pfw_enable = "0";

	// Remove portforwards scripts
	remove(_PATH_PFW_FILE);
	remove(_PATH_PFW_FILE_VPN);
	remove(_PATH_PFW_FILE_ACCESS);

	// Generate new
	if (strcmp(pfw_enable, "1") == 0) // Turned on?
		iptablesPortForwardBuildScript();

	// IP/Port/MAC filtering
	char *ipf_enable = nvram_get(RT2860_NVRAM, "IPPortFilterEnable");
	char *ipfinput_enable = nvram_get(RT2860_NVRAM, "IPPortFilterInputEnable");
	if (ipf_enable == NULL)
		ipf_enable = "0";
	if (ipfinput_enable == NULL)
		ipfinput_enable = "0";

	// Remove mac filter script
	remove(_PATH_MACIP_FILE);
	if (strcmp(ipf_enable, "1") == 0) // Turned on?
		iptablesIPPortFilterBuildScript();

	// Remove mac filter script
	remove(_PATH_MACIPINPUT_FILE);
	if (strcmp(ipfinput_enable, "1") == 0) // Turned on?
		iptablesIPPortFilterInputBuildScript();

	// Web filtering
	remove(_PATH_WEBS_FILE);
	iptablesWebsFilterBuildScript();
}
