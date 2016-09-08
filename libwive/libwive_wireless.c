/*
 EULA:

 WIVE FIRMWARE IS FREE FOR THE NON-COMMERCIAL USE ONLY.

 Conditions of commercial use non GPL (or other not viral license) components are discussed individually.
 For a more detailed discussion please mail me.
 News and changes in Russian: http://wive-ng.sf.net
 Bug report please send to support[at]nag.ru or sfstudio[at]mail.ru
*/

#include "libwive.h"

int wlan_ioctl(unsigned long QueryCode, const char *DeviceName, struct iwreq *wrq)
{
	int ret;
	int s = socket(AF_INET, SOCK_DGRAM, 0);

	if (s < 0) {
		syslog(LOG_ERR, "wlan: ioctl open sock failed, %s", __FUNCTION__);
		return -1;
	}

	strlcpy(wrq->ifr_name, DeviceName, sizeof(wrq->ifr_name));

	ret = ioctl(s, QueryCode, wrq);

	if (ret<0)
	    syslog(LOG_ERR, "wlan: call ioctl sock failed, %s (%i)", __FUNCTION__, errno);

	close(s);

	return ret;
}

int RtpQueryInformation(unsigned long QueryCode, const char *DeviceName, void *ptr, unsigned long PtrLength)
{
	struct iwreq wrq;

	memset(ptr, 0, PtrLength);
	memset(&wrq, 0, sizeof(wrq));

	wrq.u.data.length = PtrLength;
	wrq.u.data.pointer = (caddr_t)ptr;

	if (wlan_ioctl(QueryCode, DeviceName, &wrq))
	{
	    syslog(LOG_ERR, "wlan: ioctl failed, %s, (%lu, %s)", __FUNCTION__, QueryCode, DeviceName);
	    return -1;
	}

	return wrq.u.data.length;
}

int RtpSetInformation(const char *set_info, const char *DeviceName)
{
	struct iwreq wrq;
        char *data = calloc(strlen(set_info)+1,sizeof(char));
	if (!data)
	{
	    syslog(LOG_ERR, "wlan: unable to allocate data, %s, (%s, %s)", __FUNCTION__, set_info, DeviceName);
	    return 1;
	}

        strcpy(data, set_info);

	memset(&wrq, 0, sizeof(wrq));
	wrq.u.data.length = strlen(data)+1;
	wrq.u.data.pointer = data;
	wrq.u.data.flags = 0;

	if (wlan_ioctl(RTPRIV_IOCTL_SET, DeviceName, &wrq))
	{
	    syslog(LOG_ERR, "wlan: ioctl failed, %s, (%s, %s)", __FUNCTION__, set_info, DeviceName);
	    free(data);
	    return 2;
	}

	free(data);
	return 0;
}

int OidQueryInformation(unsigned long OidQueryCode, const char *DeviceName, void *ptr, unsigned long PtrLength)
{
	struct iwreq wrq;

	memset(ptr, 0, PtrLength);
	memset(&wrq, 0, sizeof(wrq));

	wrq.u.data.length = PtrLength;
	wrq.u.data.pointer = (caddr_t)ptr;
	wrq.u.data.flags = OidQueryCode;

	if (wlan_ioctl(RT_PRIV_IOCTL, DeviceName, &wrq))
	{
	    syslog(LOG_ERR, "wlan: ioctl failed, %s, (%lu, %s)", __FUNCTION__, OidQueryCode, DeviceName);
	    return -1;
	}

	return 0;
}

int OidSetInformation(unsigned long OidQueryCode, const char *DeviceName, void *ptr, unsigned long PtrLength)
{
	struct iwreq wrq;

	memset(&wrq, 0, sizeof(wrq));
	strlcpy(wrq.ifr_name, DeviceName, sizeof(wrq.ifr_name));
	wrq.u.data.length = PtrLength;
	wrq.u.data.pointer = (caddr_t) ptr;
	wrq.u.data.flags = OidQueryCode | OID_GET_SET_TOGGLE;

	if (wlan_ioctl(RT_PRIV_IOCTL, DeviceName, &wrq))
	{
	    syslog(LOG_ERR, "wlan: call ioctl sock failed, %s", __FUNCTION__);
	    return -1;
	}

        return 0;
}

