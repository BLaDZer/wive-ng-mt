/* vi: set sw=4 ts=4 sts=4 fdm=marker: */
/*
 *	internet.c -- Network Settings
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: internet.c,v 1.167.2.8 2009-04-22 01:31:35 chhung Exp $
 */

#include	"helpers.h"
#include	"internet.h"
#include 	"firewall.h"
#include	"management.h"
#include	"station.h"


parameter_fetch_t vpn_args[] =
{
	{ T("vpn_server"),             "vpnServer",            0,       T("") },
	{ T("vpn_range"),              "vpnRange",             0,       T("") },
	{ T("vpn_user"),               "vpnUser",              0,       T("") },
	{ T("vpn_pass"),               "vpnPassword",          0,       T("") },
	{ T("vpn_mtu"),                "vpnMTU",               0,       T("") },
	{ T("vpn_type"),               "vpnType",              0,       T("") },
	{ T("vpn_mppe"),               "vpnMPPE",              1,       T("") },
	{ T("vpn_dgw"),                "vpnDGW",               0,       T("") },
	{ T("vpn_peerdns"),            "vpnPeerDNS",           1,       T("") },
	{ T("vpn_debug"),              "vpnDebug",             1,       T("") },
	{ T("vpn_nat"),                "vpnNAT",               1,       T("") },
	{ T("vpn_lcp"),                "vpnEnableLCP",         1,       T("") },
	{ T("vpn_auth_type"),          "vpnAuthProtocol",      0,       T("") },
	{ T("vpn_pppoe_iface"),        "vpnInterface",         0,       T("") },
	{ T("vpn_pppoe_service"),      "vpnService",           0,       T("") },
	{ T("vpn_pure_pppoe"),         "vpnPurePPPOE",         2,       T("") },
	{ T("vpn_lcp_errors"),         "vpnLCPFailure",        0,       T("") },
	{ T("vpn_lcp_interval"),       "vpnLCPInterval",       0,       T("") },
	{ T("vpn_test_reachable"),     "vpnTestReachable",     2,       T("") },
	{ NULL,                        NULL,                   0,       NULL  }  // Terminator
};

parameter_fetch_t lanauth_args[] =
{
	{ T("vpn_type"),               "vpnType",              0,       T("") },
	{ T("vpn_pass"),               "vpnPassword",          0,       T("") },
	{ T("lanauth_access"),         "LANAUTH_LVL",          0,       T("") },

	{ NULL, NULL, 0, NULL } // Terminator
};


/*
 * Show PPTP VPN status
 */
static int vpnShowVPNStatus(int eid, webs_t wp, int argc, char_t **argv)
{
        const char *status_str = getVPNStatusStr();
        websWrite(wp, T("%s"), status_str);

        return 0;
}

static void formVPNSetup(webs_t wp, char_t *path, char_t *query)
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
		nvram_bufset(RT2860_NVRAM, "vpnEnabled", vpn_enabled);

		if ((strncmp(vpn_enabled, "on", 3)) || !CHK_IF_DIGIT(vpn_type, 0))
			nvram_bufset(RT2860_NVRAM, "vpnPurePPPOE", "0");

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

	//kill helpers firt sigterm second sigkill
	doSystem("killall -q W60vpnhelper");
	doSystem("killall -q vpnhelper");
	doSystem("killall -q -SIGKILL W60vpnhelper");
	doSystem("killall -q -SIGKILL vpnhelper");
	doSystem("service vpnhelper restart > /dev/console 2>&1");

	websHeader(wp);
	websDone(wp, 200);
}

/*
 * arguments: type - 1 = write Primary DNS
 *                   2 = write Secondary DNS
 * description: write DNS ip address accordingly
 */
static int getDns(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp;
	char buf[80] = {0}, ns_str[11], dns[16] = {0};
	int type, idx = 0, req = 0;

	if (ejArgs(argc, argv, T("%d"), &type) == 1) {
		if (1 == type)
			req = 1;
		else if (2 == type)
			req = 2;
		else
			return websWrite(wp, T(""));
	}

	fp = fopen("/etc/resolv.conf", "r");
	if (NULL == fp)
		return websWrite(wp, T(""));
	while ((fgets(buf, sizeof(buf), fp)) != NULL)
	{
		if (sscanf(buf, "%s %s", ns_str, dns) != 2)
			continue;
		if (strcasecmp(ns_str, "nameserver") != 0)
			continue;

		idx++;
		if (idx == req) {
			websWrite(wp, T("%s"), dns);
			break;
		}
	}
	fclose(fp);
	return 0;
}

