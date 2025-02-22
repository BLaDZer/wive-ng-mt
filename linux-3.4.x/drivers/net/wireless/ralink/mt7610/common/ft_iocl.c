/****************************************************************************
 * Ralink Tech Inc.
 * Taiwan, R.O.C.
 *
 * (c) Copyright 2002, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************/

/****************************************************************************

	Abstract:

	All related IEEE802.11r IOCTL function body.

***************************************************************************/

#ifdef DOT11R_FT_SUPPORT



#include "rt_config.h"

/* ----- Private Variable ----- */
#ifdef FT_FUNC_SIMULATION
static UCHAR gFT_MAC_STA[ETH_ALEN] = { 0x00, 0x0e, 0x2e, 0x82, 0xe7, 0x6d };
UCHAR gFT_MAC_OldAP[ETH_ALEN] = { 0x00, 0x0e, 0x2e, 0x12, 0x34, 0x56 };
#endif /* FT_FUNC_SIMULATION */


/* ----- Extern Function ----- */
#ifdef CONFIG_AP_SUPPORT
extern BOOLEAN FT_KDP_R0KH_InfoAdd(
	IN	PRTMP_ADAPTER		pAd,
	IN	UCHAR				*pR0KHID,
	IN	UCHAR				*pMAC,
	IN	UINT32				IP);
#endif /* CONFIG_AP_SUPPORT */


/* ----- Private Function ----- */
#define FT_CMD_INPUT_PARAM_DECLARATION	\
	PRTMP_ADAPTER pAd, INT32 Argc, CHAR *pArgv

#ifdef CONFIG_AP_SUPPORT
#ifndef FT_KDP_FUNC_SOCK_COMM
static VOID FT_KDP_CMD_EventList(FT_CMD_INPUT_PARAM_DECLARATION);
#endif /* FT_KDP_FUNC_SOCK_COMM */

static VOID FT_KDP_CMD_DbgFlagCtrl(FT_CMD_INPUT_PARAM_DECLARATION);

#ifdef FT_FUNC_SIMULATION
static VOID FT_KDP_CMD_SimEvtFtAssoc(FT_CMD_INPUT_PARAM_DECLARATION);
static VOID FT_KDP_CMD_SimEvtFtReAssoc(FT_CMD_INPUT_PARAM_DECLARATION);
static VOID FT_KDP_CMD_SimKeyReq(FT_CMD_INPUT_PARAM_DECLARATION);

#ifdef FT_KDP_FUNC_R0KH_IP_RECORD
static VOID FT_KDP_CMD_SimR0KH_InfoCreate(FT_CMD_INPUT_PARAM_DECLARATION);
#endif /* FT_KDP_FUNC_R0KH_IP_RECORD */

static VOID FT_RIC_CMD_SimRscReqHdlTspec(FT_CMD_INPUT_PARAM_DECLARATION);
static VOID FT_RIC_CMD_SimRscReqHandle(FT_CMD_INPUT_PARAM_DECLARATION);
static VOID FT_RRB_CMD_SimSend(FT_CMD_INPUT_PARAM_DECLARATION);
static VOID FT_11K_CMD_SimInfoReq(FT_CMD_INPUT_PARAM_DECLARATION);
static VOID FT_11K_CMD_SimKeyShow(FT_CMD_INPUT_PARAM_DECLARATION);
#endif /* FT_FUNC_SIMULATION */
#endif /* CONFIG_AP_SUPPORT */

static UINT32 FT_CMD_UtilHexGet(CHAR **ppArgv);
static UINT32 FT_CMD_UtilNumGet(CHAR **ppArgv);
static VOID FT_CMD_UtilMacGet(CHAR **ppArgv, UCHAR *pDevMac);

#ifdef CONFIG_AP_SUPPORT
void test11r(PRTMP_ADAPTER pAd)
{
	FT_KDP_CMD_SimEvtFtAssoc(pAd, 0, NULL);
}
#endif /* CONFIG_AP_SUPPORT */

/*
========================================================================
Routine Description:
	Get argument number value.

Arguments:
	**ppArgv			- input parameters

Return Value:
	decimal number

Note:
========================================================================
*/
static UINT32 TYPE_FUNC FT_CMD_UtilHexGet(
	IN	CHAR	**ppArgv)
{
	CHAR buf[3], *pNum;
	UINT32 ID;
	UCHAR Value;


	pNum = (*ppArgv);

	buf[0] = 0x30;
	buf[1] = 0x30;
	buf[2] = 0;

	for(ID=0; ID<sizeof(buf)-1; ID++)
	{
		if ((*pNum == '_') || (*pNum == 0x00))
			break;
		/* End of if */

		pNum ++;
	} /* End of for */

	if (ID == 0)
		return 0; /* argument length is too small */
	/* End of if */

	if (ID >= 2)
		memcpy(buf, (*ppArgv), 2);
	else
		buf[1] = (**ppArgv);
	/* End of if */

	(*ppArgv) += ID;
	if ((**ppArgv) == '_')
		(*ppArgv) ++; /* skip _ */
	/* End of if */

	FT_ARG_ATOH(buf, &Value);
	return (UINT32)Value;
} /* End of FT_CMD_UtilHexGet */