unsigned int ConvertRssiToSignalQuality(long RSSI)
{
	unsigned int signal_quality;

	if (RSSI >= -50)
		signal_quality = 100;
	else if (RSSI >= -80)    // between -50 ~ -80dbm
		signal_quality = (unsigned int)(24 + ((RSSI + 80) * 26)/10);
	else if (RSSI >= -90)   // between -80 ~ -90dbm
		signal_quality = (unsigned int)(((RSSI + 90) * 26)/10);
	else    // < -84 dbm
		signal_quality = 0;

	return signal_quality;
}

char* getBW(int BW)
{
	switch(BW)
	{
	    case BW_10:
		return "10MHz";
	    case BW_20:
		return "20MHz";
	    case BW_40:
		return "40MHz";
	    case BW_80:
		return "80MHz";
	    case BW_160:
		return "160MHz";
	    default:
		return "N/A";
	}
}

char* getPhyMode(int Mode)
{
	switch(Mode)
	{
	    case MODE_CCK:
		return "CCK";
	    case MODE_OFDM:
		return "OFDM";
	    case MODE_HTMIX:
		return "HTMIX";
	    case MODE_HTGREENFIELD:
		return "HTGF";
	    case MODE_VHT:
		return "VHT";
	    default:
		return "N/A";
	}
}

int getMCS(MACHTTRANSMIT_SETTING HTSetting)
{
	int mcs_1ss = (int)HTSetting.field.MCS;

	if (HTSetting.field.MODE >= MODE_VHT)
		if (mcs_1ss > 9)
			mcs_1ss %= 16;

	return mcs_1ss;
}

const int MCSMappingRateTable[] =
{
	 2,  4,   11,  22,								// CCK

	12,  18,  24,  36,  48,  72,  96, 108,						// OFDM

	13,  26,  39,  52,  78, 104, 117, 130, 26,  52,  78, 104, 156, 208, 234, 260,	// 11n: 20MHz, 800ns GI, MCS: 0 ~ 15
	39,  78, 117, 156, 234, 312, 351, 390,						// 11n: 20MHz, 800ns GI, MCS: 16 ~ 23
	27,  54,  81, 108, 162, 216, 243, 270, 54, 108, 162, 216, 324, 432, 486, 540,	// 11n: 40MHz, 800ns GI, MCS: 0 ~ 15
	81, 162, 243, 324, 486, 648, 729, 810,						// 11n: 40MHz, 800ns GI, MCS: 16 ~ 23
	14,  29,  43,  57,  87, 115, 130, 144, 29, 59,   87, 115, 173, 230, 260, 288,	// 11n: 20MHz, 400ns GI, MCS: 0 ~ 15
	43,  87, 130, 173, 260, 317, 390, 433,						// 11n: 20MHz, 400ns GI, MCS: 16 ~ 23
	30,  60,  90, 120, 180, 240, 270, 300, 60, 120, 180, 240, 360, 480, 540, 600,	// 11n: 40MHz, 400ns GI, MCS: 0 ~ 15
	90, 180, 270, 360, 540, 720, 810, 900,

	13,  26,  39,  52,  78, 104, 117, 130, 156,					// 11ac: 20Mhz, 800ns GI, MCS: 0~8
	27,  54,  81, 108, 162, 216, 243, 270, 324, 360,				// 11ac: 40Mhz, 800ns GI, MCS: 0~9
	59, 117, 176, 234, 351, 468, 527, 585, 702, 780,				// 11ac: 80Mhz, 800ns GI, MCS: 0~9
	14,  29,  43,  57,  87, 115, 130, 144, 173,					// 11ac: 20Mhz, 400ns GI, MCS: 0~8
	30,  60,  90, 120, 180, 240, 270, 300, 360, 400,				// 11ac: 40Mhz, 400ns GI, MCS: 0~9
	65, 130, 195, 260, 390, 520, 585, 650, 780, 867					// 11ac: 80Mhz, 400ns GI, MCS: 0~9
};

