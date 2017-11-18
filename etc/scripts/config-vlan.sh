#!/bin/sh

########################################################################
# config-vlan.sh - configure vlan switch particion helper for Wive-NG  #
#                                                                      #
# usage: config-vlan.sh <switch_type> <vlan_type>                      #
########################################################################

# prevent double start configure in one time
while [ -e /tmp/vlanconfig_runing ]; do
    # Sleep until file does exists/is created
    usleep 500000
done
touch /tmp/vlanconfig_runing

# include kernel config
. /etc/scripts/config.sh

LOG="logger -t ESW"

usage() {
	echo "Usage:"
	echo "  $0 3/4/5 LLLLL - ESW restore config to no VLAN partition"
	echo "  $0 3/4/5 EEEEE - ESW enable all ports 100FD"
	echo "  $0 3/4/5 DDDDD - ESW disable all ports"
	echo "  $0 3/4/5 RRRRR - ESW reset all ports"
	echo "  $0 3/4/5 WWWWW - ESW reinit WAN port at switch"
	echo "  $0 3/4/5 FFFFF - ESW full reinit switch"
	echo "  $0 3/4/5 xxxxx - ESW config free parts (x = W or x = L, W - WAN group, L - LAN group)"
	echo "_________"
	echo " 	mode 3 for vlan parted, 4 for dual rgmii mode, 5 dualrgmi + GigaExtPHY"
	echo "_________"
}

link_up() {
	# arg1:  phy address.
	state=$(mii_mgr -g -p $1 -r 0  | sed -e "s/.* = /0x/gi")
	# Get state: $state
	let "state &= 0xf7ff"
	# Modded state: $(printf "0x%x" $state)
	# power up
	mii_mgr -s -p $1 -r 0 -v $(printf "0x%x" $state)	> /dev/null 2>&1
	# link up
	mii_mgr -s -p $1 -r 0 -v 0x9000				> /dev/null 2>&1
}

link_down() {
	# arg1:  phy address.
	state=$(mii_mgr -g -p $1 -r 0  | sed -e "s/.* = /0x/gi")
	# Get state: $state
	let "state |= 0x800"
	# Modded state: $(printf "0x%x" $state)
	# power down
	mii_mgr -s -p $1 -r 0 -v $(printf "0x%x" $state)	> /dev/null 2>&1
}

disable_all_ports() {
        for port in `seq 0 4`; do
	    link_down $port
	done

	# external PHY
	if [ "$CONFIG_GE2_RGMII_AN" = "y" ]; then
	    # link down
	    link_down 5
	fi
}

enable_all_ports() {
	for port in `seq 0 4`; do
	    link_up $port
	done

	# external PHY
	if [ "$CONFIG_GE2_RGMII_AN" = "y" ]; then
	    link_up 5
	fi
}

reset_all_phys() {
	$LOG "Reset all phy port"
	if [ "$OperationMode" = "0" ] || [ "$OperationMode" = "2" ] || [ "$OperationMode" = "3" ] || [ "$CONFIG_GE2_RGMII_AN" = "y" ]; then
	    # All ports down
	    start=0
	    end=4
	else
	    # Ports down skip WAN port
	    if [ "$wan_portN" = "0" ]; then
		start=0
		end=3
	    else
		start=1
		end=4
	    fi
	fi

	# disable ports
	for port in `seq $start $end`; do
    	    link_down $port
	done
	# wait client detect link down for renew dhcp
	usleep 80000
	# enable ports
	for port in `seq $start $end`; do
    	    link_up $port
	done
}

reset_wan_phys() {
	$LOG "Reset wan phy port"
	if [ "$OperationMode" = "1" ]; then
	    if [ "$CONFIG_GE2_RGMII_AN" = "y" ]; then
		link_down 5
		link_up 5
	    else
		if [ "$wan_portN" = "0" ]; then
		    link_down 4
		    link_up 4
		else
		    link_down 0
		    link_up 0
		fi
	    fi
	fi
}

