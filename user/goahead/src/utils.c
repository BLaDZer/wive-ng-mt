/* vi: set sw=4 ts=4 sts=4: */
/*
 *	utils.c -- System Utilities
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 */
#include	<time.h>
#include	<signal.h>
#include	<sys/ioctl.h>
#include	<arpa/inet.h>
#include	<asm/types.h>
#include	<linux/if.h>
#include	<sys/ioctl.h>
#include	<sys/time.h>
#include	<sys/socket.h>
#include	<sys/sysinfo.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>

#include	"utils.h"
#include	"helpers.h"
#include	"station.h"

/*
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field'
 *            field - parameter name in nvram
 * description: read general configurations from nvram
 *              if value is NULL -> ""
 */
static int getCfgGeneral(int eid, webs_t wp, int argc, char_t **argv)
{
	int type;
	char_t *field;
	char *value;

	if (ejArgs(argc, argv, T("%d %s"), &type, &field) < 2)
		return websWrite(wp, T("Insufficient args\n"));

	value = nvram_get(RT2860_NVRAM, field);

	if ((!value) && (strcmp(field, "Language") == 0)) {
	    syslog(LOG_ERR, "Unknown lang %s. Set lang to en, %s\n", value, __FUNCTION__);
	    value = "en";
	}

        if (type == 1) {
                if (!value)
                        return websWrite(wp, T(""));
                return websWrite(wp, T("%s"), value);
        }

	if (!value)
	    ejSetResult(eid, "");
	else
	    ejSetResult(eid, value);

    return 0;
}

/*
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field'
 *            field - parameter name in nvram
 * description: read general configurations from nvram
 *              if value is NULL -> ""
 */
static int getCfgGeneralHTML(int eid, webs_t wp, int argc, char_t **argv)
{
	int type;
	char_t *field;
	char *value;

	if (ejArgs(argc, argv, T("%d %s"), &type, &field) < 2)
		return websWrite(wp, T("Insufficient args\n"));

	value = nvram_get(RT2860_NVRAM, field);

	if ((!value) && (strcmp(field, "Language") == 0))
	{
		syslog(LOG_ERR, "Unknown lang %s. Set lang to en, %s\n", value, __FUNCTION__);
		value = "en";
	}

	if (type == 1)
	{
		if (!value)
			return websWrite(wp, T(""));

		int result = 0;
		for (; *value != '\0'; value++)
		{
			switch (*value)
			{
				case '&':
					result += websWrite(wp, T("&amp;"));
					break;
				case '\"':
					result += websWrite(wp, T("&quot;"));
					break;
				case '<':
					result += websWrite(wp, T("&lt;"));
					break;
				case '>':
					result += websWrite(wp, T("&gt;"));
					break;
				default:
					result += websWrite(wp, T("%c"), *value);
					break;
			}
		}
		return result;
	}

	if (!value)
		ejSetResult(eid, "");
	else
		ejSetResult(eid, value);

	return 0;
}

/*
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field'
 *            field - parameter name in nvram
 *            idx - index of nth
 * description: read general configurations from nvram (if value is NULL -> "")
 *              parse it and return the Nth value delimited by semicolon
 */
static int getCfgNthGeneral(int eid, webs_t wp, int argc, char_t **argv)
{
	int type, idx;
	char_t *field;
	char *value;
	char *nth;

	if (ejArgs(argc, argv, T("%d %s %d"), &type, &field, &idx) < 3) {
		return websWrite(wp, T("Insufficient args\n"));
	}
	value = (char *) nvram_get(RT2860_NVRAM, field);
	if (1 == type) {
		if (NULL == value)
			return websWrite(wp, T(""));
		nth = getNthValue(idx, value);
		if (NULL == nth)
			return websWrite(wp, T(""));
		return websWrite(wp, T("%s"), nth);
	}
	if (NULL == value)
		ejSetResult(eid, "");
	nth = getNthValue(idx, value);
	if (NULL == nth)
		ejSetResult(eid, "");
	ejSetResult(eid, value);
	return 0;
}

/*
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field'
 *            field - parameter name in nvram
 * description: read general configurations from nvram
 *              if value is NULL -> "0"
 */
static int getCfgZero(int eid, webs_t wp, int argc, char_t **argv)
{
	int type;
	char_t *field;
	char *value;

	if (ejArgs(argc, argv, T("%d %s"), &type, &field) < 2) {
		return websWrite(wp, T("Insufficient args\n"));
	}
	value = (char *) nvram_get(RT2860_NVRAM, field);
	if (1 == type) {
		if (!strcmp(value, ""))
			return websWrite(wp, T("0"));
		return websWrite(wp, T("%s"), value);
	}
	if (!strcmp(value, ""))
		ejSetResult(eid, "0");
	ejSetResult(eid, value);
	return 0;
}

/*
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field'
 *            field - parameter name in nvram
 *            idx - index of nth
 * description: read general configurations from nvram (if value is NULL -> "0")
 *              parse it and return the Nth value delimited by semicolon
 */