/*
========================================================================
Routine Description:
	Get argument number value.

Arguments:
	*pArgv			- input parameters

Return Value:
	decimal number

Note:
========================================================================
*/
static UINT32 TYPE_FUNC FT_CMD_UtilNumGet(
	IN	CHAR	**ppArgv)
{
	CHAR buf[20], *pNum;
	UINT32 ID;


	pNum = (*ppArgv);

	for(ID=0; ID<sizeof(buf)-1; ID++)
	{
		if ((*pNum == '_') || (*pNum == 0x00))
			break;
		/* End of if */

		pNum ++;
	} /* End of for */

	if (ID == sizeof(buf)-1)
		return 0; /* argument length is too large */
	/* End of if */

	memcpy(buf, (*ppArgv), ID);
	buf[ID] = 0x00;

	*ppArgv += ID+1; /* skip _ */

	return FT_ARG_ATOI(buf);
} /* End of FT_CMD_UtilNumGet */


/*
========================================================================
Routine Description:
	Get argument MAC value.

Arguments:
	**ppArgv			- input parameters
	*pDevMac			- MAC address

Return Value:
	None

Note:
========================================================================
*/
static VOID TYPE_FUNC FT_CMD_UtilMacGet(
	IN	CHAR	**ppArgv,
	IN	UCHAR	*pDevMac)
{
	CHAR Buf[3];
	CHAR *pMAC = (CHAR *)(*ppArgv);
	UINT32 ID;


	if ((pMAC[0] == '0') && (pMAC[1] == '_'))
	{
		*ppArgv = (&pMAC[2]);
		return;
	} /* End of if */

	memset(pDevMac, 0, ETH_ALEN);

	/* must exist 18 octets */
	for(ID=0; ID<18; ID+=2)
	{
		if ((pMAC[ID] == '_') || (pMAC[ID] == 0x00))
		{
			*ppArgv = (&pMAC[ID]+1);
			return;
		} /* End of if */
	} /* End of for */

	/* get mac */
	for(ID=0; ID<18; ID+=3)
	{
		Buf[0] = pMAC[0];
		Buf[1] = pMAC[1];
		Buf[2] = 0x00;

		FT_ARG_ATOH(Buf, pDevMac);
		pMAC += 3;
		pDevMac ++;
	} /* End of for */

	*ppArgv += 17+1; /* skip _ */
} /* End of FT_CMD_UtilMacGet */


#ifdef CONFIG_AP_SUPPORT
#ifdef FT_KDP_FUNC_R0KH_IP_RECORD
/*
========================================================================
Routine Description:
	Display all R0KH information.

Arguments:
	pAd				- WLAN control block pointer
	*pArgv			- input parameters

Return Value:
	TRUE

Note:
========================================================================
*/
INT TYPE_FUNC FT_KDP_CMD_R0KH_InfoShow(
	IN	PRTMP_ADAPTER		pAd,
	IN	PSTRING				pArgv)
{
	FT_KDP_R0KH_INFO *pInfo;
	UINT32 IdInfo, IdArray;


	RTMP_SEM_LOCK(&(pAd->ApCfg.FtTab.FT_KdpLock));

	pInfo = FT_KDP_CB->R0KH_InfoHead;
	IdInfo = 1;

	while(pInfo != NULL)
	{
		DBGPRINT_RAW(RT_DEBUG_OFF, ("\n%03d. R0KHID = 0x", IdInfo));

		for(IdArray=0; IdArray<sizeof(pInfo->R0KHID); IdArray++)
		{
			if (IdArray == (sizeof(pInfo->R0KHID)>>1))
				DBGPRINT_RAW(RT_DEBUG_OFF, ("\n                "));
			/* End of if */

			DBGPRINT_RAW(RT_DEBUG_OFF, (" %02x", pInfo->R0KHID[IdArray]));
		} /* End of for */

		DBGPRINT_RAW(RT_DEBUG_OFF,
				("\n%03d. MAC = 0x%02x:%02x:%02x:%02x:%02x:%02x\n",
				IdInfo, pInfo->MAC[0], pInfo->MAC[1], pInfo->MAC[2],
				pInfo->MAC[3], pInfo->MAC[4], pInfo->MAC[5]));

		DBGPRINT_RAW(RT_DEBUG_OFF,
				("%03d. IP  = %d.%d.%d.%d\n", IdInfo,
				(pInfo->IP & 0x000000FF) >> 0,
				(pInfo->IP & 0x0000FF00) >> 8,
				(pInfo->IP & 0x00FF0000) >> 16,
				(pInfo->IP & 0xFF000000) >> 24));

		pInfo = pInfo->pNext;
		IdInfo ++;
	} /* End of while */

	RTMP_SEM_UNLOCK(&(pAd->ApCfg.FtTab.FT_KdpLock));
	DBGPRINT_RAW(RT_DEBUG_OFF, ("\n"));
	return TRUE;
} /* End of FT_KDP_CMD_R0KH_InfoShow */
#endif /* FT_KDP_FUNC_R0KH_IP_RECORD */