set_physmode() {
	if [ -e /bin/ethtool ] && [ "$procdir" != "" ]; then
	    # PAUSE CONTROL (must be first for correct negotinagion after link mode changes)
	    phys_portN=4
	    for num in `seq 1 5`; do
		# select switch port for tune
		echo "$phys_portN" > $procdir
		# get mode for current port
		port_fcmode=`nvram_get 2860 port"$num"_fcmode`
		if [ "$port_fcmode" = "auto" ]; then
		    ethtool -A eth2 autoneg on tx on rx on			> /dev/null 2>&1
		elif [ "$port_fcmode" = "tx" ]; then
		    ethtool -A eth2 autoneg off tx on rx off			> /dev/null 2>&1
		elif [ "$port_fcmode" = "rx" ]; then
		    ethtool -A eth2 autoneg off tx off rx on			> /dev/null 2>&1
		elif [ "$port_fcmode" = "txrx" ]; then
		    ethtool -A eth2 autoneg off tx on rx on			> /dev/null 2>&1
		else
		    ethtool -A eth2 autoneg off tx off rx off			> /dev/null 2>&1
		fi
		let "phys_portN=$phys_portN-1"
	    done
	    # AUTONEG (set link mode and restart negotination, this must be allways end)
	    phys_portN=4
	    for num in `seq 1 5`; do
		# select switch port for tune
		echo "$phys_portN" > $procdir
		# get mode for current port
		port_swmode=`nvram_get 2860 port"$num"_swmode`
		if [ "$port_swmode" != "auto" ] && [ "$port_swmode" != "" ]; then
		    $LOG ">>> Port $phys_portN set mode $port_swmode <<<"
		    # first disable autoneg
		    ethtool -s eth2 autoneg off > /dev/null 2>&1
		    if [ "$port_swmode" = "1000f" ]; then
			#set 1000Mbit full duplex and start negotinate
			ethtool -s eth2 autoneg on speed 1000 duplex full	> /dev/null 2>&1
		    elif [ "$port_swmode" = "1000h" ]; then
			#set 1000Mbit half duplex and start negotinate
			ethtool -s eth2 autoneg on speed 1000 duplex half	> /dev/null 2>&1
		    elif [ "$port_swmode" = "100f" ]; then
			#set 100Mbit full duplex and start negotinate
			ethtool -s eth2 autoneg on speed 100 duplex full	> /dev/null 2>&1
		    elif [ "$port_swmode" = "100h" ]; then
			#set 100Mbit half duplex and start negotinate
			ethtool -s eth2 autoneg on speed 100 duplex half	> /dev/null 2>&1
		    elif [ "$port_swmode" = "10f" ]; then
			#set 10Mbit full duplex and start negotinate
			ethtool -s eth2 autoneg on speed 10 duplex full		> /dev/null 2>&1
		    elif [ "$port_swmode" = "10h" ]; then
			#set 10Mbit half duplex and start negotinate
			ethtool -s eth2 autoneg on speed 10 duplex half		> /dev/null 2>&1
		    fi
		elif [ "$port_swmode" = "auto" ]; then
		    # enable autoneg
		    ethtool -s eth2 autoneg on					> /dev/null 2>&1
		fi
		let "phys_portN=$phys_portN-1"
	    done
	fi
}

