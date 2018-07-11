#include "asp_mod_wireless.h"

static int default_shown_mbssid[3]  = {0,0,0};

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

/* goform/wirelessBasic */

static void wirelessBasic(webs_t* wp, char_t *path, char_t *query)
{
	char_t	*wirelessmode, *mbssid_mode, *bssid_num, *mbcastisolated_ssids, *hidden_ssids, *isolated_ssids, *mbssidapisolated;
	char_t	*sz11gChannel, *abg_rate, *tx_power, *tx_stream, *rx_stream, *g_autoselect, *a_autoselect, *g_checktime, *a_checktime;
	char_t	*n_mode, *n_bandwidth, *n_bandwidthinic, *n_gi, *n_stbc, *n_mcs, *n_rdg, *n_extcha, *n_amsdu, *n_autoba, *n_badecline;
	char_t  *fastroaming, *bandsteering, *token, *LanWifiIsolate, *PMKCachePeriod;
	char_t pmktmpbuf[32] = {0};
#if defined(CONFIG_MT7610_AP_IDS) || defined(CONFIG_MT76X2_AP_IDS) || defined(CONFIG_MT76X3_AP_IDS) || defined(CONFIG_MT7615_AP_IDS)
	char_t *ids_enable;
#endif
#ifndef CONFIG_RT_SECOND_IF_NONE
	char_t	*wirelessmodeac, *tx_power_ac, *sz11aChannel, *ssid1ac, *ac_gi, *ac_stbc, *ac_ldpc, *ac_bw, *ac_bwsig;
	int     is_vht = 0, mode_ac;
#ifdef CONFIG_RT_SECOND_IF_RANGE_5GHZ
	char_t	*dot11h;
	char 	ieee80211h[2 * MAX_NUMBER_OF_BSSID] = "";
#endif
#endif
#if defined(CONFIG_MT76X2_AP_TXBF_SUPPORT) || defined(CONFIG_MT7615_AP_TXBF_SUPPORT)
	char_t	*ITxBfEn, *ETxBfeeEn, *ETxBfEnCond;
#endif
#if defined(CONFIG_MT7610_AP_DOT11K_RRM_SUPPORT) || defined(CONFIG_MT76X2_AP_DOT11K_RRM_SUPPORT) || defined(CONFIG_MT76X3_AP_DOT11K_RRM_SUPPORT) || defined(CONFIG_MT7615_AP_DOT11K_RRM_SUPPORT)
	char_t	*rrm;
	char 	ieee80211k[2 * MAX_NUMBER_OF_BSSID] = "";
#endif
#if defined(CONFIG_MT7610_AP_DOT11R_FT_SUPPORT) || defined(CONFIG_MT76X2_AP_DOT11R_FT_SUPPORT) || defined(CONFIG_MT76X3_AP_DOT11R_FT_SUPPORT) || defined(CONFIG_MT7615_AP_DOT11R_FT_SUPPORT)
	char_t	*ft;
	char 	ieee80211r[2 * MAX_NUMBER_OF_BSSID] = "";
#endif
	int     is_ht = 0, i = 1, ssid = 0, new_bssid_num, mode;
	char	hidden_ssid[2 * MAX_NUMBER_OF_BSSID] = "", noforwarding[2 * MAX_NUMBER_OF_BSSID] = "", noforwardingmbcast[2 * MAX_NUMBER_OF_BSSID] = "";
	char 	ssid_web_var[8] = "mssid_\0", ssid_nvram_var[8] = "SSID\0\0\0";

	// Get current mode & new mode
	char *web_radio_on = websGetVar(wp, T("radioWirelessEnabled"), T("0"));
#ifndef CONFIG_RT_SECOND_IF_NONE
	char *web_radio_ac_on = websGetVar(wp, T("radioWirelessEnabledAc"), T("0"));
#endif

	char_t *bg_protection, *beacon, *beaconinic, *dtim, *fragment, *rts, *preamble_type, *maxstanum, *tmpblockafterkick, *kickstarssilowft, *keepalive, *idletimeout, *regulatoryclassinic;
	char_t *short_slot, *tx_burst, *pkt_aggregate, *countrycode, *country_region, *rd_region, *wmm_capable, *dyn_vga;
	int ssid_num, tmp;
	char_t *ackpolicy_ssid, *life_check, *ed_mode, *submitUrl, *tokenadv;
	char ackpolicy[2 * MAX_NUMBER_OF_BSSID] = "", stanum_array[2 * MAX_NUMBER_OF_MAC] = "", keepalive_array[2 * MAX_NUMBER_OF_MAC] = "";	
#if defined(CONFIG_RT_FIRST_IF_MT7602E) || defined(CONFIG_RT_SECOND_IF_MT7612E)
	char_t *dyn_vga_long, *dyn_vga_clamp;
#endif
#if defined(CONFIG_RT_SECOND_IF_MT7610E)
	char_t *dyn_vga_long;
#endif
#if defined(CONFIG_MT7610_AP_MCAST_RATE_SPECIFIC) || defined(CONFIG_MT76X2_AP_MCAST_RATE_SPECIFIC) || defined(CONFIG_MT76X3_AP_MCAST_RATE_SPECIFIC)
	char_t *mcast_mode, *mcast_mcs;
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
	tx_power = websGetVar(wp, T("tx_power"), T("100"));
	mbssid_mode = websGetVar(wp, T("mbssid_mode"), T("ra"));
	bssid_num = websGetVar(wp, T("bssid_num"), T("1"));
	hidden_ssids = websGetVar(wp, T("hidden_ssids"), T("")); 
	isolated_ssids = websGetVar(wp, T("isolated_ssids"), T(""));
	LanWifiIsolate = websGetVar(wp, T("LanWifiIsolate"), T("0"));

	mbcastisolated_ssids = websGetVar(wp, T("mbcastisolated_ssids"), T(""));
	mbssidapisolated = websGetVar(wp, T("mbssidapisolated"), T("0"));

	sz11gChannel = websGetVar(wp, T("sz11gChannel"), T("")); 
	abg_rate = websGetVar(wp, T("abg_rate"), T("")); 
	int rate = atoi(abg_rate);

	tx_stream = websGetVar(wp, T("tx_stream"), T("1"));
	rx_stream = websGetVar(wp, T("rx_stream"), T("1"));

	n_mode = websGetVar(wp, T("n_mode"), T("1"));
	n_bandwidth = websGetVar(wp, T("n_bandwidth"), T("1"));
	n_bandwidthinic = websGetVar(wp, T("n_bandwidthinic"), T("1"));
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
	bandsteering = websGetVar(wp, T("BandSteering"), T("0"));
	bandsteering = (bandsteering == NULL) ? "0" : bandsteering;
	fastroaming = websGetVar(wp, T("FastRoaming"), T("0"));
	fastroaming = (fastroaming == NULL) ? "0" : fastroaming;
        PMKCachePeriod = websGetVar(wp, T("PMKCachePeriod"), T("480"));
#if defined(CONFIG_MT7610_AP_IDS) || defined(CONFIG_MT76X2_AP_IDS) || defined(CONFIG_MT76X3_AP_IDS) || defined(CONFIG_MT7615_AP_IDS)
	ids_enable = websGetVar(wp, T("IdsEnable"), T("0"));
	ids_enable = (ids_enable == NULL) ? "0" : ids_enable;
#endif

#ifndef CONFIG_RT_SECOND_IF_NONE
	wirelessmodeac = websGetVar(wp, T("wirelessmodeac"), T("14")); //14: a,an/ac mode
	mode_ac = atoi(wirelessmodeac);
	tx_power_ac = websGetVar(wp, T("tx_powerac"), T("100"));
	sz11aChannel = websGetVar(wp, T("sz11aChannel"), T("")); 
	ssid1ac = websGetVar(wp, T("mssidac_1"), T("0"));
	ac_gi = websGetVar(wp, T("ac_gi"), T("1"));
	ac_stbc = websGetVar(wp, T("ac_stbc"), T("1"));
	ac_ldpc = websGetVar(wp, T("ac_ldpc"), T("1"));
	ac_bw = websGetVar(wp, T("ac_bw"), T("1"));
	ac_bwsig = websGetVar(wp, T("ac_bwsig"), T("1"));
#ifdef CONFIG_RT_SECOND_IF_RANGE_5GHZ
	dot11h = websGetVar(wp, T("dot11h"), T(""));
#endif
#endif
	new_bssid_num = atoi(bssid_num);

#if defined(CONFIG_MT76X2_AP_TXBF_SUPPORT) || defined(CONFIG_MT7615_AP_TXBF_SUPPORT)
	ITxBfEn = websGetVar(wp, T("ITxBfEn"), T("1"));
	ETxBfeeEn = websGetVar(wp, T("ETxBfeeEn"), T("1"));
	ETxBfEnCond = websGetVar(wp, T("ETxBfEnCond"), T("1"));
#endif
#if defined(CONFIG_MT7610_AP_DOT11K_RRM_SUPPORT) || defined(CONFIG_MT76X2_AP_DOT11K_RRM_SUPPORT) || defined(CONFIG_MT76X3_AP_DOT11K_RRM_SUPPORT) || defined(CONFIG_MT7615_AP_DOT11K_RRM_SUPPORT)
	rrm = websGetVar(wp, T("RRMEnable"), T("1"));
#endif
#if defined(CONFIG_MT7610_AP_DOT11R_FT_SUPPORT) || defined(CONFIG_MT76X2_AP_DOT11R_FT_SUPPORT) || defined(CONFIG_MT76X3_AP_DOT11R_FT_SUPPORT) || defined(CONFIG_MT7615_AP_DOT11R_FT_SUPPORT)
	ft = websGetVar(wp, T("FtSupport"), T("1"));
#endif

	bg_protection = websGetVar(wp, T("bg_protection"), T("0"));
	beacon = websGetVar(wp, T("beacon"), T("100"));
	beaconinic = websGetVar(wp, T("beaconINIC"), T("100"));
	dtim = websGetVar(wp, T("dtim"), T("1"));
	fragment = websGetVar(wp, T("fragment"), T("2346"));
	rts = websGetVar(wp, T("rts"), T("2347"));
	preamble_type = websGetVar(wp, T("preamble_type"), T("0"));
	short_slot = websGetVar(wp, T("short_slot"), T("0"));
	tx_burst = websGetVar(wp, T("tx_burst"), T("0"));
	pkt_aggregate = websGetVar(wp, T("pkt_aggregate"), T("0"));
	rd_region = websGetVar(wp, T("rd_region"), T("FCC"));
	countrycode = websGetVar(wp, T("country_code"), T("NONE"));
	country_region = websGetVar(wp, T("country_region"), T("0"));
	wmm_capable = websGetVar(wp, T("WmmCapable"), T("0"));
	dyn_vga = websGetVar(wp, T("dyn_vga"), T("1"));
	regulatoryclassinic = websGetVar(wp, T("RegulatoryClassINIC"), T("1;2;3;4;0"));
#if defined(CONFIG_RT_FIRST_IF_MT7602E) || defined(CONFIG_RT_SECOND_IF_MT7612E)
	dyn_vga_long = websGetVar(wp, T("advDynVGALong"), T("0"));
	dyn_vga_clamp = websGetVar(wp, T("advDynVGAClamp"), T("0"));
#endif
#if defined(CONFIG_RT_SECOND_IF_MT7610E)
	dyn_vga_long = websGetVar(wp, T("advDynVGALong"), T("0"));
#endif
#if defined(CONFIG_MT7610_AP_MCAST_RATE_SPECIFIC) || defined(CONFIG_MT76X2_AP_MCAST_RATE_SPECIFIC) || defined(CONFIG_MT76X3_AP_MCAST_RATE_SPECIFIC)
	mcast_mode = websGetVar(wp, T("McastPhyMode"), T("2"));
	mcast_mcs = websGetVar(wp, T("McastMcs"), T("0"));
#endif
#if defined(CONFIG_MT7610_AP_IGMP_SNOOP) || defined(CONFIG_MT76X2_AP_IGMP_SNOOP) || defined(CONFIG_MT76X3_AP_IGMP_SNOOP) || defined(CONFIG_MT7615_AP_IGMP_SNOOP)
	m2u_enable = websGetVar(wp, T("m2u_enable"), T("0"));
#if defined(CONFIG_MT7610_AP_VIDEO_TURBINE) || defined(CONFIG_MT76X2_AP_VIDEO_TURBINE) || defined(CONFIG_MT76X3_AP_VIDEO_TURBINE) || defined(CONFIG_MT7615_AP_VIDEO_TURBINE)
	video_turbine = websGetVar(wp, T("video_turbine"), T("0"));
#endif
#endif
	maxstanum = websGetVar(wp, T("maxstanum"), T("0"));
	tmpblockafterkick = websGetVar(wp, T("TmpBlockAfterKick"), T("14"));
	kickstarssilowft = websGetVar(wp, T("KickStaRssiLowFT"), T("0"));
	keepalive = websGetVar(wp, T("keepalive"), T("0"));
	idletimeout = websGetVar(wp, T("idletimeout"), T("0"));
	life_check = websGetVar(wp, T("EntryLifeCheck"), T("0"));
	ackpolicy_ssid = websGetVar(wp, T("AckPolicy"), T("0"));
	ed_mode = websGetVar(wp, T("ED_MODE"), T("0"));

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
	ngx_nvram_bufset(wp,"WirelessMode", wirelessmode);
	ngx_nvram_bufset(wp,"TxPower", tx_power);
#ifndef CONFIG_RT_SECOND_IF_NONE
	ngx_nvram_bufset(wp,"WirelessModeINIC", wirelessmodeac);
	ngx_nvram_bufset(wp,"TxPowerINIC", tx_power_ac);
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
			sprintf(hidden_ssid, "%s%s", hidden_ssid, (strchr(hidden_ssids, ssid + '0') != NULL) ? "1" : "0");
			sprintf(hidden_ssid, "%s%s", hidden_ssid, token);
			sprintf(noforwarding, "%s%s", noforwarding, (strchr(isolated_ssids, ssid + '0') != NULL) ? "1" : "0");
			sprintf(noforwarding, "%s%s", noforwarding, token);
			sprintf(noforwardingmbcast, "%s%s", noforwardingmbcast, (strchr(mbcastisolated_ssids, ssid + '0') != NULL) ? "1" : "0");
			sprintf(noforwardingmbcast, "%s%s", noforwardingmbcast, token);
#ifndef CONFIG_RT_SECOND_IF_NONE
#ifdef CONFIG_RT_SECOND_IF_RANGE_5GHZ
			sprintf(ieee80211h, "%s%s", ieee80211h, (CHK_IF_DIGIT(dot11h, 1)) ? "1" : "0");
			sprintf(ieee80211h, "%s%s", ieee80211h, token);
#endif
#endif
#if defined(CONFIG_MT7610_AP_DOT11K_RRM_SUPPORT) || defined(CONFIG_MT76X2_AP_DOT11K_RRM_SUPPORT) || defined(CONFIG_MT76X3_AP_DOT11K_RRM_SUPPORT) || defined(CONFIG_MT7615_AP_DOT11K_RRM_SUPPORT)
			sprintf(ieee80211k, "%s%s", ieee80211k, (CHK_IF_DIGIT(rrm, 1)) ? "1" : "0");
			sprintf(ieee80211k, "%s%s", ieee80211k, token);
#endif
#if defined(CONFIG_MT7610_AP_DOT11R_FT_SUPPORT) || defined(CONFIG_MT76X2_AP_DOT11R_FT_SUPPORT) || defined(CONFIG_MT76X3_AP_DOT11R_FT_SUPPORT) || defined(CONFIG_MT7615_AP_DOT11R_FT_SUPPORT)
			sprintf(ieee80211r, "%s%s", ieee80211r, (CHK_IF_DIGIT(ft, 1)) ? "1" : "0");
			sprintf(ieee80211r, "%s%s", ieee80211r, token);
#endif
			i++;
		}
	}