#ifndef FT_KDP_FUNC_SOCK_COMM
/*
========================================================================
Routine Description:
	List all queued events.

Arguments:
	pAd				- WLAN control block pointer
	Argc			- the number of input parameters
	*pArgv			- input parameters

Return Value:
	None

Note:
========================================================================
*/
static VOID TYPE_FUNC FT_KDP_CMD_EventList(
	IN	PRTMP_ADAPTER		pAd,
	IN	INT32				Argc,
	IN	CHAR				*pArgv)
{
	FT_KDP_SIGNAL *pFtKdp;
	ULONG SplFlags;


	pFtKdp = (FT_KDP_SIGNAL *)FT_KDP_CB->EventList.pHead;
	if (pFtKdp == NULL)
	{
		DBGPRINT_RAW(RT_DEBUG_OFF, ("ft_kdp> No any event!\n"));
		return;
	} /* End of if */

	DBGPRINT_RAW(RT_DEBUG_OFF, ("\nEvent\n"));
	DBGPRINT_RAW(RT_DEBUG_OFF, ("----------------------------------------------------\n"));

	RTMP_SEM_LOCK(&(pAd->ApCfg.FtTab.FT_KdpLock));

	while(pFtKdp != NULL)
	{
		switch(pFtKdp->Sig)
		{
			case FT_KDP_SIG_KEY_TIMEOUT:
				DBGPRINT_RAW(RT_DEBUG_OFF, ("KEY TIMEOUT\n"));
				break;

			case FT_KDP_SIG_KEY_REQ:
				DBGPRINT_RAW(RT_DEBUG_OFF, ("KEY REQUEST\n"));
				break;

			case FT_KDP_SIG_FT_ASSOCIATION:
				DBGPRINT_RAW(RT_DEBUG_OFF, ("STATION FT ASSOCIATION\n"));
				break;

			case FT_KDP_SIG_TERMINATE:
				DBGPRINT_RAW(RT_DEBUG_OFF, ("TERMINATE\n"));
				break;

			default:
				DBGPRINT_RAW(RT_DEBUG_OFF, ("UNKNOWN\n"));
				break;
		} /* End of switch */

		pFtKdp = (FT_KDP_SIGNAL *)pFtKdp->pNext;
	} /* End of while  */

	RTMP_SEM_UNLOCK(&(pAd->ApCfg.FtTab.FT_KdpLock));
	DBGPRINT_RAW(RT_DEBUG_OFF, ("\n"));
} /* End of FT_KDP_CMD_EventList */
#endif /* FT_KDP_FUNC_SOCK_COMM */


/*
========================================================================
Routine Description:
	Change the debug flag of IAPP daemon.

Arguments:
	pAd				- WLAN control block pointer
	Argc			- the number of input parameters
	*pArgv			- input parameters

Return Value:
	None

Note:
========================================================================
*/
static VOID FT_KDP_CMD_DbgFlagCtrl(
	IN	PRTMP_ADAPTER		pAd,
	IN	INT32				Argc,
	IN	CHAR				*pArgv)
{
	INT32 DebugLevel;


	DebugLevel = FT_CMD_UtilNumGet(&pArgv);

	FT_KDP_EVENT_INFORM(pAd, BSS0, FT_KSP_SIG_DEBUG_TRACE,
						&DebugLevel, sizeof(DebugLevel), NULL);
} /* End of FT_KDP_CMD_DbgFlagCtrl */


#ifdef FT_FUNC_SIMULATION
/*
========================================================================
Routine Description:
	List all queued events.

Arguments:
	pAd				- WLAN control block pointer
	Argc			- the number of input parameters
	*pArgv			- input parameters

Return Value:
	None

Note:
========================================================================
*/
static VOID TYPE_FUNC FT_KDP_CMD_SimEvtFtAssoc(
	IN	PRTMP_ADAPTER		pAd,
	IN	INT32				Argc,
	IN	CHAR				*pArgv)
{
	FT_KDP_EVT_ASSOC EvtAssoc;


	/* fill the station information */
	EvtAssoc.SeqNum = 0x1234;
	NdisMoveMemory(EvtAssoc.MacAddr, gFT_MAC_STA, MAC_ADDR_LEN);

	/* inform other APs a station associated to us */
	FT_KDP_EVENT_INFORM(pAd, BSS0, FT_KDP_SIG_FT_ASSOCIATION,
						&EvtAssoc, sizeof(FT_KDP_EVT_ASSOC), NULL);
} /* End of FT_KDP_CMD_SimEvtFtAssoc */


/*
========================================================================
Routine Description:
	List all queued events.

Arguments:
	pAd				- WLAN control block pointer
	Argc			- the number of input parameters
	*pArgv			- input parameters

Return Value:
	None

Note:
========================================================================
*/
static VOID TYPE_FUNC FT_KDP_CMD_SimEvtFtReAssoc(
	IN	PRTMP_ADAPTER		pAd,
	IN	INT32				Argc,
	IN	CHAR				*pArgv)
{
	FT_KDP_EVT_REASSOC EvtReAssoc;


	/* fill the station information */
	EvtReAssoc.SeqNum = 0x1234;
	NdisMoveMemory(EvtReAssoc.MacAddr, gFT_MAC_STA, MAC_ADDR_LEN);
	NdisMoveMemory(EvtReAssoc.OldApMacAddr, gFT_MAC_OldAP, MAC_ADDR_LEN);

	/* inform other APs a station associated to us */
	FT_KDP_EVENT_INFORM(pAd, BSS0, FT_KDP_SIG_FT_REASSOCIATION,
						&EvtReAssoc, sizeof(FT_KDP_EVT_REASSOC), NULL);
} /* End of FT_KDP_CMD_SimEvtFtReAssoc */


