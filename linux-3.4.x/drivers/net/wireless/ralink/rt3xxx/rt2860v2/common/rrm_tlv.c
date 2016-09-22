/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2006, Ralink Technology, Inc.
 *
 * All rights reserved.	Ralink's source	code is	an unpublished work	and	the
 * use of a	copyright notice does not imply	otherwise. This	source code
 * contains	confidential trade secret material of Ralink Tech. Any attemp
 * or participation	in deciphering,	decoding, reverse engineering or in	any
 * way altering	the	source code	is stricitly prohibited, unless	the	prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************/

/****************************************************************************
	Abstract:

***************************************************************************/

#ifdef DOT11K_RRM_SUPPORT

#include "rt_config.h"

/*
	==========================================================================
	Description:
		Insert RRM Enable Capabilitys IE into frame.

	Parametrs:
		1. frame buffer pointer.
		2. frame length.

	Return	: None.
	==========================================================================
 */
VOID RRM_InsertRRMEnCapIE(
	IN PRTMP_ADAPTER pAd,
	OUT PUCHAR pFrameBuf,
	OUT PULONG pFrameLen,
	IN INT BssIdx)
{
	ULONG TempLen;
	UINT8 Len = 5;
	UINT8 ElementID = IE_RRM_EN_CAP;
	RRM_EN_CAP_IE RrmEnCap;
	PMULTISSID_STRUCT pMBss = NULL;

	if (BssIdx < MAX_MBSSID_NUM(pAd))
		pMBss = &pAd->ApCfg.MBSSID[BssIdx];
	else
		return;

	RrmEnCap.field.LinkMeasureCap = 0;
	RrmEnCap.field.NeighborRepCap = 0;
	RrmEnCap.field.ParallelMeasureCap = 0;
	RrmEnCap.field.RepeatMeasureCap = 0;
	RrmEnCap.field.BeaconPassiveMeasureCap = 0;
	RrmEnCap.field.BeaconActiveMeasureCap = 0;
	RrmEnCap.field.BeaconTabMeasureCap = 0;
	RrmEnCap.field.BeaconMeasureReportCndCap = 0;
	RrmEnCap.field.FrameMeasureCap = 0;
	RrmEnCap.field.ChannelLoadMeasureCap = 0;
	RrmEnCap.field.NoiseHistogramMeasureCap = 0;
	RrmEnCap.field.StatisticMeasureCap = 0;
	RrmEnCap.field.LCIMeasureCap = 0;
	RrmEnCap.field.LCIAzimuthCap = 0;
	RrmEnCap.field.TransmitStreamCap = 0;
	RrmEnCap.field.TriggeredTransmitStreamCap = 0;
	RrmEnCap.field.APChannelReportCap = 1;
	RrmEnCap.field.RRMMibCap = 0;
	RrmEnCap.field.OperatingChMaxMeasureDuration = 0;
	RrmEnCap.field.NotOperatingChMaxMeasureDuration = 0;
	RrmEnCap.field.MeasurePilotCap = 0;
	RrmEnCap.field.MeasurePilotTxInfoCap = 0;
	RrmEnCap.field.NeighReportTSFOffsetCap = 0;
	RrmEnCap.field.RCPIMeasureCap = 0;
	RrmEnCap.field.RSNIMeasureCap = 0;
	RrmEnCap.field.BssAvgAccessDelayCap = 0;
	RrmEnCap.field.BssAvaiableAcmCap = 0;
	RrmEnCap.field.AntennaInfoCap = 0;

	MakeOutgoingFrame(pFrameBuf,					&TempLen,
						1,							&ElementID,
						1,							&Len,
						Len,						(PUCHAR)&RrmEnCap.word,
						END_OF_ARGS);

	*pFrameLen = *pFrameLen + TempLen;

	return;
}
#endif /* DOT11K_RRM_SUPPORT */