static int getIgmpProxyBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_IGMP_PROXY
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getVPNBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(CONFIG_NF_CONNTRACK_PPTP) || defined(CONFIG_NF_CONNTRACK_PPTP_MODULE) || \
    defined(CONFIG_IP_NF_PPTP) || defined(CONFIG_IP_NF_PPTP_MODULE)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getWDSBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(CONFIG_RT2860V2_AP_WDS) || defined(CONFIG_MT7610_AP_WDS) || defined(CONFIG_MT76X2_AP_WDS)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getMBSSIDBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(CONFIG_RT2860V2_AP_MBSS) || defined(CONFIG_MT7610_AP_MBSS) || defined(CONFIG_MT76X2_AP_MBSS)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getUSBBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USB
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getIPv6Built(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_IPV6
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getIPv66rdBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_IPV6_SIT_6RD
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getIP6to4Built(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (CONFIG_IPV6_SIT) ||  defined (CONFIG_IPV6_SIT_MODULE)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getStorageBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_STORAGE
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getFtpBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_FTPD
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getSmbBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(CONFIG_USER_SAMBA)
       return websWrite(wp, T("1"));
#else
       return websWrite(wp, T("0"));
#endif
}

static int getPrinterSrvBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_P910ND
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getUSBModemBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USB_MODESWITCH
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getTransmissionBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_TRANSMISSION
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getDynamicRoutingBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_ZEBRA
    return websWrite(wp, T("1"));
#else
    return websWrite(wp, T("0"));
#endif
}

static int getSWQoSBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_RALINKAPP_SWQOS
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getDATEBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_DATE
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getDDNSBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_INADYN
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getLANAUTHBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_KABINET
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getSysLogBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_SYSLOGD
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

/*
 * description: write LAN ip address accordingly
 */
static int getLanIp(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_addr[16];

	if (getIfIp(getLanIfName(), if_addr) == -1) {
		//websError(wp, 500, T("getLanIp: calling getIfIp error\n"));
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_addr);
}

/*
 * description: write LAN MAC address accordingly
 */
static int getLanMac(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_mac[18];

	if (getIfMac(getLanIfName(), if_mac, ':') == -1) {
		//websError(wp, 500, T("getLanIp: calling getIfMac error\n"));
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_mac);
}

/*
 * description: write LAN subnet mask accordingly
 */
static int getLanNetmask(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_net[16];

	if (getIfNetmask(getLanIfName(), if_net) == -1) {
		//websError(wp, 500, T("getLanNetmask: calling getIfNetmask error\n"));
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_net);
}

static int getGWBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_RTESW_SWITCH_ONEPORT
	return websWrite(wp, T("0"));
#else
	return websWrite(wp, T("1"));
#endif
}

static int getDnsmasqBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_DNSMASQ
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getCdpBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_CDP
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}


