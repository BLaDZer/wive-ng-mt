#include "asp_mod_services.h"

static int getProcessList(webs_t *wp, char** params, int nparams)
{
    FUNCTION_TRACE(wp->log);

    if (nparams < 1) return 0;

    cmdline_t *proc_list = procps_list();
    cmdline_t *curr = proc_list;

    while (curr != NULL)
    {
        int i;

        if (curr->argc < 1)
        {
            curr = curr->next;
            continue;
        }

        for (i=0;i<nparams;i++)
        {
            char* proc_name = params[i]; //cstring_trim(wp->pool, params[i], '"');
            // Check if name matches
            if (strcmp(curr->argv[0], proc_name) == 0)
            {
                outWrite("%s,", curr->argv[0]);
                break;
            }
        }

        // Move pointer
        curr = curr->next;
    }

    // Free structure, NULL is properly handled
    procps_free(proc_list);

    return 0;
}


/*
 * description: write DHCP client list
 */
static int getDhcpCliList(webs_t *wp, char** params, int nparams)
{
	int i, rownum;
        unsigned int j;
	struct in_addr addr;
	uint64_t written_at, curr, expired_abs;
	char str[32] = {0};
	int emptylease = 1;

	int row_len = 0;
	struct dyn_lease* leases = getDhcpClientList(&row_len, &written_at);

	outWrite("{ \"data\": [ ");

	if (leases == NULL) {
	    outWrite("] }");
	    return 0; //if DHCP is disabled or unable to read - just exit
	}

	curr = time(NULL);

	/* Output leases file */
	for (rownum = 0; rownum < row_len; rownum++) {
		struct dyn_lease lease = leases[rownum];

		expired_abs = ntohl(lease.expires) + written_at;
		if (expired_abs > curr) {
			outWrite(T("%s{ "), (emptylease == 0) ? ", " : "");
			// Output structure
			// Host
			str[0] = '\0';
			for (j = 0; j < strlen(lease.hostname); j++) {
				if (lease.hostname[j] == '"' ||
				    lease.hostname[j] == '\\' ||
				    lease.hostname[j] == '\'' ||
				    lease.hostname[j] == '\t' ||
				    lease.hostname[j] == '\r' ||
				    lease.hostname[j] == '\n' ||
				    lease.hostname[j] == '\v' ||
				    lease.hostname[j] == '\f' ||
				    lease.hostname[j] == '\b')
					sprintf(str, "%s%c", str, '\\');
				sprintf(str, "%s%c", str, lease.hostname[j]);
			}
			outWrite(T("\"hostname\":\"%s\", "), str);
			// MAC
			outWrite(T("\"mac\":\"%02X"), lease.lease_mac[0]);
			for (i = 1; i < 6; i++)
				outWrite(T(":%02X"), lease.lease_mac[i]);
			outWrite(T("\", "));
			// IP
			addr.s_addr = lease.lease_nip;
			outWrite(T("\"ip\":\"%s\", "), inet_ntoa(addr));

			// Expire Date
			outWrite("\"expire\":\"");
			if (expired_abs > curr)
			{
				leasetime_t expires = expired_abs - curr;
				unsigned d = expires / (24*60*60);
				expires %= (24*60*60);
				unsigned h = expires / (60*60);
				expires %= (60*60);
				unsigned m = expires / 60;
				expires %= 60;
				if (d > 0)
					outWrite(T("%u days "), d);
				outWrite(T("%02u:%02u:%02u\""), h, m, (unsigned)expires);
			}
			outWrite("}");
			emptylease = 0;
		}
	}
	outWrite("] }");

	if (leases)
	    free(leases);

	return 0;
}

parameter_fetch_t dhcp_args[] =
{
	{ ("dhcpStart"),               "dhcpStart",            0,       ("") },
	{ ("dhcpEnd"),                 "dhcpEnd",              0,       ("") },
	{ ("dhcpMask"),                "dhcpMask",             0,       ("") },
	{ ("dhcpGateway"),             "dhcpGateway",          0,       ("") },
	{ ("dhcpLease"),               "dhcpLease",            0,       ("86400") },
	{ ("dhcpDomain"),              "dhcpDomain",           0,       ("localdomain") },
	{ ("dhcpARPPTimeout"),         "dhcpARPPTimeout",      0,       ("2000") },
	{ NULL, NULL, 0, NULL }
};

parameter_fetch_t dhcp_args_dns[] =
{
	{ ("dhcpPriDns"),              "dhcpPriDns",           0,       ("") },
	{ ("dhcpSecDns"),              "dhcpSecDns",           0,       ("") },
	{ NULL, NULL, 0, NULL }
};

