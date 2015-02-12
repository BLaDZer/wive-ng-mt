#include <stdlib.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
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
#include "wireless.h"
#include "management.h"

static unsigned long int prevTotal = 0, prevBusy = 0;

void scale(char_t * strBuf, long long data)
{
double p1;
 
if (data > (unsigned long long)1099511627775)
{
  p1=(double)data / (double)1099511627776;
  snprintf(strBuf, 32, "%.2f T", p1);
}
else
{
  if (data > (unsigned long long)1073741823)
  {
    p1=(double)data / (double)1073741824;
    snprintf(strBuf, 32, "%.2f G", p1);
  }
  else
  {
    if (data > (unsigned long long)1048575)
    {
      p1=(double)data / (double)1048576;
      snprintf(strBuf, 32, "%.2f M", p1);
    }
    else
    {
      if (data > 1023)
      {
        p1=(double)data / (double)1024;
        snprintf(strBuf, 32, "%.2f k", p1);
      }
      else
      {
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
	char *submitUrl;

	old_user = nvram_get(RT2860_NVRAM, "Login");
	admuser = websGetVar(wp, T("admuser"), T(""));
	admpass = websGetVar(wp, T("admpass"), T(""));

	if (!strlen(admuser)) {
		error(E_L, E_LOG, T("setSysAdm: account empty, leave it unchanged"));
		return;
	}
	if (!strlen(admpass)) {
		error(E_L, E_LOG, T("setSysAdm: password empty, leave it unchanged"));
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
#if defined(CONFIG_USER_SAMBA) || defined(CONFIG_USER_SAMBA3) || defined(CONFIG_USER_SAMBA3_WINS)
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

	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
#ifdef PRINT_DEBUG
	if (!submitUrl || !submitUrl[0])
	{
		websHeader(wp);
		websWrite(wp, T("<h2>Adminstrator Settings</h2><br>\n"));
		websWrite(wp, T("adm user: %s<br>\n"), admuser);
		websWrite(wp, T("adm pass: %s<br>\n"), admpass);
		websFooter(wp);
		websDone(wp, 200);
	} else
#endif
		websRedirect(wp, submitUrl);
}

/*
 * goform/setSysLang
 */
static void setSysLang(webs_t wp, char_t *path, char_t *query)
{
	char_t *lang;
	char *submitUrl;

	lang = websGetVar(wp, T("langSelection"), T(""));
	nvram_set(RT2860_NVRAM, "Language", lang);

	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
#ifdef PRINT_DEBUG
	if (!submitUrl || !submitUrl[0])
	{
		websHeader(wp);
		websWrite(wp, T("<h2>Language Selection</h2><br>\n"));
		websWrite(wp, T("language: %s<br>\n"), lang);
		websFooter(wp);
		websDone(wp, 200);
	} else
#endif
		websRedirect(wp, submitUrl);
}

/*
 * goform/NTP
 */
static void NTP(webs_t wp, char_t *path, char_t *query)
{
	char *tz, *ntpServer, *ntpEnabled;
	char *submitUrl;

	tz = websGetVar(wp, T("time_zone"), T(""));
	ntpServer = websGetVar(wp, T("NTPServerIP"), T(""));
	ntpEnabled = websGetVar(wp, T("ntp_enabled"), T("off"));

	nvram_init(RT2860_NVRAM);
	if (strcmp(ntpEnabled, "on")==0)
	{
		if ((strlen(tz)>0) && (!checkSemicolon(tz)))
		{
			nvram_bufset(RT2860_NVRAM, "NTPServerIP", ntpServer);
			nvram_bufset(RT2860_NVRAM, "TZ", tz);
		}
	}

	nvram_bufset(RT2860_NVRAM, "NTPEnabled", ntpEnabled);
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	if (strcmp(ntpEnabled, "on")==0)
		doSystem("service ntp restart");
	else
		doSystem("service ntp stop");

	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
#ifdef PRINT_DEBUG
	if (!submitUrl || !submitUrl[0])
	{
		websHeader(wp);
		websWrite(wp, T("<h2>NTP Settings</h2><br>\n"));
		websWrite(wp, T("NTPEnabled: %s<br>\n"), ntpEnabled);
		websWrite(wp, T("NTPserver: %s<br>\n"), ntpServer);
		websWrite(wp, T("TZ: %s<br>\n"), tz);
		websFooter(wp);
		websDone(wp, 200);
	} else
#endif
		websRedirect(wp, submitUrl);
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
	char *submitUrl;

	ddns_provider = websGetVar(wp, T("DDNSProvider"), T("none"));
	ddns = websGetVar(wp, T("DDNS"), T(""));
	ddns_acc = websGetVar(wp, T("Account"), T(""));
	ddns_pass = websGetVar(wp, T("Password"), T(""));

	if(!ddns_provider || !ddns || !ddns_acc || !ddns_pass)
		return;

	if(!strcmp(T("none"), ddns_provider )){
		ddns = ddns_acc = ddns_pass = &empty_char;
	}else{
		if(!strlen(ddns) || !strlen(ddns_acc) || !strlen(ddns_pass))
			return;
	}

	if(checkSemicolon(ddns) || checkSemicolon(ddns_acc) || checkSemicolon(ddns_pass))
		return;

        nvram_init(RT2860_NVRAM);
	nvram_bufset(RT2860_NVRAM, "DDNSProvider", ddns_provider);
	nvram_bufset(RT2860_NVRAM, "DDNS", ddns);
	nvram_bufset(RT2860_NVRAM, "DDNSAccount", ddns_acc);
	nvram_bufset(RT2860_NVRAM, "DDNSPassword", ddns_pass);
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);


	doSystem("service ddns restart");

	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
#ifdef PRINT_DEBUG
	if (!submitUrl || !submitUrl[0])
	{
		websHeader(wp);
		websWrite(wp, T("<h2>DDNS Settings</h2><br>\n"));
		websWrite(wp, T("DDNSProvider: %s<br>\n"), ddns_provider);
		websWrite(wp, T("DDNS: %s<br>\n"), ddns);
		websWrite(wp, T("DDNSAccount: %s<br>\n"), ddns_acc);
		websWrite(wp, T("DDNSPassword: %s<br>\n"), ddns_pass);
		websFooter(wp);
		websDone(wp, 200);
	} else
#endif
		websRedirect(wp, submitUrl);
}
#endif

char* getField(char *a_line, char *delim, int count)
{
	int i=0;
	char *tok;
	tok = strtok(a_line, delim);

	while (tok)
	{
		if (i == count)
			break;
		i++;
		tok = strtok(NULL, delim);
	}

	if(tok && isdigit(*tok))
		return tok;

	return NULL;
}

/*
 *   C version. (ASP version is below)
 */
static long long getIfStatistic(char *interface, int type)
{
	int found_flag = 0;
	int skip_line = 2;
	char buf[1024], *field, *semiColon = NULL;
	FILE *fp = fopen(PROC_IF_STATISTIC, "r");

	if(!fp)
	{
		printf("no proc?\n");
		return -1;
	}

	while (fgets(buf, 1024, fp))
	{
		char *ifname;
		if (skip_line != 0)
		{
			skip_line--;
			continue;
		}
		if ( !(semiColon = strchr(buf, ':')))
			continue;
		*semiColon = '\0';
		ifname = buf;
		ifname = strip_space(ifname);

		if (!strcmp(ifname, interface))
		{
			found_flag = 1;
			break;
		}
	}
	fclose(fp);

	semiColon++;

	switch(type){
	case TXBYTE:
		if(  (field = getField(semiColon, " ", 8))  ){
			return strtoll(field, NULL, 10);
		}
		break;
	case TXPACKET:
		if(  (field = getField(semiColon, " ", 9))  ){
			return strtoll(field, NULL, 10);
		}
		break;
	case RXBYTE:
		if(  (field = getField(semiColon, " ", 0))  ){
			return strtoll(field, NULL, 10);
		}
		break;
	case RXPACKET:
		if(  (field = getField(semiColon, " ", 1))  ){
			return strtoll(field, NULL, 10);
		}
		break;
	}
	return -1;
}

/*
 *     getIfStatistic()   ASP version
 */
int getIfStatisticASP(int eid, webs_t wp, int argc, char_t **argv)
{
	int found_flag = 0;
	int skip_line = 2;
	char *interface, *type, *field, *semiColon = NULL;
	char buf[1024], result[32];

	FILE *fp = fopen(PROC_IF_STATISTIC, "r");
	if(!fp)
	{
		websWrite(wp, T("no proc?\n"));
		return -1;
	}

	if (ejArgs(argc, argv, T("%s %s"), &interface, &type) != 2)
	{
		websWrite(wp, T("Wrong argument.\n"));
		fclose(fp);
		return -1;
	}

	while (fgets(buf, 1024, fp))
	{
		char *ifname;
		if (skip_line != 0)
		{
			skip_line--;
			continue;
		}
		if (!(semiColon = strchr(buf, ':')))
			continue;
		*semiColon = '\0';
		ifname = buf;
		ifname = strip_space(ifname);

		if(!strcmp(ifname, interface)){
			found_flag = 1;
			break;
		}
	}
	fclose(fp);

	semiColon++;

	if(!strcmp(type, T("TXBYTE")  )){
		if(  (field = getField(semiColon, " ", 8))  )
        {
			scale(result, strtoll(field, NULL, 10));
			ejSetResult(eid, result);
		}
	}else if(!strcmp(type, T("TXPACKET")  )){
		if(  (field = getField(semiColon, " ", 9))  )
        {
			snprintf(result, 32,"%lld",   strtoll(field, NULL, 10));
			ejSetResult(eid, result);
		}
    }else if(!strcmp(type, T("RXBYTE")  )){
		if(  (field = getField(semiColon, " ", 0))  )
        {
			scale(result, strtoll(field, NULL, 10));
			ejSetResult(eid, result);
		}
    }else if(!strcmp(type, T("RXPACKET")  )){
		if(  (field = getField(semiColon, " ", 1))  )
        {
			snprintf(result, 32,"%lld",   strtoll(field, NULL, 10));
			ejSetResult(eid, result);
		}
    }else{
		websWrite(wp, T("unknown type.") );
		return -1;
	}
	return -1;
}

int getWANRxByteASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[32];
	long long data = getIfStatistic(getWanIfName(), RXBYTE);
	scale(buf, data);
	websWrite(wp, T("%s"), buf);
	return 0;
}

int getWANRxPacketASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[32];
	long long data = getIfStatistic(getWanIfName(), RXPACKET);
	snprintf(buf, 32, "%lld", data);
	websWrite(wp, T("%s"), buf);
	return 0;
}

int getWANTxByteASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[32];
	long long data = getIfStatistic(getWanIfName(), TXBYTE);
	scale(buf, data);
	websWrite(wp, T("%s"), buf);
	return 0;
}

int getWANTxPacketASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[32];
	long long data = getIfStatistic(getWanIfName(), TXPACKET);
	snprintf(buf, 32, "%lld", data);
	websWrite(wp, T("%s"), buf);
	return 0;
}

int getLANRxByteASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[32];
	long long data = getIfStatistic(getLanIfName(), RXBYTE);
	scale(buf, data);
	websWrite(wp, T("%s"), buf);
	return 0;
}

int getLANRxPacketASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[32];
	long long data = getIfStatistic(getLanIfName(), RXPACKET);
	snprintf(buf, 32, "%lld", data);
	websWrite(wp, T("%s"), buf);
	return 0;
}

int getLANTxByteASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[32];
	long long data = getIfStatistic(getLanIfName(), TXBYTE);
	scale(buf, data);
	websWrite(wp, T("%s"), buf);
	return 0;
}

int getLANTxPacketASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[32];
	long long data = getIfStatistic(getLanIfName(), TXPACKET);
	snprintf(buf, 32, "%lld", data);
	websWrite(wp, T("%s"),buf);
	return 0;
}

int getAllNICStatisticASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char buf[1024];
	char tmp[1024];
	char_t result[32];
	int skip_line = 2;
	const char *field;
	struct ifreq ifr;
	int skfd;

	FILE *fp = fopen(PROC_IF_STATISTIC, "r");
	if (fp == NULL)
	{
		printf("no proc?\n");
		return -1;
	}

	if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("open socket failed\n");
		fclose(fp);
		return -1;
	}

	while (fgets(buf, 1024, fp))
	{
		char *ifname, *semiColon;
		if (skip_line != 0)
		{
			skip_line--;
			continue;
		}
		if (!(semiColon = strchr(buf, ':')))
			continue;
		*(semiColon++) = '\0';

		ifname = buf;
		ifname = strip_space(ifname);

		// Filter 'lo' interface
		if (strcmp(ifname, "lo")==0)
			continue;

		// Check that interface is up
		strcpy(ifr.ifr_name, ifname);
		if (ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0)
		{
			printf("ioctl() error\n");
			continue;
		}

		if ((ifr.ifr_flags & IFF_UP) == 0) // Interface is down?
			continue;

		// Now output statistics
		websWrite(wp, T("<tr>"));
		websWrite(wp, T("<td class=\"head\" colspan=\"2\">%s</td>"), ifname);

		// strtok causes string to rewrite with '\0', perform temporary buffer
		//RX P
		strcpy(tmp, semiColon);
		websWrite(wp, T("<td>%s</td>"),
			(field = getField(tmp, " ", 1)) ? field : "n/a");
		//RX B
		strcpy(tmp, semiColon);
		field = getField(tmp, " ", 0);
		if (field != NULL)
		{
			scale(result, strtoll(field, NULL, 10));
		}
		else
		{
			strcpy(result, "n/a");
		}
		websWrite(wp, T("<td>%s</td>"), result);
		//TX P
		strcpy(tmp, semiColon);
		websWrite(wp, T("<td>%s</td>"),
			(field = getField(tmp, " ", 9)) ? field : "n/a");
		//TX B
		strcpy(tmp, semiColon);
		field = getField(tmp, " ", 8);
		if (field != NULL)
		{
			scale(result, strtoll(field, NULL, 10));
		}
		else
		{
			strcpy(result, "n/a");
		}
		websWrite(wp, T("<td>%s</td>"), result);

		websWrite(wp, T("</tr>\n"));
	}

	close(skfd);
	fclose(fp);

	return 0;
}


int getMemTotalASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char buf[1024], *semiColon, *key, *value;
	FILE *fp = fopen(PROC_MEM_STATISTIC, "r");
	if(!fp){
		websWrite(wp, T("no proc?\n"));
		return -1;
	}

	while(fgets(buf, 1024, fp))
	{
		if(! (semiColon = strchr(buf, ':'))  )
			continue;
		*semiColon = '\0';
		key = buf;
		value = semiColon + 1;
		if(!strcmp(key, "MemTotal")){
			value = strip_space(value);
			websWrite(wp, T("%s"), value);
			fclose(fp);
			return 0;
		}
	}
	websWrite(wp, T(""));
	fclose(fp);

	return -1;
}

int getMemLeftASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char buf[1024], *semiColon, *key, *value;
	FILE *fp = fopen(PROC_MEM_STATISTIC, "r");
	if(!fp){
		websWrite(wp, T("no proc?\n"));
		return -1;
	}

	while(fgets(buf, 1024, fp)){
		if(! (semiColon = strchr(buf, ':'))  )
			continue;
		*semiColon = '\0';
		key = buf;
		value = semiColon + 1;
		if(!strcmp(key, "MemFree")){
			value = strip_space(value);
			websWrite(wp, T("%s"), value);
			fclose(fp);
			return 0;
		}
	}
	websWrite(wp, T(""));
	fclose(fp);
	return -1;
}

int getCpuUsageASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char buf[1024], *value;
	unsigned int i;

	float outd = 0;
	unsigned long int curBusy, curTotal = 0, deltaBusy, deltaTotal;

	union uCpuStats curCpuStats;

	FILE *fp = fopen(PROC_CPU_STATISTIC, "r");
	if(!fp){
		websWrite(wp, T("no proc?\n"));
		return -1;
	}

	if (fgets(buf, 1024, fp))
	{
		value = strtok(buf, " ");

		for (i = 0; i < 8; i++)
		{
			value = strtok(NULL, " ");
			if (value && isdigit(*value))
			{
				curCpuStats.arrData[i] = strtol(value, NULL, 10);

				curTotal += curCpuStats.arrData[i];
			}
			else
			{
				websWrite(wp, T("n/a"));
				fclose(fp);
				return -1;
			}

		}

		curBusy = curCpuStats.sepData.user + curCpuStats.sepData.nice + curCpuStats.sepData.system + curCpuStats.sepData.iowait + curCpuStats.sepData.irq + curCpuStats.sepData.softirq + curCpuStats.sepData.steal;

		if (!(curTotal > prevTotal) || !(curBusy > prevBusy))
		{
			websWrite(wp, T("n/a"));
			fclose(fp);
			return 0;
		}

		deltaBusy = (curBusy - prevBusy) * 100;
		deltaTotal = curTotal - prevTotal;

		prevTotal = curTotal;
		prevBusy = curBusy;

		outd = (float)deltaBusy / (float)deltaTotal;

		snprintf(buf, 16, "%.1f", outd);
		websWrite(wp, T("%s %%"), buf);
		fclose(fp);
		return 0;
	}
	websWrite(wp, T(""));
	fclose(fp);
	return -1;
}

