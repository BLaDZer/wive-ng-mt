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
	$LOG "${ACTION} ${idVendor}:${idProduct} may be 3G/4G modem, try drivers load"
	if [ -f "/usr/share/usb_modeswitch/${idVendor}:${idProduct}" ] && [ "${idVendor}" != "0af0" ] && [ "${idVendor}" != "1435" ]; then
	    $LOG "Load usbserial for ${idVendor}:${idProduct}"
	    if [ ! -d /sys/module/usbserial ]; then
		modprobe -q usbserial vendor=0x${idVendor} product=0x${idProduct}
	    fi
	else
	    $LOG "Uncknown or not serial modem module ${idVendor}:${idProduct}, try load all builded drivers"
	    mod="usbserial option cdc_acm cdc-wdm cdc_ether cdc_mbim cdc_ncm huawei_cdc_ncm pl2303 usb_wwan qmi_wwan qcserial hso"
	    for module in $mod
	    do
		if [ ! -d /sys/module/$module ]; then
		    modprobe -q $module
		fi
	    done
	fi
        ;;
    *)
	# skip try load modules for internal hubs
	if [ "${idVendor}:${idProduct}" = "0000:0000" ] || [ "${idVendor}" = "1d6b" ]; then
	    exit 0
	fi

        $LOG "${ACTION} device ${idVendor}:${idProduct} type ${TYPE} interface ${INTERFACE}"
	$LOG "Try load module $MODALIAS"
        modprobe -q $MODALIAS
        ;;
esac
