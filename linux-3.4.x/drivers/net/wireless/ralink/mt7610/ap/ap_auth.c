/****************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 * (c) Copyright 2002, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ****************************************************************************

    Module Name:
    auth.c
 
    Abstract:
    Handle de-auth request from local MLME
 
    Revision History:
    Who         When          What
    --------    ----------    ----------------------------------------------
    John Chang  08-04-2003    created for 11g soft-AP
 */

#include "rt_config.h"
#ifdef DOT11R_FT_SUPPORT
#include "ft.h"
#endif /* DOT11R_FT_SUPPORT */

static VOID APMlmeDeauthReqAction(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem);

static VOID APPeerDeauthReqAction(
    IN PRTMP_ADAPTER	pAd, 
    IN MLME_QUEUE_ELEM *Elem);

static VOID APPeerAuthReqAtIdleAction(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem);

static VOID APPeerAuthConfirmAction(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem);

static VOID APPeerAuthSimpleRspGenAndSend(
    IN  PRTMP_ADAPTER   pAd, 
    IN  PHEADER_802_11 pHdr80211, 
    IN  USHORT Alg, 
    IN  USHORT Seq, 
    IN  USHORT StatusCode);

/*
    ==========================================================================
    Description:
        authenticate state machine init, including state transition and timer init
    Parameters:
        Sm - pointer to the auth state machine
    Note:
        The state machine looks like this
        
                                    AP_AUTH_REQ_IDLE           
        APMT2_MLME_DEAUTH_REQ     mlme_deauth_req_action  
    ==========================================================================
 */
void APAuthStateMachineInit(
    IN PRTMP_ADAPTER pAd, 
    IN STATE_MACHINE *Sm, 
    OUT STATE_MACHINE_FUNC Trans[]) 
{
    StateMachineInit(Sm, (STATE_MACHINE_FUNC *)Trans, AP_MAX_AUTH_STATE,
					AP_MAX_AUTH_MSG, (STATE_MACHINE_FUNC)Drop,
					AP_AUTH_REQ_IDLE, AP_AUTH_MACHINE_BASE);
     
    /* the first column */
    StateMachineSetAction(Sm, AP_AUTH_REQ_IDLE, APMT2_MLME_DEAUTH_REQ,
						(STATE_MACHINE_FUNC)APMlmeDeauthReqAction);
	StateMachineSetAction(Sm, AP_AUTH_REQ_IDLE, APMT2_PEER_DEAUTH,
						(STATE_MACHINE_FUNC)APPeerDeauthReqAction);
    StateMachineSetAction(Sm, AP_AUTH_REQ_IDLE, APMT2_PEER_AUTH_REQ,
						(STATE_MACHINE_FUNC)APPeerAuthReqAtIdleAction);
    StateMachineSetAction(Sm, AP_AUTH_REQ_IDLE, APMT2_PEER_AUTH_CONFIRM,
						(STATE_MACHINE_FUNC)APPeerAuthConfirmAction);
}


/*
    ==========================================================================
    Description:
        Upper Layer request to kick out a STA
    ==========================================================================
 */
static VOID APMlmeDeauthReqAction(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem) 
{
    MLME_DEAUTH_REQ_STRUCT	*pInfo;
    HEADER_802_11			Hdr;
    PUCHAR					pOutBuffer = NULL;
    NDIS_STATUS				NStatus;
    ULONG					FrameLen = 0;
    MAC_TABLE_ENTRY			*pEntry;
	UCHAR					apidx;


    if (!pAd)
	return;

    pInfo = (MLME_DEAUTH_REQ_STRUCT *)Elem->Msg;

    if (Elem->Wcid < MAX_LEN_OF_MAC_TABLE)
    {
	pEntry = &pAd->MacTab.Content[Elem->Wcid];
	if (!pEntry)
		return;
		
#ifdef WAPI_SUPPORT
	WAPI_InternalCmdAction(pAd, 
				   pEntry->AuthMode, 
				   pEntry->apidx, 
				   pEntry->Addr, 
				   WAI_MLME_DISCONNECT);		
#endif /* WAPI_SUPPORT */
		
	/* send wireless event - for deauthentication */
	RTMPSendWirelessEvent(pAd, IW_DEAUTH_EVENT_FLAG, pInfo->Addr, 0, 0);  
	//ApLogEvent(pAd, pInfo->Addr, EVENT_DISASSOCIATED);

	apidx = pEntry->apidx;

        /* 1. remove this STA from MAC table */
        MacTableDeleteEntry(pAd, Elem->Wcid, pInfo->Addr);

        /* 2. send out DE-AUTH request frame */
        NStatus = MlmeAllocateMemory(pAd, &pOutBuffer);
        if (NStatus != NDIS_STATUS_SUCCESS) 
            return;

        printk ("%s AUTH - Send DE-AUTH req to %02x:%02x:%02x:%02x:%02x:%02x\n",
				pAd->CommonCfg.Channel > 14 ? "5GHz AP" : "2.4GHz AP",
				pInfo->Addr[0], pInfo->Addr[1], pInfo->Addr[2],
				pInfo->Addr[3], pInfo->Addr[4], pInfo->Addr[5]);
           		
        MgtMacHeaderInit(pAd, &Hdr, SUBTYPE_DEAUTH, 0, pInfo->Addr,
						pAd->ApCfg.MBSSID[apidx].Bssid);
        MakeOutgoingFrame(pOutBuffer,				&FrameLen, 
                          sizeof(HEADER_802_11),	&Hdr, 
                          2,						&pInfo->Reason, 
                          END_OF_ARGS);
        MiniportMMRequest(pAd, 0, pOutBuffer, FrameLen);

        MlmeFreeMemory(pAd, pOutBuffer);
    }
}


