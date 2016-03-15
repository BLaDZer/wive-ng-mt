#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <linux/autoconf.h>

// for max bssid num
#include "oid.h"

#include "libnvram.h"

#define DEFAULT_FLASH_ZONE_NAME "2860"

static int set_usage(char *aout)
{
	int i;

	printf("Usage example: \n");
	for (i = 0; i < getNvramNum(); i++){
		printf("\t%s %s ", aout, getNvramName(i));
		printf("lan_ipaddr 1.2.3.4\n");
	}
	return -1;
}

static int get_usage(char *aout)
{
	int i;

	printf("Usage: \n");
	for (i = 0; i < getNvramNum(); i++){
		printf("\t%s %s ", aout, getNvramName(i));
		printf("lan_ipaddr\n");
	}
	return -1;
}

static int buf_get_usage(char *aout)
{
	int i;

	printf("Usage: \n");
	for (i = 0; i < getNvramNum(); i++){
		printf("\t%s %s ", aout, getNvramName(i));
		printf("lan_ipaddr wan_ipaddr ...\n");
	}
	return -1;
}

static int ra_nv_set(int argc,char **argv)
{
	int index, rc;
	char *fz = "", *key = "", *value = "";

	if (argc == 1 || argc > 5)
		return set_usage(argv[0]);

	if (argc == 2) {
		fz = DEFAULT_FLASH_ZONE_NAME;
		key = argv[1];
		value = "";
	} else if (argc == 3) {
		fz = DEFAULT_FLASH_ZONE_NAME;
		key = argv[1];
		value = argv[2];
	} else {
		fz = argv[1];
		key = argv[2];
		value = argv[3];
	}

	if ((index = getNvramIndex(fz)) == -1) {
		fprintf(stderr,"%s: Error: \"%s\" flash zone not existed\n", argv[0], fz);
		return set_usage(argv[0]);
	}

	rc = nvram_set(index, key, value);

	return rc;
}

static int ra_nv_get(int argc, char *argv[])
{
	char *fz;
	char *key;
	char *rc;

	int index, ret;

	if (argc != 3 && argc != 2)
		return get_usage(argv[0]);

	if (argc == 2) {
		fz = DEFAULT_FLASH_ZONE_NAME;
		key = argv[1];
	} else {
		fz = argv[1];
		key = argv[2];
	}

	if ((index = getNvramIndex(fz)) == -1) {
		fprintf(stderr,"%s: Error: \"%s\" flash zone not existed\n", argv[0], fz);
		return get_usage(argv[0]);
	}

	rc = nvram_get(index, key);
	if (rc) {
	    printf("%s\n", rc);
	    ret = 0;
	} else {
	    fprintf(stderr, "nvram_get return error or not return data!\n");
	    ret = -1;
	}

	return (ret);
}

static int ra_nv_buf_get(int argc, char *argv[])
{
	char *fz, *key, *rc;
	int i, index, ret=0;

	if (argc < 3)
		return buf_get_usage(argv[0]);

	fz = argv[1];
	key = argv[2];

	if ((index = getNvramIndex(fz)) == -1) {
		fprintf(stderr,"%s: Error: \"%s\" flash zone not existed\n", argv[0], fz);
		return get_usage(argv[0]);
	}

	for (i = 2; i < argc; i++) {
	    rc = nvram_get(index, argv[i]);
	    if (rc) {
		printf("%s=\"%s\"\n",argv[i], rc);
	    } else {
		fprintf(stderr, "nvram_buf_get return error or not return data!\n");
		ret = -1;
		break;
	    }
	}

	return (ret);
}

static int isMacValid(char *str)
{
	int i, len = strlen(str);
	if(len != 17)
		return 0;

	for ( i = 0; i < 5; i++ )
	{
		if( (!isxdigit( str[i*3])) || (!isxdigit( str[i*3+1])) || (str[i*3+2] != ':') )
			return 0;
	}

	return (isxdigit(str[15]) && isxdigit(str[16])) ? 1: 0;
}