/* getWlanRate - Get client rate in MBit/s
 *
 * args: HTSetting - input struct from getWlanStationTable(...)
 * return: rate value in MBit/s
 */
int getWlanRate(MACHTTRANSMIT_SETTING HTSetting)
{
	int rate_count = sizeof(MCSMappingRateTable)/sizeof(int);
	int rate_index = 0;
	int num_ss_vht = 1;

	if (HTSetting.field.MODE >= MODE_VHT) {
		int mcs_1ss = (int)HTSetting.field.MCS;

		if (mcs_1ss > 9) {
			num_ss_vht = (mcs_1ss / 16) + 1;
			mcs_1ss %= 16;
		}
		if (HTSetting.field.BW == BW_20)
			rate_index = 108 + ((unsigned char)HTSetting.field.ShortGI * 29) + mcs_1ss;
		else if (HTSetting.field.BW == BW_40)
			rate_index = 117 + ((unsigned char)HTSetting.field.ShortGI * 29) + mcs_1ss;
		else if (HTSetting.field.BW == BW_80)
			rate_index = 127 + ((unsigned char)HTSetting.field.ShortGI * 29) + mcs_1ss;
	}
	else if (HTSetting.field.MODE >= MODE_HTMIX)
		rate_index = 12 + ((unsigned char)HTSetting.field.BW * 24) + ((unsigned char)HTSetting.field.ShortGI * 48) + ((unsigned char)HTSetting.field.MCS);
	else if (HTSetting.field.MODE == MODE_OFDM)
		rate_index = (unsigned char)(HTSetting.field.MCS) + 4;
	else if (HTSetting.field.MODE == MODE_CCK)
		rate_index = (unsigned char)(HTSetting.field.MCS);

	if (rate_index < 0)
		rate_index = 0;

	if (rate_index >= rate_count)
		rate_index = rate_count-1;

	return (MCSMappingRateTable[rate_index] * num_ss_vht * 5)/10;
}

/* getWlanRadioModuleName - Get radio module interface name by its number
 *
 * args: radio_module_ind - radio module index (1..N)
 * return: interface name or NULL
 */
char* getWlanRadioModuleName(int radio_module_ind)
{
	switch (radio_module_ind)
	{
	    case 1: return "ra0";
#ifndef CONFIG_RT_SECOND_IF_NONE
	    case 2: return "rai0";
#else
	    case 2: return 0;
#endif

	    default:
		    syslog(LOG_ERR, "libwive wireless : %s - Unknown radio module number!", __FUNCTION__); 
		    return 0;
	}
}

int getWlanStationTable(RT_802_11_MAC_TABLE* table, int radio_module_ind)
{
	char * if_name = getWlanRadioModuleName(radio_module_ind);
	return getWlanStationTableIf(table, if_name);
}

int getWlanStationTableIf(RT_802_11_MAC_TABLE* table, char *if_name)
{
	if (if_name == NULL)
	    return 2;

	if (RtpQueryInformation(RTPRIV_IOCTL_GET_MAC_TABLE, if_name, table, sizeof(RT_802_11_MAC_TABLE)) < 0) {
		syslog(LOG_ERR, "first wlan: ioctl -> RTPRIV_IOCTL_GET_MAC_TABLE failed, %s", __FUNCTION__);
		return 1;
	}

	return 0;
}

/* getWlanChannelNum - Get the channel number for selected radio module
 *
 * arg: (out) table      - station table
 * arg: radio_module_ind - radio module index (1..N)
 * return: channel number or 0.
 */
int getWlanChannelNum(int radio_module_ind)
{
	switch (radio_module_ind)
	{
	    case 1:
		    if (nvram_get_int(RT2860_NVRAM, "AutoChannelSelect", 0) == 1) return 0;
		    return nvram_get_int(RT2860_NVRAM, "Channel", 9);

#ifndef CONFIG_RT_SECOND_IF_NONE
	    case 2:
		    if (nvram_get_int(RT2860_NVRAM, "AutoChannelSelectINIC", 0) == 1) return 0;
		    return nvram_get_int(RT2860_NVRAM, "ChannelINIC", 48);
#else
	    case 2: return 0;
#endif

	    default:
		    syslog(LOG_ERR, "libwive wireless : %s - Unknown radio module number!", __FUNCTION__); 
	}

	return 0;

}

