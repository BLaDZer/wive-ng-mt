#!/bin/sh

# Ralink SDK deprecated script for support storage webui
# Need rewrite this webui and remove this script

LOG="logger -t storage_helper"

case $1 in
	"remove")
		$LOG "Umount all disks and swap."
		DiskApps="transmission samba xupnpd"
		for app in $DiskApps; do
		    service "$app" stop
		done
		umount_all.sh
		$LOG "Umount complite. services restart (exclude transmission)."
		DiskApps="samba xupnpd"
		for app in $DiskApps; do
		    service "$app" restart
		done
		;;
	"adddir")
		if [ -n "$2" ]; then
			mkdir -p "$2"
			chmod 777 "$2"
		$LOG "Add dir complete."
		fi
		;;
	"deldir")
		if [ -n "$2" ]; then
			rm -rf "$2"
		$LOG "Rem dir complete."
		fi
		;;
	"reparted")
		umount -l $2
		fdisk -D /dev/sda
		echo "fdisk -D /dev/sda"
		sleep 1
		if [ "$2" -gt "0" ]; then
			fdisk /dev/sda -p 1 -v $2
			echo "fdisk /dev/sda -p 1 -v $2 "
		fi
		sleep 1
		if [ "$3" -gt "0" ]; then
			fdisk /dev/sda -p 2 -v $3
			echo "fdisk /dev/sda -p 2 -v $3"
		fi
		sleep 1
		if [ "$4" -gt "0" ]; then
			fdisk /dev/sda -p 3 -v $4
			echo "fdisk /dev/sda -p 3 -v $4"
		fi
		sleep 1
		if [ "$5" -gt "0" ]; then
			fdisk /dev/sda -p 4 -v $5
			echo "fdisk /dev/sda -p 4 -v $5"
		fi
		sleep 1
		fdisk -r /dev/sda
		echo "fdisk -r /dev/sda"
		sleep 1
		mke2fs -F -m1 /dev/sda1
		echo "mke2fs -F -m1 /dev/sda1"
		sleep 1
		mke2fs -F -m1 /dev/sda2
		echo "mke2fs -F -m1 /dev/sda2"
		sleep 1
		mke2fs -F -m1 /dev/sda3
		echo "mke2fs -F -m1 /dev/sda3"
		sleep 1
		mke2fs -F -m1 /dev/sda4
		echo "mke2fs -F -m1 /dev/sda4"
		sleep 1
		reboot
		;;
	"format")
		umount -l $2 1>/dev/null 2>&1
		mke2fs -F -m1 $2 $3 1>/dev/null 2>&1
		;;
	"restart")
		if [ -f /bin/reg ]; then
		    echo "reset usb controller"
		    reg s 0xb01c0000
		    reg w 440 5
		    reg w 440 1005
		else
		    echo "reg tool not compiled"
		    exit 1
		fi
		;;
	*)
		#
		# usage: storage.sh
		#
		echo "run storage.sh"
		;;

esac

exit 0