/* goform/setDhcp */
static void setDhcp(webs_t* wp, char_t *path, char_t *query)
{
	char_t *dhcpEnabled, *static_leases, *reset;

	dhcpEnabled	= websGetVar(wp, T("dhcpEnabled"), T("0"));
	static_leases	= websGetVar(wp, T("dhcpAssignIP"), T(""));
	reset		= websGetVar(wp, T("reset"), T("0"));

	if (CHK_IF_DIGIT(reset, 1)) {
		nvram_fromdef(RT2860_NVRAM, 10, "dhcpStart", "dhcpEnd", "dhcpMask", "dhcpGateway", "dhcpLease",
						"dhcpDomain", "dhcpARPPTimeout", "dhcpEnabled", "dhcpPriDns", "dhcpSecDns");
		ngx_nvram_set(wp, "dhcpStatic", "");
	}
	else {
		if (dhcpEnabled == NULL)
			dhcpEnabled = "0";

		nvram_init(RT2860_NVRAM);

		// configure gateway and dns (WAN) at bridge mode
		if (CHK_IF_DIGIT(dhcpEnabled, 1)) {
			ngx_nvram_bufset(wp, "dhcpEnabled", "1");
			setupParameters(wp, dhcp_args, 0);

			int dns_proxy = ngx_nvram_get_int(wp, "dnsPEnabled", 0);
			if (dns_proxy == 1) {
				ngx_nvram_bufset(wp, "dhcpPriDns", "");
				ngx_nvram_bufset(wp, "dhcpSecDns", "");
			}
			else
				setupParameters(wp, dhcp_args_dns, 0);

			// Store leases to nvram
			ngx_nvram_bufset(wp, "dhcpStatic", static_leases);
		}
		else if (CHK_IF_DIGIT(dhcpEnabled, 0))
			ngx_nvram_bufset(wp, "dhcpEnabled", "0");

		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}
	// Remove old lease file
	doSystem("rm -f /var/udhcpd.dhcpLeases");
	// Restart DHCP service
	doSystem("service dhcpd restart");
//	websHeader(wp);
	websDone(wp, 200);
}

static int getWSDDBuilt(webs_t *wp, char** params, int nparams)
{
#if defined(CONFIG_USER_SAMBA) && defined(CONFIG_USER_WSDD2)
	outWrite(T("1"));
#else
	outWrite(T("0"));
#endif
	return 0;
}


static int getSNMPDBuilt(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_USER_SNMPD
	outWrite(T("1"));
#else
	outWrite(T("0"));
#endif
	return 0;
}

static int getFastPathBuilt(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_BCM_NAT
	outWrite(T("1"));
#else
	outWrite(T("0"));
#endif
	return 0;
}

static int getARPptBuilt(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_USER_PARPROUTED
	outWrite(T("1"));
#else
	outWrite(T("0"));
#endif
	return 0;
}

static int getTelnetdBuilt(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_TELNETD
	outWrite(T("1"));
#else
	outWrite(T("0"));
#endif
	return 0;
}

static int getIPTAccountBuilt(webs_t *wp, char** params, int nparams)
{
#if defined(CONFIG_IP_NF_MATCH_ACCOUNT) || defined(CONFIG_IP_NF_MATCH_ACCOUNT_MODULE)
	outWrite(T("1"));
#else
	outWrite(T("0"));
#endif
	return 0;
}