static void LoadDefaultSettings(webs_t wp, char_t *path, char_t *query)
{
    system("fs nvramreset");
    system("fs restore");
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
	printf("Clear system log\n");
	doSystem("service syslog stop");
	truncate("/var/log/messages", 0);
	doSystem("service syslog start");

	websDone(wp, 200);
}

static void setuplog(webs_t wp, char_t *path, char_t *query)
{
	// Set-up parameters
	setupParameters(wp, service_syslog_flags, 1);

	// Restart syslog
	doSystem("service syslog restart");

	// Redirect if possible
	char_t *submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
#ifdef PRINT_DEBUG
	if (!submitUrl || !submitUrl[0])
		websDone(wp, 200);
	else
#endif
		websRedirect(wp, submitUrl);
}

#define LOG_MAX 32768
static void syslog(webs_t wp, char_t *path, char_t *query)
{
	FILE *fp = NULL;
	char *log;

	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\n"));
	websWrite(wp, WEBS_CACHE_CONTROL_STRING);
	websWrite(wp, T("\n"));

	// LOG_MAX 32768 - 1
	fp = popen("tail -c 32767 /var/log/messages", "r");

	if(!fp){
		websWrite(wp, "-1");
		goto error;
	}

	log = malloc(LOG_MAX * sizeof(char));
	if(!log){
		websWrite(wp, "-1");
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

void formDefineManagement(void)
{
	websFormDefine(T("setSysAdm"), setSysAdm);
	websFormDefine(T("setSysLang"), setSysLang);
	websFormDefine(T("NTP"), NTP);
#ifdef CONFIG_DATE
	websFormDefine(T("NTPSyncWithHost"), NTPSyncWithHost);
#endif
#ifdef CONFIG_USER_INADYN
	websFormDefine(T("DDNS"), DDNS);
#endif
	websAspDefine(T("getMemLeftASP"), getMemLeftASP);
	websAspDefine(T("getMemTotalASP"), getMemTotalASP);
	websAspDefine(T("getCpuUsageASP"), getCpuUsageASP);
	websAspDefine(T("getWANRxByteASP"), getWANRxByteASP);
	websAspDefine(T("getWANTxByteASP"), getWANTxByteASP);
	websAspDefine(T("getLANRxByteASP"), getLANRxByteASP);
	websAspDefine(T("getLANTxByteASP"), getLANTxByteASP);
	websAspDefine(T("getWANRxPacketASP"), getWANRxPacketASP);
	websAspDefine(T("getWANTxPacketASP"), getWANTxPacketASP);
	websAspDefine(T("getLANRxPacketASP"), getLANRxPacketASP);
	websAspDefine(T("getLANTxPacketASP"), getLANTxPacketASP);
	websAspDefine(T("getAllNICStatisticASP"), getAllNICStatisticASP);
	websFormDefine(T("LoadDefaultSettings"), LoadDefaultSettings);
#ifdef CONFIG_SYSLOGD
	websFormDefine(T("syslog"), syslog);
	websFormDefine(T("clearlog"), clearlog);
	websFormDefine(T("setuplog"), setuplog);
#endif
#ifdef CONFIG_USER_WSC
	formDefineWPS();
#endif
}