/*
========================================================================
Routine Description:
	Request PMK-R1 Key from R0KH.

Arguments:
	pAd				- WLAN control block pointer
	Argc			- the number of input parameters
	*pArgv			- input parameters

Return Value:
	None

Note:
	1. Command Format:
		[R0KHID 1st byte (HEX)] [R1KHID 1st byte (HEX)]
========================================================================
*/
static VOID TYPE_FUNC FT_KDP_CMD_SimKeyReq(
	IN	PRTMP_ADAPTER		pAd,
	IN	INT32				Argc,
	IN	CHAR				*pArgv)
{
	FT_KDP_EVT_KEY_ELM EvtKeyReq, *pEvtKeyReq;
	UCHAR R0KHID_Byte, R1KHID_Byte;


	/* init */
	memset(&EvtKeyReq, 0, sizeof(FT_KDP_EVT_KEY_ELM));
	pEvtKeyReq = &EvtKeyReq;

	/* fill request content */
	pEvtKeyReq->ElmId = FT_KDP_ELM_ID_PRI;
	pEvtKeyReq->ElmLen = FT_KDP_ELM_PRI_LEN;
	pEvtKeyReq->OUI[0] = FT_KDP_ELM_PRI_OUI_0;
	pEvtKeyReq->OUI[1] = FT_KDP_ELM_PRI_OUI_1;
	pEvtKeyReq->OUI[2] = FT_KDP_ELM_PRI_OUI_2;

	memcpy(pEvtKeyReq->MacAddr, gFT_MAC_STA, ETH_ALEN);

	R0KHID_Byte = FT_CMD_UtilHexGet(&pArgv);
	memset(pEvtKeyReq->KeyInfo.R0KHID, R0KHID_Byte, FT_KDP_R0KHID_MAX_SIZE);

	R1KHID_Byte = FT_CMD_UtilHexGet(&pArgv);
	memset(pEvtKeyReq->KeyInfo.R1KHID, R1KHID_Byte, FT_KDP_R1KHID_MAX_SIZE);

	memcpy(pEvtKeyReq->KeyInfo.S1KHID, gFT_MAC_STA, FT_KDP_S1KHID_MAX_SIZE);
	memset(pEvtKeyReq->KeyInfo.RSV, 0x00, sizeof(pEvtKeyReq->KeyInfo.RSV));

	/* request PMK-R1 Key (our R1KH vs. the station) from the R0KH */
	FT_KDP_EVENT_INFORM(pAd, BSS0, FT_KDP_SIG_KEY_REQ,
						pEvtKeyReq, sizeof(FT_KDP_EVT_KEY_ELM), NULL);
} /* End of FT_KDP_CMD_SimKeyReq */


#ifdef FT_KDP_FUNC_R0KH_IP_RECORD
/*
========================================================================
Routine Description:
	Create a R0KH information.

Arguments:
	pAd				- WLAN control block pointer
	Argc			- the number of input parameters
	*pArgv			- input parameters

Return Value:
	None

Note:
	1. Command Format:
		[R0KHID 1st byte (HEX)] [IP1] [IP2] [IP3] [IP4]
========================================================================
*/
static VOID TYPE_FUNC FT_KDP_CMD_SimR0KH_InfoCreate(
	IN	PRTMP_ADAPTER		pAd,
	IN	INT32				Argc,
	IN	CHAR				*pArgv)
{
	UCHAR R0KHID[FT_KDP_R0KHID_MAX_SIZE];
	UCHAR MAC[ETH_ALEN];
	UCHAR ByteFirst;
	UCHAR IP[4];


	ByteFirst = FT_CMD_UtilHexGet(&pArgv);
	IP[0] = FT_CMD_UtilNumGet(&pArgv);
	IP[1] = FT_CMD_UtilNumGet(&pArgv);
	IP[2] = FT_CMD_UtilNumGet(&pArgv);
	IP[3] = FT_CMD_UtilNumGet(&pArgv);

	memset(R0KHID, ByteFirst, sizeof(R0KHID));
	memset(MAC, ByteFirst, sizeof(MAC));
	MAC[0] = 0x00;

	FT_KDP_R0KH_InfoAdd(pAd, R0KHID, MAC, *(UINT32 *)IP);
} /* End of FT_KDP_CMD_SimR0KH_InfoCreate */
#endif /* FT_KDP_FUNC_R0KH_IP_RECORD */


