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
	is_on=`grep -c "$MDEV" < /proc/swaps`
	if [ "$is_on" != "0" ]; then
	    $LOG "swap off dev $MDEV_PATH"
	    swapoff "$MDEV_PATH"
    	    sleep 3
	fi
    fi
}

umount_all
