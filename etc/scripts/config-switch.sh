#!/bin/sh

#########################################################
# configure LAN/WAN switch particion and mode per port
# This is high level switch configure helper for Wive-NG
#########################################################

# include global
. /etc/scripts/global.sh

LOG="logger -t ESW"

# get need variables
eval `nvram_buf_get 2860 wan_port tv_port sip_port vlan_double_tag ForceRenewDHCP`

##############################################################################
# BASE FOR ALL ESW
# Get switch type from proc
##############################################################################
get_switch_type() {
    if [ -f /proc/mt7620/gmac ]; then
	PROC="/proc/mt7620/gmac"
	SWITCH_MODE=3
    elif [ -f /proc/mt7621/gmac ]; then
	PROC="/proc/mt7621/gmac"
	SWITCH_MODE=3
    else
	$LOG "No switch in system!!!"
	PROC=
	SWITCH_MODE=
    fi
}

##########################################################################
# Call this functions only if VLAN as WAN need
##########################################################################
doublevlantag() {
    if [ -f /proc/sys/net/core/vlan_double_tag ]; then
	# always disabled in modes with all lan ports in one bridge
        if [ "$vlan_double_tag" = "1" -o "$VlanEnabled" = "1" ] && [ "$OperationMode" != "0" -a "$OperationMode" != "2" -a "$OperationMode" != "3" ]; then
	    $LOG "Double vlan tag enabled."
	    DOUBLE_TAG=1
	else
	    $LOG "Double vlan tag disabled."
	    DOUBLE_TAG=0
	fi
	sysctl -wq net.core.vlan_double_tag="$DOUBLE_TAG"
    fi
}

##########################################################################
# Configure vlans in kernel. Only one per start
##########################################################################
configs_system_vlans() {
    if [ ! -f /tmp/bootgood ]; then
	$LOG "ROOT_MACADDR $LAN_MAC_ADDR"
	doublevlantag
	ifconfig eth2 hw ether "$LAN_MAC_ADDR"
	ip link set eth2 up
	# not need wan/lan vlans in modes with all lan ports in one bridge
	if [ "$OperationMode" != "0" ] && [ "$OperationMode" != "2" ]  && [ "$OperationMode" != "3" ]; then
	    $LOG "Add vlans interfaces"
	    if [ ! -d /proc/sys/net/ipv4/conf/eth2.1 ]; then
		vconfig add eth2 1
		set_vlan_map eth2.1
	    fi
	    if [ ! -d /proc/sys/net/ipv4/conf/eth2.2 ]; then
		vconfig add eth2 2
		set_vlan_map eth2.2
	    fi
	    if [ -f /etc/scripts/switchadv.sh ]; then
		#this hook for advanced SWITCH/VLAN map configure
		$LOG "Call user /etc/scripts/switchadv.sh script VLANADD mode."
		/etc/scripts/switchadv.sh "VLANADD"
	    fi
	fi
    fi
}

set_vlan_portmap() {
##############################################################################
# Internal ESW
##############################################################################
if [ "$CONFIG_RAETH_ESW" != "" ] && [ "$SWITCH_MODE" != "" ]; then
    ##########################################################################
    $LOG '######### Clear switch partition  ###########'
    /etc/scripts/config-vlan.sh $SWITCH_MODE "LLLLL" > /dev/null 2>&1
    ##########################################################################
    configs_system_vlans
    ##########################################################################
    # In gate mode and hotspot mode configure vlans
    ##########################################################################
    if [ "$OperationMode" = "1" ] || [ "$OperationMode" = "4" ]; then
	# manual vlan configured
	if [ "$VlanEnabled" = "1" ]; then
		CMODE="VLANS"
	# tv and sip
	elif [ "$tv_port" = "1" ] && [ "$sip_port" = "1" ]; then
	    if [ "$wan_port" = "4" ]; then
		CMODE="WWWLL"
            else
		CMODE="LLWWW"
	    fi
	# only tv
	elif [ "$tv_port" = "1" ]; then
	    if [ "$wan_port" = "4" ]; then
		CMODE="WWLLL"
            else
		CMODE="LLLWW"
	    fi
	# only sip
	elif [ "$sip_port" = "1" ]; then
	    if [ "$wan_port" = "4" ]; then
		CMODE="WLWLL"
            else
		CMODE="LLWLW"
	    fi
	# without bridget ports
	else
	    if [ "$wan_port" = "4" ]; then
		CMODE="WLLLL"
            else
		CMODE="LLLLW"
	    fi
	fi
	$LOG "##### ESW config vlan partition $CMODE #####"
	/etc/scripts/config-vlan.sh $SWITCH_MODE "$CMODE" > /dev/null 2>&1
    elif [ "$OperationMode" = "0" ] || [ "$OperationMode" = "2" ] || [ "$OperationMode" = "3" ]; then
		CMODE="LLLLL"
    fi
fi
}

