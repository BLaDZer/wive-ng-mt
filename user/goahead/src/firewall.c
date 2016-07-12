/*
 *	firewall.c -- Firewall Settings 
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: firewall.c,v 1.29.2.1 2009-03-24 08:56:16 yy Exp $
 */

/*
 *	if  WAN or LAN ip changed, we must restart firewall.
 */

#include "helpers.h"
#include "firewall.h"

static int getDMZEnableASP(int eid, webs_t wp, int argc, char_t **argv)
{
	int type, value;
	char *dmze = nvram_get(RT2860_NVRAM, "DMZEnable");

	if (dmze)
		value = atoi(dmze);
	else
		value = 0;

	if( ejArgs(argc, argv, T("%d"), &type) == 1){
		if(type == value)
			websWrite(wp, T("selected"));
		else
			websWrite(wp, T(" "));
		return 0;
	}
	return -1;
}

/*
 * ASP function
 */
static int getPortForwardRules(int eid, webs_t wp, int argc, char_t **argv)
{
	int first=1, i=0;
	int prf_int, prt_int, rprf_int, rprt_int, proto, inat_loopback;
	char ip_address[32], prf[8], prt[8], rprf[8], rprt[8], comment[64], protocol[8], interface[8], nat_loopback[8];
	char rec[128];
	char_t *rules = nvram_get(RT2860_NVRAM, "PortForwardRules");

	if (rules == NULL)
		return 0;
	if (strlen(rules) == 0)
		return 0;

	/* format is :
	 * [interface],[protocol],[src_port],[dst_port],[ip_address],[redirect_src_port],[redirect_dst_port],[nat_loopback],[comment];
	 */
	while(getNthValueSafe(i++, rules, ';', rec, sizeof(rec)) != -1)
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

		// get port range "to"
		if ((getNthValueSafe(3, rec, ',', prt, sizeof(prt)) == -1))
			continue;

		if (strlen(prt) > 0)
		{
			if ((prt_int = atoi(prt)) > 65535)
				continue;
		}

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
			if ((rprf_int = atoi(rprf)) == 0 || rprf_int > 65535)
				continue;
		}

		// get port range "to"
		if ((getNthValueSafe(6, rec, ',', rprt, sizeof(rprt)) == -1))
			continue;

		if (strlen(rprt) > 0)
		{
			if ((rprt_int = atoi(rprt)) > 65535)
				continue;
		}

		// get Nat Loopback enable flag
		if ((getNthValueSafe(7, rec, ',', nat_loopback, sizeof(nat_loopback)) == -1))
			continue;

		if (strlen(nat_loopback) > 0)
			inat_loopback = atoi(nat_loopback);
		else
			inat_loopback = 0;


		// Get comment
		if ((getNthValueSafe(8, rec, ',', comment, sizeof(comment)) == -1))
			continue;

		// Output data
		websWrite(wp, T("%s[ '%s', %d, '%s', '%s', '%s', '%s', '%s', %d, '%s' ]"),
				(first) ? "" : ",\n\t",
				interface,
				proto,
				prf, prt,
				ip_address,
				rprf, rprt,
				inat_loopback,
				comment
			);

		first = 0;
	}

	return 0;
}

/*
 * ASP function
 */
static int getPortFilteringRules(int eid, webs_t wp, int argc, char_t **argv)
{
	int i = 0, first = 1;
	int sprf_int, sprt_int, proto, dprf_int, dprt_int;
	char mac_address[32];
	char sip[32], sprf[8], sprt[8], comment[16], protocol[8], action[4];
	char dip[32], dprf[8], dprt[8], iface[8], sim[32], dim[32];
	char rec[256];
	char_t *rules = nvram_get(RT2860_NVRAM, "IPPortFilterRules");

	if (rules == NULL)
		return 0;

	while (getNthValueSafe(i++, rules, ';', rec, sizeof(rec)) != -1)
	{
		// Get interface
		if ((getNthValueSafe(0, rec, ',', iface, sizeof(iface)) == -1))
			continue;

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
		if ((getNthValueSafe(11, rec, ',', action, sizeof(action)) == -1))
			continue;

		// get comment
		if ((getNthValueSafe(12, rec, ',', comment, sizeof(comment)) == -1))
			continue;

		// Output data
		websWrite(wp, T("%s[ '%s', %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s' ]"),
				(first) ? "" : ",\n\t",
				iface,
				proto,
				mac_address,
				sip, sim,
				sprf, sprt,
				dip, dim,
				dprf, dprt,
				action,
				comment
			);

		first = 0;
	}

	return 0;
}