static int getLltdBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_LLTD
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getLldpdBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_LLDPD
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getUpnpBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_MINIUPNPD
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getXupnpdBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_XUPNPD
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getSpotBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(CONFIG_USER_CHILLISPOT) || defined(CONFIG_USER_NODOGSPLASH)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getRadvdBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_RADVD
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getDhcpv6Built(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_DHCP6_SERVER
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

/*
 * description: write real_wan ip address accordingly
 */
static int getIntIp(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_addr[16];

	if (vpn_mode_enabled() == 1) {
	    /* if vpn enabled get ip from vpnif else from wan if */
	    if (getIfIp(getPPPIfName(), if_addr) == -1) {
		if (getIfIp(getWanIfName(), if_addr) == -1)
		    return websWrite(wp, T(""));
	    }
	} else {
	    /* if vpn disabled always get ip from wanif */
	    if (getIfIp(getWanIfName(), if_addr) == -1) {
		return websWrite(wp, T(""));
	    }
	}
	return websWrite(wp, T("%s"), if_addr);
}

/*
 * description: write WAN ip address accordingly
 */
static int getWanIp(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_addr[16];

	if (getIfIp(getWanIfName(), if_addr) == -1) {
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_addr);
}

/*
 * description: write WAN MAC address accordingly
 */
static int getWanMac(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_mac[18];

	if (getIfMac(getWanIfName(), if_mac, ':') == -1) {
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_mac);
}

/*
 * description: write WAN subnet mask accordingly
 */
static int getWanNetmask(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_net[16];

	if (getIfNetmask(getWanIfName(), if_net) == -1) {
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_net);
}

/*
 * description: write WAN default gateway accordingly
 */
static int getWanGateway(int eid, webs_t wp, int argc, char_t **argv)
{
	char sgw[16];
	switch (getWANGateway(sgw))
	{
		case 0: return websWrite(wp, T("%s"), sgw);
		case 1: return websWrite(wp, T("format error"));
	}

	return websWrite(wp, T(""));
}


/*
 * description: get routing table
 */
static int getRoutingTable(int eid, webs_t wp, int argc, char_t **argv)
{
	int    i = 0;
	char   interface[128];
	char   dest_str[32], gw_str[32], netmask_str[32];
	int   *running_rules = NULL;
	char *rrs;
	int  rule_count;

	// Determine work mode
	int op_mode = nvram_get_int(RT2860_NVRAM, "OperationMode", -1);
	int isBridgeMode = (op_mode == 0) ? 1 : 0;

	rrs = nvram_get(RT2860_NVRAM, "RoutingRules");
	if (rrs == NULL)
		rrs = "";
	rule_count = getNums(rrs, ';');

	// Get routing table
	int parsed_rule_count = 0;
	struct RoutingRule* table = parseRoutingTable(rrs, &parsed_rule_count);

	if (table == NULL) 
	{
	    syslog(LOG_ERR, "Routing table parse error in function %s", __FUNCTION__);
	    return -1;
	}

	if (rule_count)
	{
		running_rules = calloc(1, sizeof(int) * rule_count);
		if (!running_rules)
		{
			return -1;
		}
	}

	// true_interface[0], destination[1], gateway[2], netmask[3], flags[4], ref[5], use[6],
	// metric[7], category[8], interface[9], idle[10], comment[11], new[12]
	for (i = 0 ; i < parsed_rule_count; i++) 
	{
	    struct RoutingRule rule = table[i];

	    strncpy(dest_str, inet_ntoa(rule.dest), sizeof(dest_str));
	    strncpy(gw_str, inet_ntoa(rule.gw), sizeof(gw_str));
	    strncpy(netmask_str, inet_ntoa(rule.netmask), sizeof(netmask_str));

	    if (i > 0) websWrite(wp, T(",\n"));

	    websWrite(wp, T("[ '%s', '%s', '%s', '%s', %d, %d, %d, %d"), rule.interface, dest_str, gw_str, netmask_str, rule.flgs, rule.ref, rule.use, rule.metric); // 0-7

	    switch (rule.category)
	    {
		case 1:
			websWrite(wp, T(", %d, "), rule.internal_id); // 8

			if (rule.internal_id < rule_count)
		    	    running_rules[rule.internal_id] = 1;
			else
			    syslog(LOG_ERR, "fatal error in %s", __FUNCTION__);

			websWrite(wp, T("'%s'"), rule.iftype); // 9
			websWrite(wp, T(", 0, '%s', 0 ]"), rule.comment); // 10-12
		    break;
		case 2:
			websWrite(wp, T(", -1, ")); // 8
			websWrite(wp, T("'%s'"), getLanWanNamebyIf(rule.iftype)); // 9
    			websWrite(wp, T(", 0, '%s', 0 ]"), rule.comment); // 10-12
		    break;

		case 3:
			websWrite(wp, T(", %d, "), rule.internal_id); // 8
			websWrite(wp, T("'%s'"), rule.iftype); // 9
    			websWrite(wp, T(", %d, "), ((strcmp(interface, "VPN")==0) ? isBridgeMode : 1)); // 10
    			websWrite(wp, T("'%s', 0 ]"), rule.comment); // 11-12
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

static void editRouting(webs_t wp, char_t *path, char_t *query)
{
	char_t *trans = websGetVar(wp, T("routingTableDiff"), T(""));
	char_t *reset = websGetVar(wp, T("reset"), T("0"));

	char rec[256];
	char true_iface[32], destination[32], gateway[32], netmask[32], iface[32], c_iface[32], comment[64], action[4];
	int i = 0, iaction;

	if (CHK_IF_DIGIT(reset, 1)) {
		nvram_set(RT2860_NVRAM, "RoutingRules", "");
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
	}

	/* reconfigure system */
	doSystem("internet.sh");

	websHeader(wp);
	websDone(wp, 200);
}

#ifdef CONFIG_USER_ZEBRA
static void dynamicRouting(webs_t wp, char_t *path, char_t *query)
{
	char_t *rip   = websGetVar(wp, T("RIPSelect"), T(""));
	char_t *reset = websGetVar(wp, T("reset"), T("0"));

	if (CHK_IF_DIGIT(reset, 1)) {
		nvram_set(RT2860_NVRAM, "RIPEnable", "0");
	}
	else {
		nvram_set(RT2860_NVRAM, "RIPEnable", rip);
		doSystem("service dynroute restart");
	}

	websHeader(wp);
	websDone(wp, 200);
}
#endif

static void getMyMAC(webs_t wp, char_t *path, char_t *query)
{
	char myMAC[32];

	arplookup(wp->ipaddr, myMAC);
	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\n"));
	websWrite(wp, WEBS_CACHE_CONTROL_STRING);
	websWrite(wp, T("\n"));
	websWrite(wp, T("%s"), myMAC);
	websDone(wp, 200);
}

/* goform/setLan */
static void setLan(webs_t wp, char_t *path, char_t *query)
{
	char_t	*lan_ip, *lan_nm;
	char_t	*start_ip, *end_ip, *dgw;
	char_t	*gw = NULL, *pd = NULL, *sd = NULL;
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

	if (CHK_IF_DIGIT(reset, 1)) {
		nvram_fromdef(RT2860_NVRAM, 6, "HostName", "lan_ipaddr", "lan_netmask");
		if (opmode != 0)
		{
			nvram_fromdef(RT2860_NVRAM, 3, "wan_gateway", "wan_primary_dns", "wan_secondary_dns");
			nvram_set(RT2860_NVRAM, "wan_static_dns", "on");
		}
		if (dhcpEnabled == 1) {
			nvram_fromdef(RT2860_NVRAM, 4, "dhcpStart", "dhcpEnd", "dhcpMask", "dhcpGateway");
		}
	}
	else {
		nvram_init(RT2860_NVRAM);

		nvram_bufset(RT2860_NVRAM, "HostName", host);
		nvram_bufset(RT2860_NVRAM, "lan_ipaddr", lan_ip);
		nvram_bufset(RT2860_NVRAM, "lan_netmask", lan_nm);

		// configure gateway and dns (WAN) at bridge mode
		if (opmode == 0)
		{
			gw = websGetVar(wp, T("lanGateway"), T(""));
			pd = websGetVar(wp, T("lanPriDns"), T(""));
			sd = websGetVar(wp, T("lanSecDns"), T(""));
			nvram_bufset(RT2860_NVRAM, "wan_gateway", gw);
			nvram_bufset(RT2860_NVRAM, "wan_primary_dns", pd);
			nvram_bufset(RT2860_NVRAM, "wan_secondary_dns", sd);
			nvram_bufset(RT2860_NVRAM, "wan_static_dns", "on");
		}

		if (dhcpEnabled == 1) {
			nvram_bufset(RT2860_NVRAM, "dhcpStart", start_ip);
			nvram_bufset(RT2860_NVRAM, "dhcpEnd", end_ip);
			nvram_bufset(RT2860_NVRAM, "dhcpMask", lan_nm);
			nvram_bufset(RT2860_NVRAM, "dhcpGateway", dgw);
		}
		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}

	doSystem("internet.sh");
#if defined(CONFIG_USER_SAMBA)
	doSystem("service samba restart");
#endif

	websHeader(wp);
	websDone(wp, 200);
}

/* goform/restoremac */
static void restoremac(webs_t wp, char_t *path, char_t *query)
{
	outputTimerForReload(wp, "", 80000);
	doSystem("fs factory_mac > /dev/console 2>&1");
	reboot_now();
}

/* goform/setWan */
static void setWan(webs_t wp, char_t *path, char_t *query)
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
	char_t *st_en, *pd, *sd;

	int opmode = nvram_get_int(RT2860_NVRAM, "OperationMode", -1);

	ctype = ip = nm = gw = eth = user = pass = mac = vpn_srv = vpn_mode = l2tp_srv = l2tp_mode = NULL;

	ctype = websGetVar(wp, T("connectionType"), T("0"));
	req_ip = websGetVar(wp, T("dhcpReqIP"), T(""));
	dhcpVen = websGetVar(wp, T("dhcpVendorClass"), T(""));

	websHeader(wp);

	if (CHK_IF_DIGIT(reset, 1)) {
		nvram_fromdef(RT2860_NVRAM, 13, "wanConnectionMode", "wan_ipaddr", "wan_netmask", "wan_gateway", 
						"dhcpRequestIP", "dhcpVendorClass", "natEnabled",
						"wan_static_dns", "wan_primary_dns", "wan_secondary_dns"
						"wan_mtu", "wan_manual_mtu", "CHECKMAC" );
		doSystem("fs factory_mac > /dev/console 2>&1");
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
			nvram_bufset(RT2860_NVRAM, "wanConnectionMode", ctype);
			nvram_bufset(RT2860_NVRAM, "wan_ipaddr", ip);
			nvram_bufset(RT2860_NVRAM, "wan_netmask", nm);
			/*
			 * in Bridge Mode, lan and wan are bridged together and associated with
			 * the same ip address
			 */
			if (opmode == 0)
			{
				nvram_bufset(RT2860_NVRAM, "lan_ipaddr", ip);
				nvram_bufset(RT2860_NVRAM, "lan_netmask", nm);
			}
			nvram_bufset(RT2860_NVRAM, "wan_gateway", gw);

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
			nvram_bufset(RT2860_NVRAM, "wanConnectionMode", ctype);
			nvram_bufset(RT2860_NVRAM, "dhcpRequestIP", req_ip);
			nvram_bufset(RT2860_NVRAM, "dhcpVendorClass", dhcpVen);
			nvram_commit(RT2860_NVRAM);
			nvram_close(RT2860_NVRAM);
		}
		else if (strncmp(ctype, "ZERO", 5) == 0)
		{
			nvram_set(RT2860_NVRAM, "wanConnectionMode", ctype);
		}
		else
		{
			websDone(wp, 200);
			return;
		}

		// Primary/Seconfary DNS set
		st_en = websGetVar(wp, T("wStaticDnsEnable"), T("off"));
		pd = websGetVar(wp, T("staticPriDns"), T(""));
		sd = websGetVar(wp, T("staticSecDns"), T(""));

		nvram_init(RT2860_NVRAM);
		nvram_bufset(RT2860_NVRAM, "wan_static_dns", st_en);

		if (!strcmp(st_en, "on"))
		{
			nvram_bufset(RT2860_NVRAM, "wan_primary_dns", pd);
			nvram_bufset(RT2860_NVRAM, "wan_secondary_dns", sd);
		}

		// NAT
		if (opmode == 0)
		{
			nat_enable = websGetVar(wp, T("natEnabled"), T("off"));
			nat_enable = (strcmp(nat_enable, "on") == 0) ? "1" : "0";
			nvram_bufset(RT2860_NVRAM, "natEnabled", nat_enable);
		}

		// MTU for WAN
		wan_mtu = websGetVar(wp, T("wan_mtu"), T("0"));
		nvram_bufset(RT2860_NVRAM, "wan_manual_mtu", wan_mtu);

		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);

		// MAC
		mac = websGetVar(wp, T("wanMac"), T(""));
		oldmac = nvram_get(RT2860_NVRAM, "WAN_MAC_ADDR");
		if (strcmp(oldmac, mac) != 0) {
			nvram_init(RT2860_NVRAM);
			nvram_bufset(RT2860_NVRAM, "WAN_MAC_ADDR", mac);
			nvram_bufset(RT2860_NVRAM, "CHECKMAC", "NO");
			nvram_commit(RT2860_NVRAM);
			nvram_close(RT2860_NVRAM);
		}

		// Reboot
		if (CHK_IF_DIGIT(reboot, 1)) {
			outputTimerForReload(wp, "", 80000);
			reboot_now();
		}
	}

	/* reconfigure system */
	doSystem("internet.sh");
	websDone(wp, 200);
}

#ifdef CONFIG_IPV6
parameter_fetch_t service_ipv6_flags[] = 
{
	{ T("dhcp6c_enable"), "IPv6Dhcpc", 2, T("off") },
	{ T("ipv6_allow_forward"), "IPv6AllowForward", 2, T("off") },
	{ T("ipv6_Ipv6InVPN"), "Ipv6InVPN", 2, T("off") },
#ifdef CONFIG_USER_RADVD
	{ T("radvdEnbl"), "radvdEnabled", 0, T("0") },
#endif
#ifdef CONFIG_USER_DHCP6_SERVER
	{ T("dhcpv6Enbl"), "dhcpv6Enabled", 0, T("0") },
#endif
	{ NULL, NULL, 0, NULL } // Terminator
};

/* goform/setIPv6 */
static void setIPv6(webs_t wp, char_t *path, char_t *query)
{
	char_t *opmode;
	char_t *ipaddr, *prefix_len, *wan_ipaddr, *wan_prefix_len, *srv_ipaddr, *srv_dns_primary, *srv_dns_secondary;
	char_t *reset = websGetVar(wp, T("reset"), T("0"));

	ipaddr = prefix_len = wan_ipaddr = wan_prefix_len = srv_ipaddr = srv_dns_primary = srv_dns_secondary = NULL;

	if (CHK_IF_DIGIT(reset, 1)) {
		nvram_fromdef(RT2860_NVRAM, 12, "IPv6OpMode", "IPv6IPAddr",
			"IPv6PrefixLen", "IPv6WANIPAddr", "IPv6WANPrefixLen",
			"IPv6GWAddr", "IPv6SrvAddr", "IPv6DNSPrimary", "IPv6DNSSecondary", "IPv6Dhcpc", "IPv6AllowForward",
			"Ipv6InVPN", "radvdEnabled", "dhcpv6Enabled");
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

			nvram_bufset(RT2860_NVRAM, "IPv6IPAddr", ipaddr);
			nvram_bufset(RT2860_NVRAM, "IPv6PrefixLen", prefix_len);
			nvram_bufset(RT2860_NVRAM, "IPv6WANIPAddr", wan_ipaddr);
			nvram_bufset(RT2860_NVRAM, "IPv6WANPrefixLen", wan_prefix_len);
			nvram_bufset(RT2860_NVRAM, "IPv6GWAddr", srv_ipaddr);
			nvram_bufset(RT2860_NVRAM, "IPv6DNSPrimary", srv_dns_primary);
			nvram_bufset(RT2860_NVRAM, "IPv6DNSSecondary", srv_dns_secondary);
#if defined (CONFIG_IPV6_SIT) ||  defined (CONFIG_IPV6_SIT_MODULE)
#if defined (CONFIG_IPV6_SIT_6RD)
		} else if (!strcmp(opmode, "2")) {
			ipaddr = websGetVar(wp, T("ipv6_6rd_prefix"), T(""));
			prefix_len = websGetVar(wp, T("ipv6_6rd_prefix_len"), T(""));
			srv_ipaddr = websGetVar(wp, T("ipv6_6rd_border_ipaddr"), T(""));
			nvram_bufset(RT2860_NVRAM, "IPv6IPAddr", ipaddr);
			nvram_bufset(RT2860_NVRAM, "IPv6PrefixLen", prefix_len);
			nvram_bufset(RT2860_NVRAM, "IPv6SrvAddr", srv_ipaddr);
#endif
		} else if (!strcmp(opmode, "3")) {
			ipaddr = websGetVar(wp, T("IPv6SrvAddr"), T("192.88.99.1"));
			nvram_bufset(RT2860_NVRAM, "IPv6SrvAddr", ipaddr);
#endif
		}
		nvram_bufset(RT2860_NVRAM, "IPv6OpMode", opmode);

		setupParameters(wp, service_ipv6_flags, 0);

		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}

	doSystem("internet.sh");

	websHeader(wp);
	websDone(wp, 200);
}