static VOID APPeerDeauthReqAction(
    IN PRTMP_ADAPTER pAd, 
    IN PMLME_QUEUE_ELEM Elem) 
{
    UCHAR			Addr2[MAC_ADDR_LEN];
    USHORT			Reason;
	UINT16			SeqNum;
    MAC_TABLE_ENTRY	*pEntry;

    if (!pAd)
	return;


    if (! PeerDeauthReqSanity(pAd, Elem->Msg, Elem->MsgLen, Addr2, &SeqNum, &Reason)) 
        return;

	pEntry = NULL;

	/*pEntry = MacTableLookup(pAd, Addr2); */
	if (Elem->Wcid < MAX_LEN_OF_MAC_TABLE)
	{
		pEntry = &pAd->MacTab.Content[Elem->Wcid];
		if (!pEntry)
			return;

		{
			MULTISSID_STRUCT *pMbss = &pAd->ApCfg.MBSSID[pEntry->apidx];
			PFRAME_802_11 Fr = (PFRAME_802_11)Elem->Msg;

			if (!Fr || (memcmp(&Fr->Hdr.Addr1, pMbss->Bssid, 6) != 0))
			{
#ifdef DBG
				unsigned char *tmp = (unsigned char *)pMbss->Bssid;
				unsigned char *tmp2 = (unsigned char *)&Fr->Hdr.Addr1;

				DBGPRINT(RT_DEBUG_WARN, ("da not match bssid,bssid:0x%02x%02x%02x%02x%02x%02x, addr1:0x%02x%02x%02x%02x%02x%02x\n",
					PRINT_MAC(tmp), PRINT_MAC(tmp2)));
#endif /* DBG */
				return;
			}
		}

#ifdef DOT1X_SUPPORT    
		/* Notify 802.1x daemon to clear this sta info */
		if (pEntry->AuthMode == Ndis802_11AuthModeWPA || 
			pEntry->AuthMode == Ndis802_11AuthModeWPA2 ||
			pAd->ApCfg.MBSSID[pEntry->apidx].IEEE8021X)
			DOT1X_InternalCmdAction(pAd, pEntry, DOT1X_DISCONNECT_ENTRY);
#endif /* DOT1X_SUPPORT */

#ifdef WAPI_SUPPORT
		WAPI_InternalCmdAction(pAd, 
							   pEntry->AuthMode, 
							   pEntry->apidx, 
							   pEntry->Addr, 
							   WAI_MLME_DISCONNECT);		
#endif /* WAPI_SUPPORT */

		/* send wireless event - for deauthentication */
		RTMPSendWirelessEvent(pAd, IW_DEAUTH_EVENT_FLAG, Addr2, 0, 0);  
		//ApLogEvent(pAd, Addr2, EVENT_DISASSOCIATED);
		
    		if (pEntry->CMTimerRunning == TRUE)
    		{
        		/*
				If one who initilized Counter Measure deauth itself,
				AP doesn't log the MICFailTime
			*/
        		pAd->ApCfg.aMICFailTime = pAd->ApCfg.PrevaMICFailTime;
    		}

		if (!IS_ENTRY_CLIENT(pEntry)) {
		    DBGPRINT(RT_DEBUG_TRACE, ("%s: receive not client de-auth ###\n", __FUNCTION__));
		} else {
		    MacTableDeleteEntry(pAd, Elem->Wcid, Addr2);
		    printk("%s AUTH - receive DE-AUTH(seq-%d) from %02x:%02x:%02x:%02x:%02x:%02x, reason=%d\n", pAd->CommonCfg.Channel > 14 ? "5GHz AP" : "2.4GHz AP", SeqNum,
		    Addr2[0], Addr2[1], Addr2[2], Addr2[3], Addr2[4], Addr2[5], Reason);
		}

#ifdef MAC_REPEATER_SUPPORT
		if (pAd->ApCfg.bMACRepeaterEn == TRUE)
		{
			UCHAR apCliIdx, CliIdx;
			REPEATER_CLIENT_ENTRY *pReptEntry = NULL;
			pReptEntry = RTMPLookupRepeaterCliEntry(pAd, TRUE, Addr2);
			if (pReptEntry && (pReptEntry->CliConnectState != 0))
			{
				apCliIdx = pReptEntry->MatchApCliIdx;
				CliIdx = pReptEntry->MatchLinkIdx;
				MlmeEnqueue(pAd, APCLI_CTRL_STATE_MACHINE, APCLI_CTRL_DISCONNECT_REQ, 0, NULL,
								(64 + MAX_EXT_MAC_ADDR_SIZE*apCliIdx + CliIdx));
				RTMP_MLME_HANDLER(pAd);
				RTMPRemoveRepeaterEntry(pAd, apCliIdx, CliIdx);
			}
		}
#endif /* MAC_REPEATER_SUPPORT */
    }
}


