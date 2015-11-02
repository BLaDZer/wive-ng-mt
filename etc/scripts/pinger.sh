#!/bin/sh

eval `nvram_buf_get 2860 ping_check_time ping_check_interval RadioOn`

if [ "$ping_check_time" = "" ] || [ "$ping_check_time" = "0" ] || \
   [ "$ping_check_interval" = "" ] || [ "$ping_check_interval" = "0" ]; then
    exit 0
fi

# include global config
. /etc/scripts/global.sh

while "true"; do
    # wait before check
    sleep $ping_check_time

    ##################################RADIO################################################################################
    if [ "$RadioOn" = "1" ]; then
	# Unsolicited ARP mode, update your neighbors
	arping -U 255.255.255.255 -w1 -I"$lan_if" -b -c1 -q
	arping -A 255.255.255.255 -w1 -I"$lan_if" -b -c1 -q

	# arping for client wakeup - from arp table
	grep -v "IP" < /proc/net/arp | awk '{ print $1 }' | while read test_ip; do
	    arping "$test_ip" -I"$lan_if" -f -q -w1
	done

	sleep $ping_check_interval

	if [ -f /var/udhcpd.leases ]; then
	    size=`stat /var/udhcpd.leases | grep "Size" | awk {' print $2 '}`
	    if [ "$size" != "0" ]; then
		# arping for client wakeup - from dhcp lease table
		dumpleases | grep -v "IP" | awk '{ print $2 }' | while read test_ip; do
		    arping "$test_ip" -I"$lan_if" -f -q -w1
		done
	    fi
	fi
    fi
    ###################################UPLINK##############################################################################
    sleep $ping_check_interval
    grep -v "domain" < /etc/resolv.conf | awk {' print $2 '} | uniq | while read test_ip; do
        ping -4 -s8 -c1 -w30 -q "$test_ip" > /dev/null 2>&1
    done

    sleep $ping_check_interval
    route -n | grep -v "Dest" | grep -v "IP" | grep -v "0\.0\.0\.0" | awk {' print $2 '} | uniq | while read test_ip; do
        ping -4 -s8 -c1 -w30 -q "$test_ip" > /dev/null 2>&1
    done
    sleep $ping_check_interval
    ping -4 -s8 -c1 -w30 -q "8.8.8.8" > /dev/null 2>&1
done