static int getIPv6IntAddr(int eid, webs_t wp, int argc, char_t **argv) {
	char address[INET6_ADDRSTRLEN] = "";
	char mask[16] = "";

	if (getIPv6IntIPAddr(address, mask) != 0)
	{
		return websWrite(wp, T(""));
	}
	else
	{
		return websWrite(wp, T("%s/%s"), address, mask);
	}
}

static int getIPv6ExtAddr(int eid, webs_t wp, int argc, char_t **argv) {

	char address[INET6_ADDRSTRLEN] = "";
	char mask[16] = "";

	if (getIPv6ExtIPAddr(address, mask) != 0)
	{
		return websWrite(wp, T(""));
	}
	else
	{
		return websWrite(wp, T("%s/%s"), address, mask);
	}
}
#endif

#if defined(CONFIG_USER_CHILLISPOT) || defined(CONFIG_USER_NODOGSPLASH)
#ifdef CONFIG_USER_CHILLISPOT
// ChilliSpot variables
parameter_fetch_t chilli_vars[] =
{
	{ T("chilliEnable"),		"chilli_enable",		0,       T("") },
	{ T("spotProfile"),		"chilli_profile",		0,       T("manual") },
	{ T("sPriDns"),			"chilli_dns1",			0,       T("") },
	{ T("sSecDns"),			"chilli_dns2",			0,       T("") },
	{ T("sDomain"),			"chilli_domain",		0,       T("") },
	{ T("sStartIP"),		"chilli_dhcpstart",		0,       T("") },
	{ T("sEndIP"),			"chilli_dhcpend",		0,       T("") },
	{ T("sLease"),			"chilli_lease",			0,       T("") },
	{ T("sRadServer1"),		"chilli_radiusserver1",		0,       T("") },
	{ T("sRadServer2"),		"chilli_radiusserver2",		0,       T("") },
	{ T("sRadSecret"),		"chilli_radiussecret",		0,       T("") },
	{ T("sNasId"),			"chilli_radiusnasid",		0,       T("") },
	{ T("sRadLocId"),		"chilli_radiuslocationid",	0,       T("") },
	{ T("sRadLocName"),		"chilli_radiuslocationname",	0,       T("") },
	{ T("sRadCoaPort"),		"chilli_coaport",		0,       T("") },
	{ T("sRadCoaNoIpCheck"),	"chilli_coanoipcheck",		0,       T("") },
	{ T("sUamServer"),		"chilli_uamserver",		0,       T("") },
	{ T("sUamHomepage"),		"chilli_uamhomepage",		0,       T("") },
	{ T("sUamSecret"),		"chilli_uamsecret",		0,       T("") },
	{ T("sUamAllowed"),		"chilli_uamallowed",		0,       T("") },
	{ T("sUamDomain"),		"chilli_uamdomain",		0,       T("") },
	{ T("sUamAnyDNS"),		"chilli_uamanydns",		0,       T("") },
	{ T("sMacAllowed"),		"chilli_macallowed",		0,       T("") },
	{ NULL, 			NULL, 				0,       0     }  // Terminator
};
#endif