set_physmode_ext_phy() {
	if [ "$CONFIG_GE2_RGMII_AN" = "y" ] && [ -e /bin/ethtool ] && [ "$procdir" != "" ]; then
	    eval `nvram_buf_get 2860 phy_fcmode phy_swmode`
	    echo "5" > $procdir
	    if [ "$phy_fcmode" = "auto" ]; then
		ethtool -A eth2 autoneg on tx on rx on			> /dev/null 2>&1
	    elif [ "$phy_fcmode" = "tx" ]; then
		ethtool -A eth2 autoneg off tx on rx off		> /dev/null 2>&1
	    elif [ "$phy_fcmode" = "rx" ]; then
		ethtool -A eth2 autoneg off tx off rx on		> /dev/null 2>&1
	    elif [ "$phy_fcmode" = "txrx" ]; then
		ethtool -A eth2 autoneg off tx on rx on			> /dev/null 2>&1
	    else
		ethtool -A eth2 autoneg off tx off rx off		> /dev/null 2>&1
	    fi
	    echo "5" > $procdir
	    if [ "$phy_swmode" = "1000f" ]; then
		#set 1000Mbit full duplex and off negotinate
		ethtool -s eth2 autoneg off speed 1000 duplex full	> /dev/null 2>&1
	    elif [ "$phy_swmode" = "1000h" ]; then
		#set 1000Mbit half duplex and off negotinate
		ethtool -s eth2 autoneg off speed 1000 duplex half	> /dev/null 2>&1
	    elif [ "$phy_swmode" = "100f" ]; then
		#set 100Mbit full duplex and off negotinate
		ethtool -s eth2 autoneg off speed 100 duplex full	> /dev/null 2>&1
	    elif [ "$phy_swmode" = "100h" ]; then
		#set 100Mbit half duplex and off negotinate
		ethtool -s eth2 autoneg off speed 100 duplex half	> /dev/null 2>&1
	    elif [ "$phy_swmode" = "10f" ]; then
		#set 10Mbit full duplex and off negotinate
		ethtool -s eth2 autoneg off speed 10 duplex full	> /dev/null 2>&1
	    elif [ "$phy_swmode" = "10h" ]; then
		#set 10Mbit half duplex and off negotinate
		ethtool -s eth2 autoneg off speed 10 duplex half	> /dev/null 2>&1
	    else
		#atonegotinate enable
		ethtool -s eth2 autoneg on				> /dev/null 2>&1
	    fi
	    $LOG ">>> ExtPHY set mode $port_swmode FC $phy_fcmode<<<"
	fi
}

reinit_all_phys() {
	# Reinit all ports
	disable_all_ports
	enable_all_ports
	# LAN ports blink for stupid win machine dhcp force renew
	reset_all_phys
	# Set ESW/GSW  ports parametrs
	set_physmode
	# Set ExtPHY ports parametrs
	set_physmode_ext_phy
}

config_igmpsnoop() {
	if [ "$OperationMode" = "1" ] && [ "$igmpSnoopMode" != "n" ]; then
	    # hybrid snooping now not support in pormapped modes, use HW only snooping for this
	    # and enable if forces by user
	    if [ "$igmpSnoopMode" = "h" ] || [ "$tv_port" = "1" ] || [ "$sip_port" = "1" ]; then
		# make cpu porst static listeners allways others is off (avoid flood)
		# only cpu2lan (6 port) used for learning
		# for future, add support snooping in AP mode, need add in mask real GSW uplink port to
		snoopmask="0000001"
		switch igmpsnoop on 100 "$snoopmask"

		# enable snooping and learn
		for port in `seq 0 6`; do
		    switch igmpsnoop enable $port
		done
		$LOG "Force pure hardware mode igmp snooping enable, mask $snoopmask."
	    fi
	fi
}

restore_igmpsnoop() {
	switch igmpsnoop off
	for port in `seq 0 6`; do
	    switch igmpsnoop disable $port
	done
}

restore_onergmii()
{
        $LOG "Restore internal switch mode to dumb mode"
	for port in `seq 0 7`; do
	    switch reg w 2${port}04 ff0000	#ports 0-7 matrix mode
	    switch reg w 2${port}10 810000c0 	#ports 0-7 as transparent port, admit all frames and disable special tag
	done

	switch reg w 3500 00008000		#port 5 link down not used

	switch reg w 0010 7f7f7fe0		#port 6 as CPU Port

	# do not restore cpu port mode if trgmii enabled
	if [ "$CONFIG_GE1_TRGMII_FORCE_1200" != "y" ]; then
	    switch reg w 3600 0005e33b		#port 6 force up, 1000FD
	fi

	# clear configured vlan parts
	switch vlan clear

	# reinit all ports
	reinit_all_phys

	# disable snooping
	restore_igmpsnoop

	# clear mac table if vlan configuration changed
	switch clear
}

