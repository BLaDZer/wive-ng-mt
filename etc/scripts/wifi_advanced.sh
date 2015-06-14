#!/bin/sh

# include kernel config
. /etc/scripts/config.sh

is_up=`ip link show $1 up`
if [ "$is_up" = "" ]; then
    echo ">>>>> Wifi interface $1 not up - skip tune <<<<<<<<<<"
    exit 0
fi

echo ">>>>> RECONFIGURE WIFI IF = $1 <<<<<<<<<<"

######################################################################################################
eval `nvram_buf_get 2860 OperationMode RadioOff RadioOffINIC AutoConnect DyncVgaEnable M2UEnabled`
########################################STAMODE param#################################################
if [ "$OperationMode" = "2" ]; then
    if [ "$AutoConnect" != "" ]; then
	iwpriv "$1" set AutoReconnect="$AutoConnect"
    fi
  # in sta mode exit
  exit 0
fi
########################################APMODE param#################################################
#########################################ON/OFF param################################################
if [ "$RadioOff" = "1" -a "$2" != "5GHZ" ] || [ "$RadioOffINIC" = "1" -a "$2" = "5GHZ" ]; then
    iwpriv "$1" set RadioOn=0
    echo ">>>> WIFI $1 DISABLED <<<<"
    exit 0
else
    iwpriv "$1" set RadioOn=1
fi
########################################MULTICAST param##############################################
if [ "$CONFIG_RT2860V2_AP_IGMP_SNOOP" != "" ]; then
    # in bridged mode direct enable Multicast2Unicast in wifi drivers if enabled
    # in others modes auto enable by igmpproxy
    if [ "$OperationMode" = "0" -o "$OperationMode" = "3" ]; then
	if [ "$M2UEnabled" = "1" ]; then
	    iwpriv "$1" set IgmpSnEnable=1
	else
	    iwpriv "$1" set IgmpSnEnable=0
	fi
    fi
fi
#####################################DYNAMIC VARIABLE GAIN############################################
if [ "$DyncVgaEnable" = "1" ]; then
    iwpriv "$1" set DyncVgaEnable=1
else
    iwpriv "$1" set DyncVgaEnable=0
fi
################WORKAROUND FOR TX RING FULL IN WIFI DRIVERS (ONLY 2.4GHz)#############################
if [ "$2" != "5GHZ" ]; then
    # allow fallback to MCS0
    iwpriv $1 set AutoFallBack=1
    # decrease tx retry for decrease tx buffer free time
    iwpriv $1 set LongRetry=3
    iwpriv $1 set ShortRetry=3
fi
