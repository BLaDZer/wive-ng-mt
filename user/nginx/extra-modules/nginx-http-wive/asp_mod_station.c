#include	"asp_mod_station.h"

#if defined(CONFIG_MT76X2_STA) || defined(CONFIG_MT76X2_STA_MODULE) || defined(CONFIG_MT76X3_STA) || defined(CONFIG_MT76X3_STA_MODULE)
#define HAVE_MT76X2_3
#endif

PAIR_CHANNEL_FREQ_ENTRY ChannelFreqTable[] = {
			{1, 2412000},
			{2, 2417000},
			{3, 2422000},
			{4, 2427000},
			{5, 2432000},
			{6, 2437000},
			{7, 2442000},
			{8, 2447000},
			{9, 2452000},
			{10, 2457000},
			{11, 2462000},
			{12, 2467000},
			{13, 2472000},
			{14, 2484000},

			/*  UNII */
			{36, 5180000},
			{40, 5200000},
			{44, 5220000},
			{48, 5240000},
			{50, 5250000},
			{52, 5260000},
			{54, 5270000},
			{56, 5280000},
			{58, 5290000},
			{60, 5300000},
			{62, 5310000},
			{64, 5320000},
			{149, 5745000},
			{151, 5755000},
			{153, 5765000},
			{155, 5775000},
			{157, 5785000},
			{159, 5795000},
			{161, 5805000},
			{165, 5825000},
			{167, 5835000},
			{169, 5845000},
			{171, 5855000},
			{173, 5865000},

			/* HiperLAN2 */
			{100, 5500000},
			{102, 5510000},
			{104, 5520000},
			{106, 5530000},
			{108, 5540000},
			{110, 5550000},
			{112, 5560000},
			{114, 5570000},
			{116, 5580000},
			{118, 5590000},
			{120, 5600000},
			{122, 5610000},
			{124, 5620000},
			{126, 5630000},
			{128, 5640000},
			{132, 5660000},
			{134, 5670000},
			{136, 5680000},
			{138, 5690000},
			{140, 5700000},
			{142, 5710000},
			{144, 5720000},

			/* Japan MMAC */
			{34, 5170000},
			{38, 5190000},
			{42, 5210000},
			{46, 5230000},

			/*  Japan */
			{184, 4920000},
			{188, 4940000},
			{192, 4960000},
			{196, 4980000},

			{208, 5040000},	/* Japan, means J08 */
			{212, 5060000},	/* Japan, means J12 */
			{216, 5080000},	/* Japan, means J16 */
};


PRT_PROFILE_SETTING headerProfileSetting = NULL; //selectedProfileSetting = NULL, headerProfileSetting = NULL, currentProfileSetting = NULL;

unsigned char   Active_flag=0, nConfig_flag=0;
unsigned int    m_nSigQua[3] = {0,0,0};
unsigned long   m_lTxCount = 0;
unsigned long   m_lRxCount = 0;
unsigned long   m_lChannelQuality = 0;
char    G_bRadio = 1; //TRUE
char    G_bdBm_ischeck = 0; //false
char    G_staProfileNum = 0;
NDIS_802_11_SSID        G_SSID;
unsigned char			G_Bssid[6];
int        G_ConnectStatus = NdisMediaStateDisconnected;

int G_nChanFreqCount = sizeof (ChannelFreqTable) / sizeof(PAIR_CHANNEL_FREQ_ENTRY);

void freeHeaderProfileSettings(void)
{
	PRT_PROFILE_SETTING list = headerProfileSetting, next;

	while (list != NULL)
	{
		next = list->Next;
		free(list);
		list = next;
		next = list->Next;
	}

	headerProfileSetting = NULL;
}

/*
 * description: write station Adhoc Channel (a << 8 | bg)
 */
static int getStaAdhocChannel(webs_t *wp, char** params, int nparams)

{
	unsigned int country_region_bg, country_region_a;
	long country_region = 0;

	country_region_bg = nvram_get_int(RT2860_NVRAM, "CountryRegion", 0);
	country_region_a = nvram_get_int(RT2860_NVRAM, "CountryRegionABand", 0);

	country_region = country_region_bg | ( country_region_a << 8);
	return websWrite(wp, "%ld", country_region);
}

/*
 * description: write the BSSID list (site survey)
 */
static int getStaBSSIDList(webs_t *wp, char** params, int nparams)