config_onergmii()
{
	# cleanup switch
	restore_onergmii

	# prepare switch
	for port in `seq 0 4`; do
	    switch reg w 2${port}10 810000c0 	#ports 0-4 as transparent port, admit all frames and disable special tag
	    switch reg w 2${port}04 ff0003	#ports 0-4 as security mode
	done

	for port in `seq 6 7`; do
	    switch reg w 2${port}10 81000000	#ports 6-7 special tag disable, is user port, admit all frames
	    switch reg w 2${port}04 20df0003	#ports 6-7 egress VLAN Tag Attribution=tagged
	done

	if [ "$1" != "VLANS" ]; then
	    PMODEMASK="$1"
	    # replace W/L to 0/1 for create masks and add static mask suffix
	    masklan=`echo "$PMODEMASK" | sed 's/[0-9]/0/g;s/W/0/g;s/L/1/g' | awk {' print $1 "011" '}`
	    maskwan=`echo "$PMODEMASK" | sed 's/[0-9]/0/g;s/W/1/g;s/L/0/g' | awk {' print $1 "011" '}`

	    # replace W/L to 2/1 and add space after symbols for set pvids mask
	    pvids=`echo "$PMODEMASK" | sed 's/[0-9]/0/g;s/W/2/g;s/L/1/g' | sed -e "s/.\{1\}/&\ /g"`

	    $LOG "Config internal vlan parts switch mode $PMODEMASK, masklan:$masklan,maskwan:$maskwan,pvids:$pvids"

	    # VLAN member ports index 0 vlan 1 masklan
	    switch vlan set 0 1 $masklan
	    # VLAN member ports index 1 vlan 2 maskwan
	    switch vlan set 1 2 $maskwan

	    index=0
	    # set user ports pvid
	    for pvid in $pvids ; do
		switch pvid $index $pvid
		let index=index+1
	    done

	    # config hawdware snooping
	    config_igmpsnoop

	else
	    $LOG "TV/STB/SIP with VLANs mode enabled."
	    # internal VLAN for TV = 3, for SIP = 4
	    if [ "$wan_port" = "4" ]; then
		# tv and sip
		if [ "$tv_port" = "1" ] && [ "$sip_port" = "1" ]; then
		    #VLAN member port
		    switch vlan set 0 1 00011011
		    switch vlan set 1 2 10000011
		    switch vlan set 2 3 01000011
		    switch vlan set 3 4 00100011
		    # set PVID
		    switch pvid 0 2
		    switch pvid 1 3
		    switch pvid 2 4
		    switch pvid 3 1
		    switch pvid 4 1
		# only tv
		elif [ "$tv_port" = "1" ]; then
		    # VLAN member port
		    switch vlan set 0 1 00111011
		    switch vlan set 1 2 10000011
		    switch vlan set 2 3 01000011
		    # set PVID
		    switch pvid 0 2
		    switch pvid 1 3
		    switch pvid 2 1
		    switch pvid 3 1
		    switch pvid 4 1
		# only sip
		elif [ "$sip_port" = "1" ]; then
		    # VLAN member port
		    switch vlan set 0 1 01011011
		    switch vlan set 1 2 10000011
		    switch vlan set 2 4 00100011
		    # set PVID
		    switch pvid 0 2
		    switch pvid 1 1
		    switch pvid 2 4
		    switch pvid 3 1
		    switch pvid 4 1
		fi
	    else
		# tv and sip
		if [ "$tv_port" = "1" ] && [ "$sip_port" = "1" ]; then
		    # VLAN member port
		    switch vlan set 0 1 11000011
		    switch vlan set 1 2 00001011
		    switch vlan set 2 3 00010011
		    switch vlan set 3 4 00100011
		    # set PVID
		    switch pvid 0 1
		    switch pvid 1 1
		    switch pvid 2 4
		    switch pvid 3 3
		    switch pvid 4 2
		# only tv
		elif [ "$tv_port" = "1" ]; then
		    # VLAN member port
		    switch vlan set 0 1 11100011
		    switch vlan set 1 2 00001011
		    switch vlan set 2 3 00010011
		    # set PVID
		    switch pvid 0 1
		    switch pvid 1 1
		    switch pvid 2 1
		    switch pvid 3 3
		    switch pvid 4 2
		# only sip
		elif [ "$sip_port" = "1" ]; then
		    # VLAN member port
		    switch vlan set 0 1 11010011
		    switch vlan set 1 2 00001011
		    switch vlan set 2 4 00100011
		    # set PVID
		    switch pvid 0 1
		    switch pvid 1 1
		    switch pvid 2 1
		    switch pvid 3 4
		    switch pvid 4 2
		fi
	    fi
	fi

	# clear mac table if vlan configuration changed
	switch clear
}

