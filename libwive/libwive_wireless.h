#ifndef _WIVE_WIRELESS_H
#define _WIVE_WIRELESS_H	1

#include "oid.h"

int RtpQueryInformation(unsigned long QueryCode, const char *DeviceName, void *ptr, unsigned long PtrLength);
int RtpSetInformation(const char *set_info, const char *DeviceName);
int OidQueryInformation(unsigned long OidQueryCode, const char *DeviceName, void *ptr, unsigned long PtrLength);
int OidSetInformation(unsigned long OidQueryCode, const char *DeviceName, void *ptr, unsigned long PtrLength);

unsigned int ConvertRssiToSignalQuality(long RSSI);

char* getBW(int BW);
char* getPhyMode(int Mode);
int getMCS(MACHTTRANSMIT_SETTING HTSetting);
int getWlanRate(MACHTTRANSMIT_SETTING HTSetting);

int getWlanStationTable(RT_802_11_MAC_TABLE* table, int radio_module_ind);
int getWlanStationTableIf(RT_802_11_MAC_TABLE* table, char *if_name);

int getWlanCurrentMacAddr(char *buf, int radio_module_ind);
int getWlanChannelNum(int radio_module_ind);

int getWlanAPMac(const char *if_name, char *addr);
int getWlanWDSEntry(const char *if_name, RT_802_11_MAC_ENTRY *entry);

int wlanAPScanText(const char *if_name, char* data, unsigned int data_len);
struct WLAN_AP_ENTRY* wlanAPScan(const char *if_name, int *entry_num);

#if defined(CONFIG_RT2860V2_STA) || defined(CONFIG_RT2860V2_STA_MODULE) || defined(CONFIG_MT76X2_STA) || defined(CONFIG_MT76X2_STA_MODULE) || defined(CONFIG_MT76X3_STA) || defined(CONFIG_MT76X3_STA_MODULE)
int getWlanStationLinkQuality(int radio_module_ind);
int getWlanStationFrequencyKHz(int radio_module_ind);
int getWlanStationNoiseDbm(int radio_module_ind);

int getLastTxRateFor11n(double* fLastTxRate);
int getLastRxRateFor11n(double* fLastRxRate);

int getWlanHWRadioStatus(int radio_module_ind);
#endif

#endif