static int nvram_load_default(void)
{
	/* default macs is OK */
	int mac_err = 0;
        char *LAN_MAC_ADDR, *WAN_MAC_ADDR, *WLAN_MAC_ADDR, *WLAN2_MAC_ADDR, *CHECKMAC, *MngmtLogin, *MngmtPassword, *MngmtStoreSettings, *cwmpdEnabled;

	/* copy old remotemanagment settings if enabled */
	MngmtStoreSettings = nvram_get_copy(RT2860_NVRAM, "MngmtStoreSettings");
	if (atoi(MngmtStoreSettings) == 1) {
	    MngmtLogin		= nvram_get_copy(RT2860_NVRAM, "MngmtLogin");
	    MngmtPassword	= nvram_get_copy(RT2860_NVRAM, "MngmtPassword");
	    cwmpdEnabled	= nvram_get_copy(RT2860_NVRAM, "cwmpdEnabled");

	    printf("Store remote managment user settings.\n");
	}

	/* copy old values to new memory block */
	WLAN2_MAC_ADDR	= nvram_get_copy(RT2860_NVRAM, "WLAN2_MAC_ADDR");
	WLAN_MAC_ADDR	= nvram_get_copy(RT2860_NVRAM, "WLAN_MAC_ADDR");
	WAN_MAC_ADDR	= nvram_get_copy(RT2860_NVRAM, "WAN_MAC_ADDR");
	LAN_MAC_ADDR	= nvram_get_copy(RT2860_NVRAM, "LAN_MAC_ADDR");
	CHECKMAC	= nvram_get_copy(RT2860_NVRAM, "CHECKMAC");

	printf("Store current MAC adresses in mem: %s, %s, %s, %s. CHECKMAC: %s\n",
		WAN_MAC_ADDR, LAN_MAC_ADDR, WLAN_MAC_ADDR, WLAN2_MAC_ADDR, CHECKMAC);

	printf("Clear nvram.\n");
	nvram_clear(RT2860_NVRAM);
	printf("Load defaults nvram.\n");
	nvram_renew(RT2860_NVRAM, "/etc/default/nvram_default");

	printf("Reinit nvram before commit.\n");
	if (nvram_init(RT2860_NVRAM) == -1) {
		printf("nvram_load_default: init error!.\n");
		return -1;
	}

	printf("Restore saved in mem MAC adresses : %s, %s, %s, %s. CHECKMAC: %s\n",
		WAN_MAC_ADDR, LAN_MAC_ADDR, WLAN_MAC_ADDR, WLAN2_MAC_ADDR, CHECKMAC);

	if ((strlen(WLAN2_MAC_ADDR) > 0) && isMacValid(WLAN2_MAC_ADDR))
	    nvram_bufset(RT2860_NVRAM, "WLAN2_MAC_ADDR", WLAN2_MAC_ADDR);
	else
	    mac_err++;
	if ((strlen(WLAN_MAC_ADDR) > 0) && isMacValid(WLAN_MAC_ADDR))
	    nvram_bufset(RT2860_NVRAM, "WLAN_MAC_ADDR", WLAN_MAC_ADDR);
	else
	    mac_err++;
	if ((strlen(WAN_MAC_ADDR) > 0) && isMacValid(WAN_MAC_ADDR))
    	    nvram_bufset(RT2860_NVRAM, "WAN_MAC_ADDR",  WAN_MAC_ADDR);
	else
	    mac_err++;
	if ((strlen(LAN_MAC_ADDR) > 0) && isMacValid(LAN_MAC_ADDR))
    	    nvram_bufset(RT2860_NVRAM, "LAN_MAC_ADDR",  LAN_MAC_ADDR);
	else
	    mac_err++;

	/* mac restore ok ? */
	if (mac_err == 0) {
	    printf("MAC adresses restored OK, restore checkmac flag.\n");
    	    nvram_bufset(RT2860_NVRAM, "CHECKMAC", CHECKMAC);
	} else {
	    printf("MAC adresses not correct, set checkmac flag to yes and generate new. err_count=%d.\n", mac_err);
    	    nvram_bufset(RT2860_NVRAM, "CHECKMAC", "YES");
	}

	/* restore remotemanagment */
	if (atoi(MngmtStoreSettings) == 1) {
	    printf("Restore remote managment user settings.\n");
	    if  (strlen(MngmtLogin) > 0 && strlen(MngmtPassword) > 0) {
		nvram_bufset(RT2860_NVRAM, "MngmtLogin", MngmtLogin);
		nvram_bufset(RT2860_NVRAM, "MngmtPassword", MngmtPassword);

		/* allow remote ssh from WAN */
		nvram_bufset(RT2860_NVRAM, "RemoteSSH", "2");
	    }

	    /* restore cwmpd settings */
	    nvram_bufset(RT2860_NVRAM, "cwmpdEnabled", cwmpdEnabled);

	    /* set keep remote managment flag */
	    nvram_bufset(RT2860_NVRAM, "MngmtStoreSettings", "1");
	}

	/* set wive flag */
	nvram_bufset(RT2860_NVRAM, "IS_WIVE", "YES");
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

        return 0;
}