static int getCfgNthZero(int eid, webs_t wp, int argc, char_t **argv)
{
	int type, idx;
	char_t *field;
	char *value;
	char *nth;

	if (ejArgs(argc, argv, T("%d %s %d"), &type, &field, &idx) < 3) {
		return websWrite(wp, T("Insufficient args\n"));
	}
	value = (char *) nvram_get(RT2860_NVRAM, field);
	if (1 == type) {
		if (!strcmp(value, ""))
			return websWrite(wp, T("0"));
		nth = getNthValue(idx, value);
		if (NULL == nth)
			return websWrite(wp, T("0"));
		return websWrite(wp, T("%s"), nth);
	}
	if (!strcmp(value, ""))
		ejSetResult(eid, "0");
	nth = getNthValue(idx, value);
	if (NULL == nth)
		ejSetResult(eid, "0");
	ejSetResult(eid, value);
	return 0;
}

static int getLangBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t *lang;

	if (ejArgs(argc, argv, T("%s"), &lang) < 1) {
		return websWrite(wp, T("0"));
	}
	if (!strncmp(lang, "en", 3))
		return websWrite(wp, T("1"));
	else if (!strncmp(lang, "ru", 3))
#ifdef CONFIG_USER_GOAHEAD_LANG_RU
		return websWrite(wp, T("1"));
#else
		return websWrite(wp, T("0"));
#endif
	else	return websWrite(wp, T("0"));
}

/*
 * description: write the current system platform accordingly
 */
static int getPlatform(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(CONFIG_RALINK_MT7620) && defined(CONFIG_RAETH_ESW)
#if defined(CONFIG_RT_SECOND_IF_MT7610E)
    return websWrite(wp, T("MT7620 2T2R 2.4GHz, MT7610 1T1R 5GHz, 100FDX"));
#elif defined(CONFIG_RT_SECOND_IF_MT7612E)
    return websWrite(wp, T("MT7620 2T2R 2.4GHz, MT7612 2T2R 5GHz, 100FDX"));
#else
    return websWrite(wp, T("MT7620 2T2R 2.4GHz, 100FDX"));
#endif
#elif defined(CONFIG_RALINK_MT7621) && defined(CONFIG_MT7530_GSW)
    return websWrite(wp, T("MT7621 1000FDX MT76x2 2T2R dualband"));
#else
    return websWrite(wp, T("Unknown switch mode"));
#endif
    return 0;
}

