#!/bin/sh

##########################################################################################################
#
# MDEV netlink helper (Wive-NG-Project)
#
##########################################################################################################

if [ -f /var/run/syslogd.pid ]; then
    LOG="logger -t netlink($$)"
else
    LOG="echo netlink($$)"
fi

$LOG "ACTION '$ACTION', MDEV '$MDEV', ACTION '$ACTION', DEVPATH '$DEVPATH', SUBSYSTEM '$SUBSYSTEM', SEQNUM '$SEQNUM'"

add_qmi() {
    CDC="/dev/$(ls /sys$DEVPATH/device/usb/)"
    $LOG "Add qmi modem with ethernet port $MDEV and control port $CDC"
    echo "$MDEV=$CDC" >> /tmp/qmi_port
}

add_ncm() {
    $LOG "Add modem with ethernet port $MDEV"
    echo "$MDEV" >> /tmp/ncm_port
}

del_qmi() {
    $LOG "Del modem with ethernet port $MDEV"
    grep -v "$MDEV" /tmp/qmi_port > /tmp/qmi_port
}

del_ncm() {
    $LOG "Del modem with ethernet port $MDEV"
}

case "$ACTION" in
    add)
	[ -d "/sys$DEVPATH/qmi" ] && add_qmi
	[ -d "/sys$DEVPATH/cdc_ncm" ] && add_ncm
	service modemhelper start
	;;
    remove)
	[ -f /tmp/qmi_port ] && del_qmi
	[ -f /tmp/ncm_port ] && del_ncm
	;;
    *)
	$LOG "Opps. Unknow command '$ACTION'"
esac
