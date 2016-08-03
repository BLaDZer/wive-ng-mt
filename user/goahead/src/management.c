#ifdef USER_MANAGEMENT_SUPPORT
#include "um.h"
#endif
#include "helpers.h"
#include "internet.h"
#include "management.h"

/*
 * goform/setSysAdm
 */
static void setSysAdm(webs_t wp, char_t *path, char_t *query)
{
	char_t *admuser, *admpass;
	char *old_user;
	char_t *submitUrl;

	old_user = nvram_get(RT2860_NVRAM, "Login");
	admuser = websGetVar(wp, T("admuser"), T(""));
	admpass = websGetVar(wp, T("admpass"), T(""));

	if (!strlen(admuser)) {
		syslog(LOG_WARNING, "setSysAdm: account empty, leave it unchanged");
		return;
	}
	if (!strlen(admpass)) {
		syslog(LOG_WARNING, "setSysAdm: password empty, leave it unchanged");
		return;
	}

	nvram_init(RT2860_NVRAM);
	nvram_bufset(RT2860_NVRAM, "Login", admuser);
	nvram_bufset(RT2860_NVRAM, "Password", admpass);
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	/* modify /etc/passwd to new user name and passwd */
	doSystem("sed -e 's/^%s:/%s:/' /etc/passwd > /etc/newpw", old_user, admuser);
	doSystem("cp /etc/newpw /etc/passwd");
	doSystem("rm -f /etc/newpw");
	doSystem("service pass start");
	doSystem("service inetd restart");
#if defined(CONFIG_USER_SAMBA)
	doSystem("service samba restart");
#endif

#ifdef USER_MANAGEMENT_SUPPORT
	if (umGroupExists(T("adm")) == FALSE)
		umAddGroup(T("adm"), 0x07, AM_DIGEST, FALSE, FALSE);
	if (old_user != NULL && umUserExists(old_user))
		umDeleteUser(old_user);
	if (umUserExists(admuser))
		umDeleteUser(admuser);
	umAddUser(admuser, admpass, T("adm"), FALSE, FALSE);
#endif

#ifdef PRINT_DEBUG
	websHeader(wp);
	websWrite(wp, T("<h2>Adminstrator Settings</h2><br>\n"));
	websWrite(wp, T("adm user: %s<br>\n"), admuser);
	websWrite(wp, T("adm pass: %s<br>\n"), admpass);
	websFooter(wp);
	websDone(wp, 200);
#else
	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
#endif
}

/*
 * goform/setSysLang
 */
static void setSysLang(webs_t wp, char_t *path, char_t *query)
{
	char_t *lang;
	char_t *submitUrl;

	lang = websGetVar(wp, T("langSelection"), T(""));
	nvram_set(RT2860_NVRAM, "Language", lang);

#ifdef PRINT_DEBUG
	websHeader(wp);
	websWrite(wp, T("<h2>Language Selection</h2><br>\n"));
	websWrite(wp, T("language: %s<br>\n"), lang);
	websFooter(wp);
	websDone(wp, 200);
#else
	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
#endif
}

/*
 * goform/NTP
 */
static void NTP(webs_t wp, char_t *path, char_t *query)
{
	char_t *submitUrl;

	char_t *ntpEnabled = websGetVar(wp, T("ntp_enabled"), T("0"));

	nvram_init(RT2860_NVRAM);
	nvram_bufset(RT2860_NVRAM, "NTPEnabled", ntpEnabled);
	if (CHK_IF_DIGIT(ntpEnabled, 1))
	{
		char_t *tz = websGetVar(wp, T("time_zone"), T(""));
		char_t *ntpServer = websGetVar(wp, T("NTPServerIP"), T(""));

		if ((strlen(tz)>0) && (!checkSemicolon(tz)))
		{
			nvram_bufset(RT2860_NVRAM, "NTPServerIP", ntpServer);
			nvram_bufset(RT2860_NVRAM, "TZ", tz);
		}
	}

	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	if (CHK_IF_DIGIT(ntpEnabled, 1))
		doSystem("service ntp restart");
	else
		doSystem("service ntp stop");

#ifdef PRINT_DEBUG
	websHeader(wp);
	websWrite(wp, T("<h2>NTP Settings</h2><br>\n"));
	websWrite(wp, T("NTPEnabled: %s<br>\n"), ntpEnabled);
	websWrite(wp, T("NTPserver: %s<br>\n"), ntpServer);
	websWrite(wp, T("TZ: %s<br>\n"), tz);
	websFooter(wp);
	websDone(wp, 200);
#else
	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
#endif
}

