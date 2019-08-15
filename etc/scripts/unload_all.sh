#!/bin/sh

# Description: Wive-NG Megakill script..

#############################
# do not stop syslog/klog !
#############################

stop_serv="hotplug bndstr radius chillispot transmission samba xupnpd dynroute crontab ddns udpxy miniupnpd \
	    igmp_proxy ntp snmpd parprouted irqbalance lld2d lldpd cdp iappd dnsserver inetd"

# kill dhcp servers directly to avoid unneded deconfig and flush conntrack
kill_apps="udhcpd dhcp6s radvd chilli_stat haveged butcheck"

term_sessions() {
    eval `nvram_buf_get 2860 RemoteManagement`

    if [ "$1" = "REBOOT" ] || [ "$RemoteManagement" != "2" ]; then
	# VPN force disconnect from server
	killall -q -SIGHUP pppd
	# wait send disconnetc
	usleep 500000
	# terminate pppd daemon
	while killall -q pppd; do
	    usleep 200000
	    killall -q -SIGKILL pppd
	done
	# terminate xl2tp daemon
	while killall -q xl2tpd; do
	    usleep 200000
	    killall -q -SIGKILL xl2tpd
	done
	# send release to DHCP server
	killall -q -SIGUSR2 udhcpc
	# kill dhcp6c
	killall -q -SIGKILL dhcp6c
    fi
}

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

# terminate pppoe and
term_sessions

# unload all applications
unload_apps

# disable hotplug helper
disable_hotplug

# umount all particions and disable swap
umount_all

# remove web pages from tmpfs and link to rootfs
remove_web
