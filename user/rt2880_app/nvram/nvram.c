#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <linux/autoconf.h>


#include "oid.h"	/* for max bssid num */
#include "libnvram.h"
#include "libnvram_utils.h"

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

static int nvram_load_default(void)
{
	/* default macs is OK */
	int mac_err = 0;
        char *LAN_MAC_ADDR = NULL, *WAN_MAC_ADDR = NULL, *WLAN_MAC_ADDR = NULL, *WLAN2_MAC_ADDR = NULL, *CHECKMAC = NULL;
        char  *MngmtLogin = NULL, *MngmtPassword = NULL, *MngmtStoreSettings = NULL, *cwmpdEnabled = NULL;

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

#define FPRINT_DAT(x) fprintf(fp, #x"=%s\n", nvram_get(mode, #x));

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

	    FPRINT_DAT(RadioOn);
	    FPRINT_DAT(TxPower);
	    FPRINT_DAT(Channel);
	    FPRINT_DAT(AutoChannelSelect);
	    FPRINT_DAT(AutoChannelSkipList);
	    FPRINT_DAT(ACSCheckTime);
	    FPRINT_DAT(BasicRate);
	    FPRINT_DAT(SSID1);
#ifndef CONFIG_KERNEL_NVRAM_SPLIT_INIC
	} else {
	    // WirelessMode -> need move per ssid to goahead
	    bzero(w_mode, sizeof(w_mode));
	    sprintf(w_mode, "%s", nvram_get(mode, "WirelessModeINIC"));
	    for (i = 1; i < ssid_num; i++)
		sprintf(w_mode+strlen(w_mode), ";%s", nvram_get(mode, "WirelessModeINIC"));
	    fprintf(fp, "WirelessMode=%s\n", w_mode);

	    fprintf(fp, "RadioOn=%s\n", nvram_get(mode, "RadioOnINIC"));
	    fprintf(fp, "TxPower=%s\n", nvram_get(mode, "TxPowerINIC"));
	    fprintf(fp, "Channel=%s\n", nvram_get(mode, "ChannelINIC"));
	    fprintf(fp, "AutoChannelSelect=%s\n", nvram_get(mode, "AutoChannelSelectINIC"));
	    fprintf(fp, "AutoChannelSkipList=%s\n", nvram_get(mode, "AutoChannelSkipListINIC"));
	    fprintf(fp, "ACSCheckTime=%s\n", nvram_get(mode, "ACSCheckTimeINIC"));
	    fprintf(fp, "BasicRate=%s\n", nvram_get(mode, "BasicRateINIC"));
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

	FPRINT_DAT(CountryRegion);
	FPRINT_DAT(CountryRegionABand);
	FPRINT_DAT(CountryCode);
	FPRINT_DAT(RDRegion);
	FPRINT_DAT(BssidNum);
	FPRINT_DAT(SSID2);
	FPRINT_DAT(SSID3);
	FPRINT_DAT(SSID4);
	FPRINT_DAT(SSID5);
	FPRINT_DAT(SSID6);
	FPRINT_DAT(SSID7);
	FPRINT_DAT(SSID8);

	FPRINT_DAT(BeaconPeriod);
	FPRINT_DAT(DtimPeriod);
	FPRINT_DAT(DisableOLBC);
	FPRINT_DAT(BGProtection);
	FPRINT_DAT(RTSThreshold);
	FPRINT_DAT(TxPreamble);
	FPRINT_DAT(TxPower);
	FPRINT_DAT(TxBurst);
	FPRINT_DAT(BurstMode);
	FPRINT_DAT(PktAggregate);
	FPRINT_DAT(FragThreshold);
	FPRINT_DAT(MaxSPLength);
	FPRINT_DAT(ShortSlot);
	FPRINT_DAT(NoForwardingBTNBSSID);
	FPRINT_DAT(session_timeout_interval);
	FPRINT_DAT(quiet_interval);
	FPRINT_DAT(IdleTimeout);

	FPRINT_DAT(APSDCapable);
	FPRINT_DAT(AckPolicy);
	FPRINT_DAT(DLSCapable);
	FPRINT_DAT(NoForwarding);
	FPRINT_DAT(NoForwardingMBCast);
	FPRINT_DAT(StationKeepAlive);
	FPRINT_DAT(EntryLifeCheck);
	FPRINT_DAT(HideSSID);
	FPRINT_DAT(IEEE80211H);
	FPRINT_DAT(APAifsn);
	FPRINT_DAT(APCwmin);
	FPRINT_DAT(APCwmax);
	FPRINT_DAT(APTxop);
	FPRINT_DAT(APACM);
	FPRINT_DAT(BSSAifsn);
	FPRINT_DAT(BSSCwmin);
	FPRINT_DAT(BSSCwmax);
	FPRINT_DAT(BSSTxop);
	FPRINT_DAT(BSSACM);
	FPRINT_DAT(AuthMode);
	FPRINT_DAT(EncrypType);
    	FPRINT_DAT(RekeyMethod);
	FPRINT_DAT(RekeyInterval);

	FPRINT_DAT(WPAPSK1);
	FPRINT_DAT(WPAPSK2);
	FPRINT_DAT(WPAPSK3);
	FPRINT_DAT(WPAPSK4);
	FPRINT_DAT(WPAPSK5);
	FPRINT_DAT(WPAPSK6);
	FPRINT_DAT(WPAPSK7);
	FPRINT_DAT(WPAPSK8);

	FPRINT_DAT(DefaultKeyID);
	FPRINT_DAT(Key1Type);
	FPRINT_DAT(Key1Str1);
	FPRINT_DAT(Key1Str2);
	FPRINT_DAT(Key1Str3);
	FPRINT_DAT(Key1Str4);
	FPRINT_DAT(Key1Str5);
	FPRINT_DAT(Key1Str6);
	FPRINT_DAT(Key1Str7);
	FPRINT_DAT(Key1Str8);
	FPRINT_DAT(Key2Type);
	FPRINT_DAT(Key2Str1);
	FPRINT_DAT(Key2Str2);
	FPRINT_DAT(Key2Str3);
	FPRINT_DAT(Key2Str4);
	FPRINT_DAT(Key2Str5);
	FPRINT_DAT(Key2Str6);
	FPRINT_DAT(Key2Str7);
	FPRINT_DAT(Key2Str8);
	FPRINT_DAT(Key3Type);
	FPRINT_DAT(Key3Str1);
	FPRINT_DAT(Key3Str2);
	FPRINT_DAT(Key3Str3);
	FPRINT_DAT(Key3Str4);
	FPRINT_DAT(Key3Str5);
	FPRINT_DAT(Key3Str6);
	FPRINT_DAT(Key3Str7);
	FPRINT_DAT(Key3Str8);
	FPRINT_DAT(Key4Type);
	FPRINT_DAT(Key4Str1);
	FPRINT_DAT(Key4Str2);
	FPRINT_DAT(Key4Str3);
	FPRINT_DAT(Key4Str4);
	FPRINT_DAT(Key4Str5);
	FPRINT_DAT(Key4Str6);
	FPRINT_DAT(Key4Str7);
	FPRINT_DAT(Key4Str8);

	FPRINT_DAT(HT_HTC);
	FPRINT_DAT(HT_RDG);
	FPRINT_DAT(HT_OpMode);
	FPRINT_DAT(HT_MpduDensity);
	FPRINT_DAT(HT_BW);
	FPRINT_DAT(HT_AutoBA);
	FPRINT_DAT(HT_BADecline);
	FPRINT_DAT(HT_AMSDU);
	FPRINT_DAT(HT_BAWinSize);
	FPRINT_DAT(HT_GI);
	FPRINT_DAT(HT_STBC);
	FPRINT_DAT(HT_LDPC);
	FPRINT_DAT(HT_MCS);
	FPRINT_DAT(HT_PROTECT);
	FPRINT_DAT(HT_DisallowTKIP);
	FPRINT_DAT(HT_40MHZ_INTOLERANT);
	FPRINT_DAT(HT_MIMOPSMode);

#ifndef CONFIG_KERNEL_NVRAM_SPLIT_INIC
	if (!inic) {
#endif
	    FPRINT_DAT(HT_TxStream);
	    FPRINT_DAT(HT_RxStream);
	    FPRINT_DAT(HT_EXTCHA);
#ifndef CONFIG_KERNEL_NVRAM_SPLIT_INIC
	} else {
	    fprintf(fp, "HT_EXTCHA=%s\n", nvram_get(mode, "HT_EXTCHAINIC"));
	    fprintf(fp, "HT_TxStream=%s\n", nvram_get(mode, "HT_TxStreamINIC"));
	    fprintf(fp, "HT_RxStream=%s\n", nvram_get(mode, "HT_RxStreamINIC"));
	}
#endif
#ifndef CONFIG_RT_SECOND_IF_NONE
	// VHT
	FPRINT_DAT(VHT_BW);
	FPRINT_DAT(VHT_BW_SIGNAL);
	FPRINT_DAT(VHT_DisallowNonVHT);
	FPRINT_DAT(VHT_LDPC);
	FPRINT_DAT(VHT_SGI);
	FPRINT_DAT(VHT_STBC);
#endif
	FPRINT_DAT(AccessPolicy0);
	FPRINT_DAT(AccessControlList0);
	FPRINT_DAT(AccessPolicy1);
	FPRINT_DAT(AccessControlList1);
	FPRINT_DAT(AccessPolicy2);
	FPRINT_DAT(AccessControlList2);
	FPRINT_DAT(AccessPolicy3);
	FPRINT_DAT(AccessControlList3);
	FPRINT_DAT(AccessPolicy4);
	FPRINT_DAT(AccessControlList4);
	FPRINT_DAT(AccessPolicy5);
	FPRINT_DAT(AccessControlList5);
	FPRINT_DAT(AccessPolicy6);
	FPRINT_DAT(AccessControlList6);
	FPRINT_DAT(AccessPolicy7);
	FPRINT_DAT(AccessControlList7);
	FPRINT_DAT(RADIUS_Key1);
	FPRINT_DAT(RADIUS_Key2);
	FPRINT_DAT(RADIUS_Key3);
	FPRINT_DAT(RADIUS_Key4);
	FPRINT_DAT(RADIUS_Key5);
	FPRINT_DAT(RADIUS_Key6);
	FPRINT_DAT(RADIUS_Key7);
	FPRINT_DAT(RADIUS_Key8);
	FPRINT_DAT(MaxStaNum);

#if defined(CONFIG_RT2860V2_STA) || defined(CONFIG_RT2860V2_STA_MODULE) || defined(CONFIG_MT76X2_STA) || defined(CONFIG_MT76X2_STA_MODULE) || defined(CONFIG_MT76X3_STA) || defined(CONFIG_MT76X3_STA_MODULE)
	FPRINT_DAT(PSMode);
	FPRINT_DAT(AutoConnect);
	FPRINT_DAT(FastConnect);
	FPRINT_DAT(AutoRoaming);
#endif
#if defined(CONFIG_RT2860V2_EXT_CHANNEL_LIST) || defined(CONFIG_MT7610_AP_EXT_CHANNEL_LIST) || defined(CONFIG_MT76X2_AP_EXT_CHANNEL_LIST) || defined(CONFIG_MT76X3_AP_EXT_CHANNEL_LIST)
	FPRINT_DAT(ChannelGeography);
#endif
#if defined(CONFIG_RT2860V2_AP_DBG) || defined(CONFIG_MT7610_AP_DBG) || defined(CONFIG_MT76X2_AP_DBG) || defined(CONFIG_MT76X3_AP_DBG)
	FPRINT_DAT(WirelessEvent);
#endif
#if defined(CONFIG_RT2860V2_AP_VIDEO_TURBINE) || defined(CONFIG_MT7610_AP_VIDEO_TURBINE) || defined(CONFIG_MT76X2_AP_VIDEO_TURBINE) || defined(CONFIG_MT76X3_AP_VIDEO_TURBINE)
	FPRINT_DAT(VideoTurbine);
	FPRINT_DAT(VideoClassifierEnable);
	FPRINT_DAT(VideoHighTxMode);
	FPRINT_DAT(VideoTxLifeTimeMode);
#endif
#if defined(CONFIG_RT2860V2_AP_IGMP_SNOOP) || defined(CONFIG_MT7610_AP_IGMP_SNOOP) || defined(CONFIG_MT76X2_AP_IGMP_SNOOP) || defined(CONFIG_MT76X3_AP_IGMP_SNOOP)
	FPRINT_DAT(M2UEnabled);
#endif
#if defined(CONFIG_RT2860V2_AP_MCAST_RATE_SPECIFIC) || defined(CONFIG_MT7610_AP_MCAST_RATE_SPECIFIC) || defined(CONFIG_MT76X2_AP_MCAST_RATE_SPECIFIC) || defined(CONFIG_MT76X3_AP_MCAST_RATE_SPECIFIC)
	FPRINT_DAT(McastPhyMode);
	FPRINT_DAT(McastMcs);
#endif
#if defined(CONFIG_RT2860V2_AP_CARRIER) || defined(CONFIG_MT7610_AP_CARRIER) || defined(CONFIG_MT76X2_AP_CARRIER) || defined(CONFIG_MT76X3_AP_CARRIER)
	FPRINT_DAT(CarrierDetect);
#endif
#if defined(CONFIG_MT76X2_AP_TXBF_SUPPORT)
	FPRINT_DAT(ITxBfEn);
	FPRINT_DAT(ITxBfTimeout);
	FPRINT_DAT(ETxBfEnCond);
	FPRINT_DAT(ETxBfeeEn);
	FPRINT_DAT(ETxBfTimeout);
	FPRINT_DAT(ETxBfNoncompress);
	FPRINT_DAT(ETxBfIncapable);
#endif
#if defined(CONFIG_RT2860V2_AP_DFS) || defined(CONFIG_MT7610_AP_DFS) || defined(CONFIG_MT76X2_AP_DFS)
	FPRINT_DAT(CSPeriod);
	FPRINT_DAT(DfsIndoor);
	FPRINT_DAT(SymRoundFromCfg);
	FPRINT_DAT(BusyIdleFromCfg);
	FPRINT_DAT(DfsRssiHighFromCfg);
	FPRINT_DAT(DfsRssiLowFromCfg);
	FPRINT_DAT(DFSParamFromConfig);
	FPRINT_DAT(FCCParamCh0);
	FPRINT_DAT(FCCParamCh1);
	FPRINT_DAT(FCCParamCh2);
	FPRINT_DAT(FCCParamCh3);
	FPRINT_DAT(CEParamCh0);
	FPRINT_DAT(CEParamCh1);
	FPRINT_DAT(CEParamCh2);
	FPRINT_DAT(CEParamCh3);
	FPRINT_DAT(JAPParamCh0);
	FPRINT_DAT(JAPParamCh1);
	FPRINT_DAT(JAPParamCh2);
	FPRINT_DAT(JAPParamCh3);
	FPRINT_DAT(JAPW53ParamCh0);
	FPRINT_DAT(JAPW53ParamCh1);
	FPRINT_DAT(JAPW53ParamCh2);
	FPRINT_DAT(JAPW53ParamCh3);
#endif
#if defined(CONFIG_MT7610_AP_ED_MONITOR) || defined(CONFIG_MT76X2_AP_ED_MONITOR) || defined(CONFIG_MT76X3_AP_ED_MONITOR)
	FPRINT_DAT(EDCCA_ED_TH);
	FPRINT_DAT(ED_MODE);
	FPRINT_DAT(EDCCA_FALSE_CCA_TH);
	FPRINT_DAT(ED_LEARN_TH);
	FPRINT_DAT(EDCCA_BLOCK_CHECK_TH);
#endif
#if defined(CONFIG_RT2860V2_AP_GREENAP) || defined(CONFIG_MT76X2_AP_GREENAP) || defined(CONFIG_MT76X3_AP_GREENAP)
	FPRINT_DAT(GreenAP);
#endif
#if defined(CONFIG_RT2860V2_AP_80211N_DRAFT3) || defined(CONFIG_MT7610_AP_80211N_DRAFT3) || defined(CONFIG_MT76X2_AP_80211N_DRAFT3) || defined(CONFIG_MT76X3_AP_80211N_DRAFT3)
	FPRINT_DAT(HT_BSSCoexistence);
	FPRINT_DAT(HT_BSSCoexApCntThr);
#endif
#if defined(CONFIG_RT2860V2_AP_WDS) || defined(CONFIG_MT7610_AP_WDS) || defined(CONFIG_MT76X2_AP_WDS) || defined(CONFIG_MT76X3_AP_WDS)
	FPRINT_DAT(WdsEnable);
	FPRINT_DAT(WdsPhyMode);
	FPRINT_DAT(WdsTxMcs);
	FPRINT_DAT(WdsEncrypType);
	FPRINT_DAT(WdsList);
	FPRINT_DAT(Wds0Key);
	FPRINT_DAT(Wds1Key);
	FPRINT_DAT(Wds2Key);
	FPRINT_DAT(Wds3Key);
#endif
#if defined(CONFIG_RT2860V2_AP_APCLI) || defined(CONFIG_MT7610_AP_APCLI) || defined(CONFIG_MT76X2_AP_APCLI) || defined(CONFIG_MT76X3_AP_APCLI)
	FPRINT_DAT(ApCliEnable);
	FPRINT_DAT(ApCliSsid);
	FPRINT_DAT(ApCliBssid);
	FPRINT_DAT(ApCliAuthMode);
	FPRINT_DAT(ApCliEncrypType);
	FPRINT_DAT(ApCliDefaultKeyID);
	FPRINT_DAT(ApCliWPAPSK);
	FPRINT_DAT(ApCliTxMode);
	FPRINT_DAT(ApCliTxMcs);
	FPRINT_DAT(ApCliAPSDCapable);
#endif
#if defined(CONFIG_RT2860V2_AP_IDS) || defined(CONFIG_MT7610_AP_IDS) || defined(CONFIG_MT76X2_AP_IDS) || defined(CONFIG_MT76X3_AP_IDS)
	FPRINT_DAT(IdsEnable);
	FPRINT_DAT(AuthFloodThreshold);
	FPRINT_DAT(AssocReqFloodThreshold);
	FPRINT_DAT(ReassocReqFloodThreshold);
	FPRINT_DAT(ProbeReqFloodThreshold);
	FPRINT_DAT(DisassocFloodThreshold);
	FPRINT_DAT(DeauthFloodThreshold);
	FPRINT_DAT(EapReqFloodThreshold);
#endif
	/* 802.11x support */
	FPRINT_DAT(IEEE8021X);
	FPRINT_DAT(PreAuth);
	FPRINT_DAT(PMKCachePeriod);
	FPRINT_DAT(own_ip_addr);
	FPRINT_DAT(EAPifname);
	FPRINT_DAT(PreAuthifname);
	FPRINT_DAT(RADIUS_Server);
	FPRINT_DAT(RADIUS_Port);
	FPRINT_DAT(RADIUS_Key);

	/* LNA Control, not apply from read dat file now, need add in profile instead of iwpriv usage in future */
	FPRINT_DAT(DyncVgaEnable);	/* enable/disable dynamic LNA gain */
	FPRINT_DAT(SkipLongRangeVga);	/* skip tune gain for long distanse clients */
	FPRINT_DAT(VgaClamp);		/* limit dynamic maximum gain to reduce impact interference (1 - -4dB, 2 - -8dB, 3 - -12dB, 4 -16dB) */
#ifdef CONFIG_BAND_STEERING
	FPRINT_DAT(BandSteering);
	/* Band Steering, not apply from read dat file now, need add in profile instead of iwpriv usage in future */
	FPRINT_DAT(BndStrgRssiDiff);	/* if Rssi2.4G > Rssi5G by RssiDiff, then allow client to connect 2.4G */
	FPRINT_DAT(BndStrgRssiLow);	/* if Rssi5G < RssiLow, then this client cannot connect to 5G */
	FPRINT_DAT(BndStrgAge);		/* Entry Age Time (ms) */
	FPRINT_DAT(BndStrgHoldTime);	/* Time for holding 2.4G connection rsp (ms) */
	FPRINT_DAT(BndStrgCheckTime);	/* Time for deciding if a client is 2.4G only (ms) */
#endif
	/* Basic Roaming, need add in profile instead of iwpriv usage in future
	 *
	 * ApProbeRspTimes       - range 0 - 10 times, limit probe reqest per client, default 3
	 * AuthRspFail           - range 0 - -100 dBm, reject auth req due to weak signal, default 0 (off)
	 * AuthRspRssi           - range 0 - -100 dBm, ignore auth req due to weak signal, default 0 (off)
	 * AssocReqRssiThres     - range 0 - -100 dBm, reject assoc req due to weak signal, default 0 (off)
	 * AssocRspIgnor         - range 0 - -100 dBm, ignore assoc req due to weak signal, default 0 (off)
	 * ProbeRspRssi          - range 0 - -100 dBm, auto disonnect sta if rssi low at probe requests, default 0 (off)
	 * KickStaRssiLow        - range 0 - -100 dBm, auto disonnect sta if rssi low (active clients), default 0 (off)
	 * KickStaRssiLow        - range 0 - -100 dBm, auto disonnect sta if rssi low (PSM clients), default 0 (off)
	 * KickStaRssiLowDelay   - range 0 -  200 seconds, if in this interval all data frames have low rssi - kick STA, default 5
	 *
	*/
	FPRINT_DAT(ApProbeRspTimes);
	FPRINT_DAT(AuthRspFail);
	FPRINT_DAT(AuthRspRssi);
	FPRINT_DAT(AssocReqRssiThres);
	FPRINT_DAT(AssocRspIgnor);
	FPRINT_DAT(KickStaRssiLow);
	FPRINT_DAT(KickStaRssiLowPSM);
	FPRINT_DAT(KickStaRssiLowDelay);
	FPRINT_DAT(ProbeRspRssi);
#if defined(CONFIG_MT7610_AP_DOT11R_FT_SUPPORT) || defined(CONFIG_MT76X2_AP_DOT11R_FT_SUPPORT) || defined(CONFIG_MT76X2_AP_DOT11R_FT_SUPPORT)
	/* Fast roaming auth transitions config */
	FPRINT_DAT(FtSupport);  	/* Enable Fast BSS Transition */
	FPRINT_DAT(FtRic);		/* Enable FT resource request */
	FPRINT_DAT(FtOtd);		/* Support Over-the-DS Fast BSS Transition (over LAN/WDS, need iappd daemon? Default suppoty Over-the-Air only) */
					/* Mobility domain ID of Fast Bss. */
	FPRINT_DAT(FtMdId1);
	FPRINT_DAT(FtMdId2);
	FPRINT_DAT(FtMdId3);
	FPRINT_DAT(FtMdId4);
					/* R(N) Key Handler Identification. */
	FPRINT_DAT(FtR0khId1);
	FPRINT_DAT(FtR0khId2);
	FPRINT_DAT(FtR0khId3);
	FPRINT_DAT(FtR0khId4);
#endif
#if  defined(CONFIG_MT7610_AP_DOT11K_RRM_SUPPORT) || defined(CONFIG_MT76X2_AP_DOT11K_RRM_SUPPORT) || defined(CONFIG_MT76X3_AP_DOT11K_RRM_SUPPORT)
	FPRINT_DAT(RRMEnable);  	/* Enable Resource Radio Managment */
#ifndef CONFIG_KERNEL_NVRAM_SPLIT_INIC
	if (!inic) {
	    FPRINT_DAT(RegulatoryClass);
	} else {
	    fprintf(fp, "RegulatoryClass=%s\n", nvram_get(mode, "RegulatoryClassINIC"));
	}
#else
	FPRINT_DAT(RegulatoryClass);
#endif
#endif /* END RRM */
#ifdef CONFIG_MT76X3_AP_SMART_CARRIER_SENSE
	FPRINT_DAT(SCSEnable);
#endif
	fclose(fp);
	nvram_close(mode);
	sync();
	return 0;
}

