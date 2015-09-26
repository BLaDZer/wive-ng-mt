#!/bin/sh

#############################################################################################################
# Port/DSCP based Qos helper script for Wive-NG										    #
#############################################################################################################

# include global
. /etc/scripts/global.sh

LOG="logger -t Complex QoS"

# get config
eval `nvram_buf_get 2860 igmpEnabled offloadMode \
    QoS_rate_down QoS_rate_limit_down QoS_rate_up QoS_rate_limit_up \
    QoS_rate_vpn_up QoS_rate_vpn_limit_up`

# get users prio ports
QoS_high_pp=`nvram_get 2860 QoS_high_pp | awk '{ gsub(" ",","); print }'`
QoS_low_pp=`nvram_get 2860 QoS_low_pp | awk '{ gsub(" ",","); print }'`
# get users prio dscps
QoS_high_dscp=`nvram_get 2860 QoS_high_dscp | awk '{ gsub(" ",","); print }'`
QoS_low_dscp=`nvram_get 2860 QoS_low_dscp | awk '{ gsub(" ",","); print }'`

IPTSCR="/etc/qos_firewall"
INCOMING="iptables -A shaper_pre -t mangle"
OUTGOING="iptables -A shaper_post -t mangle"

qos_lm() {
    # Load modules
    modprobe -q sch_sfq
    modprobe -q sch_htb
    modprobe -q xt_mark
    modprobe -q xt_length
    if [ "$QoS_high_dscp" != "" ] || [ "$QoS_low_dscp" != "" ]; then
	modprobe -q xt_dscp
    fi
}

qos_nf() {
    ##################################################################################################################################
    # BUILD CHAINS
    ##################################################################################################################################
    $LOG "Generate $IPTSCR"
    echo "$LOG add qos netfilter rules" >> $IPTSCR
    echo "iptables -N shaper_pre -t mangle > /dev/null 2>&1" >> $IPTSCR
    echo "iptables -F shaper_pre -t mangle > /dev/null 2>&1" >> $IPTSCR
    echo "iptables -N shaper_post -t mangle > /dev/null 2>&1" >> $IPTSCR
    echo "iptables -F shaper_post -t mangle > /dev/null 2>&1" >> $IPTSCR
    echo "iptables -A PREROUTING -t mangle ! -d $mcast_net -j shaper_pre > /dev/null 2>&1" >> $IPTSCR
    echo "iptables -A POSTROUTING -t mangle ! -s $mcast_net -j shaper_post > /dev/null 2>&1" >> $IPTSCR
}

