#!/bin/sh

#############################################################
# config-pptp.sh - configure PPTP client helper for Wive-NG #
#############################################################

# include global
. /etc/scripts/global.sh

LOG="logger -t vpnhelper-pptp"

get_param() {
    eval `nvram_buf_get 2860 vpnServer vpnUser vpnPassword vpnMTU vpnMPPE vpnPeerDNS vpnDebug vpnAuthProtocol vpnEnableLCP vpnLCPFailure vpnLCPInterval vpnTestReachable`
    OPTFILE="/etc/ppp/options.pptp"
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
	    NS=`ipget $vpnServer | tail -qn1`
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
    if [ "$firstgw" != "" ]; then
	ipget "$vpnServer" | while read srvip; do
	    srv_net=`ipcalc "$srvip" -sn | cut -f 2- -d =`
	    if [ "$srv_net" != "" ]; then
	        # test vpn server in local net?
		vpnnotlocal=`ip -4 route get "$srvip" | grep "via" -c` > /dev/null 2>&1
		if [ "$srvip" != "$firstgw" ] && [ "$vpnnotlocal" != "0" ]; then
		    $LOG "Add static route to VPN server $srvip via $firstgw dev $wan_if"
		    ip -4 route replace $srvip via $firstgw dev $wan_if > /dev/null 2>&1
		fi
	    fi
	done
	srv_net=`ipcalc "$SERVER" -sn | cut -f 2- -d =`
	if [ "$srv_net" != "" ]; then
	    # test vpn server in local net?
	    vpnnotlocal=`ip -4 route get "$SERVER" | grep "via" -c` > /dev/null 2>&1
	    if [ "$SERVER" != "$firstgw" ] && [ "$vpnnotlocal" != "0" ]; then
		$LOG "Add static route to current selected VPN server $SERVER via $firstgw dev $wan_if"
		ip -4 route replace $SERVER via $firstgw dev $wan_if > /dev/null 2>&1
	    fi
	fi
    fi
}

load_modules() {
    if [ ! -d /sys/module/pptp ]; then
	mod="ppp_generic pppox pptp"
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
	    $LOG "Check for PPTP server $SERVER reachable"
    	    ping -q -c 1 $SERVER
    	    if [ "$?" -eq 0 ]; then
        	reachable=1
    	    else
    		$LOG "Server unreachable wait 15 sec."
        	sleep 15
		get_vpn_ip
		set_routest_to_server
        	reachable=0
    	    fi
        done
    fi

    # load ppp* modules
    load_modules

    $LOG "==================START-PPTP-CLIENT======================="

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

    if [ "$vpnMTU" = "" ] || [ "$vpnMTU" = "AUTO" ]; then
        vpnMTU=""
        vpnMRU=""
    else
        vpnMRU="mru $vpnMTU"
        vpnMTU="mtu $vpnMTU"
    fi

    if [ "$vpnDebug" = "on" ]; then
        vpnDebug="debug"
    else
	vpnDebug=""
    fi

    if [ "$vpnAuthProtocol" = "1" ]; then
	PAP="require-pap"
	CHAP="refuse-chap"
    elif [ "$vpnAuthProtocol" = "2" ]; then
	PAP="refuse-pap"
	CHAP="require-chap"
    elif [ "$vpnAuthProtocol" = "3" ]; then
	PAP="refuse-pap"
	CHAP="require-mschap-v2"
    else
	PAP=""
	CHAP=""
    fi

    if [ "$vpnEnableLCP" = "on" ]; then
        vpnEnableLCP="lcp-echo-adaptive"
    else
	vpnEnableLCP=""
    fi

    if [ "$vpnLCPFailure" = "" ] || [ "$vpnLCPInterval" = "" ]; then
	vpnLCPFailure=10
	vpnLCPInterval=25
    fi

    if [ "$IPv6OpMode" = "1" ] && [ "$Ipv6InVPN" = "1" ]; then
	SIXEN="+ipv6 ipv6cp-use-ipaddr"
    else
	SIXEN=""
    fi

    printf "
    lcp-echo-failure  $vpnLCPFailure
    lcp-echo-interval $vpnLCPInterval
    $vpnEnableLCP
    $PAP
    $CHAP
    $SIXEN
    " > $OPTFILE

    $LOG "PPTP connect to $SERVER."
    PPPDOPT="file $OPTFILE ifname $vpn_def_if -detach noipdefault noauth persist $vpnPeerDNS $vpnDebug $vpnMTU $vpnMRU $vpnMPPE plugin"
    PLUGOPT="/lib/libpptp.so pptp_server $SERVER user $vpnUser password $vpnPassword"
    FULLOPT="$PPPDOPT $PLUGOPT"
    pppd $FULLOPT &