#ifdef CONFIG_USER_NODOGSPLASH
// NoDogSplash variables
parameter_fetch_t nodog_vars[] =
{
	{ T("nodogEnable"),			"nodogsplash_enable",			0,       T("") },
	{ T("GatewayIPRange"),			"nodog_GatewayIPRange",			0,       T("0.0.0.0/0") },
	{ T("RedirectURL"),			"nodog_RedirectURL",			0,       T("http://wive-ng.sf.net") },
	{ T("MaxClients"),			"nodog_MaxClients",			0,       T("32") },
	{ T("ClientIdleTimeout"),		"nodog_ClientIdleTimeout",		0,       T("10") },
	{ T("ClientForceTimeout"),		"nodog_ClientForceTimeout",		0,       T("360") },
	{ T("AuthenticateImmediately"),		"nodog_AuthenticateImmediately",	0,       T("no") },
	{ T("MACMechanism"),			"nodog_MACMechanism",			0,       T("block") },
	{ T("BlockedMACList"),			"nodog_BlockedMACList",			0,       T("") },
	{ T("AllowedMACList"),			"nodog_AllowedMACList",			0,       T("") },
	{ T("TrustedMACList"),			"nodog_TrustedMACList",			0,       T("") },
	{ T("PasswordAuthentication"),		"nodog_PasswordAuthentication",		0,       T("no") },
	{ T("Password"),			"nodog_Password",			0,       T("nodog") },
	{ T("UsernameAuthentication"),		"nodog_UsernameAuthentication",		0,       T("no") },
	{ T("Username"),			"nodog_Username",			0,       T("guest") },
	{ T("PasswordAttempts"),		"nodog_PasswordAttempts",		0,       T("5") },
	{ NULL,					NULL,					0,       0 } // Terminator
};
#endif