/* getWlanCurrentMacAddr - Get radio module interface BSSID mac address by its number
 *
 * arg: (out) buf        - char[18] array to store the results
 * arg: radio_module_ind - radio module index (1..N)
 * return: 0 = OK
 */
int getWlanCurrentMacAddr(char *buf, int radio_module_ind)
{
	strcpy(buf, "00:00:00:00:00:00");

	char * if_name = getWlanRadioModuleName(radio_module_ind);
	if (if_name == NULL)
		return 1;

    	if (getIfMac(if_name, buf, ':') == -1)
		return 2;

	return 0;
}

/* getWlanAPMac - Get apclient connected access point mac address
 * arg: if_name - interface name
 * arg: (out) addr - char[ETH_ALEN] field for MAC.
 * return: <0 = Error, 0 = Not connected, >0 = connected
*/
int getWlanAPMac(const char *if_name, char *addr)
{

	struct iwreq wrq;

	memset(&wrq, 0, sizeof(wrq));

	if (wlan_ioctl(SIOCGIWAP, if_name, &wrq) >= 0)
	{
	    //p_wrq.u.ap_addr.sa_family = ARPHRD_ETHER;
	    memcpy(addr, wrq.u.ap_addr.sa_data, ETH_ALEN);
	}
	else
	{
	    syslog(LOG_ERR, "wlan: ioctl failed, %s, (%s)", __FUNCTION__, if_name);
	    return -1;
	}

        return addr[0] || addr[1] || addr[2] || addr[3] || addr[4] || addr[5];
}

/* getWlanMacEntry - Get single interface MAC entry
 * arg: if_name - interface name
 * arg: (out) entry - output entry
 * return: 0 = OK, 1 = not connected, -1 = Error
*/
int getWlanMacEntry(const char *if_name, RT_802_11_MAC_ENTRY *entry)
{
	if (RtpQueryInformation(RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT, if_name, entry, sizeof(RT_802_11_MAC_ENTRY)) < 0) {
		syslog(LOG_ERR, "wlan: ioctl -> RT_802_11_MAC_ENTRY failed, %s", __FUNCTION__);
		return -1;
	}

	if (entry->Addr[0] || entry->Addr[1] || entry->Addr[2] || entry->Addr[3] || entry->Addr[4] || entry->Addr[5]) 
	    return 0;
	else
	    return 1;
}

/* wlanAPScanText - Scan for wireless access points (text output)
 * arg: if_name - interface name
 * arg: (out) data - output data buffer
 * arg: data_len - maximum data buffer size
 * return: num bytes read, <= 0 for errors
*/
int wlanAPScanText(const char *if_name, char* data, unsigned int data_len)
{

	if (RtpSetInformation("SiteSurvey=1", if_name))
	{
		syslog(LOG_ERR, "wlan: SiteSurvey ioctl set failed, %s", __FUNCTION__);
		return -1;
	}

	sleep(4);

	return RtpQueryInformation(RTPRIV_IOCTL_GSITESURVEY, if_name, data, data_len);
}

/* parseSiteSurveyEntry - internal WLAN_AP_ENTRY parsing function
 *
 * arg: (out) entry - preallocated WLAN_AP_ENTRY struct pointer
 * arg: line - input data to parse (will be destroyed during parse!)
 * return: WLAN_AP_ENTRY array or NULL
 */