{
	int                         ret, retry=1, rowcount=0;
	unsigned int                lBufLen = 4096, we_version=16; // 64K
	PNDIS_802_11_BSSID_LIST_EX	pBssidList;
	PNDIS_WLAN_BSSID_EX  		pBssid;
	unsigned int                ConnectStatus = NdisMediaStateDisconnected;
	unsigned char               BssidQuery[6];
	NDIS_802_11_SSID            SSIDQuery;
	int							QueryCount=0, EAGAIN_Count=0;

	pBssidList = (PNDIS_802_11_BSSID_LIST_EX) malloc(65536*2);  //64k
	memset(pBssidList, 0x00, sizeof(char)*65536*2);

	//step 1
	while ((ConnectStatus != NdisMediaStateConnected) && (QueryCount < 3))
	{
		if (OidQueryInformation(OID_GEN_MEDIA_CONNECT_STATUS, "ra0", &ConnectStatus, sizeof(ConnectStatus)) < 0)
		{

			websError(wp, 500, "Query OID_GEN_MEDIA_CONNECT_STATUS failed!");
			free(pBssidList);
			return -1;
		}
		sleep(2);
		QueryCount++;
	}

	//step 2
	if (OidQueryInformation(RT_OID_802_11_RADIO, "ra0", &G_bRadio, sizeof(G_bRadio)) < 0)
	{
		websError(wp, 500, "Query RT_OID_802_11_RADIO failed!");
		free(pBssidList);
		return -1;
	}

	if ((ConnectStatus == NdisMediaStateConnected) && G_bRadio)
	{
		memset(&BssidQuery, 0x00, sizeof(BssidQuery));
		OidQueryInformation(OID_802_11_BSSID, "ra0", &BssidQuery, sizeof(BssidQuery));
		memset(&SSIDQuery, 0x00, sizeof(SSIDQuery));
		OidQueryInformation(OID_802_11_SSID, "ra0", &SSIDQuery, sizeof(SSIDQuery));
	}

	//step 3
	if (OidSetInformation(OID_802_11_BSSID_LIST_SCAN, "ra0", 0, 0) < 0)
	{
		websError(wp, 500, "Set OID_802_11_BSSID_LIST_SCAN failed!");
		free(pBssidList);
		return -1;
	}
	// wait a few seconds to get all AP.
	sleep(2);

	//step 4
	ret = OidQueryInformation(RT_OID_WE_VERSION_COMPILED, "ra0", &we_version, sizeof(we_version));
	if (ret < 0)
	{
		websError(wp, 500, "Query RT_OID_WE_VERSION_COMPILED error! return=%d", ret);
		free(pBssidList);
		return -1;
	}

	if (we_version >= 17)
		lBufLen=8192;
	else
		lBufLen=4096;

	ret = -1;
	retry = 1;
	while (ret < 0)
	{
		ret = OidQueryInformation(OID_802_11_BSSID_LIST, "ra0", pBssidList, lBufLen);
		if (errno == EAGAIN)
		{
			sleep(1);
			// fprintf(stderr, "errno == EAGAIN\n");
			EAGAIN_Count++;
			if (EAGAIN_Count>25)
			{
        			websError(wp, 500, "Query OID_802_11_BSSID_LIST error! errno == EAGAIN");
				free(pBssidList);
				return -1;
			}
			else
				continue;
		}
		else if (errno == E2BIG)
		{
			//fprintf(stderr, "errno == E2BIG\n");
			lBufLen = lBufLen + 4096*retry;
			if (lBufLen < 65536)
			{
				retry++;
				// fprintf(stderr,"lBufLen=%d\n",lBufLen);
				continue;
			}
			else
			{
				websError(wp, 500, "Query OID_802_11_BSSID_LIST error! E2BIG");
				free(pBssidList);
				return -1;
			}
		}
		else if( ret != 0 )
		{
			websError(wp, 500, "Query OID_802_11_BSSID_LIST error! return=%d", ret);
			free(pBssidList);
			return -1;
		}
	}
	if ( pBssidList->NumberOfItems == 0)
	{
		websError(wp, 500, "Bssid List number is 0!\n");
		free(pBssidList);
		return 0;
	}
	else
	{
		unsigned char tmpBSSIDII[16], tmpBSSID[28], tmpSSID[64+NDIS_802_11_LENGTH_SSID], tmpRSSI[16], tmpAuth[32], tmpEncry[20], tmpNetworkType[24];
		unsigned char tmpSSIDII[NDIS_802_11_LENGTH_SSID+1];

		unsigned int i=0;
                int j=0;
		unsigned int nSigQua;
		int nChannel = 1;
		unsigned char radiocheck[8];

		pBssid = (PNDIS_WLAN_BSSID_EX) pBssidList->Bssid;
		G_ConnectStatus = NdisMediaStateDisconnected;

		for (i = 0; i < pBssidList->NumberOfItems; i++)
		{
			memset(radiocheck, 0x00, sizeof(radiocheck));
			memset(tmpBSSID, 0x00, sizeof(tmpBSSID));
			memset(tmpRSSI, 0x00, sizeof(tmpRSSI));
			memset(tmpSSID, 0x00, sizeof(tmpSSID));
			memset(tmpAuth, 0x00, sizeof(tmpAuth));
			memset(tmpEncry, 0x00, sizeof(tmpEncry));
			memset(tmpNetworkType, 0x00, sizeof(tmpNetworkType));
			memset(tmpBSSIDII, 0x00, sizeof(tmpBSSIDII));
			memset(tmpSSIDII, 0x00, sizeof(tmpSSIDII));

			if (strcmp((char *)pBssid->Ssid.Ssid, "") == 0)
				sprintf((char *)tmpSSID, "&nbsp;");
			else {
				unsigned int c = 0;
				do {
					if (pBssid->Ssid.Ssid[c] < 32 || pBssid->Ssid.Ssid[c] > 126 || pBssid->Ssid.Ssid[c] == 13) // 13 is string end of Dos
					{
						strcpy((char *)pBssid->Ssid.Ssid, "&nbsp;");
						break;
					}
					c++;
				} while(c < pBssid->Ssid.SsidLength-1);
				sprintf((char *)tmpSSID, "%s", pBssid->Ssid.Ssid);
			}

			sprintf((char *)tmpBSSID, "%02X-%02X-%02X-%02X-%02X-%02X",
				pBssid->MacAddress[0], pBssid->MacAddress[1], pBssid->MacAddress[2],
				pBssid->MacAddress[3], pBssid->MacAddress[4], pBssid->MacAddress[5]);

			sprintf((char *)tmpBSSIDII, "%02X%02X%02X%02X%02X%02X",
				pBssid->MacAddress[0], pBssid->MacAddress[1], pBssid->MacAddress[2],
				pBssid->MacAddress[3], pBssid->MacAddress[4], pBssid->MacAddress[5]);

			nSigQua = ConvertRssiToSignalQuality(pBssid->Rssi);
			sprintf((char *)tmpRSSI,"%d%%", nSigQua);

			nChannel = -1;	
			for(j = 0; j < G_nChanFreqCount; j++)
			{
				//fprintf(stderr, "pBssid->Configuration.DSConfig = %d, ChannelFreqTable[j].lFreq=%d\n ", pBssid->Configuration.DSConfig, ChannelFreqTable[j].lFreq);
				if (pBssid->Configuration.DSConfig == ChannelFreqTable[j].lFreq) {
					nChannel = ChannelFreqTable[j].lChannel;
					break;
				}
			}

			if (nChannel == -1)
				continue;

			if (pBssid->InfrastructureMode == Ndis802_11Infrastructure)
				sprintf((char *)tmpNetworkType, "%s", "Infrastructure");
			else
				sprintf((char *)tmpNetworkType, "%s", "Ad Hoc");

			// work with NDIS_WLAN_BSSID_EX
			unsigned char bTKIP = FALSE;
			unsigned char bAESWRAP = FALSE;
			unsigned char bAESCCMP = FALSE;
			unsigned char bWPA = FALSE;
			unsigned char bWPAPSK = FALSE;
			unsigned char bWPANONE = FALSE;
			unsigned char bWPA2 = FALSE;
			unsigned char bWPA2PSK = FALSE;
			unsigned char bWPA2NONE = FALSE;
			unsigned char bCCKM = FALSE; // CCKM for Cisco, add by candy 2006.11.24

			if ((pBssid->Length > sizeof(NDIS_WLAN_BSSID)) && (pBssid->IELength > sizeof(NDIS_802_11_FIXED_IEs)))
			{
				unsigned int lIELoc = 0;
				PNDIS_802_11_FIXED_IEs pFixIE = (PNDIS_802_11_FIXED_IEs)pBssid->IEs;
				PNDIS_802_11_VARIABLE_IEs pVarIE = (PNDIS_802_11_VARIABLE_IEs)((char*)pFixIE + sizeof(NDIS_802_11_FIXED_IEs));
				lIELoc += sizeof(NDIS_802_11_FIXED_IEs);

				while (pBssid->IELength > (lIELoc + sizeof(NDIS_802_11_VARIABLE_IEs)))
				{
					if ((pVarIE->ElementID == 221) && (pVarIE->Length >= 16))
					{
						unsigned int* pOUI = (unsigned int*)((char*)pVarIE->data);
						if (*pOUI != WPA_OUI_TYPE)
						{
							lIELoc += pVarIE->Length;
							lIELoc += 2;
							pVarIE = (PNDIS_802_11_VARIABLE_IEs)((char*)pVarIE + pVarIE->Length + 2);

							if(pVarIE->Length <= 0)
								break;

							continue;
						}

						unsigned int* plGroupKey; 
						unsigned short* pdPairKeyCount;
						unsigned int* plPairwiseKey=NULL;
						unsigned int* plAuthenKey=NULL;
						unsigned short* pdAuthenKeyCount;
						plGroupKey = (unsigned int*)((char*)pVarIE + 8);

						unsigned int lGroupKey = *plGroupKey & 0x00ffffff;
						//fprintf(stderr, "lGroupKey=%d\n", lGroupKey);
						if (lGroupKey == WPA_OUI) {
							lGroupKey = (*plGroupKey & 0xff000000) >> 0x18;
							if (lGroupKey == 2)
								bTKIP = TRUE;
							else if (lGroupKey == 3)
								bAESWRAP = TRUE;
							else if (lGroupKey == 4)
								bAESCCMP = TRUE;
						}
						else
						{
							lIELoc += pVarIE->Length;
							lIELoc += 2;
							pVarIE = (PNDIS_802_11_VARIABLE_IEs)((char*)pVarIE + pVarIE->Length + 2);

							if(pVarIE->Length <= 0)
								break;

							continue;
						}

						pdPairKeyCount = (unsigned short*)((char*)plGroupKey + 4);
						plPairwiseKey = (unsigned int*) ((char*)pdPairKeyCount + 2);
						unsigned short k = 0;
						for (k = 0; k < *pdPairKeyCount; k++) {
							unsigned int lPairKey = *plPairwiseKey & 0x00ffffff;
							if (lPairKey == WPA_OUI )//|| (lPairKey & 0xffffff00) == WPA_OUI_1)
							{
								lPairKey = (*plPairwiseKey & 0xff000000) >> 0x18;
								if (lPairKey == 2)
									bTKIP = TRUE;
								else if (lPairKey == 3)
									bAESWRAP = TRUE;
								else if (lPairKey == 4)
									bAESCCMP = TRUE;
							}
							else
								break;

							plPairwiseKey++;
						}

						pdAuthenKeyCount = (unsigned short*)((char*)pdPairKeyCount + 2 + 4 * (*pdPairKeyCount));
						plAuthenKey = (unsigned int*)((char*)pdAuthenKeyCount + 2);

						for(k = 0; k < *pdAuthenKeyCount; k++)
						{
							unsigned int lAuthenKey = *plAuthenKey & 0x00ffffff;
							if (lAuthenKey == CISCO_OUI) {
								bCCKM = TRUE; // CCKM for Cisco
							}
							else if (lAuthenKey == WPA_OUI) {
								lAuthenKey = (*plAuthenKey & 0xff000000) >> 0x18;

								if (lAuthenKey == 1)
									bWPA = TRUE;
								else if (lAuthenKey == 0 || lAuthenKey == 2) {
									if (pBssid->InfrastructureMode)
										bWPAPSK = TRUE;
									else
										bWPANONE = TRUE;
								}
							}
							plAuthenKey++;
						}
					}
					else if (pVarIE->ElementID == 48 && pVarIE->Length >= 12)
					{
						unsigned int* plGroupKey; 
						unsigned int* plPairwiseKey; 
						unsigned short* pdPairKeyCount;
						unsigned int* plAuthenKey; 
						unsigned short* pdAuthenKeyCount;
						plGroupKey = (unsigned int*)((char*)pVarIE + 4);

						unsigned int lGroupKey = *plGroupKey & 0x00ffffff;
						if (lGroupKey == WPA2_OUI) {
							lGroupKey = (*plGroupKey & 0xff000000) >> 0x18;
							if (lGroupKey == 2)
								bTKIP = TRUE;
							else if (lGroupKey == 3)
								bAESWRAP = TRUE;
							else if (lGroupKey == 4)
								bAESCCMP = TRUE;
						}
						else
						{
							lIELoc += pVarIE->Length;
							lIELoc += 2;
							pVarIE = (PNDIS_802_11_VARIABLE_IEs)((char*)pVarIE + pVarIE->Length + 2);

							if(pVarIE->Length <= 0)
								break;

							continue;
						}

						pdPairKeyCount = (unsigned short*)((char*)plGroupKey + 4);
						plPairwiseKey = (unsigned int*)((char*)pdPairKeyCount + 2);
						unsigned short k = 0;

						for (k = 0; k < *pdPairKeyCount; k++)
						{
							unsigned int lPairKey = *plPairwiseKey & 0x00ffffff;
							if (lPairKey == WPA2_OUI) {
								lPairKey = (*plPairwiseKey & 0xff000000) >> 0x18;
								if (lPairKey == 2)
									bTKIP = TRUE;
								else if (lPairKey == 3)
									bAESWRAP = TRUE;
								else if (lPairKey == 4)
									bAESCCMP = TRUE;
							}
							else
								break;
							plPairwiseKey++;
						}

						pdAuthenKeyCount = (unsigned short*)((char*)pdPairKeyCount + 2 + 4 * *pdPairKeyCount);
						plAuthenKey = (unsigned int*)((char*)pdAuthenKeyCount + 2);
						for (k = 0; k < *pdAuthenKeyCount; k++)
						{
							unsigned int lAuthenKey = *plAuthenKey & 0x00ffffff;
							if (lAuthenKey == CISCO_OUI) {
								bCCKM = TRUE; // CCKM for Cisco
							}
							else if (lAuthenKey == WPA2_OUI) {
								lAuthenKey = (*plAuthenKey & 0xff000000) >> 0x18;
								if (lAuthenKey == 1)
									bWPA2 = TRUE;
								else if (lAuthenKey == 0 || lAuthenKey == 2) {
									if (pBssid->InfrastructureMode)
										bWPA2PSK = TRUE;
									else
										bWPA2NONE = TRUE;
								}
							}
							plAuthenKey++;
						}
					}
					lIELoc += pVarIE->Length;
					lIELoc += 2;
					pVarIE = (PNDIS_802_11_VARIABLE_IEs)((char*)pVarIE + pVarIE->Length + 2);

					if (pVarIE->Length <= 0)
						break;
				}
			}

			char strAuth[32], strEncry[32];
			memset( strAuth, 0x00, sizeof(strAuth) );
			memset( strEncry, 0x00, sizeof(strEncry) );
			if (bCCKM)
				strcat(strAuth, "CCKM; ");
			if (bWPA)
				strcat(strAuth, "WPA; ");
			if (bWPAPSK)
				strcat(strAuth, "WPA-PSK; ");
			if (bWPANONE)
				strcat(strAuth, "WPA-NONE; ");
			if (bWPA2)
				strcat(strAuth, "WPA2; ");
			if (bWPA2PSK)
				strcat(strAuth, "WPA2-PSK; ");
			if (bWPA2NONE)
				strcat(strAuth, "WPA2-NONE; ");

			if (strlen(strAuth) > 0)
			{
				strncpy((char *)tmpAuth, strAuth, strlen(strAuth) - 2);
				strcpy(strAuth, (char *)tmpAuth);
			}
			else
				strcpy((char *)strAuth, "Unknown");

			if (bTKIP)
				strcpy(strEncry, "TKIP; ");
			if (bAESWRAP || bAESCCMP)
				strcat(strEncry, "AES; ");

			if (strlen(strEncry) > 0) {
				strncpy((char *)tmpEncry, strEncry, strlen(strEncry) - 2);
				strcpy(strEncry, (char *)tmpEncry);
			}
			else
			{
				if (pBssid->Privacy)  // privacy value is on/of
					strcpy(strEncry, "WEP");
				else {
					strcpy(strEncry, "Not Use");
					strcpy(strAuth, "OPEN");
				}
			}

			sprintf((char *)tmpAuth, "%s", strAuth);
			sprintf((char *)tmpEncry, "%s", strEncry);

			strcpy((char *)tmpSSIDII, (char *)pBssid->Ssid.Ssid);
			if (strlen((char *)G_SSID.Ssid)>0 && strcmp((char *)pBssid->Ssid.Ssid, (char *)G_SSID.Ssid) == 0)
				strcpy((char*)radiocheck, "checked");
			else
				strcpy((char*)radiocheck, "");

			websWrite(wp, "%s\n[ %d, '%s', '%s', %d, '%s', %u, '%s', '%s', '%s', %d ]", 
				((rowcount++) == 0) ? "" : ",", 
				(memcmp(BssidQuery, pBssid->MacAddress, 6) == 0) ? 1 : 0,
				tmpSSID,
				tmpBSSID,
				pBssid->Rssi,
				tmpRSSI,
				nChannel,
				tmpEncry,
				tmpAuth,
				tmpNetworkType,
				pBssid->InfrastructureMode
			);

			pBssid = (PNDIS_WLAN_BSSID_EX)((char *)pBssid + pBssid->Length);
		}
	}

	free(pBssidList);

	return 0;
}

/*
 * description: write the BSSID that station connected to
 */
static int getStaConnectedBSSID(webs_t *wp, char** params, int nparams)

{
	unsigned char BssidQuery[6];

	//fprintf(stderr, "-->ssi_getStaConnectedBSSID()\n");
	//step 1
	if (OidQueryInformation(OID_GEN_MEDIA_CONNECT_STATUS, "ra0", &G_ConnectStatus, sizeof(G_ConnectStatus)) < 0)
	{
		websError(wp, 500, "Query OID_GEN_MEDIA_CONNECT_STATUS error!");
		return -1;
	}

	//step 2
	if (OidQueryInformation(RT_OID_802_11_RADIO, "ra0", &G_bRadio, sizeof(G_bRadio)) < 0)
	{
		websError(wp, 500, "Query RT_OID_802_11_RADIO error!");
		return -1;
	}

	if (G_ConnectStatus == NdisMediaStateConnected && G_bRadio)
	{
		memset(&BssidQuery, 0x00, sizeof(BssidQuery));
		OidQueryInformation(OID_802_11_BSSID, "ra0", &BssidQuery, sizeof(BssidQuery));
		websWrite(wp, "<tr><td><input type=checkbox name=mac onClick=selectedBSSID(\'%02X%02X%02X%02X%02X%02X\')> %02X:%02X:%02X:%02X:%02X:%02X</td></tr>",
				BssidQuery[0], BssidQuery[1], BssidQuery[2],BssidQuery[3], BssidQuery[4], BssidQuery[5],
				BssidQuery[0], BssidQuery[1], BssidQuery[2],BssidQuery[3], BssidQuery[4], BssidQuery[5]);
	}

	return 0;
}

/*
 * description: return G_bdBm_ischeck (displaying dbm or % type)
 */
static int getStaDbm(webs_t *wp, char** params, int nparams)

{
//FIXME
//	ejSetResult(eid, (G_bdBm_ischeck == 1) ? "1" : "0");
	return 0;
}

/*
 * description: write station extra info
 */
static int getStaExtraInfo(webs_t *wp, char** params, int nparams)

{
	unsigned long lExtraInfo;
	int ret;

	char *ExtraInfoTable[11] = {
		"Link is Up",
		"Link is Down",
		"Hardware radio off",
		"Software radio off",
		"Open authentication fail",
		"Shared authentication fail",
		"Association failed",
		"Deauthencation because MIC failure",
		"Deauthencation on 4-way handshake timeout",
		"Deauthencation on group key handshake timeout",
		"EAP successd"
	};

	ret = OidQueryInformation(RT_OID_802_11_EXTRA_INFO, "ra0", &lExtraInfo, 4);
	if (ret < 0 )
		return websWrite(wp, "&nbsp;");
	else {
		if (lExtraInfo <= 0xa)
			websWrite(wp, "%s", ExtraInfoTable[lExtraInfo]);
		else
			websWrite(wp, "&nbsp;");
	}

	return 0;
}

