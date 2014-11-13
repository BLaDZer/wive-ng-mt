#!/bin/sh

#############################################################################################################
# Qos helper script for Wive-NG										    #
#############################################################################################################

# include global
. /etc/scripts/global.sh

LOG="logger -t QoS TOS"
IPTSCR="/etc/qos_firewall"

    $LOG "Load module."
    if [ ! -d /sys/module/ipt_TOS ]; then
	modprobe -q ipt_TOS
    fi

    $LOG "Generate $IPTSCR"
    echo "$LOG add simple_qos netfilter rules" >> $IPTSCR
    echo "iptables -N simple_qos -t mangle > /dev/null 2>&1" >> $IPTSCR
    echo "iptables -F simple_qos -t mangle > /dev/null 2>&1" >> $IPTSCR
    echo "iptables -A OUTPUT -t mangle -j simple_qos > /dev/null 2>&1" >> $IPTSCR
    echo "iptables -A FORWARD ! -o $lan_if -t mangle ! -d 224.0.0.0/4 -j simple_qos > /dev/null 2>&1" >> $IPTSCR

    MIN_DEL="-j TOS --set-tos Minimize-Delay"
    MAX_THR="-j TOS --set-tos Maximize-Throughput"

    MIN_DEL_UDP_P="53 1720 5060"
    MIN_DEL_TCP_P="22 23 53 80 110 443 4190 1720 5060 5190:5193 5222:5223 5269:5270 8080"
    MAX_THR_TCP_P="20 25 21 8010"

    echo "iptables -A simple_qos -t mangle -p icmp $MIN_DEL" >> $IPTSCR
    echo "iptables -A simple_qos -t mangle -p igmp $MIN_DEL" >> $IPTSCR

    for port in $MIN_DEL_UDP_P
    do
	echo "iptables -A simple_qos -t mangle -p udp --dport $port $MIN_DEL" >> $IPTSCR
    done

    for port in $MIN_DEL_TCP_P
    do
	echo "iptables -A simple_qos -t mangle -p tcp --dport $port $MIN_DEL" >> $IPTSCR
    done

    for port in $MAX_THR_TCP_P
    do
	echo "iptables -A simple_qos -t mangle -p tcp --dport $port $MAX_THR" >> $IPTSCR
    done