void firewall_rebuild(void)
{
	//rebuild firewall scripts in etc
	firewall_rebuild_etc();
	doSystem("service iptables restart");
}

static int showDMZIPAddressASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char *DMZIPAddress = nvram_get(RT2860_NVRAM, "DMZIPAddress");

	if(!DMZIPAddress)
		return 0;
	if(!strlen(DMZIPAddress))
		return 0;

	websWrite(wp, T("%s"), DMZIPAddress);
	return 0;
}

static void portForward(webs_t wp, char_t *path, char_t *query)
{
	char_t *pfe = websGetVar(wp, T("portForwardEnabled"), T("0"));
	char_t *PortForwardRules = websGetVar(wp, T("portForwardRules"), T(""));

	pfe = (CHK_IF_DIGIT(pfe, 1)) ? "1" : "0";

	// Commit
	nvram_init(RT2860_NVRAM);
	nvram_bufset(RT2860_NVRAM, "PortForwardEnable", pfe);
	if (CHK_IF_DIGIT(pfe, 1))
		nvram_bufset(RT2860_NVRAM, "PortForwardRules", PortForwardRules);
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

#ifdef PRINT_DEBUG
	websHeader(wp);
	websWrite(wp, T("portForwardEnabled: %s<br>\n"), pfe);
	websFooter(wp);
	websDone(wp, 200);
#else
	char_t *submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
#endif

	// call iptables
	firewall_rebuild();
}

static void portFiltering(webs_t wp, char_t *path, char_t *query)
{
	char_t *firewall_enable = websGetVar(wp, T("portFilterEnabled"), T("0"));
	char_t *default_policy = websGetVar(wp, T("defaultFirewallPolicy"), T("0"));
	char_t *firewall_rules = websGetVar(wp, T("portFilteringRules"), T(""));

	firewall_enable = (CHK_IF_DIGIT(firewall_enable, 1)) ? "1" : "0";
	default_policy = (CHK_IF_DIGIT(default_policy, 1)) ? "1" : "0";

	nvram_init(RT2860_NVRAM);
	nvram_bufset(RT2860_NVRAM, "IPPortFilterEnable", firewall_enable);
	if (CHK_IF_DIGIT(firewall_enable, 1))
	{
		// Store default firewall policy & rules
		nvram_bufset(RT2860_NVRAM, "DefaultFirewallPolicy", default_policy);
		nvram_bufset(RT2860_NVRAM, "IPPortFilterRules", firewall_rules);
	}

	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

#ifdef PRINT_DEBUG
	websHeader(wp);
	websWrite(wp, T("portFilteringEnabled: %s<br>\n"), firewall_enable);
	websWrite(wp, T("default_policy: %s<br>\n"), default_policy);
	websFooter(wp);
	websDone(wp, 200);
#else
	char_t *submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
#endif

	// Call iptables
	firewall_rebuild();
	// flush conntrack for apply changes immediately
	doSystem("echo f > /proc/net/nf_conntrack");
}

static void DMZ(webs_t wp, char_t *path, char_t *query)
{
	char *dmzE, *ip_address, *dmzLoopback;
	char_t *reset = websGetVar(wp, T("reset"), T("0"));

	if (CHK_IF_DIGIT(reset, 1)) {
		nvram_fromdef(RT2860_NVRAM, 3, "DMZEnable", "DMZIPAddress", "DMZNATLoopback");
		goto out;
	}

	dmzE = websGetVar(wp, T("DMZEnabled"), T(""));
	ip_address = websGetVar(wp, T("DMZIPAddress"), T(""));
	dmzLoopback = websGetVar(wp, T("dmzLoopback"), T("off"));
	char_t *submitUrl;

	if (CHK_IF_DIGIT(dmzE, 0)) // disable
	{
		nvram_set(RT2860_NVRAM, "DMZEnable", "0");
	}
	else if (CHK_IF_DIGIT(dmzE, 1)) // enable
	{
		if (!isIpValid(ip_address))
			return;

		nvram_init(RT2860_NVRAM);
		nvram_bufset(RT2860_NVRAM, "DMZEnable", "1");
		nvram_bufset(RT2860_NVRAM, "DMZIPAddress", ip_address);
		nvram_bufset(RT2860_NVRAM, "DMZNATLoopback", CHK_IF_DIGIT(dmzLoopback, 1) ? "1" : "0");
		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}
	else
		return;
out:
#ifdef PRINT_DEBUG
	websHeader(wp);
	websWrite(wp, T("DMZEnabled: %s<br>\n"), dmzE);
	websWrite(wp, T("ip_address: %s<br>\n"), ip_address);
	websFooter(wp);
	websDone(wp, 200);
#else
	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
#endif

	// Call iptables
	firewall_rebuild();
}

