#!/bin/sh

# Description: Megakill script..
# Try some methods for free ram before firmware update.
# Workaround over workaround. Need clean in the future.
# In <=32Mb devices also reconfigure network and unload wifi modules for memsave.

# include global
. /etc/scripts/global.sh

stop_serv="chillispot transmission samba xupnp radvd dynroute vpnserver vpnhelper shaper crontab ddns udpxy miniupnpd \
	    igmp_proxy ntp snmpd dnsserver parprouted inetd dhcpd irqbalance lld2d lldpd cdp iappd syslog watchdog"

kill_apps="chilli transmission-daemon smbd nmbd xupnpd udhcpd udhcpc dhcp6c dhcp6s radvd zebra ripd crond igmpproxy \
	    ntpd inadyn miniupnpd dnsmasq snmpd irqbalance inetd lld2d lldpd lldpcli cdp-send ralinkiappd syslogd klogd"

rmmod_mod="imq ipt_account ipt_TTL ipt_IMQ ipt_tos \
	    ipt_REDIRECT ipt_ttl ipt_TOS xt_string xt_webstr xt_connmark xt_CONNMARK xt_conntrack \
	    act_gact act_police cls_tcindex em_cmp em_u32 sch_gred sch_red act_ipt cls_fw cls_u32 \
	    nf_nat_ftp nf_nat_h323 nf_nat_pptp nf_nat_proto_gre nf_nat_sip nf_nat_rtsp \
	    nf_conntrack_ftp nf_conntrack_proto_gre nf_conntrack_h323 nf_conntrack_sip nf_conntrack_pptp nf_conntrack_rtsp \
	    em_nbyte sch_esfq sch_htb sch_sfq ts_fsm ts_kmp ts_bm \
	    usblp usb-storage usbserial hso ext2 ext3 exfat cifs"

unload_modules() {
    echo "Unload modules"
    # unload modules all unused
    rmmod -a
    # all others
    for mod in $rmmod_mod
    do
        rmmod $mod > /dev/null 2>&1
    done
    # unload full
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
	(killall -q $apps && usleep 20000 && killall -q -SIGKILL $apps) > /dev/null 2>&1
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

# disable hotplug
if [ -f /proc/sys/kernel/hotplug ]; then
    echo "" > /proc/sys/kernel/hotplug
fi

# unload all applications
unload_apps

# umount all particions and disable swap
if [ -f /etc/scripts/umount_all.sh ]; then
    /etc/scripts/umount_all.sh
fi

# unload all modules this is need after unmont
unload_modules

# disable forward
sysctl -wq net.ipv4.ip_forward=0
sysctl -wq net.ipv4.conf.all.forwarding=0
sysctl -wq net.ipv4.conf.default.forwarding=0
sysctl -wq net.ipv6.conf.all.forwarding=0
sysctl -wq net.ipv6.conf.default.forwarding=0

# clear conntrack and routes tables/caches
flush_net_caches

# This drop unneded caches to free more ram.
free_mem_cahce
