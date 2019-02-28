#include "libwive.h"

static time_t disconnectTime[] = {0,0};
static time_t connectTime[] = {0,0};


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
    static int AvgRssi = -50;
    static unsigned long long RxBytes[] = {0,0};
    static unsigned long long TxBytes[] = {0,0};    
    
    static char hwaddr[7] = {0};    

    table->Num = 0;
    
    if (!(hwaddr[0] || hwaddr[1] || hwaddr[2] || hwaddr[3] || hwaddr[4] || hwaddr[5] || hwaddr[6]))
    {
        for (int i=0;i<6;i++) {
            hwaddr[i] = (char)(rand()%255);
        }
    }

    if (radio_module_ind == 2)
    {
        return 0;
    }
    
    for (int i=0;i<2;i++) {

	if (connectTime[i] == 0) {
	    connectTime[i] = time(NULL);
	}
    
        if (time(NULL) < (disconnectTime[i]+10))
        {
	    connectTime[i] = 0;
	    continue;
        }

        table->Num++;

        int traffic_bands[] = { 11, 17, 31, 67, 139, 293, 541, 821, 1601 };
	int bandnum = sizeof(traffic_bands) / sizeof(traffic_bands[0]);
    
        time_t tm = time(NULL);
	for (int j=0;j<bandnum;j++)
        {
            if ((tm%(traffic_bands[j]+i)) > (traffic_bands[j]/2)) {
                RxBytes[i] += (rand()%(117851)) / (i+1);
                TxBytes[i] += (rand()%(117851)) / (i+1);
	    }
        }
	

	int aid = table->Num-1;
        RT_802_11_MAC_ENTRY* entry = &(table->Entry[aid]);

        if (rand()%3 == 0) {
            if ((rand()%100) > -AvgRssi) {
                AvgRssi--;
            } else {
                AvgRssi++;
            }
        }

        if (AvgRssi < -80) {
            AvgRssi = -80;
        }
        else
        if (AvgRssi > -30) {
            AvgRssi = -30;
        }

        entry->ApIdx = 0;
        strcpy(entry->Addr, hwaddr);
    	entry->Addr[5] = 1 + i;

        entry->Aid = aid+1;
        entry->Psm = 1;
        entry->MimoPs = 1;
        entry->AvgRssi0 = AvgRssi - i * 5;
        entry->AvgRssi1 = AvgRssi-rand()%5 - i * 5;
        entry->AvgRssi2 = AvgRssi-rand()%7 - i * 5;
        entry->TxBytes = TxBytes[i];
        entry->RxBytes = RxBytes[i];
        entry->ConnectedTime = 1+time(NULL)-connectTime[i];
        entry->TxRate.field.MODE = MODE_HTMIX;
        entry->TxRate.field.STBC = 1;
        entry->LastRxRate = 1000;
    }
    
    return 0;
}

int getMCS(MACHTTRANSMIT_SETTING HTSetting)
{
    return 4;
}

int getWlanRate(MACHTTRANSMIT_SETTING HTSetting)
{
    return 39;
}

int wlanDisconnectAllStations(const char *if_name)
{
    for (int i=0;i<sizeof(disconnectTime)/sizeof(disconnectTime[0]);i++)
    {
        disconnectTime[i] = time(NULL);
    }
    return 0;
}

int wlanDisconnectStation(const char *if_name, char* mac_addr)
{
    int dislen = sizeof(disconnectTime)/sizeof(disconnectTime[0]);
    
    char *lastoct = strrchr(mac_addr, ':')+1;
    if (lastoct == NULL || strlen(lastoct) != 2) {
	return 0;
    }
    
    int did = lastoct[1]-'1';
    if (did >= 0 && did < dislen) {
        disconnectTime[did] = time(NULL);
    }
    
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

unsigned int ConvertRssiToSignalQuality(long RSSI)
{
	unsigned int signal_quality;

	if (RSSI >= 0)
		signal_quality = 0;
	else if (RSSI >= -50)
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
	    case BW_5:
		return "5MHz";
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
	    case BW_8080:
		return "80+80MHz";
	    default:
		return "N/A";
	}

        return NULL;
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

        return NULL;
}