/*
========================================================================
Routine Description:
	Simulate to make a resource request.

Arguments:
	pAd				- WLAN control block pointer
	Argc			- the number of input parameters
	*pArgv			- input parameters

Return Value:
	None

Note:
	1. Command Format:
		[1-WME] [TID:0~7] [dir:0~3] [access:1~3] [UP:0~7] [ack:0~1]
		[nom size:byte] [inact:sec] [mean data rate:bps] [min phy rate:bps]
		[surp factor:>=1] [tclas processing:0~1]
	2. dir: 0 - uplink, 1 - dnlink, 2 - bidirectional link, 3 - direct link
		access: 1 - EDCA, 2 - HCCA, 3 - EDCA + HCCA
		ack: 0 - normal ACK, 1 - no ACK
========================================================================
*/
static VOID FT_RIC_CMD_SimRscReqHdlTspec(
	IN	PRTMP_ADAPTER		pAd,
	IN	INT32				Argc,
	IN	CHAR				*pArgv)
{
} /* End of FT_RIC_CMD_SimRscReqHdlTspec */


/*
========================================================================
Routine Description:
	Simulate to handle resource requests.

Arguments:
	pAd				- WLAN control block pointer
	Argc			- the number of input parameters
	*pArgv			- input parameters

Return Value:
	None

Note:
	Test example:

	iwpriv ra0 set acm=50
	iwpriv ra0 set acm=10_0_0_1_1

	1. OK case
		iwpriv ra0 set ft=512_1_7_0_1_7_0_500_3000_200000_11000000_10_0
		iwpriv ra0 set ft=512_1_6_1_1_6_0_500_3000_200000_11000000_10_0
		iwpriv ra0 set ft=512_1_5_0_1_5_0_500_3000_200000_11000000_10_0
		iwpriv ra0 set ft=512_1_4_1_1_4_0_500_3000_200000_11000000_10_0

	2. Fail case
		iwpriv ra0 set ft=512_1_7_0_1_7_0_500_3000_200000_11000000_10_0
		iwpriv ra0 set ft=512_1_5_0_1_5_0_500_3000_200000_11000000_10_0
		iwpriv ra0 set ft=512_1_6_1_1_6_0_500_3000_11000000_11000000_10_0
		iwpriv ra0 set ft=512_1_4_1_1_4_0_500_3000_11000000_11000000_10_0

	iwpriv ra0 set ft=510
========================================================================
*/
static VOID TYPE_FUNC FT_RIC_CMD_SimRscReqHandle(
	IN	PRTMP_ADAPTER		pAd,
	IN	INT32				Argc,
	IN	CHAR				*pArgv)
	{
} /* End of FT_RIC_CMD_SimRscReqHandle */


/*
========================================================================
Routine Description:
	Simulate to send a RRB frame.

Arguments:
	pAd				- WLAN control block pointer
	Argc			- the number of input parameters
	*pArgv			- input parameters

Return Value:
	None

Note:
	1. Command Format:
		[0(REQ)/1(RSP)] [PeerMAC]
========================================================================
*/
static VOID TYPE_FUNC FT_RRB_CMD_SimSend(
	IN	PRTMP_ADAPTER		pAd,
	IN	INT32				Argc,
	IN	CHAR				*pArgv)
{
	FT_KDP_EVT_ACTION ActionCB, *pActionCB;
	UCHAR MacPeer[6];


	pActionCB = &ActionCB;

	pActionCB->RequestType = FT_CMD_UtilNumGet(&pArgv);
	FT_CMD_UtilMacGet(&pArgv, MacPeer);

	memcpy(pActionCB->MacDa, MacPeer, ETH_ALEN);
	memset(pActionCB->MacSa, 0x22, ETH_ALEN);
	memset(pActionCB->MacAp, 0x33, ETH_ALEN);

	pActionCB->MacDa[0] = 0x00;
	pActionCB->MacSa[0] = 0x00;
	pActionCB->MacAp[0] = 0x00;

	FT_KDP_EventInform(pAd, BSS0, FT_KDP_SIG_ACTION,
						"testtesttesttesttest", 19, 0, (VOID *)pActionCB);
} /* End of FT_RRB_CMD_SimSend */


/*
========================================================================
Routine Description:
	Simulate to send a 11k neighbor request.

Arguments:
	pAd				- WLAN control block pointer
	Argc			- the number of input parameters
	*pArgv			- input parameters

Return Value:
	None

Note:
	1. Command Format:
		PeerSSID
========================================================================
*/
static VOID TYPE_FUNC FT_11K_CMD_SimInfoReq(
	IN	PRTMP_ADAPTER		pAd,
	IN	INT32				Argc,
	IN	CHAR				*pArgv)
{
	if (strlen((PSTRING) pArgv) <= MAX_LEN_OF_SSID)
	{
		FT_KDP_EVENT_INFORM(pAd, BSS0, FT_KDP_SIG_AP_INFO_REQ,
							pArgv, strlen((PSTRING) pArgv), NULL);
	} /* End of if */
} /* End of FT_11K_CMD_SimInfoReq */