parameter_fetch_t service_misc_flags[] =
{
	{ ("stpEnbl"),			"stpEnabled",		0, ("0")  },
#ifdef CONFIG_USER_CDP
	{ ("cdpEnbl"),			"cdpEnabled",		0, ("0")  },
#endif
#ifdef CONFIG_USER_ARPWATCH
	{ ("arpwatchEnbl"),		"arpwatch",		0, ("0")  },
#endif
#ifdef CONFIG_USER_LLTD
	{ ("lltdEnbl"),		"lltdEnabled",		0, ("0")  },
#endif
#ifdef CONFIG_USER_LLDPD
	{ ("lldpdEnbl"),		"lldpdEnabled",		0, ("0")  },
#endif
	{ ("igmpEnbl"),		"igmpEnabled",		0, ("0")  },
	{ ("igmpSnoop"),		"igmpSnoopMode",	0, ("")   },
	{ ("igmpFastL"),		"igmpFastLeave",	0, ("")   },
	{ ("igmpM2UConv"),		"igmpM2UConvMode",	0, ("")   },
#ifdef CONFIG_USER_MINIUPNPD
	{ ("upnpEnbl"),		"upnpEnabled",		0, ("0")  },
#endif
#ifdef CONFIG_USER_XUPNPD
	{ ("xupnpdEnbl"),		"xupnpd",		0, ("0")  },
#endif
	{ ("dnspEnbl"),		"dnsPEnabled",		0, ("0")  },
	{ ("rmtHTTP"),			"RemoteManagement",	0, ("0")  },
	{ ("RemoteManagementPort"),	"RemoteManagementPort",	0, ("80") },
	{ ("RemoteManagementPortHTTPS"),	"RemoteManagementPortHTTPS",	0, ("443") },
#ifdef CONFIG_USER_DROPBEAR
	{ ("rmtSSH"),			"RemoteSSH",		0, ("0")  },
	{ ("RemoteSSHPort"),		"RemoteSSHPort",	0, ("22") },
#endif
#ifdef CONFIG_TELNETD
	{ ("rmtTelnet"),		"RemoteTelnet", 0, ("0") },
#endif
#ifdef CONFIG_USER_UDPXY
	{ ("udpxyMode"),		"UDPXYMode",		0, ("0")  },
	{ ("udpxyPort"),		"UDPXYPort",		0, ("81") },
#endif
	{ ("watchdogEnable"),		"WatchdogEnabled",	0, ("0")  },
	{ ("pingWANEnbl"),		"WANPingFilter",	0, ("0")  },
	{ ("krnlPppoePass"),		"pppoe_pass",		0, ("0")  },
	{ ("krnlIpv6Pass"),		"ipv6_pass",		0, ("0")  },
	{ ("dhcpSwReset"),		"dhcpSwReset",		0, ("0")  },
	{ ("vlanDoubleTag"),		"vlanDoubleTag",	0, ("0")  },
	{ ("offloadMode"),		"offloadMode",		0, ("0")  },
	{ ("hw_nat_wifiPT"),		"hw_nat_wifi",		0, ("0")  },
	{ ("hw_nat_udpPT"),		"hw_nat_udp",		0, ("0")  },
	{ ("hw_nat_sixPT"),		"hw_nat_six",		0, ("0")  },
	{ ("natMode"),			"nat_mode",		0, ("1")  },
	{ ("natFastpath"),		"natFastpath",		0, ("0")  },
	{ ("routeFastpath"),		"routeFastpath",	0, ("1")  },
	{ ("filterFastpath"),		"filterFastpath",	0, ("1")  },
#ifdef CONFIG_CROND
	{ ("CrondEnable"),		"CrondEnable",		0, ("0")  },
#endif
	{ ("ForceRenewDHCP"),		"ForceRenewDHCP",	0, ("1")  },
#ifdef CONFIG_USER_PARPROUTED
	{ ("arpPT"),			"parproutedEnabled",	0, ("0")  },
#endif
	{ ("pingerEnable"),		"pinger_check_on",	0, ("0")  },
	{ ("ping_check_time"),		"ping_check_time",	0, ("0")  },
	{ ("ping_check_interval"),	"ping_check_interval",	0, ("0")  },
	{ ("ttlStore"),		"store_ttl",		0, ("0")  },
	{ ("ttlMcastStore"),		"store_ttl_mcast",	0, ("0")  },
#ifdef CONFIG_USER_SNMPD
	{ ("SnmpdEnabled"),		"snmpd",		0, ("0")  },
	{ ("snmpdcommunity"),		"snmpdcommunity",	0, ("")   },
#endif
	{ ("mssPmtu"),			"mss_use_pmtu",		0, ("1")  },
#ifdef CONFIG_RALINK_MT7621
	{ ("IRQBalance"),		"IRQBalance",		0, ("auto") },
#endif
#ifdef CONFIG_NETFILTER_FP_SMB
	{ ("smbFastpath"),		"smbFastpath",		0, ("0")  },
#endif
	{ NULL,				NULL,			0, NULL    } // Terminator
};

/* goform/setMiscServices */
static void setMiscServices(webs_t* wp, char_t *path, char_t *query)
{
	char_t *goaheadrestart	= websGetVar(wp, T("goaheadrestart"), T("1"));
	char_t *reset		= websGetVar(wp, T("reset"), T("0"));

	websSetContentType(wp, "text/html");

	if (CHK_IF_DIGIT(reset, 1)) {
		nvram_fromdef(RT2860_NVRAM, 48, "stpEnabled", "cdpEnabled", "arpwatch", "lltdEnabled", "lldpdEnabled",
						"igmpEnabled", "igmpSnoopMode", "igmpFastLeave", "igmpM2UConvMode", "upnpEnabled",
						"xupnpd", "dnsPEnabled", "RemoteManagement", "RemoteManagementPort",  "RemoteSSH",
						"RemoteSSHPort", "RemoteTelnet", "UDPXYMode", "UDPXYPort", "WatchdogEnabled",
						"WANPingFilter", "pppoe_pass", "ipv6_pass", "dhcpSwReset", "vlanDoubleTag",
						"offloadMode", "hw_nat_wifi", "hw_nat_udp", "hw_nat_six", "nat_mode",
						"natFastpath", "routeFastpath", "filterFastpath", "CrondEnable", "ForceRenewDHCP",
						"parproutedEnabled", "pinger_check_on", "ping_check_time", "ping_check_interval", "store_ttl",
						"store_ttl_mcast", "snmpd", "snmpdcommunity", "mss_use_pmtu", "IRQBalance",
						"smbFastpath", "hw_nat_bind", "RemoteManagementPortHTTPS");
	}
	else {
		nvram_init(RT2860_NVRAM);
		setupParameters(wp, service_misc_flags, 0);

		int nat_fp = ngx_nvram_get_int(wp, "offloadMode", -1);
		if (nat_fp == 2 || nat_fp == 3)
		{
			char_t *nat_th = websGetVar(wp, "hwnatThreshold", "50");
			if (nat_th != NULL)
				ngx_nvram_bufset(wp, "hw_nat_bind", nat_th);
		}

		int dns_proxy = ngx_nvram_get_int(wp, "dnsPEnabled", 0);
		if (dns_proxy == 1)
		{
			ngx_nvram_bufset(wp, "dhcpPriDns", "");
			ngx_nvram_bufset(wp, "dhcpSecDns", "");
		}

		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}

	/* restart some services instead full reload */
	wp->on_response_ok = DO_RESTART_MISC;


	if (CHK_IF_DIGIT(goaheadrestart, 1)) {
                websHeader(wp);
		outputTimerForReload(wp, 60000);
                websFooter(wp);

                //FIXME: restart nginx only
		wp->on_response_ok = DO_REBOOT;
	}

	websDone(wp, 200);
}


