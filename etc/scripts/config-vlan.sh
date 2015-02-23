#!/bin/sh

############################################################
# config-vlan.sh - configure vlan switch particion helper  #
#                                                          #
# usage: config-vlan.sh <switch_type> <vlan_type>          #
############################################################

LOG="logger -t ESW"

usage() {
	echo "Usage:"
	echo "  $0 3/4 LLLLL - ESW restore config to no VLAN partition"
	echo "  $0 3/4 EEEEE - ESW enable all ports 100FD"
	echo "  $0 3/4 DDDDD - ESW disable all ports"
	echo "  $0 3/4 RRRRR - ESW reset all ports"
	echo "  $0 3/4 WWWWW - ESW reinit WAN port at switch"
	echo "  $0 3/4 FFFFF - ESW full reinit switch"
	echo "  $0 3/4 xxxxx - ESW config free parts (x = W or x = L, W - WAN group, L - LAN group)"
	echo "_________"
	echo " 	devnum: 3 for MT7620/MT7628, 4 for MT7621"
	echo "_________"
	exit 0
}

disable_all_ports() {
        for port in `seq 0 4`; do
	    mii_mgr -s -p $port -r 0 -v 0x0800
	done
}

enable_all_ports() {
	for port in `seq 0 4`; do
	    mii_mgr -s -p $port -r 0 -v 0x9000
	done
}

# arg1:  phy address.
link_down() {
	# get original register value
	get_mii=`mii_mgr -g -p $1 -r 0`
	orig=`echo $get_mii | sed 's/^.....................//'`

	# stupid hex value calculation.
	pre=`echo $orig | sed 's/...$//'`
	post=`echo $orig | sed 's/^..//'`
	num_hex=`echo $orig | sed 's/^.//' | sed 's/..$//'`
	case $num_hex in
		"0")	rep="8"	;;
		"1")	rep="9"	;;
		"2")	rep="a"	;;
		"3")	rep="b"	;;
		"4")	rep="c"	;;
		"5")	rep="d"	;;
		"6")	rep="e"	;;
		"7")	rep="f"	;;
		# The power is already down
		*)		$LOG "Warning in PHY reset script (link down)";return;;
	esac
	new=$pre$rep$post
	# power down
	mii_mgr -s -p "$1" -r 0 -v $new
}

link_up() {
	# get original register value
	get_mii=`mii_mgr -g -p "$1" -r 0`
	orig=`echo $get_mii | sed 's/^.....................//'`

	# stupid hex value calculation.
	pre=`echo $orig | sed 's/...$//'`
	post=`echo $orig | sed 's/^..//'` 
	num_hex=`echo $orig | sed 's/^.//' | sed 's/..$//'`
	case $num_hex in
		"8")	rep="0"	;;
		"9")	rep="1"	;;
		"a")	rep="2"	;;
		"b")	rep="3"	;;
		"c")	rep="4"	;;
		"d")	rep="5"	;;
		"e")	rep="6"	;;
		"f")	rep="7"	;;
		# The power is already up
		*)		$LOG "Warning in PHY reset script (link up)";return;;
	esac
	new=$pre$rep$post
	# power up
	mii_mgr -s -p "$1" -r 0 -v $new
	# link up
	mii_mgr -s -p "$1" -r 0 -v 0x9000
}

reset_all_phys() {
	$LOG "Reset all phy port"
	if [ "$OperationMode" = "1" ]; then
	    # Ports down skip WAN port
	    if [ "$wan_portN" = "0" ]; then
		start=0
		end=3
	    else
		start=1
		end=4
	    fi
	else
	    # All ports down
	    start=0
	    end=4
	fi

	# disable ports
	for port in `seq $start $end`; do
    	    link_down $port
	done

	# force Windows clients to renew IP and update DNS server
	sleep 1

	# enable ports
	for port in `seq $start $end`; do
    	    link_up $port
	done
}

reset_wan_phys() {
	$LOG "Reset wan phy port"
	if [ "$OperationMode" = "1" ]; then
	    if [ "$wan_portN" = "0" ]; then
		link_down 4
		link_up 4
	    else
		link_down 0
		link_up 0
	    fi
	fi
}

