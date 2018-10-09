#include "asp_mod_internet.h"

/*
 * Show PPTP VPN status
 */
static int vpnShowVPNStatus(webs_t *wp, char** params, int nparams)
{
        const char *status_str = getVPNStatusStr();
        outWrite(T("%s"), status_str);

        return 0;
}

static void formVPNSetup(webs_t* wp, char_t *path, char_t *query)
{
	char_t *vpn_enabled, *vpn_type;
	char_t *reset = websGetVar(wp, T("reset"), T("0"));

	if (CHK_IF_DIGIT(reset, 1)) {
		nvram_fromdef(RT2860_NVRAM, 20, "vpnEnabled", "vpnType", "vpnServer", "vpnMPPE", "vpnPeerDNS", "vpnDebug",
			"vpnNAT", "vpnDGW", "vpnAuthProtocol", "vpnEnableLCP", "vpnPurePPPOE", "vpnLCPFailure", "vpnLCPInterval",
			"vpnTestReachable", "LANAUTH_LVL", "vpnMTU", "vpnUser", "vpnPassword", "vpnInterface", "vpnService");
	}
	else {
		vpn_enabled = websGetVar(wp, T("vpn_enabled"), T(""));
		vpn_type = websGetVar(wp, T("vpn_type"), T("0"));

		if (vpn_enabled[0] == '\0')
			vpn_enabled="off";

		nvram_init(RT2860_NVRAM);
		ngx_nvram_bufset(wp, "vpnEnabled", vpn_enabled);

		if ((strncmp(vpn_enabled, "on", 3)) || !CHK_IF_DIGIT(vpn_type, 0))
			ngx_nvram_bufset(wp, "vpnPurePPPOE", "0");

		if (!strncmp(vpn_enabled, "on", 3)) {
			parameter_fetch_t *fetch = vpn_args;
#ifdef CONFIG_USER_KABINET
			if (CHK_IF_DIGIT(vpn_type, 3))
			fetch = lanauth_args;
#endif
			setupParameters(wp, fetch, 0);
		}
		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}

	doSystem("service vpnhelper restart");

	websDone(wp, 200);
}

/*
 * arguments: type - 1 = write Primary DNS
 *                   2 = write Secondary DNS
 * description: write DNS ip address accordingly
 */
static int getDns(webs_t *wp, char** params, int nparams)
{
    FILE *fp;
    char buf[80] = {0}, ns_str[11], dns[16] = {0};
    int idx = 0, req = 0;

    if (nparams < 1)
    {
        return NGX_OK; // do not output anything
    }

    char* type = params[0];

    if (type[0] == '1') req = 1;
    else
    if (type[0] == '2') req = 2;
    else
    {
        return NGX_OK; // do not output anything
    }

    fp = fopen("/etc/resolv.conf", "r");
    if (NULL == fp)
    {
        return outWrite(T(""));
    }

    while ((fgets(buf, sizeof(buf), fp)) != NULL)
    {
        if (sscanf(buf, "%s %s", ns_str, dns) != 2)
            continue;
        if (strcasecmp(ns_str, "nameserver") != 0)
            continue;

        idx++;
        if (idx == req) {
            outWrite(T("%s"), dns);
            break;
        }
    }

    fclose(fp);

    return 0;
}

