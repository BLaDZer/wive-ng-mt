#!/bin/sh

##########################################################################################################
#
# Auto umount all particions and disable swap (Wive-NG-Project)
#
##########################################################################################################

umount_all() {
    # umount all exclude base system fs
    mounted=`mount | grep -vE "tmpfs|ramfs|squashfs|proc|sysfs|root|pts" | cut -f1 -d" " | cut -f3 -d "/"`
    if [ -n "$mounted" ]; then
	for disk in $mounted; do
	    echo "Umount external drive /dev/$disk."
	    (sync && umount -fl /dev/$disk && sync) &
	done
	sleep 2
    fi

    # disable swaps
    if [ -f /proc/swaps ] && [ -f /bin/swapoff ]; then
	swapparts=`cat /proc/swaps | grep dev | awk {' print $1 '}`
	for disk in $swapparts; do
	    $LOG "swap off dev $disk"
	    swapoff "$disk"
    	    sleep 2
	done
	swapoff -a
    fi
}

umount_all