static int gen_wifi_config(int mode, int genmode)
{
	FILE *fp = NULL;
	int  i, ssid_num = 0;
#ifndef CONFIG_KERNEL_NVRAM_SPLIT_INIC
	int inic = 0;
#endif
	char w_mode[64], wmm_enable[64];

	if (genmode == RT2860_NVRAM) {
		system("mkdir -p /etc/Wireless/RT2860");
		fp = fopen("/etc/Wireless/RT2860/RT2860.dat", "w+");
		printf("Build config for fist WiFi module.\n");
	} else if (genmode == RTINIC_NVRAM) {
		system("mkdir -p /etc/Wireless/iNIC");
		fp = fopen("/etc/Wireless/iNIC/iNIC_ap.dat", "w+");
		/* after select file for write back to native 2860 mode */
#ifndef CONFIG_KERNEL_NVRAM_SPLIT_INIC
		/* use one source offset */
		inic = 1;
#endif
		printf("Build config for second WiFi module.\n");
	} else {
		printf("gen_wifi_config: mode unknown...\n");
		return 0;
	}

	if (!fp){
		printf("gen_wifi_config: fopen failed.\n");
		return 0;
	}

	if (nvram_init(mode) == -1) {
		printf("gen_wifi_config: nvram init failed...\n");
		fclose(fp);
		return -1;
	}

	fprintf(fp, "#The word of \"Default\" must not be removed\n");
	fprintf(fp, "Default\n");

#define FPRINT_NUM(x) fprintf(fp, #x"=%d\n", atoi(nvram_get(mode, #x)));
#define FPRINT_STR(x) fprintf(fp, #x"=%s\n", nvram_get(mode, #x));

	// MAC adresses per devices
	if (genmode == RT2860_NVRAM)
	    fprintf(fp, "MacAddress=%s\n", nvram_get(mode, "WLAN_MAC_ADDR"));
	else if (genmode == RTINIC_NVRAM)
	    fprintf(fp, "MacAddress=%s\n", nvram_get(mode, "WLAN2_MAC_ADDR"));

	// Limit numbers ssid
	ssid_num = atoi(nvram_get(mode, "BssidNum"));
	if (ssid_num > MAX_NUMBER_OF_BSSID || ssid_num <= 0)
		ssid_num = MAX_NUMBER_OF_BSSID;

#ifndef CONFIG_KERNEL_NVRAM_SPLIT_INIC
	if (!inic) {
#endif
	    // WirelessMode -> need move per ssid to goahead
	    bzero(w_mode, sizeof(w_mode));
	    sprintf(w_mode, "%s", nvram_get(mode, "WirelessMode"));
	    for (i = 1; i < ssid_num; i++)
		sprintf(w_mode+strlen(w_mode), ";%s", nvram_get(mode, "WirelessMode"));
	    fprintf(fp, "WirelessMode=%s\n", w_mode);

	    FPRINT_NUM(RadioOn);
	    FPRINT_NUM(TxPower);
	    FPRINT_NUM(Channel);
	    FPRINT_NUM(AutoChannelSelect);
	    FPRINT_STR(AutoChannelSkipList);
	    FPRINT_NUM(ACSCheckTime);
	    FPRINT_NUM(BasicRate);
	    FPRINT_STR(SSID1);
#ifndef CONFIG_KERNEL_NVRAM_SPLIT_INIC
	} else {
	    // WirelessMode -> need move per ssid to goahead
	    bzero(w_mode, sizeof(w_mode));
	    sprintf(w_mode, "%s", nvram_get(mode, "WirelessModeINIC"));
	    for (i = 1; i < ssid_num; i++)
		sprintf(w_mode+strlen(w_mode), ";%s", nvram_get(mode, "WirelessModeINIC"));
	    fprintf(fp, "WirelessMode=%s\n", w_mode);

	    fprintf(fp, "RadioOn=%d\n", atoi(nvram_get(mode, "RadioOnINIC")));
	    fprintf(fp, "TxPower=%d\n", atoi(nvram_get(mode, "TxPowerINIC")));
	    fprintf(fp, "Channel=%d\n", atoi(nvram_get(mode, "ChannelINIC")));
	    fprintf(fp, "AutoChannelSelect=%d\n", atoi(nvram_get(mode, "AutoChannelSelectINIC")));
	    fprintf(fp, "AutoChannelSkipList=%d\n", atoi(nvram_get(mode, "AutoChannelSkipListINIC")));
	    fprintf(fp, "ACSCheckTime=%d\n", atoi(nvram_get(mode, "ACSCheckTimeINIC")));
	    fprintf(fp, "BasicRate=%d\n", atoi(nvram_get(mode, "BasicRateINIC")));
	    fprintf(fp, "SSID1=%s\n", nvram_get(mode, "SSID1INIC"));
	}
#endif
	// Stub
	fprintf(fp, "FixedTxMode=\n");

	// WmmCapable -> need move per ssid to goahead
	bzero(wmm_enable, sizeof(wmm_enable));
	sprintf(wmm_enable, "%s", nvram_get(mode, "WmmCapable"));
	for (i = 1; i < ssid_num; i++)
		sprintf(wmm_enable+strlen(wmm_enable), ";%s", nvram_get(mode, "WmmCapable"));
	fprintf(fp, "WmmCapable=%s\n", wmm_enable);

	FPRINT_NUM(CountryRegion);
	FPRINT_NUM(CountryRegionABand);
	FPRINT_STR(CountryCode);
	FPRINT_STR(RDRegion);
	FPRINT_NUM(BssidNum);
	FPRINT_STR(SSID2);
	FPRINT_STR(SSID3);
	FPRINT_STR(SSID4);
	FPRINT_STR(SSID5);
	FPRINT_STR(SSID6);
	FPRINT_STR(SSID7);
	FPRINT_STR(SSID8);

	FPRINT_NUM(BeaconPeriod);
	FPRINT_NUM(DtimPeriod);
	FPRINT_NUM(DisableOLBC);
	FPRINT_NUM(BGProtection);
	FPRINT_NUM(RTSThreshold);
	FPRINT_NUM(TxPreamble);
	FPRINT_NUM(TxPower);
	FPRINT_NUM(TxBurst);
	FPRINT_NUM(BurstMode);
	FPRINT_NUM(PktAggregate);
	FPRINT_NUM(FragThreshold);
	FPRINT_NUM(MaxSPLength);
	FPRINT_NUM(ShortSlot);
	FPRINT_NUM(NoForwardingBTNBSSID);
	FPRINT_NUM(session_timeout_interval);
	FPRINT_NUM(quiet_interval);
	FPRINT_NUM(IdleTimeout);

	FPRINT_STR(APSDCapable);
	FPRINT_STR(AckPolicy);
	FPRINT_STR(DLSCapable);
	FPRINT_STR(NoForwarding);
	FPRINT_STR(NoForwardingMBCast);
	FPRINT_STR(StationKeepAlive);
	FPRINT_STR(EntryLifeCheck);
	FPRINT_STR(HideSSID);
	FPRINT_STR(IEEE8021X);
	FPRINT_STR(IEEE80211H);
	FPRINT_STR(APAifsn);
	FPRINT_STR(APCwmin);
	FPRINT_STR(APCwmax);
	FPRINT_STR(APTxop);
	FPRINT_STR(APACM);
	FPRINT_STR(BSSAifsn);
	FPRINT_STR(BSSCwmin);
	FPRINT_STR(BSSCwmax);
	FPRINT_STR(BSSTxop);
	FPRINT_STR(BSSACM);
	FPRINT_STR(PreAuth);
	FPRINT_STR(AuthMode);
	FPRINT_STR(EncrypType);
    	FPRINT_STR(RekeyMethod);
	FPRINT_STR(RekeyInterval);
	FPRINT_STR(PMKCachePeriod);

	FPRINT_STR(WPAPSK1);
	FPRINT_STR(WPAPSK2);
	FPRINT_STR(WPAPSK3);
	FPRINT_STR(WPAPSK4);
	FPRINT_STR(WPAPSK5);
	FPRINT_STR(WPAPSK6);
	FPRINT_STR(WPAPSK7);
	FPRINT_STR(WPAPSK8);

	FPRINT_STR(DefaultKeyID);
	FPRINT_STR(Key1Type);
	FPRINT_STR(Key1Str1);
	FPRINT_STR(Key1Str2);
	FPRINT_STR(Key1Str3);
	FPRINT_STR(Key1Str4);
	FPRINT_STR(Key1Str5);
	FPRINT_STR(Key1Str6);
	FPRINT_STR(Key1Str7);
	FPRINT_STR(Key1Str8);
	FPRINT_STR(Key2Type);
	FPRINT_STR(Key2Str1);
	FPRINT_STR(Key2Str2);
	FPRINT_STR(Key2Str3);
	FPRINT_STR(Key2Str4);
	FPRINT_STR(Key2Str5);
	FPRINT_STR(Key2Str6);
	FPRINT_STR(Key2Str7);
	FPRINT_STR(Key2Str8);
	FPRINT_STR(Key3Type);
	FPRINT_STR(Key3Str1);
	FPRINT_STR(Key3Str2);
	FPRINT_STR(Key3Str3);
	FPRINT_STR(Key3Str4);
	FPRINT_STR(Key3Str5);
	FPRINT_STR(Key3Str6);
	FPRINT_STR(Key3Str7);
	FPRINT_STR(Key3Str8);
	FPRINT_STR(Key4Type);
	FPRINT_STR(Key4Str1);
	FPRINT_STR(Key4Str2);
	FPRINT_STR(Key4Str3);
	FPRINT_STR(Key4Str4);
	FPRINT_STR(Key4Str5);
	FPRINT_STR(Key4Str6);
	FPRINT_STR(Key4Str7);
	FPRINT_STR(Key4Str8);

	FPRINT_NUM(HT_HTC);
	FPRINT_NUM(HT_RDG);
	FPRINT_NUM(HT_OpMode);
	FPRINT_NUM(HT_MpduDensity);
	FPRINT_NUM(HT_BW);
	FPRINT_NUM(HT_AutoBA);
	FPRINT_NUM(HT_BADecline);
	FPRINT_NUM(HT_AMSDU);
	FPRINT_NUM(HT_BAWinSize);
	FPRINT_NUM(HT_GI);
	FPRINT_NUM(HT_STBC);
	FPRINT_NUM(HT_LDPC);
	FPRINT_STR(HT_MCS);
	FPRINT_NUM(HT_PROTECT);
	FPRINT_NUM(HT_DisallowTKIP);
	FPRINT_NUM(HT_40MHZ_INTOLERANT);
	FPRINT_NUM(HT_MIMOPSMode);

#ifndef CONFIG_KERNEL_NVRAM_SPLIT_INIC
	if (!inic) {
#endif
	    FPRINT_NUM(HT_TxStream);
	    FPRINT_NUM(HT_RxStream);
	    FPRINT_NUM(HT_EXTCHA);
#ifndef CONFIG_KERNEL_NVRAM_SPLIT_INIC
	} else {
	    fprintf(fp, "HT_EXTCHA=%d\n", atoi(nvram_get(mode, "HT_EXTCHAINIC")));
	    fprintf(fp, "HT_TxStream=%d\n", atoi(nvram_get(mode, "HT_TxStreamINIC")));
	    fprintf(fp, "HT_RxStream=%d\n", atoi(nvram_get(mode, "HT_RxStreamINIC")));
	}
#endif
#ifndef CONFIG_RT_SECOND_IF_NONE
	// VHT
	FPRINT_NUM(VHT_BW);
	FPRINT_NUM(VHT_BW_SIGNAL);
	FPRINT_NUM(VHT_DisallowNonVHT);
	FPRINT_NUM(VHT_LDPC);
	FPRINT_NUM(VHT_SGI);
	FPRINT_NUM(VHT_STBC);
#endif
        FPRINT_NUM(AccessPolicy0);
	FPRINT_STR(AccessControlList0);
	FPRINT_NUM(AccessPolicy1);
	FPRINT_STR(AccessControlList1);
	FPRINT_NUM(AccessPolicy2);
	FPRINT_STR(AccessControlList2);
	FPRINT_NUM(AccessPolicy3);
	FPRINT_STR(AccessControlList3);
	FPRINT_NUM(AccessPolicy4);
	FPRINT_STR(AccessControlList4);
	FPRINT_NUM(AccessPolicy5);
	FPRINT_STR(AccessControlList5);
	FPRINT_NUM(AccessPolicy6);
	FPRINT_STR(AccessControlList6);
	FPRINT_NUM(AccessPolicy7);
	FPRINT_STR(AccessControlList7);
	FPRINT_STR(RADIUS_Server);
	FPRINT_STR(RADIUS_Port);
	FPRINT_STR(RADIUS_Key);
	FPRINT_STR(RADIUS_Key1);
	FPRINT_STR(RADIUS_Key2);
	FPRINT_STR(RADIUS_Key3);
	FPRINT_STR(RADIUS_Key4);
	FPRINT_STR(RADIUS_Key5);
	FPRINT_STR(RADIUS_Key6);
	FPRINT_STR(RADIUS_Key7);
	FPRINT_STR(RADIUS_Key8);
	FPRINT_STR(own_ip_addr);
	FPRINT_STR(EAPifname);
	FPRINT_STR(PreAuthifname);
	FPRINT_STR(MaxStaNum);

#if defined(CONFIG_RT2860V2_STA) || defined(CONFIG_RT2860V2_STA_MODULE) || defined(CONFIG_MT76X2_STA) || defined(CONFIG_MT76X2_STA_MODULE)
	FPRINT_STR(PSMode);
	FPRINT_NUM(AutoConnect);
	FPRINT_NUM(FastConnect);
	FPRINT_NUM(AutoRoaming);
#endif
#if defined(CONFIG_RT2860V2_EXT_CHANNEL_LIST) || defined(CONFIG_MT7610_AP_EXT_CHANNEL_LIST) ||  defined(CONFIG_MT76X2_AP_EXT_CHANNEL_LIST)
	FPRINT_NUM(ChannelGeography);
#endif
#if defined(CONFIG_RT2860V2_AP_DBG) || defined(CONFIG_MT7610_AP_DBG) || defined(CONFIG_MT76X2_AP_DBG)
	FPRINT_NUM(WirelessEvent);
#endif
#if defined(CONFIG_RT2860V2_AP_VIDEO_TURBINE) || defined(CONFIG_MT7610_AP_VIDEO_TURBINE) || defined(CONFIG_MT76X2_AP_VIDEO_TURBINE)
	FPRINT_NUM(VideoTurbine);
	FPRINT_NUM(VideoClassifierEnable);
	FPRINT_NUM(VideoHighTxMode);
	FPRINT_NUM(VideoTxLifeTimeMode);
#endif
#if defined(CONFIG_RT2860V2_AP_IGMP_SNOOP) || defined(CONFIG_MT7610_AP_IGMP_SNOOP) || defined(CONFIG_MT76X2_AP_IGMP_SNOOP)
	FPRINT_NUM(M2UEnabled);
#endif
#if defined(CONFIG_RT2860V2_AP_MCAST_RATE_SPECIFIC) || defined(CONFIG_MT7610_AP_MCAST_RATE_SPECIFIC) || defined(CONFIG_MT76X2_AP_MCAST_RATE_SPECIFIC)
	FPRINT_NUM(McastPhyMode);
	FPRINT_NUM(McastMcs);
#endif
#if defined(CONFIG_RT2860V2_AP_CARRIER) || defined(CONFIG_MT7610_AP_CARRIER) || defined(CONFIG_MT76X2_AP_CARRIER)
	FPRINT_NUM(CarrierDetect);
#endif
#if defined(CONFIG_MT76X2_AP_TXBF_SUPPORT)
	FPRINT_NUM(ITxBfEn);
	FPRINT_NUM(ITxBfTimeout);
	FPRINT_NUM(ETxBfEnCond);
	FPRINT_NUM(ETxBfeeEn);
	FPRINT_NUM(ETxBfTimeout);
	FPRINT_NUM(ETxBfNoncompress);
	FPRINT_NUM(ETxBfIncapable);
#endif
#if defined(CONFIG_RT2860V2_AP_DFS) || defined(CONFIG_MT7610_AP_DFS) || defined(CONFIG_MT76X2_AP_DFS)
	FPRINT_NUM(CSPeriod);
	FPRINT_NUM(DfsIndoor);
	FPRINT_NUM(SymRoundFromCfg);
	FPRINT_NUM(BusyIdleFromCfg);
	FPRINT_NUM(DfsRssiHighFromCfg);
	FPRINT_NUM(DfsRssiLowFromCfg);
	FPRINT_NUM(DFSParamFromConfig);
	FPRINT_STR(FCCParamCh0);
	FPRINT_STR(FCCParamCh1);
	FPRINT_STR(FCCParamCh2);
	FPRINT_STR(FCCParamCh3);
	FPRINT_STR(CEParamCh0);
	FPRINT_STR(CEParamCh1);
	FPRINT_STR(CEParamCh2);
	FPRINT_STR(CEParamCh3);
	FPRINT_STR(JAPParamCh0);
	FPRINT_STR(JAPParamCh1);
	FPRINT_STR(JAPParamCh2);
	FPRINT_STR(JAPParamCh3);
	FPRINT_STR(JAPW53ParamCh0);
	FPRINT_STR(JAPW53ParamCh1);
	FPRINT_STR(JAPW53ParamCh2);
	FPRINT_STR(JAPW53ParamCh3);
#endif
#if defined(CONFIG_MT7610_AP_ED_MONITOR) ||  defined(CONFIG_MT76X2_AP_ED_MONITOR)
	FPRINT_NUM(EDCCA_ED_TH);
	FPRINT_NUM(ED_MODE);
	FPRINT_NUM(EDCCA_FALSE_CCA_TH);
	FPRINT_NUM(ED_LEARN_TH);
	FPRINT_NUM(EDCCA_BLOCK_CHECK_TH);
#endif
#if defined(CONFIG_RT2860V2_AP_GREENAP) || defined(CONFIG_MT76X2_AP_GREENAP)
	FPRINT_NUM(GreenAP);
#endif
#if defined(CONFIG_RT2860V2_AP_80211N_DRAFT3) || defined(CONFIG_MT7610_AP_80211N_DRAFT3) || defined(CONFIG_MT76X2_AP_80211N_DRAFT3)
	FPRINT_NUM(HT_BSSCoexistence);
	FPRINT_NUM(HT_BSSCoexApCntThr);
#endif
#if defined(CONFIG_RT2860V2_AP_WDS) || defined(CONFIG_MT7610_AP_WDS) || defined(CONFIG_MT76X2_AP_WDS)
	FPRINT_NUM(WdsEnable);
	FPRINT_STR(WdsPhyMode);
	FPRINT_STR(WdsTxMcs);
	FPRINT_STR(WdsEncrypType);
	FPRINT_STR(WdsList);
	FPRINT_STR(Wds0Key);
	FPRINT_STR(Wds1Key);
	FPRINT_STR(Wds2Key);
	FPRINT_STR(Wds3Key);
#endif
#if defined(CONFIG_RT2860V2_AP_APCLI) || defined(CONFIG_MT7610_AP_APCLI) || defined(CONFIG_MT76X2_AP_APCLI)
	FPRINT_NUM(ApCliEnable);
	FPRINT_STR(ApCliSsid);
	FPRINT_STR(ApCliBssid);
	FPRINT_STR(ApCliAuthMode);
	FPRINT_STR(ApCliEncrypType);
	FPRINT_STR(ApCliDefaultKeyID);
	FPRINT_STR(ApCliWPAPSK);
	FPRINT_STR(ApCliTxMode);
	FPRINT_STR(ApCliTxMcs);
	FPRINT_STR(ApCliAPSDCapable);
#endif
#if defined(CONFIG_RT2860V2_AP_IDS) || defined(CONFIG_MT7610_AP_IDS) || defined(CONFIG_MT76X2_AP_IDS)
	FPRINT_NUM(IdsEnable);
	FPRINT_NUM(AuthFloodThreshold);
	FPRINT_NUM(AssocReqFloodThreshold);
	FPRINT_NUM(ReassocReqFloodThreshold);
	FPRINT_NUM(ProbeReqFloodThreshold);
	FPRINT_NUM(DisassocFloodThreshold);
	FPRINT_NUM(DeauthFloodThreshold);
	FPRINT_NUM(EapReqFloodThreshold);
#endif
	/* Basic Roaming, need add in profile instead of iwpriv usage in future */
	FPRINT_NUM(ApProbeRspTimes);
	FPRINT_NUM(AuthRspFail);
	FPRINT_NUM(AuthRspRssi);
	FPRINT_NUM(AssocReqRssiThres);
	FPRINT_NUM(AssocRspIgnor);
	FPRINT_NUM(KickStaRssiLow);
	FPRINT_NUM(KickStaRssiLowDelay);
	FPRINT_NUM(ProbeRspRssi);
#ifdef CONFIG_MT76X2_AP_DOT11R_FT_SUPPORT
	/* Fast roaming auth transitions config */
	FPRINT_STR(FtSupport);  /* Enable Fast BSS Transition */
	FPRINT_STR(FtRic);	/* Enable FT resource request */
	FPRINT_STR(FtOtd);	/* Support Over-the-DS Fast BSS Transition (over LAN/WDS, need iappd daemon? Default suppoty Oveк-the-Air only) */
				/* Mobility domain ID of Fast Bss. */
	FPRINT_STR(FtMdId1);
	FPRINT_STR(FtMdId2);
	FPRINT_STR(FtMdId3);
	FPRINT_STR(FtMdId4);
				/* R0 Key Handler Identification. */
	FPRINT_STR(FtR0khId1);
	FPRINT_STR(FtR0khId2);
	FPRINT_STR(FtR0khId3);
	FPRINT_STR(FtR0khId4);
#endif
#ifdef CONFIG_MT76X2_AP_DOT11K_RRM_SUPPORT
	FPRINT_STR(RRMEnable);  	/* Enable Resource Radio Managment */
#ifndef CONFIG_KERNEL_NVRAM_SPLIT_INIC
	if (!inic) {
	    FPRINT_STR(RegulatoryClass);
	} else {
	    fprintf(fp, "RegulatoryClass=%d\n", atoi(nvram_get(mode, "RegulatoryClassINIC")));
	}
#else
	FPRINT_STR(RegulatoryClass);
#endif
#endif
#ifdef CONFIG_BAND_STEERING
	FPRINT_NUM(BandSteering);
#endif
	fclose(fp);
	nvram_close(mode);
	sync();
	return 0;
}

