/* vi: set sw=4 ts=4 sts=4: */
/*
 *	wireless.c -- Wireless Settings 
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: wireless.c,v 1.113.2.3 2009-04-17 03:37:53 chhung Exp $
 */

#include	<stdlib.h>
#include	<sys/ioctl.h>
#include	<arpa/inet.h>
#include	<linux/types.h>
#include	<linux/socket.h>
#include	<linux/if.h>

#include	"utils.h"
#include	"internet.h"
#include 	"helpers.h"

static int  getWlanApcliBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int  getWlanWdsBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int  getWlanM2UBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int  getGreenAPBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int  get802_1XBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int  getVideoTurbineBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int  getIdsEnableBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int  getWlanChannel(int eid, webs_t wp, int argc, char_t **argv);
static int  getWlanChannelAC(int eid, webs_t wp, int argc, char_t **argv);
static int  getWlanCurrentMac(int eid, webs_t wp, int argc, char_t **argv);
static int  getWlanCurrentMacAC(int eid, webs_t wp, int argc, char_t **argv);
static int  getWlanStaInfo(int eid, webs_t wp, int argc, char_t **argv);
static int  getWlan11aChannels(int eid, webs_t wp, int argc, char_t **argv);
static int  getWlan11gChannels(int eid, webs_t wp, int argc, char_t **argv);
static int  listCountryCodes(int eid, webs_t wp, int argc, char_t **argv);
static int  is3t3r(int eid, webs_t wp, int argc, char_t **argv);
static int  is5gh_support(int eid, webs_t wp, int argc, char_t **argv);
static int  is5gh_1t1r(int eid, webs_t wp, int argc, char_t **argv);
static int  getTXBFBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int  getMaxStaNum(int eid, webs_t wp, int argc, char_t **argv);
static int  getBSSIDNum(int eid, webs_t wp, int argc, char_t **argv);
static int  getBandSteeringBuilt(int eid, webs_t wp, int argc, char_t **argv);
static void wirelessBasic(webs_t wp, char_t *path, char_t *query);
static void disconnectSta(webs_t wp, char_t *path, char_t *query);
static void wirelessAdvanced(webs_t wp, char_t *path, char_t *query);
static void wirelessWds(webs_t wp, char_t *path, char_t *query);
static void wirelessApcli(webs_t wp, char_t *path, char_t *query);
static void wirelessGetSecurity(webs_t wp, char_t *path, char_t *query);
static void APSecurity(webs_t wp, char_t *path, char_t *query);
static void APDeleteAccessPolicyList(webs_t wp, char_t *path, char_t *query);

static int default_shown_mbssid[3]  = {0,0,0};

typedef struct country_code_t
{
	const char *iso;
	const char *name;
} country_code_t;

const country_code_t country_codes[] =
{
	{ "AL", "ALBANIA" },
	{ "DZ", "ALGERIA" },
	{ "AR", "ARGENTINA" },
	{ "AM", "ARMENIA" },
	{ "AU", "AUSTRALIA" },
	{ "AT", "AUSTRIA" },
	{ "AZ", "AZERBAIJAN" },
	{ "BH", "BAHRAIN" },
	{ "BY", "BELARUS" },
	{ "BE", "BELGIUM" },
	{ "BZ", "BELIZE" },
	{ "BO", "BOLIVIA" },
	{ "BR", "BRAZIL" },
	{ "BN", "BRUNEI DARUSSALAM" },
	{ "BG", "BULGARIA" },
	{ "CA", "CANADA" },
	{ "CL", "CHILE" },
	{ "CN", "CHINA" },
	{ "CO", "COLOMBIA" },
	{ "CR", "COSTA RICA" },
	{ "HR", "CROATIA" },
	{ "CY", "CYPRUS" },
	{ "CZ", "CZECH REPUBLIC" },
	{ "DK", "DENMARK" },
	{ "DO", "DOMINICAN REPUBLIC" },
	{ "EC", "ECUADOR" },
	{ "EG", "EGYPT" },
	{ "SV", "EL SALVADOR" },
	{ "EE", "ESTONIA" },
	{ "FI", "FINLAND" },
	{ "FR", "FRANCE" },
	{ "GE", "GEORGIA" },
	{ "DE", "GERMANY" },
	{ "GR", "GREECE" },
	{ "GT", "GUATEMALA" },
	{ "HN", "HONDURAS" },
	{ "HK", "HONG KONG" },
	{ "HU", "HUNGARY" },
	{ "IS", "ICELAND" },
	{ "IN", "INDIA" },
	{ "ID", "INDONESIA" },
	{ "IR", "IRAN" },
	{ "IE", "IRELAND" },
	{ "IL", "ISRAEL" },
	{ "IT", "ITALY" },
	{ "JP", "JAPAN" },
	{ "JO", "JORDAN" },
	{ "KZ", "KAZAKHSTAN" },
	{ "KP", "KOREA DEMOCRATIC PEOPLE'S REPUBLIC OF" },
	{ "KR", "KOREA REPUBLIC OF" },
	{ "KW", "KUWAIT" },
	{ "LV", "LATVIA" },
	{ "LB", "LEBANON" },
	{ "LI", "LIECHTENSTEIN" },
	{ "LT", "LITHUANIA" },
	{ "LU", "LUXEMBOURG" },
	{ "MO", "MACAU" },
	{ "MK", "MACEDONIA" },
	{ "MY", "MALAYSIA" },
	{ "MX", "MEXICO" },
	{ "MC", "MONACO" },
	{ "MA", "MOROCCO" },
	{ "NL", "NETHERLANDS" },
	{ "NZ", "NEW ZEALAND" },
	{ "NO", "NORWAY" },
	{ "OM", "OMAN" },
	{ "PK", "PAKISTAN" },
	{ "PA", "PANAMA" },
	{ "PE", "PERU" },
	{ "PH", "PHILIPPINES" },
	{ "PL", "POLAND" },
	{ "PT", "PORTUGAL" },
	{ "PR", "PUERTO RICO" },
	{ "QA", "QATAR" },
	{ "RO", "ROMANIA" },
	{ "RU", "RUSSIAN FEDERATION" },
	{ "SA", "SAUDI ARABIA" },
	{ "SG", "SINGAPORE" },
	{ "SK", "SLOVAKIA" },
	{ "SI", "SLOVENIA" },
	{ "ZA", "SOUTH AFRICA" },
	{ "ES", "SPAIN" },
	{ "SE", "SWEDEN" },
	{ "CH", "SWITZERLAND" },
	{ "SY", "SYRIAN ARAB REPUBLIC" },
	{ "TW", "TAIWAN" },
	{ "TH", "THAILAND" },
	{ "TT", "TRINIDAD AND TOBAGO" },
	{ "TN", "TUNISIA" },
	{ "TR", "TURKEY" },
	{ "UA", "UKRAINE" },
	{ "AE", "UNITED ARAB EMIRATES" },
	{ "GB", "UNITED KINGDOM" },
	{ "US", "UNITED STATES" },
	{ "UY", "URUGUAY" },
	{ "UZ", "UZBEKISTAN" },
	{ "VE", "VENEZUELA" },
	{ "VN", "VIET NAM" },
	{ "YE", "YEMEN" },
	{ "ZW", "ZIMBABWE" },
	{ NULL, NULL }
};

void formDefineWireless(void)
{
	websAspDefine(T("getVideoTurbineBuilt"), getVideoTurbineBuilt);
	websAspDefine(T("getIdsEnableBuilt"), getIdsEnableBuilt);
	websAspDefine(T("getWlan11aChannels"), getWlan11aChannels);
	websAspDefine(T("getWlan11gChannels"), getWlan11gChannels);
	websAspDefine(T("getWlanApcliBuilt"), getWlanApcliBuilt);
	websAspDefine(T("getWlanWdsBuilt"), getWlanWdsBuilt);
	websAspDefine(T("getWlanChannel"), getWlanChannel);
	websAspDefine(T("getWlanChannelAC"), getWlanChannelAC);
	websAspDefine(T("getWlanCurrentMac"), getWlanCurrentMac);
	websAspDefine(T("getWlanCurrentMacAC"), getWlanCurrentMacAC);
	websAspDefine(T("getWlanStaInfo"), getWlanStaInfo);
	websAspDefine(T("get802_1XBuilt"), get802_1XBuilt);
	websAspDefine(T("getWlanM2UBuilt"), getWlanM2UBuilt);
	websAspDefine(T("getGreenAPBuilt"), getGreenAPBuilt);
	websAspDefine(T("listCountryCodes"), listCountryCodes);
	websAspDefine(T("is3t3r"), is3t3r);
	websAspDefine(T("is5gh_support"), is5gh_support);
	websAspDefine(T("is5gh_1t1r"), is5gh_1t1r);
	websAspDefine(T("getTXBFBuilt"), getTXBFBuilt);
	websAspDefine(T("getMaxStaNum"), getMaxStaNum);
	websAspDefine(T("getBSSIDNum"), getBSSIDNum);
	websAspDefine(T("getBandSteeringBuilt"), getBandSteeringBuilt);
	websFormDefine(T("wirelessBasic"), wirelessBasic);
	websFormDefine(T("disconnectSta"), disconnectSta);
	websFormDefine(T("wirelessAdvanced"), wirelessAdvanced);
	websFormDefine(T("wirelessWds"), wirelessWds);
	websFormDefine(T("wirelessApcli"), wirelessApcli);
	websFormDefine(T("wirelessGetSecurity"), wirelessGetSecurity);
	websFormDefine(T("APSecurity"), APSecurity);
	websFormDefine(T("APDeleteAccessPolicyList"), APDeleteAccessPolicyList);
}

static int getWlanApcliBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(CONFIG_RT2860V2_AP_APCLI) || defined(CONFIG_MT7610_AP_APCLI) || defined(CONFIG_MT76X2_AP_APCLI)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getWlanWdsBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(CONFIG_RT2860V2_AP_WDS) || defined(CONFIG_MT7610_AP_WDS) || defined(CONFIG_MT76X2_AP_WDS)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int listCountryCodes(int eid, webs_t wp, int argc, char_t **argv)
{
	const country_code_t *codes = country_codes;
	char *c_code = nvram_get(RT2860_NVRAM, "CountryCode");

	websWrite(wp, T("\t<option value=\"NONE\">NONE</option>\n"));

	while (codes->iso != NULL)
	{
		websWrite(wp, T("\t<option value=\"%s\"%s>%s (%s)</option>\n"),
			codes->iso,
			(strcmp(c_code, codes->iso) == 0) ? " selected" : "",
			codes->name, codes->iso);
		codes++;
	}

	return 0;
}