restore_dualrgmii()
{
        $LOG "Restore internal MT7621 switch mode to dumb mode"

	for port in `seq 0 7`; do
	    switch reg w 2${port}04 ff0000	#ports 0-7 matrix mode
	    switch reg w 2${port}10 810000c0 	#ports 0-7 as transparent port, admit all frames and disable special tag
	done

	# clear configured vlan parts
	switch vlan clear

	# reinit all ports
	reinit_all_phys

	# disable snooping
	restore_igmpsnoop

	#clear mac table if vlan configuration changed
	switch clear
}

config_dualrgmii()
{
	# cleanup swicth
	restore_dualrgmii

	# prepare switch
	for port in `seq 0 4`; do
	    switch reg w 2${port}10 810000c0 	#ports 0-4 as transparent port, admit all frames and disable special tag
	    switch reg w 2${port}04 ff0003	#ports 0-4 as security mode
	done

	for port in `seq 5 7`; do
	    switch reg w 2${port}10 81000000	#ports 5-7 special tag disable, is user port, admit all frames
	    switch reg w 2${port}04 20df0003	#ports 5-7 egress VLAN Tag Attribution=tagged
	done

	# set cpu ports pvids
	switch pvid 5 2
	switch pvid 6 1

	if [ "$1" != "VLANS" ]; then
	    $LOG "Config internal MT7621 switch mode $1"
	    # replace W/L to 0/1 for create masks and add static mask suffix
	    masklan=`echo "$1" | sed 's/[0-9]/0/g;s/W/0/g;s/L/1/g' | awk {' print $1 "010" '}`
	    maskwan=`echo "$1" | sed 's/[0-9]/0/g;s/W/1/g;s/L/0/g' | awk {' print $1 "100" '}`
	    # replace W/L to 2/1 and add space after symbols for set pvids mask
	    pvids=`echo "$1" | sed 's/[0-9]/0/g;s/W/2/g;s/L/1/g' | sed -e "s/.\{1\}/&\ /g"`

	    $LOG "Config internal vlan parts switch mode $1, masklan:$masklan,maskwan:$maskwan,pvids:$pvids"

	    # VLAN member ports index 0 vlan 1 masklan
	    switch vlan set 0 1 $masklan
	    # VLAN member ports index 1 vlan 2 maskwan
	    switch vlan set 1 2 $maskwan

	    index=0
	    # set user ports pvid
	    for pvid in $pvids ; do
		switch pvid $index $pvid
		let index=index+1
	    done

	    # detag internal tagged packets before send to cpu
	    for port in `seq 5 7`; do
		switch tag off $port
	    done

	    # config hawdware snooping
	    config_igmpsnoop

         else
	    $LOG "TV/STB/SIP with VLANs mode enabled."
	    # internal VLAN for TV = 3, for SIP = 4
	    if [ "$wan_port" = "4" ]; then
		# tv and sip
		if [ "$tv_port" = "1" ] && [ "$sip_port" = "1" ]; then
		    #VLAN member port
		    switch vlan set 0 1 00011010 0 0 -----uuu
		    switch vlan set 1 2 10000100 0 0 -----uuu
		    switch vlan set 2 3 01000010
		    switch vlan set 3 4 00100010
		    # set PVID
		    switch pvid 0 2
		    switch pvid 1 3
		    switch pvid 2 4
		    switch pvid 3 1
		    switch pvid 4 1
		# only tv
		elif [ "$tv_port" = "1" ]; then
		    # VLAN member port
		    switch vlan set 0 1 00111010 0 0 -----uuu
		    switch vlan set 1 2 10000100 0 0 -----uuu
		    switch vlan set 2 3 01000010
		    # set PVID
		    switch pvid 0 2
		    switch pvid 1 3
		    switch pvid 2 1
		    switch pvid 3 1
		    switch pvid 4 1
		# only sip
		elif [ "$sip_port" = "1" ]; then
		    # VLAN member port
		    switch vlan set 0 1 01011010 0 0 -----uuu
		    switch vlan set 1 2 10000100 0 0 -----uuu
		    switch vlan set 2 4 00100010
		    # set PVID
		    switch pvid 0 2
		    switch pvid 1 1
		    switch pvid 2 4
		    switch pvid 3 1
		    switch pvid 4 1
		fi
	    else
		# tv and sip
		if [ "$tv_port" = "1" ] && [ "$sip_port" = "1" ]; then
		    # VLAN member port
		    switch vlan set 0 1 11000010 0 0 -----uuu
		    switch vlan set 1 2 00001100 0 0 -----uuu
		    switch vlan set 2 3 00010010
		    switch vlan set 3 4 00100010
		    # set PVID
		    switch pvid 0 1
		    switch pvid 1 1
		    switch pvid 2 4
		    switch pvid 3 3
		    switch pvid 4 2
		# only tv
		elif [ "$tv_port" = "1" ]; then
		    # VLAN member port
		    switch vlan set 0 1 11100010 0 0 -----uuu
		    switch vlan set 1 2 00001100 0 0 -----uuu
		    switch vlan set 2 3 00010010
		    # set PVID
		    switch pvid 0 1
		    switch pvid 1 1
		    switch pvid 2 1
		    switch pvid 3 3
		    switch pvid 4 2
		# only sip
		elif [ "$sip_port" = "1" ]; then
		    # VLAN member port
		    switch vlan set 0 1 11010010 0 0 -----uuu
		    switch vlan set 1 2 00001100 0 0 -----uuu
		    switch vlan set 2 4 00100010
		    # set PVID
		    switch pvid 0 1
		    switch pvid 1 1
		    switch pvid 2 1
		    switch pvid 3 4
		    switch pvid 4 2
		fi
	    fi
	    # forward external vlan ports to wan cpu as tagged
	    if [ "$sip_portVLAN" != "" ]; then
		switch vlan set 4 $sip_portVLAN 11111100 0 0 tttttttt
	    fi
	    if [ "$tv_portVLAN" != "" ]; then
		switch vlan set 5 $tv_portVLAN  11111100 0 0 tttttttt
	    fi
	fi

	# pass selected tagged traffic to cpu port as tagged
	VlanLan=`nvram_get 2860 VlanLan | awk '{ gsub(","," "); print }'`
	VlanWifiWan=`nvram_get 2860 VlanWifiWan | awk '{ gsub(","," "); print }'`
	VlanWifiWanINIC=`nvram_get 2860 VlanWifiWanINIC | awk '{ gsub(","," "); print }'`
	VlanWifiLan=`nvram_get 2860 VlanWifiLan | awk '{ gsub(","," "); print }'`
	VlanWifiLanINIC=`nvram_get 2860 VlanWifiLanINIC | awk '{ gsub(","," "); print }'`

	count=6
	# from WAN
	for vid in $VlanWifiWan $VlanWifiWanINIC
	do
	    if [ "$vid" != "0" ]; then
		switch vlan set $count $vid 11111100 0 0 tttttttt
		count="$(($count+1))"
	    fi
	done
	# from LAN
	for vid in $VlanWifiLan $VlanWifiLanINIC $VlanLan
	do
	    if [ "$vid" != "0" ]; then
		switch vlan set $count $vid 11111010 0 0 tttttttt
		count="$(($count+1))"
	    fi
	done

	# clear mac table if vlan configuration changed
	switch clear
}

