#include <stdlib.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <arpa/inet.h>
#include <asm/types.h>
#include <linux/if.h>
#include <ctype.h>
#include <errno.h>

#include "utils.h"
#include "helpers.h"

#ifdef USER_MANAGEMENT_SUPPORT
#include "um.h"
#endif
#include "internet.h"
#include "management.h"

static void scale(char_t * strBuf, unsigned long long data)
{
    double p1;

    if (data > (unsigned long long)1099511627775) {
	p1=(double)data / (double)1099511627776;
	snprintf(strBuf, 32, "%.2f T", p1);
    } else {
	if (data > (unsigned long long)1073741823) {
	    p1=(double)data / (double)1073741824;
	    snprintf(strBuf, 32, "%.2f G", p1);
	} else {
	    if (data > (unsigned long long)1048575) {
		p1=(double)data / (double)1048576;
		snprintf(strBuf, 32, "%.2f M", p1);
	    } else {
		if (data > 1023) {
		    p1=(double)data / (double)1024;
		    snprintf(strBuf, 32, "%.2f k", p1);
		} else {
		    snprintf(strBuf, 32, "%lld", data);
		}
	    }
	}
    }
}

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

/*
 * arguments: ifname - interface name
 * description: return 1 if interface is up
 *              return 0 if interface is down
 */
static int getIfIsUp(char *ifname)
{
	struct ifreq ifr;
	int skfd;

	skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (skfd == -1) {
		syslog(LOG_ERR, "open socket failed, %s\n", __FUNCTION__);
		return -1;
	}

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1);
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';
	if (ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0) {
		close(skfd);
		syslog(LOG_ERR, "ioctl call failed, %s\n", __FUNCTION__);
		return -1;
	}
	close(skfd);

	if (ifr.ifr_flags & IFF_UP)
		return 1;
	else
		return 0;
}

#if defined(CONFIG_ETHTOOL)
/*
 * description: get link info from ethtool (return defaults values in error path for compat with webswrite)
 */
static int linkspeed(const char *ifname) {
	struct ethtool_cmd ecmd = { .cmd = ETHTOOL_GSET, };
	int sd, iocret, speed = SPEED_10;
	struct ifreq ifr;

	if(strlen(ifname) > IFNAMSIZ)
		return SPEED_10;

	if((sd = socket(AF_INET,SOCK_DGRAM,0)) < 0)
		return SPEED_10;

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1);
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';
	ifr.ifr_data = (caddr_t)&ecmd;
	if((iocret = ioctl(sd,SIOCETHTOOL,&ifr)) == 0)
		speed = ecmd.speed;
	else
		syslog(LOG_ERR, "ioctl error, %s\n", __FUNCTION__);

	close(sd);

	/* validate nic link speed */
	switch (speed) {
	    case SPEED_10:
	    case SPEED_100:
#ifdef CONFIG_RALINK_MT7621
	    case SPEED_1000:
#endif

	    return speed;
	}

	return SPEED_10;
}

static int linkduplex(const char *ifname) {
	struct ethtool_cmd ecmd = { .cmd = ETHTOOL_GSET, };
	int sd, iocret, duplex = DUPLEX_HALF;
	struct ifreq ifr;

	if(strlen(ifname) > IFNAMSIZ)
		return DUPLEX_HALF;

	if((sd = socket(AF_INET,SOCK_DGRAM,0)) < 0)
		return DUPLEX_HALF;

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1);
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';
	ifr.ifr_data = (caddr_t)&ecmd;
	if((iocret = ioctl(sd,SIOCETHTOOL,&ifr)) == 0)
		duplex = ecmd.duplex;
	else
		syslog(LOG_ERR, "ioctl error, %s\n", __FUNCTION__);

	close(sd);

	/* validate duplex mode */
	switch (duplex) {
	    case DUPLEX_HALF:
	    case DUPLEX_FULL:

	    return duplex;
	}

	return DUPLEX_HALF;
}

static int linkstatus(const char *ifname) {
	struct ethtool_value ethval = { .cmd = ETHTOOL_GLINK, };
	int sd, iocret, ret = 0;
	struct ifreq ifr;

	if(strlen(ifname) > IFNAMSIZ)
		return 0;

	if((sd = socket(AF_INET,SOCK_DGRAM,0)) < 0)
		return 0;

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1);
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';
	ifr.ifr_data = (caddr_t)&ethval;
	if((iocret = ioctl(sd,SIOCETHTOOL,&ifr)) == 0)
		ret = ethval.data;
	else
		syslog(LOG_ERR, "ioctl error, %s\n", __FUNCTION__);

	close(sd);

	/* validate link status */
	if (ret)
	    return 1;
	else
	    return 0;
}
#endif