/*
========================================================================
Routine Description:
	Show encryption/decryption key information.

Arguments:
	pAd				- WLAN control block pointer
	Argc			- the number of input parameters
	*pArgv			- input parameters

Return Value:
	None

Note:
========================================================================
*/
static VOID TYPE_FUNC FT_11K_CMD_SimKeyShow(
	IN	PRTMP_ADAPTER		pAd,
	IN	INT32				Argc,
	IN	CHAR				*pArgv)
{
	hex_dump("Key=", FT_KDP_CB->CryptKey, sizeof(FT_KDP_CB->CryptKey));
} /* End of FT_11K_CMD_SimInfoReq */
#endif /* FT_FUNC_SIMULATION */


/*
========================================================================
Routine Description:
	Display a R0KH information.

Arguments:
	pAd				- WLAN control block pointer
	Argc			- the number of input parameters
	*pArgv			- input parameters

Return Value:
	None

Note:

========================================================================
*/
static VOID TYPE_FUNC FT_R1KH_InfoShow(
	IN	PRTMP_ADAPTER		pAd,
	IN	INT32				Argc,
	IN	CHAR				*pArgv)
{
	INT i;
	INT HashIdx;
	PFT_R1HK_ENTRY pEntry;

	RTMP_SEM_LOCK(&pAd->ApCfg.FtTab.FT_R1khEntryTabLock);
	for (HashIdx = 0; HashIdx < FT_R1KH_ENTRY_HASH_TABLE_SIZE; HashIdx++)
	{
		pEntry = (PFT_R1HK_ENTRY)\
						(pAd->ApCfg.FtTab.FT_R1khEntryTab[HashIdx].pHead);

		while (pEntry != NULL)
		{
			DBGPRINT_RAW(RT_DEBUG_OFF, ("StaMac=%02x:%02x:%02x:%02x:%02x:%02x, ",
				pEntry->StaMac[0], pEntry->StaMac[1], pEntry->StaMac[2], 
				pEntry->StaMac[3], pEntry->StaMac[4], pEntry->StaMac[5]));
			DBGPRINT_RAW(RT_DEBUG_OFF, ("\nKeyLifeTime=%d, ", pEntry->KeyLifeTime));
			DBGPRINT_RAW(RT_DEBUG_OFF, ("\nRassocDeadline=%d\n",
											pEntry->RassocDeadline));
			DBGPRINT_RAW(RT_DEBUG_OFF, ("PairwisChipher="));
			for (i=0; i<4; i++)
				DBGPRINT_RAW(RT_DEBUG_OFF, ("%02x:", pEntry->PairwisChipher[i]));
			DBGPRINT_RAW(RT_DEBUG_OFF, ("\nAkmSuite="));
			for (i=0; i<4; i++)
				DBGPRINT_RAW(RT_DEBUG_OFF, ("%02x:", pEntry->AkmSuite[i]));
			DBGPRINT_RAW(RT_DEBUG_OFF, ("\nPmkR0Name="));
			for (i=0; i<16; i++)
				DBGPRINT_RAW(RT_DEBUG_OFF, ("%02x:", pEntry->PmkR0Name[i]));
			DBGPRINT_RAW(RT_DEBUG_OFF, ("\nPmkR1Key="));
			for (i=0; i<32; i++)
				DBGPRINT_RAW(RT_DEBUG_OFF, ("%02x:", pEntry->PmkR1Key[i]));
			DBGPRINT_RAW(RT_DEBUG_OFF, ("\nPmkR1Name="));
			for (i=0; i<16; i++)
				DBGPRINT_RAW(RT_DEBUG_OFF, ("%02x:", pEntry->PmkR1Name[i]));
			DBGPRINT_RAW(RT_DEBUG_OFF, ("\n"));
			pEntry = pEntry->pNext;
		}
	}
	RTMP_SEM_UNLOCK(&pAd->ApCfg.FtTab.FT_R1khEntryTabLock);
} /* End of FT_R1KH_InfoShow */


static PMAC_TABLE_ENTRY FtEntry = NULL;
static VOID FT_OverDs_SimReq(
	IN	PRTMP_ADAPTER		pAd,
	IN	INT32				Argc,
	IN	CHAR				*pArgv)
{
	PUCHAR pOutBuffer = NULL;
	ULONG FrameLen;
	HEADER_802_11 Hdr;
	FT_INFO FtInfoBuf;
	NDIS_STATUS NStatus;

	UCHAR StaAddr[MAC_ADDR_LEN] = {0x00,0x0c,0x43,0x00,0x00,0x00};
	UCHAR TargetAddr[MAC_ADDR_LEN] = {0x00,0x0c,0x43,0x28,0x60,0x10};

	if (FtEntry == NULL)
	{
		FtEntry = MacTableInsertEntry(pAd, StaAddr, 0, OPMODE_AP, TRUE);
		FtEntry->Sst = SST_ASSOC;
	}

	NdisZeroMemory(&FtInfoBuf, sizeof(FT_INFO));
	NStatus = MlmeAllocateMemory(pAd, &pOutBuffer);
	if (NStatus != NDIS_STATUS_SUCCESS) 
	{
		DBGPRINT(RT_DEBUG_ERROR,("%s: allocate memory failed.\n",
									__FUNCTION__));
		return;
	}

	/* Make 802.11 header. */
	ActHeaderInit(pAd, &Hdr, pAd->ApCfg.MBSSID[FtEntry->apidx].Bssid,
		FtEntry->Addr, pAd->ApCfg.MBSSID[FtEntry->apidx].Bssid);

	FrameLen = 0;
	MakeOutgoingFrame(pOutBuffer,				&FrameLen,
					sizeof(HEADER_802_11),		&Hdr,
					END_OF_ARGS);

	FtInfoBuf.MdIeInfo.Len = 3;
	FT_SET_MDID(FtInfoBuf.MdIeInfo.MdId, FT_DEFAULT_MDID);
	FtInfoBuf.MdIeInfo.FtCapPlc.field.FtOverDs = 1;
	FtInfoBuf.MdIeInfo.FtCapPlc.field.RsrReqCap = 1;
	FT_MakeFtActFrame(pAd, pOutBuffer, &FrameLen,
		FT_ACTION_BT_REQ, FtEntry->Addr, TargetAddr, 0,
		&FtInfoBuf);

	/* enqueue it into FT action state machine. */
	REPORT_MGMT_FRAME_TO_MLME(pAd, FtEntry->Aid, pOutBuffer, \
		FrameLen, 0, 0, 0, 0, OPMODE_AP);

	if (pOutBuffer)
		os_free_mem(pAd, pOutBuffer);

}