restore_extdualrgmii()
{
        $LOG "Restore internal MT7621 switch mode to dumb mode (ExtPHY)"

	for port in `seq 0 7`; do
	    switch reg w 2${port}04 ff0000	#ports 0-7 matrix mode
	    switch reg w 2${port}10 810000c0 	#ports 0-7 as transparent port, admit all frames and disable special tag
	done

	# switch port 5 disable (unused)
	switch reg w 3500 00008000

	# clear configured vlan parts
	switch vlan clear

	# reinit all ports
	reinit_all_phys

	# disable snooping
	restore_igmpsnoop

	#clear mac table if vlan configuration changed
	switch clear
}

config_extdualrgmii()
{
	# incomplete write logic in future
        # now only static config without VLANS support
	# use vlan vid 1 for all internal GSW (security mode config)
	# port5 = external GigaPHY

	# cleanup swicth
	restore_extdualrgmii

	$LOG "Config internal MT7621 switch + external GigaPHY $1"

	# prepare switch
	for port in `seq 0 4`; do
	    switch reg w 2${port}10 810000c0 	#ports 0-4 as transparent port, admit all frames and disable special tag
	    switch reg w 2${port}04 ff0003	#ports 0-4 as security mode
	done

	for port in `seq 6 7`; do
	    switch reg w 2${port}10 81000000	#ports 5-7 special tag disable, is user port, admit all frames
	    switch reg w 2${port}04 20df0003	#ports 5-7 egress VLAN Tag Attribution=tagged
	done

	# set cpu port pvid for lan
	switch pvid 6 1

	# VLAN member ports index 0 vlan 1 for lan
	switch vlan set 0 1 11111010

	# set all lan ports pvid to 1
	for index in `seq 0 4` ; do
	    switch pvid $index 1
	done

	# detag internal esw and qdma tagged packets before send to cpu
	for port in `seq 6 7`; do
	    switch tag off $port
	done

	# config hawdware snooping
	config_igmpsnoop

	# clear mac table if vlan configuration changed
	switch clear
}

