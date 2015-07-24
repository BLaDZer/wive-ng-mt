#!/bin/sh

#################################################
# config-l2tp.sh - configure L2TP client helper #
#################################################

# include global config
. /etc/scripts/global.sh

# stop all pppd/xl2tpd daemons
service vpnhelper killall_vpn

LOG="logger -t vpnhelper-l2tp"

get_param() {
    eval `nvram_buf_get 2860 vpnServer vpnUser vpnPassword vpnMTU vpnMPPE vpnPeerDNS vpnDebug vpnAuthProtocol vpnEnableLCP vpnLCPFailure vpnLCPInterval vpnTestReachable`

    ppp="/etc/ppp"
    var="/var/run/xl2tpd"
    mkdir -p $var
}

check_param() {
    if [ "$vpnServer" = "" ] || [ "$vpnUser" = "" ] || [ "$vpnPassword" = "" ]; then
	$LOG "Server adress, username or password not set. Exit!"
	exit 1
    fi
}

get_vpn_ip() {
    ##############################################
    # Check $vpnServer variable for ip/name set
    ##############################################
    is_ip=`ipcalc -sn $vpnServer`
    if [ "$is_ip" = "" ]; then
	##########################################
	##########TRY RESOLV SERVER IP############
	##########################################
	$LOG "Get vpn server $vpnServer ip adress"
	count=0
	resolved=0
	while [ $resolved -eq 0 ]; do
	    NS=`ipget $vpnServer | tail -q -n1`
	    if [ "$NS" != "" ]; then
		resolved=1
		SERVER="$NS"
	    fi
	    if [ "$count" = "10" ]; then
		resolved=1
    		SERVER="$vpnServer"
    		$LOG "Not resolve adress for $SERVER"
	    fi
	    usleep 500000
	    count="$(($count+1))"
	done
    else
	    SERVER="$vpnServer"
    fi

    $LOG "Server adress is $SERVER"

    # always create /tmp/vpnip
    # fix if server string = IP
    echo "$SERVER" > /tmp/vpnip
}

set_routest_to_server() {
    $LOG "Set routes to vpn servers."
    if [ "$wanConnectionMode" != "STATIC" ]; then
	if [ -f /tmp/default.gw ]; then
	    newdgw=`cat /tmp/default.gw`
	else
	    newdgw=""
	fi
    else
	newdgw="$wan_gateway"
	if [ "$newdgw" = "0.0.0.0" ]; then
	    newdgw=""
	fi
    fi

    if [ "$newdgw" != "" ] && [ "$newdgw" != "$SERVER" ]; then
	dgw_net=`ipcalc "$newdgw" -sn | cut -f 2- -d =`
	srv_net=`ipcalc "$SERVER" -sn | cut -f 2- -d =`
	if [ "$dgw_net" != "" ] && [ "$srv_net" != "" ] && [ "$dgw_net" != "$srv_net" ]; then
	    $LOG "Add static routes for all VPN servers ip adresses by ip"
	    ipget "$vpnServer" | while read srvip; do
		$LOG "Add route to $srvip via $newdgw dev $wan_if"
		ip -4 route replace $srvip via $newdgw dev $wan_if
	    done
	    $LOG "Add route to $SERVER via $newdgw dev $wan_if"
	    ip -4 route replace $SERVER via $newdgw dev $wan_if
	fi
    fi
}

load_modules() {
    if [ ! -d /sys/module/pppol2tp ]; then
	mod="ppp_generic pppox pppol2tp"
	for module in $mod
	do
    	    modprobe -q $module
	done
    fi
}

    get_param
    check_param
    get_vpn_ip
    set_routest_to_server

    reachable=0

    if [ "$vpnTestReachable" = "1" ]; then
	while [ $reachable -eq 0 ]; do
	    $LOG "Check for L2TP server $SERVER reachable"
    	    ping -q -c 1 $SERVER
    	    if [ "$?" -eq 0 ]; then
        	reachable=1
    	    else
        	$LOG "Server unreachable wait 15 sec."
        	sleep 15
		get_vpn_ip
        	reachable=0
    	    fi
	done
    fi

    # load ppp* modules
    load_modules

    $LOG "==================START-L2TP-CLIENT======================="

    if [ "$vpnPeerDNS" = "on" ]; then
	vpnPeerDNS=usepeerdns
    else
	vpnPeerDNS=
    fi

    if [ "$vpnMPPE" = "on" ]; then
	mod="crypto_algapi cryptomgr blkcipher ppp_mppe"
	for module in $mod
	do
	    modprobe -q $module
	done
        vpnMPPE=allow-mppe-128
    else
        vpnMPPE=
    fi

    if [ "$vpnDebug" = "on" ]; then
        vpnDebug="debug"
    else
        vpnDebug=""
    fi

    if [ "$vpnMTU" = "" ] || [ "$vpnMTU" = "AUTO" ]; then
        vpnMTU=""
        vpnMRU=""
        else
        vpnMRU="mru $vpnMTU"
        vpnMTU="mtu $vpnMTU"
    fi

    if [ "$vpnAuthProtocol" = "1" ]; then
	L2TPPAP="require pap = yes"
	L2TPCHAP="require chap = no"
	PAP="require-pap"
	CHAP="refuse-chap"
    elif [ "$vpnAuthProtocol" = "2" ]; then
	L2TPPAP="require pap = no"
	L2TPCHAP="require chap = yes"
	PAP="refuse-pap"
	CHAP="require-chap"
    elif [ "$vpnAuthProtocol" = "3" ]; then
	L2TPPAP="require pap = no"
	L2TPCHAP="require chap = no"
	PAP="refuse-pap"
	CHAP="require-mschap-v2"
    else
	L2TPPAP=""
	L2TPCHAP=""
	PAP=""
	CHAP=""
    fi

    if [ "$vpnEnableLCP" = "on" ]; then
        vpnEnableLCP="lcp-echo-adaptive"
    else
	vpnEnableLCP=""
    fi

    if [ "$vpnLCPFailure" = "" ] || [ "$vpnLCPInterval" = "" ]; then
	vpnLCPFailure=5
	vpnLCPInterval=30
    fi

    if [ "$IPv6OpMode" = "1" ] && [ "$Ipv6InVPN" = "1" ]; then
        SIXEN="+ipv6 ipv6cp-use-ipaddr"
    else
	SIXEN=""
    fi

    printf "[global]
    access control = yes
    rand source = dev
    [lac $SERVER]
    redial = yes
    redial timeout = 20
    $L2TPPAP
    $L2TPCHAP
    require authentication = no
    lns = $SERVER
    name = $vpnUser
    autodial = yes
    tx bps = 100000000
    rx bps = 100000000
    tunnel rws = 8
    pppoptfile = $ppp/options.l2tp
    " > $ppp/l2tpd.conf

    printf "
    $vpnDebug
    $SIXEN
    $PAP
    $CHAP
    $vpnMTU
    $vpnMRU
    $vpnMPPE
    $vpnPeerDNS
    lcp-echo-failure  $vpnLCPFailure
    lcp-echo-interval $vpnLCPInterval
    $vpnEnableLCP
    ifname $vpn_def_if
    " > $ppp/options.l2tp

    $LOG "L2TP connect to $SERVER."
    FULLOPTS="-c $ppp/l2tpd.conf -s $ppp/chap-secrets -p $var/l2tpd.pid -l"
    xl2tpd $FULLOPTS &