reinit_all_phys() {
	disable_all_ports
	enable_all_ports
	reset_all_phys
}

restore7620Esw()
{
        $LOG "Restore internal MT7620 switch mode to dumb mode"
	for port in `seq 0 7`; do
	    switch reg w 2${port}04 ff0000	#ports 0-7 matrix mode
	    switch reg w 2${port}10 810000c0 	#ports 0-7 as transparent mode
	done

	switch reg w 3500 00008000		#port 5 link down
	switch reg w 0010 7f7f7fe0		#port 6 as CPU Port
	switch reg w 3600 0005e33b		#port 6 force up, 1000FD

	# clear mac table if vlan configuration changed
	switch clear
}

config7620Esw()
{
	#####################################################################
	# now config support only internal 100FDX ESW
	#####################################################################

	# cleanup switch
	restore7620Esw

	# prepare switch
	for port in `seq 6 7`; do
	    switch reg w 2${port}04 20df0003	#ports 6-7 egress VLAN Tag Attribution=tagged
	    switch reg w 2${port}10 8100000	#ports 6-7 special tag disable
	    switch reg w 2${port}10 81000000	#ports 6-7 is user port, admit all frames
	done

	for port in `seq 0 5`; do
	    switch reg w 2${port}04 ff0003	#ports 0-5 as security mode
	    switch reg w 2${port}10 810000c0	#ports 0-5 as transparent port
	done

	if [ "$1" != "VLANS" ]; then
	    $LOG "Config internal MT7620 switch mode $1"
	    # replace W/L to 0/1 for create masks and add static mask suffix
	    mask1=`echo "$1" | sed 's/[0-9]/0/g;s/W/0/g;s/L/1/g' | awk {' print $1 "111" '}`
	    mask2=`echo "$1" | sed 's/[0-9]/0/g;s/W/1/g;s/L/0/g' | awk {' print $1 "011" '}`
	    # replace W/L to 2/1 and add space after symbols for set pvids mask
	    pvids=`echo "$1" | sed 's/[0-9]/0/g;s/W/2/g;s/L/1/g' | sed -e "s/.\{1\}/&\ /g"`

	    # VLAN member ports index 0 vlan 1 mask1
	    switch vlan set 0 1 $mask1
	    # VLAN member ports index 1 vlan 2 $mask2
	    switch vlan set 1 2 $mask2

	    index=0
	    # set user ports pvid
	    for pvid in $pvids ; do
		switch pvid $index $pvid
		let index=index+1
	    done
	    $LOG "mask1:$mask1,mask2:$mask2,pvids:$pvids"
	else
	    $LOG "TV/STB/SIP with VLANs mode enabled."
	    # internal VLAN for TV = 3, for SIP = 4
	    if [ "$wan_port" = "4" ]; then
		# tv and sip
		if [ "$tv_port" = "1" ] && [ "$sip_port" = "1" ]; then
		    #V LAN member port
		    switch vlan set 0 1 00011111
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
		    switch vlan set 0 1 00111111
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
		# without bridget ports
		    # VLAN member port
		    switch vlan set 0 1 01011111
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
		    switch vlan set 0 1 11000111
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
		    switch vlan set 0 1 11100111
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
		    switch vlan set 0 1 11010111
		    switch vlan set 1 2 00001011
		    switch vlan set 2 4 00100011
		    # set PVID
		    switch pvid 0 1
		    switch pvid 1 1
		    switch pvid 2 1
		    switch pvid 3 4
		    switch pvid 4 2
		else
		    $LOG "Error vlan config!"
		fi
	    fi
	fi

	# set cpu port pvid
	switch pvid 5 1

	# post config
	switch reg w 3500 00008000		#port 5 link down
	switch reg w 0010 7f7f7fe0		#port 6 as CPU Port
	switch reg w 3600 0005e33b		#port 6 force up, 1000FD

	# clear mac table if vlan configuration changed
	switch clear
}

