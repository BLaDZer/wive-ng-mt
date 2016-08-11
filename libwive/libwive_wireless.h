#ifndef _WIVE_WIRELESS_H
#define _WIVE_WIRELESS_H	1

#include "oid.h"

int RtpQueryInformation(unsigned long QueryCode, char *DeviceName, void *ptr, unsigned long PtrLength);
int OidQueryInformation(unsigned long OidQueryCode, char *DeviceName, void *ptr, unsigned long PtrLength);
int OidSetInformation(unsigned long OidQueryCode, char *DeviceName, void *ptr, unsigned long PtrLength);

unsigned int ConvertRssiToSignalQuality(long RSSI);

char* getBW(int BW);
char* getPhyMode(int Mode);
int getMCS(MACHTTRANSMIT_SETTING HTSetting);
int getRate(MACHTTRANSMIT_SETTING HTSetting);
int getWlanStationTable(RT_802_11_MAC_TABLE* table, int radio_module_ind);
int getWlanChannelNum(int radio_module_ind);

int getWlanStationLinkQuality(int radio_module_ind);
int getWlanStationFrequencyKHz(int radio_module_ind);
int getWlanStationNoiseDbm(int radio_module_ind);

int getWlanCurrentMacAddr(char *buf, int radio_module_ind);

int getLastTxRateFor11n(double* fLastTxRate);
int getLastRxRateFor11n(double* fLastRxRate);

int getWlanHWRadioStatus(int radio_module_ind);

#endif