#ifndef CONFIG_RT_SECOND_IF_NONE
	// Fist SSID for iNIC
	ngx_nvram_bufset(wp,"SSID1INIC", ssid1ac);
#ifdef CONFIG_RT_SECOND_IF_RANGE_5GHZ
	ngx_nvram_bufset(wp,"IEEE80211H", ieee80211h);
#endif
#endif
	// SSID settings
	ngx_nvram_bufset(wp,"BssidNum", bssid_num);
	ngx_nvram_bufset(wp,"HideSSID", hidden_ssid);
	ngx_nvram_bufset(wp,"LanWifiIsolate", LanWifiIsolate);
	ngx_nvram_bufset(wp,"NoForwarding", noforwarding);
	ngx_nvram_bufset(wp,"NoForwardingBTNBSSID", mbssidapisolated);
	ngx_nvram_bufset(wp,"NoForwardingMBCast", noforwardingmbcast);
#if defined(CONFIG_MT7610_AP_DOT11K_RRM_SUPPORT) || defined(CONFIG_MT76X2_AP_DOT11K_RRM_SUPPORT) || defined(CONFIG_MT76X3_AP_DOT11K_RRM_SUPPORT) || defined(CONFIG_MT7615_AP_DOT11K_RRM_SUPPORT)
	ngx_nvram_bufset(wp,"RRMEnable", ieee80211k);
