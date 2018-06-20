#!/bin/sh

##########################################################################################################
#
# Auto attach printer and load firmware helper (Wive-NG-Project)
#
##########################################################################################################


#include global config
. /etc/scripts/global.sh

LOG="logger -t prnctrl"

upload_firmware() {
    # For GDI printers put printers firmware
    # file in /etc, rename to prnfw.dl
    if [ -f /opt/etc/prnfw.dl ]; then
	FW="/opt/etc/prnfw.dl"
    elif [ -f /etc/prnfw.dl ]; then
        FW="/etc/prnfw.dl"
    else
	FW=""
    fi
    # if found - upload
    if [ "$FW" != "" ]; then
	$LOG "Upload firmware $FW to printer $MDEV to $devs."
	cat $FW > "$devs"
    else
	$LOG "Not found firmware for printer"
    fi
}

eval `nvram_buf_get 2860 PrinterSrvEnabled PrinterSrvBidir`
killall -q p910nd
killall -q -SIGKILL p910nd

if [ "$ACTION" = "add" ]; then
    if [ "$PrinterSrvEnabled" = "1" ]; then
	# Create dev node. Only one printer support.
	if [ ! -f /dev/usb/lp0 ]; then
	    $LOG "Create devs node... for printer"
	    mknod /dev/usb/lp0 c 180 0
	fi
	upload_firmware
	$LOG "Start p910nd daemon on port 9100"
	if [ "$PrinterSrvBidir" = "1" ]; then
	    /bin/p910nd -b -f /dev/usb/lp0 0
	else
	    /bin/p910nd -f /dev/usb/lp0 0
	fi
    fi
fi