#ifdef CONFIG_USER_CHILLISPOT
/*
 * description: write hotspot network ip address
 */
static int getSpotIp(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t *nvdata = nvram_get(RT2860_NVRAM, "chilli_net");

	char* slashPos = strchr(nvdata, '/');
	if (slashPos == NULL) {
		return websWrite(wp, T("172.16.0.0"));
	}
	slashPos[0] = '\0';
	return websWrite(wp, T("%s"), nvdata);
}

/*
 * description: write hotspot network netmask
 */
static int getSpotNetmask(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t *nvdata = nvram_get(RT2860_NVRAM, "chilli_net");
	unsigned int imask;
	struct in_addr mask;

	char* slashPos = strchr(nvdata, '/');
	if (slashPos == NULL) {
		return websWrite(wp, T("255.255.255.0"));
	}
	imask = atoi(slashPos+1);
	if (!imask)
		imask = 24;
	mask.s_addr = ntohl(0 - (1 << (32 - imask)));

	return websWrite(wp, T("%s"), inet_ntoa(mask));
}
#endif

/* goform/setHotspot */
static void setHotspot(webs_t wp, char_t *path, char_t *query)
{
	char_t *enabled = websGetVar(wp, T("spotEnable"), T("0"));
	char_t *reset   = websGetVar(wp, T("reset"), T("0"));

	if (CHK_IF_DIGIT(reset, 1)) {
		nvram_fromdef(RT2860_NVRAM, 39, "chilli_enable", "chilli_profile", "chilli_dns1",
			"chilli_dns2", "chilli_domain", "chilli_dhcpstart", "chilli_dhcpend",
			"chilli_lease", "chilli_radiusserver1", "chilli_radiusserver2",
			"chilli_radiussecret", "chilli_radiusnasid", "chilli_radiuslocationid",
			"chilli_radiuslocationname", "chilli_coaport", "chilli_coanoipcheck",
			"chilli_uamserver", "chilli_uamhomepage", "chilli_uamsecret", "chilli_uamallowed",
			"chilli_uamdomain", "chilli_uamanydns", "chilli_macallowed", "nodogsplash_enable",
			"nodog_GatewayIPRange", "nodog_RedirectURL", "nodog_MaxClients",
			"nodog_ClientIdleTimeout", "nodog_ClientForceTimeout", "nodog_AuthenticateImmediately",
			"nodog_MACMechanism", "nodog_BlockedMACList", "nodog_AllowedMACList",
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
			if (nvram_bufset(RT2860_NVRAM, "chilli_net", (void *)subnet)!=0) //!!!
				syslog(LOG_ERR, "set chilli_net nvram error, %s", __FUNCTION__);
			nvram_bufset(RT2860_NVRAM, "nodogsplash_enable", "0");
		}
		else
#endif
#ifdef CONFIG_USER_NODOGSPLASH
		if(CHK_IF_DIGIT(enabled, 2)) {
			setupParameters(wp, nodog_vars, 0);
			nvram_bufset(RT2860_NVRAM, "chilli_enable", "no");
		}
		else
#endif
		if(CHK_IF_DIGIT(enabled, 0)) {
#ifdef CONFIG_USER_CHILLISPOT
			nvram_bufset(RT2860_NVRAM, "chilli_enable", "no");
#endif
#ifdef CONFIG_USER_NODOGSPLASH
			nvram_bufset(RT2860_NVRAM, "nodogsplash_enable", "0");
#endif
		}

		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}
	doSystem("services_restart.sh all");
	websHeader(wp);
	websDone(wp, 200);
}

