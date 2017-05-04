#!/bin/sh

########################################################
# internet.sh - reconfigure network helper for goahead #
########################################################

# prevent double start configure in one time
while [ -e /tmp/reconfigure_runing ]; do
    # Sleep until file does exists/is created
    usleep 500000
done
touch /tmp/reconfigure_runing

# include global config
. /etc/scripts/global.sh

# restart mode
MODE="$1"

LOG="logger -t reconfig"

addWds() {
        eval `nvram_buf_get 2860 WdsEnable WdsNum`
	wdsif="$1"
	# WdsEnable must be 0...4, this mode of WDS work"
	if [ "$WdsEnable" != "0" ] && [ "$WdsEnable" != "" ]; then
	    if [ "$WdsNum" = "" ]; then
		WdsNum=1
	    fi
	    let "wdsrealnum=$WdsNum-1"
    	    for ifnum in `seq 0 $wdsrealnum`; do
		$LOG "Readd ${wdsif}${ifnum} in br0"
		readdif_to_br br0 ${wdsif}${ifnum}
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
		readdif_to_br br0 ${mbssidif}${ifnum}
	    done
	fi
}

bridge_config() {
	$LOG "Bridge OperationMode: $OperationMode"
	# flush eth2 ip and remove from bridge
	# in bridge mode add only eth2 NOT ADD "$phys_lan_if" or "$phys_wan_if"
	$LOG "Readd eth2 in br0"
	readdif_to_br br0 eth2
	$LOG "Readd $first_wlan_root_if in br0"
	readdif_to_br br0 $first_wlan_root_if
	if [ "$second_wlan_root_if" != "" ]; then
	    $LOG "Readd $second_wlan_root_if in br0"
	    readdif_to_br br0 $second_wlan_root_if
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
	readdif_to_br br0 $phys_lan_if
	$LOG "Readd $first_wlan_root_if in br0"
	readdif_to_br br0 $first_wlan_root_if
	if [ "$second_wlan_root_if" != "" ]; then
	    $LOG "Readd $second_wlan_root_if in br0"
	    readdif_to_br br0 $second_wlan_root_if
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
	readdif_to_br br0 eth2
	$LOG "Readd $first_wlan_root_if in br0"
	readdif_to_br br0 $first_wlan_root_if
	if [ "$second_wlan_root_if" != "" ]; then
	    $LOG "Readd $second_wlan_root_if in br0"
	    readdif_to_br br0 $second_wlan_root_if
	fi
	if [ "$ApCliBridgeOnly" = "1" ]; then
	    if [ "$first_wlan_apcli" != "" ] && [ "$first_wlan_apcli" = "$ApCliIfName" ]; then
		$LOG "Readd $first_wlan_apcli in br0"
		readdif_to_br br0 $first_wlan_apcli
	    elif [ "$second_wlan_apcli" != "" ] && [ "$second_wlan_apcli" = "$ApCliIfName" ]; then
		$LOG "Readd $second_wlan_apcli in br0"
		readdif_to_br br0 $second_wlan_apcli
	    fi
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
	# delete ra0 from bridge and down if only apcli-bridge
	eval `nvram_buf_get 2860 ApCliClientOnly`
	if  [ "$ApCliClientOnly" = "1" ] && [ "$OperationMode" = "0" -o "$OperationMode" = "3" ]; then
	    if [ "$first_wlan_apcli" != "" ] && [ "$first_wlan_apcli" = "$ApCliIfName" ]; then
    		echo "APCLI Only client mode enable, shutdown AP interface $first_wlan_root_if."
    		delif_from_br br0 $first_wlan_root_if
    	    elif [ "$second_wlan_apcli" != "" ] && [ "$second_wlan_apcli" = "$ApCliIfName" ]; then
    		echo "APCLI Only client mode enable, shutdown AP interface $second_wlan_root_if."
    		delif_from_br br0 $second_wlan_root_if
    	    fi
	fi
}

ethcv_config() {
	$LOG "Ethernet Converter OperationMode: $OperationMode"
}

retune_wifi() {
	/etc/scripts/wifi_advanced.sh $first_wlan_root_if
	if [ "$first_wlan_apcli" != "" ] && [ "$OperationMode" = "3" ]; then
	    /etc/scripts/wifi_advanced.sh $first_wlan_apcli
	fi
	if [ "$second_wlan_root_if" != "" ]; then
	    /etc/scripts/wifi_advanced.sh $second_wlan_root_if "5GHZ"
	    if [ "$second_wlan_apcli" != "" ] && [ "$OperationMode" = "3" ]; then
		/etc/scripts/wifi_advanced.sh $second_wlan_apcli "5GHZ"
	    fi
	fi
}

roaming_wifi() {
	eval `nvram_buf_get 2860 FastRoaming`
	if [ "$FastRoaming" = "1" ] && [ "$OperationMode" != "2" ]; then
	    if [ "$first_wlan_root_if" != "" ]; then
		/etc/scripts/wifi_roaming.sh $first_wlan_root_if
	    fi
	    if [ "$second_wlan_root_if" != "" ]; then
		/etc/scripts/wifi_roaming.sh $second_wlan_root_if "5GHZ"
	    fi
	fi
}

# regenerate staticroutes and iptables rules scripts from nvram before processing
nvram_genstaticroutes 2860
nvram_geniptablesrules 2860

# some reload and reconfigure
if [ "$MODE" != "connect_sta" ]; then
    if [ "$IPv6OpMode" = "2" -o "$IPv6OpMode" = "3" ]; then
	$LOG "all tunnels in ipv6 deconfig before modules reload (prevent race)"
	service six stop
    fi
    service vpnhelper stop
    $LOG "Reload modules."
    service modules restart
    $LOG "Reconfigure lan."
    service lan restart
fi

# OperationMode adjustment:
# init ip address to all interfaces for different OperationMode:
#   0 = Bridge Mode
#   1 = Gateway Mode
#   2 = Ethernet Converter Mode
#   3 = AP Client
#
if [ "$OperationMode" = "0" ]; then
    bridge_config
elif [ "$OperationMode" = "1" ]; then
    gate_config
elif [ "$OperationMode" = "2" ] && [ "$CONFIG_MT76X2_STA" != "" -o "$CONFIG_MT76X3_STA" != "" ]; then
    ethcv_config
elif [ "$OperationMode" = "3" ] && [ "$first_wlan_apcli" != "" -o  "$second_wlan_apcli" != "" ]; then
    apcli_config
else
    $LOG "Unknown mode. Please reset device."
fi

# reconfigure wan port
service wan restart

# rebuild switch part for full renew
config-vlan.sh $switchmode $switchpart

# reconfigure external vlans
service vlan restart

# tune params not automatic readed from *.dat file
# this need allways after ifup/ifdown or reload modules
retune_wifi
roaming_wifi

# some daemons need restart
services_restart.sh all

# remove running flag
rm -f /tmp/reconfigure_runing