static VOID APPeerAuthReqAtIdleAction(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem)
{
	INT i;
	USHORT Seq, Alg, RspReason, Status;
	UCHAR Addr1[MAC_ADDR_LEN];
	UCHAR Addr2[MAC_ADDR_LEN];
	CHAR Chtxt[CIPHER_TEXT_LEN];
	UINT32 apidx;

	PHEADER_802_11 pRcvHdr;
	HEADER_802_11 AuthHdr;
	PUCHAR pOutBuffer = NULL;
	NDIS_STATUS NStatus;
	ULONG FrameLen = 0;
	MAC_TABLE_ENTRY *pEntry;
	UCHAR ChTxtIe = 16, ChTxtLen = CIPHER_TEXT_LEN;
#ifdef DOT11R_FT_SUPPORT
	PFT_CFG pFtCfg;
	FT_INFO FtInfo;
	PFT_INFO pFtInfoBuf;
#endif /* DOT11R_FT_SUPPORT */
	MULTISSID_STRUCT *pMbss;
	CHAR rssi;
#ifdef BAND_STEERING
	BOOLEAN bBndStrgCheck = TRUE;
	BOOLEAN bAllowStaConnectInHt = FALSE;
#endif /* BAND_STEERING */

	if (pAd == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: pAd is NULL\n",__FUNCTION__));
 		return;
	}

	/* disallow new association */
	if (pAd->ApCfg.BANClass3Data == TRUE)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Disallow new Association\n"));
		return;
	}

	if (!APPeerAuthSanity(pAd, Elem->Msg, Elem->MsgLen, Addr1, Addr2, &Alg, &Seq, &Status, Chtxt
#ifdef DOT11R_FT_SUPPORT
							,&FtInfo
#endif /* DOT11R_FT_SUPPORT */
	    ))
		return;
    

	/* Find which MBSSID to be authenticate */
	for (apidx=0; apidx<pAd->ApCfg.BssidNum; apidx++)
	{	
		if (RTMPEqualMemory(Addr1, pAd->ApCfg.MBSSID[apidx].Bssid, MAC_ADDR_LEN))
			break;
	}

	if (apidx >= pAd->ApCfg.BssidNum)
	{	
		DBGPRINT(RT_DEBUG_TRACE, ("AUTH - Bssid not found\n"));
		return;
	}

	if (apidx >= HW_BEACON_MAX_NUM)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Index out of bound\n"));
		return;
	}

	pMbss = &pAd->ApCfg.MBSSID[apidx];

	if ((pAd->ApCfg.MBSSID[apidx].MSSIDDev != NULL) &&
		!(RTMP_OS_NETDEV_STATE_RUNNING(pAd->ApCfg.MBSSID[apidx].MSSIDDev)))
	{
    	DBGPRINT(RT_DEBUG_TRACE, ("AUTH - Bssid IF didn't up yet.\n"));
	   	return;
	} /* End of if */


	pEntry = MacTableLookup(pAd, Addr2);
	if (pEntry && IS_ENTRY_CLIENT(pEntry))
	{
	
		if (!RTMPEqualMemory(Addr1, pAd->ApCfg.MBSSID[pEntry->apidx].Bssid, MAC_ADDR_LEN))
		{					
			MacTableDeleteEntry(pAd, pEntry->Aid, pEntry->Addr);
			pEntry = NULL;
			DBGPRINT(RT_DEBUG_WARN, ("AUTH - Bssid does not match\n"));				
		}
		else
		{
			if (pEntry->bIAmBadAtheros == TRUE)
			{
				AsicUpdateProtect(pAd, 8, ALLN_SETPROTECT, FALSE, FALSE);
				DBGPRINT(RT_DEBUG_TRACE, ("Atheros Problem. Turn on RTS/CTS!!!\n"));
				pEntry->bIAmBadAtheros = FALSE;
			}

#ifdef DOT11_N_SUPPORT
			BASessionTearDownALL(pAd, pEntry->Aid);
#endif /* DOT11_N_SUPPORT */
			ASSERT(pEntry->Aid == Elem->Wcid);
		}
	}


	pRcvHdr = (PHEADER_802_11)(Elem->Msg);
	DBGPRINT(RT_DEBUG_TRACE,
			("AUTH - MBSS(%d), Rcv AUTH seq#%d, Alg=%d, Status=%d from "
			"[wcid=%d]%02x:%02x:%02x:%02x:%02x:%02x\n",
			apidx, Seq, Alg, Status, Elem->Wcid, PRINT_MAC(Addr2)));

