#!/bin/sh

##########################################################################################################
#
# MDEV USB control helper (Wive-NG-Project)
#
##########################################################################################################

if [ -f /var/run/syslogd.pid ]; then
    LOG="logger -t usbctrl"
else
    LOG="echo usbctrl"
fi

if [ -z "$ACTION" ]; then
    for path in $(find /sys/devices -name "$MDEV" 2>/dev/null); do
        DEVPATH=${path#/sys}
    done
    MODALIAS=$(cat /sys${DEVPATH}/modalias 2>/dev/null)
    TYPE=$(echo $MODALIAS | printf '%d/%d/%d' $(sed 's/.*d[0-9]\{4\}//;s/ic.*//;s/[dscp]\+/ 0x/g'))
    PRODUCT=$(echo $MODALIAS | sed 's!^usb:\(.*\)dc.*!\1!;s![vpd]!/!g;s!/0\{1,3\}!/!g;s!^/!!;y!ABCDEF!abcdef!')
#' hack for mcedit
    INTERFACE=$(echo $MODALIAS | printf '%d/%d/%d' $(sed 's/.*dp[0-F]\{2\}//;s/[iscp]\+/ 0x/g'))
    for var in DEVPATH MODALIAS TYPE PRODUCT INTERFACE; do
        [ -z "$(eval "echo \$${var}")" ] && $LOG "Could not set uevent environment variable $var" && exit 1
    done
fi

[ -z "$DEVPATH" ] && $LOG "environment variable DEVPATH is unset" && exit 1
if [ -d /sys${DEVPATH} ]; then
    cd /sys${DEVPATH}/..
    for var in id[PV]*; do
        [ -r $var ] && eval "$var='$(cat $var)'"
    done
fi

[ 0 -eq "${TYPE%%/*}" ] && TYPE=$INTERFACE

case $TYPE in
    7/*)
	$LOG "${ACTION} ${idVendor}:${idProduct} may be printer"
	if [ ! -d /sys/module/usblp ]; then
	    $LOG "Load usblp"
	    modprobe -q usblp
	fi
	# Create dev node. Only one printer support. Fix me later.
	if [ ! -f /dev/usb/lp0 ]; then
	    $LOG "Create devs node... for ${idVendor}:${idProduct}"
	    mknod /dev/usb/lp0 c 180 0
	fi
	;;
    8/6/*)
	if [ -f "/usr/share/usb_modeswitch/${idVendor}:${idProduct}" ]; then
	    $LOG "${ACTION} ${idVendor}:${idProduct} may be 3G modem in zero CD mode. Call usb_modeswitch"
	    usb_modeswitch -v ${idVendor} -p ${idProduct} -c /share/usb_modeswitch/${idVendor}:${idProduct}
	else
	    $LOG "${ACTION} ${idVendor}:${idProduct} may be storage"
	    if [ ! -d /sys/module/usb-storage ]; then
		$LOG "Load module usb-storage and wait initialization to complete"
		modprobe -q usb-storage
		sleep 3
		count=0
		while [ ! -d /sys/module/usb_storage ]; do
		    modprobe -q usb-storage
		    if [ "$count" = "5" ]; then
			$LOG "modprobe usb-storage failed!!! please fix me"
			exit 1
		    fi
		    count="$(($count+1))"
		    sleep 5
		done
		$LOG "usb_storage init complete"
	    fi
	fi
        ;;
    255/255/255)
	if [ -f "/usr/share/usb_modeswitch/${idVendor}:${idProduct}" ]; then
	    $LOG "${ACTION} ${idVendor}:${idProduct} may be 3G/4G modem"
	    if [ "${idVendor}" = "0af0" ]; then
		if [ ! -d /sys/module/hso ]; then
		    $LOG "Load hso"
		    modprobe -q hso
		fi
	    else
		if [ ! -d /sys/module/usbserial ]; then
		    $LOG "Load usbserial for ${idVendor}:${idProduct}"
		    if [ "$CONFIG_USB_SERIAL_GENERIC" != "" ]; then
			modprobe -q usbserial vendor=0x${idVendor} product=0x${idProduct}
		    else
			modprobe -q usbserial
		    fi
		fi
	    fi
	else
	    $LOG "${ACTION} ${idVendor}:${idProduct} unknow serial device! Load all builded serial drivers."
	    modprobe -q usbserial
	    modprobe -q option
	    modprobe -q cdc_acm
	    modprobe -q cdc-wdm
	    modprobe -q cdc_ether
	    modprobe -q cdc_mbim
	    modprobe -q cdc_ncm
	    modprobe -q huawei_cdc_ncm
	    modprobe -q pl2303
	    modprobe -q usb_wwan
	    modprobe -q qmi_wwan
	    modprobe -q qcserial
	    modprobe -q hso
	fi
        ;;
    *)
	# skip try load modules for internal hubs
	if [ "${idVendor}:${idProduct}" = "0000:0000" ]; then
	    exit 0
	fi

        $LOG "${ACTION} device ${idVendor}:${idProduct} type ${TYPE} interface ${INTERFACE}"
	$LOG "Try load module $MODALIAS"
        modprobe -q $MODALIAS
        ;;
esac