static int getLinkingMode(webs_t *wp, char** params, int nparams)

{
	MACHTTRANSMIT_SETTING HTSetting;

	memset(&HTSetting, 0x00, sizeof(MACHTTRANSMIT_SETTING));
	if (OidQueryInformation(RT_OID_802_11_QUERY_LAST_TX_RATE, "ra0", &HTSetting, sizeof(MACHTTRANSMIT_SETTING)) < 0)
	{
	    syslog(LOG_ERR, "oid query failed, %s", __FUNCTION__);
	    return -1;
	}

	return websWrite(wp, (HTSetting.field.MODE > 1) ? T("0") : T("1"));
}

/*
 * description: write station HT transmit
 */
static int getStaHT(webs_t *wp, char** params, int nparams)

{
	MACHTTRANSMIT_SETTING HTSetting;
	char tmp[8], tmpBW[88], tmpGI[88], tmpSTBC[88], tmpMCS[88];

	if (G_ConnectStatus == NdisMediaStateDisconnected)
	{
		sprintf((char *)tmpBW, "<tr><td class=\"head\" bgcolor=\"#E8F8FF\" >BW</td><td >n/a</td></tr>");
		sprintf((char *)tmpGI, "<tr><td class=\"head\" bgcolor=\"#E8F8FF\" >GI</td><td >n/a</td></tr>");
		sprintf((char *)tmpSTBC,"<tr><td class=\"head\" bgcolor=\"#E8F8FF\" >STBC</td><td >n/a</td></tr>");
		sprintf((char *)tmpMCS, "<tr><td class=\"head\" bgcolor=\"#E8F8FF\" >MCS</td><td >n/a</td></tr>");
		return websWrite(wp,"%s %s %s %s", tmpBW, tmpGI, tmpSTBC, tmpMCS);
	}


	memset(&HTSetting, 0x00, sizeof(MACHTTRANSMIT_SETTING));
	if (OidQueryInformation(RT_OID_802_11_QUERY_LAST_TX_RATE, "ra0", &HTSetting, sizeof(MACHTTRANSMIT_SETTING)) < 0)
	{
	    syslog(LOG_ERR, "oid query failed, %s", __FUNCTION__);
	    return -1;
	}

	if (HTSetting.field.MODE > 1) // 0: CCK, 1:OFDM, 2:Mixedmode, 3:GreenField
	{
		if (HTSetting.field.BW == 0)
			strcpy(tmp, "20");
		else
			strcpy(tmp, "40");
		snprintf(tmpBW, 72, "<tr><td class=\"head\" bgcolor=\"#E8F8FF\" >BW</td><td >%s</td></tr>", tmp);

		if (HTSetting.field.ShortGI == 0)
			strcpy(tmp, "long");
		else
			strcpy(tmp, "short");
		snprintf(tmpGI, 72, "<tr><td class=\"head\" bgcolor=\"#E8F8FF\" >GI</td><td >%s</td></tr>", tmp);

		if (HTSetting.field.STBC == 0)
			strcpy(tmp, "none");
		else
			strcpy(tmp, "used");
		snprintf(tmpSTBC, 72, "<tr><td class=\"head\" bgcolor=\"#E8F8FF\" >STBC</td><td >%s</td></tr>", tmp);

		snprintf(tmpMCS, 72, "<tr><td class=\"head\" bgcolor=\"#E8F8FF\" >MCS</td><td >%d</td></tr>", HTSetting.field.MCS);
	} else {
		sprintf((char *)tmpBW, "<tr><td class=\"head\" bgcolor=\"#E8F8FF\" >BW</td><td >n/a</td></tr>");
		sprintf((char *)tmpGI, "<tr><td class=\"head\" bgcolor=\"#E8F8FF\" >GI</td><td >n/a</td></tr>");
		sprintf((char *)tmpSTBC,"<tr><td class=\"head\" bgcolor=\"#E8F8FF\" >STBC</td><td >n/a</td></tr>");
		sprintf((char *)tmpMCS, "<tr><td class=\"head\" bgcolor=\"#E8F8FF\" >MCS</td><td >n/a</td></tr>");
	}

	return websWrite(wp,"%s %s %s %s", tmpBW, tmpGI, tmpSTBC, tmpMCS);
}

/*
 * description: write station extra info
 */
static int getStaLinkChannel(webs_t *wp, char** params, int nparams)

{
	RT_802_11_LINK_STATUS     LinkStatus;
	MACHTTRANSMIT_SETTING HTSetting;
	int i;
	int nChannel = -1;
	int Japan_channel = 200;
	unsigned int freq = 0;

	if (G_ConnectStatus == NdisMediaStateDisconnected)
		return websWrite(wp, "&nbsp;");

#ifdef HAVE_MT76X2_3
	freq = getWlanStationFrequencyKHz(1);
#endif

	// Current Channel
	for (i = 0; i < G_nChanFreqCount; i++)
	{
		if (freq == ChannelFreqTable[i].lFreq)
		{
			nChannel = ChannelFreqTable[i].lChannel;
			break;
		}
	}

	OidQueryInformation(RT_OID_802_11_QUERY_LINK_STATUS, "ra0", &LinkStatus, sizeof(&LinkStatus));

	memset(&HTSetting, 0x00, sizeof(MACHTTRANSMIT_SETTING));
	OidQueryInformation(RT_OID_802_11_QUERY_LAST_TX_RATE, "ra0", &HTSetting, sizeof(MACHTTRANSMIT_SETTING));

	if (nChannel == -1) {
		websWrite(wp, "error!");
	} else {		// 0: CCK, 1:OFDM, 2:Mixedmode, 3:GreenField
		if (nChannel == (Japan_channel + 8))
			websWrite(wp, "J8 <--> %ld KHz", freq);
		else if (nChannel == (Japan_channel + 12))
			websWrite(wp, "J12 <--> %ld KHz", freq);
		else if (nChannel == (Japan_channel + 16))
			websWrite(wp, "J16 <--> %ld KHz", freq);
		else
			websWrite(wp, "%u <--> %ld KHz", nChannel, freq);

		if (HTSetting.field.MODE <= 1)
		    websWrite(wp, " ; Central Channel: %ld", LinkStatus.CentralChannel);
	}

	
	return 0;
}

/*
 * description: write station link quality
 */
static int getStaLinkQuality(webs_t *wp, char** params, int nparams)
{
    int channelQuality;

    if (G_ConnectStatus == NdisMediaStateDisconnected)
        return websWrite(wp, "0%%");

#ifdef HAVE_MT76X2_3
    channelQuality = getWlanStationLinkQuality(1);
#else
    channelQuality = -1;
#endif

    if (channelQuality < 0)
    {
        return websWrite(wp, "N/A");
    }

    if (m_lChannelQuality > 0)
        channelQuality = (int)((m_lChannelQuality + channelQuality) / 2.0 + 0.5);

    m_lChannelQuality = channelQuality;

    if (channelQuality > 70)
        websWrite(wp, "Good");
    else if (channelQuality > 40)
        websWrite(wp, "Normal");
    else
        websWrite(wp, "Weak");

    return websWrite(wp, " &nbsp;&nbsp;&nbsp;&nbsp; %d%%", channelQuality);
}


/*
 * description: write station link Rx rate
 */
static int getStaLinkRxRate(webs_t *wp, char** params, int nparams)

{
	char buf[32];

	if (G_ConnectStatus == NdisMediaStateDisconnected)
		return websWrite(wp, "0");

	double fLastRxRate = 1;

#ifdef HAVE_MT76X2_3
	if (getLastRxRateFor11n(&fLastRxRate))
	    return -1;

	snprintf(buf, sizeof(buf), "%.1f", fLastRxRate);
	websWrite(wp, "%s", buf);

	return 0;
#else
        return -1;
#endif
}

/*
 * description: write station link status
 */
static int getStaLinkStatus(webs_t *wp, char** params, int nparams)

{
	int s, ret;

	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		syslog(LOG_ERR, "open socket failed, %s", __FUNCTION__);
		return -1;
	}

	ret = OidQueryInformation(OID_GEN_MEDIA_CONNECT_STATUS, "ra0", &G_ConnectStatus, sizeof(G_ConnectStatus));
	if (ret < 0 )
	{
		close(s);
		return websWrite(wp, "Disconnected");
	}

	if (G_ConnectStatus == NdisMediaStateConnected)
	{
		NDIS_802_11_SSID SSID;
		unsigned char Bssid[6];

		memset(&SSID, 0x00, sizeof(NDIS_802_11_SSID));
		OidQueryInformation(OID_802_11_SSID, "ra0", &SSID, sizeof(NDIS_802_11_SSID));

		memset(&Bssid, 0x00, sizeof(Bssid));
		OidQueryInformation(OID_802_11_BSSID, "ra0", Bssid, 6);

		SSID.Ssid[SSID.SsidLength] = 0;
		websWrite(wp, "%s <--> %02X-%02X-%02X-%02X-%02X-%02X", SSID.Ssid,
				Bssid[0], Bssid[1], Bssid[2], Bssid[3], Bssid[4], Bssid[5]);
	}
	else
	{
		if (OidQueryInformation(RT_OID_802_11_RADIO, "ra0", &G_bRadio, sizeof(G_bRadio)) < 0)
		{
			websWrite(wp, "error!");
			close(s);
			return 0;
		}
		if (G_bRadio)
		{
			websWrite(wp, "Disconnected");
		}
		else
			websWrite(wp, "RF Off");
	}
	close(s);
	return 0;
}

/*
 * description: write station link Rx rate
 */
static int getStaLinkTxRate(webs_t *wp, char** params, int nparams)

{
	char buf[32];

	if (G_ConnectStatus == NdisMediaStateDisconnected)
		return websWrite(wp, "0");

#ifdef HAVE_MT76X2_3
	double fLastTxRate = 1;
	if (getLastTxRateFor11n(&fLastTxRate))
	    return -1;

	snprintf(buf, sizeof(buf), "%.1f", fLastTxRate);
	websWrite(wp, "%s", buf);

	return 0;
#else
    return -1;
#endif
}

/*
 * description: write station new profile name
 */
static int getStaNewProfileName(webs_t *wp, char** params, int nparams)

{
	char profilename[32+1]; //refer to _RT_PROFILE_SETTING.
	strcpy(profilename, "PROF00");

	if (headerProfileSetting != NULL)
	{
		int count = 1, len=0;
		char cnum;
		PRT_PROFILE_SETTING currentProfileSetting = headerProfileSetting;
		do {
			if (strncmp((char *)currentProfileSetting->Profile, "PROF", 4) == 0) {
				len = strlen((char *)currentProfileSetting->Profile);
				cnum = currentProfileSetting->Profile[len-1];

				if (cnum >='0' && cnum <= '9') {
					count = cnum-48+1;  //ascii 0 is 48.
					memset(profilename, 0x00, 32);
					strncpy(profilename, (char *)currentProfileSetting->Profile, len-1);
				}
				currentProfileSetting = currentProfileSetting->Next;
			}
			else {
				currentProfileSetting = currentProfileSetting->Next;
			}
		} while (currentProfileSetting != NULL );
		return websWrite(wp, "%s%d", profilename, count);
	}
	else
		return websWrite(wp, "PROF001");
}

/*
 * description: write station noise level
 */
static int getStaNoiseLevel(webs_t *wp, char** params, int nparams)

{
	int nNoiseDbm = 0;
	int nNoisePercent;

	if (G_ConnectStatus == NdisMediaStateDisconnected)
		return websWrite(wp, "0%%");

	// Noise Level
	// Get Noise Level From Driver

#ifdef HAVE_MT76X2_3
	nNoiseDbm = getWlanStationNoiseDbm(1);
#endif

	nNoisePercent = (nNoiseDbm + 100) * 10 / 3;

	if (nNoisePercent > 100)
		nNoisePercent = 100;
	else if (nNoisePercent < 0)
		nNoisePercent = 0;

	// Show the NoiseLevel Strength Word & Percentage
	if (nNoisePercent > 90) {
		websWrite(wp, "Strength");
	}
	else if (nNoisePercent > 50) {
		websWrite(wp, "Normal");
	}
	else {
		websWrite(wp, "Low");
	}

	if (G_bdBm_ischeck)
		return websWrite(wp, " &nbsp;&nbsp;&nbsp;&nbsp; %d dBm", nNoiseDbm);
	else
		return websWrite(wp, " &nbsp;&nbsp;&nbsp;&nbsp; %d%%", nNoisePercent);
}

