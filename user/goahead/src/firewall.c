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

/*
 * ASP function
 */
static int getPortForwardRules(int eid, webs_t wp, int argc, char_t **argv)
{
	int first=1, i=0;
	char_t *rules = nvram_get(RT2860_NVRAM, "PortForwardRules");
	struct PortForwardRule rule;

	if (rules == NULL)
		return 0;
	if (strlen(rules) == 0)
		return 0;

	while (parse_portforward_rule(rules, &rule, i++) == 0)
	{
		// Output data
		websWrite(wp, T("%s[ '%s', %d, '%s', '%s', '%s', '%s', '%s', %d, '%s' ]"),
				(first) ? "" : ",\n\t",
				rule.interface,
				rule.protocol,
				rule.prf, rule.prt,
				rule.ip_address,
				rule.rprf, rule.rprt,
				rule.nat_loopback,
				rule.comment
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
	char_t *rules = nvram_get(RT2860_NVRAM, "IPPortFilterRules");	
	struct PortFilteringRule rule;

	if (rules == NULL)
		return 0;

	while (parse_portfiltering_rule(rules, &rule, i++) == 0)
	{
		// Output data
		websWrite(wp, T("%s[ '%s', %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s' ]"),
				(first) ? "" : ",\n\t",
				rule.interface,
				rule.protocol,
				rule.mac_address,
				rule.sip, rule.sim,
				rule.sprf, rule.sprt,
				rule.dip, rule.dim,
				rule.dprf, rule.dprt,
				rule.action,
				rule.comment
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

/* goform/portForward */
static void portForward(webs_t wp, char_t *path, char_t *query)
{
	char_t *pfe = websGetVar(wp, T("portForwardEnabled"), T("0"));
	char_t *PortForwardRules = websGetVar(wp, T("portForwardRules"), T(""));

	nvram_init(RT2860_NVRAM);
	nvram_bufset(RT2860_NVRAM, "PortForwardEnable", pfe);
	if (CHK_IF_DIGIT(pfe, 1))
		nvram_bufset(RT2860_NVRAM, "PortForwardRules", PortForwardRules);
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	firewall_rebuild();
	websHeader(wp);
	websDone(wp, 200);
}

/* goform/portFiltering */
static void portFiltering(webs_t wp, char_t *path, char_t *query)
{
	char_t *firewall_enable = websGetVar(wp, T("portFilterEnabled"), T("0"));
	char_t *default_policy = websGetVar(wp, T("defaultFirewallPolicy"), T("0"));
	char_t *firewall_rules = websGetVar(wp, T("portFilteringRules"), T(""));

	nvram_init(RT2860_NVRAM);
	nvram_bufset(RT2860_NVRAM, "IPPortFilterEnable", firewall_enable);
	if (CHK_IF_DIGIT(firewall_enable, 1))
	{
		nvram_bufset(RT2860_NVRAM, "DefaultFirewallPolicy", default_policy);
		nvram_bufset(RT2860_NVRAM, "IPPortFilterRules", firewall_rules);
	}
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	firewall_rebuild();
	doSystem("echo f > /proc/net/nf_conntrack");
	websHeader(wp);
	websDone(wp, 200);
}

/* goform/setFirewallDMZ */
static void setFirewallDMZ(webs_t wp, char_t *path, char_t *query)
{
	char *dmzEnable, *dmzIPaddr, *dmzLoopback;

	dmzEnable   = websGetVar(wp, T("DMZEnabled"), T(""));
	dmzIPaddr   = websGetVar(wp, T("DMZIPAddress"), T(""));
	dmzLoopback = websGetVar(wp, T("dmzLoopback"), T("off"));

	if (CHK_IF_DIGIT(dmzEnable, 1)) {
		nvram_init(RT2860_NVRAM);
		nvram_bufset(RT2860_NVRAM, "DMZEnable", dmzEnable);
		nvram_bufset(RT2860_NVRAM, "DMZIPAddress", dmzIPaddr);
		nvram_bufset(RT2860_NVRAM, "DMZNATLoopback", dmzLoopback);
		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}
	else {
		nvram_set(RT2860_NVRAM, "DMZEnable", "0");
	}
	firewall_rebuild();
	websHeader(wp);
	websDone(wp, 200);
}

/* goform/webContentFilterSetup */
parameter_fetch_t content_filtering_args[] =
{
	{ T("urlFiltering"),		"websURLFilters",	0,	T("") },
	{ T("hostFiltering"),		"websHostFilters",	0,	T("") },
	{ T("websFilterProxy"),		"websFilterProxy",	2,	T("") },
	{ T("websFilterJava"),		"websFilterJava",	2,	T("") },
	{ T("websFilterActivex"),	"websFilterActivex",	2,	T("") },
	{ T("websFilterCookies"),	"websFilterCookies",	2,	T("") },
	{ NULL,				NULL,			0,	NULL  }  // Terminator
};

static void webContentFilterSetup(webs_t wp, char_t *path, char_t *query)
{
	setupParameters(wp, content_filtering_args, 1);
	firewall_rebuild();
	websHeader(wp);
	websDone(wp, 200);
}

/* goform/setFirewallAlg */
parameter_fetch_t alg_params[] =
{
	{ "alg_ftp",		T("fwAlgFTP"),		2,	T("") },
	{ "alg_gre",		T("fwAlgGRE"),		2,	T("") },
	{ "alg_h323",		T("fwAlgH323"),		2,	T("") },
	{ "alg_pptp",		T("fwAlgPPTP"),		2,	T("") },
	{ "alg_sip",		T("fwAlgSIP"),		2,	T("") },
	{ "alg_rtsp",		T("fwAlgRTSP"),		2,	T("") },
	{ NULL,			NULL,			0,	NULL  }  // Terminator
};

static void setFirewallALG(webs_t wp, char_t *path, char_t *query)
{
	setupParameters(wp, alg_params, 1);
	firewall_rebuild();
	doSystem("/etc/init.d/modules load_nat_helpers");
	websHeader(wp);
	websDone(wp, 200);
}

/* goform/setFirewall */
parameter_fetch_t firewall_params[] =
{
	{ "ForwardSesLimit",	T("ForwardSesLimit"),	0,	T("0") },
	{ NULL,			NULL,			0,	NULL   }  // Terminator
};

static void setFirewall(webs_t wp, char_t *path, char_t *query)
{
	setupParameters(wp, firewall_params, 1);
	firewall_rebuild();
	websHeader(wp);
	websDone(wp, 200);
}

void formDefineFirewall(void)
{
	websAspDefine(T("getPortFilteringRules"), getPortFilteringRules);
	websAspDefine(T("getPortForwardRules"), getPortForwardRules);
	websFormDefine(T("portFiltering"), portFiltering);
	websFormDefine(T("portForward"), portForward);
	websFormDefine(T("setFirewall"), setFirewall);
	websFormDefine(T("setFirewallDMZ"), setFirewallDMZ);
	websFormDefine(T("setFirewallALG"), setFirewallALG);
	websFormDefine(T("webContentFilterSetup"), webContentFilterSetup);

}