qos_nf_if() {
    ##################################################################################################################################
    # SET MARKERS FOR INCOMING
    ##################################################################################################################################
    # second user high prio ports
    if [ "$QoS_high_pp" != "" ]; then
	echo "$INCOMING -i $wan_if -p tcp -m multiport --sport $QoS_high_pp -j MARK --set-mark 20" >> $IPTSCR
	echo "$INCOMING -i $wan_if -p udp -m multiport --sport $QoS_high_pp -j MARK --set-mark 20" >> $IPTSCR
    fi

    # next user medium prio ports
    if [ "$QoS_low_pp" != "" ]; then
	echo "$INCOMING -i $wan_if -p tcp -m multiport --sport $QoS_low_pp  -j MARK --set-mark 21" >> $IPTSCR
	echo "$INCOMING -i $wan_if -p udp -m multiport --sport $QoS_low_pp  -j MARK --set-mark 21" >> $IPTSCR
    fi

    # second user high prio dscp
    if [ "$QoS_high_dscp" != "" ]; then
	echo "$INCOMING -i $wan_if -m dscp --dscp-class $QoS_high_dscp -j MARK --set-mark 20" >> $IPTSCR
    fi

    # next user medium prio dscp
    if [ "$QoS_low_dscp" != "" ]; then
	echo "$INCOMING -i $wan_if -m dscp --dscp-class $QoS_low_dscp  -j MARK --set-mark 21" >> $IPTSCR
    fi

    # if use offload do not mark all others high prio pakcets
    if [ "$offloadMode" = "0" ]; then
	# tcp SYN and small size packets to high prio
	echo "$INCOMING -i $wan_if -p tcp --syn -j MARK --set-mark 20" >> $IPTSCR
	echo "$INCOMING -i $wan_if -p tcp -m length --length :64 -j MARK --set-mark 20" >> $IPTSCR

	# all others set as low prio
	echo "$INCOMING -i $wan_if -m mark --mark 0 -j MARK --set-mark 22" >> $IPTSCR
    fi

    ##################################################################################################################################
    # SET MARKERS FOR OUTGOING
    ##################################################################################################################################
    # second user high prio ports
    if [ "$QoS_high_pp" != "" ]; then
	echo "$OUTGOING -o $wan_if -p tcp -m multiport --dport $QoS_high_pp -j MARK --set-mark 23" >> $IPTSCR
	echo "$OUTGOING -o $wan_if -p udp -m multiport --dport $QoS_high_pp -j MARK --set-mark 23" >> $IPTSCR
    fi

    # next user low prio ports
    if [ "$QoS_low_pp" != "" ]; then
	echo "$OUTGOING -o $wan_if -p tcp -m multiport --dport $QoS_low_pp -j MARK --set-mark 24" >> $IPTSCR
	echo "$OUTGOING -o $wan_if -p udp -m multiport --dport $QoS_low_pp -j MARK --set-mark 24" >> $IPTSCR
    fi

    # if use offload do not mark all others high prio pakcets
    if [ "$offloadMode" = "0" ]; then
	# SYN/ICMP and small size packets to hih prio
	echo "$OUTGOING -o $wan_if -p tcp --syn -j MARK --set-mark 23" >> $IPTSCR
	echo "$OUTGOING -o $wan_if -p tcp -m length --length :64 -j MARK --set-mark 23" >> $IPTSCR
	echo "$OUTGOING -o $wan_if -p icmp -j MARK --set-mark 23" >> $IPTSCR

	# all others set as low prio
	echo "$OUTGOING -o $wan_if -m mark --mark 0 -j MARK --set-mark 24" >> $IPTSCR
    fi
}

qos_tc_lan() {
    ##################################################################################################################################
    # FROM UPLINK TO LAN. BIND AT LAN_IF
    ##################################################################################################################################
    $LOG "All incoming $lan_if rate: normal $QoS_rate_limit_down , maximum $QoS_rate_down (kbit/s)"
    # root classes
    tc qdisc add dev $lan_if root handle 1: htb default 22
    # all trafs
    tc class add dev $lan_if parent 1: classid 1:2 htb rate ${QoS_rate_down}kbit ceil 90mbit quantum 1500 burst 256k
    # overhead class for mcast and local connections
    tc class add dev $lan_if parent 1: classid 1:3 htb rate 98mbit ceil 100mbit quantum 1500 burst 1024k

    # subclass prio
    tc class add dev $lan_if parent 1:2 classid 1:20 htb rate ${QoS_rate_limit_down}kbit ceil ${QoS_rate_down}kbit prio 1 quantum 1500
    tc class add dev $lan_if parent 1:2 classid 1:21 htb rate ${QoS_rate_limit_down}kbit ceil ${QoS_rate_down}kbit prio 2 quantum 1500
    tc class add dev $lan_if parent 1:2 classid 1:22 htb rate ${QoS_rate_limit_down}kbit ceil ${QoS_rate_down}kbit prio 3 quantum 1500

    # add sfq discipline to end htb class
    tc qdisc add dev $lan_if parent 1:3 handle 3: sfq perturb 10 quantum 1500
    tc qdisc add dev $lan_if parent 1:20 handle 20: sfq perturb 10 quantum 1500
    tc qdisc add dev $lan_if parent 1:21 handle 21: sfq perturb 10 quantum 1500
    tc qdisc add dev $lan_if parent 1:22 handle 22: sfq perturb 10 quantum 1500

    # switch sfq to use dst hash
    tc filter add dev $lan_if protocol ip pref 1 parent 1:3 handle 3 flow hash keys dst divisor 1024
    tc filter add dev $lan_if protocol ip pref 1 parent 1:20 handle 20 flow hash keys dst divisor 1024
    tc filter add dev $lan_if protocol ip pref 1 parent 1:21 handle 21 flow hash keys dst divisor 1024
    tc filter add dev $lan_if protocol ip pref 1 parent 1:22 handle 22 flow hash keys dst divisor 1024

    # local connections and multicast to router must be overheaded send to 1:3
    tc filter add dev $lan_if parent 1:0 protocol ip prio 0 u32 match ip src $lan_ipaddr flowid 1:3
    if [ "$igmpEnabled" != "0" ]; then
        tc filter add dev $lan_if parent 1:0 protocol ip prio 0 u32 match ip src $mcast_net flowid 1:3
    fi

    # filters for marked in prerouting
    if [ "$OperationMode" != "0" ] && [ "$ApCliBridgeOnly" != "1" ]; then
	# user marked filters
	tc filter add dev $lan_if parent 1:0 prio 1 protocol ip handle 20 fw flowid 1:20
	tc filter add dev $lan_if parent 1:0 prio 2 protocol ip handle 21 fw flowid 1:21
	tc filter add dev $lan_if parent 1:0 prio 3 protocol ip handle 22 fw flowid 1:22
    fi

}