#define PARSE_STR(var, p_var) \
	splitString(&split, nvram_get(RT2860_NVRAM, var), ';'); \
	for (i=0, p=headerProfileSetting ; (i < split.found) && (p != NULL); i++, p=p->Next) \
	{ \
		strcpy((char *)(p_var), split.items[i]); \
		/*printf(var "[%d]=%s\n", i, p_var);*/ \
	}

#define PARSE_INT(var, p_var) \
	splitString(&split, nvram_get(RT2860_NVRAM, var), ';'); \
	for (i=0, p=headerProfileSetting; (i < split.found) && (p != NULL); i++, p=p->Next) \
	{ \
		p_var = atoi(split.items[i]); \
		/*printf(var "[%d]=%d\n", i, p_var);*/ \
	}


/*
 * description: station profile initialization
 */
void initStaProfile(void)
{
//FIXME
/*	int i;

	G_ConnectStatus = NdisMediaStateDisconnected;

	// Free previously allocated profile settings
	freeHeaderProfileSettings();

	// Initialize string splitter
	string_split_t split;
	if (initSplitter(&split) != 0)
	{
		syslog(LOG_ERR, "Error while trying to acqiure resources for profile parsing, %s", __FUNCTION__);
		return;
	}

	// Initialize NVRAM
	nvram_init(RT2860_NVRAM);

	// Try to fetch profile names & determine number of profiles
	splitString(&split, nvram_get(RT2860_NVRAM, "staProfile"), ';');
	PRT_PROFILE_SETTING *p_new = &headerProfileSetting;

	for (i=0; i<split.found; i++)
	{
		// String is empty?
		if (CHK_IF_EMPTYSTR(split.items[i]) && (i <= 0))
		{
			syslog(LOG_WARNING, "no previous profiles defined, %s", __FUNCTION__);
			freeSplitter(&split);
			nvram_close(RT2860_NVRAM);
			return;
		}

		PRT_PROFILE_SETTING p = (PRT_PROFILE_SETTING)malloc(sizeof(RT_PROFILE_SETTING));
		if (p == NULL)
		{
			syslog(LOG_ERR, "Not enough memory, %s", __FUNCTION__);
			freeSplitter(&split);
			freeHeaderProfileSettings();
			nvram_close(RT2860_NVRAM);
			return;
		}
		bzero(p, sizeof(RT_PROFILE_SETTING));
		strcpy(p->Profile, split.items[i]); // TODO: profile name length check

		// Add to list
		p->Next = NULL;
		*p_new = p;
		p_new = &p->Next;
	}

	G_staProfileNum = split.found;

	// Now parse other variables
	PRT_PROFILE_SETTING p = headerProfileSetting;

	// Common config
	PARSE_STR("staSSID", p->SSID); // SSID

	PARSE_INT("staNetworkType", p->NetworkType); // NetworkType
	PARSE_INT("staPSMode", p->PSmode); // PSMode
	PARSE_INT("staAdhocMode", p->AdhocMode); // AdhocMode
	PARSE_INT("staChannel", p->Channel); // Channel
	PARSE_INT("staPreamType", p->PreamType); // PreamType
	PARSE_INT("staRTSCheck", p->RTSCheck); // RTSCheck
	PARSE_INT("staFragmentCheck", p->FragmentCheck); // FragmentCheck
	PARSE_INT("staRTS", p->RTS); // RTS
	PARSE_INT("staFragment", p->Fragment); // Fragment
	PARSE_INT("staActive", p->Active); // Active
	PARSE_INT("staAuth", p->Authentication); // Auth
	PARSE_INT("staEncrypt", p->Encryption); // Encryption

	// WEP config
	PARSE_STR("staKey1", p->Key1); // Key1
	PARSE_STR("staKey2", p->Key2); // Key2
	PARSE_STR("staKey3", p->Key3); // Key3
	PARSE_STR("staKey4", p->Key4); // Key4
	PARSE_INT("staKey1Type", p->Key1Type); // Key1Type
	PARSE_INT("staKey2Type", p->Key2Type); // Key2Type
	PARSE_INT("staKey3Type", p->Key3Type); // Key3Type
	PARSE_INT("staKey4Type", p->Key4Type); // Key4Type
	PARSE_INT("staKey1Length", p->Key1Length); // Key1Length
	PARSE_INT("staKey2Length", p->Key2Length); // Key2Length
	PARSE_INT("staKey3Length", p->Key3Length); // Key3Length
	PARSE_INT("staKey4Length", p->Key4Length); // Key4Length
	PARSE_INT("staKeyDefaultId", p->KeyDefaultId); // KeyDefaultId

	// WPA config
	PARSE_STR("staWpaPsk", p->WpaPsk); // WpaPsk

	// Make some magix
	for (i=0, p=headerProfileSetting; p != NULL; i++, p=p->Next)
		p->SsidLen = strlen(p->SSID);

	// Release splitter
	freeSplitter(&split);

	// Release NVRAM
	nvram_close(RT2860_NVRAM);

#undef PARSE_STR
#undef PARSE_INT
*/
}

/*
 * description: convert ascii byte to numeric
 */
unsigned char BtoH(char ch)
{
	if (ch >= '0' && ch <= '9') return (ch - '0');        // Handle numerals
	if (ch >= 'A' && ch <= 'F') return (ch - 'A' + 0xA);  // Handle capitol hex digits
	if (ch >= 'a' && ch <= 'f') return (ch - 'a' + 0xA);  // Handle small hex digits
	return(255);
}

/*
 * description: Converts ascii string to network order hex
 * parameters: src     - pointer to input ascii string
 *             dest    - pointer to output hex
 *             destlen - size of dest
 */
void AtoH(char *src, unsigned char *dest, int destlen)
{
	char *srcptr;

	srcptr = src;
	unsigned char* destTemp = dest;
	while (destlen--) {
		*destTemp = BtoH(*srcptr++) << 4;  // Put 1st ascii byte in upper nibble.
		*destTemp += BtoH(*srcptr++);      // Add 2nd ascii byte to above.
		destTemp++;
	}
}

static void shaHashBlock(A_SHA_CTX *ctx)
{
	int t;
	unsigned long A,B,C,D,E,TEMP;

#define SHA_ROTL(X,n) ((((X) << (n)) | ((X) >> (32-(n)))) & 0xffffffffL)
	for (t = 16; t <= 79; t++)
		ctx->W[t] = SHA_ROTL(ctx->W[t-3] ^ ctx->W[t-8] ^ ctx->W[t-14] ^ ctx->W[t-16], 1);

	A = ctx->H[0];
	B = ctx->H[1];
	C = ctx->H[2];
	D = ctx->H[3];
	E = ctx->H[4];

	for (t = 0; t <= 19; t++)
	{
		TEMP = (SHA_ROTL(A,5) + (((C^D)&B)^D)     + E + ctx->W[t] + 0x5a827999L) & 0xffffffffL;
		E = D; D = C; C = SHA_ROTL(B, 30); B = A; A = TEMP;
	}
	for (t = 20; t <= 39; t++)
	{
		TEMP = (SHA_ROTL(A,5) + (B^C^D)           + E + ctx->W[t] + 0x6ed9eba1L) & 0xffffffffL;
		E = D; D = C; C = SHA_ROTL(B, 30); B = A; A = TEMP;
	}
	for (t = 40; t <= 59; t++)
	{
		TEMP = (SHA_ROTL(A,5) + ((B&C)|(D&(B|C))) + E + ctx->W[t] + 0x8f1bbcdcL) & 0xffffffffL;
		E = D; D = C; C = SHA_ROTL(B, 30); B = A; A = TEMP;
	}
	for (t = 60; t <= 79; t++)
	{
		TEMP = (SHA_ROTL(A,5) + (B^C^D)           + E + ctx->W[t] + 0xca62c1d6L) & 0xffffffffL;
		E = D; D = C; C = SHA_ROTL(B, 30); B = A; A = TEMP;
	}

	ctx->H[0] += A;
	ctx->H[1] += B;
	ctx->H[2] += C;
	ctx->H[3] += D;
	ctx->H[4] += E;
}

void A_SHAInit(A_SHA_CTX *ctx)
{
	int i;

	ctx->lenW = 0;
	ctx->sizeHi = ctx->sizeLo = 0;

	/* Initialize H with the magic constants (see FIPS180 for constants)
	*/
	ctx->H[0] = 0x67452301L;
	ctx->H[1] = 0xefcdab89L;
	ctx->H[2] = 0x98badcfeL;
	ctx->H[3] = 0x10325476L;
	ctx->H[4] = 0xc3d2e1f0L;

	for (i = 0; i < 80; i++)
		ctx->W[i] = 0;
}

void A_SHAUpdate(A_SHA_CTX *ctx, unsigned char *dataIn, int len)
{
	int i;

	/* Read the data into W and process blocks as they get full */
	for (i = 0; i < len; i++)
	{
		ctx->W[ctx->lenW / 4] <<= 8;
		ctx->W[ctx->lenW / 4] |= (unsigned long)dataIn[i];
		if ((++ctx->lenW) % 64 == 0)
		{
			shaHashBlock(ctx);
			ctx->lenW = 0;
		}
		ctx->sizeLo += 8;
		ctx->sizeHi += (ctx->sizeLo < 8);
	}
}

void A_SHAFinal(A_SHA_CTX *ctx, unsigned char hashout[20])
{
	unsigned char pad0x80 = 0x80;
	unsigned char pad0x00 = 0x00;
	unsigned char padlen[8];
	int i;

	/* Pad with a binary 1 (e.g. 0x80), then zeroes, then length */
	padlen[0] = (unsigned char)((ctx->sizeHi >> 24) & 255);
	padlen[1] = (unsigned char)((ctx->sizeHi >> 16) & 255);
	padlen[2] = (unsigned char)((ctx->sizeHi >> 8) & 255);
	padlen[3] = (unsigned char)((ctx->sizeHi >> 0) & 255);
	padlen[4] = (unsigned char)((ctx->sizeLo >> 24) & 255);
	padlen[5] = (unsigned char)((ctx->sizeLo >> 16) & 255);
	padlen[6] = (unsigned char)((ctx->sizeLo >> 8) & 255);
	padlen[7] = (unsigned char)((ctx->sizeLo >> 0) & 255);
	A_SHAUpdate(ctx, &pad0x80, 1);
	while (ctx->lenW != 56)
		A_SHAUpdate(ctx, &pad0x00, 1);
	A_SHAUpdate(ctx, padlen, 8);

	/* Output hash
	*/
	for (i = 0; i < 20; i++)
	{
		hashout[i] = (unsigned char)(ctx->H[i / 4] >> 24);
		ctx->H[i / 4] <<= 8;
	}

	/*
	 *  Re-initialize the context (also zeroizes contents)
	 */
	A_SHAInit(ctx); 
}

static void hmac_sha1(unsigned char *text, int text_len, unsigned char *key, int key_len, unsigned char *digest) 
{ 
	A_SHA_CTX context;
	unsigned char k_ipad[65]; /* inner padding - key XORd with ipad */
	unsigned char k_opad[65]; /* outer padding - key XORd with opad */
	int i;

	/* if key is longer than 64 bytes reset it to key=SHA1(key) */
	if (key_len > 64)
	{
		A_SHA_CTX tctx;

		A_SHAInit(&tctx);
		A_SHAUpdate(&tctx, key, key_len);
		A_SHAFinal(&tctx, key);

		key_len = 20;
	}

	/*
	 * the HMAC_SHA1 transform looks like:
	 *
	 * SHA1(K XOR opad, SHA1(K XOR ipad, text))
	 *
	 * where K is an n byte key
	 * ipad is the byte 0x36 repeated 64 times
	 * opad is the byte 0x5c repeated 64 times
	 * and text is the data being protected
	 */

	/* start out by storing key in pads */
	memset(k_ipad, 0, sizeof k_ipad);
	memset(k_opad, 0, sizeof k_opad);
	memcpy(k_ipad, key, key_len);
	memcpy(k_opad, key, key_len);

	/* XOR key with ipad and opad values */
	for (i = 0; i < 64; i++)
	{
		k_ipad[i] ^= 0x36;
		k_opad[i] ^= 0x5c;
	}

	/* perform inner SHA1*/
	A_SHAInit(&context); /* init context for 1st pass */
	A_SHAUpdate(&context, k_ipad, 64); /* start with inner pad */
	A_SHAUpdate(&context, text, text_len); /* then text of datagram */
	A_SHAFinal(&context, digest); /* finish up 1st pass */

	/* perform outer SHA1 */
	A_SHAInit(&context); /* init context for 2nd pass */
	A_SHAUpdate(&context, k_opad, 64); /* start with outer pad */
	A_SHAUpdate(&context, digest, 20); /* then results of 1st hash */
	A_SHAFinal(&context, digest); /* finish up 2nd pass */
}

