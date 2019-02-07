#include "libwive.h"

#define ROUTING_TABLE_FILE "routing_table.conf"

const char* getVPNStatusStr()
{
    char *vpnEnabled = nvram_get(RT2860_NVRAM, "vpnEnabled");
    if ( strcmp(vpnEnabled, "on") == 0) return "online";
    return "disabled";
}

int getIfIp(const char *ifname, char *if_addr)
{
    strcpy(if_addr, "0.0.0.0");
    return 0;
}

void static_routing_rebuild_etc(void)
{
}

void firewall_rebuild_etc(void)
{
}

#ifdef CONFIG_IPV6
struct RoutingRuleIPv6* parseRoutingTableIPv6(int *res_rule_num)
{
	return NULL;
}
#endif

struct dyn_lease* getDhcpClientList(int *rownum, uint64_t *written_at)
{
    return NULL;
}

char* getLanIfName(void)
{
    return "lan0";
}

char* getWanIfName(void)
{
    return "wan0";
}

char *getLanWanNamebyIf(const char *ifname)
{
    return "if";
}

char* getPPPIfName(void)
{
    return "ppp0";
}

int getWANGateway(char* sgw)
{
    strcpy(sgw, "wan0");
    return 0;
}

int getIfIsReady(const char *ifname)
{
    return 0;
}

int getIfMac(const char *ifname, char *if_hw, char separator)
{
    strcpy(if_hw, "00:00:00:01:02:03");
    return 0;
}

int getIfNetmask(const char *ifname, char *if_net)
{
    strcpy(if_net,"255.0.0.0");
    return 0;
}

int vpn_mode_enabled(void)
{
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

	if( (! strcmp("any", str)) || (! strcmp("any/0", str)))
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

	FILE *fp = fopen(ROUTING_TABLE_FILE, "r");
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


#ifdef CONFIG_IPV6
int getIfIPv6(const char *ifname, char *if_addr, char *netmask)
{
	strcpy(if_addr, "::");
	return 0;
}

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

int getIPv6ExtIPAddr(char* address, char* mask) {
	static char tmpif[IFNAMSIZ] = "";
	static char wanif[IFNAMSIZ] = "";
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

