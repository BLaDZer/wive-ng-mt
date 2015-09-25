#!/bin/sh

#############################################################################################################
# Qos helper script for Wive-NG										    #
#############################################################################################################

# include global
. /etc/scripts/global.sh

LOG="logger -t Codel QoS"

qos_lm() {
    # Load modules
    modprobe -q sch_fq_codel
}

qos_tc() {
    $LOG "Add codel for all interfaces."
    tc qdisc add dev $lan_if root fq_codel		> /dev/null 2>&1
    # in bridge modes enable shaper only to lan path
    if [ "$OperationMode" != "0" ] && [ "$ApCliBridgeOnly" != "1" ]; then
	tc qdisc add dev $wan_if root fq_codel		> /dev/null 2>&1
	tc qdisc add dev $real_wan_if root fq_codel	> /dev/null 2>&1
    fi
}

# load modules
qos_lm
# add netsched rules
qos_tc
