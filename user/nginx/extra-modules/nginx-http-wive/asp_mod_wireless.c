#include "asp_mod_wireless.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

/*
#define CONFIG_MT7615_AP_SMART_CARRIER_SENSE
#define CONFIG_MT7615_AP_WNM_SUPPORT
#define CONFIG_MT7615_AP_VOW_SUPPORT
#define CONFIG_MT7615_AP_80211W_PMF
#define CONFIG_MT7615_AP_TXBF_SUPPORT
#define CONFIG_MT7615_AP
*/

static int default_shown_mbssid[3]  = {0,0,0};

static void setSecurity(webs_t* wp, int nvram);

static int getWlanSCSBuilt(webs_t *wp, char** params, int nparams)
{
#if defined(CONFIG_MT7615_AP_SMART_CARRIER_SENSE) || defined(CONFIG_MT76X3_AP_SMART_CARRIER_SENSE)
	return outWrite("1");
#else
	return outWrite("0");
#endif
}

static int getWlanWNMBuilt(webs_t *wp, char** params, int nparams)
{
#if defined(CONFIG_MT7615_AP_WNM_SUPPORT)
	return outWrite("1");
#else
	return outWrite("0");
#endif
}

static int getWlanVOWBuilt(webs_t *wp, char** params, int nparams)
{
#if defined(CONFIG_MT7615_AP_VOW_SUPPORT)
	return outWrite("1");
#else
	return outWrite("0");
#endif
}

static int getWlan256QAMBuilt(webs_t *wp, char** params, int nparams)
{
#if defined(CONFIG_MT7615_AP) || defined(CONFIG_MT7615_AP_MODULE)
	return outWrite("1");
#else
	return outWrite("0");
#endif
}

static int getWlanPMFBuilt(webs_t *wp, char** params, int nparams)
{
#if defined(CONFIG_MT76X3_AP_80211W_PMF) || defined(CONFIG_MT7615_AP_80211W_PMF)
	return outWrite("1");
#else
	return outWrite("0");
#endif
}

static int getWlan4T4RBuilt(webs_t *wp, char** params, int nparams)
{
#if defined(CONFIG_RT_FIRST_IF_MT7615E) || defined(CONFIG_RT_SECOND_IF_MT7615E)
	return outWrite("1");
#else
	return outWrite("0");
#endif
}

static int getWlanApcliBuilt(webs_t *wp, char** params, int nparams)
{
#if defined(CONFIG_MT7610_AP_APCLI) || defined(CONFIG_MT76X2_AP_APCLI) || defined(CONFIG_MT76X3_AP_APCLI) || defined(CONFIG_MT7615_AP_APCLI)
	return outWrite("1");
#else
	return outWrite("0");
#endif
}

static int getWlanWdsBuilt(webs_t *wp, char** params, int nparams)
{
#if defined(CONFIG_MT7610_AP_WDS) || defined(CONFIG_MT76X2_AP_WDS) || defined(CONFIG_MT76X3_AP_WDS) || defined(CONFIG_MT7615_AP_WDS)
	return outWrite("1");
#else
	return outWrite("0");
#endif
}

static int listCountryCodes(webs_t *wp, char** params, int nparams)
{
    const country_code_t *codes = country_codes;
    char *c_code = ngx_nvram_get(wp, "CountryCode");

    outWrite("\t<option value=\"NONE\">NONE</option>\n");

    while (codes->iso != NULL)
    {
        outWrite("\t<option value=\"%s\"%s>%s (%s)</option>\n", 
                    codes->iso,
                    (strcmp(c_code, codes->iso) == 0) ? " selected" : "",
                    codes->name, codes->iso
                );
        codes++;
    }

    return 0;
}

/*
 * description: write 802.11a channels in <select> tag
 */