#ifdef WSC_V2_SUPPORT
	/* Do not check ACL when WPS V2 is enabled and ACL policy is positive. */
	if ((pAd->ApCfg.MBSSID[apidx].WscControl.WscConfMode != WSC_DISABLE) &&
		(pAd->ApCfg.MBSSID[apidx].WscControl.WscV2Info.bEnableWpsV2) &&
		(pAd->ApCfg.MBSSID[apidx].WscControl.WscV2Info.bWpsEnable) &&
		(pAd->ApCfg.MBSSID[apidx].AccessControlList.Policy == 1))
		; 
	else
#endif /* WSC_V2_SUPPORT */
	/* fail in ACL checking => send an AUTH-Fail seq#2. */
    if (! ApCheckAccessControlList(pAd, Addr2, apidx))
    {
		ASSERT(Seq == 1);
		ASSERT(pEntry == NULL);
		APPeerAuthSimpleRspGenAndSend(pAd, pRcvHdr, Alg, Seq + 1, MLME_UNSPECIFY_FAIL);

		/* If this STA exists, delete it. */
		if (pEntry)
			MacTableDeleteEntry(pAd, pEntry->Aid, pEntry->Addr);

		RTMPSendWirelessEvent(pAd, IW_MAC_FILTER_LIST_EVENT_FLAG, Addr2, apidx, 0);

		DBGPRINT(RT_DEBUG_TRACE,
				("Failed in ACL checking => send an AUTH seq#2 with "
				"Status code = %d\n", MLME_UNSPECIFY_FAIL));
		return;
    }

#ifdef BAND_STEERING
	if (WMODE_CAP_N(pAd->CommonCfg.PhyMode))
		    bAllowStaConnectInHt = TRUE;

	BND_STRG_CHECK_CONNECTION_REQ(	pAd,
										NULL,
										Addr2,
										Elem->MsgType,
										Elem->Rssi0,
										Elem->Rssi1,
										Elem->Rssi2,
										bAllowStaConnectInHt,
										&bBndStrgCheck);
#ifdef BAND_STEERING_AUTH_REJ
	if (bBndStrgCheck == FALSE && pAd->CommonCfg.Channel <= 14) {
		APPeerAuthSimpleRspGenAndSend(pAd, pRcvHdr, Alg, Seq + 1, MLME_UNSPECIFY_FAIL);
		DBGPRINT(RT_DEBUG_TRACE, ("AUTH - BndStrg check failed.\n"));

                /* If this STA exists, delete it. */
                if (pEntry)
                        MacTableDeleteEntry(pAd, pEntry->Aid, pEntry->Addr);

                RTMPSendWirelessEvent(pAd, IW_MAC_FILTER_LIST_EVENT_FLAG, Addr2, apidx, 0);
		return;
	}
#endif /* BAND_STEERING_AUTH_REJ */
#endif /* BAND_STEERING */

#ifdef DOT11K_RRM_SUPPORT
	for (i = 0; i < MAX_MBSSID_NUM(pAd); i++) {
		if (pAd->OpMode == OPMODE_AP && IS_RRM_ENABLE(pAd, i) && pAd->CommonCfg.RRMFirstScan == TRUE) {
			DBGPRINT(RT_DEBUG_TRACE, ("RRM Enabled, wait bootup scan. Disallow new Association\n"));
			APPeerAuthSimpleRspGenAndSend(pAd, pRcvHdr, Alg, Seq + 1, MLME_UNSPECIFY_FAIL);
            		/* If this STA exists, delete it. */
            		if (pEntry)
                    		MacTableDeleteEntry(pAd, pEntry->Aid, pEntry->Addr);
			return;
		}
	}
