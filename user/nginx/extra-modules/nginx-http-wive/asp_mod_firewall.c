#include "asp_mod_firewall.h"

/*
 * ASP function
 */
static int getPortForwardRules(webs_t *wp, char** params, int nparams)
{
	int first=1, i=0;
	char_t *rules = ngx_nvram_get(wp, "PortForwardRules");
	struct PortForwardRule rule;

	if (rules == NULL)
		return 0;
	if (strlen(rules) == 0)
		return 0;

	while (parse_portforward_rule(rules, &rule, i++) == 0)
	{
		// Output data
		outWrite(T("%s[ '%s', %d, '%s', '%s', '%s', '%s', '%s', %d, '%s' ]"),
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
static int getPortFilteringRules(webs_t *wp, char** params, int nparams)
{
	int i = 0, first = 1;
	char_t *rules = ngx_nvram_get(wp, "IPPortFilterRules");
	struct PortFilteringRule rule;

	if (rules == NULL)
		return 0;

	while (parse_portfiltering_rule(rules, &rule, i++) == 0 && i < 65535)
	{
//                ELOG_ERR(wp->log, 0, "Parsing portfiltering rule %i \n", i);

		// Output data
		outWrite(T("%s[ '%s', %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s' ]"),
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

/*
 * ASP function
 */
static int getPortFilteringInputRules(webs_t *wp, char** params, int nparams)
{
	int i = 0, first = 1;
	char_t *rules = ngx_nvram_get(wp, "IPPortFilterInputRules");
	struct PortFilteringRule rule;

	if (rules == NULL)
		return 0;

	while (parse_portfiltering_rule(rules, &rule, i++) == 0 && i < 65535)
	{
		// Output data
		outWrite(T("%s[ '%s', %d, '%s', '%s', '%s', '%s', '%s', , , '%s', '%s', '%s', '%s' ]"),
				(first) ? "" : ",\n\t",
				rule.interface,
				rule.protocol,
				rule.mac_address,
				rule.sip, rule.sim,
				rule.sprf, rule.sprt,
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
	doSystem("echo f > /proc/net/nf_conntrack");
}

/* goform/portForward */
static void portForward(webs_t* wp, char_t *path, char_t *query)
{
	char_t *pfe = websGetVar(wp, T("portForwardEnabled"), T("0"));
	char_t *PortForwardRules = websGetVar(wp, T("portForwardRules"), T(""));

	nvram_init(RT2860_NVRAM);
	ngx_nvram_bufset(wp, "PortForwardEnable", pfe);
	if (CHK_IF_DIGIT(pfe, 1))
		ngx_nvram_bufset(wp, "PortForwardRules", PortForwardRules);
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	firewall_rebuild();
//	websHeader(wp);
	websDone(wp, 200);
}

/* goform/portFiltering */
static void portFiltering(webs_t* wp, char_t *path, char_t *query)
{
	char_t *firewall_enable = websGetVar(wp, T("portFilterEnabled"), T("0"));
	char_t *default_policy = websGetVar(wp, T("defaultFirewallPolicy"), T("0"));
	char_t *firewall_rules = websGetVar(wp, T("portFilteringRules"), T(""));

	nvram_init(RT2860_NVRAM);
	ngx_nvram_bufset(wp, "IPPortFilterEnable", firewall_enable);
	if (CHK_IF_DIGIT(firewall_enable, 1))
	{
		ngx_nvram_bufset(wp, "DefaultFirewallPolicy", default_policy);
		ngx_nvram_bufset(wp, "IPPortFilterRules", firewall_rules);
	}
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	firewall_rebuild();
//	websHeader(wp);
	websDone(wp, 200);
}

/* goform/portFilteringInput */
static void portFilteringInput(webs_t* wp, char_t *path, char_t *query)
{
	char_t *firewall_enable = websGetVar(wp, T("portFilterInputEnabled"), T("0"));
	char_t *firewall_rules = websGetVar(wp, T("portFilteringInputRules"), T(""));

	nvram_init(RT2860_NVRAM);
	ngx_nvram_bufset(wp, "IPPortFilterInputEnable", firewall_enable);
	if (CHK_IF_DIGIT(firewall_enable, 1))
		ngx_nvram_bufset(wp, "IPPortFilterInputRules", firewall_rules);
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	firewall_rebuild();
//	websHeader(wp);
	websDone(wp, 200);
}

/* goform/setFirewallDMZ */
static void setFirewallDMZ(webs_t* wp, char_t *path, char_t *query)
{
	char *dmzEnable, *dmzIPaddr, *dmzLoopback;
	char *reset = websGetVar(wp, T("reset"), T("0"));

	if (CHK_IF_DIGIT(reset, 1)) {
		nvram_fromdef(RT2860_NVRAM, 3, "DMZEnabled", "DMZIPAddress", "dmzLoopback");
	}
	else {
		dmzEnable   = websGetVar(wp, T("DMZEnabled"), T(""));
		dmzIPaddr   = websGetVar(wp, T("DMZIPAddress"), T(""));
		dmzLoopback = websGetVar(wp, T("dmzLoopback"), T("off"));

		if (CHK_IF_DIGIT(dmzEnable, 1)) {
			nvram_init(RT2860_NVRAM);
			ngx_nvram_bufset(wp, "DMZEnable", dmzEnable);
			ngx_nvram_bufset(wp, "DMZIPAddress", dmzIPaddr);
			ngx_nvram_bufset(wp, "DMZNATLoopback", dmzLoopback);
			nvram_commit(RT2860_NVRAM);
			nvram_close(RT2860_NVRAM);
		}
		else {
			ngx_nvram_set(wp, "DMZEnable", "0");
		}
	}
	firewall_rebuild();
//	websHeader(wp);
	websDone(wp, 200);
}

/* goform/setFirewallAlg */
parameter_fetch_t alg_params[] =
{
	{ "alg_ftp",		("fwAlgFTP"),		2,	("") },
	{ "alg_gre",		("fwAlgGRE"),		2,	("") },
	{ "alg_h323",		("fwAlgH323"),		2,	("") },
	{ "alg_pptp",		("fwAlgPPTP"),		2,	("") },
	{ "alg_sip",		("fwAlgSIP"),		2,	("") },
	{ "alg_rtsp",		("fwAlgRTSP"),		2,	("") },
	{ NULL,			NULL,			0,	NULL  }  // Terminator
};

static void setFirewallALG(webs_t* wp, char_t *path, char_t *query)
{
	setupParameters(wp, alg_params, 1);
	firewall_rebuild();
	doSystem("/etc/init.d/modules load_nat_helpers");
//	websHeader(wp);
	websDone(wp, 200);
}

/* goform/setFirewall */
parameter_fetch_t firewall_params[] =
{
	{ "ForwardSesLimit",	("ForwardSesLimit"),	0,	("0") },
	{ "dnsToLocalRedir",	("dnsToLocalRedir"),	0,	("0") },
	{ NULL,			NULL,			0,	NULL   }  // Terminator
};

static void setFirewall(webs_t* wp, char_t *path, char_t *query)
{
	setupParameters(wp, firewall_params, 1);
	firewall_rebuild();
//	websHeader(wp);
	websDone(wp, 200);
}

#if defined(CONFIG_NETFILTER_XT_MATCH_WEBSTR) || defined(CONFIG_NETFILTER_XT_MATCH_WEBSTR_MODULE)
/* goform/webContentFilterSetup */
parameter_fetch_t content_filtering_args[] =
{
	{ ("urlFiltering"),		"websURLFilters",	0,	("") },
	{ ("hostFiltering"),		"websHostFilters",	0,	("") },
	{ ("websFilterProxy"),		"websFilterProxy",	2,	("") },
	{ ("websFilterJava"),		"websFilterJava",	2,	("") },
	{ ("websFilterActivex"),	"websFilterActivex",	2,	("") },
	{ ("websFilterCookies"),	"websFilterCookies",	2,	("") },
	{ NULL,				NULL,			0,	NULL  }  // Terminator
};

static void webContentFilterSetup(webs_t* wp, char_t *path, char_t *query)
{
	setupParameters(wp, content_filtering_args, 1);
	firewall_rebuild();
//	websHeader(wp);
	websDone(wp, 200);
}
#endif

static int getWebstrBuilt(webs_t *wp, char** params, int nparams)
{
#if defined(CONFIG_NETFILTER_XT_MATCH_WEBSTR) || defined(CONFIG_NETFILTER_XT_MATCH_WEBSTR_MODULE)
       return outWrite(T("1"));
#else
       return outWrite(T("0"));
#endif
}

void asp_mod_firewall_init(void)
{
	aspDefineFunc("getPortFilteringRules",          getPortFilteringRules,          EVERYONE);
	aspDefineFunc("getPortFilteringInputRules",     getPortFilteringInputRules,     EVERYONE);
	aspDefineFunc("getPortForwardRules",            getPortForwardRules,            EVERYONE);

	websFormDefine("portFiltering",                 portFiltering,                  EVERYONE);
	websFormDefine("portFilteringInput",            portFilteringInput,             EVERYONE);
	websFormDefine("portForward",                   portForward,                    EVERYONE);
	websFormDefine("setFirewall",                   setFirewall,                    EVERYONE);
	websFormDefine("setFirewallDMZ",                setFirewallDMZ,                 EVERYONE);
	websFormDefine("setFirewallALG",                setFirewallALG,                 EVERYONE);

	aspDefineFunc("getWebstrBuilt",                 getWebstrBuilt,                 EVERYONE);
#if defined(CONFIG_NETFILTER_XT_MATCH_WEBSTR) || defined(CONFIG_NETFILTER_XT_MATCH_WEBSTR_MODULE)
	websFormDefine("webContentFilterSetup",         webContentFilterSetup,          EVERYONE);
#endif
}