/* goform/cwmpConfig */
static void cwmpConfig(webs_t* wp, char_t *path, char_t *query)
{
	char_t *reset = websGetVar(wp, T("reset"), T("0"));

	if (CHK_IF_DIGIT(reset, 1)) {
		nvram_fromdef(RT2860_NVRAM, 21, "cwmpdEnabled", "cwmp_cpe_auth", "cwmp_acs_url", "cwmp_acs_auth", "cwmp_acs_username"
						"cwmp_acs_password", "cwmp_cpe_auth", "cwmp_cpe_username", "cwmp_cpe_password", "cwmp_ssl_ca_file", "cwmp_ssl_private_key_file", "cwmp_ssl_public_key_file",
						"cwmp_ssl_ca_password", "cwmp_log_level", "cwmpd_httpd_port", "cwmpd_http_timeout", "cwmpd_session_connect_timeout",
						"cwmpd_session_response_timeout", "cwmp_cpe_manufacture", "cwmp_cpe_oui", "cwmp_cpe_pc", "cwmpd_inform_periodic_enable", "cwmpd_inform_periodic_interval");
	}
	else {
		char_t *cwmp_enabled = websGetVar(wp, T("cwmp_enabled"), T("0"));

		if (cwmp_enabled == NULL)
			cwmp_enabled = "0";

		nvram_init(RT2860_NVRAM);

		if (CHK_IF_DIGIT(cwmp_enabled, 1) || CHK_IF_DIGIT(cwmp_enabled, 2)) {
			ngx_nvram_bufset(wp, "cwmpdEnabled",			cwmp_enabled);
			ngx_nvram_bufset(wp, "cwmp_acs_type",			websGetVar(wp, T("cwmp_acs_type"), T("")));
			ngx_nvram_bufset(wp, "cwmp_acs_url",			websGetVar(wp, T("cwmp_acs_url"), T("")));
			ngx_nvram_bufset(wp, "cwmp_acs_auth",			websGetVar(wp, T("cwmp_acs_auth"), T("")));
			ngx_nvram_bufset(wp, "cwmp_acs_username",		websGetVar(wp, T("cwmp_acs_username"), T("")));
			ngx_nvram_bufset(wp, "cwmp_acs_password",		websGetVar(wp, T("cwmp_acs_password"), T("")));
			ngx_nvram_bufset(wp, "cwmp_cpe_auth",			websGetVar(wp, T("cwmp_cpe_auth"), T("")));
			ngx_nvram_bufset(wp, "cwmp_cpe_username",		websGetVar(wp, T("cwmp_cpe_username"), T("")));
			ngx_nvram_bufset(wp, "cwmp_cpe_password",		websGetVar(wp, T("cwmp_cpe_password"), T("")));
			ngx_nvram_bufset(wp, "cwmp_ssl_ca_file",		websGetVar(wp, T("cwmp_ssl_ca_file"), T("")));
			ngx_nvram_bufset(wp, "cwmp_ssl_ca_password",		websGetVar(wp, T("cwmp_ssl_ca_password"), T("")));
			ngx_nvram_bufset(wp, "cwmp_ssl_private_key_file",	websGetVar(wp, T("cwmp_ssl_private_key_file"), T("")));
			ngx_nvram_bufset(wp, "cwmp_ssl_public_key_file",	websGetVar(wp, T("cwmp_ssl_public_key_file"), T("")));
			ngx_nvram_bufset(wp, "cwmpd_log_level",			websGetVar(wp, T("cwmpd_log_level"), T("")));
			ngx_nvram_bufset(wp, "cwmpd_httpd_port",		websGetVar(wp, T("cwmpd_httpd_port"), T("")));
			ngx_nvram_bufset(wp, "cwmpd_httpd_timeout",		websGetVar(wp, T("cwmpd_httpd_timeout"), T("")));
			ngx_nvram_bufset(wp, "cwmpd_session_connect_timeout",	websGetVar(wp, T("cwmpd_session_connect_timeout"), T("")));
			ngx_nvram_bufset(wp, "cwmpd_session_response_timeout",	websGetVar(wp, T("cwmpd_session_response_timeout"), T("")));
			ngx_nvram_bufset(wp, "cwmp_cpe_manufacture",		websGetVar(wp, T("cwmp_cpe_manufacture"), T("")));
			ngx_nvram_bufset(wp, "cwmp_cpe_oui",			websGetVar(wp, T("cwmp_cpe_oui"), T("")));
			ngx_nvram_bufset(wp, "cwmp_cpe_pc",			websGetVar(wp, T("cwmp_cpe_pc"), T("")));
			ngx_nvram_bufset(wp, "cwmpd_inform_periodic_enable",	websGetVar(wp, T("cwmpd_inform_periodic_enable"), T("")));
			ngx_nvram_bufset(wp, "cwmpd_inform_periodic_interval",	websGetVar(wp, T("cwmpd_inform_periodic_interval"), T("")));
		}
		else
			ngx_nvram_bufset(wp, "cwmpdEnabled", "0");

		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}

	firewall_rebuild(0);
	doSystem("service cwmpd restart");

//	websHeader(wp);
	websDone(wp, 200);
}

