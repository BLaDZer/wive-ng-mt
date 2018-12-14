#!/bin/sh

#############################################################
# global.sh - correct enviroment helper for automatization  #
#############################################################
# This script calculate values and export for usb in others #
# scripts 						    #
#############################################################

#############################################################
# OperationMode:
# 0 - AP Bridge Mode (all ports and wifi in one bridge)
# 1 - AP Gateway (classic router mode)
# 2 - ETH Converter (WAN - ra0 in STA mode, others LAN)
# 3 - AP-Client/Bridge mode
#############################################################

# include kernel config
. /etc/scripts/config.sh

# include profile variables
. /etc/profile

# set some constatns
vpn_def_if="ppp0"


mcast_net="224.0.0.0/4"
upnmp_net="239.0.0.0/8"

# first get operation mode and wan mode  dns mode and relay mode vpn mode and type
eval `nvram_buf_get 2860 HostName OperationMode \
	wanConnectionMode wan_ipaddr wan_netmask wan_gateway wan_static_dns wan_manual_mtu \
	lan_ipaddr lan_netmask \
	wan_port tv_port sip_port tv_port_mcast sip_port_mcast \
	WAN_MAC_ADDR LAN_MAC_ADDR \
	RadioOn RadioOnINIC \
	ApCliAutoConnect ApCliBridgeOnly \
	ApCliIfName WdsIfName BssidIfName \
	dnsPEnabled SmbEnabled UDPXYMode UDPXYPort igmpEnabled SysLogd \
	vpnEnabled vpnPurePPPOE vpnType vpnDGW \
	IPv6OpMode IPv6Dhcpc Ipv6InVPN \
	cwmpdEnabled \
	chilli_enable chilli_net \
	MODEMENABLED MODEMTYPE`

# tune for MTK wifi optimal performance at alg work
# more and more test again and agein
getTxqlenByMode() {
    # allways use small queue for wan if for avoid txq full in ra* interfaces
    if [ "$chilli_enable" != "on" ]; then
	txqueuelenwlan="50"
    else
	# in chilli mode add bloat in tun
	# for fix this need more decrease txq to WLAN interfaces
	txqueuelenwlan="20"
    fi

    if [ "$OperationMode" = "1" ]; then
	# in router mode:
	# need more atomic traffic insert for WLAN
	if [ -e /proc/mt7621/gmac ]; then
	    txqueuelen="160"
	else
	    txqueuelen="80"
	fi
    else
	# in bridge mode:
	# use static txqlen for all interfaces
	if [ -e /proc/mt7621/gmac ]; then
	    txqueuelen="200"
	else
	    txqueuelen="80"
	fi
    fi

    # always big txq for soft tunneled interface
    # optimal txqlen for tun interface = summ all wired + wireless + bridge interfaces qlen
    if [ -e /proc/mt7621/gmac ]; then
	txqueuelentun="640"
    else
	txqueuelentun="360"
    fi

    # l2tp set txbps per device
    if [ -e /proc/mt7621/gmac ]; then
	txbps="1000000000"
    else
	txbps="100000000"
    fi
}

# name/mask for first wlanmodule used in system logic
getFirstWlanIfName() {
    if [ "$CONFIG_RT_FIRST_IF_NONE" = "" ]; then
	first_wlan_root_if="ra0"			# is first root interface name
	first_wlan="ra"					# this is mask name vifs for first wlan module
	if [ "$CONFIG_MT76X2_AP_MBSS" != "" ] || [ "$CONFIG_MT76X3_AP_MBSS" != "" ] || [ "$CONFIG_MT7615_AP_MBSS" != "" ]; then
	    first_wlan_mbss="ra"			# this is mask name vifs for first mbss wlan module
	fi
	if [ "$CONFIG_MT76X2_AP_APCLI" != "" ] || [ "$CONFIG_MT76X3_AP_APCLI" != "" ] || [ "$CONFIG_MT7615_AP_APCLI" != "" ]; then
	    first_wlan_apcli="apcli0"			# this is name vif for first acli wlan module
	fi
	if [ "$CONFIG_MT76X2_AP_WDS" != "" ] || [ "$CONFIG_MT76X3_AP_WDS" != "" ] || [ "$CONFIG_MT7615_AP_WDS" != "" ]; then
	    first_wlan_wds="wds"			# this is mask name vifs for first wds wlan module
	fi
    fi
}