restore7530Esw()
{
        $LOG "Restore internal MT7621 switch mode to dumb mode"
	for port in `seq 0 6`; do
	    switch reg w 2${port}04 ff0000		#ports 0-6 matrix mode
	    switch reg w 2${port}10 810000c0 	#ports 0-6 as transparent mode
	done

	#clear mac table if vlan configuration changed
	switch clear
}

config7530Esw()
{
	#####################################################################
	# internal 1000FDX 7530 GSW
	#####################################################################

	# cleanup swicth
	restore7530Esw

	if [ "$1" != "VLANS" ]; then
	    $LOG "Config internal MT7621 switch mode $1"
	    # replace W/L to 0/1 for create masks and add static mask suffix
	    mask1=`echo "$1" | sed 's/[0-9]/0/g;s/W/0/g;s/L/1/g' | awk {' print $1 "010" '}`
	    mask2=`echo "$1" | sed 's/[0-9]/0/g;s/W/1/g;s/L/0/g' | awk {' print $1 "100" '}`
	    # replace W/L to 2/1 and add space after symbols for set pvids mask
	    pvids=`echo "$1" | sed 's/[0-9]/0/g;s/W/2/g;s/L/1/g' | sed -e "s/.\{1\}/&\ /g"`

	    # VLAN member ports index 0 vlan 1 mask1
	    switch vlan set 0 1 $mask1
	    # VLAN member ports index 1 vlan 2 $mask2
	    switch vlan set 1 2 $mask2

	    index=0
	    # set user ports pvid
	    for pvid in $pvids ; do
		switch pvid $index $pvid
		let index=index+1
	    done
	    $LOG "mask1:$mask1,mask2:$mask2,pvids:$pvids"
         else
	    ######################################################################
	    #		SECTION TO TAGGED PORTS NEED WRITE FUTURE		 #
	    # ToDo: config switch with black holes for vlan bridgets porst by	 #
	    # 		config 7530 W0LLL command and add needed calls for bridge#
	    ######################################################################
	    $LOG "External vlan portmap not supported NOW!!!"
	    config7530Esw LLLLW
	fi

	# post config
	for port in `seq 0 6`; do
	    switch reg w 2${port}04 ff0003	#ports 0-6 as security mode
	    switch reg w 2${port}10 810000c0	#ports 0-6 as transparent port, admit all frames and disable special tag
	done

	# set cpu ports pvids
	switch pvid 5 2
	switch pvid 6 1

	# detag packets before send to cpu
	switch tag off 6
	switch tag off 5

	# clear mac table if vlan configuration changed
	switch clear

}

eval `nvram_buf_get 2860 OperationMode wan_port tv_port sip_port`

if [ "$1" = "3" ]; then
	if [ "$2" = "LLLLL" ]; then
		restore7620Esw
	elif [ "$2" = "EEEEE" ]; then
		enable_all_ports
	elif [ "$2" = "DDDDD" ]; then
		disable_all_ports
	elif [ "$2" = "RRRRR" ]; then
		reset_all_phys
	elif [ "$2" = "WWWWW" ]; then
		reset_wan_phys
	elif [ "$2" = "FFFFF" ]; then
		reinit_all_phys
	elif [ "$2" = "VLANS" ]; then
		config7620Esw VLANS
	else
		config7620Esw $2
	fi
elif [ "$1" = "4" ]; then
	if [ "$2" = "LLLLL" ]; then
		restore7530Esw
	elif [ "$2" = "EEEEE" ]; then
		enable_all_ports
	elif [ "$2" = "DDDDD" ]; then
		disable_all_ports
	elif [ "$2" = "RRRRR" ]; then
		reset_all_phys
	elif [ "$2" = "WWWWW" ]; then
		reset_wan_phys
	elif [ "$2" = "FFFFF" ]; then
		reinit_all_phys
	elif [ "$2" = "VLANS" ]; then
		config7530Esw VLANS
	else
		config7530Esw $2
	fi
else
	echo "unknown swith type $1"
	echo ""
	usage "$0"
fi