#ifdef CONFIG_DATE
/*
 * goform/NTPSyncWithHost
 */
static void NTPSyncWithHost(webs_t wp, char_t *path, char_t *query)
{
	if(!query || (!strlen(query)))
		return;
	if(strchr(query, ';'))
		return;

	doSystem("date -s %s", query);

	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\n"));
	websWrite(wp, WEBS_CACHE_CONTROL_STRING);
	websWrite(wp, T("\n"));
	websWrite(wp, T("n/a"));
	websDone(wp, 200);
}
#endif


#ifdef CONFIG_USER_INADYN
/*
 * goform/DDNS
 */
static void DDNS(webs_t wp, char_t *path, char_t *query)
{
	char *ddns_provider, *ddns, *ddns_acc, *ddns_pass;
	char empty_char = '\0';
	char_t *submitUrl;

	ddns_provider = websGetVar(wp, T("DDNSProvider"), T("none"));
	ddns = websGetVar(wp, T("DDNS"), T(""));
	ddns_acc = websGetVar(wp, T("Account"), T(""));
	ddns_pass = websGetVar(wp, T("Password"), T(""));

	if(!strcmp(T("none"), ddns_provider))
		ddns = ddns_acc = ddns_pass = &empty_char;
	else if(!strcmp(T("freedns.afraid.org"), ddns_provider)) {
		ddns_acc = &empty_char;
		if((!strlen(ddns) || !strlen(ddns_pass)) || (checkSemicolon(ddns) || checkSemicolon(ddns_pass)))
			goto invalid_values;
	} else if((!strlen(ddns) || !strlen(ddns_acc) || !strlen(ddns_pass)) ||
		(checkSemicolon(ddns) || checkSemicolon(ddns_acc) || checkSemicolon(ddns_pass)))
			goto invalid_values;

	nvram_init(RT2860_NVRAM);
	nvram_bufset(RT2860_NVRAM, "DDNSProvider", ddns_provider);
	nvram_bufset(RT2860_NVRAM, "DDNS", ddns);
	nvram_bufset(RT2860_NVRAM, "DDNSAccount", ddns_acc);
	nvram_bufset(RT2860_NVRAM, "DDNSPassword", ddns_pass);
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	doSystem("service ddns restart");

invalid_values:
#ifdef PRINT_DEBUG
	websHeader(wp);
	websWrite(wp, T("<h2>DDNS Settings</h2><br>\n"));
	websWrite(wp, T("DDNSProvider: %s<br>\n"), ddns_provider);
	websWrite(wp, T("DDNS: %s<br>\n"), ddns);
	websWrite(wp, T("DDNSAccount: %s<br>\n"), ddns_acc);
	websWrite(wp, T("DDNSPassword: %s<br>\n"), ddns_pass);
	websFooter(wp);
	websDone(wp, 200);
#else
	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
#endif
}
#endif