static int gen_staticroutes(int mode, int genmode)
{
	static_routing_rebuild_etc();

	return 0;
}

static int gen_iptablesrules(int mode, int genmode)
{
	firewall_rebuild_etc();

	return 0;
}

void usage(char *cmd)
{
	printf("Usage:\n");
	printf("nvram_<command> [<platform>] [<file>]\n\n");
	printf("command:\n");
	printf("  get              - get value from nvram for <platform>\n");
	printf("  buf_get          - buffered get value from nvram for <platform>\n");
	printf("  set              - set value to nvram for <platform>\n");
	printf("  show             - display values in nvram for <platform>\n");
	printf("  renew            - replace nvram values for <platform> with <file>\n");
	printf("  clear            - clear all entries in nvram for <platform>\n");
	printf("  default          - load default for <platform>\n");
	printf("  genwlconfig      - generate config file from nvram for <platform>\n");
	printf("  genstaticroutes  - generate static routes scripts from nvram for <platform>\n");
	printf("  geniptablesrules - generate iptables rules from nvram for <platform>\n");
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
	else if (!strncmp(cmd, "nvram_genstaticroutes", 22))
		return gen_staticroutes(mode, genmode);
	else if (!strncmp(cmd, "nvram_geniptablesrules", 23))
		return gen_iptablesrules(mode, genmode);
	else if (!strncmp(cmd, "nvram_renew", 12)) {
		if (argc >= 3)
		    return nvram_renew(mode, argv[2]);
	} else if (!strncmp(cmd, "nvram_default", 14))
		return nvram_load_default();
	else
	    usage(argv[0]);

	return 0;
}