void usage(char *cmd)
{
	printf("Usage:\n");
	printf("nvram_<command> [<platform>] [<file>]\n\n");
	printf("command:\n");
	printf("  get		- get value from nvram for <platform>\n");
	printf("  buf_get	- buffered get value from nvram for <platform>\n");
	printf("  set		- set value to nvram for <platform>\n");
	printf("  show		- display values in nvram for <platform>\n");
	printf("  renew		- replace nvram values for <platform> with <file>\n");
	printf("  clear		- clear all entries in nvram for <platform>\n");
	printf("  default	- load default for <platform>\n");
	printf("  genwlconfig	- generate config file from nvram for <platform>\n");
	printf("platform:\n");
	printf("  2860    - first module\n");
#ifndef CONFIG_RT_SECOND_IF_NONE
	printf("  rtdev    - second module\n");
#endif
	printf("file:\n");
	printf("          - file name for renew command\n");
	exit(0);
}

int main(int argc, char *argv[])
{
	char *cmd = "";
	int mode = RT2860_NVRAM, genmode = RT2860_NVRAM;

	if (argc < 2)
		usage(argv[0]);

	//call nvram_get, nvram_set or nvram_show
	if (cmd == strrchr(argv[0], '/'))
		cmd++;
	else
		cmd = argv[0];

	if (!strncmp(argv[1], "rtdev", 5)) {
#if !defined(CONFIG_RT_SECOND_IF_NONE) && defined(CONFIG_KERNEL_NVRAM_SPLIT_INIC)
	    mode = RTINIC_NVRAM;
#endif
	    genmode = RTINIC_NVRAM;
	}

	if (!strncmp(cmd, "nvram_get", 10))
		return ra_nv_get(argc, argv);
	else if (!strncmp(cmd, "nvram_buf_get", 14))
		return ra_nv_buf_get(argc, argv);
	else if (!strncmp(cmd, "nvram_set", 10))
		return ra_nv_set(argc, argv);
	else if (!strncmp(cmd, "nvram_show", 11))
		return nvram_show(mode);
	else if (!strncmp(cmd, "nvram_clear", 12))
		return nvram_clear(mode);
	else if (!strncmp(cmd, "nvram_genwlconfig", 18))
		return gen_wifi_config(mode, genmode);
	else if (!strncmp(cmd, "nvram_renew", 12)) {
		if (argc >= 3)
		    return nvram_renew(mode, argv[2]);
	} else if (!strncmp(cmd, "nvram_default", 14))
		return nvram_load_default();
	else
	    usage(argv[0]);

	return 0;
}
