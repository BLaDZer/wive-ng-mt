/* vi: set sw=4 ts=4 sts=4 fdm=marker: */
/*
 *	services.c -- services
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: services.c,v 1.167.2.8 2009-04-22 01:31:35 chhung Exp $
 */

#include	"helpers.h"
#include	"management.h"
#include	"station.h"
#include	"firewall.h"


static int getProcessList(int eid, webs_t wp, int argc, char_t **argv)
{
	if (argc <= 0)
		return 0;

	cmdline_t *proc_list = procps_list();
	cmdline_t *curr = proc_list;

	while (curr != NULL)
	{
		int i=0;
		while (i < argc)
		{
			// Check if name matches
			if ((curr->argc > 0) && (strcmp(curr->argv[0], argv[i]) == 0))
			{
				websWrite(wp, "%s,", curr->argv[0]);
				break;
			}
			i++;
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
static int getDhcpCliList(int eid, webs_t wp, int argc, char_t **argv)
{
	int i, rownum;
	struct in_addr addr;
	uint64_t written_at, curr, expired_abs;

	int row_len = 0;
	struct dyn_lease* leases = getDhcpClientList(&row_len, &written_at);

	if (leases == NULL)
	    return 0; //if DHCP is disabled or unable to read - just exit
	
	curr = time(NULL);

	/* Output leases file */
	for (rownum=0; rownum<row_len; rownum++)
	{
		struct dyn_lease lease = leases[rownum];

		expired_abs = ntohl(lease.expires) + written_at;
		if (expired_abs > curr) {
			// Output structure
			// Host
			websWrite(wp, T("<tr><td>%s&nbsp;</td>"), lease.hostname);
			// MAC
			websWrite(wp, T("<td id=\"dhclient_row%d_mac\">%02X"), rownum, lease.lease_mac[0]);
			for (i = 1; i < 6; i++)
				websWrite(wp, T(":%02X"), lease.lease_mac[i]);
			// IP
			addr.s_addr = lease.lease_nip;
			websWrite(wp, T("</td><td id=\"dhclient_row%d_ip\">%s</td><td>"), rownum, inet_ntoa(addr));

			// Expire Date
			if (expired_abs > curr)
			{
				leasetime_t expires = expired_abs - curr;
				unsigned d = expires / (24*60*60);
				expires %= (24*60*60);
				unsigned h = expires / (60*60);
				expires %= (60*60);
				unsigned m = expires / 60;
				expires %= 60;
				if (d>0)
					websWrite(wp, T("%u days "), d);
				websWrite(wp, T("%02u:%02u:%02u</td>"), h, m, (unsigned)expires);
			}

			websWrite(wp, "<td id=\"dhclient_row%d_status\" style=\"text-align: center;\">"
				"<input id=\"dhclient_row%d\" type=\"checkbox\" onchange=\"toggleDhcpTable(this);\"></td>",
				rownum, rownum);
			websWrite(wp, "</tr>\n");
		}
	}

	if (leases)
	    free(leases);

	return 0;
}

parameter_fetch_t dhcp_args[] =
{
	{ T("dhcpStart"),               "dhcpStart",            0,       T("") },
	{ T("dhcpEnd"),                 "dhcpEnd",              0,       T("") },
	{ T("dhcpMask"),                "dhcpMask",             0,       T("") },
	{ T("dhcpGateway"),             "dhcpGateway",          0,       T("") },
	{ T("dhcpLease"),               "dhcpLease",            0,       T("86400") },
	{ T("dhcpDomain"),              "dhcpDomain",           0,       T("localdomain") },
	{ T("dhcpARPPTimeout"),         "dhcpARPPTimeout",      0,       T("2000") },
	{ NULL, NULL, 0, NULL }
};

parameter_fetch_t dhcp_args_dns[] =
{
	{ T("dhcpPriDns"),              "dhcpPriDns",           0,       T("") },
	{ T("dhcpSecDns"),              "dhcpSecDns",           0,       T("") },
	{ NULL, NULL, 0, NULL }
};

/* goform/setDhcp */
static void setDhcp(webs_t wp, char_t *path, char_t *query)
{
	char_t	*dhcpEnabled, *static_leases, *submitUrl;

	dhcpEnabled = websGetVar(wp, T("dhcpEnabled"), T("0"));
	static_leases = websGetVar(wp, T("dhcpAssignIP"), T(""));

	if (dhcpEnabled == NULL)
		dhcpEnabled = "0";

	nvram_init(RT2860_NVRAM);

	// configure gateway and dns (WAN) at bridge mode
	if (CHK_IF_DIGIT(dhcpEnabled, 1))
	{
		nvram_bufset(RT2860_NVRAM, "dhcpEnabled", "1");
		setupParameters(wp, dhcp_args, 0);

		int dns_proxy = nvram_get_int(RT2860_NVRAM, "dnsPEnabled", 0);
		if (dns_proxy == 1)
		{
			nvram_bufset(RT2860_NVRAM, "dhcpPriDns", "");
			nvram_bufset(RT2860_NVRAM, "dhcpSecDns", "");
		}
		else
			setupParameters(wp, dhcp_args_dns, 0);

		// Store leases to nvram
		nvram_bufset(RT2860_NVRAM, "dhcpStatic", static_leases);
	}
	else if (CHK_IF_DIGIT(dhcpEnabled, 0))
		nvram_bufset(RT2860_NVRAM, "dhcpEnabled", "0");

	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	// Restart DHCP service
	doSystem("service dhcpd restart");

	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
}



static int getSNMPDBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_SNMPD
	websWrite(wp, T("1"));
#else
	websWrite(wp, T("0"));
#endif
	return 0;
}

static int getFastPathBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_BCM_NAT
	websWrite(wp, T("1"));
#else
	websWrite(wp, T("0"));
#endif
	return 0;
}

static int getARPptBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_PARPROUTED
	websWrite(wp, T("1"));
#else
	websWrite(wp, T("0"));
#endif
	return 0;
}