static int parseSiteSurveyEntry(struct WLAN_AP_ENTRY* entry, char* line)
{
    char* arg_start = NULL;
    char* arg_end = NULL;
    int arg_num = 0;
    int is_delimiter = 1;
    char* ptr;

    if (line == NULL)
    {
        return 1;
    }

    for (ptr = line + strlen(line); ptr>line; ptr--)
    {
        int is_delimiter_symbol = (ptr[0] == ' ' || ptr[0] == '\0' || ptr[0] == '\n');

        if (is_delimiter)
        {
            if (is_delimiter_symbol)
            {
                ptr[0] = '\0';
            }
            else
            {
                is_delimiter = 0;
                arg_end = ptr;
            }
        }
        else
        if (!is_delimiter && is_delimiter_symbol)
        {
            is_delimiter = 1;
            arg_start = ptr + 1;
            arg_num++;

            switch (arg_num)
            {
                case 1:
                    strlcpy(entry->nt, arg_start, sizeof(entry->nt));
                    break;
                case 2:
                    strlcpy(entry->extch, arg_start, sizeof(entry->extch));
                    break;
                case 3:
                    strlcpy(entry->wmode, arg_start, sizeof(entry->wmode));
                    break;
                case 4:
                    entry->signal_percent = strToIntDef(arg_start, 0);
                    break;
                case 5:
                    strlcpy(entry->security, arg_start, sizeof(entry->security));
                    break;
                case 6:
                    if (sscanf(arg_start, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", 
                        &entry->bssid[0],
                        &entry->bssid[1],
                        &entry->bssid[2],
                        &entry->bssid[3],
                        &entry->bssid[4],
                        &entry->bssid[5]) != 6)
                    {
                        return 2;
                    }

                    break;
            }

            if (arg_num == 6) break;
        }
    }

    if (arg_num != 6)
    {
        return 3;
    }

    /* chan - ssid split */
    arg_start[0] = '\0';

    arg_start = strchr(line, ' ');

    if (arg_start == NULL)
    {
        return 4;
    }

    arg_start[0] = '\0';
    arg_start++;

    entry->chan = strToIntDef(line, 0);


    /* ssid trim left */
    while (arg_start[0] == ' ')
    {
        arg_start++;
    }

    /* ssid trim right */
    arg_end = arg_start + strlen(arg_start) - 1;
    while (arg_end>arg_start && (arg_end[0] == ' '))
    {
        arg_end[0] = '\0';
        arg_end--;
    }

    strlcpy(entry->ssid, arg_start, sizeof(entry->ssid));

    return 0;
}

/* wlanAPScan - Scan for wireless access points (struct output)
 *
 * arg: if_name - interface name to scan
 * arg: (out) entry_num - output array element count
 * return: WLAN_AP_ENTRY array or NULL
 */
struct WLAN_AP_ENTRY* wlanAPScan(const char *if_name, int *entry_num)
{
    char data[8192];
    char *ptr;
    int ent_capacity = 32;
    int ent_cur = 0;
    struct WLAN_AP_ENTRY* entries;
    *(entry_num) = 0;

    if (wlanAPScanText(if_name, data, 8192) <= 0)
    {
        syslog(LOG_ERR, "wlan: SiteSurvey failed, %s", __FUNCTION__);
        return NULL;
    }

    ptr = strtok(data, "\n");

    entries = calloc(ent_capacity, sizeof(struct WLAN_AP_ENTRY));
    if (!entries) {
        syslog(LOG_ERR, "wlan: APScan memory alloc failed, %s", __FUNCTION__);
        return NULL;
    }

    while (ptr)
    {
        struct WLAN_AP_ENTRY* entry = &entries[ent_cur];
        int err_code = parseSiteSurveyEntry(entry, ptr);

        if (err_code == 0)
        {
            ent_cur++;

            if (ent_cur >= ent_capacity)
            {
                ent_capacity += 32;

                struct WLAN_AP_ENTRY* new_entries = realloc(entries, ent_capacity);
                if (new_entries != NULL)
                {
                    entries = new_entries;
                }
                else
                {
                    break;
                }
            }
        }

        ptr = strtok(NULL, "\n");
    }

    *(entry_num) = ent_cur;
    return entries;
}

#if defined(CONFIG_RT2860V2_STA) || defined(CONFIG_RT2860V2_STA_MODULE) || defined(CONFIG_MT76X2_STA) || defined(CONFIG_MT76X2_STA_MODULE) || defined(CONFIG_MT76X3_STA) || defined(CONFIG_MT76X3_STA_MODULE)
/* getWlanStationLinkQuality - Get percentage of station link quality
 *
 * arg: radio_module_ind - radio module index (1..N)
 * return: link quality (0..100) or zero in case of errors
 */
