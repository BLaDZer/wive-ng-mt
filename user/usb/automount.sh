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
  modprobe -q "$MDEV_TYPE"
  # mount with default nls configured in kernel
  if ! mount -o noatime -t "$MDEV_TYPE" "$MDEV_PATH" "$MOUNT_DST"; then
    $LOG "can not mount $MDEV_TYPE $MDEV_PATH $MOUNT_DST"
    exit 1
  fi
  if [ "$MDEV_LABEL" == "optware" ]; then
    #re read profile variables
    . /etc/profile
  fi
}

try_ntfs() {
  if ! ntfs-3g "$MDEV_PATH" "$MOUNT_DST" -o force,noatime; then
    $LOG "can not mount NTFS $MDEV_PATH $MOUNT_DST"
    exit 1
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
    if [ "$MDEV_LABEL" != "optware" ] && [ "$MDEV_TYPE" != "swap" ]; then
	# restart only if not cold boot
	. /etc/scripts/web_wait.sh
	web_wait
	# restart HDD depended services
	sync
	service xupnpd restart
	service samba restart
	service transmission restart
    fi
}


if [ "$ACTION" = "add" ]; then
  # wait for disc appear, max 15 sec
  i=0
  while [ -z "$(ls /sys/class/scsi_disk/)" -a -z "$(ls /sys/bus/ide/devices/ 2>/dev/null)" ]; do
    sleep 1
    i=$((i + 1))
    if [ $i = "1" ]; then
	$LOG "Wait $MDEV for disc appear, max 15 sec"
    fi
    if [ $i -gt 15 ]; then
	break
    fi
  done

  # start prepare and mounts procedure
  eval "$(blkid $MDEV_PATH | sed 's/^[^ ]* //;s/\([^ ]*=\)/MDEV_\1/g')"
  $LOG "add $MDEV_PATH with $MDEV_TYPE"
  case "$MDEV_TYPE" in
    ntfs)
      pre_mount
      try_ntfs
      ;;
    swap)
      swap_on
      ;;
    *)
      pre_mount
      try_mount
      ;;
  esac
elif [ "$ACTION" = "mount" ]; then
  $LOG "device $MDEV_PATH mount OK"
  touchservices
  exit 0

elif [ "$ACTION" = "umount" ]; then
  if [ -d "/media/$MDEV" ]; then
    touchservices
    rmdir "/media/$MDEV"
  fi
  exit 0

else
  $LOG "remove $MDEV_PATH"
  try_umount
  swap_off
fi

exit 0
