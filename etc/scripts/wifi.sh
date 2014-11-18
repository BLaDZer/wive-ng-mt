#!/bin/sh

# include kernel config
. /etc/scripts/config.sh

echo ">>>>> RECONFIGURE WIFI IF = $1 <<<<<<<<<<"

#################################################################################################
eval `nvram_buf_get 2860 OperationMode AutoConnect AutoChannelSelect RadioOff GreenAP HT_OpMode \
	DyncVgaEnable MO_FalseCCATh MO_LowFalseCCATh \
	AP2040Rescan HT_BSSCoexistence \
	McastPhyMode McastMcs M2UEnabled igmpEnabled ApCliBridgeOnly`
#####################WORKAROUND FOR TX RING FULL IN WIFI DRIVERS#################################
# allow fallback to MCS0
iwpriv $1 set AutoFallBack=1
# decrease tx retry for decrease tx buffer free time
iwpriv $1 set LongRetry=3
iwpriv $1 set ShortRetry=3
########################################STAMODE param############################################
if [ "$OperationMode" = "2" ]; then
    if [ "$AutoConnect" != "" ]; then
	iwpriv "$1" set AutoReconnect="$AutoConnect"
    fi
  # in sta mode exit
  exit 0
fi
########################################APMODE param#############################################
#########################################ON/OFF param############################################
if [ "$RadioOff" = "1" ]; then
    iwpriv "$1" set RadioOn=0
    echo ">>>> WIFI DISABLED <<<<"
    exit 0
else
    iwpriv "$1" set RadioOn=1
fi
########################################MULTICAST param##########################################
if [ "$CONFIG_RT2860V2_AP_IGMP_SNOOP" != "" ]; then
    if [ "$McastPhyMode" != "" ]; then
        iwpriv "$1" set McastPhyMode="$McastPhyMode"
    fi
    if [ "$McastMcs" != "" ]; then
        iwpriv "$1" set McastMcs="$McastMcs"
    fi
fi
########################################GREEN mode###############################################
if [ "$CONFIG_RT2860V2_AP_GREENAP" != "" ]; then
    if [ "$HT_OpMode" = "1" ] || [ "$GreenAP" = "1" ]; then
	iwpriv "$1" set GreenAP=1
    else
	iwpriv "$1" set GreenAP=0
    fi
fi
#####################################Reduce interference#########################################
if [ "$CONFIG_RT2860V2_AP_INTERFERENCE_REDUCE" != "" ]; then
    if [ "$DyncVgaEnable" = "1" ]; then
	iwpriv "$1" set DyncVgaEnable=1
    else
	iwpriv "$1" set DyncVgaEnable=0
    fi
    if [ "$MO_FalseCCATh" != "" ] && [ "$MO_LowFalseCCATh" != "" ]; then
	iwpriv "$1" set MO_FalseCCATh="$MO_FalseCCATh"
	iwpriv "$1" set MO_LowFalseCCATh="$MO_LowFalseCCATh"
    fi
fi
########################################Channel select###########################################
if [ "$AutoChannelSelect" = "1" ]; then
    # rescan and select optimal channel
    # first need scan
    iwpriv "$1" set SiteSurvey=1
    # second select channel
    iwpriv "$1" set AutoChannelSel=1
fi
###########################################ALWAYS END############################################
# rescan coexist mode
if [ "$CONFIG_RT2860V2_AP_80211N_DRAFT3" != "" ]; then
    if [ "$AP2040Rescan" = "1" ] && [ "$HT_BSSCoexistence" = "1" ]; then
	iwpriv "$1" set AP2040Rescan=1
    fi
fi