#endif /* DOT11K_RRM_SUPPORT */

         /* YF@20130102: Refuse the weak signal of AuthReq */
         rssi = RTMPMaxRssi(pAd,  ConvertToRssi(pAd, (CHAR)Elem->Rssi0, RSSI_0),
                                  ConvertToRssi(pAd, (CHAR)Elem->Rssi1, RSSI_1),
                                  ConvertToRssi(pAd, (CHAR)Elem->Rssi2, RSSI_2));

         if ((pMbss->AuthFailRssiThreshold != 0 && rssi != 0 && rssi < pMbss->AuthFailRssiThreshold) ||
             (pMbss->AuthNoRspRssiThreshold != 0 && rssi != 0 && rssi < pMbss->AuthNoRspRssiThreshold))
         {
    		DBGPRINT(RT_DEBUG_TRACE, ("AUTH-MBSS(%d): AUTH_FAIL_REQ Threshold = %d, AUTH_NO_RSP_REQ Threshold = %d, AUTH RSSI = %d\n",
 				  apidx, pMbss->AuthFailRssiThreshold, pMbss->AuthNoRspRssiThreshold, rssi));

		if (pMbss->AuthFailRssiThreshold != 0 && rssi < pMbss->AuthFailRssiThreshold) {
            		DBGPRINT(RT_DEBUG_TRACE, ("Reject this AUTH_REQ due to Weak Signal.\n"));
                	APPeerAuthSimpleRspGenAndSend(pAd, pRcvHdr, Alg, Seq + 1, MLME_UNSPECIFY_FAIL);
		}

                /* If this STA exists, delete it. */
                if (pEntry)
                        MacTableDeleteEntry(pAd, pEntry->Aid, pEntry->Addr);

                RTMPSendWirelessEvent(pAd, IW_MAC_FILTER_LIST_EVENT_FLAG, Addr2, apidx, 0);
                return;
         }

#ifdef DOT11R_FT_SUPPORT
	pFtCfg = &pMbss->FtCfg;
	if ((pFtCfg->FtCapFlag.Dot11rFtEnable)
		&& (Alg == AUTH_MODE_FT))
	{
		USHORT result;

		if (!pEntry)
			pEntry = MacTableInsertEntry(pAd, Addr2, apidx, OPMODE_AP, TRUE);

		if (pEntry)
		{
		    os_alloc_mem(pAd, (UCHAR **)&pFtInfoBuf, sizeof(FT_INFO));

        	    if (pFtInfoBuf)
        	    {
    			result = FT_AuthReqHandler(pAd, pEntry, &FtInfo, pFtInfoBuf);
    			if (result == MLME_SUCCESS)
    			{
    				NdisMoveMemory(&pEntry->MdIeInfo, &FtInfo.MdIeInfo, sizeof(FT_MDIE_INFO));

    				pEntry->AuthState = AS_AUTH_OPEN;
    				pEntry->Sst = SST_AUTH;
    			}

    			FT_EnqueueAuthReply(pAd, pRcvHdr, Alg, 2, result,
    						&pFtInfoBuf->MdIeInfo, &pFtInfoBuf->FtIeInfo, NULL,
    						pFtInfoBuf->RSN_IE, pFtInfoBuf->RSNIE_Len);

			NdisZeroMemory(pEntry->LastTK, LEN_TK);
            		os_free_mem(NULL, pFtInfoBuf);
			if (result == MLME_SUCCESS) {
				/* Install pairwise key */
				WPAInstallPairwiseKey(pAd, pEntry->apidx, pEntry, TRUE);
				/* Update status */
				pEntry->WpaState = AS_PTKINITDONE;
				pEntry->GTKState = REKEY_ESTABLISHED;
			}
		    }
		}
		return;
	}
	else
