#include "libnvram.h"
#include "libnvram_utils.h"

#include <syslog.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>


#define T(s) s

char *strip_space(char *str)
{
	while (isspace(*str))
		str++;
	return str;
}

static int isNumOnly(char *str){
	int i, len = strlen(str);
	for(i=0; i<len; i++){
		if((str[i] >= '0' && str[i] <= '9'))
			continue;
		return 0;
	}
	return 1;
}

static int isAllNumAndSlash(char *str){
	int i, len = strlen(str);
	for(i=0; i<len; i++){
		if( (str[i] >= '0' && str[i] <= '9') || str[i] == '.' || str[i] == '/' )
			continue;
		return 0;
	}
	return 1;
}

static int isOnlyOneSlash(char *str)
{
	int i, count=0;
	int len = strlen(str);
	for(i=0; i<len; i++)
		if( str[i] == '/')
			count++;
	return count <= 1 ? 1 : 0;
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
	static char *if_name = "br0";
	FILE *fp;
	char lan_if[16]; /* max 16 char in lan if name */

	/* set default */
	if (mode == NULL)
		return if_name;

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
	    if_name = "eth2";
	else
	    if_name = "br0";

	return if_name;
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
 * arguments: index - index of the Nth value
 *            values - un-parsed values
 * description: parse values delimited by semicolon, and return the value
 *              according to given index (starts from 0)
 * WARNING: the original values will be destroyed by strtok
 */
char *getNthValue(int index, char *values)
{
	int i;
	static char *tok;

	if (NULL == values)
		return NULL;
	for (i = 0, tok = strtok(values, ";");
			(i < index) && tok;
			i++, tok = strtok(NULL, ";")) {
		;
	}
	if (NULL == tok)
		return "";
	return tok;
}

/*
 * substitution of getNthValue which dosen't destroy the original value
 */
int getNthValueSafe(int index, char *value, char delimit, char *result, int len)
{
    int i = 0, result_len = 0;
    char *begin, *end;

    if(!value || !result || !len)
        return -1;

    begin = value;
    end = strchr(begin, delimit);
    while(i < index && end != NULL){
        begin = end + 1;
        end = strchr(begin, delimit);
        i++;
    }

    /* no delimit */
    if(!end){
	if(i == index){
	    end = begin + strlen(begin);
	    result_len = (len-1) < (end-begin) ? (len-1) : (end-begin);
	} else
		return -1;
    } else
	result_len = (len-1) < (end-begin) ? (len-1) : (end-begin);

    memcpy(result, begin, result_len);
    *(result + result_len ) = '\0';

	return 0;
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

int getNums(char *value, char delimit)
{
    char *pos = value;
    int count = 1;
    if (!pos || !(*pos))
        return 0;

    while ((pos = strchr(pos, delimit)) != NULL) {
        ++pos;
        ++count;
    }
    return count;
}

static int getRuleNums(char *rules)
{
	return getNums(rules, ';');
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
	if (ipf_enable == NULL)
		ipf_enable = "0";

	// Remove mac filter script
	remove(_PATH_MACIP_FILE);
	if (strcmp(ipf_enable, "1") == 0) // Turned on?
		iptablesIPPortFilterBuildScript();

	// Web filtering
	remove(_PATH_WEBS_FILE);
	iptablesWebsFilterBuildScript();
}

/*
 * arguments: index - index of the Nth value (starts from 0)
 *            old_values - un-parsed values
 *            new_value - new value to be replaced
 * description: parse values delimited by semicolon,
 *              replace the Nth value with new_value,
 *              and return the result
 * WARNING: return the internal static string -> use it carefully
 */
char *setNthValue(int index, char *old_values, char *new_value)
{
	int i;
	char *p, *q;
	static char ret[2048];
	char buf[8][256];

	memset(ret, 0, 2048);
	for (i = 0; i < 8; i++)
		memset(buf[i], 0, 256);

	/* copy original values */
	for ( i = 0, p = old_values, q = strchr(p, ';')  ;
	      i < 8 && q != NULL                         ;
	      i++, p = q + 1, q = strchr(p, ';')         )
		strncpy(buf[i], p, q - p);

	if (i > 7) /* limit of buf size = 8 */
	    i=7;

	strcpy(buf[i], p); /* the last one */

	/* replace buf[index] with new_value */
	strncpy(buf[index], new_value, 256);

	/* calculate maximum index */
	index = (i > index)? i : index;

	/* concatenate into a single string delimited by semicolons */
	strcat(ret, buf[0]);
	for (i = 1; i <= index; i++) {
		strncat(ret, ";", 2);
		snprintf(ret, sizeof(ret), "%s%s", ret,  buf[i]);
	}

	p = ret;
	return p;
}

/*
 *  argument:  [IN]     index -- the index array of deleted items(begin from 0)
 *             [IN]     count -- deleted itmes count.
 *             [IN/OUT] value -- original string/return string
 *             [IN]     delimit -- delimitor
 */
int deleteNthValueMulti(int index[], int count, char *value, char delimit)
{
	char *begin, *end;
	int i=0, j=0;
	int need_check_flag=0;
	char *buf = strdup(value);

	begin = buf;

	end = strchr(begin, delimit);
	while(end != NULL){
		if(i == index[j]){
			memset(begin, 0, end - begin );
			if(index[j] == 0)
				need_check_flag = 1;
			j++;
			if(j >=count)
				break;
		}
		begin = end;
		end = strchr(begin+1, delimit);
		i++;
	}

	if(!end && index[j] == i)
		memset(begin, 0, strlen(begin));

	if(need_check_flag){
		for(i=0; i < (int)strlen(value); i++){
			if(buf[i] == '\0')
				continue;
			if(buf[i] == ';')
				buf[i] = '\0';
			break;
		}
	}

	for(i=0, j=0; i < (int)strlen(value); i++){
		if(buf[i] != '\0'){
			value[j++] = buf[i];
		}
	}
	value[j] = '\0';

	free(buf);
	return 0;
}

void STFs(int nvram, int index, char *flash_key, char *value)
{
	char *result;
	char *tmp = nvram_get(nvram, flash_key);
	if(!tmp)
		tmp = "";
	result = setNthValue(index, tmp, value);
	nvram_bufset(nvram, flash_key, result);
	return ;
}
