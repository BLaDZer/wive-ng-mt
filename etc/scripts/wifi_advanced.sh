#!/bin/sh

# include kernel config
. /etc/scripts/config.sh

is_up=`ip -o link show $1 up`
if [ "$is_up" = "" ]; then
    echo ">>>>> Wifi interface $1 not up - skip tune <<<<<<<<<<"
    exit 0
fi

LOG="logger -t advanced"
$LOG "Tune wifi advanced parametrs for $1."

################################################################################################################
eval `nvram_buf_get 2860 OperationMode RadioOn RadioOnINIC AutoConnect ApCliAutoConnect M2UEnabled \
	DyncVgaEnable SkipLongRangeVga VgaClamp`
################################################STAMODE param###################################################
if [ "$OperationMode" = "2" ]; then
    if [ "$AutoConnect" = "1" ]; then
	iwpriv "$1" set AutoReconnect="$AutoConnect"
    fi
    # in sta mode exit
    exit 0
fi
################################################APMODE param####################################################
if [ "$RadioOn" = "0" -a "$2" != "5GHZ" ] || [ "$RadioOnINIC" = "0" -a "$2" = "5GHZ" ]; then
    iwpriv "$1" set RadioOn=0
    echo ">>>> WIFI $1 DISABLED <<<<"
    exit 0
fi
################################################APCLI param#####################################################
if [ "$OperationMode" = "3" ] && [ "$1" != "ra0" ] && [ "$1" != "rai0" ] && [ "$ApCliAutoConnect" = "1" ]; then
    iwpriv "$1" set ApCliAutoConnect=1
    echo ">>>> APCLI $1 START CONNECT <<<<"
    exit 0
fi
################################################MULTICAST PARAMS################################################
if [ "$CONFIG_MT7610_AP_IGMP_SNOOP" != "" ] || [ "$CONFIG_MT76X2_AP_IGMP_SNOOP" != "" ] || [ "$CONFIG_MT76X3_AP_IGMP_SNOOP" != "" ] || [ "$CONFIG_MT7615_AP_IGMP_SNOOP" != "" ]; then
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

# MT7603/MT7615 do not support DYNVGA&Co
if [ "$CONFIG_RT_FIRST_IF_MT7603E" = "y" -o "$CONFIG_RT_FIRST_IF_MT7615E" = "y" ] && [ "$1" = "ra0" ]; then
    exit 0
fi

if [ "$CONFIG_RT_SECOND_IF_MT7615E" = "y" ] && [ "$1" = "rai0" ]; then
    exit 0
fi

# enable/disable dynamic LNA gain
if [ "$DyncVgaEnable" = "1" ]; then
    iwpriv "$1" set DyncVgaEnable=1
else
    iwpriv "$1" set DyncVgaEnable=0
fi

if [ "$CONFIG_RT_FIRST_IF_MT7602E" = "y" ] || [ "$CONFIG_RT_SECOND_IF_MT7612E" = "y" ] || [ "$CONFIG_RT_SECOND_IF_MT7610E" = "y" -a "$2" = "5GHZ" ]; then
    # skip tune gain for long distanse clients
    if [ "$SkipLongRangeVga" = "1" ]; then
	iwpriv "$1" set SkipLongRangeVga=1
    else
	iwpriv "$1" set SkipLongRangeVga=0
    fi
fi
if [ "$CONFIG_RT_FIRST_IF_MT7602E" = "y" ] || [ "$CONFIG_RT_SECOND_IF_MT7612E" = "y" ]; then
    # limit dynamic maximum gain to reduce impact interference
    # 1 - -4dB, 2 - -8dB, 3 - -12dB, 4 - -16dB
    if [ "$VgaClamp" != "" ] && [ "$VgaClamp" != "0" ]; then
	iwpriv "$1" set VgaClamp="$VgaClamp"
    else
	iwpriv "$1" set VgaClamp=0
    fi
fi