#endif /* DOT11R_FT_SUPPORT */

	if ((Alg == AUTH_MODE_OPEN) &&
		(pAd->ApCfg.MBSSID[apidx].AuthMode != Ndis802_11AuthModeShared)) 
	{
		if (!pEntry)
			pEntry = MacTableInsertEntry(pAd, Addr2, apidx, OPMODE_AP, TRUE);

		if (pEntry)
		{
                        {
			pEntry->AuthState = AS_AUTH_OPEN;
			pEntry->Sst = SST_AUTH; /* what if it already in SST_ASSOC ??????? */
                        }
			APPeerAuthSimpleRspGenAndSend(pAd, pRcvHdr, Alg, Seq + 1, MLME_SUCCESS);

		}
		/* else  MAC table full, what should we respond ????? */
	}
	else if ((Alg == AUTH_MODE_KEY) && 
				((pAd->ApCfg.MBSSID[apidx].AuthMode == Ndis802_11AuthModeShared)
				|| (pAd->ApCfg.MBSSID[apidx].AuthMode == Ndis802_11AuthModeAutoSwitch)))
	{
		if (!pEntry)
			pEntry = MacTableInsertEntry(pAd, Addr2, apidx, OPMODE_AP, TRUE);

		if (pEntry)
		{
			pEntry->AuthState = AS_AUTHENTICATING;
			pEntry->Sst = SST_NOT_AUTH; /* what if it already in SST_ASSOC ??????? */

			/* log this STA in AuthRspAux machine, only one STA is stored. If two STAs using */
			/* SHARED_KEY authentication mingled together, then the late comer will win. */
			COPY_MAC_ADDR(&pAd->ApMlmeAux.Addr, Addr2);
			for(i=0; i<CIPHER_TEXT_LEN; i++) 
				pAd->ApMlmeAux.Challenge[i] = RandomByte(pAd);

			RspReason = 0;
			Seq++;
  
			NStatus = MlmeAllocateMemory(pAd, &pOutBuffer);
			if(NStatus != NDIS_STATUS_SUCCESS) 
				return;  /* if no memory, can't do anything */

			DBGPRINT(RT_DEBUG_TRACE, ("AUTH - Send AUTH seq#2 (Challenge)\n"));

			MgtMacHeaderInit(pAd, &AuthHdr, SUBTYPE_AUTH, 0, 	Addr2, 
								pAd->ApCfg.MBSSID[apidx].Bssid);
			MakeOutgoingFrame(pOutBuffer,            &FrameLen,
								sizeof(HEADER_802_11), &AuthHdr,
								2,                     &Alg,
								2,                     &Seq,
								2,                     &RspReason,
								1,                     &ChTxtIe,
								1,                     &ChTxtLen,
								CIPHER_TEXT_LEN,       pAd->ApMlmeAux.Challenge,
								END_OF_ARGS);
			MiniportMMRequest(pAd, 0, pOutBuffer, FrameLen);
			MlmeFreeMemory(pAd, pOutBuffer);
		}
		/* else  MAC table full, what should we respond ????? */
	}
	else
	{
		/* wrong algorithm */
		APPeerAuthSimpleRspGenAndSend(pAd, pRcvHdr, Alg, Seq + 1, MLME_ALG_NOT_SUPPORT);

		/* If this STA exists, delete it. */
		if (pEntry)
			MacTableDeleteEntry(pAd, pEntry->Aid, pEntry->Addr);

		DBGPRINT(RT_DEBUG_TRACE, ("AUTH - Alg=%d, Seq=%d, AuthMode=%d\n",
				Alg, Seq, pAd->ApCfg.MBSSID[apidx].AuthMode));
	}
}


