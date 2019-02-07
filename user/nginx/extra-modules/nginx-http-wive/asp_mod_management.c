#include "asp_mod_management.h"

/*
 * goform/setSysAdm
 */
static void setSysAdm(webs_t* wp, char_t *path, char_t *query)
{
        char_t *admuser, *admpass, *orduser, *ordpass, *mgmtuser, *mgmtpass;
        char *old_admin, *old_user, *old_mgmt;
        char_t *submitUrl;

        if (wp->auth_session == NULL) return;
        old_admin = nvram_get(RT2860_NVRAM, "Login");
        admuser = websGetVar(wp, T("admuser"), NULL);
        admpass = websGetVar(wp, T("admpass"), NULL);

        old_user = nvram_get(RT2860_NVRAM, "UserLogin");
        orduser = websGetVar(wp, T("orduser"), NULL);
        ordpass = websGetVar(wp, T("ordpass"), NULL);

        old_mgmt = nvram_get(RT2860_NVRAM, "MngmtLogin");
        mgmtuser = websGetVar(wp, T("mgmtuser"), NULL);
        mgmtpass = websGetVar(wp, T("mgmtpass"), NULL);


        if (!admuser) {
            admuser = old_admin;
            syslog(LOG_WARNING, "setSysAdm: admin account not provided, leave unchanged");
        }

        if (!admpass) {
            admpass = nvram_get(RT2860_NVRAM, "Password");
            syslog(LOG_WARNING, "setSysAdm: admin password not provided, leave unchanged");
        }

        if (!orduser) {
            orduser = old_user;
            syslog(LOG_WARNING, "setSysAdm: user account not provided, leave unchanged");
        }

        if (!ordpass) {
            ordpass = nvram_get(RT2860_NVRAM, "UserPassword");
            syslog(LOG_WARNING, "setSysAdm: user password not provided, leave unchanged");
        }

        if (!mgmtuser) {
            mgmtuser = old_mgmt;
            syslog(LOG_WARNING, "setSysAdm: management account not provided, leave unchanged");
        }

        if (!mgmtpass) {
            mgmtpass = nvram_get(RT2860_NVRAM, "MngmtPassword");
            syslog(LOG_WARNING, "setSysAdm: management password not provided, leave unchanged");
        }

        if (admuser[0] == '\0')
        {
            syslog(LOG_WARNING, "setSysAdm: admin login cannot be empty! Abort.");
            return;
        }

        if (strcmp(admuser, orduser) == 0)
        {
            syslog(LOG_WARNING, "setSysAdm: user login equals admin login! Abort.");
            return;
        }

        if (strcmp(mgmtuser, orduser) == 0 && mgmtuser[0] != '\0')
        {
            syslog(LOG_WARNING, "setSysAdm: user login equals management login! Abort.");
            return;
        }

        if (strcmp(mgmtuser, admuser) == 0)
        {
            syslog(LOG_WARNING, "setSysAdm: admin login equals management login! Abort.");
            return;
        }


	nvram_init(RT2860_NVRAM);
	ngx_nvram_bufset(wp, "Login", admuser);
	ngx_nvram_bufset(wp, "Password", admpass);
	ngx_nvram_bufset(wp, "UserLogin", orduser);
	ngx_nvram_bufset(wp, "UserPassword", ordpass);
	ngx_nvram_bufset(wp, "MngmtLogin", mgmtuser);
	ngx_nvram_bufset(wp, "MngmtPassword", mgmtpass);
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

        if (wp->auth_session->role == ADMIN)
        {
            closeSessionsByUser(old_admin);
            closeSessionsByUser(admuser);
            closeSessionsByUser(old_mgmt);
            closeSessionsByUser(mgmtuser);
        }

        closeSessionsByUser(old_user);
        closeSessionsByUser(orduser);

	/* modify /etc/passwd to new user name and passwd */
	doSystem("sed -e 's/^%s:/%s:/;s/^%s:/%s:/;s/^%s:/%s:/' /etc/passwd > /etc/newpw", old_admin, admuser, old_user, orduser, old_mgmt, mgmtuser);

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
static void setSysLang(webs_t* wp, char_t *path, char_t *query)
{
	char_t *lang;
	char_t *submitUrl;

	lang = websGetVar(wp, T("langSelection"), T(""));
	ngx_nvram_set(wp, "Language", lang);

#ifdef PRINT_DEBUG
//	websHeader(wp);
	outWrite(T("<h2>Language Selection</h2><br>\n"));
	outWrite(T("language: %s<br>\n"), lang);
//	websFooter(wp);
	websDone(wp, 200);
#else
	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
#endif
}

/*
 * goform/NTP
 */
static void NTP(webs_t* wp, char_t *path, char_t *query)
{
	char_t *submitUrl;

	char_t *ntpEnabled = websGetVar(wp, T("ntp_enabled"), T("0"));

	nvram_init(RT2860_NVRAM);
	ngx_nvram_bufset(wp, "NTPEnabled", ntpEnabled);
	if (CHK_IF_DIGIT(ntpEnabled, 1))
	{
		char_t *tz = websGetVar(wp, T("time_zone"), T(""));
		char_t *ntpServer = websGetVar(wp, T("NTPServerIP"), T(""));

		if ((strlen(tz)>0) && (!checkSemicolon(tz)))
		{
			ngx_nvram_bufset(wp, "NTPServerIP", ntpServer);
			ngx_nvram_bufset(wp, "TZ", tz);
		}
	}

	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	if (CHK_IF_DIGIT(ntpEnabled, 1))
		doSystem("service ntp restart");
	else
		doSystem("service ntp stop");

#ifdef PRINT_DEBUG
//	websHeader(wp);
	outWrite(T("<h2>NTP Settings</h2><br>\n"));
	outWrite(T("NTPEnabled: %s<br>\n"), ntpEnabled);
	outWrite(T("NTPserver: %s<br>\n"), ntpServer);
	outWrite(T("TZ: %s<br>\n"), tz);
//	websFooter(wp);
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
static void NTPSyncWithHost(webs_t* wp, char_t *path, char_t *query)
{
	if(!query || (!strlen(query)))
		return;
	if(strchr(query, ';'))
		return;

	doSystem("date -s %s", query);

        websSetContentType(wp, "text/plain");

//	outWrite(WEBS_CACHE_CONTROL_STRING);
//	outWrite(T("\n"));
	outWrite(T("n/a"));
	websDone(wp, 200);
}
#endif


/*
 * goform/setDns
 */
static void setDns(webs_t* wp, char_t *path, char_t *query)
{
	char *ddns_provider, *ddns, *ddns_acc, *ddns_pass;
	char empty_char = '\0';
	char_t *submitUrl;

	nvram_init(RT2860_NVRAM);

	char_t *reset		= websGetVar(wp, T("reset"), T("0"));

	if (CHK_IF_DIGIT(reset, 1)) {
		nvram_fromdef(RT2860_NVRAM, 8, "dnsPEnabled", "DDNSProvider", "DDNS", "DDNSAccount", "DDNSPassword",
						"dnsToLocalRedir", "dns_diffsrv", "dns_sec");
		return;
	}

	wp->on_response_ok = DO_RESTART_MISC;

	char_t *dns_proxy	= websGetVar(wp, T("dnsPEnabled"), T("0"));
	ngx_nvram_bufset(wp, "dnsPEnabled", dns_proxy);

	if (CHK_IF_DIGIT(dns_proxy, 1)) {
		// DNS to local redirect
		char_t *dnsToLocalRedir = websGetVar(wp, T("dnsToLocalRedir"), T(""));
		ngx_nvram_bufset(wp, "dnsToLocalRedir", dnsToLocalRedir);

		// Hosts
		char_t *dns_local_hosts = websGetVar(wp, T("dns_local_hosts"), T(""));
		ngx_nvram_bufset(wp, "dns_local_hosts", dns_local_hosts);

		// DNS Content Filter
		char_t *dns_adblock = websGetVar(wp, T("dns_adblock"), T(""));
		ngx_nvram_bufset(wp, "dns_adblock", dns_adblock);

		char_t *dns_adblock_skip = websGetVar(wp, T("dns_adblock_skip"), T(""));
		ngx_nvram_bufset(wp, "dns_adblock_skip", dns_adblock_skip);

		char_t *dns_userblock = websGetVar(wp, T("dns_userblock"), T(""));
		ngx_nvram_bufset(wp, "dns_userblock", dns_userblock);

		char_t *dns_diffsrv = websGetVar(wp, T("dns_diffsrv"), T(""));
		ngx_nvram_bufset(wp, "dns_diffsrv", dns_diffsrv);

#ifdef CONFIG_LIB_NETTLE
#ifdef CONFIG_USER_DNSMASQSEC
		// Enable DNSSEC
		char_t *dns_sec = websGetVar(wp, T("dns_sec"), T("0"));
		ngx_nvram_bufset(wp, "dns_sec", dns_sec);
#endif
#endif
		/* force remove adlist if disable block */
		if (CHK_IF_DIGIT(dns_adblock, 0))
		    doSystem("rm -f /etc/dnsmasq.d/ads.conf");

		doSystem("service dnsserver adrestart");
	}
	else
	{
		ngx_nvram_bufset(wp, "dnsToLocalRedir", "0");
		ngx_nvram_bufset(wp, "dns_diffsrv", "");
	}


#ifdef CONFIG_USER_INADYN
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

	ngx_nvram_bufset(wp, "DDNSProvider", ddns_provider);
	ngx_nvram_bufset(wp, "DDNS", ddns);
	ngx_nvram_bufset(wp, "DDNSAccount", ddns_acc);
	ngx_nvram_bufset(wp, "DDNSPassword", ddns_pass);
#endif

	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

invalid_values:
#ifdef PRINT_DEBUG
//	websHeader(wp);
	outWrite(T("<h2>DDNS Settings</h2><br>\n"));
	outWrite(T("DDNSProvider: %s<br>\n"), ddns_provider);
	outWrite(T("DDNS: %s<br>\n"), ddns);
	outWrite(T("DDNSAccount: %s<br>\n"), ddns_acc);
	outWrite(T("DDNSPassword: %s<br>\n"), ddns_pass);
//	websFooter(wp);
	websDone(wp, 200);
#else
	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
#endif
}

static int getPortStatus(webs_t *wp, char** params, int nparams)
{
	outWrite(T("{ \"ethernet\": [ "));
#if defined(CONFIG_ETHTOOL)
#if defined(CONFIG_RAETH_ESW) || defined(CONFIG_MT7530_GSW)
	int port;
        int first = 1;

#ifdef CONFIG_RTESW_SWITCH_ONEPORT
	for (port = 4; port > 3; port--)
#else
	for (port = 4; port > -1; port--)
#endif
	{
		struct port_status pst;

		portstatus(&pst, port);

#ifndef CONFIG_RTESW_SWITCH_ONEPORT
		if (first != 1) {
			outWrite(", ");
		}
#endif
		outWrite("{ \"port\": \"%d\", \"link\": \"%d\", \"speed\": \"%d\", \"duplex\": \"%s\" }", pst.portnum, pst.link, pst.speed, (pst.duplex == 1) ? "F" : "H");
                first = 0;
	}
#endif
#endif
	outWrite(T("] }"));
	return 0;
}


static int getAllNICStatisticASP(webs_t *wp, char** params, int nparams)
{
	int i;
	int elem_count = 0;
	struct nic_counts* ncs = nicscounts(&elem_count);

	outWrite(T("{ \"iface\": [ "));

	for (i = 0; i < elem_count; i++)
	{
		struct nic_counts nc = ncs[i];
		if (nc.ifname[0] == '\0')
			break;

		if (!nc.is_available)
			continue;
		else {
			char strbuf[512];
			snprintf(strbuf, sizeof(strbuf),
				"{ \"name\":\"%s\", \"rx_packet\":\"%llu\", \"rx_bytes\":\"%llu\", \"tx_packet\":\"%llu\", \"tx_bytes\":\"%llu\" }%s",
				nc.ifname, nc.rx_packets, nc.rx_bytes, nc.tx_packets, nc.tx_bytes, (i + 1 < elem_count) ? ", " : "");
			outWrite(T("%s"), strbuf);
		}
	}

	outWrite(T(" ] }"));
	free(ncs);
	return 0;
}

static int getMemTotalASP(webs_t *wp, char** params, int nparams)
{
	struct mem_stats mem;
	char buf[16];

	getMemData(&mem);

	snprintf(buf, sizeof(buf), "%lu", mem.total);
	return outWrite(T("%s"), buf);
}

static int getMemLeftASP(webs_t *wp, char** params, int nparams)
{
	struct mem_stats mem;
	char buf[16];

	getMemData(&mem);

	snprintf(buf, sizeof(buf), "%lu", mem.free);
	return outWrite(T("%s"), buf);
}

static unsigned long long prevbusy, prevtotal;
static int getCpuUsageASP(webs_t *wp, char** params, int nparams)
{
	struct cpu_stats cpu;
	char buf[16];
	float outd = 0;

	getCPUData(&cpu);

	if (cpu.total != prevtotal)
	    outd=((((float)cpu.busy-(float)prevbusy)/((float)cpu.total-(float)prevtotal))*100);

	prevbusy=cpu.busy;
	prevtotal=cpu.total;

	snprintf(buf, sizeof(buf), "%.1f", outd);
	return outWrite(T("%s %%"), buf);
}

static void LoadDefaultSettings(webs_t* wp, char_t *path, char_t *query)
{
	/* Output timer for reloading */
        websHeader(wp);
	outputTimerForReload(wp, 60000);
        websFooter(wp);

	/* restore defaults and rwfs drop */
	doSystem("fs nvramreset");
	doSystem("fs restore");

        wp->on_response_ok = DO_REBOOT;
}

#ifdef CONFIG_SYSLOGD
parameter_fetch_t service_syslog_flags[] =
{
	{ T("KLogd"), "KLogd", 0, T("0") },
	{ T("SysLogd"), "SysLogd", 0, T("0") },
	{ T("SysLogLevel"), "SysLogLevel", 0, T("7") },
	{ T("RemoteSysLogIP"), "RemoteSysLogIP", 0, T("") },
	{ NULL, NULL, 0, NULL } // Terminator
};

static void clearlog(webs_t* wp, char_t *path, char_t *query)
{
	doSystem("service syslog stop");
	truncate("/var/log/messages", 0);
	doSystem("service syslog start");

        websSetContentType(wp, "text/plain");
        outWrite(" ");
	websDone(wp, 200);
}

static void setuplog(webs_t* wp, char_t *path, char_t *query)
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
static void getsyslog(webs_t* wp, char_t *path, char_t *query)
{
    char *log;

    websSetContentType(wp, "text/plain");

    log = ngx_pcalloc(wp->pool, LOG_MAX * sizeof(char));
    if(!log){
        syslog(LOG_ERR, "no memory left, %s", __FUNCTION__);
        websDone(wp, 200);
        return;
    }

    getSyslogTail(log, LOG_MAX);
    websLongWrite(wp, log);
    ngx_pfree(wp->pool, log);

    websDone(wp, 200);
}
#endif

static int getHWStatsBuilt(webs_t *wp, char** params, int nparams) {
#ifdef CONFIG_RAETH_SNMPD
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getHWStatistic(webs_t *wp, char** params, int nparams) {
	int i;
	struct port_counts pcs;
	char tmpstr[64];

	portscounts(&pcs);

	outWrite(T("{ \"rx_bytes\": [ "));
#ifdef CONFIG_RTESW_SWITCH_ONEPORT
	for (i = 4; i >= 4; i--)
#else
	for (i = 4; i >= 0; i--)
#endif
	{
		sprintf(tmpstr, "%" PRIu64, (uint64_t)pcs.rx_count[i]);
		outWrite(T("\"%s\""), tmpstr);
#ifndef CONFIG_RTESW_SWITCH_ONEPORT
		if (i > 0)
			outWrite(T(", "));
#endif
	}
	outWrite(T(" ], \"tx_bytes\": [ "));
#ifdef CONFIG_RTESW_SWITCH_ONEPORT
	for (i = 4; i >= 4; i--)
#else
	for (i = 4; i >= 0; i--)
#endif
	{
		sprintf(tmpstr, "%" PRIu64, (uint64_t)pcs.tx_count[i]);
		outWrite(T("\"%s\""), tmpstr);
#ifndef CONFIG_RTESW_SWITCH_ONEPORT
		if (i > 0)
			outWrite(T(", "));
#endif
	}
	outWrite(T(" ] }"));
	return 0;
}

static void getDeviceConfigCallback(void* extra_ptr, char* name, char* val)
{
    webs_t* wp = (webs_t*) extra_ptr;
    outWrite("%s=%s\n", name, val);
}

static void getDeviceConfig(webs_t* wp, char_t *path, char_t *query)
{
    FUNCTION_TRACE(wp->log);

    websSetContentType(wp, "application/octet-stream");
    websAddHeader(wp, "Content-Disposition", "attachment; filename=\"config.dat\"");
    websAddHeader(wp, "Content-Transfer-Encoding", "binary");

    outWrite("#The following line must not be removed.\n");
    outWrite("Default\n");

    nvram_iter(RT2860_NVRAM, (void*) wp, getDeviceConfigCallback);

    websDone(wp, 200);
}

static void getAdblockStatus(webs_t* wp, char_t *path, char_t *query)
{
	if (access( "/tmp/adblock_runing", F_OK ) != -1) {
		if (access( "/etc/dnsmasq.d/ads.conf", F_OK ) != -1) {
			outWrite("2");
		} else {
			outWrite("1");
		}
	} else {
		outWrite("0");
	}

	websDone(wp, 200);
}


void asp_mod_management_init()
{
	aspDefineFunc(("getMemLeftASP"), getMemLeftASP, EVERYONE);
	aspDefineFunc(("getMemTotalASP"), getMemTotalASP, EVERYONE);
	aspDefineFunc(("getCpuUsageASP"), getCpuUsageASP, EVERYONE);
	aspDefineFunc(("getAllNICStatisticASP"), getAllNICStatisticASP, EVERYONE);
	aspDefineFunc(("getHWStatsBuilt"), getHWStatsBuilt, EVERYONE);
	aspDefineFunc(("getHWStatistic"), getHWStatistic, EVERYONE);
	aspDefineFunc(("getPortStatus"), getPortStatus, EVERYONE);

	websFormDefine(T("getDeviceConfig"), getDeviceConfig, ADMIN);
	websFormDefine(T("getAdblockStatus"), getAdblockStatus, EVERYONE);

#ifdef CONFIG_DATE
	websFormDefine(T("NTPSyncWithHost"), NTPSyncWithHost, EVERYONE);
#endif
#ifdef CONFIG_USER_INADYN
	websFormDefine(T("setDns"), setDns, EVERYONE);
#endif
#ifdef CONFIG_SYSLOGD
	websFormDefine(T("getsyslog"), getsyslog, EVERYONE);
	websFormDefine(T("clearlog"), clearlog, ADMIN);
	websFormDefine(T("setuplog"), setuplog, ADMIN);
#endif
	websFormDefine(T("setSysAdm"), setSysAdm, ADMIN);
	websFormDefine(T("setSysLang"), setSysLang, EVERYONE);
	websFormDefine(T("NTP"), NTP, ADMIN);
	websFormDefine(T("LoadDefaultSettings"), LoadDefaultSettings, ADMIN);

}