static int getTelnetdBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_TELNETD
	websWrite(wp, T("1"));
#else
	websWrite(wp, T("0"));
#endif
	return 0;
}

static int getIPTAccountBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(CONFIG_IP_NF_MATCH_ACCOUNT) || defined(CONFIG_IP_NF_MATCH_ACCOUNT_MODULE)
	websWrite(wp, T("1"));
#else
	websWrite(wp, T("0"));
#endif
	return 0;
}

parameter_fetch_t service_misc_flags[] =
{
	{ T("stpEnbl"),			"stpEnabled",		0, T("0")  },
#ifdef CONFIG_USER_CDP
	{ T("cdpEnbl"),			"cdpEnabled",		0, T("0")  },
#endif
#ifdef CONFIG_USER_CWMPD
	{ T("cwmpdEnbl"),		"cwmpdEnabled",		0, T("0")  },
	{ T("cwmpACSUrl"),		"cwmp_acs_url",		0, T("")   },
#endif
#ifdef CONFIG_USER_LLTD
	{ T("lltdEnbl"),		"lltdEnabled",		0, T("0")  },
#endif
#ifdef CONFIG_USER_LLDPD
	{ T("lldpdEnbl"),		"lldpdEnabled",		0, T("0")  },
#endif
	{ T("igmpEnbl"),		"igmpEnabled",		0, T("0")  },
	{ T("igmpSnoop"),		"igmpSnoopMode",	0, T("")   },
	{ T("igmpFastL"),		"igmpFastLeave",	0, T("")   },
	{ T("igmpM2UConv"),		"igmpM2UConvMode",	0, T("")   },
#ifdef CONFIG_USER_MINIUPNPD
	{ T("upnpEnbl"),		"upnpEnabled",		0, T("0")  },
#endif
#ifdef CONFIG_USER_XUPNPD
	{ T("xupnpdEnbl"),		"xupnpd",		0, T("0")  },
#endif
	{ T("dnspEnbl"),		"dnsPEnabled",		0, T("0")  },
	{ T("rmtHTTP"),			"RemoteManagement",	0, T("0")  },
	{ T("RemoteManagementPort"),	"RemoteManagementPort",	0, T("80") },
#ifdef CONFIG_USER_DROPBEAR
	{ T("rmtSSH"),			"RemoteSSH",		0, T("0")  },
	{ T("RemoteSSHPort"),		"RemoteSSHPort",	0, T("22") },
#endif
#ifdef CONFIG_TELNETD
	{ T("rmtTelnet"),		"RemoteTelnet", 0, T("0") },
#endif
#ifdef CONFIG_USER_UDPXY
	{ T("udpxyMode"),		"UDPXYMode",		0, T("0")  },
	{ T("udpxyPort"),		"UDPXYPort",		0, T("81") },
#endif
	{ T("watchdogEnable"),		"WatchdogEnabled",	0, T("0")  },
	{ T("pingWANEnbl"),		"WANPingFilter",	0, T("0")  },
	{ T("krnlPppoePass"),		"pppoe_pass",		0, T("0")  },
	{ T("krnlIpv6Pass"),		"ipv6_pass",		0, T("0")  },
	{ T("dhcpSwReset"),		"dhcpSwReset",		0, T("0")  },
	{ T("vlanDoubleTag"),		"vlanDoubleTag",	0, T("0")  },
	{ T("offloadMode"),		"offloadMode",		0, T("0")  },
	{ T("hw_nat_wifiPT"),		"hw_nat_wifi",		0, T("0")  },
	{ T("hw_nat_udpPT"),		"hw_nat_udp",		0, T("0")  },
	{ T("hw_nat_sixPT"),		"hw_nat_six",		0, T("0")  },
	{ T("natMode"),			"nat_mode",		0, T("1")  },
	{ T("natFastpath"),		"natFastpath",		0, T("0")  },
	{ T("routeFastpath"),		"routeFastpath",	0, T("1")  },
	{ T("filterFastpath"),		"filterFastpath",	0, T("1")  },
#ifdef CONFIG_CROND
	{ T("CrondEnable"),		"CrondEnable",		0, T("0")  },
#endif
	{ T("ForceRenewDHCP"),		"ForceRenewDHCP",	0, T("1")  },
#ifdef CONFIG_USER_PARPROUTED
	{ T("arpPT"),			"parproutedEnabled",	0, T("0")  },
#endif
	{ T("pingerEnable"),		"pinger_check_on",	0, T("0")  },
	{ T("ping_check_time"),		"ping_check_time",	0, T("0")  },
	{ T("ping_check_interval"),	"ping_check_interval",	0, T("0")  },
	{ T("ttlStore"),		"store_ttl",		0, T("0")  },
	{ T("ttlMcastStore"),		"store_ttl_mcast",	0, T("0")  },
#ifdef CONFIG_USER_SNMPD
	{ T("SnmpdEnabled"),		"snmpd",		0, T("0")  },
	{ T("snmpdcommunity"),		"snmpdcommunity",	0, T("")   },
#endif
	{ T("mssPmtu"),			"mss_use_pmtu",		0, T("1")  },
#ifdef CONFIG_RALINK_MT7621
	{ T("IRQBalance"),		"IRQBalance",		0, T("auto") },
#endif
#ifdef CONFIG_NETFILTER_FP_SMB
	{ T("smbFastpath"),		"smbFastpath",		0, T("0")  },
#endif
	{ NULL,				NULL,			0, NULL    } // Terminator
};

