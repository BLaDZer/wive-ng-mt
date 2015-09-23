#!/bin/sh

########################################################
# internet.sh - reconfigure network helper for goahead #
########################################################

# include global config
. /etc/scripts/global.sh

# restart mode
MODE="$1"

LOG="logger -t reconfig"

addWds() {
        eval `nvram_buf_get 2860 WdsEnable WdsNum`
	wdsif="$1"
	if [ "$WdsEnable" != "0" ]; then
	    if [ "$WdsNum" = "" ]; then
		WdsNum=1
	    fi
	    let "wdsrealnum=$WdsNum-1"
    	    for ifnum in `seq 0 $wdsrealnum`; do
		$LOG "Readd ${wdsif}${ifnum} in br0"
		readdif_to_br ${wdsif}${ifnum}
    	    done
	fi
}

addMBSSID() {
	eval `nvram_buf_get 2860 BssidNum`
	mbssidif="$1"
	# skip root if readd
	if [ "$BssidNum" != "0" ] && [ "$BssidNum" != "1" ]; then
	    let "bssrealnum=$BssidNum-1"
	    for ifnum in `seq 1 $bssrealnum`; do
		$LOG "Readd ${mbssidif}${ifnum} in br0"
		readdif_to_br ${mbssidif}${ifnum}
	    done
	fi
}

bridge_config() {
	$LOG "Bridge OperationMode: $OperationMode"
	# flush eth2 ip and remove from bridge
	# in bridge mode add only eth2 NOT ADD "$phys_lan_if" or "$phys_wan_if"
	$LOG "Readd eth2 in br0"
	readdif_to_br eth2
	$LOG "Readd $first_wlan_root_if in br0"
	readdif_to_br $first_wlan_root_if
	if [ "$second_wlan_root_if" != "" ]; then
	    $LOG "Readd $second_wlan_root_if in br0"
	    readdif_to_br $second_wlan_root_if
	fi
	if [ "$first_wlan_mbss" != "" ] && [ "$first_wlan_mbss" = "$BssidIfName" ]; then
	    addMBSSID $first_wlan_mbss
	elif [ "$second_wlan_mbss" != "" ] && [ "$second_wlan_mbss" = "$BssidIfName" ]; then
	    addMBSSID $second_wlan_mbss
	fi
	if [ "$first_wlan_wds" != "" ] && [ "$first_wlan_wds" = "$WdsIfName" ]; then
	    addWds $first_wlan_wds
	elif [ "$second_wlan_wds" != "" ] && [ "$second_wlan_wds" = "$WdsIfName" ]; then
	    addWds $second_wlan_wds
	fi
}

gate_config() {
	$LOG "Gateway OperationMode: $OperationMode"
	# flush "$phys_lan_if" ip and remove from bridge
	# add lan interface
	$LOG "Readd $phys_lan_if in br0"
	readdif_to_br $phys_lan_if
	$LOG "Readd $first_wlan_root_if in br0"
	readdif_to_br $first_wlan_root_if
	if [ "$second_wlan_root_if" != "" ]; then
	    $LOG "Readd $second_wlan_root_if in br0"
	    readdif_to_br $second_wlan_root_if
	fi
	if [ "$first_wlan_mbss" != "" ] && [ "$first_wlan_mbss" = "$BssidIfName" ]; then
	    addMBSSID $first_wlan_mbss
	elif [ "$second_wlan_mbss" != "" ] && [ "$second_wlan_mbss" = "$BssidIfName" ]; then
	    addMBSSID $second_wlan_mbss
	fi
	if [ "$first_wlan_wds" != "" ] && [ "$first_wlan_wds" = "$WdsIfName" ]; then
	    addWds $first_wlan_wds
	elif [ "$second_wlan_wds" != "" ] && [ "$second_wlan_wds" = "$WdsIfName" ]; then
	    addWds $second_wlan_wds
	fi
}