#endif
#if defined(CONFIG_MT7610_AP_DOT11R_FT_SUPPORT) || defined(CONFIG_MT76X2_AP_DOT11R_FT_SUPPORT) || defined(CONFIG_MT76X3_AP_DOT11R_FT_SUPPORT) || defined(CONFIG_MT7615_AP_DOT11R_FT_SUPPORT)
	ngx_nvram_bufset(wp,"FtSupport", ieee80211r);
#endif

	// Channel & automatic channel select
#ifndef CONFIG_RT_SECOND_IF_NONE
	if (CHK_IF_SET(sz11aChannel)) {
		ngx_nvram_bufset(wp,"ChannelINIC", sz11aChannel);
		if ((!strncmp(sz11aChannel, "0", 2)) && (CHK_IF_SET(a_autoselect))) {
		    ngx_nvram_bufset(wp,"AutoChannelSelectINIC", a_autoselect);
		    ngx_nvram_bufset(wp,"ACSCheckTimeINIC", (CHK_IF_SET(a_checktime)) ? a_checktime : "24");
		} else {
		    ngx_nvram_bufset(wp,"AutoChannelSelectINIC", "0");
		    ngx_nvram_bufset(wp,"ACSCheckTimeINIC", "0");
		}
	}
#endif
	if (CHK_IF_SET(sz11gChannel)) {
		ngx_nvram_bufset(wp,"Channel", sz11gChannel);
		if ((!strncmp(sz11gChannel, "0", 2)) && (CHK_IF_SET(g_autoselect))) {
		    ngx_nvram_bufset(wp,"AutoChannelSelect", g_autoselect);
		    ngx_nvram_bufset(wp,"ACSCheckTime", (CHK_IF_SET(g_checktime)) ? g_checktime : "24");
		} else {
		    ngx_nvram_bufset(wp,"AutoChannelSelect", "0");
		    ngx_nvram_bufset(wp,"ACSCheckTime", "0");
		}
	}

	// HT_OpMode, HT_BW, HT_GI, VHT_SGI, VHT_LDPC, HT_MCS, HT_HTC, HT_RDG, HT_EXTCHA, HT_AMSDU, HT_TxStream, HT_RxStream
	if (is_ht) {
		ngx_nvram_bufset(wp,"HT_MCS", n_mcs);
		ngx_nvram_bufset(wp,"HT_OpMode", n_mode);
		ngx_nvram_bufset(wp,"HT_BW", n_bandwidth);
		ngx_nvram_bufset(wp,"HT_BWINIC", n_bandwidthinic);
		ngx_nvram_bufset(wp,"HT_GI", n_gi);
		ngx_nvram_bufset(wp,"HT_STBC", n_stbc);
		ngx_nvram_bufset(wp,"HT_EXTCHA", n_extcha);
		ngx_nvram_bufset(wp,"HT_AMSDU", n_amsdu);
		ngx_nvram_bufset(wp,"HT_AutoBA", n_autoba);
		ngx_nvram_bufset(wp,"HT_BADecline", n_badecline);
		ngx_nvram_bufset(wp,"HT_TxStream", tx_stream);
		ngx_nvram_bufset(wp,"HT_RxStream", rx_stream);
		// HT_RGD depend at HT_HTC+ frame support
		ngx_nvram_bufset(wp,"HT_HTC", n_rdg);
		ngx_nvram_bufset(wp,"HT_RDG", n_rdg);

	} else {
		// Rate for a, b, g, n, ac
		// In the future need allow set this per MBSSID
		if (strncmp(abg_rate, "", 1)) {
		    switch (rate) {
			case 1:
				ngx_nvram_bufset(wp,"HT_MCS", "0");
				break;
			case 2:
				ngx_nvram_bufset(wp,"HT_MCS", "1");
				break;
			case 5:
				ngx_nvram_bufset(wp,"HT_MCS", "2");
				break;
			case 6:
				ngx_nvram_bufset(wp,"HT_MCS", "0");
				break;
			case 9:
				ngx_nvram_bufset(wp,"HT_MCS", "1");
				break;
			case 11:
				ngx_nvram_bufset(wp,"HT_MCS", "3");
				break;
			case 12:
				ngx_nvram_bufset(wp,"HT_MCS", "2");
				break;
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
	}

#if defined(CONFIG_MT76X2_AP_TXBF_SUPPORT) || defined(CONFIG_MT7615_AP_TXBF_SUPPORT)
	ngx_nvram_bufset(wp,"ITxBfEn", ITxBfEn);
	ngx_nvram_bufset(wp,"ETxBfeeEn", ETxBfeeEn);
	ngx_nvram_bufset(wp,"ETxBfEnCond", ETxBfEnCond);
#endif

#ifndef CONFIG_RT_SECOND_IF_NONE
	// VHT_Modes
	if (is_vht)
	{
// STBC for 1T1R module (as 7610) always disable (support only in STA mode for 1T1R)
#if (CONFIG_RT_SECOND_CARD == 7610)
		ngx_nvram_bufset(wp,"VHT_STBC", "0");
#else
		ngx_nvram_bufset(wp,"VHT_STBC", ac_stbc);
#endif
// LDPC support ONLY for 7602/7612/7615
#if (CONFIG_RT_FIRST_CARD == 7602) || (CONFIG_RT_SECOND_CARD == 7612) || (CONFIG_RT_FIRST_CARD == 7615) || (CONFIG_RT_SECOND_CARD == 7615)
		ngx_nvram_bufset(wp,"VHT_LDPC", ac_ldpc);
		ngx_nvram_bufset(wp,"HT_LDPC", ac_ldpc);
#else
		ngx_nvram_bufset(wp,"VHT_LDPC", "0");
		ngx_nvram_bufset(wp,"HT_LDPC", "0");
#endif
		ngx_nvram_bufset(wp,"VHT_SGI", ac_gi);
		ngx_nvram_bufset(wp,"VHT_BW", ac_bw);
		ngx_nvram_bufset(wp,"VHT_BW_SIGNAL", ac_bwsig);
	}
#endif
	ngx_nvram_bufset(wp,"RadioOn", web_radio_on);
#ifndef CONFIG_RT_SECOND_IF_NONE
	ngx_nvram_bufset(wp,"RadioOnINIC", web_radio_ac_on);
#endif

	// Band Steering
	ngx_nvram_bufset(wp,"BandSteering", bandsteering);
	if (CHK_IF_DIGIT(bandsteering, 1))
		setupParameters(wp, band_steering_flags, 0);

	// Fast roaming
	ngx_nvram_bufset(wp,"FastRoaming", fastroaming);
	if (CHK_IF_DIGIT(fastroaming, 1))
		setupParameters(wp, fast_roaming_flags, 0);

        int cachePeriod = strToIntDef(PMKCachePeriod, 480);
        if (cachePeriod > 0)
        {
            snprintf(pmktmpbuf, 31, "%i;%i;%i;%i", cachePeriod, cachePeriod, cachePeriod, cachePeriod);
            ngx_nvram_bufset(wp,"PMKCachePeriod", pmktmpbuf);
        }

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
	ngx_nvram_bufset(wp,"BGProtection", bg_protection);
	ngx_nvram_bufset(wp,"BeaconPeriod", beacon);
	ngx_nvram_bufset(wp,"BeaconPeriodINIC", beaconinic);
	ngx_nvram_bufset(wp,"DtimPeriod", dtim);
	ngx_nvram_bufset(wp,"FragThreshold", fragment);
	ngx_nvram_bufset(wp,"RTSThreshold", rts);
	ngx_nvram_bufset(wp,"TxPreamble", preamble_type);
	ngx_nvram_bufset(wp,"ShortSlot", short_slot);
	ngx_nvram_bufset(wp,"PktAggregate", pkt_aggregate);
	ngx_nvram_bufset(wp,"RDRegion", rd_region);
	//txburst and burst mode set in one place
	ngx_nvram_bufset(wp,"TxBurst", tx_burst);

	//switch to normal txburst mode (speed regression with some adapters)
	ngx_nvram_bufset(wp,"BurstMode", "0");

	if (NULL != maxstanum) {
		tmp = atoi(maxstanum);
		if ((tmp < 0) || (tmp > MAX_NUMBER_OF_MAC))
			tmp = MAX_NUMBER_OF_MAC;
		sprintf(stanum_array, "%d", tmp);
		for (i=2; i <= ssid_num; i++)
			sprintf(stanum_array, "%s;%d", stanum_array, tmp);
		ngx_nvram_bufset(wp,"MaxStaNum", stanum_array);
	}

	ngx_nvram_bufset(wp,"TmpBlockAfterKick", tmpblockafterkick);
	ngx_nvram_bufset(wp,"KickStaRssiLowFT", kickstarssilowft);

	if (NULL != keepalive) {
		tmp = atoi(keepalive);
		if ((tmp < 10) || (tmp > 300))
			tmp = 60;
		sprintf(keepalive_array, "%d", tmp);
		for (i=2; i <= ssid_num; i++)
			sprintf(keepalive_array, "%s;%d", keepalive_array, tmp);
		ngx_nvram_bufset(wp,"StationKeepAlive", keepalive_array);
	}

	if (NULL != idletimeout) {
		tmp = atoi(idletimeout);
		if ((tmp < 60) || (tmp > 300))
			tmp = 60;
		sprintf(idletimeout, "%d", tmp);
		ngx_nvram_bufset(wp,"IdleTimeout", idletimeout);
	}

	if (NULL != life_check) {
		tmp = atoi(life_check);
		if (tmp < 256)
			tmp = 256;

		if (tmp > 4096)
			tmp = 4096;

		sprintf(life_check, "%d", tmp);
		ngx_nvram_bufset(wp,"EntryLifeCheck", life_check);
	}

	i = 1;
	for (ssid=0; ssid < ssid_num; ssid++) {
		tokenadv = (i != ssid_num) ? ";" : "";
		sprintf(ackpolicy, "%s%s", ackpolicy, ackpolicy_ssid);
		sprintf(ackpolicy, "%s%s", ackpolicy, tokenadv);
		i++;
	}

	ngx_nvram_bufset(wp,"AckPolicy", ackpolicy);
	ngx_nvram_bufset(wp,"WmmCapable", wmm_capable);
	ngx_nvram_bufset(wp,"DyncVgaEnable", dyn_vga);
#if defined(CONFIG_RT_FIRST_IF_MT7602E) || defined(CONFIG_RT_SECOND_IF_MT7612E)
	ngx_nvram_bufset(wp,"SkipLongRangeVga", dyn_vga_long);
	ngx_nvram_bufset(wp,"VgaClamp", dyn_vga_clamp);
#endif
#if defined(CONFIG_RT_SECOND_IF_MT7610E)
	ngx_nvram_bufset(wp,"SkipLongRangeVga", dyn_vga_long);
#endif
#if defined(CONFIG_MT7610_AP_MCAST_RATE_SPECIFIC) || defined(CONFIG_MT76X2_AP_MCAST_RATE_SPECIFIC) || defined(CONFIG_MT76X3_AP_MCAST_RATE_SPECIFIC)
	ngx_nvram_bufset(wp,"McastPhyMode", mcast_mode);
	ngx_nvram_bufset(wp,"McastMcs", mcast_mcs);
#endif
#if defined(CONFIG_MT7610_AP_IGMP_SNOOP) || defined(CONFIG_MT76X2_AP_IGMP_SNOOP) || defined(CONFIG_MT76X3_AP_IGMP_SNOOP) || defined(CONFIG_MT7615_AP_IGMP_SNOOP)
	ngx_nvram_bufset(wp,"M2UEnabled", m2u_enable);
#if defined(CONFIG_MT7610_AP_VIDEO_TURBINE) || defined(CONFIG_MT76X2_AP_VIDEO_TURBINE) || defined(CONFIG_MT76X3_AP_VIDEO_TURBINE) || defined(CONFIG_MT7615_AP_VIDEO_TURBINE)
	ngx_nvram_bufset(wp,"VideoTurbine", video_turbine);
#endif
#endif
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
		if (CHK_IF_DIGIT(sz11aChannel, 0))
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
	} else { // default uncknown 
		ngx_nvram_bufset(wp,"CountryRegionABand", "7");
		ngx_nvram_bufset(wp,"RegulatoryClassINIC", "0");
		ngx_nvram_bufset(wp,"RegulatoryClass", "0");
	}

	// Set-up country region
	ngx_nvram_bufset(wp,"CountryRegion", country_region);
	ngx_nvram_bufset(wp,"ED_MODE", ed_mode);

	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

#ifdef PRINT_DEBUG
	// debug print
	websHeader(wp);
	outWrite(T("<h2>mode: %s</h2><br>\n"), wirelessmode);
	outWrite(T("ssid: %s, bssid_num: %s<br>\n"), ssid, bssid_num);
	outWrite(T("hidden_ssids: %s<br>\n"), hssid);
	outWrite(T("isolated_ssids: %s<br>\n"), isolated_ssids);
	outWrite(T("mbssidapisolated: %s<br>\n"), mbssidapisolated);
	outWrite(T("sz11aChannel: %s<br>\n"), sz11aChannel);
	outWrite(T("sz11gChannel: %s<br>\n"), sz11gChannel);
	outWrite(T("tx_power: %s<br>\n"), tx_power);
	if (strncmp(abg_rate, "", 1)) {
		outWrite(T("abg_rate: %s<br>\n"), abg_rate);
	}
	if (is_ht) {
		outWrite(T("n_mode: %s<br>\n"), n_mode);
		outWrite(T("n_bandwidth: %s<br>\n"), n_bandwidth);
		outWrite(T("n_bandwidthinic: %s<br>\n"), n_bandwidthinic);
		outWrite(T("n_gi: %s<br>\n"), n_gi);
		outWrite(T("n_stbc: %s<br>\n"), n_stbc);
		outWrite(T("n_mcs: %s<br>\n"), n_mcs);
		outWrite(T("n_rdg: %s<br>\n"), n_rdg);
		outWrite(T("n_extcha: %s<br>\n"), n_extcha);
		outWrite(T("n_amsdu: %s<br>\n"), n_amsdu);
		outWrite(T("n_autoba: %s<br>\n"), n_autoba);
		outWrite(T("n_badecline: %s<br>\n"), n_badecline);

	}
#ifndef CONFIG_RT_SECOND_IF_NONE
	outWrite(T("mode ac: %s<br>\n"), wirelessmodeac);
	outWrite(T("mssidac_1: %s<br>\n"), ssid1ac);
	outWrite(T("tx_power_ac: %s<br>\n"), tx_power_ac);
	if (is_vht)
	{
		outWrite(T("ac_gi: %s<br>\n"), ac_gi);
		outWrite(T("ac_stbc: %s<br>\n"), ac_gi);
		outWrite(T("ac_ldpc: %s<br>\n"), ac_ldpc);
		outWrite(T("ac_bw: %s<br>\n"), ac_bw);
		outWrite(T("ac_bwsig: %s<br>\n"), ac_bwsig);
	}
#endif
	outWrite(T("tx_stream: %s<br>\n"), tx_stream);
	outWrite(T("rx_stream: %s<br>\n"), rx_stream);
	outWrite(T("bg_protection: %s<br>\n"), bg_protection);
	outWrite(T("beacon: %s<br>\n"), beacon);
	outWrite(T("dtim: %s<br>\n"), dtim);
	outWrite(T("fragment: %s<br>\n"), fragment);
	outWrite(T("rts: %s<br>\n"), rts);
	outWrite(T("preamble_type: %s<br>\n"), preamble_type);
	outWrite(T("short_slot: %s<br>\n"), short_slot);
	outWrite(T("tx_burst: %s<br>\n"), tx_burst);
	outWrite(T("pkt_aggregate: %s<br>\n"), pkt_aggregate);
	outWrite(T("rd_region: %s<br>\n"), rd_region);
	outWrite(T("countrycode: %s<br>\n"), countrycode);
#if defined(CONFIG_MT7610_AP_MCAST_RATE_SPECIFIC) || defined(CONFIG_MT76X2_AP_MCAST_RATE_SPECIFIC) || defined(CONFIG_MT76X3_AP_MCAST_RATE_SPECIFIC)
	outWrite(T("McastPhyMode: %s<br>\n"), mcast_mode);
	outWrite(T("mcast_mcs: %s<br>\n"), mcast_mcs);
#endif
#if defined(CONFIG_MT7610_AP_IGMP_SNOOP) || defined(CONFIG_MT76X2_AP_IGMP_SNOOP) || defined(CONFIG_MT76X3_AP_IGMP_SNOOP) || defined(CONFIG_MT7615_AP_IGMP_SNOOP)
	outWrite(T("m2u_enable: %s<br>\n"), m2u_enable);
#endif
	outWrite(T("ED_MODE: %s<br>\n"), ed_mode);

//	websFooter(wp);
	websDone(wp, 200);
#else
	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
#endif
	// reconfigure system 
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
	char_t *reset    = websGetVar(wp,  T("reset"), T(""));

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
		*Key3Type, *Key4Type, *RekeyMethod, *RekeyInterval,
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
	if(!strlen(RadiusServerSessionTimeout))
		RadiusServerSessionTimeout = "0";

	if(strlen(RadiusServerIP)) {
	    STFs(nvram, mbssid, "RADIUS_Server", RadiusServerIP);
	    STFs(nvram, mbssid, "RADIUS_Port", RadiusServerPort);
	    STFs(nvram, mbssid, "RADIUS_Key", RadiusServerSecret);
	    nvram_init(RT2860_NVRAM);
	    nvram_bufset(nvram, "session_timeout_interval", RadiusServerSessionTimeout);
	    nvram_commit(RT2860_NVRAM);
	    nvram_close(RT2860_NVRAM);
	}

}
#endif

