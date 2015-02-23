#!/bin/sh

# Wive-NG  udhcpc script

# include global config
. /etc/scripts/global.sh

[ -z "$1" ] && echo "Error: should be called from udhcpc" && exit 1

LOG="logger -t udhcpc"
RESOLV_CONF="/etc/resolv.conf"
WINS_CONF="/tmp/wins.dhcp"

# All route list
ROUTELIST=""
# Static route list
ROUTELIST_ST=""
# Static classful route list
ROUTELIST_CS=""
# Default gateway list
ROUTELIST_DGW=""
# Stub for first hop`s dgw
ROUTELIST_FGW=""

# Get MTU config and VPN DGW mode
eval `nvram_buf_get 2860 vpnDGW vpnPeerDNS dhcpSwReset`

# Renew flags
FULL_RENEW=1
REPLACE_DGW=1
REPLACE_DNS=1

# If mode = pppoe, dgw need in pppoe, dns get from pppoe and pppoe is up - no need replace default gw and dns at lease renew
if [ "$vpnEnabled" = "on" ] && [ "$vpnType" = "0" ] && [ -f /tmp/vpn_if_name ]; then
    if  [ "$vpnDGW" = "1" ]; then
	REPLACE_DGW=0
    fi
    if [ "$vpnPeerDNS" = "on" ]; then
	REPLACE_DNS=0
    fi
fi

#############################################################################################################################

if [ "$broadcast" != "" ]; then
    BROADCAST="broadcast $broadcast"
fi

if [ "$subnet" != "" ]; then
    NETMASK="netmask $subnet"
fi