/*
 * F(P, S, c, i) = U1 xor U2 xor ... Uc
 * U1 = PRF(P, S || Int(i))
 * U2 = PRF(P, U1)
 * Uc = PRF(P, Uc-1)
 */

static void F(char *password, unsigned char *ssid, int ssidlength, int iterations, int count, unsigned char *output)
{
	unsigned char digest[36], digest1[A_SHA_DIGEST_LEN];
	int i, j;

	/* U1 = PRF(P, S || int(i)) */
	memcpy(digest, ssid, ssidlength);
	digest[ssidlength] = (unsigned char)((count>>24) & 0xff);
	digest[ssidlength+1] = (unsigned char)((count>>16) & 0xff);
	digest[ssidlength+2] = (unsigned char)((count>>8) & 0xff);
	digest[ssidlength+3] = (unsigned char)(count & 0xff);
	hmac_sha1(digest, ssidlength+4, (unsigned char*) password, (int) strlen(password), digest1); // for WPA update

	/* output = U1 */
	memcpy(output, digest1, A_SHA_DIGEST_LEN);

	for (i = 1; i < iterations; i++)
	{
		/* Un = PRF(P, Un-1) */
		hmac_sha1(digest1, A_SHA_DIGEST_LEN, (unsigned char*) password, (int) strlen(password), digest); // for WPA update
		memcpy(digest1, digest, A_SHA_DIGEST_LEN);

		/* output = output xor Un */
		for (j = 0; j < A_SHA_DIGEST_LEN; j++)
		{
			output[j] ^= digest[j];
		}
	}
}

/*
 * password - ascii string up to 63 characters in length
 * ssid - octet string up to 32 octets
 * ssidlength - length of ssid in octets
 * output must be 40 octets in length and outputs 256 bits of key
 */
int PasswordHash(char *password, unsigned char *ssid, int ssidlength, unsigned char *output)
{
	if ((strlen(password) > 63) || (ssidlength > 32))
		return 0;

	F(password, ssid, ssidlength, 4096, 1, output);
	F(password, ssid, ssidlength, 4096, 2, &output[A_SHA_DIGEST_LEN]);
	return 1;
}

static int getCACLCertList(webs_t *wp, char** params, int nparams)

{
	websWrite(wp, T("<option value=\"\"></option>"));

	return 0;
}

static int getKeyCertList(webs_t *wp, char** params, int nparams)

{
	websWrite(wp, T("<option value=\"\"></option>"));

	return 0;
}

/*
 * description: connect to AP according to the active profile
 */
void initStaConnection(webs_t* wp)
{
	int i;
        int j, ret;

	// Clear current SSID
	ngx_nvram_set(wp, "staCur_SSID", "");

	PRT_PROFILE_SETTING p = headerProfileSetting;
	if (p == NULL)
		return;
	while (p->Active == 0)
	{
		p = p->Next;
		if (p == NULL)
			return;
	}

	syslog(LOG_ERR, "Activate profile: %s, %s", p->Profile, __FUNCTION__);

	// Set-up current SSID
	ngx_nvram_set(wp, "staCur_SSID", (char*)p->SSID);

	// Fetch current wireless mode
	unsigned long CurrentWirelessMode;
	if (OidQueryInformation(RT_OID_802_11_PHY_MODE, "ra0", &CurrentWirelessMode, sizeof(unsigned char)) < 0 )
	{
		syslog(LOG_ERR, "Query OID_802_11_QUERY_WirelessMode error!, %s", __FUNCTION__);
		return;
	}

	ret = 0;
	// step 0: OID_802_11_INFRASTRUCTURE_MODE
	ret = OidSetInformation(OID_802_11_INFRASTRUCTURE_MODE, "ra0", &p->NetworkType, sizeof(int));
	if (ret < 0)
		syslog(LOG_ERR, "Set OID_802_11_INFRASTRUCTURE_MODE has error!, %s\n", __FUNCTION__);

	// step 1:
	// RTS Threshold
	if (!p->RTSCheck)
		p->RTS = 2347;
	OidSetInformation(OID_802_11_RTS_THRESHOLD, "ra0", &p->RTS, sizeof(NDIS_802_11_RTS_THRESHOLD));

	// Fragment Threshold
	if (!p->FragmentCheck)
		p->Fragment = 2346;
	OidSetInformation(OID_802_11_FRAGMENTATION_THRESHOLD, "ra0", &p->Fragment, sizeof(NDIS_802_11_FRAGMENTATION_THRESHOLD));

	// Network type
	if (p->NetworkType == Ndis802_11Infrastructure)
	{
		OidSetInformation(OID_802_11_POWER_MODE, "ra0", &p->PSmode, sizeof(NDIS_802_11_POWER_MODE));
		OidSetInformation(RT_OID_802_11_PREAMBLE, "ra0", &p->PreamType, sizeof(RT_802_11_PREAMBLE));
	}
	else if (p->NetworkType == Ndis802_11IBSS)
	{
		unsigned long	lFreq = 0;
		NDIS_802_11_CONFIGURATION	Configuration;
		OidQueryInformation(OID_802_11_CONFIGURATION, "ra0", &Configuration, sizeof(Configuration));

		for (i = 0; i < G_nChanFreqCount; i++)
		{
			if (p->Channel == ChannelFreqTable[i].lChannel)
			{
				lFreq = ChannelFreqTable[i].lFreq;
				break;
			}
		}
		if (lFreq != Configuration.DSConfig)
		{
			Configuration.DSConfig = lFreq/1000;
			ret = OidSetInformation(OID_802_11_CONFIGURATION, "ra0", &Configuration, sizeof(Configuration));
			if (ret < 0)
				syslog(LOG_ERR, "Set OID_802_11_CONFIGURATION has error=%d, %s", ret, __FUNCTION__);
		}
	}

	// step 2: Security mode
	// Authentication
	ret = OidSetInformation(OID_802_11_AUTHENTICATION_MODE, "ra0", &p->Authentication, sizeof(p->Authentication));
	if (ret < 0)
		syslog(LOG_ERR, "Set OID_802_11_AUTHENTICATION_MODE has error =%d, auth=%d, %s", ret, p->Authentication, __FUNCTION__);

	// Encryption
	ret = OidSetInformation(OID_802_11_ENCRYPTION_STATUS, "ra0", &p->Encryption, sizeof(p->Encryption));
	if (ret < 0)
		syslog(LOG_ERR, "Set OID_802_11_ENCRYPTION_STATUS has error =%d, encry=%d, %s", ret, p->Encryption, __FUNCTION__);

	// WEP
	if (p->Authentication == Ndis802_11AuthModeOpen)
	{
		if (p->Encryption == Ndis802_11WEPEnabled)
		{
			// Initialize array of WEP keys
			char *wep_keys[4];
			wep_keys[0] = (char *)p->Key1;
			wep_keys[1] = (char *)p->Key2;
			wep_keys[2] = (char *)p->Key3;
			wep_keys[3] = (char *)p->Key4;

			// For each WEP key
			for (i=0; i<4; i++)
			{
				int nKeyLen = strlen(wep_keys[i]);
				if (nKeyLen == 0)
				{
					NDIS_802_11_REMOVE_KEY removeKey;

					removeKey.Length = sizeof(NDIS_802_11_REMOVE_KEY);
					removeKey.KeyIndex = 0;
					for (j = 0; j < 6; j++)
						removeKey.BSSID[j] = 0xff;
					ret = OidSetInformation(OID_802_11_REMOVE_KEY, "ra0", &removeKey, removeKey.Length);
					if (ret < 0)
						syslog(LOG_ERR, "Set OID_802_11_REMOVE_KEY has error =%d, %s", ret, __FUNCTION__);
				}
				else if (strcmp(wep_keys[i], "0") != 0)
				{
					int wep_key_len = nKeyLen;
					if (wep_key_len == 10)
						wep_key_len = 5;
					else if (wep_key_len == 26)
						wep_key_len = 13;

					int lBufLen = sizeof(NDIS_802_11_WEP) + nKeyLen + 1;
					// Allocate Resource
					PNDIS_802_11_WEP pWepKey = (PNDIS_802_11_WEP)malloc(lBufLen);
					bzero(pWepKey, lBufLen);
					pWepKey->Length    = lBufLen - 2;
					pWepKey->KeyLength = wep_key_len;
					pWepKey->KeyIndex  = i;

					if (p->KeyDefaultId == (unsigned int)(i+1))
						pWepKey->KeyIndex |= 0x80000000;

					if (nKeyLen == 5)
						memcpy(pWepKey->KeyMaterial, wep_keys[i], 5);
					else if (nKeyLen == 10)
						AtoH(wep_keys[i], pWepKey->KeyMaterial, 5);
					else if (nKeyLen == 13)
						memcpy(pWepKey->KeyMaterial, wep_keys[i], 13);
					else if (nKeyLen == 26)
						AtoH(wep_keys[i], pWepKey->KeyMaterial, 13);

					OidSetInformation(OID_802_11_ADD_WEP, "ra0", pWepKey, pWepKey->Length);
					free(pWepKey);
				}
			} // end WEP key cycle
		} // WEP encryption
	}
	else if ((p->Authentication == Ndis802_11AuthModeWPAPSK) || 
		(p->Authentication == Ndis802_11AuthModeWPA2PSK) ||
		(p->Authentication == Ndis802_11AuthModeWPANone))
	{
		int nKeyLen = 32;
		int lBufLen = (sizeof(NDIS_802_11_KEY) + nKeyLen - 1);
		unsigned char keyMaterial[40];

		// Allocate Resouce
		PNDIS_802_11_KEY pKey = (PNDIS_802_11_KEY)malloc(lBufLen); // Don't use GMEM_ZEROINIT to get random key
		pKey->Length = lBufLen;
		pKey->KeyLength = nKeyLen;
		pKey->KeyIndex = 0x80000000;

		if (strlen(p->WpaPsk) == 64)
		{
			AtoH(p->WpaPsk, keyMaterial, 32);
			memcpy(pKey->KeyMaterial, keyMaterial, 32);
		}
		else
		{
			PasswordHash(p->WpaPsk, p->SSID, p->SsidLen, keyMaterial);
			memcpy(pKey->KeyMaterial, keyMaterial, 32);
		}

		int PassphraseBufLen = sizeof(NDIS_802_11_PASSPHRASE) + strlen(p->WpaPsk) - 1;
		PNDIS_802_11_PASSPHRASE pPassPhrase=(PNDIS_802_11_PASSPHRASE)malloc(PassphraseBufLen);
		pPassPhrase->KeyLength = strlen(p->WpaPsk);

		memcpy(pPassPhrase->KeyMaterial, p->WpaPsk, pPassPhrase->KeyLength);
		OidSetInformation(OID_802_11_SET_PASSPHRASE, "ra0", pPassPhrase, PassphraseBufLen);
		OidSetInformation(RT_OID_802_11_ADD_WPA, "ra0", pKey, pKey->Length);
		free(pKey);
	}

	//step 3: SSID
	NDIS_802_11_SSID Ssid;
	strcpy((char *)Ssid.Ssid ,(char *)p->SSID);
	Ssid.SsidLength = p->SsidLen;

	if (p->NetworkType == Ndis802_11IBSS ) // Ad hoc use SSID
	{
		ret = OidSetInformation(OID_802_11_SSID, "ra0", &Ssid, sizeof(NDIS_802_11_SSID));
		if (ret < 0)
			syslog(LOG_ERR, "Set OID_802_11_SSID has error =%d, Ssid.Ssid=%s, %s", ret, Ssid.Ssid, __FUNCTION__);
		else
			memcpy(&G_SSID, &Ssid, sizeof(NDIS_802_11_SSID));
	}
	else
	{
		ret = OidSetInformation(OID_802_11_SSID, "ra0", &Ssid, sizeof(NDIS_802_11_SSID));
		if (ret < 0)
			syslog(LOG_ERR, "Set OID_802_11_SSID has error =%d, Ssid.Ssid=%s, %s", ret, Ssid.Ssid, __FUNCTION__);
		else
			memcpy(&G_SSID, &Ssid, sizeof(NDIS_802_11_SSID));

		if (p->Encryption == Ndis802_11WEPEnabled)
		{
			char Bssid[6];
			memset(Bssid, 0x00, sizeof(Bssid));
			AtoH((char*)p->SSID, (unsigned char*)Bssid, sizeof(Bssid));

			memcpy(G_Bssid, Bssid, sizeof(Bssid));
		}
		sleep(1);
	}

	// Configure wan and get param from dhcp and restart all service. Not use wifi only mode
	doSystem("internet.sh connect_sta");
}