static void cwmpEnableAuto(webs_t* wp, char_t *path, char_t *query)
{
	nvram_init(RT2860_NVRAM);

	ngx_nvram_bufset(wp, "cwmpdEnabled", "2");

	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	firewall_rebuild(0);
	doSystem("service cwmpd restart");
	websDone(wp, 200);
}


//------------------------------------------------------------------------------
// Samba/Wins setup
parameter_fetch_t service_samba_flags[] =
{
	{ ("WsddEnabled"), "WsddEnabled", 0, ("") },
	{ ("WorkGroup"), "WorkGroup", 0, ("") },
	{ ("SmbNetBIOS"), "SmbNetBIOS", 0, ("") },
	{ ("SmbString"), "SmbString", 0, ("") },
	{ ("SmbOsLevel"), "SmbOsLevel", 0, ("") },
	{ ("SmbTimeserver"), "SmbTimeserver", 0, ("0") },
	{ NULL, NULL, 0, NULL } // Terminator
};

static void setSamba(webs_t* wp, char_t *path, char_t *query)
{
	char_t *smb_enabled = websGetVar(wp, T("SmbEnabled"), T("0"));
	char_t *submitUrl;

	if (smb_enabled == NULL)
		smb_enabled = "0";

	nvram_init(RT2860_NVRAM);
	ngx_nvram_bufset(wp, "SmbEnabled", smb_enabled);

	if (CHK_IF_DIGIT(smb_enabled, 1))
		setupParameters(wp, service_samba_flags, 0);

	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	//restart some services instead full reload
	doSystem("service dhcpd restart");
	doSystem("service samba restart");

	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
}

//------------------------------------------------------------------------------
// IPT Accounting
void formIptAccounting(webs_t* wp, char_t *path, char_t *query)
{
	char_t *iptEnable, *reset, *resetCounters;

	iptEnable	= websGetVar(wp, T("iptEnable"), T("0"));
	reset		= websGetVar(wp, T("reset"), T("0"));
	resetCounters	= websGetVar(wp, T("resetCounters"), T("0"));

	if (CHK_IF_DIGIT(reset, 1)) {
		ngx_nvram_set(wp, "ipt_account", "0");
	}
	else {
		ngx_nvram_set(wp, "ipt_account", iptEnable);
		if (CHK_IF_DIGIT(resetCounters, 1) || CHK_IF_DIGIT(iptEnable, 0)) {
			FILE *fd = fopen(_PATH_IPT_ACCOUNTING_FILE, "w");
			if (fd != NULL) {
				fputs("reset", fd);
				fclose(fd);
			}
		}
	}

    firewall_rebuild(0);
    websRedirect(wp, "/services/account.asp");
}

#ifndef IPT_SHORT_ACCOUNT
const char *iptProtocolNames[]=
{
	"All", "TCP", "UDP", "ICMP", "Others"
};
#endif /* !IPT_SHORT_ACCOUNT */