static void websSysFirewall(webs_t wp, char_t *path, char_t *query)
{
	char *wpfE = websGetVar(wp, T("pingFrmWANFilterEnabled"), T(""));

	// someone use malform page.....
	if(!wpfE || !strlen(wpfE))
		return;


	websHeader(wp);
	websWrite(wp, T("WANPingFilter: %s<br>\n"), wpfE);
	websFooter(wp);
	websDone(wp, 200);

	// call iptables
	firewall_rebuild();
}


parameter_fetch_t content_filtering_args[] =
{
	{ T("urlFiltering"),           "websURLFilters",       0,   T("") },
	{ T("hostFiltering"),          "websHostFilters",      0,   T("") },
	{ T("websFilterProxy"),        "websFilterProxy",      2,   T("") },
	{ T("websFilterJava"),         "websFilterJava",       2,   T("") },
	{ T("websFilterActivex"),      "websFilterActivex",    2,   T("") },
	{ T("websFilterCookies"),      "websFilterCookies",    2,   T("") },

	{ NULL, NULL, 0, NULL } // Terminator
};

static void webContentFilterSetup(webs_t wp, char_t *path, char_t *query)
{
	char_t *submitUrl;

	// Store firewall parameters
	setupParameters(wp, content_filtering_args, 1);

	//call iptables
	firewall_rebuild();

	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);

}

/* goform/setFirewallAlg */
parameter_fetch_t alg_params[] =
{
	{ "alg_ftp",		T("fwAlgFTP"),			2,   T("") },
	{ "alg_gre",		T("fwAlgGRE"),			2,   T("") },
	{ "alg_h323",		T("fwAlgH323"),			2,   T("") },
	{ "alg_pptp",		T("fwAlgPPTP"),			2,   T("") },
	{ "alg_sip",		T("fwAlgSIP"),			2,   T("") },
	{ "alg_rtsp",		T("fwAlgRTSP"),			2,   T("") },
	{ NULL, NULL, 0, NULL } // Terminator
};

static void setFirewallAlg(webs_t wp, char_t *path, char_t *query)
{
	char_t *submitUrl;

	// Store firewall parameters
	setupParameters(wp, alg_params, 1);

	//call iptables
	firewall_rebuild();

	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);

}

/* goform/setFirewall */
parameter_fetch_t firewall_params[] =
{
	{ "ForwardSesLimit",		T("ForwardSesLimit"),			0,   T("0") },
	{ NULL, NULL, 0, NULL } // Terminator
};

static void setFirewall(webs_t wp, char_t *path, char_t *query)
{
	// Store firewall parameters
	setupParameters(wp, firewall_params, 1);

	//call iptables
	firewall_rebuild();

	char_t *submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
}

void formDefineFirewall(void)
{
	websFormDefine(T("portFiltering"), portFiltering);

	websAspDefine(T("getPortFilteringRules"), getPortFilteringRules);

	websFormDefine(T("DMZ"), DMZ);
	websAspDefine(T("getDMZEnableASP"), getDMZEnableASP);
	websAspDefine(T("showDMZIPAddressASP"), showDMZIPAddressASP);

	websFormDefine(T("setFirewallAlg"), setFirewallAlg);

	websAspDefine(T("getPortForwardRules"), getPortForwardRules);
	websFormDefine(T("portForward"), portForward);

	websFormDefine(T("websSysFirewall"), websSysFirewall);
	websFormDefine(T("webContentFilterSetup"), webContentFilterSetup);

	websFormDefine(T("setFirewall"), setFirewall);
}