static int getActiveProfileStatus(webs_t *wp, char** params, int nparams)

{
	unsigned int ConnectStatus = 0;

	// Check if profiles are read
	if (headerProfileSetting == NULL)
		initStaProfile();

	if (headerProfileSetting == NULL)
		return 0;

	// Find active profile in configuration
	PRT_PROFILE_SETTING p = headerProfileSetting;
	while ((p!= NULL) && (!p->Active))
		p = p->Next;
	if (p == NULL)
		return 0;

	//step 1
	if (OidQueryInformation(OID_GEN_MEDIA_CONNECT_STATUS, "ra0", &ConnectStatus, sizeof(ConnectStatus)) < 0)
	{
		syslog(LOG_ERR, "Query OID_GEN_MEDIA_CONNECT_STATUS error!, %s", __FUNCTION__);
		return 0;
	}

	//step 2
	if (OidQueryInformation(RT_OID_802_11_RADIO, "ra0", &G_bRadio, sizeof(G_bRadio)) < 0)
	{
		syslog(LOG_ERR, "Query RT_OID_802_11_RADIO error!, %s", __FUNCTION__);
		return 0;
	}

	const char *status = "0";

	if ((ConnectStatus == 1) && G_bRadio)
	{
		// Encryption
		NDIS_802_11_WEP_STATUS Encryp = Ndis802_11WEPDisabled;
		OidQueryInformation(OID_802_11_WEP_STATUS, "ra0", &Encryp, sizeof(Encryp) );

		// Auth type
		NDIS_802_11_AUTHENTICATION_MODE AuthenType = Ndis802_11AuthModeOpen;
		OidQueryInformation(OID_802_11_AUTHENTICATION_MODE, "ra0", &AuthenType, sizeof(AuthenType));

		// Network type
		NDIS_802_11_NETWORK_INFRASTRUCTURE NetworkType = Ndis802_11Infrastructure;
		OidQueryInformation(OID_802_11_INFRASTRUCTURE_MODE, "ra0", &NetworkType, sizeof(NetworkType));

		// SSID
		NDIS_802_11_SSID SsidQuery;
		bzero(&SsidQuery, sizeof(SsidQuery));
		OidQueryInformation(OID_802_11_SSID, "ra0", &SsidQuery, sizeof(SsidQuery));

		// Check if parameters match
		if (strcmp((char *)SsidQuery.Ssid, (char *)p->SSID) == 0 &&
			p->Encryption == Encryp &&
			p->Authentication == AuthenType &&
			p->NetworkType == NetworkType)
			status = "1";
	}

	// Write active profile status
	websWrite(wp, "%s", status);

	return 0;
}

typedef struct auth_modes_t
{
	int code;
	const char *desc;
} auth_modes_t;

const auth_modes_t authModes[] =
{
	{ Ndis802_11AuthModeOpen, "OPEN" }, // 0
	{ Ndis802_11AuthModeShared, "SHARED" }, // 1
	{ Ndis802_11AuthModeAutoSwitch, "AUTO" }, // 2
	{ Ndis802_11AuthModeWPA, "WPA" }, // 3
	{ Ndis802_11AuthModeWPAPSK, "WPA-PSK" }, // 4
	{ Ndis802_11AuthModeWPANone, "WPA-NONE" }, // 5
	{ Ndis802_11AuthModeWPA2, "WPA2" }, // 6
	{ Ndis802_11AuthModeWPA2PSK, "WPA2-PSK" }, // 7
	{ Ndis802_11AuthModeWPA1WPA2, "WPA, WPA2" }, // 8
	{ Ndis802_11AuthModeWPA1PSKWPA2PSK, "WPA-PSK, WPA2-PSK" }, // 9
	{ -1, NULL }
};

const auth_modes_t encryptModes[] =
{
	{ Ndis802_11WEPEnabled, "WEP" },
	{ Ndis802_11WEPDisabled, "NONE" },
	{ Ndis802_11Encryption2Enabled, "TKIP" },
	{ Ndis802_11Encryption3Enabled, "AES" },
	{ -1, NULL }
};

static int getStaModes(webs_t* wp, const auth_modes_t *pt)
{
	while (pt->desc != NULL)
	{
		websWrite(wp, "\t'%d': '%s',\n", pt->code, pt->desc);
		pt++;
	}

	return 0;
}

static int getStaAuthModes(webs_t *wp, char** params, int nparams)

{
	return getStaModes(wp, authModes);
}

static int getStaEncryptModes(webs_t *wp, char** params, int nparams)

{
	return getStaModes(wp, encryptModes);
}

/*
 * description: return station radio status
 */
static int getStaRadioStatus(webs_t *wp, char** params, int nparams)

{
#ifdef HAVE_MT76X2_3
	int ret = getWlanHWRadioStatus(1);

	if (ret < 0)
		syslog(LOG_ERR, "getStaRadioStatus: Query RT_OID_802_11_RADIO failed!, %s", __FUNCTION__);

// FIXME
//	ejSetResult(eid, ret ? "1" : "0");
#endif
	return 0;
}

/*
 * description: write station link Rx throughput
 */
static int getStaRxThroughput(webs_t *wp, char** params, int nparams)

{
	RT_802_11_LINK_STATUS LinkStatus;
	char tmp[8];

	if (G_ConnectStatus == NdisMediaStateDisconnected)
		return websWrite(wp, "0");

	// Get Link Status Info from driver
	if (OidQueryInformation(RT_OID_802_11_QUERY_LINK_STATUS, "ra0", &LinkStatus, sizeof(RT_802_11_LINK_STATUS)) < 0)
	{
	    return -1;
	}

	// Rx Throughput (KBits/sec) (LinkStatus.RxByteCount - m_lRxCount) * 8(bits) /1000 / 2(secs)
	if (m_lRxCount != 0)
		snprintf(tmp, 8, "%.1f", (double)(LinkStatus.RxByteCount - m_lRxCount) / 250);
	else
		snprintf(tmp, 8, "%.1f", (double)0);

	websWrite(wp, "%s", tmp);
	m_lRxCount = LinkStatus.RxByteCount;
	return 0;
}

/*
 * description: write station link Tx throughput
 */
static int getStaTxThroughput(webs_t *wp, char** params, int nparams)

{
	RT_802_11_LINK_STATUS LinkStatus;
	char tmp[8];

	if (G_ConnectStatus == NdisMediaStateDisconnected)
		return websWrite(wp, "0");

	// Get Link Status Info from driver
	OidQueryInformation(RT_OID_802_11_QUERY_LINK_STATUS, "ra0", &LinkStatus, sizeof(RT_802_11_LINK_STATUS));

	// Tx Throughput (KBits/sec) (LinkStatus.TxByteCount - m_lTxCount) * 8(bits) /1000 / 2(secs)
	if (m_lTxCount != 0)
		snprintf(tmp, 8, "%.1f", (double)(LinkStatus.TxByteCount - m_lTxCount) / 250);
	else
		snprintf(tmp, 8, "%.1f", (double)0);

	websWrite(wp, "%s", tmp);
	m_lTxCount = LinkStatus.TxByteCount;
	return 0;
}

static int getRSSI(webs_t* wp, int antenna)
{
//	T_802_11_LINK_STATUS LinkStatus;
	unsigned int nSigQua;
	long RSSI;
	int oid[3] = {RT_OID_802_11_RSSI, RT_OID_802_11_RSSI_1, RT_OID_802_11_RSSI_2};

	if (G_ConnectStatus == NdisMediaStateDisconnected) {
		websWrite(wp, "0%%");
		return 0;
	}

	// Get Link Status Info from driver
//	OidQueryInformation(RT_OID_802_11_QUERY_LINK_STATUS, "ra0", &LinkStatus, sizeof(RT_802_11_LINK_STATUS));

	// Signal Strength

	// Get Rssi Value from driver
	if (OidQueryInformation(oid[antenna], "ra0", &RSSI, sizeof(RSSI)) < 0)
	{
	    return -1;
	}

	if (RSSI > 20 || RSSI < -200)
		return websWrite(wp, "None");

	// Use convert formula to getSignal Quality
	nSigQua = ConvertRssiToSignalQuality(RSSI);
	if (m_nSigQua[antenna] != 0)
		nSigQua = (unsigned int)((m_nSigQua[antenna] + nSigQua) / 2.0 + 0.5);

	m_nSigQua[antenna] = nSigQua;
	if (nSigQua > 70) {
		if (G_bdBm_ischeck == 1) { //checked
			return websWrite(wp, "Good &nbsp;&nbsp;&nbsp;&nbsp; %ld dBm", RSSI);
		}
		else {
			return websWrite(wp, "Good &nbsp;&nbsp;&nbsp;&nbsp; %d%%", nSigQua);
		}
	}
	else if (nSigQua > 40) {
		if (G_bdBm_ischeck == 1) { //checked
			return websWrite(wp, "Normal &nbsp;&nbsp;&nbsp;&nbsp; %ld dBm", RSSI);
		}
		else {
			return websWrite(wp, "Normal &nbsp;&nbsp;&nbsp;&nbsp; %d%%", nSigQua);
		}
	}
	else {
		if (G_bdBm_ischeck == 1) { //checked
			return websWrite(wp, "Weak &nbsp;&nbsp;&nbsp;&nbsp; %ld dBm", RSSI);
		}
		else {
			return websWrite(wp, "Weak &nbsp;&nbsp;&nbsp;&nbsp; %d%%", nSigQua);
		}
	}
}

/*
 * description: write station signal strength
 */
static int getStaSignalStrength(webs_t *wp, char** params, int nparams)

{
	return getRSSI(wp, 0);
}

/*
 * description: write station signal strength
 */
static int getStaSignalStrength_1(webs_t *wp, char** params, int nparams)

{
	return getRSSI(wp, 1);
}

/*
 * description: write station signal strength
 */
static int getStaSignalStrength_2(webs_t *wp, char** params, int nparams)

{
	return getRSSI(wp, 2);
}

/*
 * description: write station SNR
 */
static int getStaSNR(webs_t *wp, char** params, int nparams)

{
	int n=0, ret;
	unsigned long SNR;

        if (nparams  < 1)
        {
            return websWrite(wp, T("Insufficient args\n"));
        }

        n = strToIntDef(params[0], 0);

	if (G_ConnectStatus == NdisMediaStateDisconnected) {
		return  websWrite(wp, "n/a");
	}

	if (n == 0)
		ret = OidQueryInformation(RT_OID_802_11_SNR_0, "ra0", &SNR, sizeof(SNR));
	else if (n == 1)
		ret = OidQueryInformation(RT_OID_802_11_SNR_1, "ra0", &SNR, sizeof(SNR));
#ifdef CONFIG_RALINK_RT3883_3T3R
	else if (n == 2)
		ret = OidQueryInformation(RT_OID_802_11_SNR_2, "ra0", &SNR, sizeof(SNR));
#endif
	else
		ret = -1;

	//fprintf(stderr, "SNR%d = %ld\n", n, SNR);
	if (ret < 0)
		return websWrite(wp, "n/a");
	else
		return websWrite(wp, "%ld", SNR);
}

/*
 * description: write station statistics Rx CRC error
 */
static int getStaStatsRxCRCErr(webs_t *wp, char** params, int nparams)

{
	NDIS_802_11_STATISTICS  Statistics;

	memset(&Statistics, 0x00, sizeof(Statistics));

	// Frames Received With CRC Error
	if (OidQueryInformation(OID_802_11_STATISTICS, "ra0", &Statistics, sizeof(Statistics)) >= 0)
		websWrite(wp, "%ld", Statistics.FCSErrorCount.QuadPart);
	else
		websWrite(wp, "0");

	return 0;
}

/*
 * description: write station statistics Rx duplicate
 */
static int getStaStatsRxDup(webs_t *wp, char** params, int nparams)