/*
 * description: write 802.11a channels in <select> tag
 */
static int getWlan11aChannels(int eid, webs_t wp, int argc, char_t **argv)
{
	int  idx = 0, channel;
	const char *value = nvram_bufget(RT2860_NVRAM,"CountryRegionABand");
	const char *channel_s = nvram_bufget(RT2860_NVRAM, "ChannelINIC");

	channel = (channel_s == NULL)? 0 : atoi(channel_s);
	if ((value == NULL) || (strcmp(value, "") == 0) ||
		(strcmp(value, "7") == 0)) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~140 */
		for (idx = 16; idx < 27; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~165 */
		for (idx = 28; idx < 33; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "0") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~165 */
		for (idx = 28; idx < 33; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "1") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~140 */
		for (idx = 16; idx < 27; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
	} else if (strcmp(value, "2") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
	} else if (strcmp(value, "3") == 0) {
		/* 52~64 */
		for (idx = 4; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "4") == 0) {
		/* 149~165 */
		for (idx = 28; idx < 33; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "5") == 0) {
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "6") == 0) {
		/* 36~48 */
		for (idx = 0; idx < 4; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
	} else if (strcmp(value, "8") == 0) {
		/* 52~64 */
		for (idx = 4; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
	} else if (strcmp(value, "9") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~116 */
		for (idx = 16; idx < 21; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 132~140 */
		for (idx = 24; idx < 27; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~165 */
		for (idx = 28; idx < 33; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "10") == 0) {
		/* 36~48 */
		for (idx = 0; idx < 4; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~165 */
		for (idx = 28; idx < 33; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	} else if (strcmp(value, "11") == 0) {
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~120 */
		for (idx = 16; idx < 22; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			websWrite(wp, T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
	}

	return 0;
}

/*
 * description: write 802.11b/g/n channels in <select> tag
 */
static int getWlan11gChannels(int eid, webs_t wp, int argc, char_t **argv)
{
    int idx = 0, channel;
    char *channel_s = nvram_get(RT2860_NVRAM, "Channel");

    channel = (channel_s == NULL)? 0 : atoi(channel_s);

    for (idx = 0; idx < 13; idx++)
	websWrite(wp, T("%s%d %s>%d%s%d%s"), 
		"<option value=", idx+1, (idx+1 == channel)? "selected" : "", 2412+5*idx, "MHz (Channel ", idx+1, ")</option>");

    return websWrite(wp, T("<option value=14 %s>2484MHz (Channel 14)</option>\n"),(14 == channel)? "selected" : "");
}

/*
 * description: write channel number or 0 if auto-select
 */
static int getWlanChannel(int eid, webs_t wp, int argc, char_t **argv)
{
	char *value = nvram_get(RT2860_NVRAM, "AutoChannelSelect");

	if (NULL == value)
		return websWrite(wp, T("9"));
	if (!strncmp(value, "1", 2))
		return websWrite(wp, T("0"));

	value = nvram_get(RT2860_NVRAM, "Channel");
	if (NULL == value)
		return websWrite(wp, T("9"));
	else
		return websWrite(wp, T("%s"), value);
}

static int getWlanChannelAC(int eid, webs_t wp, int argc, char_t **argv)
{
#ifndef CONFIG_RT_SECOND_IF_NONE
	char *value = nvram_get(RT2860_NVRAM, "AutoChannelSelectINIC");

	if (NULL == value)
		return websWrite(wp, T("48"));
	if (!strncmp(value, "1", 2))
		return websWrite(wp, T("0"));

	value = nvram_get(RT2860_NVRAM, "ChannelINIC");
	if (NULL == value)
		return websWrite(wp, T("48"));
	else
		return websWrite(wp, T("%s"), value);
#else
	return websWrite(wp, T("0"));
#endif
}

/*
 * description: write MAC address from interface 'ra0'
 */
static int getWlanCurrentMac(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_hw[18] = {0};

	if (getIfMac("ra0", if_hw) == -1)
		return websWrite(wp, T("00:00:00:00:00:00"));
	return websWrite(wp, T("%s"), if_hw);
}

static int getWlanCurrentMacAC(int eid, webs_t wp, int argc, char_t **argv)
{
#ifndef CONFIG_RT_SECOND_IF_NONE
	char if_hw[18] = {0};

	if (getIfMac("rai0", if_hw) == -1)
		return websWrite(wp, T("00:00:00:00:00:00"));
	return websWrite(wp, T("%s"), if_hw);
#else
	return websWrite(wp, T("00:00:00:00:00:00"));
#endif
}

static int getWlanStaInfo(int eid, webs_t wp, int argc, char_t **argv)
{
	int i, s, err = 0;
	struct iwreq iwr;
	RT_802_11_MAC_TABLE table = {0};
	s = socket(AF_INET, SOCK_DGRAM, 0);
	strncpy(iwr.ifr_name, "ra0", IFNAMSIZ);
	iwr.u.data.pointer = (caddr_t) &table;

	if (s < 0) {
		printf("goahead: first wlan: ioctl sock failed!");
		err = -1;
		goto out24;
	}

	if (ioctl(s, RTPRIV_IOCTL_GET_MAC_TABLE, &iwr) < 0) {
		printf("goahead: first wlan: ioctl -> RTPRIV_IOCTL_GET_MAC_TABLE failed!");
		err = -1;
		goto out24;
	}

	for (i = 0; i < table.Num; i++) {
	    RT_802_11_MAC_ENTRY *pe = &(table.Entry[i]);

	    // MAC Address
	    websWrite(wp, T("<tr><td bgcolor=\"#c4d7ff\">%02X:%02X:%02X:%02X:%02X:%02X</td>"),
			pe->Addr[0], pe->Addr[1], pe->Addr[2], pe->Addr[3], pe->Addr[4], pe->Addr[5]);

	    // Connection Time
	    websWrite(wp, T("<td>%02u:%02u:%02u</td>"), (pe->ConnectedTime / (unsigned)3600), ((pe->ConnectedTime % (unsigned)3600) / (unsigned)60), (pe->ConnectedTime % (unsigned)60));

	    // AID, Power Save mode, MIMO Power Save
	    websWrite(wp, T("<td>%d</td><td>%s</td><td>%d</td>"), pe->Aid, (pe->Psm == 0)? "NO " : "YES", pe->MimoPs);

	    // TX Rate
	    websWrite(wp, T("<td>%d</td>"), pe->TxRate.field.MCS);

	    // Bandwith
	    switch (pe->TxRate.field.BW) {
		case 0: websWrite(wp, T("<td>%s</td>"), "20MHz"); break;
		case 1: websWrite(wp, T("<td>%s</td>"), "40MHz"); break;
		case 2: websWrite(wp, T("<td>%s</td>"), "80MHz"); break;
		case 3: websWrite(wp, T("<td>%s</td>"), "BOTH"); break;
		//case 4: websWrite(wp, T("<td>%s</td>"), "10MHz"); break;
		default : websWrite(wp, T("<td>%s</td>"), "20MHz");
	    }

	    // SGI/STBC/LDPC
	    websWrite(wp, T("<td>%s</td><td>%s</td><td>%s</td>"),
		(pe->TxRate.field.ShortGI == 0)? "NO " : "YES",
		(pe->TxRate.field.STBC == 0)? "NO " : "YES",
		(pe->TxRate.field.ldpc == 0)? "NO " : "YES");

	    // HT/VHT Modes
	    switch (pe->TxRate.field.MODE) {
		case 0: websWrite(wp, T("<td>%s</td>"), "CCK"); break;
		case 1: websWrite(wp, T("<td>%s</td>"), "OFDM"); break;
		case 2: websWrite(wp, T("<td>%s</td>"), "HT"); break;
		case 3: websWrite(wp, T("<td>%s</td>"), "HTGRF"); break;
		case 4: websWrite(wp, T("<td>%s</td>"), "VHT"); break;
		default : websWrite(wp, T("<td>%s</td>"), "");
	    }

	    // RSSI
#if defined(CONFIG_RT_FIRST_IF_MT7610E)
	    websWrite(wp, T("<td>%d</td>"), (int)(pe->AvgRssi0));
#elif defined(CONFIG_RALINK_MT7620) || defined(CONFIG_RT_FIRST_IF_MT7602E)
	    websWrite(wp, T("<td>%d,%d</td>"), (int)(pe->AvgRssi0), (int)(pe->AvgRssi1));
#else
	    websWrite(wp, T("<td>%d,%d,%d</td>"), (int)(pe->AvgRssi0), (int)(pe->AvgRssi1), (int)(pe->AvgRssi2));
#endif
	    // Action
	    websWrite(wp, T("<td><input type=\"button\" id=\"disconnect\" value=\"disconnect\" onclick=\"doDisconnectSta(this.form, '%02X:%02X:%02X:%02X:%02X:%02X')\"></td>"),
			pe->Addr[0], pe->Addr[1], pe->Addr[2], pe->Addr[3], pe->Addr[4], pe->Addr[5]);
	    websWrite(wp, T("</tr>"));
	}
out24:
	close(s);
#ifndef CONFIG_RT_SECOND_IF_NONE
	/* second radio module */
	s = socket(AF_INET, SOCK_DGRAM, 0);
	strncpy(iwr.ifr_name, "rai0", IFNAMSIZ);
	iwr.u.data.pointer = (caddr_t) &table;

	if (s < 0) {
		printf("goahead: second wlan: ioctl sock failed!");
		err = -1;
		goto out5;
	}

	if (ioctl(s, RTPRIV_IOCTL_GET_MAC_TABLE, &iwr) < 0) {
		printf("goahead: second wlan: ioctl -> RTPRIV_IOCTL_GET_MAC_TABLE failed!");
		err = -1;
		goto out5;
	}

	for (i = 0; i < table.Num; i++) {
	    RT_802_11_MAC_ENTRY *pe = &(table.Entry[i]);

	    // MAC Address
	    websWrite(wp, T("<tr><td bgcolor=\"#c4ffc4\">%02X:%02X:%02X:%02X:%02X:%02X</td>"),
			pe->Addr[0], pe->Addr[1], pe->Addr[2], pe->Addr[3], pe->Addr[4], pe->Addr[5]);

	    // Connection Time
	    websWrite(wp, T("<td>%02u:%02u:%02u</td>"), (pe->ConnectedTime / (unsigned)3600), ((pe->ConnectedTime % (unsigned)3600) / (unsigned)60), (pe->ConnectedTime % (unsigned)60));

	    // AID, Power Save mode, MIMO Power Save
	    websWrite(wp, T("<td>%d</td><td>%s</td><td>%d</td>"), pe->Aid, (pe->Psm == 0)? "NO " : "YES", pe->MimoPs);

	    // TX Rate
	    websWrite(wp, T("<td>%d</td>"), pe->TxRate.field.MCS);

	    // Bandwith
	    switch (pe->TxRate.field.BW) {
		case 0: websWrite(wp, T("<td>%s</td>"), "20MHz"); break;
		case 1: websWrite(wp, T("<td>%s</td>"), "40MHz"); break;
		case 2: websWrite(wp, T("<td>%s</td>"), "80MHz"); break;
		case 3: websWrite(wp, T("<td>%s</td>"), "BOTH"); break;
		//case 4: websWrite(wp, T("<td>%s</td>"), "10MHz"); break;
		default : websWrite(wp, T("<td>%s</td>"), "20MHz");
	    }

	    // SGI/STBC/LDPC
	    websWrite(wp, T("<td>%s</td><td>%s</td><td>%s</td>"),
		(pe->TxRate.field.ShortGI == 0)? "NO " : "YES",
		(pe->TxRate.field.STBC == 0)? "NO " : "YES",
		(pe->TxRate.field.ldpc == 0)? "NO " : "YES");

	    // HT/VHT Modes
	    switch (pe->TxRate.field.MODE) {
		case 0: websWrite(wp, T("<td>%s</td>"), "CCK"); break;
		case 1: websWrite(wp, T("<td>%s</td>"), "OFDM"); break;
		case 2: websWrite(wp, T("<td>%s</td>"), "HT"); break;
		case 3: websWrite(wp, T("<td>%s</td>"), "HTGRF"); break;
		case 4: websWrite(wp, T("<td>%s</td>"), "VHT"); break;
		default : websWrite(wp, T("<td>%s</td>"), "");
	    }

	    // RSSI
#if defined(CONFIG_MT7610_AP) || defined(CONFIG_RT_SECOND_IF_MT7610E)
	    websWrite(wp, T("<td>%d</td>"), (int)(pe->AvgRssi0));
#elif defined(CONFIG_RT_SECOND_IF_MT7612E)
	    websWrite(wp, T("<td>%d,%d</td>"), (int)(pe->AvgRssi0), (int)(pe->AvgRssi1));
#else
	    websWrite(wp, T("<td>%d,%d,%d</td>"), (int)(pe->AvgRssi0), (int)(pe->AvgRssi1), (int)(pe->AvgRssi2));
#endif
	    // Action
	    websWrite(wp, T("<td><input type=\"button\" id=\"disconnect\" value=\"disconnect\" onclick=\"doDisconnectSta(this.form, '%02X:%02X:%02X:%02X:%02X:%02X')\"></td>"),
			pe->Addr[0], pe->Addr[1], pe->Addr[2], pe->Addr[3], pe->Addr[4], pe->Addr[5]);
	    websWrite(wp, T("</tr>"));
	}
out5:
	close(s);
#endif
	return err;
}

static int getWlanM2UBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(CONFIG_RT2860V2_AP_IGMP_SNOOP) || defined(CONFIG_MT7610_AP_IGMP_SNOOP) || defined(CONFIG_MT76X2_AP_IGMP_SNOOP)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getGreenAPBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(CONFIG_RT2860V2_AP_GREENAP) || defined(CONFIG_MT76X2_AP_GREENAP)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

// Fast roaming parametrs
const parameter_fetch_t fast_roaming_flags[] =
{
	{ T("ApProbeRspTimes"), "ApProbeRspTimes", 0, T("3") },
	{ T("AuthRspFail"), "AuthRspFail", 0, T("0") },
	{ T("AuthRspRssi"), "AuthRspRssi", 0, T("0") },
	{ T("AssocReqRssiThres"), "AssocReqRssiThres", 0, T("0") },
	{ T("AssocRspIgnor"), "AssocRspIgnor", 0, T("0") },
	{ T("KickStaRssiLow"), "KickStaRssiLow", 0, T("0") },
	{ T("KickStaRssiLowDelay"), "KickStaRssiLowDelay", 0, T("0") },
	{ T("ProbeRspRssi"), "ProbeRspRssi", 0, T("0") },
	{ NULL, NULL, 0, NULL } // Terminator
};

#if defined(CONFIG_RT2860V2_AP_IDS) || defined(CONFIG_MT7610_AP_IDS) || defined(CONFIG_MT76X2_AP_IDS)
// IDS parametrs
const parameter_fetch_t ids_flags[] =
{
	{ T("AuthFloodThreshold"), "AuthFloodThreshold", 0, T("64") },
	{ T("AssocReqFloodThreshold"), "AssocReqFloodThreshold", 0, T("64") },
	{ T("ReassocReqFloodThreshold"), "ReassocReqFloodThreshold", 0, T("64") },
	{ T("ProbeReqFloodThreshold"), "ProbeReqFloodThreshold", 0, T("64") },
	{ T("DisassocFloodThreshold"), "DisassocFloodThreshold", 0, T("64") },
	{ T("DeauthFloodThreshold"), "DeauthFloodThreshold", 0, T("64") },
	{ T("EapReqFloodThreshold"), "EapReqFloodThreshold", 0, T("64") },
	{ NULL, NULL, 0, NULL } // Terminator
};
#endif

/* goform/wirelessBasic */
static void wirelessBasic(webs_t wp, char_t *path, char_t *query)
{
	char_t	*wirelessmode, *mbssid_mode, *apcli_mode, *wds_mode, *bssid_num, *mbcastisolated_ssid, *hssid, *isolated_ssid, *mbssidapisolated;
	char_t	*sz11gChannel, *abg_rate, *tx_power, *tx_stream, *rx_stream, *g_autoselect, *a_autoselect, *g_checktime, *a_checktime;
	char_t	*n_mode, *n_bandwidth, *n_gi, *n_stbc, *n_mcs, *n_rdg, *n_extcha, *n_amsdu, *n_autoba, *n_badecline, *dot11h;
	char_t  *fastroaming, *token;
#if defined(CONFIG_RT2860V2_AP_IDS) || defined(CONFIG_MT7610_AP_IDS) || defined(CONFIG_MT76X2_AP_IDS)
	char_t *ids_enable;
#endif
#ifndef CONFIG_RT_SECOND_IF_NONE
	char_t	*wirelessmodeac, *tx_power_ac, *sz11aChannel, *ssid1ac, *ac_gi, *ac_stbc, *ac_ldpc, *ac_bw, *ac_bwsig;
	int     is_vht = 0, mode_ac;
	char 	ieee80211h[2 * MAX_NUMBER_OF_BSSID] = "";
	char 	txModeInic[5 * MAX_NUMBER_OF_BSSID] = "";
#endif
#if (CONFIG_RT_SECOND_CARD==7612) || (CONFIG_RT_FIRST_CARD==7602)
	char_t	*ITxBfEn, *ETxBfeeEn, *ETxBfEnCond;
#endif
	int     is_ht = 0, i = 1, ssid = 0, new_bssid_num, mode;
	char	hidden_ssid[2 * MAX_NUMBER_OF_BSSID] = "", noforwarding[2 * MAX_NUMBER_OF_BSSID] = "", noforwardingmbcast[2 * MAX_NUMBER_OF_BSSID] = "";
	char 	ssid_web_var[8] = "mssid_\0", ssid_nvram_var[8] = "SSID\0\0\0";
	char 	txMode[5 * MAX_NUMBER_OF_BSSID] = "";
	char	*submitUrl;

	// Get current mode & new mode
	char *web_radio_on = websGetVar(wp, T("radioWirelessEnabled"), T("0"));
#ifndef CONFIG_RT_SECOND_IF_NONE
	char *web_radio_ac_on = websGetVar(wp, T("radioWirelessEnabledAc"), T("0"));
#endif
	// fetch from web input
	wirelessmode = websGetVar(wp, T("wirelessmode"), T("9")); //9: bgn mode
	mode = atoi(wirelessmode);
	tx_power = websGetVar(wp, T("tx_power"), T("100"));
	mbssid_mode = websGetVar(wp, T("mbssid_mode"), T("ra"));
	apcli_mode = websGetVar(wp, T("apcli_mode"), T("apcli0"));
	wds_mode = websGetVar(wp, T("wds_mode"), T("wds"));
	bssid_num = websGetVar(wp, T("bssid_num"), T("1"));
	hssid = websGetVar(wp, T("hssid"), T("")); 
	isolated_ssid = websGetVar(wp, T("isolated_ssid"), T(""));
	mbcastisolated_ssid = websGetVar(wp, T("mbcastisolated_ssid"), T(""));
	mbssidapisolated = websGetVar(wp, T("mbssidapisolated"), T("0"));

	sz11gChannel = websGetVar(wp, T("sz11gChannel"), T("")); 
	abg_rate = websGetVar(wp, T("abg_rate"), T("")); 
	int rate = atoi(abg_rate);

	tx_stream = websGetVar(wp, T("tx_stream"), T("1"));
	rx_stream = websGetVar(wp, T("rx_stream"), T("1"));

	n_mode = websGetVar(wp, T("n_mode"), T("1"));
	n_bandwidth = websGetVar(wp, T("n_bandwidth"), T("1"));
	n_gi = websGetVar(wp, T("n_gi"), T("1"));
	n_stbc = websGetVar(wp, T("n_stbc"), T("1"));
	n_mcs = websGetVar(wp, T("n_mcs"), T("33"));
	n_rdg = websGetVar(wp, T("n_rdg"), T("0"));
	n_extcha = websGetVar(wp, T("n_extcha"), T("0"));
	n_amsdu = websGetVar(wp, T("n_amsdu"), T("1"));
	n_autoba = websGetVar(wp, T("n_autoba"), T("1"));
	n_badecline = websGetVar(wp, T("n_badecline"), T("0"));
	g_autoselect = websGetVar(wp, T("autoselect_g"), T("0"));
	a_autoselect = websGetVar(wp, T("autoselect_a"), T("0"));
	g_checktime = websGetVar(wp, T("checktime_g"), T("0"));
	a_checktime = websGetVar(wp, T("checktime_a"), T("0"));
	fastroaming = websGetVar(wp, T("FastRoaming"), T("0"));
	fastroaming = (fastroaming == NULL) ? "0" : fastroaming;
#if defined(CONFIG_RT2860V2_AP_IDS) || defined(CONFIG_MT7610_AP_IDS) || defined(CONFIG_MT76X2_AP_IDS)
	ids_enable = websGetVar(wp, T("IdsEnable"), T("0"));
	ids_enable = (ids_enable == NULL) ? "0" : ids_enable;
#endif

#ifndef CONFIG_RT_SECOND_IF_NONE
	wirelessmodeac = websGetVar(wp, T("wirelessmodeac"), T("15")); //15: a/an/ac mode
	mode_ac = atoi(wirelessmodeac);
	tx_power_ac = websGetVar(wp, T("tx_power_ac"), T("100"));
	sz11aChannel = websGetVar(wp, T("sz11aChannel"), T("")); 
	ssid1ac = websGetVar(wp, T("mssidac_1"), T("0"));
	ac_gi = websGetVar(wp, T("ac_gi"), T("1"));
	ac_stbc = websGetVar(wp, T("ac_stbc"), T("1"));
	ac_ldpc = websGetVar(wp, T("ac_ldpc"), T("1"));
	ac_bw = websGetVar(wp, T("ac_bw"), T("1"));
	ac_bwsig = websGetVar(wp, T("ac_bwsig"), T("1"));
#endif
	new_bssid_num = atoi(bssid_num);
	dot11h = websGetVar(wp, T("dot11h"), T(""));

#if (CONFIG_RT_SECOND_CARD==7612) || (CONFIG_RT_FIRST_CARD==7602)
	ITxBfEn = websGetVar(wp, T("ITxBfEn"), T("1"));
	ETxBfeeEn = websGetVar(wp, T("ETxBfeeEn"), T("1"));
	ETxBfEnCond = websGetVar(wp, T("ETxBfEnCond"), T("1"));
#endif

	if (new_bssid_num < 1 || new_bssid_num > MAX_NUMBER_OF_BSSID) {
		websError(wp, 403, T("'bssid_num' %s is out of range!"), bssid_num);
		return;
	}

#ifndef CONFIG_RT_SECOND_IF_NONE
	// 11abgnac Channel or AutoSelect
	if ((0 == strlen(sz11aChannel)) && (0 == strlen(sz11gChannel)))
#else
	// bgn Channel or AutoSelect
	if (0 == strlen(sz11gChannel))
#endif
	{
		websError(wp, 403, T("'Channel' should not be empty!"));
		return;
	}

	is_ht = (mode >= 5) ? 1 : 0;
#ifndef CONFIG_RT_SECOND_IF_NONE
	is_vht = (mode_ac >= 14) ? 1 : 0;
#endif

	nvram_init(RT2860_NVRAM);

	// Wireless mode
	nvram_bufset(RT2860_NVRAM, "WirelessMode", wirelessmode);
	nvram_bufset(RT2860_NVRAM, "TxPower", tx_power);
#ifndef CONFIG_RT_SECOND_IF_NONE
	nvram_bufset(RT2860_NVRAM, "WirelessModeINIC", wirelessmodeac);
	nvram_bufset(RT2860_NVRAM, "TxPowerINIC", tx_power_ac);
#endif
	// Virtual iface modes
	nvram_bufset(RT2860_NVRAM, "BssidIfName", mbssid_mode);
	nvram_bufset(RT2860_NVRAM, "ApCliIfName", apcli_mode);
	nvram_bufset(RT2860_NVRAM, "WdsIfName", wds_mode);

	// BasicRate: bg,bgn,n:15, b:3; g,gn:351
	if ((mode == 4) || (mode == 7)) //g, gn
		nvram_bufset(RT2860_NVRAM, "BasicRate", "351");
	else if ((mode == 1)) //b
		nvram_bufset(RT2860_NVRAM, "BasicRate", "3");
	else // bg,bgn,n
		nvram_bufset(RT2860_NVRAM, "BasicRate", "15");

	default_shown_mbssid[RT2860_NVRAM] = 0;

	// Fill-in SSID
	for (ssid=0; ssid < new_bssid_num; ssid++)
	{
		ssid_web_var[6] = ssid  + '1';
		ssid_nvram_var[4] = i  + '0';
		token = (i != new_bssid_num) ? ";" : "";

		char_t *mssid = websGetVar(wp, ssid_web_var, T(""));
		if (CHK_IF_SET(mssid))
		{
			nvram_bufset(RT2860_NVRAM, ssid_nvram_var, mssid);
			sprintf(hidden_ssid, "%s%s", hidden_ssid, (strchr(hssid, ssid + '0') != NULL) ? "1" : "0");
			sprintf(hidden_ssid, "%s%s", hidden_ssid, token);
			sprintf(noforwarding, "%s%s", noforwarding, (strchr(isolated_ssid, ssid + '0') != NULL) ? "1" : "0");
			sprintf(noforwarding, "%s%s", noforwarding, token);
			sprintf(noforwardingmbcast, "%s%s", noforwardingmbcast, (strchr(mbcastisolated_ssid, ssid + '0') != NULL) ? "1" : "0");
			sprintf(noforwardingmbcast, "%s%s", noforwardingmbcast, token);
			if (strncmp(abg_rate, "", 1)) {
				if (mode == 1) {
					sprintf(txMode, "%s%s", txMode, "CCK");
				} else if ((mode >= 5)) {
					sprintf(txMode, "%s%s", txMode, "HT");
				} else {
					if (rate == 1 || rate == 2 || rate == 5 || rate == 11)
						sprintf(txMode, "%s%s", txMode, "CCK");
					else
						sprintf(txMode, "%s%s", txMode, "OFDM");
				}
#ifndef CONFIG_RT_SECOND_IF_NONE
				if (mode_ac >= 14)
					sprintf(txModeInic, "%s%s", txModeInic, "VHT");
				else if (mode_ac >= 5)
					sprintf(txModeInic, "%s%s", txModeInic, "HT");
				else
					sprintf(txModeInic, "%s%s", txModeInic, "OFDM");
#endif
			} else {
				sprintf(txMode, "%s%s", txMode, "HT");
#ifndef CONFIG_RT_SECOND_IF_NONE
				sprintf(txModeInic, "%s%s", txModeInic, "VHT");
#endif
			}
			sprintf(txMode, "%s%s", txMode, token);
#ifndef CONFIG_RT_SECOND_IF_NONE
			sprintf(txModeInic, "%s%s", txModeInic, token);
			sprintf(ieee80211h, "%s%s", ieee80211h, (CHK_IF_DIGIT(dot11h, 1)) ? "1" : "0");
			sprintf(ieee80211h, "%s%s", ieee80211h, token);
#endif
			i++;
		}
	}
#ifndef CONFIG_RT_SECOND_IF_NONE
	// Fist SSID for iNIC
	nvram_bufset(RT2860_NVRAM, "SSID1INIC", ssid1ac);
	nvram_bufset(RT2860_NVRAM, "IEEE80211H", ieee80211h);
	nvram_bufset(RT2860_NVRAM, "FixedTxModeINIC", txModeInic);
#endif
	// SSID settings
	nvram_bufset(RT2860_NVRAM, "BssidNum", bssid_num);
	nvram_bufset(RT2860_NVRAM, "HideSSID", hidden_ssid);
	nvram_bufset(RT2860_NVRAM, "NoForwarding", noforwarding);
	nvram_bufset(RT2860_NVRAM, "NoForwardingBTNBSSID", mbssidapisolated);
	nvram_bufset(RT2860_NVRAM, "NoForwardingMBCast", noforwardingmbcast);
	nvram_bufset(RT2860_NVRAM, "FixedTxMode", txMode);

	// Channel & automatic channel select
#ifndef CONFIG_RT_SECOND_IF_NONE
	if (CHK_IF_SET(sz11aChannel)) {
		nvram_bufset(RT2860_NVRAM, "ChannelINIC", sz11aChannel);
		if ((!strncmp(sz11aChannel, "0", 2)) && (CHK_IF_SET(a_autoselect))) {
		    nvram_bufset(RT2860_NVRAM, "AutoChannelSelectINIC", a_autoselect);
		    nvram_bufset(RT2860_NVRAM, "ACSCheckTimeINIC", (CHK_IF_SET(a_checktime)) ? a_checktime : "24");
		} else {
		    nvram_bufset(RT2860_NVRAM, "AutoChannelSelectINIC", "0");
		    nvram_bufset(RT2860_NVRAM, "ACSCheckTimeINIC", "0");
		}
	}
#endif
	if (CHK_IF_SET(sz11gChannel)) {
		nvram_bufset(RT2860_NVRAM, "Channel", sz11gChannel);
		if ((!strncmp(sz11gChannel, "0", 2)) && (CHK_IF_SET(g_autoselect))) {
		    nvram_bufset(RT2860_NVRAM, "AutoChannelSelect", g_autoselect);
		    nvram_bufset(RT2860_NVRAM, "ACSCheckTime", (CHK_IF_SET(g_checktime)) ? g_checktime : "24");
		} else {
		    nvram_bufset(RT2860_NVRAM, "AutoChannelSelect", "0");
		    nvram_bufset(RT2860_NVRAM, "ACSCheckTime", "0");
		}
	}

	// Rate for a, b, g, n, ac
	// In the future need allow set this per MBSSID
	if (strncmp(abg_rate, "", 1))
		switch (rate) {
			case 1:
				nvram_bufset(RT2860_NVRAM, "HT_MCS", "0");
				break;
			case 2:
				nvram_bufset(RT2860_NVRAM, "HT_MCS", "1");
				break;
			case 5:
				nvram_bufset(RT2860_NVRAM, "HT_MCS", "2");
				break;
			case 6:
				nvram_bufset(RT2860_NVRAM, "HT_MCS", "0");
				break;
			case 9:
				nvram_bufset(RT2860_NVRAM, "HT_MCS", "1");
				break;
			case 11:
				nvram_bufset(RT2860_NVRAM, "HT_MCS", "3");
				break;
			case 12:
				nvram_bufset(RT2860_NVRAM, "HT_MCS", "2");
				break;
			case 18:
				nvram_bufset(RT2860_NVRAM, "HT_MCS", "3");
				break;
			case 24:
				nvram_bufset(RT2860_NVRAM, "HT_MCS", "4");
				break;
			case 36:
				nvram_bufset(RT2860_NVRAM, "HT_MCS", "5");
				break;
			case 48:
				nvram_bufset(RT2860_NVRAM, "HT_MCS", "6");
				break;
			case 54:
				nvram_bufset(RT2860_NVRAM, "HT_MCS", "7");
				break;
			default:
				nvram_bufset(RT2860_NVRAM, "HT_MCS", "33");
				break;
		}

	// HT_OpMode, HT_BW, HT_GI, VHT_SGI, VHT_LDPC, HT_MCS, HT_HTC, HT_RDG, HT_EXTCHA, HT_AMSDU, HT_TxStream, HT_RxStream
	if (is_ht) {
		nvram_bufset(RT2860_NVRAM, "HT_OpMode", n_mode);
		nvram_bufset(RT2860_NVRAM, "HT_BW", n_bandwidth);
		nvram_bufset(RT2860_NVRAM, "HT_GI", n_gi);
		nvram_bufset(RT2860_NVRAM, "HT_STBC", n_stbc);
		nvram_bufset(RT2860_NVRAM, "HT_MCS", n_mcs);
		nvram_bufset(RT2860_NVRAM, "HT_EXTCHA", n_extcha);
		nvram_bufset(RT2860_NVRAM, "HT_AMSDU", n_amsdu);
		nvram_bufset(RT2860_NVRAM, "HT_AutoBA", n_autoba);
		nvram_bufset(RT2860_NVRAM, "HT_BADecline", n_badecline);
		nvram_bufset(RT2860_NVRAM, "HT_TxStream", tx_stream);
		nvram_bufset(RT2860_NVRAM, "HT_RxStream", rx_stream);
		// HT_RGD depend at HT_HTC+ frame support
		nvram_bufset(RT2860_NVRAM, "HT_HTC", n_rdg);
		nvram_bufset(RT2860_NVRAM, "HT_RDG", n_rdg);
	}

#if (CONFIG_RT_SECOND_CARD==7612) || (CONFIG_RT_FIRST_CARD==7602)
	nvram_bufset(RT2860_NVRAM, "ITxBfEn", ITxBfEn);
	nvram_bufset(RT2860_NVRAM, "ETxBfeeEn", ETxBfeeEn);
	nvram_bufset(RT2860_NVRAM, "ETxBfEnCond", ETxBfEnCond);
#endif

#ifndef CONFIG_RT_SECOND_IF_NONE
	// VHT_Modes
	if (is_vht)
	{
#if defined(CONFIG_MT7610_AP) || defined(CONFIG_MT7610_AP_MODULE)
		// for 1T1R module always disable (support only in STA mode for 1T1R)
		nvram_bufset(RT2860_NVRAM, "VHT_LDPC", "0");
		nvram_bufset(RT2860_NVRAM, "HT_LDPC", "0");
#else
		nvram_bufset(RT2860_NVRAM, "VHT_LDPC", ac_ldpc);
		nvram_bufset(RT2860_NVRAM, "HT_LDPC", ac_ldpc);
#endif
		nvram_bufset(RT2860_NVRAM, "VHT_STBC", ac_stbc);
		nvram_bufset(RT2860_NVRAM, "VHT_SGI", ac_gi);
		nvram_bufset(RT2860_NVRAM, "VHT_BW", ac_bw);
		nvram_bufset(RT2860_NVRAM, "VHT_BW_SIGNAL", ac_bwsig);
	}
#endif
	nvram_bufset(RT2860_NVRAM, "RadioOn", web_radio_on);
#ifndef CONFIG_RT_SECOND_IF_NONE
	nvram_bufset(RT2860_NVRAM, "RadioOnINIC", web_radio_ac_on);
#endif

	// Fast roaming
	nvram_bufset(RT2860_NVRAM, "FastRoaming", fastroaming);
	if (CHK_IF_DIGIT(fastroaming, 1))
		setupParameters(wp, fast_roaming_flags, 0);

#if defined(CONFIG_RT2860V2_AP_IDS) || defined(CONFIG_MT7610_AP_IDS) || defined(CONFIG_MT76X2_AP_IDS)
	nvram_bufset(RT2860_NVRAM, "IdsEnable", ids_enable);
	if (CHK_IF_DIGIT(ids_enable, 1))
		setupParameters(wp, ids_flags, 0);
#endif

	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

#ifdef PRINT_DEBUG
	// debug print
	websHeader(wp);
	websWrite(wp, T("<h2>mode: %s</h2><br>\n"), wirelessmode);
	websWrite(wp, T("ssid: %s, bssid_num: %s<br>\n"), ssid, bssid_num);
	websWrite(wp, T("hssid: %s<br>\n"), hssid);
	websWrite(wp, T("isolated_ssid: %s<br>\n"), isolated_ssid);
	websWrite(wp, T("mbssidapisolated: %s<br>\n"), mbssidapisolated);
	websWrite(wp, T("sz11aChannel: %s<br>\n"), sz11aChannel);
	websWrite(wp, T("sz11gChannel: %s<br>\n"), sz11gChannel);
	websWrite(wp, T("tx_power: %s<br>\n"), tx_power);
	if (strncmp(abg_rate, "", 1)) {
		websWrite(wp, T("abg_rate: %s<br>\n"), abg_rate);
	}
	if (is_ht) {
		websWrite(wp, T("n_mode: %s<br>\n"), n_mode);
		websWrite(wp, T("n_bandwidth: %s<br>\n"), n_bandwidth);
		websWrite(wp, T("n_gi: %s<br>\n"), n_gi);
		websWrite(wp, T("n_stbc: %s<br>\n"), n_stbc);
		websWrite(wp, T("n_mcs: %s<br>\n"), n_mcs);
		websWrite(wp, T("n_rdg: %s<br>\n"), n_rdg);
		websWrite(wp, T("n_extcha: %s<br>\n"), n_extcha);
		websWrite(wp, T("n_amsdu: %s<br>\n"), n_amsdu);
		websWrite(wp, T("n_autoba: %s<br>\n"), n_autoba);
		websWrite(wp, T("n_badecline: %s<br>\n"), n_badecline);

	}
#ifndef CONFIG_RT_SECOND_IF_NONE
	websWrite(wp, T("mode ac: %s<br>\n"), wirelessmodeac);
	websWrite(wp, T("mssidac_1: %s<br>\n"), ssid1ac);
	websWrite(wp, T("tx_power_ac: %s<br>\n"), tx_power_ac);
	if (is_vht)
	{
		websWrite(wp, T("ac_gi: %s<br>\n"), ac_gi);
		websWrite(wp, T("ac_stbc: %s<br>\n"), ac_gi);
		websWrite(wp, T("ac_ldpc: %s<br>\n"), ac_ldpc);
		websWrite(wp, T("ac_bw: %s<br>\n"), ac_bw);
		websWrite(wp, T("ac_bwsig: %s<br>\n"), ac_bwsig);
	}
#endif
	websWrite(wp, T("tx_stream: %s<br>\n"), tx_stream);
	websWrite(wp, T("rx_stream: %s<br>\n"), rx_stream);
	websFooter(wp);
	websDone(wp, 200);
#else
	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
#endif

	doSystem("internet.sh wifionly");
}

static int getVideoTurbineBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(CONFIG_RT2860V2_AP_IGMP_SNOOP) || defined(CONFIG_MT7610_AP_IGMP_SNOOP) || defined(CONFIG_MT76X2_AP_IGMP_SNOOP)
	#if defined(CONFIG_RT2860V2_AP_VIDEO_TURBINE) || defined(CONFIG_MT7610_AP_VIDEO_TURBINE) || defined(CONFIG_MT76X2_AP_VIDEO_TURBINE)
		websWrite(wp, T("1"));
	#else
		websWrite(wp, T("0"));
	#endif
#else
	websWrite(wp, T("0"));
#endif
	return 0;
}

static int getIdsEnableBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(CONFIG_RT2860V2_AP_IDS) || defined(CONFIG_MT7610_AP_IDS) || defined(CONFIG_MT76X2_AP_IDS)
	websWrite(wp, T("1"));
#else
	websWrite(wp, T("0"));
#endif
	return 0;
}

/* goform/wirelessAdvanced */
static void wirelessAdvanced(webs_t wp, char_t *path, char_t *query)
{
	char_t	*bg_protection, *beacon, *dtim, *fragment, *rts, *short_preamble, *maxstanum, *keepalive, *idletimeout,
		*short_slot, *tx_burst, *pkt_aggregate, *countrycode, *country_region, *rd_region, *wmm_capable;
	int ssid_num, tmp, i;
	char_t *life_check, *submitUrl;
	char stanum_array[32] = "", keepalive_array[32] = "";
#if defined(CONFIG_RT2860V2_AP_MCAST_RATE_SPECIFIC) || defined(CONFIG_MT7610_AP_MCAST_RATE_SPECIFIC) || defined(CONFIG_MT76X2_AP_MCAST_RATE_SPECIFIC)
	char_t	*mcast_mcs;
#endif
#if defined(CONFIG_RT2860V2_AP_IGMP_SNOOP) || defined(CONFIG_MT7610_AP_IGMP_SNOOP) || defined(CONFIG_MT76X2_AP_IGMP_SNOOP)
	char_t	*m2u_enable;
#if defined(CONFIG_RT2860V2_AP_VIDEO_TURBINE) || defined(CONFIG_MT7610_AP_VIDEO_TURBINE) || defined(CONFIG_MT76X2_AP_VIDEO_TURBINE)
	char_t *video_turbine;
#endif
#endif
	//fetch from web input
	bg_protection = websGetVar(wp, T("bg_protection"), T("0"));
	beacon = websGetVar(wp, T("beacon"), T("100"));
	dtim = websGetVar(wp, T("dtim"), T("1"));
	fragment = websGetVar(wp, T("fragment"), T("2346"));
	rts = websGetVar(wp, T("rts"), T("2347"));
	short_preamble = websGetVar(wp, T("short_preamble"), T("0"));
	short_slot = websGetVar(wp, T("short_slot"), T("0"));
	tx_burst = websGetVar(wp, T("tx_burst"), T("0"));
	pkt_aggregate = websGetVar(wp, T("pkt_aggregate"), T("0"));
	rd_region = websGetVar(wp, T("rd_region"), T("CE"));
	countrycode = websGetVar(wp, T("country_code"), T("NONE"));
	country_region = websGetVar(wp, T("country_region"), T("0"));
	wmm_capable = websGetVar(wp, T("WmmCapable"), T("0"));
#if defined(CONFIG_RT2860V2_AP_MCAST_RATE_SPECIFIC) || defined(CONFIG_MT7610_AP_MCAST_RATE_SPECIFIC) || defined(CONFIG_MT76X2_AP_MCAST_RATE_SPECIFIC)
	mcast_mcs = websGetVar(wp, T("McastMcs"), T("0"));
#endif
#if defined(CONFIG_RT2860V2_AP_IGMP_SNOOP) || defined(CONFIG_MT7610_AP_IGMP_SNOOP) || defined(CONFIG_MT76X2_AP_IGMP_SNOOP)
	m2u_enable = websGetVar(wp, T("m2u_enable"), T("0"));
#if defined(CONFIG_RT2860V2_AP_VIDEO_TURBINE) || defined(CONFIG_MT7610_AP_VIDEO_TURBINE) || defined(CONFIG_MT76X2_AP_VIDEO_TURBINE)
	video_turbine = websGetVar(wp, T("video_turbine"), T("0"));
#endif
#endif
	maxstanum = websGetVar(wp, T("maxstanum"), T("0"));
	keepalive = websGetVar(wp, T("keepalive"), T("0"));
	idletimeout = websGetVar(wp, T("idletimeout"), T("0"));
	life_check = websGetVar(wp, T("EntryLifeCheck"), T("0"));

	char *num_s = nvram_get(RT2860_NVRAM, "BssidNum");
	if (NULL != num_s)
		ssid_num = atoi(num_s);
	else
		ssid_num = 1;

	//Radar Detect region
	if ((rd_region == NULL) || (strlen(rd_region)<=0))
		rd_region = "CE";

	//set to nvram
	nvram_init(RT2860_NVRAM);
	nvram_bufset(RT2860_NVRAM, "BGProtection", bg_protection);
	nvram_bufset(RT2860_NVRAM, "BeaconPeriod", beacon);
	nvram_bufset(RT2860_NVRAM, "DtimPeriod", dtim);
	nvram_bufset(RT2860_NVRAM, "FragThreshold", fragment);
	nvram_bufset(RT2860_NVRAM, "RTSThreshold", rts);
	nvram_bufset(RT2860_NVRAM, "TxPreamble", short_preamble);
	nvram_bufset(RT2860_NVRAM, "ShortSlot", short_slot);
	nvram_bufset(RT2860_NVRAM, "PktAggregate", pkt_aggregate);
	nvram_bufset(RT2860_NVRAM, "RDRegion", rd_region);
	//txburst and burst mode set in one place
	nvram_bufset(RT2860_NVRAM, "TxBurst", tx_burst);
	nvram_bufset(RT2860_NVRAM, "BurstMode", tx_burst);

	if (NULL != maxstanum) {
		tmp = atoi(maxstanum);
		if ((tmp < 0) || (tmp > MAX_NUMBER_OF_MAC))
			tmp = MAX_NUMBER_OF_MAC;
		sprintf(stanum_array, "%d", tmp);
		for (i=2; i <= ssid_num; i++)
			sprintf(stanum_array, "%s;%d", stanum_array, tmp);
		nvram_bufset(RT2860_NVRAM, "MaxStaNum", stanum_array);
	}

	if (NULL != keepalive) {
		tmp = atoi(keepalive);
		if ((tmp < 10) || (tmp > 300))
			tmp = 60;
		sprintf(keepalive_array, "%d", tmp);
		for (i=2; i <= ssid_num; i++)
			sprintf(keepalive_array, "%s;%d", keepalive_array, tmp);
		nvram_bufset(RT2860_NVRAM, "StationKeepAlive", keepalive_array);
	}

	if (NULL != idletimeout) {
		tmp = atoi(idletimeout);
		if ((tmp < 60) || (tmp > 300))
			tmp = 60;
		sprintf(idletimeout, "%d", tmp);
		nvram_bufset(RT2860_NVRAM, "IdleTimeout", idletimeout);
	}

	if (NULL != life_check) {
		tmp = atoi(life_check);
		if ((tmp < 128) || (tmp > 2048))
			tmp = 512;
		sprintf(life_check, "%d", tmp);
		nvram_bufset(RT2860_NVRAM, "EntryLifeCheck", life_check);
	}

	nvram_bufset(RT2860_NVRAM, "WmmCapable", wmm_capable);
#if defined(CONFIG_RT2860V2_AP_MCAST_RATE_SPECIFIC) || defined(CONFIG_MT7610_AP_MCAST_RATE_SPECIFIC) || defined(CONFIG_MT76X2_AP_MCAST_RATE_SPECIFIC)
	nvram_bufset(RT2860_NVRAM, "McastMcs", mcast_mcs);
#endif
#if defined(CONFIG_RT2860V2_AP_IGMP_SNOOP) || defined(CONFIG_MT7610_AP_IGMP_SNOOP) || defined(CONFIG_MT76X2_AP_IGMP_SNOOP)
	nvram_bufset(RT2860_NVRAM, "M2UEnabled", m2u_enable);
	nvram_bufset(RT2860_NVRAM, "IgmpSnEnable", m2u_enable);
#if defined(CONFIG_RT2860V2_AP_VIDEO_TURBINE) || defined(CONFIG_MT7610_AP_VIDEO_TURBINE) || defined(CONFIG_MT76X2_AP_VIDEO_TURBINE)
	nvram_bufset(RT2860_NVRAM, "VideoTurbine", video_turbine);
#endif
#endif
	nvram_bufset(RT2860_NVRAM, "CountryCode", countrycode);
	if (!strncmp(countrycode, "US", 3)) {
		nvram_bufset(RT2860_NVRAM, "CountryRegionABand", "0");
	}
	else if (!strncmp(countrycode, "JP", 3)) {
		nvram_bufset(RT2860_NVRAM, "CountryRegionABand", "6");
	}
	else if (!strncmp(countrycode, "RU", 3)) {
		nvram_bufset(RT2860_NVRAM, "CountryRegionABand", "7");
	}
	else if (!strncmp(countrycode, "FR", 3)) {
		nvram_bufset(RT2860_NVRAM, "CountryRegionABand", "2");
	}
	else if (!strncmp(countrycode, "TW", 3)) {
		nvram_bufset(RT2860_NVRAM, "CountryRegionABand", "3");
	}
	else if (!strncmp(countrycode, "IE", 3)) {
		nvram_bufset(RT2860_NVRAM, "CountryRegionABand", "2");
	}
	else if (!strncmp(countrycode, "HK", 3)) {
		nvram_bufset(RT2860_NVRAM, "CountryRegionABand", "0");
	} else  /* default RU */
		nvram_bufset(RT2860_NVRAM, "CountryRegionABand", "0");

	// Set-up country region
	nvram_bufset(RT2860_NVRAM, "CountryRegion", country_region);

#ifdef CONFIG_BAND_STEERING
	char_t *bandsteering;
	bandsteering = websGetVar(wp, T("BandSteering"), T("0"));
	nvram_bufset(RT2860_NVRAM, "BandSteering", bandsteering);
#endif

	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

#ifdef PRINT_DEBUG
	//debug print
	websHeader(wp);
	websWrite(wp, T("bg_protection: %s<br>\n"), bg_protection);
	websWrite(wp, T("beacon: %s<br>\n"), beacon);
	websWrite(wp, T("dtim: %s<br>\n"), dtim);
	websWrite(wp, T("fragment: %s<br>\n"), fragment);
	websWrite(wp, T("rts: %s<br>\n"), rts);
	websWrite(wp, T("short_preamble: %s<br>\n"), short_preamble);
	websWrite(wp, T("short_slot: %s<br>\n"), short_slot);
	websWrite(wp, T("tx_burst: %s<br>\n"), tx_burst);
	websWrite(wp, T("pkt_aggregate: %s<br>\n"), pkt_aggregate);
	websWrite(wp, T("rd_region: %s<br>\n"), rd_region);
	websWrite(wp, T("countrycode: %s<br>\n"), countrycode);
#if defined(CONFIG_RT2860V2_AP_MCAST_RATE_SPECIFIC) || defined(CONFIG_MT7610_AP_MCAST_RATE_SPECIFIC) || defined(CONFIG_MT76X2_AP_MCAST_RATE_SPECIFIC)
	websWrite(wp, T("mcast_mcs: %s<br>\n"), mcast_mcs);
#endif
#if defined(CONFIG_RT2860V2_AP_IGMP_SNOOP) || defined(CONFIG_MT7610_AP_IGMP_SNOOP) || defined(CONFIG_MT76X2_AP_IGMP_SNOOP)
	websWrite(wp, T("m2u_enable: %s<br>\n"), m2u_enable);
#endif
	websFooter(wp);
	websDone(wp, 200);
#else
	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
#endif

	// restart wireless network
	doSystem("internet.sh wifionly");
}

#if defined(CONFIG_RT2860V2_AP_WDS) || defined(CONFIG_MT7610_AP_WDS) || defined(CONFIG_MT76X2_AP_WDS)
const parameter_fetch_t wds_args[] =
{
	{ T("wds_phy_mode"), "WdsPhyMode", 0, T("") },
	{ T("wds_encryp_type"), "WdsEncrypType", 0,       T("") },
	{ T("wds_encryp_key0"), "Wds0Key", 0, T("") },
	{ T("wds_encryp_key1"), "Wds1Key", 0, T("") },
	{ T("wds_encryp_key2"), "Wds2Key", 0, T("") },
	{ T("wds_encryp_key3"), "Wds3Key", 0, T("") },
	{ T("wds_num"), "WdsNum", 0, T("0") },
	{ NULL, NULL, 0, NULL }
};

/* goform/wirelessWds */
static void wirelessWds(webs_t wp, char_t *path, char_t *query)
{
	char_t	*wds_mode, *wds_list;
	char_t *submitUrl;

	wds_mode = websGetVar(wp, T("wds_mode"), T("0"));
	wds_list = websGetVar(wp, T("wds_list"), T(""));

	nvram_init(RT2860_NVRAM);
	nvram_bufset(RT2860_NVRAM, "WdsEnable", wds_mode);
	if (!CHK_IF_DIGIT(wds_mode, 0)) {
		setupParameters(wp, wds_args, 0);
		if (CHK_IF_DIGIT(wds_mode, 2) || CHK_IF_DIGIT(wds_mode, 3)) {
			if (0 != strlen(wds_list))
				nvram_bufset(RT2860_NVRAM, "WdsList", wds_list);
		} else {
			nvram_bufset(RT2860_NVRAM, "WdsList", "");
		}
	}
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);

	// restart wireless network
	doSystem("internet.sh wifionly");
}
#endif

#if defined(CONFIG_RT2860V2_AP_APCLI) || defined(CONFIG_MT7610_AP_APCLI) || defined(CONFIG_MT76X2_AP_APCLI)
const parameter_fetch_t apcli_args[] =
{
	{ T("apcli_ssid"),              "ApCliSsid",            0,       T("") },
	{ T("apcli_bssid"),             "ApCliBssid",           0,       T("") },
	{ T("apcli_mode"),              "ApCliAuthMode",        0,       T("OPEN") },
	{ T("apcli_enc"),               "ApCliEncrypType",      0,       T("NONE") },
	{ T("apcli_wpapsk"),            "ApCliWPAPSK",          0,       T("12345678") },
	{ T("apcli_autoscan"),          "ApCliAutoConnect",     2,       T("off") },
	{ T("apcli_apiface"),           "ApCliClientOnly",      2,       T("off") },
	{ T("apcli_bridge"),            "ApCliBridgeOnly",      2,       T("off") },
	{ NULL, NULL, 0, NULL }
};

/* goform/wirelessApcli */
static void wirelessApcli(webs_t wp, char_t *path, char_t *query)
{
	char_t *submitUrl;

	//fetch from web input
	nvram_init(RT2860_NVRAM);
	nvram_bufset(RT2860_NVRAM, "ApCliEnable", "1");
	setupParameters(wp, apcli_args, 0);
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	char_t *reboot_flag = websGetVar(wp, T("reboot"), T("0"));
	if (CHK_IF_DIGIT(reboot_flag, 1)) {
		/* Output timer for reloading */
		outputTimerForReload(wp, 80000);

		/* Reboot */
		reboot_now();
	} else {
		submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
		websRedirect(wp, submitUrl);
	}
}
#endif

/* STF means "Save To Flash" ...*/
void STFs(int nvram, int index, char *flash_key, char *value)
{
	char *result;
	char *tmp = nvram_bufget(nvram, flash_key);
	if(!tmp)
		tmp = "";
	result = setNthValue(index, tmp, value);
	nvram_bufset(nvram, flash_key, result);
	return ;
}
#define STF(nvram, index, flash_key)    STFs(nvram, index, #flash_key, flash_key)

/* LFF means "Load From Flash" ...*/
#define LFF(result, nvram, x, n)								\
				do{		char tmp[128];					\
				    if(! ( x  = nvram_get(nvram, #x)) )				\
					tmp[0] = '\0';						\
				    else{							\
					if( getNthValueSafe(n, x, ';', tmp, 128) != -1){	\
					    gstrncat(result, tmp, 4096);			\
					}							\
				    }gstrncat(result, "\r", 4096);				\
				}while(0)

/* Load from Web */
#define LFW(x, y) do{ if(! ( x = websGetVar(wp, T(#y), T("")))) return;	}while(0)

static void getSecurity(int nvram, webs_t wp, char_t *path, char_t *query)
{
	int num_ssid, i;
	char *num_s = nvram_get(nvram, "BssidNum");
	char_t result[4096];
	char_t *PreAuth, *AuthMode, *EncrypType, *DefaultKeyID, *Key1Type, *Key2Type,
		   *Key3Type, *Key4Type, *RekeyMethod, *RekeyInterval, *PMKCachePeriod, *IEEE8021X;
	char_t *RADIUS_Server, *RADIUS_Port, *RADIUS_Key, *session_timeout_interval;

	if(num_s)
		num_ssid = atoi(num_s);
	else
		num_ssid = 1;

	result[0] = '\0';

	// deal with shown MBSSID
	if(default_shown_mbssid[nvram] > atoi(num_s))
		default_shown_mbssid[nvram] = 0;
	sprintf(result, "%d\n",  default_shown_mbssid[nvram]);

	if (RT2860_NVRAM == nvram) {
		for(i=0; i<num_ssid; i++) {
			gstrncat(result, nvram_get(nvram, racat("SSID", i+1)), 4096);
			gstrncat(result, "\r", 4096);
			LFF(result, nvram, PreAuth, i);
			LFF(result, nvram, AuthMode, i);
			LFF(result, nvram, EncrypType, i);
			LFF(result, nvram, DefaultKeyID, i);
			LFF(result, nvram, Key1Type, i);
			gstrncat(result, nvram_get(nvram, racat("Key1Str", i+1)), 4096);
			gstrncat(result, "\r", 4096);
			LFF(result, nvram, Key2Type, i);
			gstrncat(result, nvram_get(nvram, racat("Key2Str", i+1)), 4096);
			gstrncat(result, "\r", 4096);
			LFF(result, nvram, Key3Type, i);
			gstrncat(result, nvram_get(nvram, racat("Key3Str", i+1)), 4096);
			gstrncat(result, "\r", 4096);
			LFF(result, nvram, Key4Type, i);
			gstrncat(result, nvram_get(nvram, racat("Key4Str", i+1)), 4096);
			gstrncat(result, "\r", 4096);
			gstrncat(result, nvram_get(nvram, racat("WPAPSK", i+1)), 4096);
			gstrncat(result, "\r", 4096);

			LFF(result, nvram, RekeyMethod, i);
			LFF(result, nvram, RekeyInterval, i);
			LFF(result, nvram, PMKCachePeriod, i);
			LFF(result, nvram, IEEE8021X, i);
			LFF(result, nvram, RADIUS_Server, i);
			LFF(result, nvram, RADIUS_Port, i);
			LFF(result, nvram, RADIUS_Key, i);
			LFF(result, nvram, session_timeout_interval, i);

			// access control related.
			gstrncat(result, nvram_get(nvram, racat("AccessPolicy", i)), 4096);
			gstrncat(result, "\r", 4096);
			gstrncat(result, nvram_get(nvram, racat("AccessControlList", i)), 4096);
			gstrncat(result, "\r", 4096);
			gstrncat(result, "\n", 4096);
		}
	}

	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\n"));
	websWrite(wp, WEBS_CACHE_CONTROL_STRING);
	websWrite(wp, T("\n"));
	websWrite(wp, T("%s"), result);
	websDone(wp, 200);

}

static void wirelessGetSecurity(webs_t wp, char_t *path, char_t *query)
{
	return getSecurity(RT2860_NVRAM, wp, path, query);
}

static int AccessPolicyHandle(int nvram, webs_t wp, int mbssid)
{
	char_t *apselect, *newap_list;
	char str[32];
	char ap_list[2048];

	if(mbssid > 8 || mbssid < 0)
		return -1;

	sprintf(str, "apselect_%d", mbssid);	// UI on web page
	apselect = websGetVar(wp, str, T(""));
	if(!apselect){
		printf("goahead: cant find %s\n", apselect);
		return -1;
	}

	sprintf(str, "AccessPolicy%d", mbssid);
	nvram_set(nvram, str, apselect);

	sprintf(str, "newap_text_%d", mbssid);
	newap_list = websGetVar(wp, str, T(""));

	if(!newap_list)
		return -1;

	if(!gstrlen(newap_list))
		return 0;

	sprintf(str, "AccessControlList%d", mbssid);
	sprintf(ap_list, "%s", nvram_get(nvram, str));
	if(strlen(ap_list))
		sprintf(ap_list, "%s;%s", ap_list, newap_list);
	else
		sprintf(ap_list, "%s", newap_list);

	nvram_set(nvram, str, ap_list);
	return 0;
}

#ifdef CONFIG_USER_802_1X
static void clearRadiusSetting(int nvram, int mbssid)
{
	char *RADIUS_Server, *RADIUS_Port, *RADIUS_Key;

	RADIUS_Server = nvram_get(nvram, "RADIUS_Server");
	RADIUS_Port = nvram_get(nvram, "RADIUS_Port");
	RADIUS_Key = nvram_get(nvram, "RADIUS_Key");

	nvram_init(RT2860_NVRAM);
	nvram_bufset(nvram, "RADIUS_Server", setNthValue(mbssid, RADIUS_Server, ""));
	nvram_bufset(nvram, "RADIUS_Port", setNthValue(mbssid, RADIUS_Port, "1812"));
	nvram_bufset(nvram, "RADIUS_Key", setNthValue(mbssid, RADIUS_Key, "wive-ng-mt"));
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);
}

static void conf8021x(int nvram, webs_t wp, int mbssid)
{
	char_t *RadiusServerIP, *RadiusServerPort, *RadiusServerSecret, *RadiusServerSessionTimeout;
	char lan_if_addr[32];

	if (getIfIp(getLanIfName(), lan_if_addr) != -1) {
	    nvram_bufset(nvram, "own_ip_addr", lan_if_addr);
	    /* temp static code raius server at LAN network, in future need select in UI */
	    nvram_bufset(nvram, "EAPifname", "br0");
	    nvram_bufset(nvram, "PreAuthifname", "br0");
	}

	LFW(RadiusServerIP, RadiusServerIP);
	LFW(RadiusServerPort, RadiusServerPort);
	LFW(RadiusServerSecret, RadiusServerSecret);
	LFW(RadiusServerSessionTimeout, RadiusServerSessionTimeout);
	if(!gstrlen(RadiusServerSessionTimeout))
		RadiusServerSessionTimeout = "0";

	if(gstrlen(RadiusServerIP)) {
	    STFs(nvram, mbssid, "RADIUS_Server", RadiusServerIP);
	    STFs(nvram, mbssid, "RADIUS_Port", RadiusServerPort);
	    STFs(nvram, mbssid, "RADIUS_Key", RadiusServerSecret);
	    STFs(nvram, mbssid, "session_timeout_interval", RadiusServerSessionTimeout);
	}

}
#endif

static void confWEP(int nvram, webs_t wp, int mbssid)
{
	char_t *DefaultKeyID, *Key1Type, *Key1Str, *Key2Type, *Key2Str, *Key3Type, *Key3Str, *Key4Type, *Key4Str;

	LFW(DefaultKeyID, wep_default_key);
	LFW(Key1Str, wep_key_1);
	LFW(Key2Str, wep_key_2);
	LFW(Key3Str, wep_key_3);
	LFW(Key4Str, wep_key_4);
	LFW(Key1Type, WEP1Select);
	LFW(Key2Type, WEP2Select);
	LFW(Key3Type, WEP3Select);
	LFW(Key4Type, WEP4Select);

	STF(nvram, mbssid, DefaultKeyID);
	STF(nvram, mbssid, Key1Type);
	STF(nvram, mbssid, Key2Type);
	STF(nvram, mbssid, Key3Type);
	STF(nvram, mbssid, Key4Type);

	nvram_init(RT2860_NVRAM);
	nvram_bufset(nvram, racat("Key1Str", mbssid+1), Key1Str);
	nvram_bufset(nvram, racat("Key2Str", mbssid+1), Key2Str);
	nvram_bufset(nvram, racat("Key3Str", mbssid+1), Key3Str);
	nvram_bufset(nvram, racat("Key4Str", mbssid+1), Key4Str);
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);
}

static void confWPAGeneral(int nvram, webs_t wp, int mbssid)
{
	char *cipher_str;
	char *key_renewal_interval;

	LFW(cipher_str, cipher);
	LFW(key_renewal_interval, keyRenewalInterval);

	nvram_init(RT2860_NVRAM);
	switch(cipher_str[0]){
	case '0':
		STFs(nvram, mbssid, "EncrypType", "TKIP");
		break;
	case '1':
		STFs(nvram, mbssid, "EncrypType", "AES");
		break;
	case '2':
		// there is no tkip-aes mixed mode in WPA-PSK.
		STFs(nvram, mbssid, "EncrypType", "TKIPAES");
		goto out;
	}
	STFs(nvram, mbssid, "DefaultKeyID", "1");
	STFs(nvram, mbssid, "RekeyInterval", key_renewal_interval);
	STFs(nvram, mbssid, "RekeyMethod", "TIME");
	STFs(nvram, mbssid, "IEEE8021X", "0");
out:
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);
	return;
}

static void Security(int nvram, webs_t wp, char_t *path, char_t *query)
{
	char_t *SSID;
	int mbssid;
	char_t *security_mode;
	char_t *submitUrl;

	LFW(SSID, ssidIndex);
	if (!CHK_IF_SET(SSID))
		return;

	mbssid = atoi(SSID);

	default_shown_mbssid[nvram] = mbssid;

	LFW(security_mode, security_mode);

#ifdef CONFIG_USER_802_1X
	/* clear Radius settings */
	clearRadiusSetting(nvram, mbssid);
#endif

	nvram_init(RT2860_NVRAM);
	if ( !strcmp(security_mode, "Disable"))				// !------------------       Disable Mode --------------
	{
		STFs(nvram, mbssid, "AuthMode", "OPEN");
		STFs(nvram, mbssid, "EncrypType", "NONE");
		STFs(nvram, mbssid, "IEEE8021X", "0");
	}else if( !strcmp(security_mode, "OPEN")){			// !------------------       Open Mode ----------------
		confWEP(nvram, wp, mbssid);
		STFs(nvram, mbssid, "AuthMode", security_mode);
		STFs(nvram, mbssid, "EncrypType", "WEP");
		STFs(nvram, mbssid, "IEEE8021X", "0");
	}else if( !strcmp(security_mode, "SHARED")){			// !------------------       Shared Mode ----------------
		char *security_shared_mode;
		confWEP(nvram, wp, mbssid);

		LFW(security_shared_mode, security_shared_mode);

		STFs(nvram, mbssid, "AuthMode", security_mode);
		if( !strcmp(security_shared_mode, "None"))
			STFs(nvram, mbssid, "EncrypType", "NONE");
		else
			STFs(nvram, mbssid, "EncrypType", "WEP");

		STFs(nvram, mbssid, "IEEE8021X", "0");
	}else if( !strcmp(security_mode, "WEPAUTO")){ 				// !------------------       WEP Auto Mode ----------------
		confWEP(nvram, wp, mbssid);
		STFs(nvram, mbssid, "AuthMode", security_mode);
		STFs(nvram, mbssid, "EncrypType", "WEP");
		STFs(nvram, mbssid, "IEEE8021X", "0");
	}else if( !strcmp(security_mode, "WPA") ||
				!strcmp(security_mode, "WPA1WPA2") ){		// !------------------		WPA Enterprise Mode ----------------
#ifdef CONFIG_USER_802_1X
		conf8021x(nvram, wp, mbssid);					// !------------------		WPA1WPA2 mixed mode
#endif
		confWPAGeneral(nvram, wp, mbssid);

		STFs(nvram, mbssid, "AuthMode", security_mode);
		STFs(nvram, mbssid, "IEEE8021X", "0");
	}else if( !strcmp(security_mode, "WPAPSK")){ 				// !------------------       WPA Personal Mode ----------------
		char *pass_phrase_str;

		confWPAGeneral(nvram, wp, mbssid);
		LFW(pass_phrase_str, passphrase);
		STFs(nvram, mbssid, "AuthMode", security_mode);
		STFs(nvram, mbssid, "IEEE8021X", "0");
		nvram_bufset(nvram, racat("WPAPSK", mbssid+1), pass_phrase_str);
	}else if( !strcmp(security_mode, "WPA2")){				// !------------------  WPA2 Enterprise Mode ----------------
		char *pass_phrase_str;
		char *PMKCachePeriod;
		char *PreAuth;

#ifdef CONFIG_USER_802_1X
		conf8021x(nvram, wp, mbssid);
#endif
		confWPAGeneral(nvram, wp, mbssid);

		LFW(pass_phrase_str, passphrase);
		LFW(PMKCachePeriod, PMKCachePeriod);
		LFW(PreAuth, PreAuthentication);

		STFs(nvram, mbssid, "AuthMode", security_mode);
		STFs(nvram, mbssid, "IEEE8021X", "0");
		nvram_bufset(nvram, racat("WPAPSK", mbssid+1), pass_phrase_str);
		STF(nvram, mbssid, PMKCachePeriod);
		STF(nvram, mbssid, PreAuth);
	}else if( !strcmp(security_mode, "WPA2PSK") ||				// !------------------  WPA2 Personal Mode ----------------
				!strcmp(security_mode, "WPAPSKWPA2PSK") ){ 	// !-------------   WPA PSK WPA2 PSK mixed
		char *pass_phrase_str;

		confWPAGeneral(nvram, wp, mbssid);
		LFW(pass_phrase_str, passphrase);

		STFs(nvram, mbssid, "AuthMode", security_mode);
		STFs(nvram, mbssid, "IEEE8021X", "0");
		nvram_bufset(nvram, racat("WPAPSK", mbssid+1), pass_phrase_str);
	}else if( !strcmp(security_mode, "IEEE8021X")){				// !------------------ 802.1X WEP Mode ----------------
		char *ieee8021x_wep;

#ifdef CONFIG_USER_802_1X
		conf8021x(nvram, wp, mbssid);
#endif
		STFs(nvram, mbssid, "IEEE8021X", "1");
		STFs(nvram, mbssid, "AuthMode", "OPEN");

		LFW(ieee8021x_wep, ieee8021x_wep);
		if(ieee8021x_wep[0] == '0')
			STFs(nvram, mbssid, "EncrypType", "NONE");
		else
			STFs(nvram, mbssid, "EncrypType", "WEP");
	} else {
		goto out;
	}

	// Access Policy
	if(AccessPolicyHandle(nvram, wp, mbssid) == -1)
		trace(0, "** error in AccessPolicyHandle()\n");

out:
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

#ifdef PRINT_DEBUG
	//debug print
	websHeader(wp);
	websWrite(wp, T("<h2>MBSSID index: %d, Security Mode: %s Done</h2><br>\n"), mbssid, security_mode);
	websFooter(wp);
	websDone(wp, 200);
#else
	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
#endif
	doSystem("internet.sh wifionly");
}

static void APSecurity(webs_t wp, char_t *path, char_t *query)
{
	Security(RT2860_NVRAM, wp, path, query);
}

static void DeleteAccessPolicyList(int nvram, webs_t wp, char_t *path, char_t *query)
{
	int mbssid, aplist_num;
	char str[32], apl[64*20], *tmp;

	sscanf(query, "%d,%d", &mbssid, &aplist_num);
	sprintf(str, "AccessControlList%d", mbssid);
	if(!(tmp = nvram_get(nvram, str)))
		return;

	strcpy(apl, tmp);
	deleteNthValueMulti(&aplist_num, 1, apl, ';');
	nvram_set(nvram, str, apl);

	default_shown_mbssid[nvram] = mbssid;

	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\n"));
	websWrite(wp, WEBS_CACHE_CONTROL_STRING);
	websWrite(wp, T("\n"));
	websWrite(wp, T("ok done"));
	websDone(wp, 200);
}

static void APDeleteAccessPolicyList(webs_t wp, char_t *path, char_t *query)
{
	DeleteAccessPolicyList(RT2860_NVRAM, wp, path, query);
}

static int is3t3r(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(CONFIG_RALINK_RT3883_3T3R)
	websWrite(wp, T("1"));
#else
	websWrite(wp, T("0"));
#endif
	return 0;
}

static int is5gh_support(int eid, webs_t wp, int argc, char_t **argv)
{
#ifndef CONFIG_RT_SECOND_IF_NONE
	websWrite(wp, T("1"));
#else
	websWrite(wp, T("0"));
#endif
	return 0;
}

static int is5gh_1t1r(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(CONFIG_MT7610_AP) || defined(CONFIG_MT7610_AP_MODULE)
	websWrite(wp, T("1"));
#else
	websWrite(wp, T("0"));
#endif
	return 0;
}

static int getTXBFBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if CONFIG_RT_SECOND_CARD==7612 || CONFIG_RT_FIRST_CARD==7602
	websWrite(wp, T("1"));
#else
	websWrite(wp, T("0"));
#endif
	return 0;
}

static int get802_1XBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_802_1X
	websWrite(wp, T("1"));
#else
	websWrite(wp, T("0"));
#endif /* CONFIG_USER_802_1X */
    return 0;
}

static void disconnectSta(webs_t wp, char_t *path, char_t *query)
{
	char_t *mac = websGetVar(wp, T("disconnectSta"), "");
	char_t *submitUrl;

	if ((mac != NULL) && (strlen(mac) > 0))
	{
		if (strcmp(mac, "*") == 0) {
			doSystem("iwpriv ra0 set DisConnectAllSta=1");
#ifndef CONFIG_RT_SECOND_IF_NONE
			doSystem("iwpriv rai0 set DisConnectAllSta=1");
#endif
		} else if (strlen(mac) == 17) {
			char cmd[80];

			sprintf(cmd, "iwpriv ra0 set DisConnectSta=%s", mac);
			doSystem(cmd);
#ifndef CONFIG_RT_SECOND_IF_NONE
			sprintf(cmd, "iwpriv rai0 set DisConnectSta=%s", mac);
			doSystem(cmd);
#endif
		}
	}

	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
}

static int getMaxStaNum(int eid, webs_t wp, int argc, char_t **argv)
{
	return websWrite(wp, T("%d"), MAX_NUMBER_OF_MAC);
}

static int getBSSIDNum(int eid, webs_t wp, int argc, char_t **argv)
{
	return websWrite(wp, T("%d"), MAX_NUMBER_OF_BSSID);
}

static int getBandSteeringBuilt(int eid, webs_t wp, int argc, char_t **argv) {
#ifdef CONFIG_BAND_STEERING
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}
