/*
 EULA:

 WIVE FIRMWARE IS FREE FOR THE NON-COMMERCIAL USE ONLY.

 Conditions of commercial use non GPL (or other not viral license) components are discussed individually.
 For a more detailed discussion please mail me.
 News and changes in Russian: http://wive-ng.sf.net
 Bug report please send to support[at]nag.ru or sfstudio[at]mail.ru
*/

#include "libwive.h"

#include "oid.h"

int RtpQueryInformation(unsigned long QueryCode, int socket_id, char *DeviceName, void *ptr, unsigned long PtrLength)
{
	struct iwreq wrq;

	memset(&wrq, 0, sizeof(wrq));
	strncpy(wrq.ifr_name, DeviceName, IFNAMSIZ - 1);
	wrq.ifr_name[IFNAMSIZ - 1] = '\0';
	wrq.u.data.length = PtrLength;
	wrq.u.data.pointer = (caddr_t)ptr;

	return (ioctl(socket_id, QueryCode, &wrq));
}

int OidQueryInformation(unsigned long OidQueryCode, int socket_id, char *DeviceName, void *ptr, unsigned long PtrLength)
{
	struct iwreq wrq;

	memset(&wrq, 0, sizeof(wrq));
	strncpy(wrq.ifr_name, DeviceName, IFNAMSIZ - 1);
	wrq.ifr_name[IFNAMSIZ - 1] = '\0';
	wrq.u.data.length = PtrLength;
	wrq.u.data.pointer = (caddr_t)ptr;
	wrq.u.data.flags = OidQueryCode;

	if (OidQueryCode == OID_802_11_BSSID_LIST)
		wrq.u.data.length = 8192;

	return (ioctl(socket_id, RT_PRIV_IOCTL, &wrq));
}

int OidSetInformation(unsigned long OidQueryCode, int socket_id, char *DeviceName, void *ptr, unsigned long PtrLength)
{
	struct iwreq wrq;

	memset(&wrq, 0, sizeof(wrq));
	strncpy(wrq.ifr_name, DeviceName, IFNAMSIZ - 1);
	wrq.ifr_name[IFNAMSIZ - 1] = '\0';
	wrq.u.data.length = PtrLength;
	wrq.u.data.pointer = (caddr_t) ptr;
	wrq.u.data.flags = OidQueryCode | OID_GET_SET_TOGGLE;

	return (ioctl(socket_id, RT_PRIV_IOCTL, &wrq));
}

unsigned int ConvertRssiToSignalQuality(long RSSI)
{
	unsigned int signal_quality;
	if (RSSI >= -50)
		signal_quality = 100;
	else if (RSSI >= -80)    // between -50 ~ -80dbm
		signal_quality = (unsigned int)(24 + (RSSI + 80) * 2.6);
	else if (RSSI >= -90)   // between -80 ~ -90dbm
		signal_quality = (unsigned int)((RSSI + 90) * 2.6);
	else    // < -84 dbm
		signal_quality = 0;

	return signal_quality;
}