static int getIgmpProxyBuilt(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_USER_IGMP_PROXY
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getVPNBuilt(webs_t *wp, char** params, int nparams)
{
#if defined(CONFIG_NF_CONNTRACK_PPTP) || defined(CONFIG_NF_CONNTRACK_PPTP_MODULE) || \
    defined(CONFIG_IP_NF_PPTP) || defined(CONFIG_IP_NF_PPTP_MODULE)
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getWDSBuilt(webs_t *wp, char** params, int nparams)
{
#if defined(CONFIG_MT7610_AP_WDS) || defined(CONFIG_MT76X2_AP_WDS) || defined(CONFIG_MT76X3_AP_WDS) || defined(CONFIG_MT7615_AP_WDS)
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getMBSSIDBuilt(webs_t *wp, char** params, int nparams)
{
#if defined(CONFIG_MT7610_AP_MBSS) || defined(CONFIG_MT76X2_AP_MBSS) || defined(CONFIG_MT76X3_AP_MBSS) || defined(CONFIG_MT7615_AP_MBSS)
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getUSBBuilt(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_USB
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getIPv6Built(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_IPV6
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getIPv66rdBuilt(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_IPV6_SIT_6RD
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getIP6to4Built(webs_t *wp, char** params, int nparams)
{
#if defined (CONFIG_IPV6_SIT) || defined (CONFIG_IPV6_SIT_MODULE)
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getStorageBuilt(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_USER_STORAGE
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getFtpBuilt(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_FTPD
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getSmbBuilt(webs_t *wp, char** params, int nparams)
{
#if defined(CONFIG_USER_SAMBA)
       return outWrite(T("1"));
#else
       return outWrite(T("0"));
#endif
}

static int getRadiusBuilt(webs_t *wp, char** params, int nparams)
{
#if defined(CONFIG_USER_RADIUS)
       return outWrite(T("1"));
#else
       return outWrite(T("0"));
#endif
}

static int getCwmpBuilt(webs_t *wp, char** params, int nparams)
{
#if defined(CONFIG_USER_CWMPD)
       return outWrite(T("1"));
#else
       return outWrite(T("0"));
#endif
}

static int getPrinterSrvBuilt(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_USER_P910ND
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getUSBModemBuilt(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_USB_MODESWITCH
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getTransmissionBuilt(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_USER_TRANSMISSION
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getDynamicRoutingBuilt(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_USER_ZEBRA
    return outWrite(T("1"));
#else
    return outWrite(T("0"));
#endif
}

static int getSWQoSBuilt(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_RALINKAPP_SWQOS
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getDATEBuilt(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_DATE
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getDDNSBuilt(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_USER_INADYN
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getLANAUTHBuilt(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_USER_KABINET
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getSysLogBuilt(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_SYSLOGD
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

/*
 * description: write LAN ip address accordingly
 */
static int getLanIp(webs_t *wp, char** params, int nparams)
{
	char if_addr[16] = {0};

	if (getIfIp(getLanIfName(), if_addr) == -1) {
		websError(wp, 500, T("getLanIp: calling getIfIp error\n"));
                return 0;
//		return outWrite(T(""));
	}
	return outWrite(T("%s"), if_addr);
}

/*
 * description: write LAN MAC address accordingly
 */
static int getLanMac(webs_t *wp, char** params, int nparams)
{
	char if_mac[18] = {0};

	if (getIfMac(getLanIfName(), if_mac, ':') == -1) {
		websError(wp, 500, T("getLanIp: calling getIfMac error\n"));
                return 0;
//		return outWrite(T(""));
	}
	return outWrite(T("%s"), if_mac);
}

/*
 * description: write LAN subnet mask accordingly
 */
static int getLanNetmask(webs_t *wp, char** params, int nparams)
{
	char if_net[16] = {0};
	char* ifname = getLanIfName();

	if (getIfNetmask(ifname, if_net) == -1) {
		websError(wp, 500, T("getLanNetmask: calling getIfNetmask(ifname=%s) error\n"), ifname);
                return 0;
//		return outWrite(T(""));
	}
	return outWrite(T("%s"), if_net);
}

static int getGWBuilt(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_RTESW_SWITCH_ONEPORT
	return outWrite(T("0"));
#else
	return outWrite(T("1"));
#endif
}

static int getDnsmasqBuilt(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_USER_DNSMASQ
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getCdpBuilt(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_USER_CDP
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}


static int getLltdBuilt(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_USER_LLTD
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getLldpdBuilt(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_USER_LLDPD
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getUpnpBuilt(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_USER_MINIUPNPD
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getXupnpdBuilt(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_USER_XUPNPD
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getSpotBuilt(webs_t *wp, char** params, int nparams)
{
#if defined(CONFIG_USER_CHILLISPOT) || defined(CONFIG_USER_NODOGSPLASH)
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getRadvdBuilt(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_USER_RADVD
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getDhcpv6Built(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_USER_DHCP6_SERVER
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

/*
 * description: write real_wan ip address accordingly
 */
static int getIntIp(webs_t *wp, char** params, int nparams)
{
	char if_addr[16] = {0};
	char* ppp_if_name = getPPPIfName();

	if (vpn_mode_enabled() == 1) {
	    /* if vpn enabled get ip from vpnif else from wan if */
	    if (!ppp_if_name || ppp_if_name[0] == '\0' || getIfIp(ppp_if_name, if_addr) == -1) {

		if (getIfIp(getWanIfName(), if_addr) == -1)
		    return outWrite(T(""));
	    }
	} else {
	    /* if vpn disabled always get ip from wanif */
	    if (getIfIp(getWanIfName(), if_addr) == -1) {
		return outWrite(T(""));
	    }
	}
	return outWrite(T("%s"), if_addr);
}


/*
 * description: write WAN ip address accordingly
 */
static int getWanIp(webs_t *wp, char** params, int nparams)
{
	char if_addr[16] = {0};

	if (getIfIp(getWanIfName(), if_addr) == -1) {
		return outWrite(T(""));
	}

	return outWrite(T("%s"), if_addr);
}

/*
 * description: write WAN MAC address accordingly
 */
static int getWanMac(webs_t *wp, char** params, int nparams)
{
	char if_mac[18];

	if (getIfMac(getWanIfName(), if_mac, ':') == -1) {
		return outWrite(T(""));
	}
	return outWrite(T("%s"), if_mac);
}

/*
 * description: write WAN subnet mask accordingly
 */
static int getWanNetmask(webs_t *wp, char** params, int nparams)
{
	char if_net[16];

	if (getIfNetmask(getWanIfName(), if_net) == -1) {
		return outWrite(T(""));
	}
	return outWrite(T("%s"), if_net);
}

/*
 * description: write WAN default gateway accordingly
 */
static int getWanGateway(webs_t *wp, char** params, int nparams)
{
	char sgw[16];
	switch (getWANGateway(sgw))
	{
		case 0: return outWrite(T("%s"), sgw);
		case 1: return outWrite(T("format error"));
	}

	return outWrite(T(""));
}


/*
 * description: get routing table
 */
static int getRoutingTable(webs_t *wp, char** params, int nparams)
{
	int   i = 0;
	char  dest_str[32], gw_str[32], netmask_str[32];
	int   *running_rules = NULL;
	char  *rrs;
	int   rule_count;

	// Determine work mode
	int isBridgeMode = (nvram_get_int(RT2860_NVRAM, "OperationMode", -1) == 0) ? 1 : 0;
	int isVPN = (strcmp(getVPNStatusStr(), "online") == 0 || 
		     strcmp(getVPNStatusStr(), "kabinet networks") == 0 || 
		     strcmp(getVPNStatusStr(), "full access") == 0) ? 1 : 0;

	rrs = nvram_get(RT2860_NVRAM, "RoutingRules");
	if (rrs == NULL)
		rrs = "";
	rule_count = getNums(rrs, ';');

	// Get routing table
	int parsed_rule_count = 0;
	struct RoutingRule* table = parseRoutingTable(rrs, &parsed_rule_count);

	if (table == NULL) {
		syslog(LOG_ERR, "Routing table parse error in function %s", __FUNCTION__);
		return -1;
	}

	if (rule_count) {
		running_rules = calloc(1, sizeof(int) * rule_count);
		if (!running_rules)
			return -1;
	}

	// true_interface[0], destination[1], gateway[2], netmask[3], flags[4], ref[5], use[6],
	// metric[7], category[8], interface[9], idle[10], comment[11], new[12]
	for (i = 0 ; i < parsed_rule_count; i++) {
		struct RoutingRule rule = table[i];

		strncpy(dest_str, inet_ntoa(rule.dest), sizeof(dest_str));
		strncpy(gw_str, inet_ntoa(rule.gw), sizeof(gw_str));
		strncpy(netmask_str, inet_ntoa(rule.netmask), sizeof(netmask_str));

		if (i > 0) outWrite(T(",\n"));

		outWrite(T("[ '%s', '%s', '%s', '%s', %d, %d, %d, %d"), rule.interface, dest_str, gw_str, netmask_str, rule.flgs, rule.ref, rule.use, rule.metric); // 0-7

		switch (rule.category) {
			case 1:
				outWrite(T(", %d, "), rule.internal_id); // 8

				if (rule.internal_id < rule_count)
					running_rules[rule.internal_id] = 1;
				else
					syslog(LOG_ERR, "fatal error in %s", __FUNCTION__);

				outWrite(T("'%s'"), rule.iftype); // 9
				outWrite(T(", 0, '%s', 0 ]"), rule.comment); // 10-12
				break;
			case 2:
				outWrite(T(", -1, ")); // 8
				outWrite(T("'%s'"), getLanWanNamebyIf(rule.interface)); // 9
				outWrite(T(", 0, '%s', 0 ]"), rule.comment); // 10-12
				break;

			case 3:
				outWrite(T(", %d, "), rule.internal_id); // 8
				outWrite(T("'%s'"), rule.iftype); // 9

				if (strcmp(rule.iftype, "VPN") == 0)
				{
					outWrite(T(", %d, "), (isVPN == 1) ? 0 : 1); // 10
				}
				else
				{
					outWrite(T(", %d, "), 0); // 10
				}

				outWrite(T("'%s', 0 ]"), rule.comment); // 11-12
				break;

			default:
				syslog(LOG_ERR, "%s: unknown routing rule category number: %i", __FUNCTION__, rule.category);
		}
	}

	if (running_rules)
		free(running_rules);

	free(table);

	return 0;
}

static void editRouting(webs_t* wp, char_t *path, char_t *query)
{
	char_t *trans = websGetVar(wp, T("routingTableDiff"), T(""));
#ifdef CONFIG_USER_ZEBRA
	char_t *rip   = websGetVar(wp, T("RIPSelect"), T(""));
#endif
	char_t *reset = websGetVar(wp, T("reset"), T("0"));

	char rec[256];
	char true_iface[32], destination[32], gateway[32], netmask[32], iface[32], c_iface[32], comment[64], action[4];
	int i = 0, iaction;

	if (CHK_IF_DIGIT(reset, 1)) {
		ngx_nvram_set(wp, "RoutingRules", "");
#ifdef CONFIG_USER_ZEBRA
		ngx_nvram_set(wp, "RIPEnable", "0");
#endif
	}
	else {
		while (getNthValueSafe(i++, trans, ';', rec, sizeof(rec)) != -1)
		{
			// Get true interface
			if ((getNthValueSafe(0, rec, ',', c_iface, sizeof(c_iface)) == -1))
				continue;
			// Get destination
			if ((getNthValueSafe(1, rec, ',', destination, sizeof(destination)) == -1))
				continue;
			// Get gateway
			if ((getNthValueSafe(2, rec, ',', gateway, sizeof(gateway)) == -1))
				continue;
			// Get netmask
			if ((getNthValueSafe(3, rec, ',', netmask, sizeof(netmask)) == -1))
				continue;
			// Get interface
			if ((getNthValueSafe(9, rec, ',', iface, sizeof(iface)) == -1))
				continue;
			// Get comment
			if ((getNthValueSafe(11, rec, ',', comment, sizeof(comment)) == -1))
				continue;
			// Get action
			if ((getNthValueSafe(12, rec, ',', action, sizeof(action)) == -1))
				continue;

			// Check action
			iaction = atoi(action);
			if (! ((iaction == 1) || (iaction == 2)))
				continue;

			// Check interface
			strcpy(true_iface, c_iface);
			if (strcmp(iface, "WAN")==0)
				strcpy(true_iface, getWanIfName());
			else if (strcmp(iface, "LAN")==0)
				strcpy(true_iface, getLanIfName());
			else if (strcmp(iface, "Custom")==0)
			{
				if (strlen(true_iface)<=0)
					strcpy(true_iface, getWanIfName());
			}
			else if (strcmp(iface, "VPN")==0)
			{
				strcpy(true_iface, "ppp+");
			}
			else
			{
				strcpy(iface, "LAN");
				strcpy(true_iface, getLanIfName());
			}

			if (iaction == 1) // Add route
			{
				addRoutingRuleNvram(iface, destination, netmask, gateway, true_iface, c_iface, comment);
			}
			else if (iaction == 2) // Remove route
			{
				removeRoutingRuleNvram(iface, destination, netmask, gateway);
			}
		}
#ifdef CONFIG_USER_ZEBRA
		ngx_nvram_set(wp, "RIPEnable", rip);
#endif
	}

	/* reconfigure system */
        wp->on_response_ok = DO_RECONFIGURE;

//	websHeader(wp);
	websDone(wp, 200);
}

static void getMyMAC(webs_t* wp, char_t *path, char_t *query)
{
    char myMAC[32] = {0};

    char* ipaddr = ngx_to_cstring(wp->pool, wp->request->connection->addr_text);
    if (ipaddr == NULL)
    {
        websDone(wp, 500);
        return;
    }

    arplookup(ipaddr, myMAC);
    ngx_pfree(wp->pool, ipaddr);

    websSetContentType(wp, "text/plain");

    outWrite(T("%s"), myMAC);
    websDone(wp, 200);
}

/* goform/setLan */
static void setLan(webs_t* wp, char_t *path, char_t *query)
{
	char_t	*lan_ip, *lan_nm;
	char_t	*start_ip, *end_ip, *dgw;
	char_t	*gw = NULL, *pd = NULL, *sd = NULL, *dns_profile = NULL, *dns_y_profile = NULL, *dns_ag_profile = NULL;
	char_t	*host;
	char_t	*reset		= websGetVar(wp, T("reset"), T("0"));

	int opmode	= nvram_get_int(RT2860_NVRAM, "OperationMode", -1);
	int dhcpEnabled	= nvram_get_int(RT2860_NVRAM, "dhcpEnabled", -1);

	lan_ip		= websGetVar(wp, T("lanIp"), T(""));
	lan_nm		= websGetVar(wp, T("lanNetmask"), T(""));
	start_ip	= websGetVar(wp, T("dhcpStart"), T(""));
	end_ip		= websGetVar(wp, T("dhcpEnd"), T(""));
	dgw		= websGetVar(wp, T("dhcpGateway"), T(""));
	host		= websGetVar(wp, T("hostname"), T("0"));
	dns_profile	= websGetVar(wp, T("wStaticDnsProfile"), T("manual"));
	dns_y_profile	= websGetVar(wp, T("wStaticDnsYandexProfile"), T("basic"));
	dns_ag_profile	= websGetVar(wp, T("wStaticDnsAdguardProfile"), T("default"));

	if (CHK_IF_DIGIT(reset, 1)) {
		nvram_fromdef(RT2860_NVRAM, 3, "HostName", "lan_ipaddr", "lan_netmask");
		if (opmode == 0) {
			nvram_fromdef(RT2860_NVRAM, 6, "wan_gateway", "wan_primary_dns", "wan_secondary_dns", "wan_static_dns_profile", "wan_static_dns_profile_yandex", "wan_static_dns_profile_adguard");
			ngx_nvram_set(wp, "wan_static_dns", "on");
		}
		if (dhcpEnabled == 1) {
			nvram_fromdef(RT2860_NVRAM, 4, "dhcpStart", "dhcpEnd", "dhcpMask", "dhcpGateway");
		}
	}
	else {
		nvram_init(RT2860_NVRAM);

		ngx_nvram_bufset(wp, "HostName", host);
		ngx_nvram_bufset(wp, "lan_ipaddr", lan_ip);
		ngx_nvram_bufset(wp, "lan_netmask", lan_nm);

		// configure gateway and dns (WAN) at bridge mode
		if (opmode == 0)
		{
			gw = websGetVar(wp, T("lanGateway"), T(""));
			pd = websGetVar(wp, T("lanPriDns"), T(""));
			sd = websGetVar(wp, T("lanSecDns"), T(""));
			ngx_nvram_bufset(wp, "wan_gateway", gw);
			ngx_nvram_bufset(wp, "wan_primary_dns", pd);
			ngx_nvram_bufset(wp, "wan_secondary_dns", sd);
			ngx_nvram_bufset(wp, "wan_static_dns_profile", dns_profile);
			ngx_nvram_bufset(wp, "wan_static_dns_profile_yandex", dns_y_profile);
			ngx_nvram_bufset(wp, "wan_static_dns_profile_adguard", dns_ag_profile);
			ngx_nvram_bufset(wp, "wan_static_dns", "on");
		}

		if (dhcpEnabled == 1) {
			ngx_nvram_bufset(wp, "dhcpStart", start_ip);
			ngx_nvram_bufset(wp, "dhcpEnd", end_ip);
			ngx_nvram_bufset(wp, "dhcpMask", lan_nm);
			ngx_nvram_bufset(wp, "dhcpGateway", dgw);
		}
		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}

        wp->on_response_ok = DO_RECONFIGURE_AND_SAMBA_RESTART;

//	websHeader(wp);
	websDone(wp, 200);
}

/* goform/setTvSipVLAN */
static void setTvSipVLAN(webs_t* wp, char_t *path, char_t *query)
{
	char* tv_stb_enable	= websGetVar(wp, T("tv_stbEnabled"),   T("0"));
	char* sip_stb_enable	= websGetVar(wp, T("sip_stbEnabled"),  T("0"));
	char* tv_port_mcast	= websGetVar(wp, T("tv_stbMcast"),     T("0"));
	char* sip_port_mcast	= websGetVar(wp, T("sip_stbMcast"),    T("0"));
	char* tv_portVLAN	= websGetVar(wp, T("tv_stbVLAN"),      T(""));
	char* sip_portVLAN	= websGetVar(wp, T("sip_stbVLAN"),     T(""));
	char* tv_portVLANprio	= websGetVar(wp, T("tv_stbVLANprio"),  T(""));
	char* sip_portVLANprio	= websGetVar(wp, T("sip_stbVLANprio"), T(""));
	char* reboot		= websGetVar(wp, T("reboot"),          T("0"));
	char* reset		= websGetVar(wp, T("reset"),           T("0"));

	if (CHK_IF_DIGIT(reset, 1)) {
		nvram_fromdef(RT2860_NVRAM, 8, "tv_port", "sip_port", "tv_port_mcast", "sip_port_mcast", "tv_portVLAN", "sip_portVLAN", "tv_portVLANPRIO", "sip_portVLANPRIO");
	}
	else {
		nvram_init(RT2860_NVRAM);
		ngx_nvram_bufset(wp, "tv_port",          tv_stb_enable);
		ngx_nvram_bufset(wp, "sip_port",         sip_stb_enable);
		ngx_nvram_bufset(wp, "tv_port_mcast",    tv_port_mcast);
		ngx_nvram_bufset(wp, "sip_port_mcast",   sip_port_mcast);
		ngx_nvram_bufset(wp, "tv_portVLAN",      tv_portVLAN);
		ngx_nvram_bufset(wp, "sip_portVLAN",     sip_portVLAN);
		ngx_nvram_bufset(wp, "tv_portVLANPRIO",  tv_portVLANprio);
		ngx_nvram_bufset(wp, "sip_portVLANPRIO", sip_portVLANprio);
		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}

	if (CHK_IF_DIGIT(reboot, 1)) {
                websHeader(wp);
		outputTimerForReload(wp, 60000);
                websFooter(wp);
		websDone(wp, 200);
                wp->on_response_ok = DO_REBOOT;
	} else {
//		websHeader(wp);
		websDone(wp, 200);
	}
}

/* goform/setWlanLanVLAN */
static void setWlanLanVLAN(webs_t* wp, char_t *path, char_t *query)
{
	char* wifi_lan		= websGetVar(wp, T("wifi_lan"), T("0"));
	char* wifi_lan_inic	= websGetVar(wp, T("wifi_lan_inic"), T("0"));
	char* wifi_wan		= websGetVar(wp, T("wifi_wan"), T("0"));
	char* wifi_wan_inic	= websGetVar(wp, T("wifi_wan_inic"), T("0"));
	char* vlan_lan		= websGetVar(wp, T("vlan_lan"), T("0"));
	char* vlan_lan_isolate	= websGetVar(wp, T("vlan_lan_isolate"), T("0"));
	char* reboot		= websGetVar(wp, T("reboot"), T(""));
	char* reset		= websGetVar(wp, T("reset"), T(""));

	if (CHK_IF_DIGIT(reset, 1)) {
		nvram_fromdef(RT2860_NVRAM, 6, "VlanWifiLan", "VlanWifiLanINIC", "VlanWifiWan", "VlanWifiWanINIC",
						"VlanLan", "VlanLanIsolate");
	}
	else {
		nvram_init(RT2860_NVRAM);
		ngx_nvram_bufset(wp, "VlanWifiLan",     wifi_lan);
		ngx_nvram_bufset(wp, "VlanWifiLanINIC", wifi_lan_inic);
		ngx_nvram_bufset(wp, "VlanWifiWan",     wifi_wan);
		ngx_nvram_bufset(wp, "VlanWifiWanINIC", wifi_wan_inic);
		ngx_nvram_bufset(wp, "VlanLan",         vlan_lan);
		ngx_nvram_bufset(wp, "VlanLanIsolate",  vlan_lan_isolate);
		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}

	if (CHK_IF_DIGIT(reboot, 1)) {
            websHeader(wp);
	    outputTimerForReload(wp, 60000);
            websFooter(wp);
            websDone(wp, 200);
            wp->on_response_ok = DO_REBOOT;
	} else {
//	    websHeader(wp);
	    websDone(wp, 200);
	}
}

/* goform/restoremac */
static void restoremac(webs_t* wp, char_t *path, char_t *query)
{
        websHeader(wp);
	outputTimerForReload(wp, 60000);
        websFooter(wp);
        websDone(wp, 200);
	doSystem("fs factory_mac");
        wp->on_response_ok = DO_REBOOT;
}

/* goform/setWan */
static void setWan(webs_t* wp, char_t *path, char_t *query)
{
	char_t *ctype, *req_ip, *dhcpVen;
	char_t *ip, *nm, *gw, *mac, *oldmac;
	char_t *eth, *user, *pass;
	char_t *nat_enable;
	char_t *vpn_srv, *vpn_mode;
	char_t *l2tp_srv, *l2tp_mode;
	char_t *reset = websGetVar(wp, T("reset"), T("0"));
	char_t *reboot = websGetVar(wp, T("reboot"), T("0"));

	char_t *wan_mtu;
	char_t *st_en, *pd, *sd, *st_pr, *st_pr_ya, *st_pr_ag;

	int opmode = nvram_get_int(RT2860_NVRAM, "OperationMode", -1);

	ip = nm = gw = eth = user = pass = mac = vpn_srv = vpn_mode = l2tp_srv = l2tp_mode = NULL;

	ctype = websGetVar(wp, T("connectionType"), T("0"));
	req_ip = websGetVar(wp, T("dhcpReqIP"), T(""));
	dhcpVen = websGetVar(wp, T("dhcpVendorClass"), T(""));

//	websHeader(wp);
        wp->on_response_ok = DO_RECONFIGURE;

	if (CHK_IF_DIGIT(reset, 1)) {
		nvram_fromdef(RT2860_NVRAM, 16, "wanConnectionMode", "wan_ipaddr", "wan_netmask", "wan_gateway", "dhcpRequestIP",
						"dhcpVendorClass", "natEnabled", "wan_static_dns", "wan_static_dns_profile", "wan_static_dns_profile_yandex",
						"wan_static_dns_profile_adguard", "wan_primary_dns", "wan_secondary_dns", "wan_mtu", "wan_manual_mtu",
						"CHECKMAC" );
		doSystem("fs factory_mac");
	}
	else {
		if (!strncmp(ctype, "STATIC", 7) || opmode == 0)
		{
			FILE *fd;

			//always treat bridge mode having static wan connection
			ip = websGetVar(wp, T("staticIp"), T(""));
			nm = websGetVar(wp, T("staticNetmask"), T("0"));
			gw = websGetVar(wp, T("staticGateway"), T(""));

			nvram_init(RT2860_NVRAM);
			ngx_nvram_bufset(wp, "wanConnectionMode", ctype);
			ngx_nvram_bufset(wp, "wan_ipaddr", ip);
			ngx_nvram_bufset(wp, "wan_netmask", nm);
			/*
			 * in Bridge Mode, lan and wan are bridged together and associated with
			 * the same ip address
			 */
			if (opmode == 0)
			{
				ngx_nvram_bufset(wp, "lan_ipaddr", ip);
				ngx_nvram_bufset(wp, "lan_netmask", nm);
			}
			ngx_nvram_bufset(wp, "wan_gateway", gw);

			nvram_commit(RT2860_NVRAM);
			nvram_close(RT2860_NVRAM);

			// Reset /etc/resolv.conf
			fd = fopen("/etc/resolv.conf", "w");
			if (fd != NULL)
				fclose(fd);
		}
		else if (strncmp(ctype, "DHCP", 5) == 0)
		{
			nvram_init(RT2860_NVRAM);
			ngx_nvram_bufset(wp, "wanConnectionMode", ctype);
			ngx_nvram_bufset(wp, "dhcpRequestIP", req_ip);
			ngx_nvram_bufset(wp, "dhcpVendorClass", dhcpVen);
			nvram_commit(RT2860_NVRAM);
			nvram_close(RT2860_NVRAM);
		}
		else if (strncmp(ctype, "ZERO", 5) == 0)
		{
			ngx_nvram_set(wp, "wanConnectionMode", ctype);
		}
		else
		{
			websDone(wp, 200);
			return;
		}

		// Primary/Seconfary DNS set
		st_en = websGetVar(wp, T("wStaticDnsEnable"), T("off"));
		st_pr = websGetVar(wp, T("wStaticDnsProfile"), T("manual"));
		st_pr_ya = websGetVar(wp, T("wStaticDnsYandexProfile"), T("basic"));
		st_pr_ag = websGetVar(wp, T("wStaticDnsAdguardProfile"), T("default"));
		pd = websGetVar(wp, T("staticPriDns"), T(""));
		sd = websGetVar(wp, T("staticSecDns"), T(""));


		nvram_init(RT2860_NVRAM);
		ngx_nvram_bufset(wp, "wan_static_dns", st_en);

		if (!strcmp(st_en, "on"))
		{
			ngx_nvram_bufset(wp, "wan_primary_dns", pd);
			ngx_nvram_bufset(wp, "wan_secondary_dns", sd);
			ngx_nvram_bufset(wp, "wan_static_dns_profile", st_pr);
			ngx_nvram_bufset(wp, "wan_static_dns_profile_yandex", st_pr_ya);
			ngx_nvram_bufset(wp, "wan_static_dns_profile_adguard", st_pr_ag);
		}

		// NAT
		if (opmode != 0)
		{
			nat_enable = websGetVar(wp, T("natEnabled"), T("off"));
			nat_enable = (strcmp(nat_enable, "on") == 0) ? "1" : "0";
			ngx_nvram_bufset(wp, "natEnabled", nat_enable);
		}

		// MTU for WAN
		wan_mtu = websGetVar(wp, T("wan_mtu"), T("0"));
		ngx_nvram_bufset(wp, "wan_manual_mtu", wan_mtu);

		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);

		// MAC
		mac = websGetVar(wp, T("wanMac"), T(""));
		oldmac = nvram_get(RT2860_NVRAM, "WAN_MAC_ADDR");
		if (strcmp(oldmac, mac) != 0) {
			nvram_init(RT2860_NVRAM);
			ngx_nvram_bufset(wp, "WAN_MAC_ADDR", mac);
			ngx_nvram_bufset(wp, "CHECKMAC", "NO");
			nvram_commit(RT2860_NVRAM);
			nvram_close(RT2860_NVRAM);
		}

		// Reboot
		if (CHK_IF_DIGIT(reboot, 1)) {
                        websHeader(wp);
			outputTimerForReload(wp, 60000);
                        websFooter(wp);
                        websDone(wp, 200);
                        wp->on_response_ok = DO_REBOOT;

			return;
		}
	}

	websDone(wp, 200);
}

#ifdef CONFIG_IPV6
parameter_fetch_t service_ipv6_flags[] = 
{
	{ ("dhcp6c_enable"), "IPv6Dhcpc", 2, ("off") },
	{ ("ipv6_allow_forward"), "IPv6AllowForward", 2, ("off") },
	{ ("ipv6_Ipv6InVPN"), "Ipv6InVPN", 2, ("off") },
	{ ("ipv6_ianaDisable"), "IPv6DisableIANA", 2, ("off") },
#ifdef CONFIG_USER_RADVD
	{ ("radvdEnbl"), "radvdEnabled", 0, ("0") },
#endif
#ifdef CONFIG_USER_DHCP6_SERVER
	{ ("dhcpv6Enbl"), "dhcpv6Enabled", 0, ("0") },
#endif
	{ NULL, NULL, 0, NULL } // Terminator
};

/* goform/setIPv6 */
static void setIPv6(webs_t* wp, char_t *path, char_t *query)
{
	char_t *opmode;
	char_t *ipaddr, *isp_prefix, *prefix_len, *wan_ipaddr, *wan_prefix_len, *srv_ipaddr, *srv_dns_primary, *srv_dns_secondary, *ipv6_manual_mtu;
	char_t *reset = websGetVar(wp, T("reset"), T("0"));

	ipaddr = prefix_len = wan_ipaddr = wan_prefix_len = srv_ipaddr = srv_dns_primary = srv_dns_secondary = NULL;

	if (CHK_IF_DIGIT(reset, 1)) {
		nvram_fromdef(RT2860_NVRAM, 16, "IPv6OpMode", "IPv6IPAddr", "IPv6PrefixLen", "IPv6WANIPAddr", "IPv6WANPrefixLen",
			"IPv6GWAddr", "IPv6SrvAddr", "IPv6DNSPrimary", "IPv6DNSSecondary", "IPv6Dhcpc",
			"IPv6AllowForward", "Ipv6InVPN", "IPv6DisableIANA", "IPv6ManualMTU", "radvdEnabled",
			"dhcpv6Enabled");
			int ipv6_mtu = nvram_get_int(RT2860_NVRAM, "IPv6ManualMTU", -1);
			if (ipv6_mtu > 0)
				ngx_nvram_set(wp, "IPv6ManualMTU", "0");
	}
	else {
    		opmode = websGetVar(wp, T("ipv6_opmode"), T("0"));

		nvram_init(RT2860_NVRAM);

		if (!strcmp(opmode, "1")) {
			ipaddr = websGetVar(wp, T("ipv6_lan_ipaddr"), T(""));
			prefix_len = websGetVar(wp, T("ipv6_lan_prefix_len"), T(""));
			wan_ipaddr = websGetVar(wp, T("ipv6_wan_ipaddr"), T(""));
			wan_prefix_len = websGetVar(wp, T("ipv6_wan_prefix_len"), T(""));
			srv_ipaddr = websGetVar(wp, T("ipv6_static_gw"), T(""));
			srv_dns_primary = websGetVar(wp, T("ipv6_static_dns_primary"), T(""));
			srv_dns_secondary = websGetVar(wp, T("ipv6_static_dns_secondary"), T(""));
			ipv6_manual_mtu = websGetVar(wp, T("ipv6_manual_mtu"), T("0"));

			ngx_nvram_bufset(wp, "IPv6IPAddr", ipaddr);
			ngx_nvram_bufset(wp, "IPv6PrefixLen", prefix_len);
			ngx_nvram_bufset(wp, "IPv6WANIPAddr", wan_ipaddr);
			ngx_nvram_bufset(wp, "IPv6WANPrefixLen", wan_prefix_len);
			ngx_nvram_bufset(wp, "IPv6GWAddr", srv_ipaddr);
			ngx_nvram_bufset(wp, "IPv6DNSPrimary", srv_dns_primary);
			ngx_nvram_bufset(wp, "IPv6DNSSecondary", srv_dns_secondary);
			ngx_nvram_bufset(wp, "IPv6ManualMTU", ipv6_manual_mtu);
#if defined (CONFIG_IPV6_SIT) || defined (CONFIG_IPV6_SIT_MODULE)
#if defined (CONFIG_IPV6_SIT_6RD)
		} else if (!strcmp(opmode, "2")) {
			ipaddr = websGetVar(wp, T("ipv6_6rd_prefix"), T(""));
			prefix_len = websGetVar(wp, T("ipv6_6rd_prefix_len"), T(""));
			srv_ipaddr = websGetVar(wp, T("ipv6_6rd_border_ipaddr"), T(""));
			ngx_nvram_bufset(wp, "IPv6IPAddr", ipaddr);
			ngx_nvram_bufset(wp, "IPv6PrefixLen", prefix_len);
			ngx_nvram_bufset(wp, "IPv6SrvAddr", srv_ipaddr);
#endif
		} else if (!strcmp(opmode, "3")) {
			ipaddr = websGetVar(wp, T("ipv6_6to4_prefix"), T(""));
			isp_prefix = websGetVar(wp, T("ipv6_6to4_isp_prefix"), T("off"));
			prefix_len = websGetVar(wp, T("ipv6_6to4_prefix_len"), T(""));
			srv_ipaddr = websGetVar(wp, T("IPv6SrvAddr"), T("192.88.99.1"));
			srv_dns_primary = websGetVar(wp, T("ipv6_6to4_dns_primary"), T(""));
			srv_dns_secondary = websGetVar(wp, T("ipv6_6to4_dns_secondary"), T(""));
			if (!strcmp(isp_prefix, "on")) {
			    ngx_nvram_bufset(wp, "IPv6IPAddr", ipaddr);
			    ngx_nvram_bufset(wp, "IPv6PrefixLen", prefix_len);
			    ngx_nvram_bufset(wp, "IPv6DNSPrimary", srv_dns_primary);
			    ngx_nvram_bufset(wp, "IPv6DNSSecondary", srv_dns_secondary);
			}
			ngx_nvram_bufset(wp, "IPv6ISPPrefix", isp_prefix);
			ngx_nvram_bufset(wp, "IPv6SrvAddr", srv_ipaddr);
#endif
		}
		ngx_nvram_bufset(wp, "IPv6OpMode", opmode);

		setupParameters(wp, service_ipv6_flags, 0);

		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}

        wp->on_response_ok = DO_RECONFIGURE;

//	websHeader(wp);
	websDone(wp, 200);
}

static int getIPv6IntAddr(webs_t *wp, char** params, int nparams) {
	char address[INET6_ADDRSTRLEN] = "";
	char mask[16] = "";

	if (getIPv6IntIPAddr(address, mask) != 0)
	{
		return outWrite(T(""));
	}
	else
	{
		return outWrite(T("%s/%s"), address, mask);
	}
}

static int getIPv6ExtAddr(webs_t *wp, char** params, int nparams) {

	char address[INET6_ADDRSTRLEN] = "";
	char mask[16] = "";

	if (getIPv6ExtIPAddr(address, mask) != 0)
	{
		return outWrite(T(""));
	}
	else
	{
		return outWrite(T("%s/%s"), address, mask);
	}
}
#endif

#if defined(CONFIG_USER_CHILLISPOT) || defined(CONFIG_USER_NODOGSPLASH)
#ifdef CONFIG_USER_CHILLISPOT
// ChilliSpot variables
parameter_fetch_t chilli_vars[] =
{
	{ ("chilliEnable"),		"chilli_enable",		0,       ("") },
	{ ("spotProfile"),		"chilli_profile",		0,       ("manual") },
	{ ("sPriDns"),			"chilli_dns1",			0,       ("") },
	{ ("sSecDns"),			"chilli_dns2",			0,       ("") },
	{ ("sDomain"),			"chilli_domain",		0,       ("") },
	{ ("sStartIP"),		"chilli_dhcpstart",			0,       ("") },
	{ ("sEndIP"),			"chilli_dhcpend",		0,       ("") },
	{ ("sLease"),			"chilli_lease",			0,       ("") },
	{ ("sRadServer1"),		"chilli_radiusserver1",		0,       ("") },
	{ ("sRadServer2"),		"chilli_radiusserver2",		0,       ("") },
	{ ("sRadSecret"),		"chilli_radiussecret",		0,       ("") },
	{ ("sNasId"),			"chilli_radiusnasid",		0,       ("") },
	{ ("sRadLocId"),		"chilli_radiuslocationid",	0,       ("") },
	{ ("sRadLocName"),		"chilli_radiuslocationname",	0,       ("") },
	{ ("sRadCoaPort"),		"chilli_coaport",		0,       ("") },
	{ ("sRadCoaNoIpCheck"),	"chilli_coanoipcheck",			0,       ("") },
	{ ("sUamServer"),		"chilli_uamserver",		0,       ("") },
	{ ("sUamHomepage"),		"chilli_uamhomepage",		0,       ("") },
	{ ("sUamSecret"),		"chilli_uamsecret",		0,       ("") },
	{ ("sUamAllowed"),		"chilli_uamallowed",		0,       ("") },
	{ ("sUamDomain"),		"chilli_uamdomain",		0,       ("") },
	{ ("sUamAnyDNS"),		"chilli_uamanydns",		0,       ("") },
	{ ("sMacAllowed"),		"chilli_macallowed",		0,       ("") },
	{ ("sC2Cisolate"),		"chilli_clisolate",		0,       ("") },
	{ ("sDropDNS"),		"chilli_dnsparanoia",			0,       ("") },
	{ ("sMaxClients"),		"chilli_maxclients",		0,       ("") },
	{ ("sHttpsRedirect"),		"chilli_https",			0,       ("") },
	{ NULL, 			NULL, 				0,       0     }  // Terminator
};
#endif

#ifdef CONFIG_USER_NODOGSPLASH
// NoDogSplash variables
parameter_fetch_t nodog_vars[] =
{
	{ ("nodogEnable"),			"nodogsplash_enable",			0,       ("") },
	{ ("GatewayIPRange"),			"nodog_GatewayIPRange",			0,       ("0.0.0.0/0") },
	{ ("RedirectURL"),			"nodog_RedirectURL",			0,       ("http://wi-cat.ru") },
	{ ("MaxClients"),			"nodog_MaxClients",			0,       ("32") },
	{ ("ClientIdleTimeout"),		"nodog_ClientIdleTimeout",		0,       ("10") },
	{ ("ClientForceTimeout"),		"nodog_ClientForceTimeout",		0,       ("360") },
	{ ("AuthenticateImmediately"),		"nodog_AuthenticateImmediately",	0,       ("no") },
	{ ("MACMechanism"),			"nodog_MACMechanism",			0,       ("block") },
	{ ("BlockedMACList"),			"nodog_BlockedMACList",			0,       ("") },
	{ ("AllowedMACList"),			"nodog_AllowedMACList",			0,       ("") },
	{ ("TrustedMACList"),			"nodog_TrustedMACList",			0,       ("") },
	{ ("PasswordAuthentication"),		"nodog_PasswordAuthentication",		0,       ("no") },
	{ ("Password"),				"nodog_Password",			0,       ("nodog") },
	{ ("UsernameAuthentication"),		"nodog_UsernameAuthentication",		0,       ("no") },
	{ ("Username"),				"nodog_Username",			0,       ("guest") },
	{ ("PasswordAttempts"),			"nodog_PasswordAttempts",		0,       ("5") },
	{ NULL,					NULL,					0,       0 } // Terminator
};
#endif

#ifdef CONFIG_USER_CHILLISPOT
/*
 * description: write hotspot network ip address
 */
static int getSpotIp(webs_t *wp, char** params, int nparams)
{
	char_t *nvdata = nvram_get(RT2860_NVRAM, "chilli_net");

	char* slashPos = strchr(nvdata, '/');
	if (slashPos == NULL) {
		return outWrite(T("172.16.0.0"));
	}
	slashPos[0] = '\0';
	return outWrite(T("%s"), nvdata);
}

/*
 * description: write hotspot network netmask
 */
static int getSpotNetmask(webs_t *wp, char** params, int nparams)
{
	char_t *nvdata = nvram_get(RT2860_NVRAM, "chilli_net");
	unsigned int imask;
	struct in_addr mask;

	char* slashPos = strchr(nvdata, '/');
	if (slashPos == NULL) {
		return outWrite(T("255.255.255.0"));
	}
	imask = atoi(slashPos+1);
	if (!imask)
		imask = 24;
	mask.s_addr = ntohl(0 - (1 << (32 - imask)));

	return outWrite(T("%s"), inet_ntoa(mask));
}
#endif

/* goform/setHotspot */
static void setHotspot(webs_t* wp, char_t *path, char_t *query)
{
	char_t *enabled = websGetVar(wp, T("spotEnable"), T("0"));
	char_t *reset   = websGetVar(wp, T("reset"), T("0"));

	if (CHK_IF_DIGIT(reset, 1)) {
		nvram_fromdef(RT2860_NVRAM, 42, "chilli_enable", "chilli_profile", "chilli_dns1",
			"chilli_dns2", "chilli_domain", "chilli_dhcpstart", "chilli_dhcpend",
			"chilli_lease", "chilli_radiusserver1", "chilli_radiusserver2",
			"chilli_radiussecret", "chilli_radiusnasid", "chilli_radiuslocationid",
			"chilli_radiuslocationname", "chilli_coaport", "chilli_coanoipcheck",
			"chilli_uamserver", "chilli_uamhomepage", "chilli_uamsecret", "chilli_uamallowed",
			"chilli_uamdomain", "chilli_uamanydns", "chilli_macallowed", "chilli_clisolate", "chilli_https",
			"chilli_dnsparanoia", "chilli_maxclients", "nodogsplash_enable", "nodog_GatewayIPRange",
			"nodog_RedirectURL", "nodog_MaxClients", "nodog_ClientIdleTimeout", "nodog_ClientForceTimeout",
			"nodog_AuthenticateImmediately", "nodog_MACMechanism", "nodog_BlockedMACList", "nodog_AllowedMACList",
			"nodog_TrustedMACList", "nodog_PasswordAuthentication", "nodog_Password",
			"nodog_UsernameAuthentication", "nodog_Username", "nodog_PasswordAttempts");
	}
	else {
#ifdef CONFIG_USER_CHILLISPOT
		char_t *ip = websGetVar(wp, T("sIp"), T("192.168.182.0"));
		char_t *amask = websGetVar(wp, T("sNetmask"), T("255.255.255.0"));
		struct in_addr iip;
		struct in_addr imask;

		iip.s_addr = inet_addr(ip);
		imask.s_addr = inet_addr(amask);
		int h_mask=ntohl(imask.s_addr);
		int i;
		for (i = 30; i > 0; i--) {
			if (h_mask >= 0 - (1 << (32 - i)))
				break;
		}
		if (!i) i = 24;
		iip.s_addr &= ntohl(0 - (1 << (32 - i)));

		char_t subnet[20];
		sprintf(subnet, "%s/%d", inet_ntoa(iip), i);
#endif
		nvram_init(RT2860_NVRAM);
#ifdef CONFIG_USER_CHILLISPOT
		if(CHK_IF_DIGIT(enabled, 1)) {
			setupParameters(wp, chilli_vars, 0);
			if (ngx_nvram_bufset(wp, "chilli_net", (void *)subnet)!=0) //!!!
				syslog(LOG_ERR, "set chilli_net nvram error, %s", __FUNCTION__);
			ngx_nvram_bufset(wp, "nodogsplash_enable", "0");
		}
		else
#endif
#ifdef CONFIG_USER_NODOGSPLASH
		if(CHK_IF_DIGIT(enabled, 2)) {
			setupParameters(wp, nodog_vars, 0);
			ngx_nvram_bufset(wp, "chilli_enable", "off");
		}
		else
#endif
		if(CHK_IF_DIGIT(enabled, 0)) {
#ifdef CONFIG_USER_CHILLISPOT
			ngx_nvram_bufset(wp, "chilli_enable", "off");
#endif
#ifdef CONFIG_USER_NODOGSPLASH
			ngx_nvram_bufset(wp, "nodogsplash_enable", "0");
#endif
		}

		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}
	doSystem("services_restart.sh all &");
	websDone(wp, 200);
}

static void resetHotspot(webs_t* wp, char_t *path, char_t *query)
{
	char_t *profile   = websGetVar(wp, T("profile"), T("0"));

	nvram_fromdef(RT2860_NVRAM, 42, "chilli_enable", "chilli_profile", "chilli_dns1",
		"chilli_dns2", "chilli_domain", "chilli_dhcpstart", "chilli_dhcpend",
		"chilli_lease", "chilli_radiusserver1", "chilli_radiusserver2",
		"chilli_radiussecret", "chilli_radiusnasid", "chilli_radiuslocationid",
		"chilli_radiuslocationname", "chilli_coaport", "chilli_coanoipcheck",
		"chilli_uamserver", "chilli_uamhomepage", "chilli_uamsecret", "chilli_uamallowed",
		"chilli_uamdomain", "chilli_uamanydns", "chilli_macallowed", "chilli_clisolate", "chilli_https",
		"chilli_dnsparanoia", "chilli_maxclients", "nodogsplash_enable", "nodog_GatewayIPRange",
		"nodog_RedirectURL", "nodog_MaxClients", "nodog_ClientIdleTimeout", "nodog_ClientForceTimeout",
		"nodog_AuthenticateImmediately", "nodog_MACMechanism", "nodog_BlockedMACList", "nodog_AllowedMACList",
		"nodog_TrustedMACList", "nodog_PasswordAuthentication", "nodog_Password",
		"nodog_UsernameAuthentication", "nodog_Username", "nodog_PasswordAttempts");

	nvram_init(RT2860_NVRAM);
	ngx_nvram_bufset(wp, "nodogsplash_enable", "0");
	ngx_nvram_bufset(wp, "chilli_enable", "on");
	ngx_nvram_bufset(wp, "chilli_profile", profile);
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

//	websHeader(wp);
	websDone(wp, 200);
}
#endif // HOTSPOT

static int getChilliBuilt(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_USER_CHILLISPOT
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getNoDogBuilt(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_USER_NODOGSPLASH
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

parameter_fetch_t service_eoip_flags[] =
{
	{ ("eoip_local"),		"eoip_local",			0, ("")     },
	{ ("eoip_remote"),		"eoip_remote",			0, ("")     },
	{ ("eoip_brifs"),		"eoip_brifs",			0, ("")     },
	{ ("eoip_tid"),			"eoip_tid",			0, ("0")    },
	{ NULL,				NULL,				0, NULL      } // Terminator
};

static void eoipConfig(webs_t* wp, char_t *path, char_t *query)
{
	char user_var[16];
	char pass_var[16];
	int i, count;
	char_t *reset = websGetVar(wp, T("reset"), T("0"));

	if (CHK_IF_DIGIT(reset, 1)) {
		nvram_fromdef(RT2860_NVRAM, 5, "eoip_enabled", "eoip_local", "eoip_remote", "eoip_brifs", "eoip_tid");
	}
	else {
		char_t *eoip_enabled = websGetVar(wp, T("eoip_enabled"), T("0"));

		if (eoip_enabled == NULL)
			eoip_enabled = "0";

		nvram_init(RT2860_NVRAM);

		if (CHK_IF_DIGIT(eoip_enabled, 1)) {
			setupParameters(wp, service_eoip_flags, 0);
			ngx_nvram_bufset(wp, "eoip_enabled", "1");
		}
		else
			ngx_nvram_bufset(wp, "eoip_enabled", "0");

		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}
}


parameter_fetch_t service_l2tp_eth_flags[] =
{
	{ ("l2tp_eth_local"),		"l2tp_eth_local",			0, ("")     },
	{ ("l2tp_eth_remote"),		"l2tp_eth_remote",			0, ("")     },
	{ ("l2tp_eth_brifs"),		"l2tp_eth_brifs",			0, ("")     },
	{ ("l2tp_eth_tid"),		"l2tp_eth_tid",			0, ("0")    },
	{ ("l2tp_eth_ptid"),		"l2tp_eth_ptid",		0, ("0")    },
	{ ("l2tp_eth_sid"),		"l2tp_eth_sid",			0, ("0")    },
	{ ("l2tp_eth_psid"),		"l2tp_eth_psid",		0, ("0")    },
	{ ("l2tp_eth_sport"),		"l2tp_eth_sport",		0, ("0")    },
	{ ("l2tp_eth_dport"),		"l2tp_eth_dport",		0, ("0")    },
	{ NULL,				NULL,				0, NULL      } // Terminator
};

static void l2tpv3Config(webs_t* wp, char_t *path, char_t *query)
{
	char user_var[16];
	char pass_var[16];
	int i, count;
	char_t *reset = websGetVar(wp, T("reset"), T("0"));

	if (CHK_IF_DIGIT(reset, 1)) {
		nvram_fromdef(RT2860_NVRAM, 10,  "l2tp_eth_enabled", "l2tp_eth_local", "l2tp_eth_remote", "l2tp_eth_brifs", "l2tp_eth_tid",
						"l2tp_eth_ptid", "l2tp_eth_sid", "l2tp_eth_psid", "l2tp_eth_sport", "l2tp_eth_dport");
	}
	else {
		char_t *l2tp_eth_enabled = websGetVar(wp, T("l2tp_eth_enabled"), T("0"));

		if (l2tp_eth_enabled == NULL)
			l2tp_eth_enabled = "0";

		nvram_init(RT2860_NVRAM);

		if (CHK_IF_DIGIT(l2tp_eth_enabled, 1)) {
			setupParameters(wp, service_l2tp_eth_flags, 0);
			ngx_nvram_bufset(wp, "l2tp_eth_enabled", "1");
		}
		else
			ngx_nvram_bufset(wp, "l2tp_eth_enabled", "0");

		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}

}

static void l2tunnelsConfig(webs_t* wp, char_t *path, char_t *query)
{
	eoipConfig(wp, path, query);
	l2tpv3Config(wp, path, query);
	doSystem("service ethtun restart");
	websDone(wp, 200);
}

static int getEOIPBuilt(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_USER_EOIP
	outWrite(T("1"));
#else
	outWrite(T("0"));
#endif
	return 0;
}

int asp_mod_internet_init()
{

	aspDefineFunc(("getDns"), getDns, EVERYONE);
	aspDefineFunc(("getIgmpProxyBuilt"), getIgmpProxyBuilt, EVERYONE);
	aspDefineFunc(("getVPNBuilt"), getVPNBuilt, EVERYONE);
	aspDefineFunc(("getLanIp"), getLanIp, EVERYONE);
	aspDefineFunc(("getLanMac"), getLanMac, EVERYONE);
	aspDefineFunc(("getLanNetmask"), getLanNetmask, EVERYONE);
	aspDefineFunc(("getDnsmasqBuilt"), getDnsmasqBuilt, EVERYONE);
	aspDefineFunc(("getGWBuilt"), getGWBuilt, EVERYONE);
	aspDefineFunc(("getCdpBuilt"), getCdpBuilt, EVERYONE);
	aspDefineFunc(("getLltdBuilt"), getLltdBuilt, EVERYONE);
	aspDefineFunc(("getLldpdBuilt"), getLldpdBuilt, EVERYONE);
	aspDefineFunc(("getUpnpBuilt"), getUpnpBuilt, EVERYONE);
	aspDefineFunc(("getXupnpdBuilt"), getXupnpdBuilt, EVERYONE);
	aspDefineFunc(("getIntIp"), getIntIp, EVERYONE);
	aspDefineFunc(("getWanIp"), getWanIp, EVERYONE);
	aspDefineFunc(("getWanMac"), getWanMac, EVERYONE);
	aspDefineFunc(("getWanNetmask"), getWanNetmask, EVERYONE);
	aspDefineFunc(("getWanGateway"), getWanGateway, EVERYONE);
	aspDefineFunc(("getRoutingTable"), getRoutingTable, EVERYONE);
	aspDefineFunc(("getWDSBuilt"), getWDSBuilt, EVERYONE);
	aspDefineFunc(("getMBSSIDBuilt"), getMBSSIDBuilt, EVERYONE);
	aspDefineFunc(("getUSBBuilt"), getUSBBuilt, EVERYONE);
	aspDefineFunc(("getStorageBuilt"), getStorageBuilt, EVERYONE);
	aspDefineFunc(("getFtpBuilt"), getFtpBuilt, EVERYONE);
	aspDefineFunc(("getSmbBuilt"), getSmbBuilt, EVERYONE);
	aspDefineFunc(("getRadiusBuilt"), getRadiusBuilt, EVERYONE);
	aspDefineFunc(("getCwmpBuilt"), getCwmpBuilt, EVERYONE);
	aspDefineFunc(("getPrinterSrvBuilt"), getPrinterSrvBuilt, EVERYONE);
	aspDefineFunc(("getUSBModemBuilt"), getUSBModemBuilt, EVERYONE);
	websFormDefine(T("setLan"), setLan, ADMIN);
	websFormDefine(T("setWan"), setWan, ADMIN);
	websFormDefine(T("setTvSipVLAN"), setTvSipVLAN, ADMIN);
	websFormDefine(T("setWlanLanVLAN"), setWlanLanVLAN, ADMIN);
	aspDefineFunc(("getRadvdBuilt"), getRadvdBuilt, EVERYONE);
	aspDefineFunc(("getDhcpv6Built"), getDhcpv6Built, EVERYONE);
	aspDefineFunc(("getIPv6Built"), getIPv6Built, EVERYONE);
	aspDefineFunc(("getIPv66rdBuilt"), getIPv66rdBuilt, EVERYONE);
	aspDefineFunc(("getIP6to4Built"), getIP6to4Built, EVERYONE);
#ifdef CONFIG_IPV6
	websFormDefine(T("setIPv6"), setIPv6, ADMIN);
	aspDefineFunc(("getIPv6IntAddr"), getIPv6IntAddr, EVERYONE);
	aspDefineFunc(("getIPv6ExtAddr"), getIPv6ExtAddr, EVERYONE);
#endif
	websFormDefine(T("getMyMAC"), getMyMAC, EVERYONE);
	websFormDefine(T("editRouting"), editRouting, ADMIN);
	aspDefineFunc(("getTransmissionBuilt"), getTransmissionBuilt, EVERYONE);
	aspDefineFunc(("getDynamicRoutingBuilt"), getDynamicRoutingBuilt, EVERYONE);
	aspDefineFunc(("getSWQoSBuilt"), getSWQoSBuilt, EVERYONE);
	aspDefineFunc(("getDATEBuilt"), getDATEBuilt, EVERYONE);
	aspDefineFunc(("getDDNSBuilt"), getDDNSBuilt, EVERYONE);
	aspDefineFunc(("getSpotBuilt"), getSpotBuilt, EVERYONE);

	aspDefineFunc(("getLANAUTHBuilt"), getLANAUTHBuilt, EVERYONE);

	aspDefineFunc(("getSysLogBuilt"), getSysLogBuilt, EVERYONE);

	aspDefineFunc(("vpnShowVPNStatus"), vpnShowVPNStatus, EVERYONE);
	websFormDefine(T("formVPNSetup"), formVPNSetup, ADMIN);

	websFormDefine(T("restoremac"), restoremac, ADMIN);

#ifdef CONFIG_USER_CHILLISPOT
	aspDefineFunc(("getSpotIp"), getSpotIp, EVERYONE);
	aspDefineFunc(("getSpotNetmask"), getSpotNetmask, EVERYONE);
#endif
#if defined(CONFIG_USER_CHILLISPOT) || defined(CONFIG_USER_NODOGSPLASH)
	websFormDefine(T("setHotspot"), setHotspot, ADMIN);
	websFormDefine(T("resetHotspot"), resetHotspot, ADMIN);
#endif
	aspDefineFunc(("getChilliBuilt"), getChilliBuilt, EVERYONE);
	aspDefineFunc(("getNoDogBuilt"), getNoDogBuilt, EVERYONE);

	aspDefineFunc(("getEOIPBuilt"), getEOIPBuilt, EVERYONE);
	websFormDefine(T("l2tunnelsConfig"), l2tunnelsConfig, ADMIN);

    return 0;
}