gos_tc_wan() {
    ##################################################################################################################################
    # FROM LAN TO UPLINK. BIND AT WAN_IF
    ##################################################################################################################################
    $LOG "All outgoing $wan_if rate: normal $QoS_rate_limit_up , maximum $QoS_rate_up (kbit/s)"
    tc qdisc add dev $wan_if root handle 1: htb default 24
    tc class add dev $wan_if parent 1:  classid 1:1 htb rate ${QoS_rate_up}kbit quantum 1500 burst 50k

    tc class add dev $wan_if parent 1:1 classid 1:23 htb rate ${QoS_rate_limit_up}kbit ceil ${QoS_rate_up}kbit prio 0 quantum 1500
    tc class add dev $wan_if parent 1:1 classid 1:24 htb rate ${QoS_rate_limit_up}kbit ceil ${QoS_rate_up}kbit prio 1 quantum 1500

    tc qdisc add dev $wan_if parent 1:23 handle 23: sfq perturb 10 quantum 1500
    tc qdisc add dev $wan_if parent 1:24 handle 24: sfq perturb 10 quantum 1500

    # filters for marked in prerouting
    if [ "$OperationMode" != "0" ] && [ "$ApCliBridgeOnly" != "1" ]; then
	tc filter add dev $wan_if parent 1:0 prio 0 protocol ip handle 23 fw flowid 1:23
	tc filter add dev $wan_if parent 1:0 prio 1 protocol ip handle 24 fw flowid 1:24
    fi
}

# load modules
qos_lm

# add netsched htb tubes
qos_tc_lan

# create netfilters chains for qos
if  [ "$OperationMode" != "0" -a "$ApCliBridgeOnly" != "1" ] || [ "$vpnEnabled" = "on" -a "$wan_if" != "$real_wan_if" ]; then
    qos_nf
fi

if [ "$OperationMode" != "0" ] && [ "$ApCliBridgeOnly" != "1" ] && [ "$purepppoemode" != "1" ]; then
    # build rules for WAN
    # not need shape at wan if in pure pppoe mode or bridget mode
    qos_nf_if
    gos_tc_wan
fi

# build rules for VPN
if [ "$vpnEnabled" = "on" ] && [ "$wan_if" != "$real_wan_if" ]; then
    # reload global
    . /etc/scripts/global.sh
    # check device ready
    if [ "$real_wan_if" != "" ] && [ -e "/sys/devices/virtual/net/$real_wan_if" ]; then
	wan_if="$real_wan_if"
	QoS_rate_up="$QoS_rate_vpn_up"
	QoS_rate_limit_up="$QoS_rate_vpn_limit_up"
	qos_nf_if
	gos_tc_wan
    fi
fi