static void confWEP(webs_t* wp, int nvram, int mbssid)
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

static void confWPAGeneral(webs_t* wp, int nvram, int mbssid)
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

static void setSecurity(webs_t* wp, int nvram)
{
	int mbssid, i;
	char_t *SSID;
	char_t *security_mode;
	char_t *AccessPolicy, *AccessControlList;
	char_t *reset = websGetVar(wp,  T("reset"), T(""));

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
		nvram_fromdef(RT2860_NVRAM, 13, "PreAuth", "AuthMode", "EncrypType", "DefaultKeyID", "RekeyMethod",
						"RekeyInterval", "session_timeout_interval", "Key1Type", "Key2Type", "Key3Type",
						"Key4Type", "WPAPSK1", "WPAPSK1INIC");
		default_shown_mbssid[nvram] = 0;
#ifdef CONFIG_USER_802_1X
		for (i = 0; i < MAX_NUMBER_OF_BSSID; i++) {
			/* clear Radius settings */
			clearRadiusSetting(nvram, i);
		}
#endif
	}
	else {
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
			char *pass_phrase_str;
			char *pass_phrase_inic_str;

			confWPAGeneral(wp, RT2860_NVRAM, mbssid);
			LFW(pass_phrase_str, passphrase);
			LFW(pass_phrase_inic_str, passphraseinic);
			STFs(nvram, mbssid, "AuthMode", security_mode);
			STFs(nvram, mbssid, "IEEE8021X", "0");
			nvram_bufset(nvram, racat("WPAPSK", mbssid+1), pass_phrase_str);
			nvram_bufset(nvram, "WPAPSK1INIC", pass_phrase_inic_str);
		}
		else if(!strcmp(security_mode, "WPA2")) {				// !------------------        WPA2 Enterprise Mode ----------------
			char *pass_phrase_str;
			char *pass_phrase_inic_str;
			char *PreAuth;
#ifdef CONFIG_USER_802_1X
			conf8021x(wp, RT2860_NVRAM, mbssid);
#endif
			confWPAGeneral(wp, RT2860_NVRAM, mbssid);

			LFW(pass_phrase_str, passphrase);
			LFW(pass_phrase_inic_str, passphraseinic);
			LFW(PreAuth, PreAuthentication);

			STFs(nvram, mbssid, "AuthMode", security_mode);
			STFs(nvram, mbssid, "IEEE8021X", "0");
			nvram_bufset(nvram, racat("WPAPSK", mbssid + 1), pass_phrase_str);
			nvram_bufset(nvram, "WPAPSK1INIC", pass_phrase_inic_str);
			STF(nvram, mbssid, PreAuth);
		}
		else if(!strcmp(security_mode, "WPA2PSK") ||				// !------------------       WPA2 Personal Mode ----------------
			!strcmp(security_mode, "WPAPSKWPA2PSK") ){ 			// !------------------       WPA PSK WPA2 PSK mixed
			char *pass_phrase_str;
			char *pass_phrase_inic_str;

			confWPAGeneral(wp, RT2860_NVRAM, mbssid);
			LFW(pass_phrase_str, passphrase);
			LFW(pass_phrase_inic_str, passphraseinic);

			STFs(nvram, mbssid, "AuthMode", security_mode);
			STFs(nvram, mbssid, "IEEE8021X", "0");
			nvram_bufset(nvram, racat("WPAPSK", mbssid+1), pass_phrase_str);
			nvram_bufset(nvram, "WPAPSK1INIC", pass_phrase_inic_str);
		}
		else
			goto out;
		// Access Policy
		for (i = 0; i < MAX_NUMBER_OF_BSSID; i++) {
			AccessPolicy = websGetVar(wp,  racat("AccessPolicy", i), T("0"));
			AccessControlList = websGetVar(wp,  racat("AccessControlList", i), T("0"));
			nvram_bufset(nvram, racat("AccessPolicy", i), AccessPolicy);
			nvram_bufset(nvram, racat("AccessControlList", i), AccessControlList);
		}
out:
		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}

	/* reconfigure system */
        wp->on_response_ok = DO_RECONFIGURE;

	websDone(wp, 200);
}

static void wirelessSetSecurity(webs_t* wp, char_t *path, char_t *query)
{
	setSecurity(wp, RT2860_NVRAM);
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
#if defined(CONFIG_MT7610_ED_MONITOR) || defined(CONFIG_MT76X2_AP_ED_MONITOR) || defined(CONFIG_MT76X3_AP_ED_MONITOR) || defined(CONFIG_MT7615_AP_ED_MONITOR)
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

static void getScanAp(webs_t* wp, char_t *path, char_t *query)
{
	int i;
        unsigned int j;
	int ent_count = 0;
	char str[18];
	char ssid[64];
	int inic;
	struct WLAN_AP_ENTRY *entries;

	sscanf(query, "%d", &inic);
	if (inic == 0) {
		entries = wlanAPScan("ra0", &ent_count);
	}
	else {
		entries = wlanAPScan("rai0", &ent_count);
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
	websFormDefine("wirelessGetSecurity", wirelessGetSecurity, EVERYONE);
	websFormDefine("wirelessSetSecurity", wirelessSetSecurity, EVERYONE);


    return 0;
}