// Output IP Accounting statistics
int iptStatList(webs_t *wp, char** params, int nparams)
{
	FILE *fd;
	char_t ip[32], line[256];
	long long b_src[5], p_src[5], b_dst[5], p_dst[5], time;
	int lines = 0;
	int first = 1;

	outWrite("{ \"data\": [ ");

	// Do not show anything if nat_fastpath is set
	int nat_fp = ngx_nvram_get_int(wp, "offloadMode", -1);
	if (nat_fp > 0) {
		outWrite("] }");
		return 0;
	}

	if ((fd = fopen(_PATH_IPT_ACCOUNTING_FILE, "r"))!=NULL) {
		// Output table header
		while ((fgets(line, sizeof(line), fd)) != NULL)
		{
			lines++;
			if (first == 0)
				outWrite(", ");
#ifdef IPT_SHORT_ACCOUNT
			sscanf(line,
				"%*s %*s %s "   // IP
				"%*s %*s %lld "  // bytes_src
				"%*s %*s %lld "  // packets_src
				"%*s %*s %lld "  // bytes_dst
				"%*s %*s %lld "  // packets_dst
				"%*s %*s %lld ", // time
				ip, &b_src[0], &p_src[0], &b_dst[0], &p_dst[0], &time);
			const char *src_sz = normalizeSize(&b_src[0]);
			const char *dst_sz = normalizeSize(&b_dst[0]);

			outWrite(T(" { "
				"\"ip\":\"%s\", "
				"\"proto\":\"\", "
				"\"tx_bytes\":\"%ld %s\", "
				"\"tx_packets\":\"%ld\", "
				"\"rx_bytes\":\"%ld %s\", "
				"\"rx_packets\":\"%ld\", "
				"\"time\":\"%ld\"} "
				),
				ip, (long)b_src[0], src_sz, (long)p_src[0], (long)b_dst[0], dst_sz, (long)p_dst[0], (long)time);
#else
			sscanf(line,
				"%*s %*s %s "   // IP
				"%*s %*s %lld %lld %lld %lld %lld "  // bytes_src
				"%*s %*s %lld %lld %lld %lld %lld "  // packets_src
				"%*s %*s %lld %lld %lld %lld %lld "  // bytes_dst
				"%*s %*s %lld %lld %lld %lld %lld "  // packets_dst
				"%*s %*s %lld ", // time
				ip,
				&b_src[0], &b_src[1], &b_src[2], &b_src[3], &b_src[4],
				&p_src[0], &p_src[1], &p_src[2], &p_src[3], &p_src[4],
				&b_dst[0], &b_dst[1], &b_dst[2], &b_dst[3], &b_dst[4],
				&p_dst[0], &p_dst[1], &p_dst[2], &p_dst[3], &p_dst[4],
				&time);

			int i;
			for (i = 0; i < 5; i++) {
				const char *src_sz = normalizeSize(&b_src[i]);
				const char *dst_sz = normalizeSize(&b_dst[i]);
				outWrite(T(" { "
					"\"ip\":\"%s\", "
					"\"proto\":\"%s\", "
					"\"num\":\"%i\", "
					"\"tx_bytes\":\"%ld %s\", "
					"\"tx_packets\":\"%ld\", "
					"\"rx_bytes\":\"%ld %s\", "
					"\"rx_packets\":\"%ld\", "
					"\"time\":\"%ld\"} "
					),
					ip, iptProtocolNames[i], i, b_src[i], src_sz, p_src[i], b_dst[i], dst_sz, p_dst[i], time);
			}
#endif /* IPT_SHORT_ACCOUNT */
			first = 0;
		}

		fclose(fd);
	}

	outWrite("] }");
	return 0;
}

parameter_fetch_t service_l2tp_flags[] =
{
	{ ("l2tp_srv_ip_local"),	"l2tp_srv_ip_local",		0, ("")     },
	{ ("l2tp_srv_ip_range"),	"l2tp_srv_ip_range",		0, ("")     },
	{ ("l2tp_srv_lcp_adapt"),	"l2tp_srv_lcp_adapt",		2, ("off")  },
	{ ("l2tp_srv_debug"),		"l2tp_srv_debug",		2, ("off")  },
	{ ("l2tp_srv_nat_enabled"),	"l2tp_srv_nat_enabled",		2, ("off")  },
	{ ("l2tp_srv_mppe_enabled"),	"l2tp_srv_mppe_enabled",	2, ("off")  },
	{ ("l2tp_srv_proxyarp"),	"l2tp_srv_proxyarp",		2, ("off")  },
	{ ("l2tp_srv_mtu_size"),	"l2tp_srv_mtu_size",		0, ("1460") },
	{ ("l2tp_srv_mru_size"),	"l2tp_srv_mru_size",		0, ("1460") },
	{ ("l2tp_srv_user_num"),	"l2tp_srv_user_num",		0, ("0")    },
	{ NULL,				NULL,				0, NULL      } // Terminator
};

static void l2tpConfig(webs_t* wp, char_t *path, char_t *query)
{
	char user_var[16];
	char pass_var[16];
	int i, count;
	char_t *reset = websGetVar(wp, T("reset"), T("0"));

	if (CHK_IF_DIGIT(reset, 1)) {
		count = ngx_nvram_get_int(wp, "l2tp_srv_user_num", 0);
		nvram_fromdef(RT2860_NVRAM, 11, "l2tp_srv_enabled", "l2tp_srv_ip_local", "l2tp_srv_ip_range", "l2tp_srv_lcp_adapt", "l2tp_srv_debug",
			"l2tp_srv_nat_enabled", "l2tp_srv_mppe_enabled", "l2tp_srv_proxyarp", "l2tp_srv_mtu_size", "l2tp_srv_mru_size", "l2tp_srv_user_num");
		nvram_init(RT2860_NVRAM);
		for (i = 0; i < count; i++) {
			sprintf(user_var, "l2tp_srv_user%d", i);
			sprintf(pass_var, "l2tp_srv_pass%d", i);
			nvram_bufset(RT2860_NVRAM, user_var, "");
			nvram_bufset(RT2860_NVRAM, pass_var, "");
		}
		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}
	else {
		char_t *l2tp_enabled = websGetVar(wp, T("l2tp_srv_enabled"), T("0"));

		if (l2tp_enabled == NULL)
			l2tp_enabled = "0";

		nvram_init(RT2860_NVRAM);

		if (CHK_IF_DIGIT(l2tp_enabled, 1)) {
			int oldcount = ngx_nvram_get_int(wp, "l2tp_srv_user_num", 0);
			ngx_nvram_bufset(wp, "l2tp_srv_enabled", "1");
			setupParameters(wp, service_l2tp_flags, 0);

			// Set-up logins
			char_t *l2tp_srv_user_num = websGetVar(wp, T("l2tp_srv_user_num"), T("0"));
			count = atoi(l2tp_srv_user_num);
			for (i = 0; i < count; i++) {
				sprintf(user_var, "l2tp_srv_user%d", i);
				sprintf(pass_var, "l2tp_srv_pass%d", i);

				char_t *user = websGetVar(wp, user_var, "");
				char_t *pass = websGetVar(wp, pass_var, "");

				if (!(CHK_IF_SET(user) || CHK_IF_SET(pass))) {
					user = "";
					pass = "";
				}

				nvram_bufset(RT2860_NVRAM, user_var, user);
				nvram_bufset(RT2860_NVRAM, pass_var, pass);
			}
			// Clear old logins
			if (oldcount > count)
				for (i = count; i < oldcount; i++) {
					sprintf(user_var, "l2tp_srv_user%d", i);
					sprintf(pass_var, "l2tp_srv_pass%d", i);
					nvram_bufset(RT2860_NVRAM, user_var, "");
					nvram_bufset(RT2860_NVRAM, pass_var, "");
				}
		}
		else
			ngx_nvram_bufset(wp, "l2tp_srv_enabled", "0");

		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}

	firewall_rebuild(1);
	doSystem("service vpnserver restart");

//	websHeader(wp);
	websDone(wp, 200);
}

