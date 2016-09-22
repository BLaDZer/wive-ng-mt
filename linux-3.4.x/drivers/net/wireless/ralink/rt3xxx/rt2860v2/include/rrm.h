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


#ifndef __RRM_H
#define __RRM_H

#ifdef DOT11K_RRM_SUPPORT
#include "rtmp_type.h"

#define RRM_CAP_BIT                     (1 << 12) /* Bit 12 in Capability information field. */
#define IE_RRM_EN_CAP                   70      /* 802.11k. RRM Enable Capability element. */

typedef union GNU_PACKED __RRM_EN_CAP_IE
{
	struct GNU_PACKED
	{
#ifdef RT_BIG_ENDIAN
		UINT64 :30;
		UINT64 AntennaInfoCap:1;
		UINT64 BssAvaiableAcmCap:1;
		UINT64 BssAvgAccessDelayCap:1;
		UINT64 RSNIMeasureCap:1;
		UINT64 RCPIMeasureCap:1;
		UINT64 NeighReportTSFOffsetCap:1;
		UINT64 MeasurePilotTxInfoCap:1;
		UINT64 MeasurePilotCap:3;
		UINT64 NotOperatingChMaxMeasureDuration:3;
		/*UINT64 RRMMibCap:1; */
		UINT64 OperatingChMaxMeasureDuration:3;
		UINT64 RRMMibCap:1;
		UINT64 APChannelReportCap:1;
		UINT64 TriggeredTransmitStreamCap:1;
		UINT64 TransmitStreamCap:1;
		UINT64 LCIAzimuthCap:1;
		UINT64 LCIMeasureCap:1;
		UINT64 StatisticMeasureCap:1;
		UINT64 NoiseHistogramMeasureCap:1;
		UINT64 ChannelLoadMeasureCap:1;
		UINT64 FrameMeasureCap:1;
		UINT64 BeaconMeasureReportCndCap:1;
		UINT64 BeaconTabMeasureCap:1;
		UINT64 BeaconActiveMeasureCap:1;
		UINT64 BeaconPassiveMeasureCap:1;
		UINT64 RepeatMeasureCap:1;
		UINT64 ParallelMeasureCap:1;
		UINT64 NeighborRepCap:1;
		UINT64 LinkMeasureCap:1;
#else
		UINT64 LinkMeasureCap:1;
		UINT64 NeighborRepCap:1;
		UINT64 ParallelMeasureCap:1;
		UINT64 RepeatMeasureCap:1;
		UINT64 BeaconPassiveMeasureCap:1;
		UINT64 BeaconActiveMeasureCap:1;
		UINT64 BeaconTabMeasureCap:1;
		UINT64 BeaconMeasureReportCndCap:1;
		UINT64 FrameMeasureCap:1;
		UINT64 ChannelLoadMeasureCap:1;
		UINT64 NoiseHistogramMeasureCap:1;
		UINT64 StatisticMeasureCap:1;
		UINT64 LCIMeasureCap:1;
		UINT64 LCIAzimuthCap:1;
		UINT64 TransmitStreamCap:1;
		UINT64 TriggeredTransmitStreamCap:1;
		UINT64 APChannelReportCap:1;
		UINT64 RRMMibCap:1;
		UINT64 OperatingChMaxMeasureDuration:3;
		UINT64 NotOperatingChMaxMeasureDuration:3;
		UINT64 MeasurePilotCap:3;
		UINT64 MeasurePilotTxInfoCap:1;
		UINT64 NeighReportTSFOffsetCap:1;
		UINT64 RCPIMeasureCap:1;
		UINT64 RSNIMeasureCap:1;
		UINT64 BssAvgAccessDelayCap:1;
		UINT64 BssAvaiableAcmCap:1;
		UINT64 AntennaInfoCap:1;
		UINT64 :30;
#endif
	} field;
	UINT64 word;
} RRM_EN_CAP_IE, *PRRM_EN_CAP_IE;

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
	IN INT BssIdx);

#endif
#endif /* __RRM_H */