static VOID FT_OverDs_SimConfirm(
	IN	PRTMP_ADAPTER		pAd,
	IN	INT32				Argc,
	IN	CHAR				*pArgv)
{
}
#endif /* CONFIG_AP_SUPPORT */

#define FT_KDP_CMD_DAEMON_KILL			0
#define FT_KDP_CMD_EVENT_LIST			1
#define FT_KDP_CMD_DEBUG_FLAG_CTRL		2

#define FT_KDP_CMD_SM_EVT_FT_ASSOC		500
#define FT_KDP_CMD_SM_EVT_FT_REASSOC	501
#define FT_KDP_CMD_SM_EVT_KEY_REQ		502
#define FT_KDP_CMD_SM_R0KH_INFO_SHOW	503
#define FT_KDP_CMD_SM_R0KH_INFO_CREATE	504
#define FT_R1KH_INFO_SHOW				505
#define FT_RIC_CMD_SM_REQ_START			506
#define FT_RIC_CMD_SM_REQ				507
#define FT_RIC_CMD_SM_REQ_END			508
#define FT_RIC_CMD_SM_REQ_RSP_LIST		509
#define FT_RIC_CMD_SM_REQ_HANDLE		510
#define FT_RIC_CMD_SM_RRB_SEND			511
#define FT_RIC_CMD_SM_REQ_HDL_TSPEC		512
#define FT_11K_CMD_INFO_REQ				513
#define FT_KDP_KEY_SHOW					514

#define FT_REQ_ACT						600
#define FT_CONFIRM_ACT					601
#define FT_ROAMING_ACT					602