static int getL2TPUserList(webs_t *wp, char** params, int nparams)
{
	char user_var[16];
	char pass_var[16];
	char str[32] = {0};
	int count = ngx_nvram_get_int(wp, "l2tp_srv_user_num", 0);
	int i;
        unsigned int j;

	if (count > 0) {
		for (i = 0; i < count; i++) {
			sprintf(user_var, "l2tp_srv_user%d", i);
			sprintf(pass_var, "l2tp_srv_pass%d", i);
			char *user = nvram_get(RT2860_NVRAM, user_var);
			char *pass = nvram_get(RT2860_NVRAM, pass_var);

			str[0] = '\0';
			for (j = 0; j < strlen(pass); j++) {
				if (pass[j] == '\\' || pass[j] == '\'')
					sprintf(str, "%s%c", str, '\\');
				sprintf(str, "%s%c", str, pass[j]);
			}

			outWrite(T("[ '%s', '%s' ]%s"), user, str, (i + 1 < count) ? ", " : " ");
		}
	}
	return 0;
}

static void radiusConfig(webs_t* wp, char_t *path, char_t *query)
{
	char user_var[20];
	char pass_var[20];
	int i, count;
	char_t *reset = websGetVar(wp, T("reset"), T("0"));
	char_t *radius_secret = websGetVar(wp, T("radius_srv_secret"), T(""));
	char *ssecret = ngx_nvram_get(wp, "radius_srv_secret");

	if (CHK_IF_DIGIT(reset, 1)) {
		count = ngx_nvram_get_int(wp, "radius_srv_user_num", 0);
		nvram_fromdef(RT2860_NVRAM, 3, "radius_srv_enabled", "radius_srv_user_num", "radius_srv_secret");
		nvram_init(RT2860_NVRAM);
		for (i = 0; i < count; i++) {
			sprintf(user_var, "radius_srv_user%d", i);
			sprintf(pass_var, "radius_srv_pass%d", i);
			nvram_bufset(RT2860_NVRAM, user_var, "");
			nvram_bufset(RT2860_NVRAM, pass_var, "");
		}
		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
		ssecret = ngx_nvram_get(wp, "radius_srv_secret");
	}
	else {
		char_t *radius_enabled = websGetVar(wp, T("radius_srv_enabled"), T("0"));

		if (radius_enabled == NULL)
			radius_enabled = "0";

		nvram_init(RT2860_NVRAM);

		if (CHK_IF_DIGIT(radius_enabled, 1)) {
			int oldcount = ngx_nvram_get_int(wp, "radius_srv_user_num", 0);
			char_t *radius_user_num = websGetVar(wp, T("radius_srv_user_num"), T("0"));

			ngx_nvram_bufset(wp, "radius_srv_enabled", "1");
			ngx_nvram_bufset(wp, "radius_srv_user_num", radius_user_num);
			ngx_nvram_bufset(wp, "radius_srv_secret", radius_secret);

			// Set-up logins
			char_t *radius_srv_user_num = websGetVar(wp, T("radius_srv_user_num"), T("0"));
			count = atoi(radius_srv_user_num);
			for (i = 0; i < count; i++) {
				sprintf(user_var, "radius_srv_user%d", i);
				sprintf(pass_var, "radius_srv_pass%d", i);

				char_t *user = websGetVar(wp, user_var, "");
				char_t *pass = websGetVar(wp, pass_var, "");

				if (!(CHK_IF_SET(user) || CHK_IF_SET(pass))) {
					user = "";
					pass = "";
				}

				nvram_bufset(RT2860_NVRAM, user_var, user);
				nvram_bufset(RT2860_NVRAM, pass_var, pass);
			}
			// Clear old logins
			if (oldcount > count)
				for (i = count; i < oldcount; i++) {
					sprintf(user_var, "radius_srv_user%d", i);
					sprintf(pass_var, "radius_srv_pass%d", i);
					nvram_bufset(RT2860_NVRAM, user_var, "");
					nvram_bufset(RT2860_NVRAM, pass_var, "");
				}
		}
		else
			ngx_nvram_bufset(wp, "radius_srv_enabled", "0");

		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}

	firewall_rebuild(0);
	if (!strcmp(radius_secret, ssecret))
		doSystem("service radius reload");
	else
		doSystem("service radius restart");

//	websHeader(wp);
	websDone(wp, 200);
}


