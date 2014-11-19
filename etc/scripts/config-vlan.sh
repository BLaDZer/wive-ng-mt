#!/bin/sh

############################################################
# config-vlan.sh - configure vlan switch particion helper  #
#                                                          #
# usage: config-vlan.sh <switch_type> <vlan_type>          #
#   switch_type: 0=IC+, 1=vtss, 2=esw3050x                 #
#   vlan_type: 0=no_vlan, 1=vlan, LLLLW=wan_4, WLLLL=wan_0 #
############################################################

LOG="logger -t ESW"

usage() {
	echo "Usage:"
	echo "  $0 3 0 - restore Ralink RT6855/MT7620/MT7621 ESW to no VLAN partition"
	echo "  $0 3 EEEEE - config RT3052 Enable all ports 100FD"
	echo "  $0 3 DDDDD - config RT3052 Disable all ports"
	echo "  $0 3 RRRRR - config RT3052 Reset all ports"
	echo "  $0 3 WWWWW - config RT3052 Reinit WAN port at switch"
	echo "  $0 3 FFFFF - config RT3052 Full reinit switch"
	echo "  $0 3 LLLLW - config Ralink RT6855/MT7620/MT7621 ESW with VLAN and WAN at port 4"
	echo "  $0 3 WLLLL - config Ralink RT6855/MT7620/MT7621 ESW with VLAN and WAN at port 0"
	echo "  $0 3 LLLWW - config Ralink RT6855/MT7620/MT7621 ESW with LAN at ports 0-2 and WAN at port 3-4"
	echo "  $0 3 WWLLL - config Ralink RT6855/MT7620/MT7621 ESW with LAN at ports 2-4 and WAN at port 0-1"
	echo "  $0 3 12345 - config Ralink RT6855/MT7620/MT7621 ESW with VLAN 1~5 at port 0~4"
	exit 0
}

disableEsw() {
        for i in `seq 0 4`; do
	    mii_mgr -s -p $i -r 0 -v 0x0800
	done
}

enableEsw() {
	for i in `seq 0 4`; do
	    mii_mgr -s -p $i -r 0 -v 0x9000
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
	eval `nvram_buf_get 2860 OperationMode wan_port`
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
	for i in `seq $start $end`; do
    	    link_down $i
	done

	# force Windows clients to renew IP and update DNS server
	sleep 1

	# enable ports
	for i in `seq $start $end`; do
    	    link_up $i
	done
}

reset_wan_phys() {
	$LOG "Reset wan phy port"
	eval `nvram_buf_get 2860 OperationMode wan_port`
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
	disableEsw
	enableEsw
	reset_all_phys
}