/* goform/setMiscServices */
static void setMiscServices(webs_t wp, char_t *path, char_t *query)
{
	char_t *goaheadrestart	= websGetVar(wp, T("goaheadrestart"), T("1"));
	char_t *reset		= websGetVar(wp, T("reset"), T("0"));

	if (CHK_IF_DIGIT(reset, 1)) {
		nvram_fromdef(RT2860_NVRAM, 47, "stpEnabled", "cdpEnabled", "cwmpdEnabled", "cwmp_acs_url", "lltdEnabled",
						"lldpdEnabled", "igmpEnabled", "igmpSnoopMode", "igmpFastLeave", "igmpM2UConvMode",
						"upnpEnabled", "xupnpd", "dnsPEnabled", "RemoteManagement", "RemoteManagementPort",
						"RemoteSSH", "RemoteSSHPort", "RemoteTelnet", "UDPXYMode", "UDPXYPort",
						"WatchdogEnabled", "WANPingFilter", "pppoe_pass", "ipv6_pass", "dhcpSwReset",
						"vlanDoubleTag", "offloadMode", "hw_nat_wifi", "hw_nat_udp", "hw_nat_six",
						"nat_mode", "natFastpath", "routeFastpath", "filterFastpath", "CrondEnable",
						"ForceRenewDHCP", "parproutedEnabled", "pinger_check_on", "ping_check_time", "ping_check_interval",
						"store_ttl", "store_ttl_mcast", "snmpd", "snmpdcommunity", "mss_use_pmtu",
						"IRQBalance", "smbFastpath");
	}
	else {
		nvram_init(RT2860_NVRAM);
		setupParameters(wp, service_misc_flags, 0);

		int nat_fp = nvram_get_int(RT2860_NVRAM, "offloadMode", -1);
		if (nat_fp == 2 || nat_fp == 3)
		{
			char_t *nat_th = websGetVar(wp, "hwnatThreshold", "50");
			if (nat_th != NULL)
				nvram_bufset(RT2860_NVRAM, "hw_nat_bind", nat_th);
		}

		int dns_proxy = nvram_get_int(RT2860_NVRAM, "dnsPEnabled", 0);
		if (dns_proxy == 1)
		{
			nvram_bufset(RT2860_NVRAM, "dhcpPriDns", "");
			nvram_bufset(RT2860_NVRAM, "dhcpSecDns", "");
		}

		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}

	/* restart some services instead full reload */
	doSystem("services_restart.sh misc &");
	if (CHK_IF_DIGIT(goaheadrestart, 1)) {
		/* Output timer for reloading goahead, only for true redirect need, time set to really small */
		outputTimerForReload(wp, "", 15000);
		/* Correct stop goahead and wait restart by webmon */
		exit(0);
	} else {
		websHeader(wp);
		websDone(wp, 200);
	}
}