static int getRadiusUserList(webs_t *wp, char** params, int nparams)
{
	char user_var[20];
	char pass_var[20];
	char str[40] = {0};
	int count = ngx_nvram_get_int(wp, "radius_srv_user_num", 0);
	int i;
        unsigned int j;

	if (count > 0) {
		for (i = 0; i < count; i++) {
			sprintf(user_var, "radius_srv_user%d", i);
			sprintf(pass_var, "radius_srv_pass%d", i);
			char *user = nvram_get(RT2860_NVRAM, user_var);
			char *pass = nvram_get(RT2860_NVRAM, pass_var);

			str[0] = '\0';
			for (j = 0; j < strlen(pass); j++) {
				if (pass[j] == '\\' || pass[j] == '\'')
					sprintf(str, "%s%c", str, '\\');
				sprintf(str, "%s%c", str, pass[j]);
			}

			outWrite(T("[ '%s', '%s' ]%s"), user, str, (i + 1 < count) ? ", " : " ");
		}
	}
	return 0;
}


static int getSMPBuilt(webs_t *wp, char** params, int nparams) {
#ifdef CONFIG_RALINK_MT7621
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getOpenSSLBuilt(webs_t *wp, char** params, int nparams) {
#ifdef CONFIG_LIB_OPENSSL
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getSmbFPBuilt(webs_t *wp, char** params, int nparams) {
#ifdef CONFIG_NETFILTER_FP_SMB
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getARPwatchBuilt(webs_t *wp, char** params, int nparams) {
#ifdef CONFIG_USER_ARPWATCH
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getCWMPAutoAvailable(webs_t *wp, char** params, int nparams) {
#if defined(CONFIG_USER_CWMPD)
	FILE *f = fopen("/tmp/acsurl", "rt");
	if (f) {
		fclose(f);
		return outWrite(T("1"));
	}
#endif
	return outWrite(T("0"));
}

void asp_mod_services_init()
{
	// Define forms

	websFormDefine(T("setDhcp"), setDhcp, EVERYONE);
	websFormDefine(T("formSamba"), setSamba, EVERYONE);
	websFormDefine(T("setMiscServices"), setMiscServices, EVERYONE);
	websFormDefine(T("formIptAccounting"), formIptAccounting, EVERYONE);
	websFormDefine(T("l2tpConfig"), l2tpConfig, ADMIN);
	websFormDefine(T("radiusConfig"), radiusConfig, ADMIN);
	websFormDefine(T("cwmpConfig"), cwmpConfig, ADMIN);
	websFormDefine(T("cwmpEnableAuto"), cwmpEnableAuto, ADMIN);

	// Define functions
	aspDefineFunc(("getL2TPUserList"), getL2TPUserList, EVERYONE);
	aspDefineFunc(("getRadiusUserList"), getRadiusUserList, EVERYONE);
	aspDefineFunc(("getDhcpCliList"), getDhcpCliList, EVERYONE);
	aspDefineFunc(("iptStatList"), iptStatList, EVERYONE);
	aspDefineFunc(("getWSDDBuilt"), getWSDDBuilt, EVERYONE);
	aspDefineFunc(("getARPptBuilt"), getARPptBuilt, EVERYONE);
	aspDefineFunc(("getFastPathBuilt"), getFastPathBuilt, EVERYONE);
	aspDefineFunc(("getTelnetdBuilt"), getTelnetdBuilt, EVERYONE);
	aspDefineFunc(("getSNMPDBuilt"), getSNMPDBuilt, EVERYONE);
	aspDefineFunc(("getIPTAccountBuilt"), getIPTAccountBuilt, EVERYONE);
	aspDefineFunc(("getProcessList"), getProcessList, EVERYONE);
	aspDefineFunc(("getSMPBuilt"), getSMPBuilt, EVERYONE);
	aspDefineFunc(("getOpenSSLBuilt"), getOpenSSLBuilt, EVERYONE);
	aspDefineFunc(("getSmbFPBuilt"), getSmbFPBuilt, EVERYONE);
	aspDefineFunc(("getARPwatchBuilt"), getARPwatchBuilt, EVERYONE);
	aspDefineFunc(("getCWMPAutoAvailable"), getCWMPAutoAvailable, EVERYONE);
}
