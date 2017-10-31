#!/bin/sh

##########################################################################################################
#
# MDEV USB control helper (Wive-NG-Project)
#
##########################################################################################################

if [ -f /var/run/syslogd.pid ]; then
    LOG="logger -t usbctrl($$)"
else
    LOG="echo usbctrl($$)"
fi

while kill -0 `cat /tmp/hotplug.pid` 2> /dev/null; do sleep 1; done;
echo "$$" > /tmp/hotplug.pid

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

# Use class code info from Interface Descriptors
[ 0 -eq "${TYPE%%/*}" ] && TYPE=$INTERFACE
$LOG "TYPE=${TYPE} INTERFACE=${INTERFACE}"

modload() {
    if [ ! -d /sys/module/$1 ]; then
	$LOG "Try load $@"
	modprobe -q $@
    fi
}

probe_cdc() {
    case $INTERFACE in
	2/2/*)
	    $LOG "Found Abstract Control Model(USBPSTN). Try load module cdc_acm"
	    modload cdc_acm
	    ;;
	2/6/*)
	    $LOG "Found Ethernet Network Control Model(USBECM). Try load module cdc_ether"
	    modload cdc_ether
	    ;;
	2/9/*)
	    $LOG "Found Device Management(USBWMC). Try load module cdc_wdm"
	    modload cdc_wdm
	    ;;
	2/12/7)
	    $LOG "Found Ethernet Emulation Model. Try load module cdc_eem"
	    modload cdc_eem
	    ;;
	2/13/*)
	    $LOG "Found Network Control Model(USBNCM). Try load module cdc_ncm"
	    modload cdc_ncm
	    ;;
	2/14/*)
	    $LOG "Found Mobile Broadband Interface Model(USBMBIM). Try load module cdc_mbim"
	    modload cdc_mbim
	    ;;
	2/*)
	    $LOG "Found CDC unknow subclass"
	    ;;
	10/*)
	    $LOG "Found Data Class Interface, in spec sub-class is only 00h, but e5573 10/6/0"
	    ;;
    esac
}

probe_huawei_cdc() {
    case $INTERFACE in
	255/2/22|255/2/70|255/2/118|255/3/22)
	    $LOG "Found Huawei device using the CDC NCM protocol as transport layer"
	    modload huawei_cdc_ncm
	    ;;
    esac
}

case $TYPE in
    1/*)
	$LOG "${ACTION} ${idVendor}:${idProduct} may be audio device, but now not support"
	;;
    2/*)
	$LOG "${ACTION} ${idVendor}:${idProduct} may be CDC"
	probe_cdc
	;;
    3/*)
	$LOG "${ACTION} ${idVendor}:${idProduct} may be HID device class, but now not support"
	;;
    5/*)
	$LOG "${ACTION} ${idVendor}:${idProduct} may be physical device class, but now not support"
	;;
    6/*)
	$LOG "${ACTION} ${idVendor}:${idProduct} may be Still Imaging device, but now not support"
	;;
    7/*)
	$LOG "${ACTION} ${idVendor}:${idProduct} may be printer"
	modload usblp
	# Create dev node. Only one printer support. Fix me later.
	if [ ! -f /dev/usb/lp0 ]; then
	    $LOG "Create devs node... for ${idVendor}:${idProduct}"
	    mknod /dev/usb/lp0 c 180 0
	fi
	;;
    8/*)
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
    9/*)
	# skip try load modules for internal hubs
	if [ "${idVendor}:${idProduct}" = "0000:0000" ] || [ "${idVendor}" = "1d6b" ]; then
	    exit 0
	fi
	$LOG "${ACTION} ${idVendor}:${idProduct} may be hub, but now not support"
	;;
    10/*)
	$LOG "${ACTION} ${idVendor}:${idProduct} may be CDC data device, but now not support"
	;;
    11/*)
	$LOG "${ACTION} ${idVendor}:${idProduct} may be Smart Card device, but now not support"
	;;
    13/0/0)
	$LOG "${ACTION} ${idVendor}:${idProduct} may be Content Security device, but now not support"
	;;
    14/*)
	$LOG "${ACTION} ${idVendor}:${idProduct} may be Video device, but now not support"
	;;
    15/*)
	$LOG "${ACTION} ${idVendor}:${idProduct} may be Personal Healthcare device, but now not support"
	;;
    224/*)
	$LOG "${ACTION} ${idVendor}:${idProduct} may be bluetooth, but now not support"
	;;
    255/*)
	$LOG "${ACTION} ${idVendor}:${idProduct} may be 3G/4G modem, try drivers load"
	[ "$idVendor" = "12d1" ] && probe_huawei_cdc
	if [ "${idVendor}" = "0af0" ]; then
	    modload hso
	elif [ -f "/usr/share/usb_modeswitch/${idVendor}:${idProduct}" ]; then
	    modload usbserial vendor=0x${idVendor} product=0x${idProduct}
	else
	    $LOG "Unknown or not serial modem module ${idVendor}:${idProduct}, try load all builded drivers"
	    mod="usbserial option pl2303 qmi_wwan qcserial"
	    for module in $mod; do
		modload $module
	    done
	fi
        ;;
    *)
        $LOG "${ACTION} device ${idVendor}:${idProduct} type ${TYPE} interface ${INTERFACE}"
        modload $MODALIAS
        ;;
esac