static int getPortStatus(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(CONFIG_ETHTOOL)
#if defined(CONFIG_RAETH_ESW) || defined(CONFIG_MT7530_GSW)
	int port, first = 1;

	if (!getIfIsUp(IOCTL_IF)) {
	    syslog(LOG_ERR, "ioctl iface down, %s\n", __FUNCTION__);
	    return -1;
	}

	for (port=4; port>-1; port--)
	{
		char buf[16];
		char duplex;
		int link, speed;
		FILE *proc_file;

		/* switch phy to needed port */
		proc_file = fopen(PROCREG_GMAC, "w");
		if (!proc_file) {
		    syslog(LOG_ERR, "no proc, %s\n", __FUNCTION__);
		    websWrite(wp, T(" "));
		    return -1;
		}
		fprintf(proc_file, "%d", port);
		fclose(proc_file);

		link = linkstatus(IOCTL_IF);
		speed = linkspeed(IOCTL_IF);

		if (linkduplex(IOCTL_IF))
			duplex = 'F';
		else
			duplex = 'H';

		/* create string in new buffer and write to web (this more safe of direct write) */
		snprintf(buf, sizeof(buf), ("%s%d,%d,%c"), (first) ? "" : ";", link, speed, duplex);
		websWrite(wp, T("%s"), buf);

		first = 0;
	}
#endif
#endif
	return 0;
}
static int getAllNICStatisticASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char buf[512];
	char_t result[32];
	FILE *fp = fopen(_PATH_PROCNET_DEV, "r");

	if (fp == NULL)
	{
		syslog(LOG_ERR, "no proc, %s\n", __FUNCTION__);
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

		if (buf == NULL || buf[0] == '\n')
			continue;

		// find : , extract ifname, move pointer to next block semicolon
		semiColon = strchr(buf, ':');
		if (semiColon == NULL || (*semiColon++ = 0, sscanf(buf, "%s", ifname) != 1)) {
			syslog(LOG_ERR, "wrong format string in /proc/net/dev");
			continue;
		}

		// filter 'lo' interface
		if (strcmp(ifname, "lo") == 0)
			continue;

		// check that interface is up
		if (getIfIsUp(ifname) != 1)
			continue;

		// now output statistics
		websWrite(wp, T("<tr><td class=\"head\" colspan=\"2\">%s</td>"), ifname);

		// extract scale and print statistic
		if (sscanf(semiColon, "%llu%llu%llu%llu%llu%llu%llu%*d%llu%llu%llu%llu%llu%llu%llu",
			   &rx_bytes, &rx_packets, &rx_errs, &rx_drops, &rx_fifo, &rx_frame, &rx_multi,
			    &tx_bytes, &tx_packets, &tx_errs, &tx_drops, &tx_fifo, &tx_colls, &tx_carrier) != 14) {
			// not extracted - print n/a
			strcpy(result, "n/a");
			websWrite(wp, T("<td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n"), result, result, result, result);
			continue;
		} else {
			// scale and print result
			scale(result, rx_packets);
			websWrite(wp, T("<td>%s</td>"), result);
			scale(result, rx_bytes);
			websWrite(wp, T("<td>%s</td>"), result);
			scale(result, tx_packets);
			websWrite(wp, T("<td>%s</td>"), result);
			scale(result, tx_bytes);
			websWrite(wp, T("<td>%s</td>\n</tr>\n"), result);
		}
	}
	fclose(fp);

	return 0;
}

struct mem_stats {
	unsigned long int total;    // RAM total
	unsigned long int free;     // RAM free
	unsigned long int buffers;  // RAM buffers
	unsigned long int cached;   // RAM cached
	unsigned long int sw_total; // Swap total
	unsigned long int sw_free;  // Swap free
};