int getWlanStationLinkQuality(int radio_module_ind)
{
	RT_802_11_LINK_STATUS LinkStatus;

	char * if_name = getWlanRadioModuleName(radio_module_ind);
	if (if_name == NULL)
		return 0;

	// Get Link Status Info from driver
	if (OidQueryInformation(RT_OID_802_11_QUERY_LINK_STATUS, if_name, &LinkStatus, sizeof(RT_802_11_LINK_STATUS)) != 0)
	    return 0;

	LinkStatus.ChannelQuality = (unsigned long)(LinkStatus.ChannelQuality * 1.2 + 10);
	if (LinkStatus.ChannelQuality > 100)
		LinkStatus.ChannelQuality = 100;

	return (int)LinkStatus.ChannelQuality;
}

/* getWlanStationLinkQuality - Get station current frequency
 *
 * arg: radio_module_ind - radio module index (1..N)
 * return: station frequency in KHz, < 0 in case of errors
 */
int getWlanStationFrequencyKHz(int radio_module_ind)
{
	NDIS_802_11_CONFIGURATION Configuration;

	char * if_name = getWlanRadioModuleName(radio_module_ind);
	if (if_name == NULL)
		return -1;

	if (OidQueryInformation(OID_802_11_CONFIGURATION, if_name, &Configuration, sizeof(NDIS_802_11_CONFIGURATION)) != 0)
		return -2;

	return Configuration.DSConfig;

}

/* getWlanStationNoiseDbm - Get station current noise level
 *
 * arg: radio_module_ind - radio module index (1..N)
 * return: station noise level in Dbm, < 0 in case of errors
 */
int getWlanStationNoiseDbm(int radio_module_ind)
{
	unsigned char lNoise; // this value is (ULONG) in Ndis driver (NOTICE!!!)

	char * if_name = getWlanRadioModuleName(radio_module_ind);
	if (if_name == NULL)
		return -1;

	// Noise Level
	// Get Noise Level From Driver
	OidQueryInformation(RT_OID_802_11_QUERY_NOISE_LEVEL, if_name, &lNoise, sizeof(lNoise));

	return lNoise-143;
}

static char bGetHTTxRateByBW_GI_MCS(int nBW, int nGI, int nMCS, double* dRate)
{
	//fprintf(stderr, "bGetHTTxRateByBW_GI_MCS()\n");
	double HTTxRate20_800[24]={6.5, 13.0, 19.5, 26.0, 39.0, 52.0, 58.5, 65.0, 13.0, 26.0, 39.0, 52.0, 78.0, 104.0, 117.0, 130.0,
								19.5, 39.0, 58.5, 78.0, 117.0, 156.0, 175.5, 195.0};
	double HTTxRate20_400[24]={7.2, 14.4, 21.7, 28.9, 43.3, 57.8, 65.0, 72.2, 14.444, 28.889, 43.333, 57.778, 86.667, 115.556, 130.000, 144.444,
								21.7, 43.3, 65.0, 86.7, 130.0, 173.3, 195.0, 216.7};
	double HTTxRate40_800[25]={13.5, 27.0, 40.5, 54.0, 81.0, 108.0, 121.5, 135.0, 27.0, 54.0, 81.0, 108.0, 162.0, 216.0, 243.0, 270.0,
								40.5, 81.0, 121.5, 162.0, 243.0, 324.0, 364.5, 405.0, 6.0};
	double HTTxRate40_400[25]={15.0, 30.0, 45.0, 60.0, 90.0, 120.0, 135.0, 150.0, 30.0, 60.0, 90.0, 120.0, 180.0, 240.0, 270.0, 300.0,
								45.0, 90.0, 135.0, 180.0, 270.0, 360.0, 405.0, 450.0, 6.7};

	// no TxRate for (BW = 20, GI = 400, MCS = 32) & (BW = 20, GI = 400, MCS = 32)
	if (((nBW == BW_20) && (nGI == GI_400) && (nMCS == 32)) ||
			((nBW == BW_20) && (nGI == GI_800) && (nMCS == 32)))
	{
		return 0;
	}

	if (nMCS == 32)
		nMCS = 25;

	if (nBW == BW_20 && nGI == GI_800)
		*dRate = HTTxRate20_800[nMCS];
	else if (nBW == BW_20 && nGI == GI_400)
		*dRate = HTTxRate20_400[nMCS];
	else if (nBW == BW_40 && nGI == GI_800)
		*dRate = HTTxRate40_800[nMCS];
	else if (nBW == BW_40 && nGI == GI_400)
		*dRate = HTTxRate40_400[nMCS];
	else
		return 0; //false

	//fprintf(stderr, "dRate=%.1f\n", *dRate);
	return 1; //true
}

