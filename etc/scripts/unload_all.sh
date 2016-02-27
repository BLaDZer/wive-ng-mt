#!/bin/sh

# Description: Wive-NG Megakill script..

stop_serv="chillispot transmission samba xupnp radvd dynroute shaper crontab ddns udpxy miniupnpd \
	    igmp_proxy ntp snmpd dnsserver parprouted inetd dhcpd irqbalance lld2d lldpd cdp iappd syslog watchdog"

kill_apps="chilli_stat chilli transmission-daemon smbd nmbd xupnpd udhcpd udhcpc dhcp6c dhcp6s radvd zebra ripd crond igmpproxy \
	    ntpd inadyn miniupnpd dnsmasq snmpd irqbalance inetd lld2d lldpd lldpcli cdp-send ralinkiappd syslogd klogd"

unload_modules() {
    echo "Unload modules"
    # unload all unused modules
    rmmod_mod=`lsmod | awk {' print $1'}`
    for mod in $rmmod_mod
    do
	if [ "$mod" != "rt2860v2_ap" -a "$mod" != "rt2860v2_sta" -a "$mod" != "mt7610_ap" -a "$mod" != "mt76x2_ap" -a "$mod" != "mt76x2_sta" -a "$mod" != "hw_nat" ]; then
    	    rmmod $mod > /dev/null 2>&1
	fi
    done
    rmmod -a
}

unload_apps() {
    echo "Stop services." # first step stop services
    for serv in $stop_serv
    do
	service $serv stop > /dev/null 2>&1
    done
    echo "Kill aplications." # second step terminate and kill application
    for apps in $kill_apps
    do
	(killall -q $apps && usleep 10000 && killall -q -SIGKILL $apps) > /dev/null 2>&1
    done
    # remove web pages from tmpfs and link to rootfs
    if [ -d /tmp/web ]; then
	echo "Remove web pages from tmpfs."
	rm -rf /tmp/web
	ln -sf /web /tmp/web
    fi
}

free_mem_cahce() {
    # small workaround for defrag and clean mem
    sysctl -wq vm.min_free_kbytes=4096
    sync
    sysctl -wq vm.min_free_kbytes=2048
    sync
}

disable_hotplug() {
    if [ -f /proc/sys/kernel/hotplug ]; then
	echo "" > /proc/sys/kernel/hotplug
    fi
}

umount_all() {
    if [ -f /etc/scripts/umount_all.sh ]; then
	/etc/scripts/umount_all.sh
    fi
}

# disable hotplug helper
disable_hotplug

# unload all applications
unload_apps

# umount all particions and disable swap
umount_all

# unload all modules this is need after unmont
unload_modules

# this drop unneded caches to free more ram.
free_mem_cahce