static int getStationBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(CONFIG_RT2860V2_STA) || defined(CONFIG_RT2860V2_STA_MODULE) || defined(CONFIG_MT76X2_STA) || defined(CONFIG_MT76X2_STA_MODULE)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int gigaphy(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(CONFIG_RALINK_MT7621)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

/*
 * description: write RT288x SDK version
 */
static int getSdkVersion(int eid, webs_t wp, int argc, char_t **argv)
{
	return websWrite(wp, T("%s"), RT288X_SDK_VERSION);
}

/*
 * description: write System Uptime
 */
static int getSysUptime(int eid, webs_t wp, int argc, char_t **argv)
{
	struct sysinfo sinf;

	if (sysinfo(&sinf) == 0)
	{
	    return websWrite(wp, T("%d day%s, %d hour%s, %d min%s, %d sec%s"),
			sinf.uptime/(unsigned)86400, ((sinf.uptime/(unsigned)86400) == 1)? "" : "s",
    			(sinf.uptime%(unsigned)86400)/(unsigned)3600, (((sinf.uptime%(unsigned)86400)/(unsigned)3600) == 1)? "" : "s",
			(sinf.uptime%(unsigned)3600)/(unsigned)60, (((sinf.uptime%(unsigned)3600)/(unsigned)60) == 1)? "" : "s",
			sinf.uptime%(unsigned)60, ((sinf.uptime%(unsigned)60) == 1)? "" : "s");
	} else
	    return -1;
}

/*
 * description: write Current date, time
 */
static int getSysDateTime(int eid, webs_t wp, int argc, char_t **argv)
{
	struct tm *utime;
	time_t usecs;

	time(&usecs);
	utime = localtime(&usecs);
	if (utime == NULL)
	    return -1;

	return websWrite(wp, T("%02u:%02u:%02u %02u.%02u.%04u"),
				utime->tm_hour, utime->tm_min, utime->tm_sec,
                utime->tm_mday, (utime->tm_mon + 1), (1900 + utime->tm_year));
}

/* goform/setOpMode */
static void setOpMode(webs_t wp, char_t *path, char_t *query)
{
	char *mode, *old_mode;

	nvram_init(RT2860_NVRAM);

	/* get modes */
	mode = websGetVar(wp, T("opMode"), T("0"));
	old_mode = nvram_get(RT2860_NVRAM, "OperationMode");

	/* new OperationMode */
	if (strncmp(mode, old_mode, 2))
	{
		nvram_bufset(RT2860_NVRAM, "OperationMode", mode);
		/* from or to ap client mode */
		if (!strncmp(mode, "3", 2))
			nvram_bufset(RT2860_NVRAM, "ApCliEnable", "1");
		else if (!strncmp(old_mode, "3", 2)) {
			/* from apcli to other mode need drop bridge only flag to */
			nvram_bufset(RT2860_NVRAM, "ApCliEnable", "0");
			nvram_bufset(RT2860_NVRAM, "ApCliBridgeOnly", "0");
		}
		nvram_commit(RT2860_NVRAM);
	}
	nvram_close(RT2860_NVRAM);

	/* Output timer for reloading */
	outputTimerForReload(wp, "", 80000);

	/* Reboot */
	reboot_now();
}

static void setWanPort(webs_t wp, char_t *path, char_t *query)
{
	int i;
	char w_name[20];
	char* w_port = websGetVar(wp, T("wan_port"), T("0"));
	char* l_port = websGetVar(wp, T("lan_port"), T("near"));
	char* tv_port = websGetVar(wp, T("tv_stbEnabled"), T("off"));
	char* sip_port = websGetVar(wp, T("sip_stbEnabled"), T("off"));
	char* tv_port_mcast = websGetVar(wp, T("tv_stbMcast"), T("off"));
	char* sip_port_mcast = websGetVar(wp, T("sip_stbMcast"), T("off"));
	char* tv_portVLAN = websGetVar(wp, T("tv_stbVLAN"), T(""));
	char* sip_portVLAN = websGetVar(wp, T("sip_stbVLAN"), T(""));

	nvram_init(RT2860_NVRAM);

	/* Set-up WAN port */
	if ((w_port != NULL) && (strlen(w_port) == 1))
	{
		if ((w_port[0] >= '0') && (w_port[0] <= '4'))
			nvram_bufset(RT2860_NVRAM, "wan_port", w_port);
	}

	/* Set-up first LAN port */
	if ((l_port != NULL) && ((strcmp(l_port, "near") == 0) || (strcmp(l_port, "distant") == 0)))
		nvram_bufset(RT2860_NVRAM, "lan_port", l_port);

	/* Now test values */
	for (i=1; i<=5; i++)
	{
		snprintf(w_name, sizeof(w_name), "port%d_swmode", i);
		w_port = websGetVar(wp, w_name, T("auto"));
		if ((w_port != NULL) && (strlen(w_port)>0))
			nvram_bufset(RT2860_NVRAM, w_name, w_port);
	}

	nvram_bufset(RT2860_NVRAM, "tv_port", (strcmp(tv_port, "on")==0) ? "1" : "0");
	nvram_bufset(RT2860_NVRAM, "sip_port", (strcmp(sip_port, "on")==0) ? "1" : "0");
	nvram_bufset(RT2860_NVRAM, "tv_port_mcast", (strcmp(tv_port_mcast, "on")==0) ? "1" : "0");
	nvram_bufset(RT2860_NVRAM, "sip_port_mcast", (strcmp(sip_port_mcast, "on")==0) ? "1" : "0");
	nvram_bufset(RT2860_NVRAM, "tv_portVLAN", tv_portVLAN);
	nvram_bufset(RT2860_NVRAM, "sip_portVLAN", sip_portVLAN);

	/* Commit & close NVRAM */
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	char_t *reboot_flag = websGetVar(wp, T("reboot"), T("0"));
	char_t *submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	if (CHK_IF_DIGIT(reboot_flag, 1)) {
		/* Output timer for reloading */
		outputTimerForReload(wp, "" /* submitUrl */, 80000);

		/* Reboot */
		reboot_now();
	} else {
		websRedirect(wp, submitUrl);
	}
}

/* goform/reboot */
static void reboot_web(webs_t wp, char_t *path, char_t *query)
{
	/* Output timer for reloading */
	outputTimerForReload(wp, "", 80000);

	/* only by save and reboot logic must save rwfs */
	doSystem("fs save > /dev/null 2>&1");

	/* Reboot */
	reboot_now();
}

/*********************************************************************
 * Web Related Utilities
 */
void formDefineUtilities(void)
{
	websAspDefine(T("getCfgGeneral"), getCfgGeneral);
	websAspDefine(T("getCfgGeneralHTML"), getCfgGeneralHTML);
	websAspDefine(T("getCfgNthGeneral"), getCfgNthGeneral);
	websAspDefine(T("getCfgZero"), getCfgZero);
	websAspDefine(T("getCfgNthZero"), getCfgNthZero);
	websAspDefine(T("getLangBuilt"), getLangBuilt);
	websAspDefine(T("getPlatform"), getPlatform);
	websAspDefine(T("getStationBuilt"), getStationBuilt);
	websAspDefine(T("getSdkVersion"), getSdkVersion);
	websAspDefine(T("getSysUptime"), getSysUptime);
	websAspDefine(T("getSysDateTime"), getSysDateTime);
	websFormDefine(T("setOpMode"), setOpMode);
	websFormDefine(T("setWanPort"), setWanPort);
	websFormDefine(T("reboot"), reboot_web);
	websAspDefine(T("gigaphy"), gigaphy);
}