apcli_config() {
	$LOG "ApClient OperationMode: $OperationMode"
	# flush eth2 ip and remove from bridge
	# in bridge mode add only eth2 NOT ADD "$phys_lan_if" or "$phys_wan_if"
	$LOG "Readd eth2 in br0"
	readdif_to_br eth2
	$LOG "Readd $first_wlan_root_if in br0"
	readdif_to_br $first_wlan_root_if
	if [ "$second_wlan_root_if" != "" ]; then
	    $LOG "Readd $second_wlan_root_if in br0"
	    readdif_to_br $second_wlan_root_if
	fi
	if [ "$ApCliBridgeOnly" = "1" ]; then
	    if [ "$first_wlan_apcli" != "" ] && [ "$first_wlan_apcli" = "$ApCliIfName" ]; then
		$LOG "Readd $first_wlan_apcli in br0"
		readdif_to_br $first_wlan_apcli
	    elif [ "$second_wlan_apcli" != "" ] && [ "$second_wlan_apcli" = "$ApCliIfName" ]; then
		$LOG "Readd $second_wlan_apcli in br0"
		readdif_to_br $second_wlan_apcli
	    fi
	fi
	if [ "$first_wlan_mbss" != "" ] && [ "$first_wlan_mbss" = "$BssidIfName" ]; then
	    addMBSSID $first_wlan_mbss
	elif [ "$second_wlan_mbss" != "" ] && [ "$second_wlan_mbss" = "$BssidIfName" ]; then
	    addMBSSID $second_wlan_mbss
	fi
	# delete ra0 from bridge and down if only apcli-bridge
	eval `nvram_buf_get 2860 ApCliClientOnly`
	if  [ "$ApCliClientOnly" = "1" ] && [ "$OperationMode" = "0" -o "$OperationMode" = "3" ]; then
	    if [ "$first_wlan_apcli" != "" ] && [ "$first_wlan_apcli" = "$ApCliIfName" ]; then
    		echo "APCLI Only client mode enable, shutdown AP interface $first_wlan_root_if."
    		delif_from_br $first_wlan_root_if
    	    elif [ "$second_wlan_apcli" != "" ] && [ "$second_wlan_apcli" = "$ApCliIfName" ]; then
    		echo "APCLI Only client mode enable, shutdown AP interface $second_wlan_root_if."
    		delif_from_br $second_wlan_root_if
    	    fi
	fi
}

ethcv_config() {
	$LOG "Ethernet Converter OperationMode: $OperationMode"
}

# some reload and reconfigure
if [ "$MODE" != "connect_sta" ]; then
    if [ "$MODE" != "wifionly" ]; then
	if [ "$IPv6OpMode" = "2" -o "$IPv6OpMode" = "3" ]; then
	    $LOG "all tunnels in ipv6 deconfig before modules reload (prevent race)"
	    service six stop
	fi
	service vpnhelper stop_safe
    fi
    $LOG "Reload wireless modules."
    service modules restart
    if [ "$MODE" != "wifionly" ]; then
	$LOG "Reconfigure lan."
	service lan restart
    fi
fi

# OperationMode adjustment:
# init ip address to all interfaces for different OperationMode:
#   0 = Bridge Mode
#   1 = Gateway Mode
#   2 = Ethernet Converter Mode
#   3 = AP Client
#   4 = Hotspot Mode
#
if [ "$OperationMode" = "0" ]; then
    bridge_config
elif [ "$OperationMode" = "1" ]; then
    gate_config
elif [ "$OperationMode" = "2" ] && [ "$CONFIG_RT2860V2_STA" != "" -o "$CONFIG_MT76X2_STA" != "" ]; then
    ethcv_config
elif [ "$OperationMode" = "3" ] && [ "$first_wlan_apcli" != "" -o  "$second_wlan_apcli" != "" ]; then
    apcli_config
else
    $LOG "Unknown mode. Please reset device."
fi

# reconfigure wan port
service wan restart

# reconfigure ipv6
service six restart

# rebuild switch part for full renew
config-vlan.sh $switchmode $switchpart

# some daemons need restart
services_restart.sh all