static int getWlan11aChannels(webs_t *wp, char** params, int nparams)
{
	int  idx = 0;
	int value = ngx_nvram_get_int(wp, "CountryRegionABand", -1);
	int channel = ngx_nvram_get_int(wp, "ChannelINIC", 0);

	switch (value)
	{
	    case -1:
	    case 0:
	    case 7:
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			outWrite(T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 132~144 */
		for (idx = 24; idx < 28; idx++)
		        outWrite(T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~165 */
		for (idx = 28; idx < 33; idx++)
			outWrite(T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
		break;

	    case 1:
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			outWrite(T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~140 */
		for (idx = 16; idx < 27; idx++)
			outWrite(T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		break;

	    case 2:
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			outWrite(T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		break;

	    case 3:
		/* 52~64 */
		for (idx = 4; idx < 8; idx++)
			outWrite(T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			outWrite(T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
		break;

	    case 4:
		/* 149~165 */
		for (idx = 28; idx < 33; idx++)
			outWrite(T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
		break;

	    case 5:
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			outWrite(T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
		break;

	    case 6:
		/* 36~48 */
		for (idx = 0; idx < 4; idx++)
			outWrite(T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		break;

	    case 8:
		/* 52~64 */
		for (idx = 4; idx < 8; idx++)
			outWrite(T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		break;

	    case 9:
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			outWrite(T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~116 */
		for (idx = 16; idx < 21; idx++)
			outWrite(T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 132~140 */
		for (idx = 24; idx < 27; idx++)
			outWrite(T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~165 */
		for (idx = 28; idx < 33; idx++)
			outWrite(T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
		break;

	    case 10:
		/* 36~48 */
		for (idx = 0; idx < 4; idx++)
			outWrite(T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~165 */
		for (idx = 28; idx < 33; idx++)
			outWrite(T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
		break;

	    case 11:
		/* 36~64 */
		for (idx = 0; idx < 8; idx++)
			outWrite(T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
					(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 100~120 */
		for (idx = 16; idx < 22; idx++)
			outWrite(T("%s%d %s>%d%s%d%s"), "<option value=", 36+4*idx,
				   	(36+4*idx == channel)? "selected" : "", 5180+20*idx,
					"MHz (Channel ", 36+4*idx, ")</option>");
		/* 149~161 */
		for (idx = 28; idx < 32; idx++)
			outWrite(T("%s%d %s>%d%s%d%s"), "<option value=",
					36+4*idx+1, (36+4*idx+1 == channel)? "selected" : "",
					5180+20*idx+5, "MHz (Channel ", 36+4*idx+1, ")</option>");
		break;
	}

	return 0;
}

/*
 * description: write 802.11b/g/n channels in <select> tag
 */
static int getWlan11gChannels(webs_t *wp, char** params, int nparams)
{
    int idx = 0;
    int channel = ngx_nvram_get_int(wp, "Channel", 0);

    for (idx = 0; idx < 13; idx++)
	outWrite(T("%s%d %s>%d%s%d%s"), 
		"<option value=", idx+1, (idx+1 == channel)? "selected" : "", 2412+5*idx, "MHz (Channel ", idx+1, ")</option>");

    return outWrite(T("<option value=14 %s>2484MHz (Channel 14)</option>\n"),(14 == channel)? "selected" : "");
}

/*
 * description: write channel number or 0 if auto-select
 */
static int getWlanChannel(webs_t *wp, char** params, int nparams)
{
	int value =  getWlanChannelNum_nvram(1);
	return outWrite(T("%d"), value);
}

static int getWlanChannelAC(webs_t *wp, char** params, int nparams)
{
	int value =  getWlanChannelNum_nvram(2);
	return outWrite(T("%d"), value);
}

/*
 * description: write ioctl channel number
 */
static int getWlanChannel_ioctl(webs_t *wp, char** params, int nparams)
{
	int value =  getWlanChannelNum_ioctl(1);
	return outWrite(T("%d"), value);
}

static int getWlanChannelAC_ioctl(webs_t *wp, char** params, int nparams)
{
#ifndef CONFIG_RT_SECOND_IF_NONE
	int value =  getWlanChannelNum_ioctl(2);
	return outWrite(T("%d"), value);
#else
	return outWrite("0");
#endif
}

/*
 * description: write MAC address from interface 'ra0'
 */
static int getWlanCurrentMac(webs_t *wp, char** params, int nparams)
{
	char if_hw[18] = {0};
	int errcode = getWlanCurrentMacAddr(if_hw, 1);
	if (errcode)
	{
	    syslog(LOG_ERR, "Wlan interface 1 mac address receive error: %i", errcode); 
	}

	return outWrite(T("%s"), if_hw);
}

static int getWlanCurrentMacAC(webs_t *wp, char** params, int nparams)
{
#ifndef CONFIG_RT_SECOND_IF_NONE
	char if_hw[18] = {0};

	int errcode = getWlanCurrentMacAddr(if_hw, 2);
	if (errcode)
	{
	    syslog(LOG_ERR, "Wlan interface 2 mac address receive error: %i", errcode);
	}

	return outWrite(T("%s"), if_hw);
#else
	return outWrite(T("00:00:00:00:00:00"));
#endif
}

static int getWlanStaInfo(webs_t *wp, char** params, int nparams)
{
	unsigned int i;
	int err = 0;
	char str[64];

	RT_802_11_MAC_TABLE table = {0};

	outWrite(T("{ "));
	if (getWlanStationTable(&table, 1) != 0) {
	    err = -1;
	    syslog(LOG_ERR, "Unable to receive wlan station table 1"); 
	}
	else {
		outWrite(T("\"stationlist24\": [ "));
		for (i = 0; i < table.Num; i++) {
			RT_802_11_MAC_ENTRY *pe = &(table.Entry[i]);
			// AID
			outWrite(T("{ \"aid\":\"%d\", "), pe->Aid);
			// MAC Address
			outWrite(T("\"mac\":\"%02X:%02X:%02X:%02X:%02X:%02X\", "), pe->Addr[0], pe->Addr[1], pe->Addr[2], pe->Addr[3], pe->Addr[4], pe->Addr[5]);
			// Connection Time
			outWrite(T("\"conntime\":\"%02u:%02u:%02u\", "), (pe->ConnectedTime / (unsigned)3600), ((pe->ConnectedTime % (unsigned)3600) / (unsigned)60), (pe->ConnectedTime % (unsigned)60));
			// Power Save mode, MIMO Power Save
			outWrite(T("\"psm\":\"%s\", \"mimo\":\"%s\", "), (pe->Psm == 0)? "NO" : "YES", (pe->MimoPs == 0)? "NO" : "YES");
			// TX MCS
			outWrite(T("\"mcs\":\"%d\", "), getMCS(pe->TxRate));
			// Bandwith
			outWrite(T("\"bw\":\"%s\", "), getBW(pe->TxRate.field.BW));
			// SGI/STBC/LDPC
			outWrite(T("\"sgi\":\"%s\", \"stbc\":\"%s\", \"ldpc\":\"%s\", "), (pe->TxRate.field.ShortGI == 0)? "NO" : "YES", (pe->TxRate.field.STBC == 0)? "NO" : "YES", (pe->TxRate.field.ldpc == 0)? "NO" : "YES");
			// HT/VHT Modes
			outWrite(T("\"mode\":\"%s\", "), getPhyMode(pe->TxRate.field.MODE));
			// Tx Rate
			outWrite(T("\"txrate\":\"%d\", "), getWlanRate(pe->TxRate));
			// RSSI
#if defined(CONFIG_RT_FIRST_IF_MT7620) || defined(CONFIG_RT_FIRST_IF_MT7602E) || defined(CONFIG_RT_FIRST_IF_MT7603E) || defined(CONFIG_MT7615_AP_DBDC_MODE)
			outWrite(T("\"rssi0\":\"%d\", \"rssi1\":\"%d\", "), (int)(pe->AvgRssi0), (int)(pe->AvgRssi1));
			outWrite(T("\"quality0\":\"%d\", \"quality1\":\"%d\", "), ConvertRssiToSignalQuality(pe->AvgRssi0), ConvertRssiToSignalQuality(pe->AvgRssi1));
#else
			outWrite(T("\"rssi0\":\"%d\", \"rssi1\":\"%d\", \"rssi2\":\"%d\", "), (int)(pe->AvgRssi0), (int)(pe->AvgRssi1), (int)(pe->AvgRssi2));
			outWrite(T("\"quality0\":\"%d\", \"quiality1\":\"%d\", \"quiality2\":\"%d\", "), ConvertRssiToSignalQuality(pe->AvgRssi0), ConvertRssiToSignalQuality(pe->AvgRssi1), ConvertRssiToSignalQuality(pe->AvgRssi2));
#endif
			// Rx/Tx bytes
			sprintf(str, "\"rxbytes\":\" %llu \", \"txbytes\":\" %llu \" }", (long long unsigned)pe->RxBytes, (long long unsigned)pe->TxBytes);
			outWrite(T("%s"), str);
			if (i < (table.Num - 1))
				outWrite(T(", "));
		}
		outWrite(T(" ]"));
	}
#ifndef CONFIG_RT_SECOND_IF_NONE
	/* second radio module */
	if (getWlanStationTable(&table, 2) != 0) {
	    err = -1;
	    syslog(LOG_ERR, "Unable to receive wlan station table 2");
	}
	else {
		if (err == 0)
			outWrite(T(", "));
		outWrite(T("\"stationlist5\": [ "));
		for (i = 0; i < table.Num; i++) {
			RT_802_11_MAC_ENTRY *pe = &(table.Entry[i]);
			// AID
			outWrite(T("{ \"aid\":\"%d\", "), pe->Aid);
			// MAC Address
			outWrite(T("\"mac\":\"%02X:%02X:%02X:%02X:%02X:%02X\", "), pe->Addr[0], pe->Addr[1], pe->Addr[2], pe->Addr[3], pe->Addr[4], pe->Addr[5]);
			// Connection Time
			outWrite(T("\"conntime\":\"%02u:%02u:%02u\", "), (pe->ConnectedTime / (unsigned)3600), ((pe->ConnectedTime % (unsigned)3600) / (unsigned)60), (pe->ConnectedTime % (unsigned)60));
			// Power Save mode, MIMO Power Save
			outWrite(T("\"psm\":\"%s\", \"mimo\":\"%s\", "), (pe->Psm == 0)? "NO" : "YES", (pe->MimoPs == 0)? "NO" : "YES");
			// TX MCS
			outWrite(T("\"mcs\":\"%d\", "), getMCS(pe->TxRate));
			// Bandwith
			outWrite(T("\"bw\":\"%s\", "), getBW(pe->TxRate.field.BW));
			// SGI/STBC/LDPC
			outWrite(T("\"sgi\":\"%s\", \"stbc\":\"%s\", \"ldpc\":\"%s\", "), (pe->TxRate.field.ShortGI == 0)? "NO" : "YES", (pe->TxRate.field.STBC == 0)? "NO" : "YES", (pe->TxRate.field.ldpc == 0)? "NO" : "YES");
			// HT/VHT Modes
			outWrite(T("\"mode\":\"%s\", "), getPhyMode(pe->TxRate.field.MODE));
			// Tx Rate
			outWrite(T("\"txrate\":\"%d\", "), getWlanRate(pe->TxRate));
			// RSSI
#if defined(CONFIG_RT_SECOND_IF_MT7610E)
			outWrite(T("\"rssi0\":\"%d\", "), (int)(pe->AvgRssi0));
			outWrite(T("\"quality0\":\"%d\", "), ConvertRssiToSignalQuality(pe->AvgRssi0));
#elif defined(CONFIG_RT_SECOND_IF_MT7612E) || defined(CONFIG_MT7615_AP_DBDC_MODE)
			outWrite(T("\"rssi0\":\"%d\", \"rssi1\":\"%d\", "), (int)(pe->AvgRssi0), (int)(pe->AvgRssi1));
			outWrite(T("\"quality0\":\"%d\", \"quality1\":\"%d\", "), ConvertRssiToSignalQuality(pe->AvgRssi0), ConvertRssiToSignalQuality(pe->AvgRssi1));
#else
			outWrite(T("\"rssi0\":\"%d\", \"rssi1\":\"%d\", \"rssi2\":\"%d\", "), (int)(pe->AvgRssi0), (int)(pe->AvgRssi1), (int)(pe->AvgRssi2));
			outWrite(T("\"quality0\":\"%d\", \"quiality1\":\"%d\", \"quiality2\":\"%d\", "), ConvertRssiToSignalQuality(pe->AvgRssi0), ConvertRssiToSignalQuality(pe->AvgRssi1), ConvertRssiToSignalQuality(pe->AvgRssi2));
#endif
			// Rx/Tx bytes
			sprintf(str, "\"rxbytes\":\" %llu \", \"txbytes\":\" %llu \" }", (long long unsigned)pe->RxBytes, (long long unsigned)pe->TxBytes);
			outWrite(T("%s"), str);
			if (i < (table.Num - 1))
				outWrite(T(", "));
		}
		outWrite(T(" ]"));
	}
#endif
	outWrite(T(" }"));
	return err;
}

static int getWlanM2UBuilt(webs_t *wp, char** params, int nparams)
{
#if defined(CONFIG_MT7610_AP_IGMP_SNOOP) || defined(CONFIG_MT76X2_AP_IGMP_SNOOP) || defined(CONFIG_MT76X3_AP_IGMP_SNOOP) || defined(CONFIG_MT7615_AP_IGMP_SNOOP)
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

// BandSteering parametrs
parameter_fetch_t band_steering_flags[] =
{
	{ "BndStrgRssiDiff", "BndStrgRssiDiff", 0, "20" },
	{ "BndStrgRssiLow", "BndStrgRssiLow", 0, "-80" },
	{ "BndStrgAge", "BndStrgAge", 0, "800000" },
	{ "BndStrgHoldTime", "BndStrgHoldTime", 0, "8000" },
	{ "BndStrgCheckTime", "BndStrgCheckTime", 0, "4000" },
	{ NULL, NULL, 0, NULL } // Terminator
};

// Fast roaming parametrs
parameter_fetch_t fast_roaming_flags[] =
{
	{ "ApProbeRspTimes", "ApProbeRspTimes", 0, ("3") },
	{ "BandDeltaRssi", "BandDeltaRssi", 0, ("-5") },
	{ "AuthRspFail", "AuthRspFail", 0, ("0") },
	{ "AuthRspRssi", "AuthRspRssi", 0, ("0") },
	{ "AssocReqRssiThres", "AssocReqRssiThres", 0, ("-70") },
	{ "AssocRspIgnor", "AssocRspIgnor", 0, ("0") },
	{ "KickStaRssiLow", "KickStaRssiLow", 0, ("-80") },
	{ "KickStaRssiLowPSM", "KickStaRssiLowPSM", 0, ("-90") },
	{ "KickStaRssiLowDelay", "KickStaRssiLowDelay", 0, ("5") },
	{ "ProbeRspRssi", "ProbeRspRssi", 0, ("-95") },
	{ NULL, NULL, 0, NULL } // Terminator
};

#if defined(CONFIG_MT7610_AP_IDS) || defined(CONFIG_MT76X2_AP_IDS) || defined(CONFIG_MT76X3_AP_IDS) || defined(CONFIG_MT7615_AP_IDS)
// IDS parametrs
parameter_fetch_t ids_flags[] =
{
	{ ("AuthFloodThreshold"), "AuthFloodThreshold", 0, ("64") },
	{ ("AssocReqFloodThreshold"), "AssocReqFloodThreshold", 0, ("64") },
	{ ("ReassocReqFloodThreshold"), "ReassocReqFloodThreshold", 0, ("64") },
	{ ("ProbeReqFloodThreshold"), "ProbeReqFloodThreshold", 0, ("64") },
	{ ("DisassocFloodThreshold"), "DisassocFloodThreshold", 0, ("64") },
	{ ("DeauthFloodThreshold"), "DeauthFloodThreshold", 0, ("64") },
	{ ("EapReqFloodThreshold"), "EapReqFloodThreshold", 0, ("64") },
	{ NULL, NULL, 0, NULL } // Terminator
};
#endif

static char* repeatValueTimes(char* buf, char* value, int times)
{
	int i;
	buf[0] = '\0';

	for (i=0; i < times; i++)
	{
		if (i > 0)
			sprintf(buf, "%s;%s", buf, value);
		else
			sprintf(buf, "%s%s", buf, value);
	}
	return buf;
}



/* goform/wirelessBasic */

static void wirelessBasic(webs_t* wp, char_t *path, char_t *query)
{
	int i;
	char_t	*wirelessmode, *mbssid_mode, *bssid_num;
	char_t	*sz11gChannel;
	char_t  *bandsteering, *token;
	char 	mbssidtmpbuf[8 * MAX_NUMBER_OF_BSSID] = "";
#if defined(CONFIG_MT7610_AP_IDS) || defined(CONFIG_MT76X2_AP_IDS) || defined(CONFIG_MT76X3_AP_IDS) || defined(CONFIG_MT7615_AP_IDS)
	char_t *ids_enable;
#endif
#ifndef CONFIG_RT_SECOND_IF_NONE
	char_t	*wirelessmodeac, *sz11aChannel, *ssid1ac;
	int     mode_ac;
#endif
	int     is_ht = 0, new_bssid_num, mode;

	char_t *rd_region, *dyn_vga;
	int ssid_num;

#if defined(CONFIG_RT_FIRST_IF_MT7602E) || defined(CONFIG_RT_SECOND_IF_MT7612E)
	char_t *dyn_vga_long, *dyn_vga_clamp;
#endif
#if defined(CONFIG_RT_SECOND_IF_MT7610E)
	char_t *dyn_vga_long;
#endif
#if defined(CONFIG_MT7610_AP_IGMP_SNOOP) || defined(CONFIG_MT76X2_AP_IGMP_SNOOP) || defined(CONFIG_MT76X3_AP_IGMP_SNOOP) || defined(CONFIG_MT7615_AP_IGMP_SNOOP)
	char_t *m2u_enable;
#if defined(CONFIG_MT7610_AP_VIDEO_TURBINE) || defined(CONFIG_MT76X2_AP_VIDEO_TURBINE) || defined(CONFIG_MT76X3_AP_VIDEO_TURBINE) || defined(CONFIG_MT7615_AP_VIDEO_TURBINE)
	char_t *video_turbine;
#endif
#endif

	// fetch from web input
	wirelessmode = websGetVar(wp, T("wirelessmode"), T("7")); //7: gn mode
	mode = atoi(wirelessmode);
	mbssid_mode = websGetVar(wp, T("mbssid_mode"), T("ra"));

	bssid_num = websGetVar(wp, T("bssid_num"), T("1"));
	new_bssid_num = atoi(bssid_num);
	if (new_bssid_num < 1 || new_bssid_num > MAX_NUMBER_OF_BSSID) {
		websError(wp, 500, T("'bssid_num' %s is out of range!"), bssid_num);
		return;
	}

	sz11gChannel = websGetVar(wp, T("sz11gChannel"), T("")); 

	bandsteering = websGetVar(wp, T("BandSteering"), T("0"));
	bandsteering = (bandsteering == NULL) ? "0" : bandsteering;
#if defined(CONFIG_MT7610_AP_IDS) || defined(CONFIG_MT76X2_AP_IDS) || defined(CONFIG_MT76X3_AP_IDS) || defined(CONFIG_MT7615_AP_IDS)
	ids_enable = websGetVar(wp, T("IdsEnable"), T("0"));
	ids_enable = (ids_enable == NULL) ? "0" : ids_enable;
#endif

#ifndef CONFIG_RT_SECOND_IF_NONE
	wirelessmodeac = websGetVar(wp, T("wirelessmodeac"), T("14")); //14: a,an/ac mode
	mode_ac = atoi(wirelessmodeac);
	sz11aChannel = websGetVar(wp, T("sz11aChannel"), T(""));
	ssid1ac = websGetVar(wp, T("mssidac_1"), T("0"));
#endif


	rd_region = websGetVar(wp, T("rd_region"), T("FCC"));
	dyn_vga = websGetVar(wp, T("dyn_vga"), T("1"));
#if defined(CONFIG_RT_FIRST_IF_MT7602E) || defined(CONFIG_RT_SECOND_IF_MT7612E)
	dyn_vga_long = websGetVar(wp, T("advDynVGALong"), T("0"));
	dyn_vga_clamp = websGetVar(wp, T("advDynVGAClamp"), T("0"));
#endif
#if defined(CONFIG_RT_SECOND_IF_MT7610E)
	dyn_vga_long = websGetVar(wp, T("advDynVGALong"), T("0"));
#endif
#if defined(CONFIG_MT7610_AP_IGMP_SNOOP) || defined(CONFIG_MT76X2_AP_IGMP_SNOOP) || defined(CONFIG_MT76X3_AP_IGMP_SNOOP) || defined(CONFIG_MT7615_AP_IGMP_SNOOP)
	m2u_enable = websGetVar(wp, T("m2u_enable"), T("0"));
#if defined(CONFIG_MT7610_AP_VIDEO_TURBINE) || defined(CONFIG_MT76X2_AP_VIDEO_TURBINE) || defined(CONFIG_MT76X3_AP_VIDEO_TURBINE) || defined(CONFIG_MT7615_AP_VIDEO_TURBINE)
	video_turbine = websGetVar(wp, T("video_turbine"), T("0"));
#endif
#endif


#ifndef CONFIG_RT_SECOND_IF_NONE
	// 11abgnac Channel or AutoSelect
	if ((0 == strlen(sz11aChannel)) && (0 == strlen(sz11gChannel)))
#else
	// bgn Channel or AutoSelect
	if (0 == strlen(sz11gChannel))
#endif
	{
		websError(wp, 500, T("'Channel' should not be empty!"));
		return;
	}

	is_ht = (mode >= 5) ? 1 : 0;

	nvram_init(RT2860_NVRAM);

#ifdef PRINT_DEBUG
	websHeader(wp);
#endif

	// Virtual iface modes
	ngx_nvram_bufset(wp,"BssidIfName", mbssid_mode);

	// BasicRate: a,an,ac: 336; bg,bgn:15; b:3; g,gn,n:351
	if (mode == 2 || mode == 8 || mode == 11 || mode == 14 || mode == 15) //a, a/an, an, a/an/ac, an/ac
		ngx_nvram_bufset(wp,"BasicRate", "336"); // a,an,ac
	else if (mode == 4 || mode == 6 || mode == 7) //g, n, gn
		ngx_nvram_bufset(wp,"BasicRate", "351");
	else if ((mode == 1)) //b
		ngx_nvram_bufset(wp,"BasicRate", "3");
	else // bg,bgn,abgn (9,0,5)
		ngx_nvram_bufset(wp,"BasicRate", "15");

#ifndef CONFIG_RT_SECOND_IF_NONE
	ngx_nvram_bufset(wp,"BasicRateINIC", "336"); // a,an,ac
#endif
	default_shown_mbssid[RT2860_NVRAM] = 0;

////////////////
	void wirelessBasic_net24() {
		// 2.4GHz enable
		char *web_radio_on = websGetVar(wp, T("radioWirelessEnabled"), T("0"));
		ngx_nvram_bufset(wp,"RadioOn", web_radio_on);

		// Network Mode (2.4GHz)
		ngx_nvram_bufset(wp,"WirelessMode", wirelessmode);

		// TX Power (2.4GHz)
		char_t *tx_power = websGetVar(wp, T("tx_power"), T("100"));
		ngx_nvram_bufset(wp,"TxPower", tx_power);

		// Channel (2.4GHz)
		if (CHK_IF_SET(sz11gChannel)) {
			ngx_nvram_bufset(wp,"Channel", sz11gChannel);
		}

		// Channel Autoselect (2.4GHz)
		char_t *g_autoselect = websGetVar(wp, T("autoselect_g"), T("0"));
		char_t *g_checktime = websGetVar(wp, T("checktime_g"), T("0"));

		if (CHK_IF_SET(sz11gChannel)) {
			if ((!strncmp(sz11gChannel, "0", 2)) && (CHK_IF_SET(g_autoselect))) {
				ngx_nvram_bufset(wp,"AutoChannelSelect", g_autoselect);
				ngx_nvram_bufset(wp,"ACSCheckTime", (CHK_IF_SET(g_checktime)) ? g_checktime : "24");
			} else {
				ngx_nvram_bufset(wp,"AutoChannelSelect", "0");
				ngx_nvram_bufset(wp,"ACSCheckTime", "0");
			}
		}

		// Channel BandWidth (2.4GHz)
		char_t *n_bandwidth = websGetVar(wp, T("n_bandwidth"), T("1"));
		if (is_ht) {
			ngx_nvram_bufset(wp,"HT_BW", n_bandwidth);
		}

#ifdef PRINT_DEBUG
		outWrite(T("<h2>mode: %s</h2><br>\n"), wirelessmode);
		outWrite(T("tx_power: %s<br>\n"), tx_power);
		outWrite(T("sz11gChannel: %s<br>\n"), sz11gChannel);

		if (is_ht) {
			outWrite(T("n_bandwidth: %s<br>\n"), n_bandwidth);
		}
#endif
	}

////////////////
	void wirelessBasic_net5() {
#ifndef CONFIG_RT_SECOND_IF_NONE
		// 5GHz enable
		char *web_radio_ac_on = websGetVar(wp, T("radioWirelessEnabledAc"), T("0"));
		ngx_nvram_bufset(wp,"RadioOnINIC", web_radio_ac_on);

		// Network Mode (5GHz)
		ngx_nvram_bufset(wp,"WirelessModeINIC", wirelessmodeac);

		// TX Power (5GHz)
		char_t *tx_power_ac = websGetVar(wp, T("tx_powerac"), T("100"));
		ngx_nvram_bufset(wp,"TxPowerINIC", tx_power_ac);

		// Channel Autoselect (5GHz)
		char_t *a_autoselect = websGetVar(wp, T("autoselect_a"), T("0"));
		char_t *a_checktime = websGetVar(wp, T("checktime_a"), T("0"));

		// Channel (5GHz)
		if (CHK_IF_SET(sz11aChannel)) {
			ngx_nvram_bufset(wp,"ChannelINIC", sz11aChannel);
		}

		// Channel & automatic channel select
		if (CHK_IF_SET(sz11aChannel)) {
			if ((!strncmp(sz11aChannel, "0", 2)) && (CHK_IF_SET(a_autoselect))) {
			    ngx_nvram_bufset(wp,"AutoChannelSelectINIC", a_autoselect);
			    ngx_nvram_bufset(wp,"ACSCheckTimeINIC", (CHK_IF_SET(a_checktime)) ? a_checktime : "24");
			} else {
			    ngx_nvram_bufset(wp,"AutoChannelSelectINIC", "0");
			    ngx_nvram_bufset(wp,"ACSCheckTimeINIC", "0");
			}
		}

		// Channel BandWidth (5GHz)
		char_t *ac_bw = websGetVar(wp, T("ac_bw"), T("1"));
		char bw_str[2] = "0";
		if (is_ht && strlen(ac_bw) == 2) {
			bw_str[0] = ac_bw[0];
			ngx_nvram_bufset(wp,"HT_BWINIC", bw_str);
			bw_str[0] = ac_bw[1];
			ngx_nvram_bufset(wp,"VHT_BW", bw_str );
		}

#ifdef PRINT_DEBUG
	outWrite(T("mode ac: %s<br>\n"), wirelessmodeac);
	outWrite(T("tx_power_ac: %s<br>\n"), tx_power_ac);
	outWrite(T("ac_bw: %s<br>\n"), ac_bw);
#endif
#endif
	}

////////////////
	void wirelessBasic_ssid() {
		char_t *hidden_ssids = websGetVar(wp, T("hidden_ssids"), T(""));
		char_t *isolated_ssids = websGetVar(wp, T("isolated_ssids"), T(""));
		char_t *mbcastisolated_ssids = websGetVar(wp, T("mbcastisolated_ssids"), T(""));
		char	hidden_ssid[2 * MAX_NUMBER_OF_BSSID] = "", noforwarding[2 * MAX_NUMBER_OF_BSSID] = "", noforwardingmbcast[2 * MAX_NUMBER_OF_BSSID] = "";
		char	ssid_web_var[8] = "mssid_\0", ssid_nvram_var[8] = "SSID\0\0\0";
		int ssid;
		i = 1;

		// Fill-in SSID
		for (ssid=0; ssid < new_bssid_num; ssid++)
		{
			ssid_web_var[6] = ssid  + '1';
			ssid_nvram_var[4] = i  + '0';
			token = (i != new_bssid_num) ? ";" : "";

			char_t *mssid = websGetVar(wp, ssid_web_var, T(""));
			if (CHK_IF_SET(mssid))
			{
				ngx_nvram_bufset(wp, ssid_nvram_var, mssid);
				STFs(RT2860_NVRAM, ssid, "WirelessMode", wirelessmode);
#ifndef CONFIG_RT_SECOND_IF_NONE
				STFs(RT2860_NVRAM, ssid, "WirelessModeINIC", wirelessmodeac);
#endif

				sprintf(hidden_ssid, "%s%s", hidden_ssid, (strchr(hidden_ssids, ssid + '0') != NULL) ? "1" : "0");
				sprintf(hidden_ssid, "%s%s", hidden_ssid, token);
				sprintf(noforwarding, "%s%s", noforwarding, (strchr(isolated_ssids, ssid + '0') != NULL) ? "1" : "0");
				sprintf(noforwarding, "%s%s", noforwarding, token);
				sprintf(noforwardingmbcast, "%s%s", noforwardingmbcast, (strchr(mbcastisolated_ssids, ssid + '0') != NULL) ? "1" : "0");
				sprintf(noforwardingmbcast, "%s%s", noforwardingmbcast, token);

				i++;
			}
		}

#ifndef CONFIG_RT_SECOND_IF_NONE
		// First SSID for iNIC
		ngx_nvram_bufset(wp,"SSID1INIC", ssid1ac);
#endif

		// SSID settings
		ngx_nvram_bufset(wp,"BssidNum", bssid_num);
		ngx_nvram_bufset(wp,"HideSSID", hidden_ssid);

		char_t *LanWifiIsolate = websGetVar(wp, T("LanWifiIsolate"), T("0"));
		ngx_nvram_bufset(wp,"LanWifiIsolate", LanWifiIsolate);

		ngx_nvram_bufset(wp,"NoForwarding", noforwarding);

		char_t *mbssidapisolated = websGetVar(wp, T("mbssidapisolated"), T("0"));
		ngx_nvram_bufset(wp,"NoForwardingBTNBSSID", mbssidapisolated);

		ngx_nvram_bufset(wp,"NoForwardingMBCast", noforwardingmbcast);
	}

////////////////

	void wirelessBasic_no_ht() {
		// Rate for a, b, g, n, ac
		// In the future need allow set this per MBSSID

		char_t *abg_rate = websGetVar(wp, T("abg_rate"), T("")); 
		int rate = atoi(abg_rate);

		if (strncmp(abg_rate, "", 1)) {
		    switch (rate) {
			case 1:
			case 6:
				ngx_nvram_bufset(wp,"HT_MCS", "0");
				break;
			case 2:
			case 9:
				ngx_nvram_bufset(wp,"HT_MCS", "1");
				break;
			case 5:
			case 12:
				ngx_nvram_bufset(wp,"HT_MCS", "2");
				break;
			case 11:
			case 18:
				ngx_nvram_bufset(wp,"HT_MCS", "3");
				break;
			case 24:
				ngx_nvram_bufset(wp,"HT_MCS", "4");
				break;
			case 36:
				ngx_nvram_bufset(wp,"HT_MCS", "5");
				break;
			case 48:
				ngx_nvram_bufset(wp,"HT_MCS", "6");
				break;
			case 54:
				ngx_nvram_bufset(wp,"HT_MCS", "7");
				break;
			default:
				ngx_nvram_bufset(wp,"HT_MCS", "33");
				break;
		    }
		}

#ifdef PRINT_DEBUG
		if (strncmp(abg_rate, "", 1)) {
			outWrite(T("abg_rate: %s<br>\n"), abg_rate);
		}
#endif
	}

	void wirelessBasic_ht() {
		if (!is_ht) {
			wirelessBasic_no_ht();
			return;
		}

		// Extension Channel
		char_t *n_extcha = websGetVar(wp, T("n_extcha"), T("0"));
		ngx_nvram_bufset(wp,"HT_EXTCHA", n_extcha);
		// HT_RGD depend at HT_HTC+ frame support
		// FIXME
		//ngx_nvram_bufset(wp,"HT_HTC", n_rdg);

		// HT TxStream
		char_t *tx_stream = websGetVar(wp, T("tx_stream"), T("1"));
		ngx_nvram_bufset(wp,"HT_TxStream", tx_stream);

		// HT RxStream
		char_t *rx_stream = websGetVar(wp, T("rx_stream"), T("1"));
		ngx_nvram_bufset(wp,"HT_RxStream", rx_stream);

		// MCS
		char_t *n_mcs = websGetVar(wp, T("n_mcs"), T("33"));
		ngx_nvram_bufset(wp,"HT_MCS", n_mcs);

		// Guard Interval
		char_t *n_gi = websGetVar(wp, T("n_gi"), T("1"));
		ngx_nvram_bufset(wp,"HT_GI", n_gi);

		// Space-Time Block Coding
		char_t *n_stbc = websGetVar(wp, T("n_stbc"), T("1"));
		ngx_nvram_bufset(wp,"HT_STBC", n_stbc);

		// Aggregation MSDU
		char_t *n_amsdu = websGetVar(wp, T("n_amsdu"), T("1"));
		ngx_nvram_bufset(wp,"HT_AMSDU", n_amsdu);

		// Auto Block ACK
		char_t *n_autoba = websGetVar(wp, T("n_autoba"), T("1"));
		ngx_nvram_bufset(wp,"HT_AutoBA", n_autoba);

		// Greenfield Mode
		char_t *n_mode = websGetVar(wp, T("n_mode"), T("1"));
		ngx_nvram_bufset(wp,"HT_OpMode", n_mode);

#ifdef PRINT_DEBUG
		outWrite(T("n_extcha: %s<br>\n"), n_extcha);
		outWrite(T("tx_stream: %s<br>\n"), tx_stream);
		outWrite(T("rx_stream: %s<br>\n"), rx_stream);
		outWrite(T("n_mcs: %s<br>\n"), n_mcs);
		outWrite(T("n_gi: %s<br>\n"), n_gi);
		outWrite(T("n_stbc: %s<br>\n"), n_stbc);
		outWrite(T("n_amsdu: %s<br>\n"), n_amsdu);
		outWrite(T("n_autoba: %s<br>\n"), n_autoba);
		outWrite(T("n_mode: %s<br>\n"), n_mode);
#endif
	}

////////////////
	void wirelessBasic_vht() {
#ifndef CONFIG_RT_SECOND_IF_NONE
		int is_vht = (mode_ac >= 14) ? 1 : 0;

		if (!is_vht)
			return;

		// BandWidth Signaling Mode
		char_t *ac_bwsig = websGetVar(wp, T("ac_bwsig"), T("1"));
		ngx_nvram_bufset(wp,"VHT_BW_SIGNAL", ac_bwsig);

		// Guard Interval
		char_t *ac_gi = websGetVar(wp, T("ac_gi"), T("1"));
		ngx_nvram_bufset(wp,"VHT_SGI", ac_gi);

		// Space-Time Block Coding
		char_t *ac_stbc = websGetVar(wp, T("ac_stbc"), T("1"));
// STBC for 1T1R module (as 7610) always disable (support only in STA mode for 1T1R)
#if (CONFIG_RT_SECOND_CARD == 7610)
		ngx_nvram_bufset(wp,"VHT_STBC", "0");
#else
		ngx_nvram_bufset(wp,"VHT_STBC", ac_stbc);
#endif

		// Low Density Parity Check (only for 7602/7612/7615)
		char_t *ac_ldpc = websGetVar(wp, T("ac_ldpc"), T("1"));
#if (CONFIG_RT_FIRST_CARD == 7602) || (CONFIG_RT_SECOND_CARD == 7612) || (CONFIG_RT_FIRST_CARD == 7615) || (CONFIG_RT_SECOND_CARD == 7615)
		ngx_nvram_bufset(wp,"VHT_LDPC", ac_ldpc);
		ngx_nvram_bufset(wp,"HT_LDPC", ac_ldpc);
#else
		ngx_nvram_bufset(wp,"VHT_LDPC", "0");
		ngx_nvram_bufset(wp,"HT_LDPC", "0");
#endif

#ifdef PRINT_DEBUG
		outWrite(T("ac_bwsig: %s<br>\n"), ac_bwsig);
		outWrite(T("ac_gi: %s<br>\n"), ac_gi);
		outWrite(T("ac_stbc: %s<br>\n"), ac_stbc);
		outWrite(T("ac_ldpc: %s<br>\n"), ac_ldpc);
#endif
#endif
	}

////////////////
	void wirelessBasic_advanced_region() {
		char_t *countrycode = websGetVar(wp, T("country_code"), T("NONE"));
		char_t *country_region = websGetVar(wp, T("country_region"), T("0"));
		char_t *regulatoryclassinic = websGetVar(wp, T("RegulatoryClassINIC"), T("1;2;3;4;0"));

		ngx_nvram_bufset(wp,"CountryRegion", country_region);
		ngx_nvram_bufset(wp,"CountryCode", countrycode);
		if (!strncmp(countrycode, "US", 3)) {
			ngx_nvram_bufset(wp,"CountryRegionABand", "0");
			ngx_nvram_bufset(wp,"RegulatoryClass", "0");
			ngx_nvram_bufset(wp,"RegulatoryClassINIC", "0");
		}
		else if (!strncmp(countrycode, "JP", 3)) {
			ngx_nvram_bufset(wp,"CountryRegionABand", "6");
			ngx_nvram_bufset(wp,"RegulatoryClassINIC", "0");
			ngx_nvram_bufset(wp,"RegulatoryClass", "0");
		}
		else if (!strncmp(countrycode, "RU", 3)) {
			ngx_nvram_bufset(wp,"CountryRegionABand", "7");
			// set regulatory class for current county see spectrum.c for Russia, subbands divided ;
			ngx_nvram_bufset(wp,"RegulatoryClass", "5;0;0;0;0");
#ifndef CONFIG_RT_SECOND_IF_NONE
			if (CHK_IF_DIGIT(websGetVar(wp, T("sz11aChannel"), T("")), 0))
			    ngx_nvram_bufset(wp,"RegulatoryClassINIC", "1;2;3;4;0");
			else
			    ngx_nvram_bufset(wp,"RegulatoryClassINIC", regulatoryclassinic);
#else
			ngx_nvram_bufset(wp,"RegulatoryClassINIC", "0");
#endif
		}
		else if (!strncmp(countrycode, "FR", 3)) {
			ngx_nvram_bufset(wp,"CountryRegionABand", "2");
			ngx_nvram_bufset(wp,"RegulatoryClassINIC", "0");
			ngx_nvram_bufset(wp,"RegulatoryClass", "0");
		}
		else if (!strncmp(countrycode, "TW", 3)) {
			ngx_nvram_bufset(wp,"CountryRegionABand", "3");
			ngx_nvram_bufset(wp,"RegulatoryClassINIC", "0");
			ngx_nvram_bufset(wp,"RegulatoryClass", "0");
		}
		else if (!strncmp(countrycode, "IE", 3)) {
			ngx_nvram_bufset(wp,"CountryRegionABand", "2");
			ngx_nvram_bufset(wp,"RegulatoryClassINIC", "0");
			ngx_nvram_bufset(wp,"RegulatoryClass", "0");
		}
		else if (!strncmp(countrycode, "HK", 3)) {
			ngx_nvram_bufset(wp,"CountryRegionABand", "0");
			ngx_nvram_bufset(wp,"RegulatoryClassINIC", "0");
			ngx_nvram_bufset(wp,"RegulatoryClass", "0");
		} else { // default unknown
			ngx_nvram_bufset(wp,"CountryRegionABand", "7");
			ngx_nvram_bufset(wp,"RegulatoryClassINIC", "0");
			ngx_nvram_bufset(wp,"RegulatoryClass", "0");
		}
#ifdef PRINT_DEBUG
		outWrite(T("countrycode: %s<br>\n"), countrycode);
#endif
	}

////////////////
	void wirelessBasic_advanced() {
		// BG Protection Mode
		char_t *bg_protection = websGetVar(wp, T("bg_protection"), T("0"));
		ngx_nvram_bufset(wp,"BGProtection", bg_protection );

		// Data Beacon Rate
		char_t *dtim = websGetVar(wp, T("dtim"), T("1"));
		ngx_nvram_bufset(wp,"DtimPeriod", dtim);

		// Fragment Threshold
		char_t *fragment = websGetVar(wp, T("fragment"), T("2346"));
		ngx_nvram_bufset(wp,"FragThreshold", fragment);

		// RTS Threshold
		char_t *rts = websGetVar(wp, T("rts"), T("2347"));
		ngx_nvram_bufset(wp,"RTSThreshold", rts);

		// Station Keep-Alive
		char_t *keepalive = websGetVar(wp, T("keepalive"), T("0"));
		char_t keepalive_str[4] = "60";

		if (NULL != keepalive) {
			int tmp = atoi(keepalive);
			if ((tmp >= 10) && (tmp <= 300))
				sprintf(keepalive_str, "%d", tmp);

			ngx_nvram_bufset(wp, "StationKeepAlive", repeatValueTimes(mbssidtmpbuf, keepalive_str, new_bssid_num));
		}

		// Preamble Type
		char_t *preamble_type = websGetVar(wp, T("preamble_type"), T("0"));
		ngx_nvram_bufset(wp,"TxPreamble", preamble_type);

		// Short Slot
		char_t *short_slot = websGetVar(wp, T("short_slot"), T("0"));
		ngx_nvram_bufset(wp,"ShortSlot", short_slot);

		// Tx Burst
		char_t *tx_burst = websGetVar(wp, T("tx_burst"), T("0"));
			//txburst and burst mode set in one place
		ngx_nvram_bufset(wp,"TxBurst", tx_burst);
			//switch to normal txburst mode (speed regression with some adapters)
		ngx_nvram_bufset(wp,"BurstMode", "0");

		// Pkt Aggregate
		char_t *pkt_aggregate = websGetVar(wp, T("pkt_aggregate"), T("0"));
		ngx_nvram_bufset(wp,"PktAggregate", pkt_aggregate);

		// Wireless Multimedia
		char_t *wmm_capable = websGetVar(wp, T("WmmCapable"), T("0"));
		ngx_nvram_bufset(wp, "WmmCapable", repeatValueTimes(mbssidtmpbuf, wmm_capable, new_bssid_num));

		// ACK Policy
		char_t *ackpolicy = websGetVar(wp, T("AckPolicy"), T("0"));
		ngx_nvram_bufset(wp, "AckPolicy", repeatValueTimes(mbssidtmpbuf, ackpolicy, new_bssid_num));

		// IEEE 802.11h support
#ifndef CONFIG_RT_SECOND_IF_NONE
#ifdef CONFIG_RT_SECOND_IF_RANGE_5GHZ
		char_t	*dot11h = websGetVar(wp, T("dot11h"), T(""));
		ngx_nvram_bufset(wp,"IEEE80211H", repeatValueTimes(mbssidtmpbuf, CHK_IF_DIGIT(dot11h, 1) ? "1" : "0", new_bssid_num));
#endif
#endif
		// 256QAM Support (2.4GHz)
		char_t *g256qam = websGetVar(wp, T("g256qam"), T(""));
		ngx_nvram_bufset(wp,"G_BAND_256QAM", repeatValueTimes(mbssidtmpbuf, CHK_IF_DIGIT(g256qam, 1) ? "1" : "0", new_bssid_num));

		// Airtime Fairness
#if defined(CONFIG_MT7615_AP_VOW_SUPPORT)
		char_t *vow_airtime_fairness = websGetVar(wp, T("vow_airtime_fairness"), T(""));
		ngx_nvram_bufset(wp,"VOW_Airtime_Ctrl_En",     repeatValueTimes(mbssidtmpbuf, CHK_IF_DIGIT(vow_airtime_fairness, 1) ? "1" : "0", new_bssid_num));
		ngx_nvram_bufset(wp,"VOW_Airtime_Fairness_En", CHK_IF_DIGIT(vow_airtime_fairness, 1) ? "1" : "0");
		ngx_nvram_bufset(wp,"VOW_WATF_Enable",         CHK_IF_DIGIT(vow_airtime_fairness, 1) ? "1" : "0");
#endif

		// Multicast TX Mode
#if defined(CONFIG_MT7610_AP_MCAST_RATE_SPECIFIC) || defined(CONFIG_MT76X2_AP_MCAST_RATE_SPECIFIC) || defined(CONFIG_MT76X3_AP_MCAST_RATE_SPECIFIC) || defined(CONFIG_MT7615_AP_MCAST_RATE_SPECIFIC)
		char_t *mcast_mode = websGetVar(wp, T("McastPhyMode"), T("2"));
		char_t *mcast_mcs = websGetVar(wp, T("McastMcs"), T("0"));
		ngx_nvram_bufset(wp,"McastPhyMode", mcast_mode);
		ngx_nvram_bufset(wp,"McastMcs", mcast_mcs);
#endif

		// Clear-Channel Assessment Energy Detection
#if defined(CONFIG_MT7615_AP) || defined(CONFIG_MT7615_AP_MODULE)
		char_t *EDCCAEnable = websGetVar(wp, T("EDCCAEnable"), T(""));
		ngx_nvram_bufset(wp,"EDCCAEnable", repeatValueTimes(mbssidtmpbuf, CHK_IF_DIGIT(EDCCAEnable, 1) ? "1" : "0", new_bssid_num));
#endif

		// Smart Carrier Sense
#if defined(CONFIG_MT7615_AP_SMART_CARRIER_SENSE) || defined(CONFIG_MT76X3_AP_SMART_CARRIER_SENSE)
		char_t *SCSEnable= websGetVar(wp, T("SCSEnable"), T(""));
		ngx_nvram_bufset(wp,"SCSEnable", SCSEnable);
#endif

		// Region settings
		wirelessBasic_advanced_region();

#ifdef PRINT_DEBUG
		outWrite(T("bg_protection: %s<br>\n"), bg_protection);
		outWrite(T("dtim: %s<br>\n"), dtim);
		outWrite(T("fragment: %s<br>\n"), fragment);
		outWrite(T("rts: %s<br>\n"), rts);
		outWrite(T("preamble_type: %s<br>\n"), preamble_type);
		outWrite(T("short_slot: %s<br>\n"), short_slot);
		outWrite(T("tx_burst: %s<br>\n"), tx_burst);
		outWrite(T("pkt_aggregate: %s<br>\n"), pkt_aggregate);
#if defined(CONFIG_MT7610_AP_MCAST_RATE_SPECIFIC) || defined(CONFIG_MT76X2_AP_MCAST_RATE_SPECIFIC) || defined(CONFIG_MT76X3_AP_MCAST_RATE_SPECIFIC) || defined(CONFIG_MT7615_AP_MCAST_RATE_SPECIFIC)
		outWrite(T("McastPhyMode: %s<br>\n"), mcast_mode);
		outWrite(T("mcast_mcs: %s<br>\n"), mcast_mcs);
#endif
#endif
	}

////////////////
	void wirelessBasic_txbf() {
#if defined(CONFIG_MT76X2_AP_TXBF_SUPPORT) || defined(CONFIG_MT7615_AP_TXBF_SUPPORT)
		// Implicit TX Beamforming
		char_t *ITxBfEn = websGetVar(wp, T("ITxBfEn"), T("1"));
		ngx_nvram_bufset(wp,"ITxBfEn", ITxBfEn);

		// Explicit TX Beamforming
		char_t *ETxBfEnCond = websGetVar(wp, T("ETxBfEnCond"), T("1"));
		ngx_nvram_bufset(wp,"ETxBfEnCond", ETxBfEnCond);

		// Multiuser TX Beamforming
		char_t *MUTxRxEnable = websGetVar(wp, T("MUTxRxEnable"), T("3"));
		ngx_nvram_bufset(wp,"MUTxRxEnable", MUTxRxEnable);
#endif
	}

////////////////
	void wirelessBasic_roaming() {
		char_t *fastroaming = websGetVar(wp, T("FastRoaming"), T("0"));
		fastroaming = (fastroaming == NULL) ? "0" : fastroaming;

		ngx_nvram_bufset(wp,"FastRoaming", fastroaming);
		if (CHK_IF_DIGIT(fastroaming, 1))
			setupParameters(wp, fast_roaming_flags, 0);

		// Auto disonnect sta if rssi low (FT clients)
		char_t *kickstarssilowft = websGetVar(wp, T("KickStaRssiLowFT"), T("0"));
		ngx_nvram_bufset(wp,"KickStaRssiLowFT", kickstarssilowft);

		// Temporary block probe/assoc req from kicked client
		char_t *tmpblockafterkick = websGetVar(wp, T("TmpBlockAfterKick"), T("14"));
		ngx_nvram_bufset(wp,"TmpBlockAfterKick", tmpblockafterkick);

		// Radio Resource Managment 802.11K
#if defined(CONFIG_MT7610_AP_DOT11K_RRM_SUPPORT) || defined(CONFIG_MT76X2_AP_DOT11K_RRM_SUPPORT) || defined(CONFIG_MT76X3_AP_DOT11K_RRM_SUPPORT) || defined(CONFIG_MT7615_AP_DOT11K_RRM_SUPPORT)
		char_t *rrm = websGetVar(wp, T("RRMEnable"), T("1"));
		ngx_nvram_bufset(wp,"RRMEnable", repeatValueTimes(mbssidtmpbuf, CHK_IF_DIGIT(rrm, 1) ? "1" : "0", new_bssid_num));
#endif

		// Fast Transition 802.11R
#if defined(CONFIG_MT7610_AP_DOT11R_FT_SUPPORT) || defined(CONFIG_MT76X2_AP_DOT11R_FT_SUPPORT) || defined(CONFIG_MT76X3_AP_DOT11R_FT_SUPPORT) || defined(CONFIG_MT7615_AP_DOT11R_FT_SUPPORT)
		char_t *ft = websGetVar(wp, T("FtSupport"), T("1"));
		ngx_nvram_bufset(wp,"FtSupport", repeatValueTimes(mbssidtmpbuf, CHK_IF_DIGIT(ft, 1) ? "1" : "0", new_bssid_num));
#endif

		// Wireless Network Management
#if defined(CONFIG_MT7615_AP_WNM_SUPPORT)
		char_t *WNMEnable = websGetVar(wp, T("WNMEnable"), T(""));
		ngx_nvram_bufset(wp,"WNMEnable", repeatValueTimes(mbssidtmpbuf, CHK_IF_DIGIT(WNMEnable, 1) ? "1" : "0", new_bssid_num));
#endif

		// Mobility Domain ID, APs Key Holder
#if defined(CONFIG_MT7610_AP_DOT11R_FT_SUPPORT) || defined(CONFIG_MT76X2_AP_DOT11R_FT_SUPPORT) || defined(CONFIG_MT76X3_AP_DOT11R_FT_SUPPORT) || defined(CONFIG_MT7615_AP_DOT11R_FT_SUPPORT)
		char_t *ftmdid = websGetVar(wp, T("FtMdId"), T("A1"));
		char_t *ftr0khid = websGetVar(wp, T("FtR0khId"), T("4f577274"));
		char 	ft_mdid_var[8] = "FtMdId\0\0";
		char 	ft_r0khid_var[10] = "FtR0khId\0\0";

		if (CHK_IF_DIGIT(ft, 1)) {
			for (i=1;i<=4;i++) {
				ft_mdid_var[6] = '0' + i;
				ft_r0khid_var[8] = '0' + i;
				ngx_nvram_bufset(wp, ft_mdid_var, ftmdid);
				ngx_nvram_bufset(wp, ft_r0khid_var, ftr0khid);
			}
		}
#endif

		// Maximum clients per SSID
		char_t *maxstanum = websGetVar(wp, T("maxstanum"), T("0"));
		char_t maxstanum_str[8] = "";
		sprintf(maxstanum_str, "%d", MAX_NUMBER_OF_MAC);

		if (NULL != maxstanum) {
			int tmp = atoi(maxstanum);
			if ((tmp >= 0) && (tmp <= MAX_NUMBER_OF_MAC))
				sprintf(maxstanum_str, "%d", tmp);

			ngx_nvram_bufset(wp, "MaxStaNum", repeatValueTimes(mbssidtmpbuf, maxstanum_str, new_bssid_num));
		}

		// Idle timeout before disconnect
		char_t *idletimeout = websGetVar(wp, T("idletimeout"), T("0"));
		char_t idletimeout_str[8] = "480";

		if (NULL != idletimeout) {
			int tmp = atoi(idletimeout);
			if ((tmp >= 120) && (tmp <= 600))
				sprintf(idletimeout_str, "%d", tmp);

			ngx_nvram_bufset(wp, "IdleTimeout", idletimeout_str);
		}

		// TX retry fail before disconnect
		char_t *life_check = websGetVar(wp, T("EntryLifeCheck"), T("0"));
		char_t life_check_str[8] = "1024";

		if (NULL != life_check) {
			int tmp = atoi(life_check);
			if (tmp < 256)
				tmp = 256;

			if (tmp > 4096)
				tmp = 4096;

			sprintf(life_check_str, "%d", tmp);
			ngx_nvram_bufset(wp,"EntryLifeCheck", life_check_str);
		}

		// Beacon Interval
		char_t *beacon = websGetVar(wp, T("beacon"), T("100"));
		ngx_nvram_bufset(wp,"BeaconPeriod", beacon);

		// Beacon Interval (5GHz)
		char_t *beaconinic = websGetVar(wp, T("beaconINIC"), T("100"));
		ngx_nvram_bufset(wp,"BeaconPeriodINIC", beaconinic);

		// PMK Cache Period
		char_t *PMKCachePeriod = websGetVar(wp, T("PMKCachePeriod"), T("480"));
		int cachePeriod = strToIntDef(PMKCachePeriod, -1);
		if (cachePeriod > 0 && cachePeriod < 10000)
		{
			ngx_nvram_bufset(wp, "PMKCachePeriod", repeatValueTimes(mbssidtmpbuf, PMKCachePeriod, new_bssid_num));
		}

#ifdef PRINT_DEBUG
		outWrite(T("beacon: %s<br>\n"), beacon);
#endif
	}

////////////////
	wirelessBasic_net24();
	wirelessBasic_net5();
	wirelessBasic_ssid();
	wirelessBasic_ht();
	wirelessBasic_vht();
	wirelessBasic_advanced();
	wirelessBasic_txbf();
	wirelessBasic_roaming();

	// Band Steering
	ngx_nvram_bufset(wp,"BandSteering", bandsteering);
	if (CHK_IF_DIGIT(bandsteering, 1))
		setupParameters(wp, band_steering_flags, 0);


#if defined(CONFIG_MT7610_AP_IDS) || defined(CONFIG_MT76X2_AP_IDS) || defined(CONFIG_MT76X3_AP_IDS) || defined(CONFIG_MT7615_AP_IDS)
	ngx_nvram_bufset(wp,"IdsEnable", ids_enable);
	if (CHK_IF_DIGIT(ids_enable, 1))
		setupParameters(wp, ids_flags, 0);
#endif

	ssid_num = ngx_nvram_get_int(wp, "BssidNum", 1);

	//Radar Detect region
	if ((rd_region == NULL) || (strlen(rd_region)<=0))
		rd_region = "FCC";

	//set to nvram
	nvram_init(RT2860_NVRAM);
	ngx_nvram_bufset(wp,"RDRegion", rd_region);

	ngx_nvram_bufset(wp,"DyncVgaEnable", dyn_vga);
#if defined(CONFIG_RT_FIRST_IF_MT7602E) || defined(CONFIG_RT_SECOND_IF_MT7612E)
	ngx_nvram_bufset(wp,"SkipLongRangeVga", dyn_vga_long);
	ngx_nvram_bufset(wp,"VgaClamp", dyn_vga_clamp);
#endif
#if defined(CONFIG_RT_SECOND_IF_MT7610E)
	ngx_nvram_bufset(wp,"SkipLongRangeVga", dyn_vga_long);
#endif
#if defined(CONFIG_MT7610_AP_IGMP_SNOOP) || defined(CONFIG_MT76X2_AP_IGMP_SNOOP) || defined(CONFIG_MT76X3_AP_IGMP_SNOOP) || defined(CONFIG_MT7615_AP_IGMP_SNOOP)
	ngx_nvram_bufset(wp,"M2UEnabled", m2u_enable);
#if defined(CONFIG_MT7610_AP_VIDEO_TURBINE) || defined(CONFIG_MT76X2_AP_VIDEO_TURBINE) || defined(CONFIG_MT76X3_AP_VIDEO_TURBINE) || defined(CONFIG_MT7615_AP_VIDEO_TURBINE)
	ngx_nvram_bufset(wp,"VideoTurbine", video_turbine);
#endif
#endif

	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	setSecurity(wp, RT2860_NVRAM);

#ifdef PRINT_DEBUG
	// debug print
	outWrite(T("ssid: %s, bssid_num: %s<br>\n"), ssid, bssid_num);
	outWrite(T("mbssidapisolated: %s<br>\n"), mbssidapisolated);
	outWrite(T("sz11aChannel: %s<br>\n"), sz11aChannel);
#ifndef CONFIG_RT_SECOND_IF_NONE
	outWrite(T("mssidac_1: %s<br>\n"), ssid1ac);
#endif
	outWrite(T("rd_region: %s<br>\n"), rd_region);
#if defined(CONFIG_MT7610_AP_IGMP_SNOOP) || defined(CONFIG_MT76X2_AP_IGMP_SNOOP) || defined(CONFIG_MT76X3_AP_IGMP_SNOOP) || defined(CONFIG_MT7615_AP_IGMP_SNOOP)
	outWrite(T("m2u_enable: %s<br>\n"), m2u_enable);
#endif
#endif
	// reconfigure system
	websDone(wp, 200);
	wp->on_response_ok = DO_RECONFIGURE;
}

static int getVideoTurbineBuilt(webs_t *wp, char** params, int nparams)
{
#if defined(CONFIG_MT7610_AP_VIDEO_TURBINE) || defined(CONFIG_MT76X2_AP_VIDEO_TURBINE) || defined(CONFIG_MT76X3_AP_VIDEO_TURBINE) || defined(CONFIG_MT7615_AP_VIDEO_TURBINE)
	outWrite(T("1"));
#else
	outWrite(T("0"));
#endif
	return 0;
}

static int getIdsEnableBuilt(webs_t *wp, char** params, int nparams)
{
#if defined(CONFIG_MT7610_AP_IDS) || defined(CONFIG_MT76X2_AP_IDS) || defined(CONFIG_MT76X3_AP_IDS) || defined(CONFIG_MT7615_AP_IDS)
	outWrite(T("1"));
#else
	outWrite(T("0"));
#endif
	return 0;
}

#if defined(CONFIG_MT7610_AP_WDS) || defined(CONFIG_MT76X2_AP_WDS) || defined(CONFIG_MT76X3_AP_WDS) || defined(CONFIG_MT7615_AP_WDS)
parameter_fetch_t wds_args[] =
{
	{ ("wds_interface"),   "WdsIfName",     0, ("")  },
	{ ("wds_phy_mode"),    "WdsPhyMode",    0, ("")  },
	{ ("wds_encryp_type"), "WdsEncrypType", 0, ("")  },
	{ ("wds_encryp_key0"), "Wds0Key",       0, ("")  },
	{ ("wds_encryp_key1"), "Wds1Key",       0, ("")  },
	{ ("wds_encryp_key2"), "Wds2Key",       0, ("")  },
	{ ("wds_encryp_key3"), "Wds3Key",       0, ("")  },
	{ ("wds_num"),         "WdsNum",        0, ("1") },
	{ NULL,                 NULL,            0, NULL   }
};

/* goform/wirelessWds */
static void wirelessWds(webs_t* wp, char_t *path, char_t *query)
{
	char_t *wds_mode = websGetVar(wp,  T("wds_mode"), T("0"));
	char_t *wds_list = websGetVar(wp,  T("wds_list"), T(""));
	char_t *reset    = websGetVar(wp,  T("reset"), T("0"));

	if (!CHK_IF_DIGIT(reset, 0)) {
		nvram_fromdef(RT2860_NVRAM, 10, "WdsEnable", "WdsList", "WdsIfName", "WdsPhyMode", "WdsEncrypType",
						"Wds0Key", "Wds1Key", "Wds2Key", "Wds3Key", "WdsNum");
	}
	else {
		nvram_init(RT2860_NVRAM);
		ngx_nvram_bufset(wp,"WdsEnable", wds_mode);
		if (!CHK_IF_DIGIT(wds_mode, 0)) {
			setupParameters(wp, wds_args, 0);
			if (CHK_IF_DIGIT(wds_mode, 2) || CHK_IF_DIGIT(wds_mode, 3)) {
				if (0 != strlen(wds_list))
					ngx_nvram_bufset(wp,"WdsList", wds_list);
			} else {
				ngx_nvram_bufset(wp,"WdsList", "");
			}
		}
		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}

        wp->on_response_ok = DO_RECONFIGURE;
//	websHeader(wp);
	websDone(wp, 204);
}
#endif

#if defined(CONFIG_MT7610_AP_APCLI) || defined(CONFIG_MT76X2_AP_APCLI) || defined(CONFIG_MT76X3_AP_APCLI) || defined(CONFIG_MT7615_AP_APCLI)
parameter_fetch_t apcli_args[] =
{
	{ ("apcli_interface"),         "ApCliIfName",          0,       ("")         },
	{ ("apcli_ssid"),              "ApCliSsid",            0,       ("")         },
	{ ("apcli_bssid"),             "ApCliBssid",           0,       ("")         },
	{ ("apcli_mode"),              "ApCliAuthMode",        0,       ("OPEN")     },
	{ ("apcli_enc"),               "ApCliEncrypType",      0,       ("NONE")     },
	{ ("apcli_wpapsk"),            "ApCliWPAPSK",          0,       ("12345678") },
	{ ("apcli_autoscan"),          "ApCliAutoConnect",     2,       ("off")      },
	{ ("apcli_apiface"),           "ApCliClientOnly",      2,       ("off")      },
	{ ("apcli_bridge"),            "ApCliBridgeOnly",      2,       ("off")      },
	{ NULL,                         NULL,                   0,       NULL          }
};

/* goform/wirelessApcli */
static void wirelessApcli(webs_t* wp, char_t *path, char_t *query)
{
	char_t *reset        = websGetVar(wp,  T("reset"), T("0"));
	char_t *apcli_enable = websGetVar(wp,  T("apcli_enable"), T("0"));

	if (CHK_IF_DIGIT(reset, 1)) {
		nvram_fromdef(RT2860_NVRAM, 10, "ApCliEnable", "ApCliIfName", "ApCliSsid", "ApCliBssid", "ApCliAuthMode", 
						"ApCliEncrypType", "ApCliWPAPSK", "ApCliAutoConnect", "ApCliClientOnly", "ApCliBridgeOnly");
//	    websHeader(wp);
	    websDone(wp, 204);
	}
	else {
		if (strcmp(apcli_enable, "on") == 0)
		    apcli_enable="1";
		else
		    apcli_enable="0";

		nvram_init(RT2860_NVRAM);
		ngx_nvram_bufset(wp,"ApCliEnable", apcli_enable);
		setupParameters(wp, apcli_args, 0);
		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);

		/* prevent old apcliif start before band select */
		doSystem("service wan stop");

		/* reconfigure system */
                wp->on_response_ok = DO_RECONFIGURE;

//		websHeader(wp);
		websDone(wp, 204);
	}
}

static int getAPCliStatus(webs_t *wp, char** params, int nparams)
{
	char addr[ETH_ALEN] = { 0 };
	char str[17];
	char *apcliifname = ngx_nvram_get(wp, "ApCliIfName");
	int ap_ret;

	if (!strcmp(apcliifname, "apcli0")) {
		ap_ret = getWlanAPMac("apcli0", addr);
	}
	else {
		ap_ret = getWlanAPMac("apclii0", addr);
	}

	if (ap_ret <= 0)
		outWrite(T("%i"), ap_ret);
	else {
		sprintf(str, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
		outWrite(T("%s"), str);
	}

	return ap_ret;
}
#endif

static void getSecurity(webs_t* wp, char_t *path, char_t *query)
{
	int i;
        unsigned int j;
        int nvram = RT2860_NVRAM;

	char_t	*PreAuth, *AuthMode, *EncrypType, *DefaultKeyID, *Key1Type, *Key2Type,
		*Key3Type, *Key4Type, *RekeyMethod, *RekeyInterval, *PMFMFPC, *PMFMFPR, *PMFSHA256,
		*RADIUS_Server, *RADIUS_Port, *RADIUS_Key, *STR;
	char	str[64];

	int num_ssid = ngx_nvram_get_int(wp, "BssidNum", 1);
	char_t result[16384];

	result[0] = '\0';

	if(default_shown_mbssid[nvram] > num_ssid)
		default_shown_mbssid[nvram] = 0;
	sprintf(result, "{ \"default_mbssid\":\"%d\", ",  default_shown_mbssid[nvram]);

	strcat(result, "\"data\": [ ");
	if (RT2860_NVRAM == nvram) {
		for(i = 0; i < num_ssid; i++) {
			strcat(result, "{ ");

			strcat(result, "\"SSID\":\"");
			str[0] = '\0';
			STR = ngx_nvram_get(wp, racat("SSID", i+1));
			for (j = 0; j < strlen(STR); j++) {
				if (STR[j] == '"' || STR[j] == '\\')
					sprintf(str, "%s%c", str, '\\');
				sprintf(str, "%s%c", str, STR[j]);
			}
			strcat(result, str);
			strcat(result, "\", ");

			strcat(result, "\"PreAuth\":\"");
			LFF(result, nvram, PreAuth, i);
			strcat(result, "\", ");

			strcat(result, "\"AuthMode\":\"");
			LFF(result, nvram, AuthMode, i);
			strcat(result, "\", ");

			strcat(result, "\"EncrypType\":\"");
			LFF(result, nvram, EncrypType, i);
			strcat(result, "\", ");

			strcat(result, "\"DefaultKeyID\":\"");
			LFF(result, nvram, DefaultKeyID, i);
			strcat(result, "\", ");

			strcat(result, "\"Key1Type\":\"");
			LFF(result, nvram, Key1Type, i);
			strcat(result, "\", ");

			strcat(result, "\"Key1Str\":\"");
			strcat(result, ngx_nvram_get(wp, racat("Key1Str", i + 1)));
			strcat(result, "\", ");

			strcat(result, "\"Key2Type\":\"");
			LFF(result, nvram, Key2Type, i);
			strcat(result, "\", ");

			strcat(result, "\"Key2Str\":\"");
			strcat(result, ngx_nvram_get(wp, racat("Key2Str", i + 1)));
			strcat(result, "\", ");

			strcat(result, "\"Key3Type\":\"");
			LFF(result, nvram, Key3Type, i);
			strcat(result, "\", ");

			strcat(result, "\"Key3Str\":\"");
			strcat(result, ngx_nvram_get(wp, racat("Key3Str", i + 1)));
			strcat(result, "\", ");

			strcat(result, "\"Key4Type\":\"");
			LFF(result, nvram, Key4Type, i);
			strcat(result, "\", ");

			strcat(result, "\"Key4Str\":\"");
			strcat(result, ngx_nvram_get(wp, racat("Key4Str", i + 1)));
			strcat(result, "\", ");

			strcat(result, "\"WPAPSK\":\"");
			str[0] = '\0';
			STR = ngx_nvram_get(wp, racat("WPAPSK", i + 1));
			for (j = 0; j < strlen(STR); j++) {
				if (STR[j] == '"' || STR[j] == '\\')
					sprintf(str, "%s%c", str, '\\');
				sprintf(str, "%s%c", str, STR[j]);
			}
			strcat(result, str);
			strcat(result, "\", ");

			strcat(result, "\"RekeyMethod\":\"");
			LFF(result, nvram, RekeyMethod, i);
			strcat(result, "\", ");

			strcat(result, "\"RekeyInterval\":\"");
			LFF(result, nvram, RekeyInterval, i);
			strcat(result, "\", ");

			strcat(result, "\"PMFMode\":\"");

			str[0] = '\0';
			PMFMFPC = ngx_nvram_get(wp, "PMFMFPC");
			PMFMFPR = ngx_nvram_get(wp, "PMFMFPR");
			if (PMFMFPC && (getNthValueSafe(i, PMFMFPC, ';', str, 64) != -1) && str[0] == '1') {
				strcat(result, "1");
			}
			else
			{
				str[0] = '\0';
				if (PMFMFPR && (getNthValueSafe(i, PMFMFPR, ';', str, 64) != -1) && str[0] == '1') {
					strcat(result, "2");
				}
				else
				{
					strcat(result, "0");
				}
			}

			strcat(result, "\", ");

			strcat(result, "\"PMFSHA256\":\"");
			LFF(result, nvram, PMFSHA256, i);
			strcat(result, "\", ");

			strcat(result, "\"RADIUS_Server\":\"");
			LFF(result, nvram, RADIUS_Server, i);
			strcat(result, "\", ");

			strcat(result, "\"RADIUS_Port\":\"");
			LFF(result, nvram, RADIUS_Port, i);
			strcat(result, "\", ");

			strcat(result, "\"RADIUS_Key\":\"");
			LFF(result, nvram, RADIUS_Key, i);
			strcat(result, "\", ");

			strcat(result, "\"session_timeout_interval\":\"");
			strcat(result, ngx_nvram_get(wp, "session_timeout_interval"));
			strcat(result, "\", ");

			strcat(result, "\"AccessPolicy\":\"");
			strcat(result, ngx_nvram_get(wp, racat("AccessPolicy", i)));
			strcat(result, "\", ");

			strcat(result, "\"AccessControlList\":\"");
			strcat(result, ngx_nvram_get(wp, racat("AccessControlList", i)));
			strcat(result, "\"");

			strcat(result, " }");
			if (i < num_ssid - 1)
				strcat(result, ", ");
		}
	}
	strcat(result, " ] }");

        websSetContentType(wp, "text/plain");

//	outWrite(WEBS_CACHE_CONTROL_STRING);
//	outWrite(T("\n"));
	outWrite(T("%s"), result);
	websDone(wp, 200);
}

static void wirelessGetSecurity(webs_t* wp, char_t *path, char_t *query)
{
	return getSecurity(wp, path, query);
}

static int getSSIDsList(webs_t *wp, char** params, int nparams)
{
	int i;
        unsigned int j;
	char *ssid;
	char SSID[64];
	int num_ssid = ngx_nvram_get_int(wp, "BssidNum", 1);
	char *mbssid = ngx_nvram_get(wp, "BssidIfName");

	outWrite(T("{ \"wireless\": [ "));
	for(i = 0; i < num_ssid; i++) {
		ssid = ngx_nvram_get(wp, racat("SSID", i + 1));
		if ((i == 0) && (strcmp(mbssid, "rai") == 0)) {
			ssid = ngx_nvram_get(wp, "SSID1INIC");
		}
		SSID[0] = '\0';
		for (j = 0; j < strlen(ssid); j++) {
			if (ssid[j] == '"' || ssid[j] == '\\')
				sprintf(SSID, "%s%c", SSID, '\\');
			sprintf(SSID, "%s%c", SSID, ssid[j]);
		}
		outWrite(T("{ \"ssid\":\"%s\" }%s"), SSID, (i + 1  == num_ssid) ? "" : ", ");
	}
	outWrite(T(" ] }"));
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

static void conf8021x(webs_t* wp, int nvram, int mbssid)
{
	char lan_if_addr[32];

	if (getIfIp(getLanIfName(), lan_if_addr) != -1) {
	    nvram_bufset(nvram, "own_ip_addr", lan_if_addr);
	    /* temp static code raius server at LAN network, in future need select in UI */
	    nvram_bufset(nvram, "EAPifname", "br0");
	    nvram_bufset(nvram, "PreAuthifname", "br0");
	}

	char_t *RadiusServerIP = websGetVar(wp,  racat("RADIUS_Server", mbssid), T(""));
	char_t *RadiusServerPort = websGetVar(wp,  racat("RADIUS_Port", mbssid), T("1812"));
	char_t *RadiusServerSecret = websGetVar(wp,  racat("RADIUS_Key", mbssid), T("wive-ng-mt"));
	char_t *RadiusServerSessionTimeout = websGetVar(wp,  racat("RADIUS_SessionTimeout", mbssid), T("0"));

	if(!strlen(RadiusServerSessionTimeout))
		RadiusServerSessionTimeout = "0";

	if(strlen(RadiusServerIP)) {
	    STFs(nvram, mbssid, "RADIUS_Server", RadiusServerIP);
	    STFs(nvram, mbssid, "RADIUS_Port", RadiusServerPort);
	    STFs(nvram, mbssid, "RADIUS_Key", RadiusServerSecret);
	    nvram_bufset(nvram, "session_timeout_interval", RadiusServerSessionTimeout);
	}

}
#endif

static void confWEP(webs_t* wp, int nvram, int mbssid)
{
	char_t *DefaultKeyID = websGetVar(wp, racat("DefaultKeyID", mbssid), T(""));
	char_t *Key1Str = websGetVar(wp, racat("Key1Str", mbssid), T(""));
	char_t *Key2Str = websGetVar(wp, racat("Key2Str", mbssid), T(""));
	char_t *Key3Str = websGetVar(wp, racat("Key3Str", mbssid), T(""));
	char_t *Key4Str = websGetVar(wp, racat("Key4Str", mbssid), T(""));

	char_t *Key1Type = websGetVar(wp, racat("Key1Type", mbssid), T(""));
	char_t *Key2Type = websGetVar(wp, racat("Key2Type", mbssid), T(""));
	char_t *Key3Type = websGetVar(wp, racat("Key3Type", mbssid), T(""));
	char_t *Key4Type = websGetVar(wp, racat("Key4Type", mbssid), T(""));

	STF(nvram, mbssid, DefaultKeyID);
	STF(nvram, mbssid, Key1Type);
	STF(nvram, mbssid, Key2Type);
	STF(nvram, mbssid, Key3Type);
	STF(nvram, mbssid, Key4Type);

	nvram_bufset(nvram, racat("Key1Str", mbssid+1), Key1Str);
	nvram_bufset(nvram, racat("Key2Str", mbssid+1), Key2Str);
	nvram_bufset(nvram, racat("Key3Str", mbssid+1), Key3Str);
	nvram_bufset(nvram, racat("Key4Str", mbssid+1), Key4Str);
}

static void confWPAGeneral(webs_t* wp, int nvram, int mbssid)
{
	char_t *encType = websGetVar(wp, racat("EncrypType", mbssid), T("AES"));
	char_t *key_renewal_interval = websGetVar(wp, racat("RekeyInterval", mbssid), T(""));

	char_t *pmf_mode = websGetVar(wp, racat("PMFMode", mbssid), T(""));
	char_t *pmf_sha = websGetVar(wp, racat("PMFSHA256_", mbssid), T(""));

	STFs(nvram, mbssid, "EncrypType", encType);

/*
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
*/

	STFs(nvram, mbssid, "PMFMFPC", (pmf_mode[0] == '1')?"1":"0");
	STFs(nvram, mbssid, "PMFMFPR", (pmf_mode[0] == '2')?"1":"0");

	STFs(nvram, mbssid, "PMFSHA256", pmf_sha);

	STFs(nvram, mbssid, "DefaultKeyID", "1");
	STFs(nvram, mbssid, "RekeyInterval", key_renewal_interval);
	STFs(nvram, mbssid, "RekeyMethod", "TIME");
	STFs(nvram, mbssid, "IEEE8021X", "0");
}

static void setSecurity(webs_t* wp, int nvram)
{
	int mbssid, i;
	char_t *AccessPolicy, *AccessControlList;
	char_t *reset = websGetVar(wp,  T("reset"), T("0"));
	int bssid_num = strToIntDef(websGetVar(wp, T("bssid_num"), T("")), 1);
	bssid_num = (bssid_num > MAX_NUMBER_OF_BSSID) ? MAX_NUMBER_OF_BSSID : bssid_num;

	if (!CHK_IF_DIGIT(reset, 0)) {
		nvram_init(RT2860_NVRAM);
		for (i = 0; i < MAX_NUMBER_OF_BSSID; i++) {
			nvram_bufset(nvram, racat("Key1Str", i), "");
			nvram_bufset(nvram, racat("Key2Str", i), "");
			nvram_bufset(nvram, racat("Key3Str", i), "");
			nvram_bufset(nvram, racat("Key4Str", i), "");
			nvram_bufset(nvram, racat("WPAPSK", i + 1), "");
			nvram_bufset(nvram, racat("AccessPolicy", i), "0");
			nvram_bufset(nvram, racat("AccessControlList", i), "");
		}
		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
		nvram_fromdef(RT2860_NVRAM, 16, "PreAuth", "AuthMode", "EncrypType", "DefaultKeyID", "RekeyMethod",
						"RekeyInterval", "session_timeout_interval", "Key1Type", "Key2Type", "Key3Type",
						"Key4Type", "WPAPSK1", "WPAPSK1INIC", "PMFMFPC", "PMFMFPR", 
						"PMFSHA256");
		default_shown_mbssid[nvram] = 0;
#ifdef CONFIG_USER_802_1X
		for (i = 0; i < MAX_NUMBER_OF_BSSID; i++) {
			/* clear Radius settings */
			clearRadiusSetting(nvram, i);
		}
#endif
	}
	else {
	nvram_fromdef(RT2860_NVRAM, 13, "AuthMode", "EncrypType", "RekeyInterval", "RADIUS_Port", "RADIUS_Key",
					"RADIUS_Server", "IEEE8021X", "PreAuth", "RekeyMethod", "DefaultKeyID",
					"PMFMFPC", "PMFMFPR", "PMFSHA256");

	for (mbssid = 0; mbssid < bssid_num; mbssid++) {
		char_t *security_mode = websGetVar(wp,  racat("AuthMode", mbssid), T(""));
		if (!security_mode || security_mode[0] == '\0') break;

#ifdef CONFIG_USER_802_1X
		/* clear Radius settings */
		clearRadiusSetting(nvram, mbssid);
#endif
		nvram_init(RT2860_NVRAM);

		if (!strcmp(security_mode, "Disable")) {				// !------------------       Disable Mode --------------
			STFs(nvram, mbssid, "AuthMode", "OPEN");
			STFs(nvram, mbssid, "EncrypType", "NONE");
			STFs(nvram, mbssid, "IEEE8021X", "0");
		}
		else if( !strcmp(security_mode, "WEPAUTO")) {				// !------------------       WEP Auto Mode ----------------
			confWEP(wp,RT2860_NVRAM, mbssid);
			STFs(nvram, mbssid, "AuthMode", security_mode);
			STFs(nvram, mbssid, "EncrypType", "WEP");
			STFs(nvram, mbssid, "IEEE8021X", "0");
		}
		else if(!strcmp(security_mode, "WPA") ||
			!strcmp(security_mode, "WPA1WPA2")) {				// !------------------       WPA Enterprise Mode ----------------
#ifdef CONFIG_USER_802_1X
			conf8021x(wp, RT2860_NVRAM, mbssid);					// !------------------       WPA1WPA2 mixed mode
#endif
			confWPAGeneral(wp, RT2860_NVRAM, mbssid);

			STFs(nvram, mbssid, "AuthMode", security_mode);
			STFs(nvram, mbssid, "IEEE8021X", "0");
		}
		else if(!strcmp(security_mode, "WPAPSK")) {				// !------------------       WPA Personal Mode ----------------
			char *pass_phrase_str = websGetVar(wp, racat("passphrase", mbssid), "");
			char *pass_phrase_inic_str = websGetVar(wp, "passphraseinic", "");

			confWPAGeneral(wp, RT2860_NVRAM, mbssid);
			STFs(nvram, mbssid, "AuthMode", security_mode);
			STFs(nvram, mbssid, "IEEE8021X", "0");
			nvram_bufset(nvram, racat("WPAPSK", mbssid+1), pass_phrase_str);
			nvram_bufset(nvram, "WPAPSK1INIC", pass_phrase_inic_str);
		}
		else if(!strcmp(security_mode, "WPA2")) {				// !------------------        WPA2 Enterprise Mode ----------------
			char *pass_phrase_str = websGetVar(wp, racat("passphrase", mbssid), "");
			char *pass_phrase_inic_str = websGetVar(wp, "passphraseinic", "");
			char *PreAuth = websGetVar(wp, racat("PreAuth", mbssid), "0");
#ifdef CONFIG_USER_802_1X
			conf8021x(wp, RT2860_NVRAM, mbssid);
#endif
			confWPAGeneral(wp, RT2860_NVRAM, mbssid);

			STFs(nvram, mbssid, "AuthMode", security_mode);
			STFs(nvram, mbssid, "IEEE8021X", "0");
			STFs(nvram, mbssid, "PreAuth", PreAuth);

			nvram_bufset(nvram, racat("WPAPSK", mbssid + 1), pass_phrase_str);
			nvram_bufset(nvram, "WPAPSK1INIC", pass_phrase_inic_str);
		}
		else if(!strcmp(security_mode, "WPA2PSK") ||				// !------------------       WPA2 Personal Mode ----------------
			!strcmp(security_mode, "WPAPSKWPA2PSK") ){ 			// !------------------       WPA PSK WPA2 PSK mixed
			char *pass_phrase_str = websGetVar(wp, racat("passphrase", mbssid), "");
			char *pass_phrase_inic_str = websGetVar(wp, "passphraseinic", "");

			confWPAGeneral(wp, RT2860_NVRAM, mbssid);

			STFs(nvram, mbssid, "AuthMode", security_mode);
			STFs(nvram, mbssid, "IEEE8021X", "0");
			nvram_bufset(nvram, racat("WPAPSK", mbssid+1), pass_phrase_str);
			nvram_bufset(nvram, "WPAPSK1INIC", pass_phrase_inic_str);
		}
		else
			break;
		// Access Policy
		for (i = 0; i < MAX_NUMBER_OF_BSSID; i++) {
			AccessPolicy = websGetVar(wp,  racat("AccessPolicy", i), T("0"));
			AccessControlList = websGetVar(wp,  racat("AccessControlList", i), T("0"));
			nvram_bufset(nvram, racat("AccessPolicy", i), AccessPolicy);
			nvram_bufset(nvram, racat("AccessControlList", i), AccessControlList);
		}

		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}//for

	}
}

static int is4t4r(webs_t *wp, char** params, int nparams)
{
#if (((CONFIG_RT_FIRST_CARD == 7615) || (CONFIG_RT_SECOND_CARD == 7615)) && !defined(CONFIG_MT7615_AP_DBDC_MODE))
	outWrite(T("1"));
#else
	outWrite(T("0"));
#endif
	return 0;
}

static int is5gh_support(webs_t *wp, char** params, int nparams)
{
#ifndef CONFIG_RT_SECOND_IF_NONE
	outWrite(T("1"));
#else
	outWrite(T("0"));
#endif
	return 0;
}

static int is5gh_1t1r(webs_t *wp, char** params, int nparams)
{
#if defined(CONFIG_MT7610_AP_MODULE)
	outWrite(T("1"));
#else
	outWrite(T("0"));
#endif
	return 0;
}

static int getLDPCBuilt(webs_t *wp, char** params, int nparams)
{
#if (CONFIG_RT_FIRST_CARD == 7602) || (CONFIG_RT_SECOND_CARD == 7612) || (CONFIG_RT_FIRST_CARD == 7615) || (CONFIG_RT_SECOND_CARD == 7615)
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getTXBFBuilt(webs_t *wp, char** params, int nparams)
{
#if defined(CONFIG_MT76X2_AP_TXBF_SUPPORT) || defined(CONFIG_MT7615_AP_TXBF_SUPPORT)
	outWrite(T("1"));
#else
	outWrite(T("0"));
#endif
	return 0;
}

static int get802_1XBuilt(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_USER_802_1X
	outWrite(T("1"));
#else
	outWrite(T("0"));
#endif /* CONFIG_USER_802_1X */
    return 0;
}

static void disconnectSta(webs_t* wp, char_t *path, char_t *query)
{
	char_t *mac = websGetVar(wp,  T("disconnectSta"), "");

	if ((mac != NULL) && (strlen(mac) > 0))
	{
		if (strcmp(mac, "*") == 0) {
			wlanDisconnectAllStations("ra0");
#ifndef CONFIG_RT_SECOND_IF_NONE
			wlanDisconnectAllStations("rai0");
#endif
		} else if (strlen(mac) == 17) {
			wlanDisconnectStation("ra0", mac);
#ifndef CONFIG_RT_SECOND_IF_NONE
			wlanDisconnectStation("rai0", mac);
#endif
		}
	}
//	websHeader(wp);
	websDone(wp, 204);
}

static int getMaxStaNum(webs_t *wp, char** params, int nparams)
{
	return outWrite(T("%d"), MAX_NUMBER_OF_MAC);
}

static int getBSSIDNum(webs_t *wp, char** params, int nparams)
{
	return outWrite(T("%d"), MAX_NUMBER_OF_BSSID);
}

static int getVHTSTBCBuilt(webs_t *wp, char** params, int nparams) {
#if defined(CONFIG_RT_SECOND_IF_NONE) || defined(CONFIG_RT_SECOND_IF_MT7610E)
	return outWrite(T("0"));
#else
	return outWrite(T("1"));
#endif
}

static int getBandSteeringBuilt(webs_t *wp, char** params, int nparams) {
#if defined(CONFIG_USER_BNDSTR) && defined(CONFIG_BAND_STEERING)
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getDFSBuilt(webs_t *wp, char** params, int nparams) {
/* IEEE80211H variable enable spectrum managment and/or DFS (depended by DFS builtin) need for all dualband builds */
#ifdef CONFIG_RT_SECOND_IF_RANGE_5GHZ
#ifndef CONFIG_RT_SECOND_IF_NONE
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
#else
	return outWrite(T("0"));
#endif
}

static int getRRMBuilt(webs_t *wp, char** params, int nparams) {
#if defined(CONFIG_MT7610_AP_DOT11K_RRM_SUPPORT) || defined(CONFIG_MT76X2_AP_DOT11K_RRM_SUPPORT) || defined(CONFIG_MT76X3_AP_DOT11K_RRM_SUPPORT) || defined(CONFIG_MT7615_AP_DOT11K_RRM_SUPPORT)
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getFTBuilt(webs_t *wp, char** params, int nparams) {
#if defined(CONFIG_MT7610_AP_DOT11R_FT_SUPPORT) || defined(CONFIG_MT76X2_AP_DOT11R_FT_SUPPORT) || defined(CONFIG_MT76X3_AP_DOT11R_FT_SUPPORT) || defined(CONFIG_MT7615_AP_DOT11R_FT_SUPPORT)
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getEDCCABuilt(webs_t *wp, char** params, int nparams) {
#if defined(CONFIG_MT7615_AP) || defined(CONFIG_MT7615_AP_MODULE)
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getClampBuilt(webs_t *wp, char** params, int nparams) {
#if defined(CONFIG_RT_FIRST_IF_MT7602E) || defined(CONFIG_RT_SECOND_IF_MT7612E) || defined(CONFIG_RT_SECOND_IF_MT7610E)
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getWirelessFirst(webs_t *wp, char** params, int nparams) {
#ifdef CONFIG_RT_FIRST_CARD
	return outWrite(TOSTRING(CONFIG_RT_FIRST_CARD));
#else
	return outWrite(T(""));
#endif
}

static int getWirelessSecond(webs_t *wp, char** params, int nparams) {
#ifdef CONFIG_RT_SECOND_CARD
	return outWrite(TOSTRING(CONFIG_RT_SECOND_CARD));
#else
	return outWrite(T(""));
#endif
}

static void getScanAp(webs_t* wp, char_t *path, char_t *query)
{
	int i;
        unsigned int j;
	int ent_count = 0;
	char str[18];
	char ssid[64];
	struct WLAN_AP_ENTRY *entries;
        char *iface = NULL;

	char *inic = websGetVar(wp, T("inic"), T("0"));
	char *rescan = websGetVar(wp, T("rescan"), T("1"));

	if (CHK_IF_DIGIT(inic, 1)) {
		iface = getWlanRadioModuleName(2);
	}

	if (!iface) {
		iface = getWlanRadioModuleName(1);
	}

	if (CHK_IF_DIGIT(rescan, 1)) {
		entries = wlanAPScan(iface, &ent_count);
	}
	else {
		entries = getWlanAPScanResult(iface, &ent_count);
	}

        websSetContentType(wp, "text/plain");

	outWrite(T("{ \"wireless\": [ "));
	for (i = 0; i < ent_count; i++)
	{
		ssid[0] = '\0';
		for (j = 0; j < strlen(entries[i].ssid); j++) {
			if (entries[i].ssid[j] == '"')
				sprintf(ssid, "%s%c", ssid, '\\');
			sprintf(ssid, "%s%c", ssid, entries[i].ssid[j]);
		}
		outWrite(T("{ "));
		outWrite(T("\"channel\":\"%d\", "), (unsigned char) entries[i].chan);
		outWrite(T("\"ssid\":\"%s\", "), ssid);
		sprintf(str, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx", entries[i].bssid[0], entries[i].bssid[1], entries[i].bssid[2], entries[i].bssid[3], entries[i].bssid[4], entries[i].bssid[5]);
		outWrite(T("\"bssid\":\"%s\", "), str);
		outWrite(T("\"security\":\"%s\", "), entries[i].security);
		outWrite(T("\"signal\":\"%d\", "), (unsigned char) entries[i].signal_percent);
		outWrite(T("\"wmode\":\"%s\", "), entries[i].wmode);
		outWrite(T("\"extch\":\"%s\", "), entries[i].extch);
		outWrite(T("\"nt\":\"%s\""), entries[i].nt);

		outWrite(T(" }%s "), (i + 1 < ent_count) ? "," : "");
	}
	free(entries);
	outWrite(T("] }"));
	websDone(wp, 200);
}

int asp_mod_wireless_init()
{
	aspDefineFunc("getVideoTurbineBuilt", getVideoTurbineBuilt, EVERYONE);
	aspDefineFunc("getIdsEnableBuilt", getIdsEnableBuilt, EVERYONE);
	aspDefineFunc("getWlan11aChannels", getWlan11aChannels, EVERYONE);
	aspDefineFunc("getWlan11gChannels", getWlan11gChannels, EVERYONE);
	aspDefineFunc("getWlanApcliBuilt", getWlanApcliBuilt, EVERYONE);
	aspDefineFunc("getWlan256QAMBuilt", getWlan256QAMBuilt, EVERYONE);
	aspDefineFunc("getWlanSCSBuilt", getWlanSCSBuilt, EVERYONE);
	aspDefineFunc("getWlanVOWBuilt", getWlanVOWBuilt, EVERYONE);
	aspDefineFunc("getWlanWNMBuilt", getWlanWNMBuilt, EVERYONE);
	aspDefineFunc("getWlanPMFBuilt", getWlanPMFBuilt, EVERYONE);
	aspDefineFunc("getWlan4T4RBuilt", getWlan4T4RBuilt, EVERYONE);
	aspDefineFunc("getWlanWdsBuilt", getWlanWdsBuilt, EVERYONE);
	aspDefineFunc("getWlanChannel", getWlanChannel, EVERYONE);
	aspDefineFunc("getWlanChannelAC", getWlanChannelAC, EVERYONE);
	aspDefineFunc("getWlanChannel_ioctl", getWlanChannel_ioctl, EVERYONE);
	aspDefineFunc("getWlanChannelAC_ioctl", getWlanChannelAC_ioctl, EVERYONE);
	aspDefineFunc("getWlanCurrentMac", getWlanCurrentMac, EVERYONE);
	aspDefineFunc("getWlanCurrentMacAC", getWlanCurrentMacAC, EVERYONE);
	aspDefineFunc("getWlanStaInfo", getWlanStaInfo, EVERYONE);
	aspDefineFunc("get802_1XBuilt", get802_1XBuilt, EVERYONE);
	aspDefineFunc("getWlanM2UBuilt", getWlanM2UBuilt, EVERYONE);

	aspDefineFunc("listCountryCodes", listCountryCodes, EVERYONE);
	aspDefineFunc("is4t4r", is4t4r, EVERYONE);
	aspDefineFunc("is5gh_support", is5gh_support, EVERYONE);
	aspDefineFunc("is5gh_1t1r", is5gh_1t1r, EVERYONE);
	aspDefineFunc("getLDPCBuilt", getLDPCBuilt, EVERYONE);
	aspDefineFunc("getVHTSTBCBuilt", getVHTSTBCBuilt, EVERYONE);
	aspDefineFunc("getTXBFBuilt", getTXBFBuilt, EVERYONE);
	aspDefineFunc("getMaxStaNum", getMaxStaNum, EVERYONE);
	aspDefineFunc("getBSSIDNum", getBSSIDNum, EVERYONE);
	aspDefineFunc("getBandSteeringBuilt", getBandSteeringBuilt, EVERYONE);
	aspDefineFunc("getDFSBuilt", getDFSBuilt, EVERYONE);
	aspDefineFunc("getRRMBuilt", getRRMBuilt, EVERYONE);
	aspDefineFunc("getFTBuilt", getFTBuilt, EVERYONE);
	aspDefineFunc("getEDCCABuilt", getEDCCABuilt, EVERYONE);
	aspDefineFunc("getClampBuilt", getClampBuilt, EVERYONE);
	websFormDefine("getScanAp", getScanAp, EVERYONE);
	aspDefineFunc("getSSIDsList", getSSIDsList, EVERYONE);
	websFormDefine("wirelessBasic", wirelessBasic, EVERYONE);
	websFormDefine("disconnectSta", disconnectSta, EVERYONE);
#if defined(CONFIG_MT7610_AP_WDS) || defined(CONFIG_MT76X2_AP_WDS) || defined(CONFIG_MT76X3_AP_WDS) || defined(CONFIG_MT7615_AP_WDS)
	websFormDefine("wirelessWds", wirelessWds, ADMIN);
#endif
#if defined(CONFIG_MT7610_AP_APCLI) || defined(CONFIG_MT76X2_AP_APCLI) || defined(CONFIG_MT76X3_AP_APCLI) || defined(CONFIG_MT7615_AP_APCLI)
	websFormDefine("wirelessApcli", wirelessApcli, EVERYONE);
	aspDefineFunc("getAPCliStatus", getAPCliStatus, EVERYONE);
#endif

	aspDefineFunc("getWirelessFirst", getWirelessFirst, EVERYONE);
	aspDefineFunc("getWirelessSecond", getWirelessSecond, EVERYONE);

	websFormDefine("wirelessGetSecurity", wirelessGetSecurity, EVERYONE);


    return 0;
}