static void get_memdata(struct mem_stats *st)
{
	FILE *fp;
	char line_buf[64];

	st->total = st->free = st->buffers = st->cached = st->sw_total = st->sw_free = 0;

	fp = fopen("/proc/meminfo", "r");
	if(!fp){
		syslog(LOG_ERR, "no proc, %s\n", __FUNCTION__);
		return;
	}

	if (fgets(line_buf, sizeof(line_buf), fp) && sscanf(line_buf, "MemTotal: %lu %*s", &st->total) == 1) {
		fgets(line_buf, sizeof(line_buf), fp);
		sscanf(line_buf, "MemFree: %lu %*s", &st->free);

		fgets(line_buf, sizeof(line_buf), fp);
		sscanf(line_buf, "Buffers: %lu %*s", &st->buffers);

		fgets(line_buf, sizeof(line_buf), fp);
		sscanf(line_buf, "Cached: %lu %*s", &st->cached);
	}
	fclose(fp);
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

struct cpu_stats {
	unsigned long long user;    // user (application) usage
	unsigned long long nice;    // user usage with "niced" priority
	unsigned long long system;  // system (kernel) level usage
	unsigned long long idle;    // CPU idle and no disk I/O outstanding
	unsigned long long iowait;  // CPU idle but with outstanding disk I/O
	unsigned long long irq;     // Interrupt requests
	unsigned long long sirq;    // Soft interrupt requests
	unsigned long long steal;   // Invol wait, hypervisor svcing other virtual CPU
	unsigned long long busy;
	unsigned long long total;
};

static void getcpudata(struct cpu_stats *st)
{
	FILE *fp;
	char line_buf[256];

	st->user = st->nice = st->system = st->idle = st->iowait = st->irq = st->sirq = st->steal = st->busy = st->total = 0;

	fp = fopen(PROC_CPU_STATISTIC, "r");
	if(!fp){
		syslog(LOG_ERR, "no proc, %s\n", __FUNCTION__);
		return;
	}

	if (fgets(line_buf, sizeof(line_buf), fp)) {
		if (sscanf(line_buf, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
			&st->user, &st->nice, &st->system, &st->idle, &st->iowait, &st->irq, &st->sirq, &st->steal) >= 4) {
			/* calculate busy/total */
			st->busy = st->user + st->nice + st->system + st->irq + st->sirq + st->steal + st->iowait;
			st->total = st->busy + st->idle;
		}
	}
	fclose(fp);
}

static unsigned long long prevbusy, prevtotal;
static int getCpuUsageASP(int eid, webs_t wp, int argc, char_t **argv)
{
	struct cpu_stats cpu;
	char buf[16];
	float outd = 0;

	getcpudata(&cpu);

	if (cpu.total-prevtotal > 0)
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
const parameter_fetch_t service_syslog_flags[] =
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
		syslog(LOG_ERR, "no log exist, %s\n", __FUNCTION__);
		goto error;
	}

	log = malloc(LOG_MAX * sizeof(char));
	if(!log){
		syslog(LOG_ERR, "no memory left, %s\n", __FUNCTION__);
		goto error;
	}

	memset(log, 0, LOG_MAX);
	fread(log, 1, LOG_MAX, fp);
	websLongWrite(wp, log);

	free(log);
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
	char_t port_buf[32];
	static unsigned long long rx_count[6], tx_count[6];
#ifdef CONFIG_RAETH_SNMPD
	char buf[256];
	FILE *fp;
#endif
	rx_count[0] = rx_count[1] = rx_count[2] = rx_count[3] = rx_count[4] = rx_count[5] = 0;
	tx_count[0] = tx_count[1] = tx_count[2] = tx_count[3] = tx_count[4] = tx_count[5] = 0;

#ifdef CONFIG_RAETH_SNMPD
	fp = fopen(PROCREG_SNMP, "r");
	if (fp == NULL) {
		syslog(LOG_ERR, "no snmp, %s\n", __FUNCTION__);
		return -1;
	}

	while ((fgets(buf, sizeof(buf), fp)) != NULL) {
		if (buf == NULL || buf[0] == '\n')
		    continue;
		if (6 == sscanf(buf, "rx64 counters: %llu %llu %llu %llu %llu %llu\n", &rx_count[0], &rx_count[1], &rx_count[2], &rx_count[3], &rx_count[4], &rx_count[5]))
		    continue;
		if (6 == sscanf(buf, "tx64 counters: %llu %llu %llu %llu %llu %llu\n", &tx_count[0], &tx_count[1], &tx_count[2], &tx_count[3], &tx_count[4], &tx_count[5]))
		    break;
	}
	fclose(fp);
#endif

	websWrite(wp, T("<tr>\n<td class=\"head\" id=\"stats_rx\">Rx</td>\n"));
	for (i = 4; i >= 0; i--)
	{
		scale(port_buf, rx_count[i]);
		websWrite(wp, T("<td>%s</td>\n"), port_buf);
	}
	websWrite(wp, T("</tr>\n<tr>\n<td class=\"head\" id=\"stats_tx\">Tx</td>\n"));
	for (i = 4; i >= 0; i--)
	{
		scale(port_buf, tx_count[i]);
		websWrite(wp, T("<td>%s</td>\n"), port_buf);
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