# this if flag say second physical wlan module exist
# and name/mask for second wlanmodule used in system logic
getSecWlanIfName() {
    if [ "$CONFIG_RT_SECOND_IF_NONE" = "" ]; then
	second_wlan_root_if="rai0"			# is root interface name
	second_wlan="rai"				# this is mask name vifs for second wlan module
	if [ "$CONFIG_MT7610_AP_MBSS" != "" ] || [ "$CONFIG_MT76X2_AP_MBSS" != "" ] || [ "$CONFIG_MT76X3_AP_MBSS" != "" ] || [ "$CONFIG_MT7615_AP_MBSS" != "" ]; then
	    second_wlan_mbss="rai"			# this is mask name vifs for second mbss wlan module
	fi
	if [ "$CONFIG_MT7610_AP_APCLI" != "" ] || [ "$CONFIG_MT76X2_AP_APCLI" != "" ] || [ "$CONFIG_MT76X3_AP_APCLI" != "" ] || [ "$CONFIG_MT7615_AP_APCLI" != "" ]; then
	    second_wlan_apcli="apclii0"			# this is name vif for first acli wlan module
	fi
	if [ "$CONFIG_MT7610_AP_WDS" != "" ] || [ "$CONFIG_MT76X2_AP_WDS" != "" ] || [ "$CONFIG_MT76X3_AP_WDS" != "" ] || [ "$CONFIG_MT7615_AP_WDS" != "" ]; then
	    second_wlan_wds="wdsi"			# this is mask name vifs for second wds wlan module
	fi
    fi
}

# DEF pysical interface name -> $phys_*_if
getPhysIfName() {
    # physical names
    if [ "$CONFIG_RAETH_BOTH_GMAC" = "y" ]; then
	# external switch support
	phys_lan_if="eth2"
	phys_wan_if="eth3"
    else
	# internal switch or not switchible device
	# support vlan parted
	phys_lan_if="eth2.1"
	phys_wan_if="eth2.2"
    fi
}

# LAN interface name -> $lan_if
getLanIfName() {
    # logical names
    if [ "$OperationMode" = "2" ]; then
	lan_if="eth2"
    else
	lan_if="br0"
    fi
    # export for web
    echo -n "$lan_if" > /tmp/lan_if_name
}

# VPN interface name -> $vpn_if
getVpnIfName() {
    if [ -e /tmp/vpn_if_name ]; then
	if_tmp=`grep -v "^$" < /tmp/vpn_if_name`
	if [ "$if_tmp" != "" ]; then
	    vpn_if="$if_tmp"
	fi
    fi
}

# WAN interface name -> $wan_if
getWanIfName() {
    # real wan name
    if [ "$OperationMode" = "0" ]; then
	# bridge
	wan_if="br0"
    elif [ "$OperationMode" = "1" ]; then
	# gateway/chillispot
	wan_if="$phys_wan_if"
    elif [ "$OperationMode" = "2" ]; then
	# ethernet converter
	wan_if="$first_wlan_root_if"
    elif [ "$OperationMode" = "3" ]; then
        if [ "$ApCliBridgeOnly" = "1" ]; then
    	    # apclient bridge
	    wan_if="br0"
	else
    	    # apclient gw
	    wan_if="$ApCliIfName"
	fi
    fi
    # services wan name (vpntype == 3 is lanauth - not vpn is IPOE)
    if [ "$vpnEnabled" = "on" ] && [ "$vpnDGW" = "1" ] && [ "$vpnType" != "3" ]; then
	if [ "$vpn_if" != "" ]; then
	    # read from file generated by ip-up script
    	    real_wan_if="$vpn_if"
	else
	    # first ppp iface is client
	    get_ppp_wan_if=`ip -o -4 link show | grep ppp | awk {' print $2 '} | cut -f -1 -d :`
	    if [ "$get_ppp_wan_if" != "" ]; then
    		real_wan_if="$get_ppp_wan_if"
	    fi
	fi
    else
        real_wan_if="$wan_if"
    fi
    # correct vpn_if for some services start
    # this workaround for preconfig igmprpoxy and vpnhelper
    if [ "$vpn_if" = "" ]; then
	vpn_if="$vpn_def_if"
    fi
}

