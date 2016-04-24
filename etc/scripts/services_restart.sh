#!/bin/sh

##########################################################
# This scipt restart needed services                     #
#							 #
# MODES:						 #
# all -  call from internel.sh - full reconfigure	 #
# dhcp - call from dhcp script after full renew		 #
# pppd - call from ip-up/down scripts			 #
# misc - call from goahead by apply change in misc page	 #
##########################################################


# prevent double start configure in one time
while [ -e /tmp/servicerestart_runing ]; do
    # Sleep until file does exists/is created
    usleep 500000
done
touch /tmp/servicerestart_runing

# include global config
. /etc/scripts/global.sh

LOG="logger -t services"
MODE="$1"

$LOG "Restart needed services and scripts. Mode $MODE"

##########################################################
# Always reload shaper and netfilter rules		 #
##########################################################
    service shaper restart
    service iptables restart

##########################################################
# full reconfigure ipv6 if settings apply or first start
# reconfigure ipv6 tunnels after wan/vpn adress renew
# and reconfigure and start radvd/dhcp6s at apply in misc
##########################################################
if [ "$MODE" = "all" ]; then
    service six restart
elif [ "$IPv6OpMode" = "2" -o "$IPv6OpMode" = "3" ]  && [ "$MODE" = "dhcp" -o "$MODE" = "pppd" ]; then
    service six restart
elif [ "$MODE" = "misc" ]; then
    service six dhcpradvdreconf
fi

##########################################################
# Regenerate resolv only if wan_static_dns on
##########################################################
if [ "$wan_static_dns" = "on" ]; then
    service resolv restart
fi

##########################################################
# Always reload some services				 #
##########################################################
if [ -e /etc/init.d/parprouted ]; then
    service parprouted restart
fi
if [ -e /etc/init.d/dynroute ]; then
    service dynroute restart
fi
service dnsserver reload
service ddns restart
service ntp restart
service miniupnpd restart

##########################################################
# restart nodogsplash/chillispot hotspot daemon		 #
# need always restart - netfilter reconfigure by daemon  #
##########################################################
if [ -e /bin/nodogsplash ]; then
    service nodogsplash restart
fi
if [ -e /bin/chilli ]; then
    service chillispot restart
fi

##########################################################
# Need restart this:					 #
# 1) if physycal wan ip adress change			 #
# 2) if apply changes from web				 #
# 3) if full reconfigure				 #
##########################################################
if [ "$MODE" = "dhcp" ] || [ "$MODE" = "misc" ] || [ "$MODE" = "all" ]; then
    if [ -e /etc/init.d/igmp_proxy ]; then
        service igmp_proxy restart
    fi
    if [ -e /etc/init.d/xupnpd ]; then
	service xupnpd restart
    fi
    if [ -e /etc/init.d/udpxy ]; then
	service udpxy restart
    fi
fi

##########################################################
# Need restart this:					 #
# 1) if misc chane apply				 #
# 2) full reconfigure					 #
##########################################################
if [ "$MODE" = "misc" ] || [ "$MODE" = "all" ]; then
    service kext restart
    if [ -e /etc/init.d/lld2d ]; then
	service lld2d restart
    fi
    if [ -e /etc/init.d/snmpd ]; then
        service snmpd restart
    fi
    if [ -e /etc/init.d/inetd ]; then
        service inetd restart
    fi
    if [ -e /etc/init.d/transmission ]; then
	service transmission restart
    fi
fi

##########################################################
# Reconfigure and restart samba:			 #
# 1) if recive wins server adress from dhcp		 #
##########################################################
if [ "$MODE" = "dhcp" ] && [ -e /tmp/wins.dhcp ]; then
    service samba restart
fi

# renew /etc/udhcpd.conf and restart dhcp server
if [ "$dnsPEnabled" = "1" -o "$wan_static_dns" = "on" ] && [ "$MODE" = "misc" -o "$MODE" = "all" ]; then
    # if dnsmasq or static dns enabled and aplly at web or full reconfigure
    service dhcpd restart
elif [ "$dnsPEnabled" != "1" -a "$wan_static_dns" != "on" ] && [ "$MODE" = "pppd" -o "$MODE" = "dhcp" ]; then
    # if dnsmasq or static dns disabled and mode=pppd/dhcp (renew/reconnect ISP)
    sleep 3
    service dhcpd restart
fi

##########################################################
# Always send CDP/SONMP/FDP/EDP to ISP			 #
##########################################################
if [ -e /bin/cdp-send ]; then
	service cdp restart
fi
if [ -e /bin/lldpd ]; then
	service lldpd restart
fi

##########################################################
# restart 802.1x wireless daemon			 #
##########################################################
if [ -e /bin/rt2860apd ]; then
	service apd restart
fi

##########################################################
# restart bandsteering wireless daemon			 #
##########################################################
if [ -e /bin/bndstrg ]; then
	service bndstr restart
fi

##########################################################
# restart 802.1f wireless daemon			 #
##########################################################
if [ -e /bin/ralinkiappd ]; then
	service iappd restart
fi

##########################################################
# restart radius auth daemon				 #
##########################################################
if [ -e /bin/radiusd ]; then
	service radius restart
fi

##########################################################
# After physical reinit 				 #
##########################################################
if [ "$MODE" = "all" ]; then
    ##########################################################
    # This is hook for exec user script after connection     #
    # configured. May be used for add scripts needed by some #
    # provides.                                              #
    # Example: http://wive-ng.sf.net/downloads/wan_up.sh     #
    # This script load external routes for www.kvidex.ru ISP #
    ##########################################################
    if [ -e /etc/scripts/wan_up.sh ]; then
	$LOG "Call user /etc/scripts/wan_up.sh script."
	sh /etc/scripts/wan_up.sh
    fi
    ##########################################################
    # in dhcp client mode restart from dhcp script           #
    # in static/zeroconf/pure pppoe mode need restart anyway #
    ##########################################################
    if [ "$wanConnectionMode" != "DHCP" ] || [ "$purepppoemode" = "1" ]; then
	service vpnhelper restart
    fi
fi

##########################################################
# need restart L2TP server every netfilter rules replaced
# or ppp session to uplink restarted or new adress recived
##########################################################
service vpnserver restart

##########################################################
# last stage after apply all changes restart cwmp client
##########################################################
if [ -e /bin/cwmpd ]; then
    service cwmpd restart
fi

##########################################################
# Always rebalance irq by cpus				 #
##########################################################
if [ -e /bin/irqbalance ]; then
    service irqbalance restart
fi

# remove running flag
rm -f /tmp/servicerestart_runing