##########################################################################
# Call this function for set HW_ADDR to interfaces
##########################################################################
set_mac_wan_lan() {
    # set MAC adresses LAN for phys iface (always set for physycal external switch one or dual phy mode)
    # set MAC adresses LAN/WAN if not bridge and not ethernet converter modes
    # in gw/hotspot modes set mac to wan (always set for physycal external dual phy mode swicth)
    if [ "$OperationMode" = "1" ] || [ "$OperationMode" = "4" ]; then
	# LAN mac config
	$LOG "$phys_lan_if MACADDR $LAN_MAC_ADDR"
	ifconfig "$phys_lan_if" down
	ifconfig "$phys_lan_if" hw ether "$LAN_MAC_ADDR" txqueuelen "$txqueuelen" up
	# WAN mac config
	$LOG "$phys_wan_if MACADDR $WAN_MAC_ADDR"
	ifconfig "$phys_wan_if" down
	ifconfig "$phys_wan_if" hw ether "$WAN_MAC_ADDR" txqueuelen "$txqueuelen" up
    fi
}

##########################################################################
# Set speed and duplex modes per port
##########################################################################
set_perport_physmode() {
    if [ -f /bin/ethtool ] && [ "$PROC" != "" ]; then
	##################################
	# start configure by ethtool
	##################################
	phys_portN=4
	for i in `seq 1 5`; do
	    # select switch port for tune
	    echo "$phys_portN" > $PROC
	    # get mode for current port
	    port_swmode=`nvram_get 2860 port"$i"_swmode`
	    if [ "$port_swmode" != "auto" ] && [ "$port_swmode" != "" ]; then
		$LOG ">>> Port $phys_portN set mode $port_swmode <<<"
		# first disable autoneg
		ethtool -s eth2 autoneg off > /dev/null 2>&1
		if [ "$port_swmode" = "100f" ]; then
		    #set 100Mbit full duplex and start negotinate
		    ethtool -s eth2 autoneg on speed 100 duplex full	> /dev/null 2>&1
		elif [ "$port_swmode" = "100h" ]; then
		    #set 100Mbit half duplex and start negotinate
		    ethtool -s eth2 autoneg on speed 100 duplex half	> /dev/null 2>&1
		elif [ "$port_swmode" = "10f" ]; then
		    #set 10Mbit full duplex and start negotinate
		    ethtool -s eth2 autoneg on speed 10 duplex full	> /dev/null 2>&1
		elif [ "$port_swmode" = "10h" ]; then
		    #set 10Mbit half duplex and start negotinate
		    ethtool -s eth2 autoneg on speed 10 duplex half	> /dev/null 2>&1
		fi
	    elif [ "$port_swmode" = "auto" ]; then
		# enable autoneg
		ethtool -s eth2 autoneg on > /dev/null 2>&1
	    fi
	    let "phys_portN=$phys_portN-1"
	done
    fi
}

##########################################################################
# Configure touch dhcp from driver in kernel.
##########################################################################
set_dhcptouch_portnum() {
    if [ "$CONFIG_RAETH_DHCP_TOUCH" != "" ]; then
        if [ "$OperationMode" = "0" ] || [ "$OperationMode" = "2" ] || [ "$OperationMode" = "3" ] ; then
	    # disable dhcp renew from driver
	    sysctl -wq net.ipv4.send_sigusr_dhcpc=9
	else
	    if [ "$ForceRenewDHCP" != "0" ] && [ "$wan_port" != "" ]; then
		# configure event wait port
		sysctl -wq net.ipv4.send_sigusr_dhcpc="$wan_port"
	    else
		# disable dhcp renew from driver
		sysctl -wq net.ipv4.send_sigusr_dhcpc=9
	    fi
	fi
    fi
}

##########################################################################
get_switch_type
##########################################################################
if [ $CONFIG_RAETH_ESW != "" ] && [ "$CONFIG_RAETH_GMAC2" = "" ]; then
    set_vlan_portmap
fi
set_mac_wan_lan
set_perport_physmode
set_dhcptouch_portnum
##########################################################################