static void resetHotspot(webs_t wp, char_t *path, char_t *query)
{
	char_t *profile   = websGetVar(wp, T("profile"), T("0"));

	nvram_fromdef(RT2860_NVRAM, 39, "chilli_enable", "chilli_profile", "chilli_dns1",
		"chilli_dns2", "chilli_domain", "chilli_dhcpstart", "chilli_dhcpend",
		"chilli_lease", "chilli_radiusserver1", "chilli_radiusserver2",
		"chilli_radiussecret", "chilli_radiusnasid", "chilli_radiuslocationid",
		"chilli_radiuslocationname", "chilli_coaport", "chilli_coanoipcheck",
		"chilli_uamserver", "chilli_uamhomepage", "chilli_uamsecret", "chilli_uamallowed",
		"chilli_uamdomain", "chilli_uamanydns", "chilli_macallowed", "nodogsplash_enable",
		"nodog_GatewayIPRange", "nodog_RedirectURL", "nodog_MaxClients",
		"nodog_ClientIdleTimeout", "nodog_ClientForceTimeout", "nodog_AuthenticateImmediately",
		"nodog_MACMechanism", "nodog_BlockedMACList", "nodog_AllowedMACList",
		"nodog_TrustedMACList", "nodog_PasswordAuthentication", "nodog_Password",
		"nodog_UsernameAuthentication", "nodog_Username", "nodog_PasswordAttempts");

	nvram_init(RT2860_NVRAM);
	nvram_bufset(RT2860_NVRAM, "nodogsplash_enable", "0");
	nvram_bufset(RT2860_NVRAM, "chilli_enable", "on");
	nvram_bufset(RT2860_NVRAM, "chilli_profile", profile);
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	websHeader(wp);
	websDone(wp, 200);
}
#endif // HOTSPOT

