#!/bin/sh

# include kernel config
. /etc/scripts/config.sh

is_up=`ip -o link show $1 up`
if [ "$is_up" = "" ]; then
    echo ">>>>> Wifi interface $1 not up - skip roaming tune <<<<<<<<<<"
    exit 0
fi

LOG="logger -t roaming"
$LOG "Tune wifi roaming parametrs for $1."

###############################################PARAMETRS TABLE###################################################################################
# ApProbeRspTimes	- range 0 - 10 times, limit probe reqest per client, default 3
# AuthRspFail		- range 0 - -100 dBm, reject auth req due to weak signal, default 0 (off)
# AuthRspRssi		- range 0 - -100 dBm, ignore auth req due to weak signal, default 0 (off)
# AssocReqRssiThres	- range 0 - -100 dBm, reject assoc req due to weak signal, default 0 (off)
# AssocRspIgnor		- range 0 - -100 dBm, ignore assoc req due to weak signal, default 0 (off)
# ProbeRspRssi		- range 0 - -100 dBm, auto disonnect sta if rssi low at probe requests, default 0 (off)
# KickStaRssiLow	- range 0 - -100 dBm, auto disonnect sta if rssi low (active clients), default 0 (off)
# KickStaRssiLow	- range 0 - -100 dBm, auto disonnect sta if rssi low (PSM clients), default 0 (off)
# KickStaRssiLowDelay	- range 0 -  200 seconds, if in this interval all data frames have low rssi - kick STA, default 5
#################################################################################################################################################
eval `nvram_buf_get 2860 BandDeltaRssi ApProbeRspTimes AuthRspFail AuthRspRssi AssocReqRssiThres AssocRspIgnor \
			    KickStaRssiLow KickStaRssiLowPSM ProbeRspRssi KickStaRssiLowDelay`
#################################################################################################################################################

if [ "$2" = "5GHZ" ]; then
    if [ "$BandDeltaRssi" = "" ]; then
	BandDeltaRssi=-5
    fi
    # calculate roaming parametrs for second band
    # limit parametrs to minimul value
    # min RSSI
    minrssi=-100

    if [ "$AuthRspFail" != "" ] && [ "$AuthRspFail" != "0" ]; then
	let AuthRspFail=AuthRspFail+BandDeltaRssi
	if [ $AuthRspFail -lt $minrssi ]; then
	    AuthRspFail=$minrssi
	fi
    fi
    if [ "$AuthRspRssi" != "" ] && [ "$AuthRspRssi" != "0" ]; then
	let AuthRspRssi=AuthRspRssi+BandDeltaRssi
	if [ $AuthRspRssi -lt $minrssi ]; then
	    AuthRspRssi=$minrssi
	fi
    fi
    if [ "$AssocReqRssiThres" != "" ] && [ "$AssocReqRssiThres" != "0" ]; then
	let AssocReqRssiThres=AssocReqRssiThres+BandDeltaRssi
	if [ $AssocReqRssiThres -lt $minrssi ]; then
	    AssocReqRssiThres=$minrssi
	fi
    fi
    if [ "$AssocRspIgnor" != "" ] && [ "$AssocRspIgnor" != "0" ]; then
	let AssocRspIgnor=AssocRspIgnor+BandDeltaRssi
	if [ $AssocRspIgnor -lt $minrssi ]; then
	    AssocRspIgnor=$minrssi
	fi
    fi
    if [ "$KickStaRssiLow" != "" ] && [ "$KickStaRssiLow" != "0" ]; then
	let KickStaRssiLow=KickStaRssiLow+BandDeltaRssi
	if [ $KickStaRssiLow -lt $minrssi ]; then
	    KickStaRssiLow=$minrssi
	fi
    fi
    if [ "$KickStaRssiLowPSM" != "" ] && [ "$KickStaRssiLowPSM" != "0" ]; then
	let KickStaRssiLowPSM=KickStaRssiLowPSM+BandDeltaRssi
	if [ $KickStaRssiLowPSM -lt $minrssi ]; then
	    KickStaRssiLowPSM=$minrssi
	fi
    fi
    if [ "$ProbeRspRssi" != "" ] && [ "$ProbeRspRssi" != "0" ]; then
	let ProbeRspRssi=ProbeRspRssi+BandDeltaRssi
	if [ $ProbeRspRssi -lt $minrssi ]; then
	    ProbeRspRssi=$minrssi
	fi
    fi
fi

# set handover parametrs in driver
if [ "$ApProbeRspTimes" != "" ]; then
    iwpriv "$1" set ApProbeRspTimes="$ApProbeRspTimes"
fi
if [ "$AuthRspFail" != "" ]; then
    iwpriv "$1" set AuthRspFail="$AuthRspFail"
fi
if [ "$AuthRspRssi" != "" ]; then
    iwpriv "$1" set AuthRspRssi="$AuthRspRssi"
fi
if [ "$AssocReqRssiThres" != "" ]; then
    iwpriv "$1" set AssocReqRssiThres="$AssocReqRssiThres"
fi
if [ "$AssocRspIgnor" != "" ]; then
    iwpriv "$1" set AssocRspIgnor="$AssocRspIgnor"
fi
if [ "$ProbeRspRssi" != "" ]; then
    iwpriv "$1" set ProbeRspRssi="$ProbeRspRssi"
fi
if [ "$KickStaRssiLow" != "" ]; then
    iwpriv "$1" set KickStaRssiLow="$KickStaRssiLow"
fi
if [ "$KickStaRssiLowPSM" != "" ]; then
    iwpriv "$1" set KickStaRssiLowPSM="$KickStaRssiLowPSM"
fi
if [ "$KickStaRssiLowDelay" != "" ]; then
    iwpriv "$1" set KickStaRssiLowDelay="$KickStaRssiLowDelay"
fi
$LOG "Roaming parametrs mask for $1: $ApProbeRspTimes;$AuthRspFail;$AuthRspRssi;$AssocReqRssiThres;$AssocRspIgnor;$KickStaRssiLow;$KickStaRssiLowPSM;$KickStaRssiLowDelay;$ProbeRspRssi"