static int getPortStatus(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(CONFIG_ETHTOOL)
#if defined(CONFIG_RAETH_ESW) || defined(CONFIG_MT7530_GSW)
	int port;

	for (port=4; port>-1; port--)
	{
		struct port_status pst;
		char buf[16];

		portstatus(&pst, port);

		/* create string in new buffer and write to web (this more safe of direct write) */
		snprintf(buf, sizeof(buf), ("%s%d,%d,%s"), (pst.portnum == 4) ? "" : ";", pst.link, pst.speed, (pst.duplex == 1) ? "F" : "H");
		websWrite(wp, T("%s"), buf);
	}
#endif
#endif
	return 0;
}
static int getAllNICStatisticASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char buf[512];
	FILE *fp = fopen(_PATH_PROCNET_DEV, "r");

	if (fp == NULL)
	{
		syslog(LOG_ERR, "no proc, %s", __FUNCTION__);
		return -1;
	}

	// skip headers
	fgets(buf, sizeof(buf), fp);
	fgets(buf, sizeof(buf), fp);

	while (fgets(buf, sizeof(buf), fp) != NULL)
	{
		char *semiColon;
		char ifname[IFNAMSIZ];
		unsigned long long rx_bytes = 0, rx_packets = 0, rx_errs = 0, rx_drops = 0, rx_fifo = 0, rx_frame = 0,
			tx_bytes = 0, tx_packets = 0, tx_errs = 0, tx_drops = 0, tx_fifo = 0, tx_colls = 0, tx_carrier = 0, rx_multi = 0;

		// find : , extract ifname, move pointer to next block semicolon
		semiColon = strchr(buf, ':');
		if (semiColon == NULL || (*semiColon++ = 0, sscanf(buf, "%s", ifname) != 1)) {
			syslog(LOG_ERR, "wrong format string in /proc/net/dev, %s", __FUNCTION__);
			continue;
		}

		// not correct parse
		if(ifname[0]=='\0')
			continue;

		// long ifname
		if(strlen(ifname) > IFNAMSIZ)
			continue;

		// filter 'lo' interface
		if (strcmp(ifname, "lo") == 0)
			continue;

		// check that interface is up
		if (getIfIsReady(ifname) != 1)
			continue;

		// extract scale and print statistic
		if (sscanf(semiColon, "%llu%llu%llu%llu%llu%llu%llu%*d%llu%llu%llu%llu%llu%llu%llu",
			   &rx_bytes, &rx_packets, &rx_errs, &rx_drops, &rx_fifo, &rx_frame, &rx_multi,
			    &tx_bytes, &tx_packets, &tx_errs, &tx_drops, &tx_fifo, &tx_colls, &tx_carrier) != 14) {
			// not extracted - print n/a
			websWrite(wp, T("<tr><td class=\"head\" colspan=\"2\">n/a</td><td>n/a</td><td>n/a</td><td>n/a</td><td>n/a</td></tr>\n"));
			continue;
		} else {
			char strbuf[512];
			char *rx_tmpstr, *tx_tmpstr;

			// scale bytes to K/M/G/Tb
			rx_tmpstr = scale((uint64_t)rx_bytes);
			tx_tmpstr = scale((uint64_t)tx_bytes);

			if (rx_tmpstr && tx_tmpstr) {
			    // format string in buffer
			    snprintf(strbuf, sizeof(strbuf),
					"<tr><td class=\"head\" colspan=\"2\">%s</td><td>%llu</td><td>%s</td><td>%llu</td><td>%s</td></tr>\n",
											    ifname, rx_packets, rx_tmpstr, tx_packets, tx_tmpstr);
			    bfree(B_L, rx_tmpstr);
			    bfree(B_L, tx_tmpstr);

			    // write to web
			    websWrite(wp, T("%s"), strbuf);
			} else {
			    websWrite(wp, T("<td>n/a</td><td>n/a</td><td>n/a</td><td>n/a</td></tr>\n"));
			}
		}
	}
	fclose(fp);

	return 0;
}

static int getMemTotalASP(int eid, webs_t wp, int argc, char_t **argv)
{
	struct mem_stats mem;
	char buf[16];

	get_memdata(&mem);

	snprintf(buf, sizeof(buf), "%lu", mem.total);
	return websWrite(wp, T("%s"), buf);
}

static int getMemLeftASP(int eid, webs_t wp, int argc, char_t **argv)
{
	struct mem_stats mem;
	char buf[16];

	get_memdata(&mem);

	snprintf(buf, sizeof(buf), "%lu", mem.free);
	return websWrite(wp, T("%s"), buf);
}

static unsigned long long prevbusy, prevtotal;
static int getCpuUsageASP(int eid, webs_t wp, int argc, char_t **argv)
{
	struct cpu_stats cpu;
	char buf[16];
	float outd = 0;

	getcpudata(&cpu);

	if (cpu.total != prevtotal)
	    outd=((((float)cpu.busy-(float)prevbusy)/((float)cpu.total-(float)prevtotal))*100);

	prevbusy=cpu.busy;
	prevtotal=cpu.total;

	snprintf(buf, sizeof(buf), "%.1f", outd);
	return websWrite(wp, T("%s %%"), buf);
}

static void LoadDefaultSettings(webs_t wp, char_t *path, char_t *query)
{
	/* Output timer for reloading */
	outputTimerForReload(wp, "", 80000);

	/* restore defaults and rwfs drop */
	doSystem("fs nvramreset > /dev/null 2>&1");
	doSystem("fs restore > /dev/null 2>&1");
	reboot_now();
}

#ifdef CONFIG_SYSLOGD
parameter_fetch_t service_syslog_flags[] =
{
	{ T("KLogd"), "KLogd", 0, T("0") },
	{ T("SysLogd"), "SysLogd", 0, T("0") },
	{ T("RemoteSysLogIP"), "RemoteSysLogIP", 0, T("") },
	{ NULL, NULL, 0, NULL } // Terminator
};