/* (static) getLastTxRxRateFor11n - Get station 11n RX/TX rate
 *
 * arg: nID                 - OID identifier: RT_OID_802_11_QUERY_LAST_TX_RATE or RT_OID_802_11_QUERY_LAST_RX_RATE
 * arg: (out) fLastTxRxRate - output rate
 * return: 0 = OK
 */
static int getLastTxRxRateFor11n(int nID, double* fLastTxRxRate)
{
	unsigned long lHTSetting;
	MACHTTRANSMIT_SETTING HTSetting;
	double b_mode[] ={1, 2, 5.5, 11};
	float g_Rate[] = { 6,9,12,18,24,36,48,54};

	if (OidQueryInformation(nID, "ra0", &lHTSetting, sizeof(lHTSetting)) < 0)
	    return 1;

	memset(&HTSetting, 0x00, sizeof(HTSetting));
	memcpy(&HTSetting, &lHTSetting, sizeof(HTSetting));

	switch(HTSetting.field.MODE)
	{
		case 0:
			if (HTSetting.field.MCS >=0 && HTSetting.field.MCS<=3)
				*fLastTxRxRate = b_mode[HTSetting.field.MCS];
			else if (HTSetting.field.MCS >=8 && HTSetting.field.MCS<=11)
				*fLastTxRxRate = b_mode[HTSetting.field.MCS-8];
			else
				*fLastTxRxRate = 0;

			break;
		case 1:
			if ((HTSetting.field.MCS >= 0) && (HTSetting.field.MCS < 8))
				*fLastTxRxRate = g_Rate[HTSetting.field.MCS];
			else
				*fLastTxRxRate = 0;

			break;
		case 2:
		case 3:
			if (0 == bGetHTTxRateByBW_GI_MCS(HTSetting.field.BW,
						HTSetting.field.ShortGI,
						HTSetting.field.MCS,
						fLastTxRxRate))
			{
				*fLastTxRxRate = 0;
			}
			break;
		default:
			*fLastTxRxRate = 0;
			break;
	}

	return 0;
}

/* getLastTxRxRateFor11n - Get station 11n TX rate
 *
 * arg: (out) fLastTxRxRate - output rate
 * return: 0 = OK
 */
int getLastTxRateFor11n(double* fLastTxRate)
{
    return getLastTxRxRateFor11n(RT_OID_802_11_QUERY_LAST_TX_RATE, fLastTxRate);
}

/* getLastTxRxRateFor11n - Get station 11n RX rate
 *
 * arg: (out) fLastTxRxRate - output rate
 * return: 0 = OK
 */
int getLastRxRateFor11n(double* fLastRxRate)
{
    return getLastTxRxRateFor11n(RT_OID_802_11_QUERY_LAST_RX_RATE, fLastRxRate);
}

/* getWlanHWRadioStatus - Get station hardware radio module status
 *
 * arg: radio_module_ind - radio module index (1..N)
 * return: > 0 = On, 0 = Off, < 0 = Error
 */
int getWlanHWRadioStatus(int radio_module_ind)
{
	unsigned long RadioStatus=0;

	char * if_name = getWlanRadioModuleName(radio_module_ind);
	if (if_name == NULL)
    	    return -1;

	if (OidQueryInformation(RT_OID_802_11_RADIO, "ra0", &RadioStatus, sizeof(RadioStatus)) < 0)
	    return -2;

	return RadioStatus==1;
}
#endif
