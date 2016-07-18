#ifndef _WIVE_WIRELESS_H
#define _WIVE_WIRELESS_H	1

int RtpQueryInformation(unsigned long QueryCode, int socket_id, char *DeviceName, void *ptr, unsigned long PtrLength);
unsigned int ConvertRssiToSignalQuality(long RSSI);

#endif