{
	NDIS_802_11_STATISTICS  Statistics;

	memset(&Statistics, 0x00, sizeof(Statistics));
	// Duplicate Frames Received
	if (OidQueryInformation(OID_802_11_STATISTICS, "ra0", &Statistics, sizeof(Statistics)) >= 0)
		websWrite(wp, "%ld", Statistics.FrameDuplicateCount.QuadPart);
	else
		websWrite(wp, "0");

	return 0;
}

/*
 * description: write station statistics Rx ok
 */
static int getStaStatsRxOk(webs_t *wp, char** params, int nparams)

{
	unsigned long lRcvOk = 0;

	// Frames Received Successfully
	if (OidQueryInformation(OID_GEN_RCV_OK, "ra0", &lRcvOk, sizeof(lRcvOk)) >= 0)
		websWrite(wp, "%ld", lRcvOk);
	else
		websWrite(wp, "0");

	return 0;
}

/*
 * description: write station statistics Rx no buffer
 */
static int getStaStatsRxNoBuf(webs_t *wp, char** params, int nparams)

{
	unsigned long lRcvNoBuf = 0;

	// Frames Dropped Due To Out-of-Resource
	if (OidQueryInformation(OID_GEN_RCV_NO_BUFFER, "ra0", &lRcvNoBuf, sizeof(lRcvNoBuf)) >= 0)
		websWrite(wp, "%ld", lRcvNoBuf);
	else
		websWrite(wp, "0");

	return 0;
}

/*
 * description: write station statistics Tx all
 */
static int getStaStatsTx(webs_t *wp, char** params, int nparams)

{
	NDIS_802_11_STATISTICS  Statistics;
	char  tmpStatisics[16];
	int   ret=0;

	memset(&tmpStatisics, 0x00, sizeof(tmpStatisics));

	// Transmit Section
	memset(&Statistics, 0x00, sizeof(Statistics));
	ret = OidQueryInformation(OID_802_11_STATISTICS, "ra0", &Statistics, sizeof(Statistics));

	if (ret >= 0)
	{
		// Frames Transmitted Successfully
		sprintf(tmpStatisics, "%8lld", Statistics.TransmittedFragmentCount.QuadPart);
		websWrite(wp, "<tr><td class=\"head\">%s</td><td>%s</td></tr>", "Frames Transmitted Successfully", tmpStatisics);

		// Frames Transmitted Successfully  Without Retry(s)
		sprintf(tmpStatisics, "%8lld", Statistics.TransmittedFragmentCount.QuadPart - Statistics.RetryCount.QuadPart);
		websWrite(wp, "<tr><td class=\"head\">%s</td><td>%s</td></tr>", "Frames Transmitted Successfully Without Retry", tmpStatisics);

		// Frames Transmitted Successfully After Retry(s)
		sprintf(tmpStatisics, "%8lld", Statistics.RetryCount.QuadPart);
		websWrite(wp, "<tr><td class=\"head\">%s</td><td>%s</td></tr>", "Frames Transmitted Successfully After Retry(s)", tmpStatisics);

		// Frames Fail To Receive ACK After All Retries
		sprintf(tmpStatisics, "%8lld", Statistics.FailedCount.QuadPart);
		websWrite(wp, "<tr><td class=\"head\">%s</td><td>%s</td></tr>", "Frames Fail To Receive ACK After All Retries", tmpStatisics);

		// RTS Frames Successfully Receive CTS
		sprintf(tmpStatisics, "%8lld", Statistics.RTSSuccessCount.QuadPart);
		websWrite(wp, "<tr><td class=\"head\">%s</td><td>%s</td></tr>", "RTS Frames Sucessfully Receive CTS", tmpStatisics);

		// RTS Frames Fail To Receive CTS
		sprintf(tmpStatisics, "%8lld", Statistics.RTSFailureCount.QuadPart);
		websWrite(wp, "<tr><td class=\"head\">%s</td><td>%s</td></tr>", "RTS Frames Fail To Receive CTS", tmpStatisics);

		// Frames Received Successfully
		sprintf(tmpStatisics, "%8lld", Statistics.ReceivedFragmentCount.QuadPart);
		websWrite(wp, "<tr><td class=\"title\" colspan=2 id=statisticRx>Receive Statistics</td></tr>");
		websWrite(wp, "<tr><td class=\"head\">%s</td><td>%s</td></tr>", "Frames Received Successfully", tmpStatisics);
		return 0;
	}
	else
		return websWrite(wp, "<tr><td>no data</td></tr>");
}

static int myGetSuppAMode(void)
{
	unsigned long lBufLen = sizeof(NDIS_802_11_NETWORK_TYPE_LIST) + sizeof(NDIS_802_11_NETWORK_TYPE)*3 ;
	PNDIS_802_11_NETWORK_TYPE_LIST pNetworkTypeList = (PNDIS_802_11_NETWORK_TYPE_LIST) malloc(lBufLen);
	unsigned int i;
        int G_bSupportAMode=0;

	if (OidQueryInformation(OID_802_11_NETWORK_TYPES_SUPPORTED, "ra0", pNetworkTypeList, lBufLen) >= 0)
	{
		for (i = 0 ; i < pNetworkTypeList->NumberOfItems ; i++)
		{
			if ( pNetworkTypeList->NetworkType[i] == Ndis802_11OFDM5 )
			{
				G_bSupportAMode = 1;
				break;
			}
		}
	}
	free(pNetworkTypeList);
	return G_bSupportAMode;
}

/*
 * description: return station support A band
 */
static int getStaSuppAMode(webs_t *wp, char** params, int nparams)

{
//FIXME
//	ejSetResult(eid, (myGetSuppAMode() == 1) ? "1" : "0");
	return 0;
}

/*
 * description: write station wireless mode
 */
static int getStaWirelessMode(webs_t *wp, char** params, int nparams)

{
	int opmode = nvram_get_int(RT2860_NVRAM, "OperationMode",0);
	int mode = nvram_get_int(RT2860_NVRAM, "WirelessMode", 0);
	int bSuppA = myGetSuppAMode();
	int StaOn = (opmode == 2);

	websWrite(wp, "<option value=\"0\" %s>802.11 B/G mixed mode</option>", (mode == 0)? "selected" : "");
	websWrite(wp, "<option value=\"1\" %s>802.11 B Only</option>", (mode == 1)? "selected" : "");
	if (bSuppA) {
		websWrite(wp, "<option value=\"2\" %s>802.11 A Only</option>", (mode == 2)? "selected" : "");
		websWrite(wp, "<option value=\"3\" %s>802.11 A/B/G mixed mode</option>", (mode == 3)? "selected" : "");
	}
	websWrite(wp, "<option value=\"4\" %s>802.11 G Only</option>", (mode == 4)? "selected" : "");
	websWrite(wp, "<option value=\"6\" %s>802.11 N Only</option>", (mode == 6)? "selected" : "");
	if (!StaOn)
		websWrite(wp, "<option value=\"7\" %s>802.11 GN mixed mode</option>", (mode == 7)? "selected" : "");
	if (bSuppA) {
		websWrite(wp, "<option value=\"8\" %s>802.11 AN mixed mode</option>", (mode == 8)? "selected" : "");
	}

	websWrite(wp, "<option value=\"9\" %s>802.11 B/G/N mixed mode</option>", (mode == 9)? "selected" : "");
	if (bSuppA) {
		if (!StaOn)
			websWrite(wp, "<option value=\"10\" %s>802.11 A/G/N mixed mode</option>", (mode == 10) ? "selected" : "");
		websWrite(wp, "<option value=\"5\" %s>802.11 A/B/G/N mixed mode</option>", (mode == 5)? "selected" : "");
	}

	return 0;
}

/*
 * description: goform - reset statistics counters
 */
static void resetStaCounters(webs_t* wp, char_t *path, char_t *query)
{
	OidSetInformation(RT_OID_802_11_RESET_COUNTERS, "ra0", 0, 0);
	websRedirect(wp, "station/statistics.asp");
	return;
}

/*
 * description: goform - set 11n configuration
 */
static void setSta11nCfg(webs_t* wp, char_t *path, char_t *query)
{
	char_t *a_mpdu_enable, *autoBA, *mpdu_density, *a_msdu_enable;
	int policy;
	OID_BACAP_STRUC BACap;

	a_mpdu_enable = websGetVar(wp, T("a_mpdu_enable"), T("off"));
	autoBA = websGetVar(wp, T("autoBA"), T("0"));
	mpdu_density = websGetVar(wp, T("mpdu_density"), T("0"));
	a_msdu_enable = websGetVar(wp, T("a_msdu_enable"), T("off"));

	nvram_init(RT2860_NVRAM);
	if (!strcmp(a_mpdu_enable, "on"))
	{
		policy = 1;
		ngx_nvram_bufset(wp, "staPolicy", "1"); //FIXME: typo?
	}
	else
	{
		policy = 0;
		ngx_nvram_bufset(wp, "staPolicy", "0");
	}
	ngx_nvram_bufset(wp, "HT_AutoBA", autoBA);
	ngx_nvram_bufset(wp, "HT_MpduDensity", mpdu_density);
	ngx_nvram_bufset(wp, "HT_AMSDU", strcmp(a_msdu_enable, "off")? "1":"0");
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	if (OidQueryInformation(RT_OID_802_11_QUERY_IMME_BA_CAP, "ra0", &BACap, sizeof(BACap)) < 0)
	{
	    syslog(LOG_ERR, "oid query failed, %s", __FUNCTION__);
	    return;
	}

	BACap.Policy = policy;
	BACap.AutoBA = atoi(autoBA);
	BACap.MpduDensity = atoi(mpdu_density);
	if (!strcmp(a_msdu_enable, "on"))
			BACap.AmsduEnable = 1;

	OidSetInformation(RT_OID_802_11_SET_IMME_BA_CAP, "ra0", &BACap, sizeof(BACap));
}

void StartStaConnect(webs_t* wp)
{
	int opmode = nvram_get_int(RT2860_NVRAM, "OperationMode", 0);

	/* automatically connect to AP according to the active profile */
	if (opmode == 2) {
	    initStaProfile();
	    initStaConnection(wp);
	}
}

/*
 * description: goform - set advanced configuration
 */
static void setStaAdvance(webs_t* wp, char_t *path, char_t *query)
{
	char_t *w_mode, *cr_bg, *cr_a, *bg_prot, *burst;
	char_t *ht, *bw, *gi, *mcs, *tx_power, *sta_ar, *sta_ac, *sta_fc;
	int ret, wireless_mode=0;

	unsigned char radio_status=0;

	w_mode = websGetVar(wp, T("wireless_mode"), T("0"));
	wireless_mode = atoi(w_mode);
	cr_bg = websGetVar(wp, T("country_region_bg"), T("0"));
	cr_a = websGetVar(wp, T("country_region_a"), T("0"));
	bg_prot = websGetVar(wp, T("bg_protection"), T("0"));
	burst = websGetVar(wp, T("tx_burst"), T("off"));
	ht = websGetVar(wp, T("n_mode"), T("0"));
	bw = websGetVar(wp, T("n_bandwidth"), T("0"));
	gi = websGetVar(wp, T("n_gi"), T("0"));
	mcs = websGetVar(wp, T("n_mcs"), T("0"));
	tx_power = websGetVar(wp, T("tx_power"), T("100"));
	sta_ar = websGetVar(wp, T("staAutoRoaming"), T("off"));
	sta_ac = websGetVar(wp, T("staAutoConnect"), T("off"));
	sta_fc = websGetVar(wp, T("staFastConnect"), T("off"));
	char_t *tx_stream = websGetVar(wp, T("tx_stream"), T("1"));
	char_t *rx_stream = websGetVar(wp, T("rx_stream"), T("1"));

        // mac clone && sta roaming atts
	char_t *clone_en = websGetVar(wp, T("macCloneEnbl"), T("0"));
	char_t *clone_mac = websGetVar(wp, T("macCloneMac"), T(""));

	OidSetInformation(RT_OID_802_11_RADIO, "ra0", &radio_status, sizeof(radio_status));

	ret = OidSetInformation(OID_802_11_BSSID_LIST_SCAN, "ra0", 0, 0);
	if (ret < 0)
		syslog(LOG_ERR, "Set OID_802_11_BSSID_LIST_SCAN error = %d, %s", ret, __FUNCTION__);

	sleep(3);
	if (G_SSID.SsidLength > 0)
	{
		ret = OidSetInformation(OID_802_11_SSID, "ra0", &G_SSID, sizeof(NDIS_802_11_SSID));
		if (ret < 0)
			syslog(LOG_ERR, "Set OID_802_11_SSID error = %d, %s", ret, __FUNCTION__);
	}

	nvram_init(RT2860_NVRAM);
	ngx_nvram_bufset(wp, "macCloneEnabled", clone_en);
	if (!strncmp(clone_en, "1", 2))
		ngx_nvram_bufset(wp, "macCloneMac", clone_mac);
	else
		ngx_nvram_bufset(wp, "macCloneMac", "");

	ngx_nvram_bufset(wp, "AutoRoaming", (strcmp(sta_ar, "on")==0) ? "1" : "0");
	ngx_nvram_bufset(wp, "AutoConnect", (strcmp(sta_ac, "on")==0) ? "1" : "0");
	ngx_nvram_bufset(wp, "FastConnect", (strcmp(sta_fc, "on")==0) ? "1" : "0");
	ngx_nvram_bufset(wp, "WirelessMode", w_mode);
	ngx_nvram_bufset(wp, "CountryRegion", cr_bg);
	ngx_nvram_bufset(wp, "CountryRegionABand", cr_a);
	ngx_nvram_bufset(wp, "BGProtection", bg_prot);
	ngx_nvram_bufset(wp, "TxPower", tx_power);
	ngx_nvram_bufset(wp, "HT_TxStream", tx_stream);
	ngx_nvram_bufset(wp, "HT_RxStream", rx_stream);

	if (!strncmp(burst, "on", 3))
		ngx_nvram_bufset(wp, "TxBurst", "1");
	else
		ngx_nvram_bufset(wp, "TxBurst", "0");

	ngx_nvram_bufset(wp, "HT_OpMode", ht);
	ngx_nvram_bufset(wp, "HT_BW", bw);
	ngx_nvram_bufset(wp, "HT_GI", gi);
	ngx_nvram_bufset(wp, "HT_MCS", mcs);
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	//set-up 11N configuration
	setSta11nCfg(wp, path, query);

	//refresh station profiles
	initStaProfile();

	//push wifi config to config mtd part and config generate
	doSystem("service modules gen_wifi_config > /dev/console 2>&1");

	// reconnect to AP and renew dhcp, pppoe etc
	initStaConnection(wp);

	websRedirect(wp,"station/advance.asp");
	return;
}

