#include "libwive.h"

int getWlanStationFrequencyKHz(int radio_module_ind)
{
    return 0;
}

int getWlanChannelNum_ioctl(int radio_module_ind)
{
    return getWlanChannelNum_nvram(radio_module_ind);
}

int getWlanCurrentMacAddr(char *buf, int radio_module_ind)
{
    switch (radio_module_ind) {
        case 1:
            sprintf(buf, "F8:F0:82:00:00:01");
            break;
        case 2:
            sprintf(buf, "F8:F0:82:00:00:02");
            break;
        default :
            sprintf(buf, "F8:F0:82:00:00:03");
    }

    return 0;
}

int getWlanStationTable(RT_802_11_MAC_TABLE* table, int radio_module_ind)
{
    return 0;
}

int getMCS(MACHTTRANSMIT_SETTING HTSetting)
{
    return 0;
}

char* getBW(int BW)
{
    return "0";
}

char* getPhyMode(int Mode)
{
    return "none";
}

int getWlanRate(MACHTTRANSMIT_SETTING HTSetting)
{
    return 0;
}

unsigned int ConvertRssiToSignalQuality(long RSSI)
{
    return 0;
}

int wlanDisconnectAllStations(const char *if_name)
{
    return 0;
}

int wlanDisconnectStation(const char *if_name, char* mac_addr)
{
    return 0;
}

struct WLAN_AP_ENTRY* wlanAPScan(const char *if_name, int *entry_num)
{
    return NULL;
}


int getWlanAPMac(const char *if_name, char *addr)
{
	memset(addr, 0, ETH_ALEN);
	return 0; // not connected
}

int RtpQueryInformation(unsigned long QueryCode, const char *DeviceName, void *ptr, unsigned long PtrLength)
{
    return 0;
}

int RtpSetInformation(const char *set_info, const char *DeviceName)
{
    return 0;
}


int OidQueryInformation(unsigned long OidQueryCode, const char *DeviceName, void *ptr, unsigned long PtrLength)
{
    return 0;
}


int OidSetInformation(unsigned long OidQueryCode, const char *DeviceName, void *ptr, unsigned long PtrLength)
{
    return 0;
}

int getWlanStationNoiseDbm(int radio_module_ind)
{
    return 0;
}

int getWlanStationLinkQuality(int radio_module_ind)
{
    return 0;
}

int getLastTxRateFor11n(double* fLastTxRate)
{
    return 0;
}

int getLastRxRateFor11n(double* fLastRxRate)
{
    return 0;
}

int getWlanHWRadioStatus(int radio_module_ind)
{
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int getWlanChannelNum_nvram(int radio_module_ind)
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

char* getWlanRadioModuleName(int radio_module_ind)
{
	switch (radio_module_ind)
	{
	    case 1: return "ra0";
#ifndef CONFIG_RT_SECOND_IF_NONE
	    case 2: return "rai0";
#else
	    case 2: return NULL;
#endif

	    default:
		    syslog(LOG_ERR, "libwive wireless : %s - Unknown radio module number!", __FUNCTION__); 
		    return NULL;
	}

	return NULL;
}

struct WLAN_AP_ENTRY* getWlanAPScanResult(const char *if_name, int *entry_num)
{
    int ent_capacity = 1;
    struct WLAN_AP_ENTRY* entries;
    *(entry_num) = 0;

    entries = calloc(ent_capacity, sizeof(struct WLAN_AP_ENTRY));
    if (!entries) {
        syslog(LOG_ERR, "wlan: APScan memory alloc failed, %s", __FUNCTION__);
        return NULL;
    }

    return entries;
}