getSixWanIfName() {
    if [ "$IPv6OpMode" = "1" ]; then
	# services wan ipv6 name (vpntype == 3 is lanauth - not vpn is IPOE)
	if [ "$vpnEnabled" = "on" ] && [ "$Ipv6InVPN" = "1" ] && [ "$vpnType" != "3" ]; then
	    # first ppp iface is client
	    get_ppp_wan_if=`ip -o -4 link show | grep ppp | awk {' print $2 '} | cut -f -1 -d :`
	    if [ "$get_ppp_wan_if" != "" ]; then
    		six_wan_if="$get_ppp_wan_if"
	    else
		six_wan_if="$vpn_def_if"
	    fi
	else
	    six_wan_if="$wan_if"
	fi
    elif [ "$IPv6OpMode" = "2" ]; then
	# 6rd tunif name
	six_wan_if="6rd"
    elif [ "$IPv6OpMode" = "3" ]; then
	# 6to4 tunif name
	six_wan_if="sit0"
    fi
    # export for dhcp6c
    echo -n "$six_wan_if" > /tmp/six_wan_if_name
}

getMdmIfName() {
    if [ "$MODEMTYPE" = "2" ]; then
	if [ -e /tmp/wwanif ]; then
	    mdm_if=`tail -qn1 /tmp/wwanif`
	fi
	if [ "$mdm_if" = "" ]; then
	    mdm_if="wwan0"
	fi
    else
	mdm_if="modem"
    fi
}

getWanIpaddr() {
    # in WAN non static mode and in full bridget modes always get parametrs direct from if
    if [ "$wanConnectionMode" != "STATIC" ] || [ "$OperationMode" = "0" ] || [ "$ApCliBridgeOnly" = "1" ]; then
	wan_ipaddr=""
	wan_netmask=""
	if [ "$OperationMode" != "0" -a "$ApCliBridgeOnly" != "1" ]; then
	    # allow router acess to internet in bridge mode
	    wan_gateway=""
	fi
    fi

    # if dgw is 0.0.0.0 replace to null
    if [ "$wan_gateway" = "0.0.0.0" ]; then
	    wan_gateway=""
    fi

    # get from if and return physical wan ip
    if [ "$wan_ipaddr" = "" ]; then
	wan_ipaddr=`ip -o -4 addr show dev "$wan_if" scope global | awk {' print $4 '} | cut -f1 -d"/"` > /dev/null 2>&1
    fi

    # get from if and return vpn or physical wan ip
    real_wan_ipaddr=`ip -o -4 addr show dev "$real_wan_if" scope global | awk {' print $4 '} | cut -f1 -d"/"` > /dev/null 2>&1
    if [ "$real_wan_ipaddr" = "" ]; then
	real_wan_ipaddr="$wan_ipaddr"
    fi

    # get first ISP gateway
    if [ "$wanConnectionMode" != "STATIC" ]; then
	if [ -e /tmp/default.gw ]; then
	    firstgw=`tail -qn1 /tmp/default.gw`
	else
	    firstgw=""
	fi
    else
	firstgw="$wan_gateway"
    fi
}

getWanReady() {
    # check interfaces configured
    wan_is_not_null=`ip -o -4 addr show "$wan_if" scope global | wc -l` > /dev/null 2>&1
    realwan_is_not_null=`ip -o -4 addr show "$real_wan_if" scope global | wc -l` > /dev/null 2>&1
}

# reconnect to AP
wifi_reconnect() {
    if [ "$OperationMode" = "2" ]; then
	staCur_SSID=`nvram_get 2860 staCur_SSID`
	if [ "$staCur_SSID" != "" ]; then
	    # Reconnect to STA
	    iwpriv $first_wlan_root_if set SSID="$staCur_SSID"
	    usleep 500000
	fi
    fi
}

# wait connect to AP
wait_connect() {
    if [ "$OperationMode" = "2" ]; then
	# Get connection status
	connected=`iwpriv $ApCliIfName connStatus | grep "Connected" -c`
	if [ "$connected" = "0" ]; then
	    exit 1
	fi
    elif [ "$OperationMode" = "3" ] && [ "$ApCliBridgeOnly" != "1" ]; then
	# Get connection status
	notconnected=`iwconfig $ApCliIfName | grep -c "Not-Associated"`
	if [ "$noconnected" = "1" ]; then
	    exit 1
	fi
    fi
}