static int getChilliBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_CHILLISPOT
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getNoDogBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_NODOGSPLASH
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

void formDefineInternet(void) {
	websAspDefine(T("getDns"), getDns);
	websAspDefine(T("getIgmpProxyBuilt"), getIgmpProxyBuilt);
	websAspDefine(T("getVPNBuilt"), getVPNBuilt);
	websAspDefine(T("getLanIp"), getLanIp);
	websAspDefine(T("getLanMac"), getLanMac);
	websAspDefine(T("getLanNetmask"), getLanNetmask);
	websAspDefine(T("getDnsmasqBuilt"), getDnsmasqBuilt);
	websAspDefine(T("getGWBuilt"), getGWBuilt);
	websAspDefine(T("getCdpBuilt"), getCdpBuilt);
	websAspDefine(T("getLltdBuilt"), getLltdBuilt);
	websAspDefine(T("getLldpdBuilt"), getLldpdBuilt);
	websAspDefine(T("getUpnpBuilt"), getUpnpBuilt);
	websAspDefine(T("getXupnpdBuilt"), getXupnpdBuilt);
	websAspDefine(T("getIntIp"), getIntIp);
	websAspDefine(T("getWanIp"), getWanIp);
	websAspDefine(T("getWanMac"), getWanMac);
	websAspDefine(T("getWanNetmask"), getWanNetmask);
	websAspDefine(T("getWanGateway"), getWanGateway);
	websAspDefine(T("getRoutingTable"), getRoutingTable);
	websAspDefine(T("getWDSBuilt"), getWDSBuilt);
	websAspDefine(T("getMBSSIDBuilt"), getMBSSIDBuilt);
	websAspDefine(T("getUSBBuilt"), getUSBBuilt);
	websAspDefine(T("getStorageBuilt"), getStorageBuilt);
	websAspDefine(T("getFtpBuilt"), getFtpBuilt);
	websAspDefine(T("getSmbBuilt"), getSmbBuilt);
	websAspDefine(T("getPrinterSrvBuilt"), getPrinterSrvBuilt);
	websAspDefine(T("getUSBModemBuilt"), getUSBModemBuilt);
	websFormDefine(T("setLan"), setLan);
	websFormDefine(T("setWan"), setWan);
	websAspDefine(T("getRadvdBuilt"), getRadvdBuilt);
	websAspDefine(T("getDhcpv6Built"), getDhcpv6Built);
	websAspDefine(T("getIPv6Built"), getIPv6Built);
	websAspDefine(T("getIPv66rdBuilt"), getIPv66rdBuilt);
	websAspDefine(T("getIP6to4Built"), getIP6to4Built);
#ifdef CONFIG_IPV6
	websFormDefine(T("setIPv6"), setIPv6);
	websAspDefine(T("getIPv6IntAddr"), getIPv6IntAddr);
	websAspDefine(T("getIPv6ExtAddr"), getIPv6ExtAddr);
#endif
	websFormDefine(T("getMyMAC"), getMyMAC);
	websFormDefine(T("editRouting"), editRouting);
	websAspDefine(T("getTransmissionBuilt"), getTransmissionBuilt);

#ifdef CONFIG_USER_ZEBRA
	websFormDefine(T("dynamicRouting"), dynamicRouting);
#endif
	websAspDefine(T("getDynamicRoutingBuilt"), getDynamicRoutingBuilt);
	websAspDefine(T("getSWQoSBuilt"), getSWQoSBuilt);
	websAspDefine(T("getDATEBuilt"), getDATEBuilt);
	websAspDefine(T("getDDNSBuilt"), getDDNSBuilt);
	websAspDefine(T("getSpotBuilt"), getSpotBuilt);

	websAspDefine(T("getLANAUTHBuilt"), getLANAUTHBuilt);

	websAspDefine(T("getSysLogBuilt"), getSysLogBuilt);

	websAspDefine(T("vpnShowVPNStatus"), vpnShowVPNStatus);
	websFormDefine(T("formVPNSetup"), formVPNSetup);

	websFormDefine(T("restoremac"), restoremac);

#ifdef CONFIG_USER_CHILLISPOT
	websAspDefine(T("getSpotIp"), getSpotIp);
	websAspDefine(T("getSpotNetmask"), getSpotNetmask);
#endif
#if defined(CONFIG_USER_CHILLISPOT) || defined(CONFIG_USER_NODOGSPLASH)
	websFormDefine(T("setHotspot"), setHotspot);
	websFormDefine(T("resetHotspot"), resetHotspot);
#endif
	websAspDefine(T("getChilliBuilt"), getChilliBuilt);
	websAspDefine(T("getNoDogBuilt"), getNoDogBuilt);
}
