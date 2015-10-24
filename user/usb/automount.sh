#!/bin/sh

##########################################################################################################
#
# Auto mount helper (Wive-NG-Project)
#
##########################################################################################################

# include global config
. /etc/scripts/global.sh

LOG="logger -t automount"
MDEV_PATH=/dev/$MDEV

check_media() {
  is_mounted=`mount | grep -c "media"`
  if [ "$is_mounted" != "0" ]; then
    $LOG "/media is binding to rw"
  else
    $LOG "prepare /media"
    mount -t tmpfs -o size=4K tmpfs /media
  fi
}

pre_mount() {
  if [ "$MDEV_LABEL" == "optware" ]; then
    $LOG "detect optware part on $MDEV"
    MOUNT_DST="/opt"
  else
    check_media
    MOUNT_DST="/media/$MDEV"
    if [ -d "$MOUNT_DST" ]; then
      $LOG "dir $MOUNT_DST is exist"
      exit 1
    else
      if ! mkdir -p "$MOUNT_DST"; then
	$LOG "can not create $MOUNT_DST"
	exit 1
      fi
    fi
  fi
}

try_mount() {
  if [ "$MDEV_TYPE" != "ntfs" ]; then
    modprobe -q "$MDEV_TYPE"
  fi
  # mount with default nls configured in kernel
  if ! $MOUNT_CMD -o "$MOUNT_OPT" "$MDEV_PATH" "$MOUNT_DST"; then
    $LOG "can not mount $MDEV_TYPE $MDEV_PATH $MOUNT_DST"
    exit 1
  fi
  if [ "$MDEV_LABEL" == "optware" ]; then
    #re read profile variables
    . /etc/profile
  fi
}

swap_on() {
  if [ -f /proc/swaps ] && [ -f /bin/swapon ]; then
    is_on=`grep -c "$MDEV" < /proc/swaps`
    if [ "$is_on" = "0" ]; then
	$LOG "Swap on dev $MDEV_PATH"
	swapon "$MDEV_PATH" > /dev/null 2>&1
    fi
  fi
}

try_umount() {
  MOUNT_DST=`mount | grep "$MDEV" | awk '{print $3}'`
  if [ "$MOUNT_DST" ]; then
    $LOG "umount $MOUNT_DST"
    if ! umount "$MOUNT_DST"; then
      sleep 3
      if ! umount -fl "$MOUNT_DST"; then
	$LOG "can not unmount $MOUNT_DST"
	exit 1
      fi
    fi
  fi
}

swap_off() {
  if [ -f /proc/swaps ] && [ -f /bin/swapoff ]; then
    is_on=`grep -c "$MDEV" < /proc/swaps`
    if [ "$is_on" != "0" ]; then
	$LOG "swap off dev $MDEV_PATH"
	swapoff "$MDEV_PATH"
        sleep 3
    fi
  fi
}

touchservices() {
    # restart only if not cold boot
    sync
    if [ -f /tmp/bootgood ] && [ "$MDEV_LABEL" != "optware" ] && [ "$MDEV_TYPE" != "swap" ]; then
	# restart HDD depended services
	service xupnpd restart
	service samba restart
	service transmission restart
    fi
}


if [ "$ACTION" = "add" ]; then
  # only for usb disks need wait (prevent mmc mount freez)
  isdisk=`echo "$MDEV_PATH" | grep "sd" -c`
  if [ $isdisk != "0" ]; then
    i=0
    # wait for disc appear, max 15 sec
    while [ -z "$(ls /sys/class/scsi_disk/)" ]; do
	sleep 1
	i=$((i + 1))
	if [ $i = "1" ]; then
	    $LOG "Wait $MDEV for disc appear, max 10 sec"
	fi
	if [ $i -gt 10 ]; then
	    break
	fi
    done
  fi

  # start prepare and mounts procedure
  eval "$(blkid $MDEV_PATH | sed 's/^[^ ]* //;s/\([^ ]*=\)/MDEV_\1/g')"
  if [ -z "$MDEV_TYPE" ]; then
    $LOG "unknow fs type on $MDEV_PATH"
    exit 1
  fi
  $LOG "add $MDEV_PATH with $MDEV_TYPE"
  MOUNT_CMD="mount -t $MDEV_TYPE"
  MOUNT_OPT="noatime"
  case "$MDEV_TYPE" in
    swap)
      swap_on
      ;;
    *fat)
      MOUNT_OPT="noatime,umask=0"
      pre_mount
      try_mount
      ;;
    ntfs)
      MOUNT_CMD="ntfs-3g"
      MOUNT_OPT="force,noatime,umask=0"
      pre_mount
      try_mount
      ;;
    *)
      pre_mount
      try_mount
      ;;
  esac

elif [ "$ACTION" = "mount" ]; then
  $LOG "device $MDEV_PATH mount OK"
  touchservices

elif [ "$ACTION" = "umount" ]; then
  if [ -d "/media/$MDEV" ]; then
    touchservices
    rmdir "/media/$MDEV"
  fi

else
  $LOG "remove $MDEV_PATH"
  try_umount
  swap_off
fi

exit 0