static VOID APPeerAuthConfirmAction(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem)
{
	USHORT          Seq, Alg, Status;
	UCHAR           Addr2[MAC_ADDR_LEN];
	PHEADER_802_11  pRcvHdr;
	CHAR            Chtxt[CIPHER_TEXT_LEN];
	MAC_TABLE_ENTRY *pEntry;
	UCHAR			Addr1[MAC_ADDR_LEN];
	UINT32			apidx;

#ifdef DOT11R_FT_SUPPORT
	PFT_CFG pFtCfg;
	FT_INFO FtInfo;
	PFT_INFO pFtInfoBuf;
#endif /* DOT11R_FT_SUPPORT */

	if (pAd == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: pAd is NULL\n",__FUNCTION__));
 		return;
	}

	if (! APPeerAuthSanity(pAd, Elem->Msg, Elem->MsgLen, Addr1, Addr2, &Alg, &Seq, &Status, Chtxt
#ifdef DOT11R_FT_SUPPORT
							,&FtInfo
#endif /* DOT11R_FT_SUPPORT */
	    ))
		return;

	for (apidx=0; apidx<pAd->ApCfg.BssidNum; apidx++)
	{	
		if (RTMPEqualMemory(Addr1, pAd->ApCfg.MBSSID[apidx].Bssid, MAC_ADDR_LEN))
			break;
	}

	if (apidx >= pAd->ApCfg.BssidNum)
	{	
		DBGPRINT(RT_DEBUG_TRACE, ("AUTH - Bssid not found\n"));
		return;
	}

	if (apidx >= HW_BEACON_MAX_NUM)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Index out of bound\n"));
		return;
	}

	if ((pAd->ApCfg.MBSSID[apidx].MSSIDDev != NULL) &&
		!(RTMP_OS_NETDEV_STATE_RUNNING(pAd->ApCfg.MBSSID[apidx].MSSIDDev)))
	{
    	DBGPRINT(RT_DEBUG_TRACE, ("AUTH - Bssid IF didn't up yet.\n"));
	   	return;
	} /* End of if */

	if (Elem->Wcid >= MAX_LEN_OF_MAC_TABLE)
	{
    	DBGPRINT(RT_DEBUG_ERROR, ("AUTH - Invalid wcid (%d).\n", Elem->Wcid));		
		return; 
	}

	pEntry = &pAd->MacTab.Content[Elem->Wcid];
	if (pEntry && IS_ENTRY_CLIENT(pEntry))
	{
		if (!RTMPEqualMemory(Addr1, pAd->ApCfg.MBSSID[pEntry->apidx].Bssid, MAC_ADDR_LEN))
		{
			MacTableDeleteEntry(pAd, pEntry->Aid, pEntry->Addr);
			pEntry = NULL;
			DBGPRINT(RT_DEBUG_WARN, ("AUTH - Bssid does not match\n"));
		}
		else
		{
			if (pEntry->bIAmBadAtheros == TRUE)
			{
				AsicUpdateProtect(pAd, 8, ALLN_SETPROTECT, FALSE, FALSE);
				DBGPRINT(RT_DEBUG_TRACE, ("Atheros Problem. Turn on RTS/CTS!!!\n"));
				pEntry->bIAmBadAtheros = FALSE;
			}
#ifdef DOT11_N_SUPPORT
			BASessionTearDownALL(pAd, pEntry->Aid);
#endif /* DOT11_N_SUPPORT */
			ASSERT(pEntry->Aid == Elem->Wcid);
		}
	}

    pRcvHdr = (PHEADER_802_11)(Elem->Msg);

	DBGPRINT(RT_DEBUG_TRACE,
			("AUTH - MBSS(%d), Rcv AUTH seq#%d, Alg=%d, Status=%d from "
			"[wcid=%d]%02x:%02x:%02x:%02x:%02x:%02x\n",
			apidx, Seq, Alg, Status, Elem->Wcid, PRINT_MAC(Addr2)));

	if (pEntry && MAC_ADDR_EQUAL(Addr2, pAd->ApMlmeAux.Addr)) 
	{
#ifdef DOT11R_FT_SUPPORT
		pFtCfg = &pAd->ApCfg.MBSSID[apidx].FtCfg;
		if ((pFtCfg->FtCapFlag.Dot11rFtEnable) && (Alg == AUTH_MODE_FT))
		{
			USHORT result;

            os_alloc_mem(pAd, (UCHAR **)&pFtInfoBuf, sizeof(FT_INFO));
            if (pFtInfoBuf)
            {
                NdisZeroMemory(pFtInfoBuf, sizeof(FT_INFO));
                os_alloc_mem(pAd, (UCHAR **)&(pFtInfoBuf->RicInfo.pRicInfo), 512);
                if (pFtInfoBuf->RicInfo.pRicInfo != NULL)
                {
                    result = FT_AuthConfirmHandler(pAd, pEntry, &FtInfo,	pFtInfoBuf);
    				FT_EnqueueAuthReply(pAd, pRcvHdr, Alg, 4, result,
    					&pFtInfoBuf->MdIeInfo, &pFtInfoBuf->FtIeInfo,
    					&pFtInfoBuf->RicInfo, pFtInfoBuf->RSN_IE, pFtInfoBuf->RSNIE_Len);
    				os_free_mem(NULL, pFtInfoBuf->RicInfo.pRicInfo);
                }
                os_free_mem(NULL, pFtInfoBuf);
			}
			else
			{
				return;
			}
		}
		else
#endif /* DOT11R_FT_SUPPORT */
		if ((pRcvHdr->FC.Wep == 1) &&
			NdisEqualMemory(Chtxt, pAd->ApMlmeAux.Challenge, CIPHER_TEXT_LEN)) 
		{
			/* Successful */
			APPeerAuthSimpleRspGenAndSend(pAd, pRcvHdr, Alg, Seq + 1, MLME_SUCCESS);
			pEntry->AuthState = AS_AUTH_KEY;
			pEntry->Sst = SST_AUTH;
		}
		else 
		{
	
			/* send wireless event - Authentication rejected because of challenge failure */
			RTMPSendWirelessEvent(pAd, IW_AUTH_REJECT_CHALLENGE_FAILURE, pEntry->Addr, 0, 0);  

			/* fail - wep bit is not set or challenge text is not equal */
			APPeerAuthSimpleRspGenAndSend(pAd, pRcvHdr, Alg, Seq + 1, MLME_REJ_CHALLENGE_FAILURE);
			MacTableDeleteEntry(pAd, pEntry->Aid, pEntry->Addr);

			/*Chtxt[127]='\0'; */
			/*pAd->ApMlmeAux.Challenge[127]='\0'; */
			DBGPRINT(RT_DEBUG_TRACE,
					("%s\n", ((pRcvHdr->FC.Wep == 1) ? "challenge text is not equal" : "wep bit is not set")));
			/*DBGPRINT(RT_DEBUG_TRACE, ("Sent Challenge = %s\n",&pAd->ApMlmeAux.Challenge[100])); */
			/*DBGPRINT(RT_DEBUG_TRACE, ("Rcv Challenge = %s\n",&Chtxt[100])); */
		}
	} 
	else 
	{
		/* fail for unknown reason. most likely is AuthRspAux machine be overwritten by another */
		/* STA also using SHARED_KEY authentication */
		APPeerAuthSimpleRspGenAndSend(pAd, pRcvHdr, Alg, Seq + 1, MLME_UNSPECIFY_FAIL);

		/* If this STA exists, delete it. */
		if (pEntry)
			MacTableDeleteEntry(pAd, pEntry->Aid, pEntry->Addr);
	}
}