case "$1" in
    deconfig)
	$LOG "All deconfig."
	if [ -d /proc/sys/net/ipv6 ] && [ "$IPv6OpMode" = "2" -o "$IPv6OpMode" = "3" ]; then
	    $LOG "all tunnels in ipv6 deconfig before modules reload (prevent race)"
	    service six stop
	fi
	service vpnhelper stop_safe
	# disable forward for paranoid users
	sysctl -wq net.ipv4.conf.all.forwarding=0
	# generate random ip from zeroconfig range end set
	# this is hack for some ISPs checked client alive by arping
	# and prevent fake unset FULL_RENEW flag at next time bound
	rndip="169.254.$(($RANDOM%253+1)).$(($RANDOM%253+1))"
	ip -4 addr flush dev $interface
	ip -4 addr add $rndip/32 dev $interface
	# never need down iface
	ip link set $interface up
	rm -f $WINS_CONF
    ;;

    leasefail)
	$LOG "Lease fail."
	if [ -d /proc/sys/net/ipv6 ] && [ "$IPv6OpMode" = "2" -o "$IPv6OpMode" = "3" ]; then
	    $LOG "all tunnels in ipv6 deconfig before modules reload (prevent race)"
	    service six stop
	fi
	service vpnhelper stop_safe
	# disable forward for paranoid users
	sysctl -wq net.ipv4.conf.all.forwarding=0
	# Workaround for infinite OFFER wait
	if [ "$OperationMode" != "2" ] && [ "$dhcpSwReset" = "1" ]; then
	    if [ "$CONFIG_RAETH_ESW" != "" ] || [ "$CONFIG_MT7530_GSW" != "" ]; then
		$LOG "Restart WAN switch port."
		config-vlan.sh $switchmode WWWWW
	    fi
	elif [ "$OperationMode" = "2" ] || [ "$OperationMode" = "3" ]; then
	    # Try reconnect at lease failed
	    $LOG "Reconnect to AP if need."
	    wifi_reconnect
	fi
	rm -f $WINS_CONF
    ;;

    renew|bound)
	NEW_IP="$ip"
	OLD_IP=`ip -4 addr show dev $interface | awk '/inet / {print $2}' | cut -f1 -d"/"` > /dev/null 2>&1
	# set full renew flag
	if [ "$NEW_IP" = "$OLD_IP" ]; then
	    FULL_RENEW=0
	fi

    ########################################################################################################
    # IP/NETMASK/MTU
    ########################################################################################################

	# check ip/netmask
	if [ "$NEW_IP" = "" ] || [ "$NETMASK" = "" ]; then
	    $LOG "ERROR: DHCP not send IP/MASK.... Call you provider support!!!"
	    exit 1
	fi

	# check subnets
	lan_net=`ipcalc "$lan_ipaddr" "$lan_netmask" -ns | cut -f 2- -d =`
	wan_net=`ipcalc "$NEW_IP" "$NETMASK" -ns | cut -f 2- -d =`
	if [ "$NEW_IP" = "$lan_ipaddr" ] || [ "$lan_net" = "$wan_net" ]; then
	    $LOG "ERROR: WAN ip in lan subnet. Need change LAN_IP address!!!"
	fi

	# if dgw iface changed need fullrenew procedure
	DGW_IF_CHANGED=`ip -4 route show default | grep -v "dev $interface " | sed 's,.*dev \([^ ]*\) .*,\1,g''`
	if [ "$DGW_IF_CHANGED" =! "" ]; then
	    FULL_RENEW=1
	fi

	# set ip/netmask
	if [ "$FULL_RENEW" = "1" ]; then
	    $LOG "Renew ip adress $NEW_IP and $NETMASK for $interface from dhcp"
	    ifconfig $interface $NEW_IP $BROADCAST $NETMASK
	    # MTU is default for all session time.
	    # Get MTU from dhcp server and set
	    if [ "$mtu" = "576" ]; then
		$LOG "MTU size recived from DHCP server is incorrect (ugly ISP? QWERTY?), use 1500bytes instead"
		mtu=1500
	    fi
	    if [ "$mtu" ] && [ "$wan_manual_mtu" = "0" -o "$wan_manual_mtu" = "" ]; then
		$LOG "Set MTU to $mtu bytes from dhcp server"
		ip link set mtu $mtu dev $interface
	    fi
	fi

    ########################################################################################################
    # DGW/MSROUTES/STATICROUTES/CLASSFULLROUTES/USERROUTES
    ########################################################################################################
        #
	# update routes if first exec script or new ip selected
	# default route with metric 0 is through $iface?
	#
	if [ "$FULL_RENEW" = "1" ] || [ ! -f /tmp/routes_applied ]; then
	    # Get default gateway
	    if [ "$router" != "" ]; then
		# if ip not changed not need delete old default route
		# this is workaroud for ppp used tunnels up over not default routes
		if [ "$FULL_RENEW" = "1" ] && [ "$REPLACE_DGW" = "1" ]; then
		    $LOG "Deleting default route dev $interface"
		    while ip -4 route del default dev $interface ; do
		        :
		    done
		fi
		# always parse router variable
		metric=0
		for raddr in $router ; do
		    # add route $raddr/32:0.0.0.0 dev $interface metric $metric to route list
		    ROUTELIST_FGW="$ROUTELIST_FGW $raddr/32:0.0.0.0:$interface:"
		    if [ "$REPLACE_DGW" = "1" ]; then
		        # add default $raddr dev $interface metric $metric to route dgw list
		        ROUTELIST_DGW="$ROUTELIST_DGW default:$raddr:$interface:$metric"
		        # save first dgw with metric=0 to use in corbina hack
		        if [ "$metric" = "0" ]; then
			    echo $raddr > /tmp/default.gw
			    first_dgw="$raddr"
			fi
			# increase metric only for dgw
			: $(( metric += 1 ))
		    fi
		done
	    fi
	    # classful routes
	    if [ "$routes" != "" ]; then
		for raddr in $routes; do
		    NW=`echo $raddr | sed 's,/.*,,'`
		    GW=`echo $raddr | sed 's,.*/,,'`
		    shift 1
		    MASK=`echo $NW | awk '{w=32; split($0,a,"."); \
			for (i=4; i>0; i--) {if (a[i]==0) w-=8; else {\
			while (a[i]%2==0) {w--; a[i]=a[i]/2}; break}\
			}; print w }'`
		    if [ "$GW" = "0.0.0.0" ] || [ -z "$GW" ]; then
			ip -4 route replace $NW/$MASK dev $interface
		    else
			ROUTELIST_CS="$ROUTELIST_CS $NW/$MASK:$GW:$interface:"
		    fi
		done
	    fi
	    # MSSTATIC ROUTES AND STATIC ROUTES (rfc3442)
	    STATICROUTES="$staticroutes $msstaticroutes"
	    if [ "$STATICROUTES" != " " ]; then
		set $STATICROUTES
		while [ -n "$1" ]; do
		    NW="$1"
		    GW="$2"
		    shift 2
		    if [ "$GW" = "0.0.0.0" ] || [ -z "$GW" ]; then
			ip -4 route replace $NW dev $interface
		    else
			ROUTELIST_ST="$ROUTELIST_ST $NW:$GW:$interface:"
		    fi
		done
	    fi
	    # first add stub for routes next add static routes and
	    # default gateways need replace/add at end route parces
	    # replace dgw must be replaced only if ip selected
	    if [ "$REPLACE_DGW" = "1" ] && [ "$FULL_RENEW" = "1" ]; then
		ROUTELIST="$ROUTELIST_FGW $ROUTELIST_DGW $ROUTELIST_ST $ROUTELIST_CS"
		$LOG "Apply route list. And replace DGW."
	    else
		ROUTELIST="$ROUTELIST_FGW $ROUTELIST_ST $ROUTELIST_CS"
		$LOG "Apply route list without modify DGW."
	    fi
	    # aplly parsed route
	    for raddr in `echo $ROUTELIST | sed 's/ /\n/g'`; do
		IPCMD=`echo $raddr | awk '{split($0,a,":"); \
		    printf " %s via %s dev %s", a[1], a[2], a[3]; \
		    if (a[4]!="") printf " metric %s", a[4]}'`
		ip -4 route replace $IPCMD
		$LOG "ip route replace $IPCMD"
	    done
	    # workaround for some buggy ISP
	    if [ "$REPLACE_DGW" = "1" ] && [ "$FULL_RENEW" = "1" ] && [ "$first_dgw" != "" ]; then
		$LOG "Set fist default gateway to $first_dgw dev $interface metric 0"
		ip -4 route replace default dev "$interface" via "$first_dgw" metric 0
	    fi
	    # add route to multicast subnet
	    if [ "$igmpEnabled" = "1" -o "$UDPXYMode" != "0" ]; then
		$LOG "Add route to multicast subnet."
		ip -4 route replace "$mcast_net" dev "$interface"
	    fi
	    # add routes configured in web
	    if [ -f /etc/routes_replace ]; then
		$LOG "Apply user routes."
		/etc/routes_replace replace $lan_if $interface
	    fi
	    # set routes applied flag
	    touch /tmp/routes_applied
	fi

    ########################################################################################################
    # DNS/WINS and services
    ########################################################################################################
        if [ "$FULL_RENEW" = "1" ]; then
	    # get DNS servers
	    if [ "$wan_static_dns" = "on" ]; then
		$LOG "Use static DNS."
		# resolv.conf allready generated in internet.sh
	    else
		if [ "$REPLACE_DNS" = "1" ]; then
		    rm -f $RESOLV_CONF
        	    # set domain name from DHCP/NVRAM
    		    if [ "$domain" != "" ]; then
			echo "domain $domain" >> $RESOLV_CONF
		    else
			echo "domain $HostName.lo" >> $RESOLV_CONF
		    fi
		    if [ "$dns" != "" ]; then
			$LOG "Renew DNS from dhcp $dns $domain"
			# parce dnsservers
			for daddr in $dns ; do
	    		    echo nameserver $daddr >> $RESOLV_CONF
			    ROUTE_NS=`ip -4 -o route get "$daddr" | cut -f -3 -d " "`
			    if [ "$ROUTE_NS" != "" ] && [ "$daddr" != "$first_dgw" ]; then
				$LOG "Add static route to DNS $ROUTE_NS dev $interface"
				REPLACE="ip route replace $ROUTE_NS dev $interface"
				$REPLACE
			    fi
			done
		    else
			$LOG "Server not send DNS. Please manual set DNS in wan config. Temp use google dns."
	    		echo "nameserver 8.8.8.8" >> $RESOLV_CONF
	    		echo "nameserver 8.8.4.4" >> $RESOLV_CONF
		    fi
		    if [ "$IPv6OpMode" != "0" ]; then
			if [ -f /etc/resolvipv6.conf ]; then
			    $LOG "Add ISP ipv6 servers for local resolv"
			    cat /etc/resolvipv6.conf | grep "nameserver" >> $RESOLV_CONF
			else
			    $LOG "Add google ipv6 servers for local resolv"
			    echo "nameserver 2001:4860:4860::8888" >> $RESOLV_CONF
			    echo "nameserver 2001:4860:4860::8844" >> $RESOLV_CONF
			fi
		    fi
		    # read for all write by root
		    chmod 644 "$RESOLV_CONF" > /dev/null 2>&1
		fi
	    fi
	    # get wins servers
	    if [ "$wins" != "" ]; then
		echo "$wins" > $WINS_CONF
		chmod 644 "$WINS_CONF" > /dev/null 2>&1
	    fi
	fi

    ########################################################################################################
    # VPN Logic and workarounds
    ########################################################################################################
	# if dhcp disables restart must from internet.sh
	# this is restart vpn and others if need
    	if [ "$FULL_RENEW" = "1" ]; then
	    # restart some external services depended by wan ip
	    $LOG "Restart needed services"
	    services_restart.sh dhcp
	    # restart vpn if ip changed and vpn != pppoe
	    # or vpn enabled and not started
	    if [ "$vpnEnabled" = "on" ] && [ "$vpnType" != "0" -o ! -f /var/run/$vpn_if.pid ]; then
		$LOG "Restart vpnhelper.."
		service vpnhelper restart
	    fi
	    $LOG "End renew procedure..."
	fi
	# reenable forward for paranoid users
	sysctl -wq net.ipv4.conf.all.forwarding=1
    ;;
esac