/*
========================================================================
Routine Description:
	IO control handler for IEEE802.11r.

Arguments:
	pAd				- WLAN control block pointer
	pArgvIn			- the data flow information

Return Value:
	None

Note:
	All test commands are listed as follows:
		iwpriv ra0 set ft=[cmd id]_[arg1]_[arg2]_......_[argn]
		[cmd id] = xx, such as 000, 001, 002, 003, ...
========================================================================
*/
INT TYPE_FUNC FT_Ioctl(
	IN	PRTMP_ADAPTER		pAd,
	IN	PSTRING				pArgvIn)
{
	CHAR BufCmd[4] = { 0, 0, 0, 0 };
	CHAR *pArgv, *pParam;
	UINT32 Command, CmdEndOffset, IdCmd;
	INT32 Argc;


	/* init */
	pArgv = (CHAR *)pArgvIn;

	/* get command type */
	/* command format is iwpriv ra0 set acm=[cmd id]_[arg1]_......_[argn] */
	for(IdCmd=0; IdCmd<=3; IdCmd++)
	{
		if ((pArgv[IdCmd] == 0x00) || (pArgv[IdCmd] == '_'))
		{
			CmdEndOffset = IdCmd;
			break;
		} /* End of if */
	} /* End of for */

	memcpy(BufCmd, pArgv, IdCmd);
	Command = FT_ARG_ATOI(BufCmd);
	pArgv += IdCmd; /* skip command field */

	/* get Argc number */
	Argc = 0;
	pParam = pArgv;

	while(1)
	{
		if (*pParam == '_')
			Argc ++;
		/* End of if */

		if ((*pParam == 0x00) || (Argc > 20))
			break;
		/* End of if */

		pParam++;
	} /* End of while */

	pArgv++; /* skip _ points to arg1 */

	/* handle the command */
	switch(Command)
	{
#ifdef CONFIG_AP_SUPPORT
		/* normal commands */
		case FT_KDP_CMD_DAEMON_KILL:
			DBGPRINT(RT_DEBUG_TRACE, ("ft_iocl> kill daemon!\n"));
			FT_KDP_EventInform(pAd, BSS0, FT_KDP_SIG_TERMINATE, NULL, 0, 0, NULL);
			break;

#ifndef FT_KDP_FUNC_SOCK_COMM
		case FT_KDP_CMD_EVENT_LIST:
			DBGPRINT(RT_DEBUG_TRACE, ("ft_iocl> event list!\n"));
			FT_KDP_CMD_EventList(pAd, Argc, pArgv);
			break;
#endif /* FT_KDP_FUNC_SOCK_COMM */

		case FT_KDP_CMD_DEBUG_FLAG_CTRL:
			DBGPRINT(RT_DEBUG_TRACE, ("ft_iocl> IAPP daemon debug flag control!\n"));
			FT_KDP_CMD_DbgFlagCtrl(pAd, Argc, pArgv);
			break;

#ifdef FT_FUNC_SIMULATION
		/* simulation commands */
		case FT_KDP_CMD_SM_EVT_FT_ASSOC:
			DBGPRINT(RT_DEBUG_TRACE, ("ft_iocl> (sm) ft assoc!\n"));
			FT_KDP_CMD_SimEvtFtAssoc(pAd, Argc, pArgv);
			break;

		case FT_KDP_CMD_SM_EVT_FT_REASSOC:
			DBGPRINT(RT_DEBUG_TRACE, ("ft_iocl> (sm) ft reassoc!\n"));
			FT_KDP_CMD_SimEvtFtReAssoc(pAd, Argc, pArgv);
			break;

		case FT_KDP_CMD_SM_EVT_KEY_REQ:
			DBGPRINT(RT_DEBUG_TRACE, ("ft_iocl> (sm) key req!\n"));
			FT_KDP_CMD_SimKeyReq(pAd, Argc, pArgv);
			break;

#ifdef FT_KDP_FUNC_R0KH_IP_RECORD
		case FT_KDP_CMD_SM_R0KH_INFO_CREATE:
			DBGPRINT(RT_DEBUG_TRACE, ("ft_iocl> (sm) R0KH INFO create!\n"));
			FT_KDP_CMD_SimR0KH_InfoCreate(pAd, Argc, pArgv);
			break;
#endif /* FT_KDP_FUNC_R0KH_IP_RECORD */

		case FT_RIC_CMD_SM_REQ_HANDLE:
			DBGPRINT(RT_DEBUG_TRACE, ("ft_iocl> (sm) handle a resource request!\n"));
			FT_RIC_CMD_SimRscReqHandle(pAd, Argc, pArgv);
			break;

		case FT_RIC_CMD_SM_REQ_HDL_TSPEC:
			DBGPRINT(RT_DEBUG_TRACE, ("ft_iocl> (sm) make a resource request TSPEC!\n"));
			FT_RIC_CMD_SimRscReqHdlTspec(pAd, Argc, pArgv);
			break;

		case FT_RIC_CMD_SM_RRB_SEND:
			DBGPRINT(RT_DEBUG_TRACE, ("ft_iocl> (sm) send a RRB frame!\n"));
			FT_RRB_CMD_SimSend(pAd, Argc, pArgv);
			break;

		case FT_11K_CMD_INFO_REQ:
			DBGPRINT(RT_DEBUG_TRACE, ("ft_iocl> (sm) send a info request frame!\n"));
			FT_11K_CMD_SimInfoReq(pAd, Argc, pArgv);
			break;

		case FT_KDP_KEY_SHOW:
			DBGPRINT(RT_DEBUG_TRACE, ("ft_iocl> (sm) show key information!\n"));
			FT_11K_CMD_SimKeyShow(pAd, Argc, pArgv);
			break;
#endif /* FT_FUNC_SIMULATION */

#ifdef FT_KDP_FUNC_R0KH_IP_RECORD
		case FT_KDP_CMD_SM_R0KH_INFO_SHOW:
			DBGPRINT(RT_DEBUG_TRACE, ("ft_iocl> (sm) R0KH INFO show!\n"));
			FT_KDP_CMD_R0KH_InfoShow(pAd, (PSTRING)pArgv);
			break;
#endif /* FT_KDP_FUNC_R0KH_IP_RECORD */

		case FT_R1KH_INFO_SHOW:
			DBGPRINT(RT_DEBUG_TRACE, ("ft_iocl> (sm) R1KH INFO show!\n"));
			FT_R1KH_InfoShow(pAd, Argc, pArgv);
			break;

		case FT_REQ_ACT:
			DBGPRINT(RT_DEBUG_TRACE, ("ft_iocl> FT_REQ_ACT!\n"));
			FT_OverDs_SimReq(pAd, Argc, pArgv);
			break;

		case FT_CONFIRM_ACT:
			DBGPRINT(RT_DEBUG_TRACE, ("ft_iocl> FT_CONFIRM_ACT!\n"));
			FT_OverDs_SimConfirm(pAd, Argc, pArgv);
			break;
#endif /* CONFIG_AP_SUPPORT */

		default: /* error command type */
			DBGPRINT(RT_DEBUG_TRACE,
						("ft_iocl> ERROR! No such command %d!\n", Command));
			return -EINVAL; /* input error */
	} /* End of switch */

	return TRUE;
} /* End of FT_Ioctl */

#endif /* DOT11R_FT_SUPPORT */

/* End of ft_iocl.c */
