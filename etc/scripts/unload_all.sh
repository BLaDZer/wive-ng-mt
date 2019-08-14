#!/bin/sh

# Description: Wive-NG Megakill script..

#############################
# do not stop syslog/klog !
#############################

stop_serv="hotplug bndstr radius chillispot transmission samba xupnpd dynroute crontab ddns udpxy miniupnpd \
	    igmp_proxy ntp snmpd parprouted irqbalance lld2d lldpd cdp iappd dnsserver inetd six dhcpd"

kill_apps="chilli_stat haveged butcheck"

eval `nvram_buf_get 2860 RemoteManagement`

if [ "$1" = "REBOOT" ] || [ "$RemoteManagement" != "2" ]; then
    # stop vpn and wan for correct terminate
    # pppoe session and DHCP address release
    stop_serv="$stop_serv vpnhelper wan"
fi

unload_apps() {
    echo "Stop services."
    # first stop adfilter (more and more ram usage)
    service dnsserver adstop
    for serv in $stop_serv
    do
	service $serv stop
    done

    # free caches
    sync

    echo "Kill aplications." # second step terminate and kill application if not correct stopped
    for app in $kill_apps
    do
	pid=`pidof $app`
	if [ "$pid" != "" ]; then
    	    while killall -q $app; do
        	usleep 10000
        	killall -q -SIGKILL $app
    	    done
	fi
    done
}

remove_web() {
    if [ -d /tmp/web ]; then
	echo "Remove web pages from tmpfs."
	rm -rf /tmp/web
	ln -sf /web /tmp/web
    fi
}

disable_hotplug() {
    if [ -e /proc/sys/kernel/hotplug ]; then
	echo "" > /proc/sys/kernel/hotplug
    fi
}

umount_all() {
    if [ -e /etc/scripts/umount_all.sh ]; then
	/etc/scripts/umount_all.sh
    fi
}

# disable hotplug helper
disable_hotplug

# unload all applications
unload_apps

# umount all particions and disable swap
umount_all

# remove web pages from tmpfs and link to rootfs
remove_web