eval `nvram_buf_get 2860 OperationMode igmpSnoopMode wan_port tv_port sip_port tv_portVLAN sip_portVLAN`

if [ -e /proc/mt7620/gmac ]; then
    procdir="/proc/mt7620/gmac"
elif [ -e /proc/mt7628/gmac ]; then
    procdir="/proc/mt7628/gmac"
elif [ -e /proc/mt7621/gmac ]; then
    procdir="/proc/mt7621/gmac"
else
    $LOG "No switch in system!!!"
    procdir=
fi

if [ "$1" = "3" ]; then
	if [ "$2" = "LLLLL" ]; then
		restore_onergmii
	elif [ "$2" = "EEEEE" ]; then
		enable_all_ports
	elif [ "$2" = "DDDDD" ]; then
		disable_all_ports
	elif [ "$2" = "RRRRR" ]; then
		reset_all_phys
	elif [ "$2" = "FFFFF" ]; then
		reinit_all_phys
	elif [ "$2" = "WWWWW" ]; then
		reset_wan_phys
	elif [ "$2" = "VLANS" ]; then
		config_onergmii VLANS
	else
		config_onergmii $2
	fi
elif [ "$1" = "4" ]; then
	if [ "$2" = "LLLLL" ]; then
		restore_dualrgmii
	elif [ "$2" = "EEEEE" ]; then
		enable_all_ports
	elif [ "$2" = "DDDDD" ]; then
		disable_all_ports
	elif [ "$2" = "RRRRR" ]; then
		reset_all_phys
	elif [ "$2" = "FFFFF" ]; then
		reinit_all_phys
	elif [ "$2" = "WWWWW" ]; then
		reset_wan_phys
	elif [ "$2" = "VLANS" ]; then
		config_dualrgmii VLANS
	else
		config_dualrgmii $2
	fi
elif [ "$1" = "5" ]; then
	if [ "$2" = "LLLLL" ]; then
		restore_extdualrgmii
	elif [ "$2" = "EEEEE" ]; then
		enable_all_ports
	elif [ "$2" = "DDDDD" ]; then
		disable_all_ports
	elif [ "$2" = "RRRRR" ]; then
		reset_all_phys
	elif [ "$2" = "FFFFF" ]; then
		reinit_all_phys
	elif [ "$2" = "WWWWW" ]; then
		reset_wan_phys
	elif [ "$2" = "VLANS" ]; then
		config_extdualrgmii VLANS
	else
		config_extdualrgmii $2
	fi
else
	echo "unknown swith type $1"
	echo ""
	usage "$0"
fi

# remove running flag
rm -f /tmp/vlanconfig_runing
