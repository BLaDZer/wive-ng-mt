/*
 EULA:

 WIVE FIRMWARE IS FREE FOR THE NON-COMMERCIAL USE ONLY.

 Conditions of commercial use non GPL (or other not viral license) components are discussed individually.
 For a more detailed discussion please mail me.
 News and changes in Russian: http://wive-ng.sf.net
 Bug report please send to support[at]nag.ru or sfstudio[at]mail.ru
*/

#include "libwive.h"

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

	strlcpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
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
	char result[96],*r;
	unsigned int i, state;

	fp = popen("ps|grep lanaut[h]", "r");
	if(!fp){
		syslog(LOG_ERR, "no ps util exist, %s", __FUNCTION__);
		return -1;
	}
	fgets(result, sizeof(result), fp);
	pclose(fp);
	for (i=0, state=0, r=result; *r && i<sizeof(result) ;i++,r++)
	{
		switch(state)
		{
			case 0:
				if (*r == '-')
					state++;
				break;
			case 1:
				if (*r == 'A')
					state++;
				else
					state = 0;
				break;
			case 2:
				if (*r == ' ' || *r == '\t')
					state = 3;
				else
					state = 0;
				break;
			case 3:
				if (*r == '0')
					return 1;
				else if (*r == '1')
					return 2;
				else if (*r == '2')
					return 3;
				else
					state = 0;
			default:
				break;
		}
	}
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
		char *vpn_type = nvram_get(RT2860_NVRAM, "vpnType");

		if (CHK_IF_DIGIT(vpn_type, 3)) {
			status = (getLANAUTHState() + 1) % 5;
		} else {
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
    {
        st_table = lanauth_statuses;
    }
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
