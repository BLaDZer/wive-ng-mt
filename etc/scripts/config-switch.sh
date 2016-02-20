#!/bin/sh

##########################################################################
# configure LAN/WAN switch particion and mode per port
# This is high level switch configure helper for Wive-NG
##########################################################################

# include global
. /etc/scripts/global.sh

LOG="logger -t ESW"

# get need variables
eval `nvram_buf_get 2860 vlanDoubleTag LANVlans VlanWifiLan VlanWifiWan lanWifiLanINIC lanWifiWanINIC ForceRenewDHCP`

##########################################################################
# Call this functions only if VLAN as WAN need
##########################################################################
doublevlantag() {
    if [ -f /proc/sys/net/core/vlan_double_tag ]; then
	# always disabled in modes with all lan ports in one bridge
        if [ "$vlanDoubleTag" = "1" ] || [ "$VlanEnabled" = "1" ] || [ "$LANVlans" != "" -a "$LANVlans" != "0" ] || \
	    [ "$VlanWifiLan" != "" ] || [ "$VlanWifiLanINIC" != "" ] || [ "$VlanWifiWan" != "" ] || [ "$VlanWifiWanINIC" != "" ]; then
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
	# not need wan/lan vlans in modes with all lan ports in one bridge
	if [ "$switchpart" != "LLLLL" ]; then
	    $LOG "Add vlans interfaces"
	    if [ ! -d /proc/sys/net/ipv4/conf/eth2.1 ]; then
		vconfig add eth2 1
		set_vlan_map eth2.1
	    fi
	    if [ ! -d /proc/sys/net/ipv4/conf/eth2.2 ]; then
		vconfig add eth2 2
		set_vlan_map eth2.2
	    fi
	fi
    fi
}

##########################################################################
# Call this function for set HW_ADDR to interfaces
##########################################################################
set_mac_wan_lan() {
    # set MAC adresses LAN for phys iface (always set for physycal external switch one or dual phy mode)
    # set MAC adresses LAN/WAN if not bridge and not ethernet converter modes
    # in gw mode set mac to wan (always set for physycal external dual phy mode swicth)
    if [ "$OperationMode" = "1" ] || [ "$CONFIG_RAETH_GMAC2" = "y" ]; then
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

set_portmap() {
##############################################################################
# Internal ESW
##############################################################################
if [ "$CONFIG_RAETH_ESW" != "" -o "$CONFIG_MT7530_GSW" != "" ] && [ "$switchmode" != "" ]; then
    ##########################################################################
    # enable/disable QinQ support
    ##########################################################################
    doublevlantag
    ##########################################################################
    # add mac to root interface if vlan part used
    ##########################################################################
    if [ "$CONFIG_RAETH_GMAC2" = "" ]; then
	$LOG "ROOT_MACADDR $LAN_MAC_ADDR"
	ifconfig eth2 hw ether "$LAN_MAC_ADDR"
	ip link set eth2 up
	configs_system_vlans
    fi
    ##########################################################################
    # after add vlans set hwaddr per new ifaces
    ##########################################################################
    set_mac_wan_lan
    ##########################################################################
    # configure switch parts depended by operation mode
    ##########################################################################
    config-vlan.sh $switchmode $switchpart
    ##########################################################################
fi
}

##########################################################################
# Configure touch dhcp from driver in kernel.
##########################################################################
set_dhcptouch_portnum() {
    if [ "$CONFIG_RAETH_DHCP_TOUCH" != "" ]; then
        if [ "$switchpart" = "LLLLL" ]; then
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
set_portmap
set_dhcptouch_portnum
##########################################################################