static void clearlog(webs_t wp, char_t *path, char_t *query)
{
	doSystem("service syslog stop");
	truncate("/var/log/messages", 0);
	doSystem("service syslog start");

	websDone(wp, 200);
}

static void setuplog(webs_t wp, char_t *path, char_t *query)
{
	char_t *submitUrl;

	// Set-up parameters
	setupParameters(wp, service_syslog_flags, 1);

	// Restart syslog
	doSystem("service syslog restart");

	// Redirect if possible
	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
}

#define LOG_MAX 32768
static void getsyslog(webs_t wp, char_t *path, char_t *query)
{
	FILE *fp = NULL;
	char *log;

	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\n"));
	websWrite(wp, WEBS_CACHE_CONTROL_STRING);
	websWrite(wp, T("\n"));

	// LOG_MAX 32768 - 1
	fp = popen("tail -c 32767 /var/log/messages", "r");
	if(!fp){
		syslog(LOG_ERR, "no log exist, %s", __FUNCTION__);
		goto error;
	}

	log = balloc(B_L, LOG_MAX * sizeof(char));
	if(!log){
		syslog(LOG_ERR, "no memory left, %s", __FUNCTION__);
		goto error;
	}

	memset(log, 0, LOG_MAX);
	fread(log, 1, LOG_MAX, fp);
	websLongWrite(wp, log);

	bfree(B_L, log);
error:
	if(fp)
	    pclose(fp);
	websDone(wp, 200);
}
#endif

static int getHWStatsBuilt(int eid, webs_t wp, int argc, char_t **argv) {
#ifdef CONFIG_RAETH_SNMPD
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getHWStatistic(int eid, webs_t wp, int argc, char_t **argv) {
	int i;
	struct port_counts pcs;

	portscounts(&pcs);

	websWrite(wp, T("<tr>\n<td class=\"head\" id=\"stats_rx\">Rx</td>\n"));
#ifdef CONFIG_RTESW_SWITCH_ONEPORT
	for (i = 4; i >= 4; i--)
#else
	for (i = 4; i >= 0; i--)
#endif
	{
		char *tmpstr = scale((uint64_t)pcs.rx_count[i]);
		websWrite(wp, T("<td>%s</td>\n"), tmpstr);
		bfree(B_L, tmpstr);
	}
	websWrite(wp, T("</tr>\n<tr>\n<td class=\"head\" id=\"stats_tx\">Tx</td>\n"));
#ifdef CONFIG_RTESW_SWITCH_ONEPORT
	for (i = 4; i >= 4; i--)
#else
	for (i = 4; i >= 0; i--)
#endif
	{
		char *tmpstr = scale((uint64_t)pcs.tx_count[i]);
		websWrite(wp, T("<td>%s</td>\n"), tmpstr);
		bfree(B_L, tmpstr);
	}
	websWrite(wp, T("</tr>\n"));
	return 0;
}

void formDefineManagement(void)
{
	websAspDefine(T("getMemLeftASP"), getMemLeftASP);
	websAspDefine(T("getMemTotalASP"), getMemTotalASP);
	websAspDefine(T("getCpuUsageASP"), getCpuUsageASP);
	websAspDefine(T("getAllNICStatisticASP"), getAllNICStatisticASP);
	websAspDefine(T("getHWStatsBuilt"), getHWStatsBuilt);
	websAspDefine(T("getHWStatistic"), getHWStatistic);
	websAspDefine(T("getPortStatus"), getPortStatus);
#ifdef CONFIG_DATE
	websFormDefine(T("NTPSyncWithHost"), NTPSyncWithHost);
#endif
#ifdef CONFIG_USER_INADYN
	websFormDefine(T("DDNS"), DDNS);
#endif
#ifdef CONFIG_SYSLOGD
	websFormDefine(T("getsyslog"), getsyslog);
	websFormDefine(T("clearlog"), clearlog);
	websFormDefine(T("setuplog"), setuplog);
#endif
	websFormDefine(T("setSysAdm"), setSysAdm);
	websFormDefine(T("setSysLang"), setSysLang);
	websFormDefine(T("NTP"), NTP);
	websFormDefine(T("LoadDefaultSettings"), LoadDefaultSettings);
}
