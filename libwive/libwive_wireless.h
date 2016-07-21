#ifndef _WIVE_WIRELESS_H
#define _WIVE_WIRELESS_H	1

#include "oid.h"

int RtpQueryInformation(unsigned long QueryCode, char *DeviceName, void *ptr, unsigned long PtrLength);
int OidQueryInformation(unsigned long OidQueryCode, int socket_id, char *DeviceName, void *ptr, unsigned long PtrLength);
int OidSetInformation(unsigned long OidQueryCode, int socket_id, char *DeviceName, void *ptr, unsigned long PtrLength);

unsigned int ConvertRssiToSignalQuality(long RSSI);

char* getBW(int BW);
char* getPhyMode(int Mode);
int getMCS(MACHTTRANSMIT_SETTING HTSetting);
int getRate(MACHTTRANSMIT_SETTING HTSetting);
#endif
