#!/bin/sh

#############################################################################################################
# Qos helper script for Wive-NG										    #
#############################################################################################################

# include global
. /etc/scripts/global.sh

LOG="logger -t Simple Prio port based QoS"

    $LOG "Load netsched  modules."
    mod="sch_esfq sch_sfq cls_u32 em_u32"
    for module in $mod
    do
	modprobe -q $module
    done

    # PRIOMAP+ESQ2LAN
    tc qdisc add dev $lan_if root handle 1: prio priomap 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 0
    tc qdisc add dev $lan_if parent 1:1 handle 10: sfq perturb 10 quantum 1500
    tc qdisc add dev $lan_if parent 1:2 handle 20: sfq perturb 10 quantum 1500
    tc qdisc add dev $lan_if parent 1:3 handle 30: sfq perturb 10 quantum 1500
    tc filter add dev $lan_if protocol ip pref 1 parent 10: handle 10 flow hash keys dst divisor 1024
    tc filter add dev $lan_if protocol ip pref 1 parent 20: handle 20 flow hash keys dst divisor 1024
    tc filter add dev $lan_if protocol ip pref 1 parent 30: handle 30 flow hash keys dst divisor 1024

    if [ "$OperationMode" != "0" ] && [ "$ApCliBridgeOnly" != "1" ]; then
	# PRIOMAP+SFQ2WAN
	tc qdisc add dev $real_wan_if root handle 1: prio priomap 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 0
	tc qdisc add dev $real_wan_if parent 1:1 handle 10: sfq
	tc qdisc add dev $real_wan_if parent 1:2 handle 20: sfq
	tc qdisc add dev $real_wan_if parent 1:3 handle 30: sfq
    fi

    # PORTS2PRIO
    HIG_P="22 23 53 80 443 1720 5060"
    MED_P="110 4190 1720 5190 5191 5192 5193 5222 5223 5269 5270 8080"
    LOW_P="20 25 21 8010"

    for port in $HIG_P
    do
	tc filter add dev $lan_if parent 1:0 protocol ip prio 1 u32 match ip sport $port 0xffff flowid 1:1
	tc filter add dev $lan_if parent 1:0 protocol ip prio 1 u32 match ip dport $port 0xffff flowid 1:1
	if [ "$OperationMode" != "0" ] && [ "$ApCliBridgeOnly" != "1" ]; then
	    tc filter add dev $real_wan_if parent 1:0 protocol ip prio 1 u32 match ip sport $port 0xffff flowid 1:1
	    tc filter add dev $real_wan_if parent 1:0 protocol ip prio 1 u32 match ip dport $port 0xffff flowid 1:1
	fi
    done

    for port in $MED_P
    do
	tc filter add dev $lan_if parent 1:0 protocol ip prio 1 u32 match ip sport $port 0xffff flowid 1:2
	tc filter add dev $lan_if parent 1:0 protocol ip prio 1 u32 match ip dport $port 0xffff flowid 1:2
	if [ "$OperationMode" != "0" ] && [ "$ApCliBridgeOnly" != "1" ]; then
	    tc filter add dev $real_wan_if parent 1:0 protocol ip prio 1 u32 match ip sport $port 0xffff flowid 1:2
	    tc filter add dev $real_wan_if parent 1:0 protocol ip prio 1 u32 match ip dport $port 0xffff flowid 1:2
	fi
    done

    for port in $LOW_P
    do
	tc filter add dev $lan_if parent 1:0 protocol ip prio 1 u32 match ip sport $port 0xffff flowid 1:3
	tc filter add dev $lan_if parent 1:0 protocol ip prio 1 u32 match ip dport $port 0xffff flowid 1:3
	if [ "$OperationMode" != "0" ] && [ "$ApCliBridgeOnly" != "1" ]; then
	    tc filter add dev $real_wan_if parent 1:0 protocol ip prio 1 u32 match ip sport $port 0xffff flowid 1:3
	    tc filter add dev $real_wan_if parent 1:0 protocol ip prio 1 u32 match ip dport $port 0xffff flowid 1:3
	fi
    done

    # icmp
    tc filter add dev $lan_if parent 1:0 protocol ip prio 1 u32 match ip protocol 1 0xff flowid 1:1
    if [ "$OperationMode" != "0" ] && [ "$ApCliBridgeOnly" != "1" ]; then
	tc filter add dev $real_wan_if parent 1:0 protocol ip prio 1 u32 match ip protocol 1 0xff flowid 1:1
    fi

    # udp
    tc filter add dev $lan_if parent 1:0 protocol ip prio 2 u32 match ip protocol 17 0xff flowid 1:2
    if [ "$OperationMode" != "0" ] && [ "$ApCliBridgeOnly" != "1" ]; then
	tc filter add dev $real_wan_if parent 1:0 protocol ip prio 2 u32 match ip protocol 17 0xff flowid 1:2
    fi
