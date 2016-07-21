/*
 EULA:

 WIVE FIRMWARE IS FREE FOR THE NON-COMMERCIAL USE ONLY.

 Conditions of commercial use non GPL (or other not viral license) components are discussed individually.
 For a more detailed discussion please mail me.
 News and changes in Russian: http://wive-ng.sf.net
 Bug report please send to support[at]nag.ru or sfstudio[at]mail.ru
*/

#include "libwive.h"

int RtpQueryInformation(unsigned long QueryCode, char *DeviceName, void *ptr, unsigned long PtrLength)
{
	struct iwreq wrq;
	int s = socket(AF_INET, SOCK_DGRAM, 0);

	if (s < 0) {
		syslog(LOG_ERR, "wlan: ioctl open sock failed, %s", __FUNCTION__);
		return -1;
	}

	memset(&wrq, 0, sizeof(wrq));
	strncpy(wrq.ifr_name, DeviceName, IFNAMSIZ - 1);
	wrq.ifr_name[IFNAMSIZ - 1] = '\0';
	wrq.u.data.length = PtrLength;
	wrq.u.data.pointer = (caddr_t)ptr;

	if (ioctl(s, QueryCode, &wrq) < 0) {
	    syslog(LOG_ERR, "wlan: call ioctl sock failed, %s", __FUNCTION__);
	    close(s);
	    return -1;
	}

	close(s);
	return 0;
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

int getRate(MACHTTRANSMIT_SETTING HTSetting)
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
