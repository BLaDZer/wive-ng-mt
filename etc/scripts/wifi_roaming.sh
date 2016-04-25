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
eval `nvram_buf_get 2860 ApProbeRspTimes AuthRspFail AuthRspRssi AssocReqRssiThres AssocRspIgnor KickStaRssiLow KickStaRssiLowPSM KickStaRssiLowDelay ProbeRspRssi`
#################################################################################################################################################
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
################################################################################################################################
