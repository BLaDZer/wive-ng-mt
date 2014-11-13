#!/bin/sh

##########################################################################################################
#
# Auto attach printer and load firmware helper (Wive-NG-Project)
#
##########################################################################################################


#include global config
. /etc/scripts/global.sh

LOG="logger -t prnctrl"
port=${MDEV##*lp}
devs="/dev/usb/$MDEV"

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

if [ "$ACTION" = "add" ]; then
    eval `nvram_buf_get 2860 PrinterSrvEnabled PrinterSrvBidir`
    if [ "$PrinterSrvEnabled" = "1" ] && [ -z "`pidof p910nd`" ]; then
	upload_firmware
	$LOG "Start p910nd daemon on port 910${port}"
	if [ "$PrinterSrvBidir" = "1" ]; then
	    /bin/p910nd -b -f "$devs" "$port"
	else
	    /bin/p910nd -f "$devs" "$port"
	fi
    fi
else
    if [ ! -z "`pidof p910nd`" ]; then
	$LOG "Stop p910nd daemon on port 910${port}"
	killall -q p910nd
	killall -q -SIGKILL p910nd
	# svae mem in 16Mb devices
	if [ -f /tmp/is_16ram_dev ]; then
	    rmmod usblp > /dev/null 2>&1
	fi
    fi
fi

exit 0