# check ifstatus and wait 20sec if not ready
# in mt7615 ifup may be very long wait need
waitifready()
{
    ifisdown=`ip link show dev $1 | grep -c DOWN`
    count=0
    while [ "$ifisdown" != "0" ]; do
	if [ "$count" = "10" ]; then
	    $LOG "Wait for interface $1 up error!"
	    break
	fi
	sleep 1
	count="$(($count+1))"
	ifisdown=`ip link show dev $1 | grep -c DOWN`
    done
}

flush_net_caches() {
    ip route flush cache > /dev/null 2>&1
    echo f > /proc/net/nf_conntrack
}

flush_arp_caches() {
    switch clear
    ip neigh flush all
}

delif_from_br() {
    ip -4 addr flush dev $2 > /dev/null 2>&1
    ip link set $2 down > /dev/null 2>&1
    brctl delif $1 $2 > /dev/null 2>&1
}

readdif_to_br() {
    delif_from_br $1 $2
    ip link set $2 up
    waitifready $2
    brctl addif $1 $2
}

get_switch_type() {
    # mode 3 - vlan particion
    # mode 4 - dual rgmi mode
    # mode 5 - dual rgmi mode with external phy
    if [ -e /proc/mt7620/gmac ]; then
	switchmode=3
    elif [ -e /proc/mt7628/gmac ]; then
	switchmode=3
    elif [ -e /proc/mt7621/gmac ]; then
	if [ "$CONFIG_RAETH_BOTH_GMAC" != "" ]; then
	    if [ "$CONFIG_GE2_RGMII_AN" != "" ]; then
		# ext gigaphy on GE2
		switchmode=5
	    else
		switchmode=4
	    fi
	else
	    switchmode=3
	fi
    else
	$LOG "No switch in system!!!"
	switchmode=
    fi
}

get_switch_part() {
    if [ "$OperationMode" = "1" ]; then
	# get vlan parts for isolated ports
	tv_portVLAN=`nvram_get 2860 tv_portVLAN | awk '{ gsub(","," "); print }'`
	sip_portVLAN=`nvram_get 2860 sip_portVLAN | awk '{ gsub(","," "); print }'`
	# is vlan configured
	if [ "$tv_port" = "1" -a "$tv_portVLAN" != "" ] || [ "$sip_port" = "1" -a "$sip_portVLAN" != "" ]; then
	    # get priomap
	    tv_portVLANPRIO=`nvram_get 2860 tv_portVLANPRIO | awk '{ gsub(","," "); print }'`
	    sip_portVLANPRIO=`nvram_get 2860 sip_portVLANPRIO | awk '{ gsub(","," "); print }'`
	    # set vlan bridge names
	    vlantvif="vlantv"
	    vlansipif="vlansip"
	    switchpart="VLANS"
	# tv and sip
	elif [ "$tv_port" = "1" ] && [ "$sip_port" = "1" ]; then
	    if [ "$wan_port" = "4" ]; then
		switchpart="WWWLL"
            else
		switchpart="LLWWW"
	    fi
	# only tv
	elif [ "$tv_port" = "1" ]; then
	    if [ "$wan_port" = "4" ]; then
		switchpart="WWLLL"
            else
		switchpart="LLLWW"
	    fi
	# only sip
	elif [ "$sip_port" = "1" ]; then
	    if [ "$wan_port" = "4" ]; then
		switchpart="WLWLL"
            else
		switchpart="LLWLW"
	    fi
	# without bridget ports
	else
	    if [ "$wan_port" = "4" ]; then
		switchpart="WLLLL"
            else
		switchpart="LLLLW"
	    fi
	fi
    elif [ "$OperationMode" = "0" ] || [ "$OperationMode" = "2" ] || [ "$OperationMode" = "3" ]; then
		switchpart="LLLLL"
    fi
}

# check pppoe vpn type
getPPPOEMode() {
    if [ "$vpnEnabled" = "on" ] && [ "$vpnType" = "0" ] && [ "$vpnPurePPPOE" = "1" ]; then
	purepppoemode="1"
    else
	purepppoemode="0"
    fi
}

# get params
get_switch_type
get_switch_part

getTxqlenByMode
getFirstWlanIfName
getSecWlanIfName
getPhysIfName
getLanIfName
getVpnIfName
getWanIfName
getSixWanIfName
getMdmIfName
getWanIpaddr
getPPPOEMode
getWanReady