/*
    ==========================================================================
    Description:
        Some STA/AP
    Note:
        This action should never trigger AUTH state transition, therefore we
        separate it from AUTH state machine, and make it as a standalone service
    ==========================================================================
 */
VOID APCls2errAction(
    IN PRTMP_ADAPTER pAd, 
	IN 	ULONG Wcid, 
    IN	PHEADER_802_11	pHeader) 
{
	HEADER_802_11 Hdr;
	PUCHAR        pOutBuffer = NULL;
	NDIS_STATUS   NStatus;
	ULONG         FrameLen = 0;
	USHORT        Reason = REASON_CLS2ERR;
	MAC_TABLE_ENTRY *pEntry = NULL;
	UCHAR idx;

	if (Wcid < MAX_LEN_OF_MAC_TABLE)
	{
		pEntry = &(pAd->MacTab.Content[Wcid]);
	}

	if (pEntry && IS_ENTRY_CLIENT(pEntry))
	{
		/*ApLogEvent(pAd, pAddr, EVENT_DISASSOCIATED); */
		MacTableDeleteEntry(pAd, pEntry->Aid, pHeader->Addr2);
	}
	else
	{
		for (idx = 0; idx < pAd->ApCfg.BssidNum; idx++)
		{
			PMULTISSID_STRUCT pMbss = &pAd->ApCfg.MBSSID[idx];

			if (NdisEqualMemory(pMbss->Bssid, pHeader->Addr1, MAC_ADDR_LEN))
				break;
		}

		if (idx >= pAd->ApCfg.BssidNum) {
			DBGPRINT(RT_DEBUG_INFO,("AUTH - Class 2 error but not my bssid %02x:%02x:%02x:%02x:%02x:%02x\n", PRINT_MAC(pHeader->Addr1)));
			return;
		}
	}

	/* send out DEAUTH request frame */
	NStatus = MlmeAllocateMemory(pAd, &pOutBuffer);
	if (NStatus != NDIS_STATUS_SUCCESS) 
		return;

	DBGPRINT(RT_DEBUG_TRACE,
			("AUTH - Class 2 error, Send DEAUTH frame to "
			"%02x:%02x:%02x:%02x:%02x:%02x\n",
			PRINT_MAC(pHeader->Addr2)));

	MgtMacHeaderInit(pAd, &Hdr, SUBTYPE_DEAUTH, 0, pHeader->Addr2, 
						pHeader->Addr1);
	MakeOutgoingFrame(pOutBuffer,            &FrameLen, 
					  sizeof(HEADER_802_11), &Hdr, 
					  2,                     &Reason, 
					  END_OF_ARGS);
	MiniportMMRequest(pAd, 0, pOutBuffer, FrameLen);
	MlmeFreeMemory(pAd, pOutBuffer);
}


/*
    ==========================================================================
    Description:
        Send out a Authentication (response) frame
    ==========================================================================
*/
VOID APPeerAuthSimpleRspGenAndSend(
    IN PRTMP_ADAPTER pAd, 
    IN PHEADER_802_11 pHdr, 
    IN USHORT Alg, 
    IN USHORT Seq, 
    IN USHORT StatusCode) 
{
    HEADER_802_11     AuthHdr;
    ULONG             FrameLen = 0;
    PUCHAR            pOutBuffer = NULL;
    NDIS_STATUS       NStatus;


    NStatus = MlmeAllocateMemory(pAd, &pOutBuffer);
    if (NStatus != NDIS_STATUS_SUCCESS) 
        return;

    if (StatusCode == MLME_SUCCESS)
    {
        DBGPRINT(RT_DEBUG_TRACE, ("AUTH_RSP - Send AUTH response (SUCCESS)...\n"));
	}
    else
    {
        /* For MAC wireless client(Macintosh), need to send AUTH_RSP with Status Code (fail reason code) to reject it. */
        DBGPRINT(RT_DEBUG_TRACE, ("AUTH_RSP - Peer AUTH fail (Status = %d)...\n", StatusCode));
    }

	MgtMacHeaderInit(pAd, &AuthHdr, SUBTYPE_AUTH, 0, pHdr->Addr2, 
						pHdr->Addr1);
	MakeOutgoingFrame(pOutBuffer,				&FrameLen,
					  sizeof(HEADER_802_11),	&AuthHdr,
					  2,						&Alg,
					  2,						&Seq,
					  2,						&StatusCode,
					  END_OF_ARGS);
	MiniportMMRequest(pAd, 0, pOutBuffer, FrameLen);
	MlmeFreeMemory(pAd, pOutBuffer);
}

/* End of ap_auth.c */