/*
 * description: goform - set G_bdBm_ischeck (displaying dbm or % type)
 */
static void setStaDbm(webs_t* wp, char_t *path, char_t *query)
{
	char_t *dbm;

	dbm = websGetVar(wp, T("dbmChecked"), T("off"));
	if (!strncmp(dbm, "on", 3))
		G_bdBm_ischeck = 1;
	else
		G_bdBm_ischeck = 0;
	websRedirect(wp, "station/link_status.asp");
	return;
}

parameter_fetch_t sta_profile_args[] =
{
	{ T("staProfile"),              "staProfile",           0,      T("") },
	{ T("staSSID"),                 "staSSID",              0,      T("") },
	{ T("staNetworkType"),          "staNetworkType",       0,      T("") },
	{ T("staPSMode"),               "staPSMode",            0,      T("") },
	{ T("staAdhocMode"),            "staAdhocMode",         0,      T("") },
	{ T("staChannel"),              "staChannel",           0,      T("") },
	{ T("staPreamType"),            "staPreamType",         0,      T("") },
	{ T("staRTSCheck"),             "staRTSCheck",          0,      T("") },
	{ T("staFragmentCheck"),        "staFragmentCheck",     0,      T("") },
	{ T("staRTS"),                  "staRTS",               0,      T("") },
	{ T("staFragment"),             "staFragment",          0,      T("") },
	{ T("staAuth"),                 "staAuth",              0,      T("") },
	{ T("staEncrypt"),              "staEncrypt",           0,      T("") },
	{ T("staKeyDefaultId"),         "staKeyDefaultId",      0,      T("") },
	{ T("staKey1Type"),             "staKey1Type",          0,      T("") },
	{ T("staKey2Type"),             "staKey2Type",          0,      T("") },
	{ T("staKey3Type"),             "staKey3Type",          0,      T("") },
	{ T("staKey4Type"),             "staKey4Type",          0,      T("") },
	{ T("staKey1Length"),           "staKey1Length",        0,      T("") },
	{ T("staKey2Length"),           "staKey2Length",        0,      T("") },
	{ T("staKey3Length"),           "staKey3Length",        0,      T("") },
	{ T("staKey4Length"),           "staKey4Length",        0,      T("") },
	{ T("staKey1"),                 "staKey1",              0,      T("") },
	{ T("staKey2"),                 "staKey2",              0,      T("") },
	{ T("staKey3"),                 "staKey3",              0,      T("") },
	{ T("staKey4"),                 "staKey4",              0,      T("") },
	{ T("staWpaPsk"),               "staWpaPsk",            0,      T("") },
	{ T("sta8021xKeyMgmt"),         "sta8021xKeyMgmt",      0,      T("") },
	{ T("sta8021xEAP"),             "sta8021xEAP",          0,      T("") },
	{ T("sta8021xTunnel"),          "sta8021xTunnel",       0,      T("") },
	{ T("sta8021xIdentity"),        "sta8021xIdentity",     0,      T("") },
	{ T("sta8021xPassword"),        "sta8021xPassword",     0,      T("") },
	{ T("sta8021xClientCert"),      "sta8021xClientCert",   0,      T("") },
	{ T("sta8021xPrivateKey"),      "sta8021xPrivateKey",   0,      T("") },
	{ T("sta8021xPrivateKeyPassword"),              "sta8021xPrivateKeyPassword",           0,      T("") },
	{ T("sta8021xCACert"),          "sta8021xCACert",       0,      T("") },
	{ T("staActive"),               "staActive",            0,      T("") },
	{ NULL, NULL, 0, NULL }
};

static void setStaProfile(webs_t* wp, char_t *path, char_t *query)
{
	// Store parameters
	setupParameters(wp, sta_profile_args, 1);

	// Initialize profiles
	initStaProfile();

	// Initialize STA connection
	initStaConnection(wp);

	// Redirect to success
	websRedirect(wp, "station/profile.asp");
}

/*
 * description: goform - add ampdu originator
 */
static void setStaOrgAdd(webs_t* wp, char_t *path, char_t *query)
{
	char_t *tid, *win_sz, *sbssid, *mac;
	char setflag = 0;
	unsigned char Bssid[6];
	OID_ADD_BA_ENTRY oriEntry;

	tid = websGetVar(wp, T("tid"), T("0"));
	win_sz = websGetVar(wp, T("ba_window_size"), T("32"));
	sbssid = websGetVar(wp, T("selectedbssid"), T("0"));
	mac = websGetVar(wp, T("mac"), T("0"));
	nConfig_flag = 1;

	if (strncmp(sbssid, "0", 2))
	{
		int i; char *tok;
		for (i = 0, tok = strtok(sbssid, ":");
				(i < 5) && tok;
				i++, tok = strtok(NULL, ":"))
		{
			Bssid[i] = (unsigned char)strtoul(tok, (char **)NULL, 16);
		}
	}

	//FIXME: selectedbssid, mac might be different
	if (!strncmp(mac, "on", 3))
		setflag = 1;

	if (setflag)
	{
		memcpy(oriEntry.MACAddr, Bssid, 6);
		oriEntry.IsRecipient = 0; //false
		oriEntry.BufSize = (unsigned char)atoi(win_sz);
		oriEntry.TID = (unsigned char)atoi(tid);
		oriEntry.TimeOut = 0;
		oriEntry.AllTid = 0; //false

		OidSetInformation(RT_OID_802_11_ADD_IMME_BA, "ra0", &oriEntry, sizeof(oriEntry));
	}
}

/*
 * description: goform - delete ampdu originator
 */
static void setStaOrgDel(webs_t* wp, char_t *path, char_t *query)
{
	char_t *button;

	button = websGetVar(wp, T("hiddenButton"), T("0"));
}

static int getStaTrModes(webs_t *wp, char** params, int nparams)

{
	char_t *result= "0";
#if defined(CONFIG_RT_FIRST_IF_MT7610E)
	result = "1";
#elif defined(CONFIG_RT_FIRST_IF_MT7620) || defined(CONFIG_RT_FIRST_IF_MT7602E) || defined(CONFIG_RT_FIRST_IF_MT7603E)
	result = "2";
#elif defined(CONFIG_RT_FIRST_IF_MT7603E)
	result = "3";
#else
	result = "1";
#endif
	return websWrite(wp, T("%s"), result);
}

void asp_mod_station_init(void)
{
	aspDefineFunc(T("getCACLCertList"), getCACLCertList, EVERYONE);
	aspDefineFunc(T("getKeyCertList"), getKeyCertList, EVERYONE);
	aspDefineFunc(T("getStaAdhocChannel"), getStaAdhocChannel, EVERYONE);
	aspDefineFunc(T("getStaBSSIDList"), getStaBSSIDList, EVERYONE);
	aspDefineFunc(T("getStaConnectedBSSID"), getStaConnectedBSSID, EVERYONE);
	aspDefineFunc(T("getStaDbm"), getStaDbm, EVERYONE);
	aspDefineFunc(T("getStaExtraInfo"), getStaExtraInfo, EVERYONE);
	aspDefineFunc(T("getLinkingMode"), getLinkingMode, EVERYONE);
	aspDefineFunc(T("getStaHT"), getStaHT, EVERYONE);
	aspDefineFunc(T("getStaLinkChannel"), getStaLinkChannel, EVERYONE);
	aspDefineFunc(T("getStaLinkQuality"), getStaLinkQuality, EVERYONE);
	aspDefineFunc(T("getStaLinkRxRate"), getStaLinkRxRate, EVERYONE);
	aspDefineFunc(T("getStaLinkStatus"), getStaLinkStatus, EVERYONE);
	aspDefineFunc(T("getStaLinkTxRate"), getStaLinkTxRate, EVERYONE);
	aspDefineFunc(T("getStaNewProfileName"), getStaNewProfileName, EVERYONE);
	aspDefineFunc(T("getStaNoiseLevel"), getStaNoiseLevel, EVERYONE);
	aspDefineFunc(T("getActiveProfileStatus"), getActiveProfileStatus, EVERYONE);
	aspDefineFunc(T("getStaAuthModes"), getStaAuthModes, EVERYONE);
	aspDefineFunc(T("getStaEncryptModes"), getStaEncryptModes, EVERYONE);
	aspDefineFunc(T("getStaRadioStatus"), getStaRadioStatus, EVERYONE);
	aspDefineFunc(T("getStaRxThroughput"), getStaRxThroughput, EVERYONE);
	aspDefineFunc(T("getStaTxThroughput"), getStaTxThroughput, EVERYONE);
	aspDefineFunc(T("getStaSignalStrength"), getStaSignalStrength, EVERYONE);
	aspDefineFunc(T("getStaSignalStrength_1"), getStaSignalStrength_1, EVERYONE);
	aspDefineFunc(T("getStaSignalStrength_2"), getStaSignalStrength_2, EVERYONE);
	aspDefineFunc(T("getStaSNR"), getStaSNR, EVERYONE);
	aspDefineFunc(T("getStaStatsRxCRCErr"), getStaStatsRxCRCErr, EVERYONE);
	aspDefineFunc(T("getStaStatsRxDup"), getStaStatsRxDup, EVERYONE);
	aspDefineFunc(T("getStaStatsRxOk"), getStaStatsRxOk, EVERYONE);
	aspDefineFunc(T("getStaStatsRxNoBuf"), getStaStatsRxNoBuf, EVERYONE);
	aspDefineFunc(T("getStaStatsTx"), getStaStatsTx, EVERYONE);
	aspDefineFunc(T("getStaSuppAMode"), getStaSuppAMode, EVERYONE);
	aspDefineFunc(T("getStaWirelessMode"), getStaWirelessMode, EVERYONE);
	aspDefineFunc(T("getStaTrModes"), getStaTrModes, EVERYONE);

	websFormDefine(T("resetStaCounters"), resetStaCounters, EVERYONE);
	websFormDefine(T("setStaAdvance"), setStaAdvance, EVERYONE);
	websFormDefine(T("setStaDbm"), setStaDbm, EVERYONE);
	websFormDefine(T("setStaProfile"), setStaProfile, EVERYONE);
	websFormDefine(T("setStaOrgAdd"), setStaOrgAdd, EVERYONE);
	websFormDefine(T("setStaOrgDel"), setStaOrgDel, EVERYONE);
}