//------------------------------------------------------------------------------
// Samba/Wins setup
parameter_fetch_t service_samba_flags[] =
{
	{ T("WorkGroup"), "WorkGroup", 0, T("") },
	{ T("SmbNetBIOS"), "SmbNetBIOS", 0, T("") },
	{ T("SmbString"), "SmbString", 0, T("") },
	{ T("SmbOsLevel"), "SmbOsLevel", 0, T("") },
	{ T("SmbTimeserver"), "SmbTimeserver", 0, T("0") },
	{ NULL, NULL, 0, NULL } // Terminator
};

static void setSamba(webs_t wp, char_t *path, char_t *query)
{
	char_t *smb_enabled = websGetVar(wp, T("SmbEnabled"), T("0"));
	char_t *submitUrl;

	if (smb_enabled == NULL)
		smb_enabled = "0";

	nvram_init(RT2860_NVRAM);
	nvram_bufset(RT2860_NVRAM, "SmbEnabled", smb_enabled);

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
void formIptAccounting(webs_t wp, char_t *path, char_t *query)
{
	char_t *strValue;
	char_t *submitUrl;
	int reset_ipt = 0;

	strValue = websGetVar(wp, T("iptEnable"), T("0"));	//reset stats
	if ((strValue != NULL) && (strcmp(strValue, "0")==0))
		reset_ipt = 1;

	nvram_set(RT2860_NVRAM, "ipt_account", strValue);

	strValue = websGetVar(wp, T("reset"), T("0"));	//reset stats
	if ((strValue != NULL) && (strcmp(strValue, "1")))
		reset_ipt = 1;

	// Reset IPT
	if (reset_ipt)
	{
		FILE *fd = fopen(_PATH_IPT_ACCOUNTING_FILE, "w");
		if (fd != NULL)
		{
			fputs("reset", fd);
			fclose(fd);
		}
	}

	firewall_rebuild();

	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
}

#ifndef IPT_SHORT_ACCOUNT
const char *iptProtocolNames[]=
{
	"All", "TCP", "UDP", "ICMP", "Others"
};
#endif /* !IPT_SHORT_ACCOUNT */

// Output IP Accounting statistics
int iptStatList(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fd;
	char_t ip[32], line[256];
	long long b_src[5], p_src[5], b_dst[5], p_dst[5], time;
	int lines = 0;

	// Do not show anything if nat_fastpath is set
	int nat_fp = nvram_get_int(RT2860_NVRAM, "offloadMode", -1);
	if (nat_fp > 0)
	{
		websWrite(wp, T("<tr><td>No IPT accounting allowed</td></tr>\n"));
		return 0;
	}

	websWrite(wp, T("<tr><td class=\"title\" colspan=\"%d\">IP Accounting table</td></tr>\n"),
#ifdef IPT_SHORT_ACCOUNT
				6
#else
				7
#endif
	);

	if ((fd = fopen(_PATH_IPT_ACCOUNTING_FILE, "r"))!=NULL)
	{
		// Output table header
#ifdef IPT_SHORT_ACCOUNT
		websWrite(wp, T(
			"<tr>\n"
			"<th width=\"30%%\" align=\"center\">IP addr</th>\n"
			"<th width=\"15%%\" align=\"center\">Tx bts</th>\n"
			"<th width=\"15%%\" align=\"center\">Tx pkt</th>\n"
			"<th width=\"15%%\" align=\"center\">Rx bts</th>\n"
			"<th width=\"15%%\" align=\"center\">Rx pkt</th>\n"
			"<th width=\"10%%\" align=\"center\">Time</th>\n"
			"</tr>\n"));
#else
		websWrite(wp, T(
			"<tr>\n"
			"<th width=\"30%%\" align=\"center\">IP addr</th>\n"
			"<th width=\"10%%\" align=\"center\">Proto</th>\n"
			"<th width=\"13%%\" align=\"center\">Tx bts</th>\n"
			"<th width=\"13%%\" align=\"center\">Tx pkt</th>\n"
			"<th width=\"13%%\" align=\"center\">Rx bts</th>\n"
			"<th width=\"13%%\" align=\"center\">Rx pkt</th>\n"
			"<th width=\"8%%\" align=\"center\">Time</th>\n"
			"</tr>\n"));

#endif /* IPT_SHORT_ACCOUNT */

		while ((fgets(line, sizeof(line), fd)) != NULL)
		{
			lines++;
#ifdef IPT_SHORT_ACCOUNT
			sscanf(line,
				"%*s %*s %s "   // IP
				"%*s %*s %lld "  // bytes_src
				"%*s %*s %lld "  // packets_src
				"%*s %*s %lld "  // bytes_dst
				"%*s %*s %lld "  // packets_dst
				"%*s %*s %lld ", // time
				ip, &b_src[0], &p_src[0], &b_dst[0], &p_dst[0], &time);

			websWrite(wp, T(
				"<tr class=\"grey\">\n"
				"<td width=\"30%%\" align=\"center\">%s</td>\n"),
				ip);

			const char *src_sz = normalizeSize(&b_src[0]);
			const char *dst_sz = normalizeSize(&b_dst[0]);

			websWrite(wp, T(
				"<td width=\"15%%\" align=\"center\">%ld %s</td>\n"
				"<td width=\"15%%\" align=\"center\">%ld</td>\n"
				"<td width=\"15%%\" align=\"center\">%ld %s</td>\n"
				"<td width=\"15%%\" align=\"center\">%ld</td>\n"
				),
				(long)b_src[0], src_sz, (long)p_src[0], (long)b_dst[0], dst_sz, (long)p_dst[0]
				);

			websWrite(wp, T(
				"<td width=\"10%%\" align=\"center\">%ld</td>\n"
				"</tr>\n"),
				(long)time);
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

			websWrite(wp, T(
				"<tr class=\"grey\">\n"
				"<td width=\"30%%\" align=\"center\">%s</td>\n"),
				ip);

			int i;
			for (i=0; i<5; i++)
			{
				if (i>0)
					websWrite(wp, T("<tr class=\"grey\">\n"));

				const char *src_sz = normalizeSize(&b_src[i]);
				const char *dst_sz = normalizeSize(&b_dst[i]);
				websWrite(wp, T(
					"<td width=\"10%%\" align=\"center\">%s</font></td>\n"
					"<td width=\"13%%\" align=\"center\">%lld %s</td>\n"
					"<td width=\"13%%\" align=\"center\">%lld</td>\n"
					"<td width=\"13%%\" align=\"center\">%lld %s</td>\n"
					"<td width=\"13%%\" align=\"center\">%lld</td>\n"),
					iptProtocolNames[i], b_src[i], src_sz, p_src[i], b_dst[i], dst_sz, p_dst[i]);

				if (i==0)
				{
					websWrite(wp, T(
						"<td width=\"10%%\" align=\"center\">%lld</td>\n"),
						time);
				}
				websWrite(wp, T("</tr>\n"));
			}
#endif /* IPT_SHORT_ACCOUNT */
		}

		fclose(fd);
	}

	if (lines<=0)
		websWrite(wp, T("<tr><td align=\"left\" colspan=\"%d\">No statistics available now</td></tr>\n"),
#ifdef IPT_SHORT_ACCOUNT
				6
#else
				7
#endif
			);
	return 0;
}

parameter_fetch_t service_l2tp_flags[] =
{
	{ T("l2tp_srv_ip_local"),	"l2tp_srv_ip_local",		0, T("")     },
	{ T("l2tp_srv_ip_range"),	"l2tp_srv_ip_range",		0, T("")     },
	{ T("l2tp_srv_lcp_adapt"),	"l2tp_srv_lcp_adapt",		2, T("off")  },
	{ T("l2tp_srv_debug"),		"l2tp_srv_debug",		2, T("off")  },
	{ T("l2tp_srv_nat_enabled"),	"l2tp_srv_nat_enabled",		2, T("off")  },
	{ T("l2tp_srv_mppe_enabled"),	"l2tp_srv_mppe_enabled",	2, T("off")  },
	{ T("l2tp_srv_proxyarp"),	"l2tp_srv_proxyarp",		2, T("off")  },
	{ T("l2tp_srv_mtu_size"),	"l2tp_srv_mtu_size",		0, T("1460") },
	{ T("l2tp_srv_mru_size"),	"l2tp_srv_mru_size",		0, T("1460") },
	{ T("l2tp_srv_user_num"),	"l2tp_srv_user_num",		0, T("0")    },
	{ NULL,				NULL,				0, NULL      } // Terminator
};

static void l2tpConfig(webs_t wp, char_t *path, char_t *query)
{
	char user_var[16];
	char pass_var[16];
	int i, count;
	char_t *reset = websGetVar(wp, T("reset"), T("0"));

	if (CHK_IF_DIGIT(reset, 1)) {
		count = nvram_get_int(RT2860_NVRAM, "l2tp_srv_user_num", 0);
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
			int oldcount = nvram_get_int(RT2860_NVRAM, "l2tp_srv_user_num", 0);
			nvram_bufset(RT2860_NVRAM, "l2tp_srv_enabled", "1");
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
			nvram_bufset(RT2860_NVRAM, "l2tp_srv_enabled", "0");

		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}

	firewall_rebuild();
	doSystem("service vpnserver restart");

	websHeader(wp);
	websDone(wp, 200);
}

static int getL2TPUserList(int eid, webs_t wp, int argc, char_t **argv)
{
	char user_var[16];
	char pass_var[16];
	int count = nvram_get_int(RT2860_NVRAM, "l2tp_srv_user_num", 0);
	int i;

	if (count > 0) {
		for (i = 0; i < count; i++) {
			sprintf(user_var, "l2tp_srv_user%d", i);
			sprintf(pass_var, "l2tp_srv_pass%d", i);
			char *user = nvram_get(RT2860_NVRAM, user_var);
			char *pass = nvram_get(RT2860_NVRAM, pass_var);
			websWrite(wp, T("[ '%s', '%s' ]%s"), user, pass, (i + 1 < count) ? ", " : " ");
		}
	}
	return 0;
}

static void radiusConfig(webs_t wp, char_t *path, char_t *query)
{
	char user_var[20];
	char pass_var[20];
	int i, count;
	char_t *reset = websGetVar(wp, T("reset"), T("0"));

	if (CHK_IF_DIGIT(reset, 1)) {
		count = nvram_get_int(RT2860_NVRAM, "radius_srv_user_num", 0);
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
	}
	else {
		char_t *radius_enabled = websGetVar(wp, T("radius_srv_enabled"), T("0"));

		if (radius_enabled == NULL)
			radius_enabled = "0";

		nvram_init(RT2860_NVRAM);

		if (CHK_IF_DIGIT(radius_enabled, 1)) {
			int oldcount = nvram_get_int(RT2860_NVRAM, "radius_srv_user_num", 0);
			char_t *radius_user_num = websGetVar(wp, T("radius_srv_user_num"), T("0"));
			char_t *radius_secret = websGetVar(wp, T("radius_srv_secret"), T(""));

			nvram_bufset(RT2860_NVRAM, "radius_srv_enabled", "1");
			nvram_bufset(RT2860_NVRAM, "radius_srv_user_num", radius_user_num);
			nvram_bufset(RT2860_NVRAM, "radius_srv_secret", radius_secret);

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
			nvram_bufset(RT2860_NVRAM, "radius_srv_enabled", "0");

		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}

	firewall_rebuild();
	doSystem("service radius reload");

	websHeader(wp);
	websDone(wp, 200);
}


static int getRadiusUserList(int eid, webs_t wp, int argc, char_t **argv)
{
	char user_var[20];
	char pass_var[20];
	int count = nvram_get_int(RT2860_NVRAM, "radius_srv_user_num", 0);
	int i;

	if (count > 0) {
		for (i = 0; i < count; i++) {
			sprintf(user_var, "radius_srv_user%d", i);
			sprintf(pass_var, "radius_srv_pass%d", i);
			char *user = nvram_get(RT2860_NVRAM, user_var);
			char *pass = nvram_get(RT2860_NVRAM, pass_var);
			websWrite(wp, T("[ '%s', '%s' ]%s"), user, pass, (i + 1 < count) ? ", " : " ");
		}
	}
	return 0;
}


static int isSMP(int eid, webs_t wp, int argc, char_t **argv) {
#ifdef CONFIG_RALINK_MT7621
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getSmbFPBuilt(int eid, webs_t wp, int argc, char_t **argv) {
#ifdef CONFIG_NETFILTER_FP_SMB
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

void formDefineServices(void)
{
	// Define forms
	websFormDefine(T("setDhcp"), setDhcp);
	websFormDefine(T("formSamba"), setSamba);
	websFormDefine(T("setMiscServices"), setMiscServices);
	websFormDefine(T("formIptAccounting"), formIptAccounting);
	websFormDefine(T("l2tpConfig"), l2tpConfig);
	websFormDefine(T("radiusConfig"), radiusConfig);

	// Define functions
	websAspDefine(T("getL2TPUserList"), getL2TPUserList);
	websAspDefine(T("getRadiusUserList"), getRadiusUserList);
	websAspDefine(T("getDhcpCliList"), getDhcpCliList);
	websAspDefine(T("iptStatList"), iptStatList);
	websAspDefine(T("getARPptBuilt"), getARPptBuilt);
	websAspDefine(T("getFastPathBuilt"), getFastPathBuilt);
	websAspDefine(T("getTelnetdBuilt"), getTelnetdBuilt);
	websAspDefine(T("getSNMPDBuilt"), getSNMPDBuilt);
	websAspDefine(T("getIPTAccountBuilt"), getIPTAccountBuilt);
	websAspDefine(T("getProcessList"), getProcessList);
	websAspDefine(T("isSMP"), isSMP);
	websAspDefine(T("getSmbFPBuilt"), getSmbFPBuilt);
}