config6855Esw()
{
	# now config support only internal 100FDX ESW
	for i in `seq 6 7`; do
	    switch reg w 2${i}04 20df0003	#ports 6-7 egress VLAN Tag Attribution=tagged
	    switch reg w 2${i}10 8100000	#ports 6-7 special tag disable
	    switch reg w 2${i}10 81000000	#ports 6-7 is user port, admit all frames
	done
	for i in `seq 0 5`; do
	    switch reg w 2${i}04 ff0003		#ports 0-5 as security mode
	    switch reg w 2${i}10 810000c0	#ports 0-5 as transparent port
	done

	if [ -f /etc/scripts/switchadv.sh ]; then
		#this hook for advanced SWITCH/VLAN map configure
		$LOG "Call user /etc/scripts/switchadv.sh script SWITCHCONF mode."
		/etc/scripts/switchadv.sh "SWITCHCONF"
	elif [ "$1" = "LLLLW" ]; then
		#set PVID
		switch reg w 2014 10001 #port0
		switch reg w 2114 10001 #port1
		switch reg w 2214 10001 #port2
		switch reg w 2314 10001 #port3
		switch reg w 2414 10002 #port4
		switch reg w 2514 10001 #port5
		#VLAN member port
		switch vlan set 0 1 11110111
		switch vlan set 1 2 00001011
	elif [ "$1" = "LLLLWW" ]; then
		#set PVID
		switch reg w 2014 10001 #port0
		switch reg w 2114 10001 #port1
		switch reg w 2214 10001 #port2
		switch reg w 2314 10002 #port3
		switch reg w 2414 10002 #port4
		switch reg w 2514 10001 #port5
		#VLAN member port
		switch vlan set 0 1 11100111
		switch vlan set 1 2 00011011
	elif [ "$1" = "WLLLL" ]; then
		#set PVID
		switch reg w 2014 10002 #port0
		switch reg w 2114 10001 #port1
		switch reg w 2214 10001 #port2
		switch reg w 2314 10001 #port3
		switch reg w 2414 10001 #port4
		switch reg w 2514 10001 #port5
		#VLAN member port
		switch vlan set 0 1 01111111
		switch vlan set 1 2 10000011
	elif [ "$1" = "WWLLL" ]; then
		#set PVID
		switch reg w 2014 10002 #port0
		switch reg w 2114 10002 #port1
		switch reg w 2214 10001 #port2
		switch reg w 2314 10001 #port3
		switch reg w 2414 10001 #port4
		switch reg w 2514 10001 #port5
		#VLAN member port
		switch vlan set 0 1 00111111
		switch vlan set 1 2 11000011
	elif [ "$1" = "12345" ]; then
		#set PVID
		switch reg w 2014 10001 #port0
		switch reg w 2114 10002 #port1
		switch reg w 2214 10003 #port2
		switch reg w 2314 10004 #port3
		switch reg w 2414 10005 #port4
		switch reg w 2514 10006 #port5
		#VLAN member port
		switch vlan set 0 1 10000011
		switch vlan set 1 2 01000011
		switch vlan set 2 3 00100011
		switch vlan set 3 4 00010011
		switch vlan set 4 5 00001011
		switch vlan set 5 6 00000111
	fi

	switch reg w 3500 00008000		#port 5 link down
	switch reg w 0010 7f7f7fe0		#port 6 as CPU Port
	switch reg w 3600 0005e33b		#port 6 force up, 1000FD

	#switch reg w 701c 0800000c		#enlarge FE2SW_IPG

	$LOG "Switch configured for $1 mode."

	#clear mac table if vlan configuration changed
	switch clear
}

restore6855Esw()
{
	# now config support only internal 100FDX ESW
	$LOG "restore GSW to dump switch mode"
	for i in `seq 0 7`; do
	    switch reg w 2${i}04 ff0000		#ports 0-7 matrix mode
	    switch reg w 2${i}10 810000c0 	#ports 0-7 as transparen mode
	done

	switch reg w 3500 00008000		#port 5 link down
	switch reg w 0010 7f7f7fe0		#port 6 as CPU Port
	switch reg w 3600 0005e33b		#port 6 force up, 1000FD

	#clear mac table if vlan configuration changed
	switch clear
}

if [ "$1" = "3" ]; then
	SWITCH_MODE=3
	if [ "$2" = "0" ] || [ "$2" = "LLLLL" ]; then
		restore6855Esw
	elif [ "$2" = "EEEEE" ]; then
		enableEsw
	elif [ "$2" = "DDDDD" ]; then
		disableEsw
	elif [ "$2" = "RRRRR" ]; then
		reset_all_phys
	elif [ "$2" = "WWWWW" ]; then
		reset_wan_phys
	elif [ "$2" = "FFFFF" ]; then
		reinit_all_phys
	elif [ "$2" = "LLLLW" ]; then
		config6855Esw LLLLW
	elif [ "$2" = "WLLLL" ]; then
		config6855Esw WLLLL
	elif [ "$2" = "LLLWW" ]; then
		config6855Esw LLLWW
	elif [ "$2" = "WWLLL" ]; then
		config6855Esw WWLLL
	elif [ "$2" = "12345" ]; then
		config6855Esw 12345
	else
		echo "unknown vlan type $2"
		echo ""
		usage $0
	fi
else
	echo "unknown swith type $1"
	echo ""
	usage "$0"
fi
