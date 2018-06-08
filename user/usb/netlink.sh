#!/bin/sh

##########################################################################################################
#
# MDEV netlink helper (Wive-NG-Project)
#
##########################################################################################################

# get params
. /etc/scripts/global.sh

LOG="logger -t netlink($$)"

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
    if [ -e /tmp/qmi_port ]; then
	$LOG "Delete $MDEV from qmi list"
	grep -v "$MDEV" /tmp/qmi_port > /tmp/qmi_port
	if [ -z "$(tail -qn1 /tmp/qmi_port)" ]; then
	    $LOG "qmi list is null, delete file"
	    rm -f /tmp/qmi_port
	fi
    fi
}

del_ncm() {
    $LOG "Del modem with ethernet port $MDEV"
    if [ -e /tmp/ncm_port ]; then
	$LOG "Delete $MDEV from ncm list"
	grep -v "$MDEV" /tmp/ncm_port > /tmp/ncm_port
	if [ -z "$(tail -qn1 /tmp/ncm_port)" ]; then
	    $LOG "ncm list is null, delete file"
	    rm -f /tmp/ncm_port
	fi
    fi
}

case "$ACTION" in
    add)
	if [ "$MODEMTYPE" = "3" ] && [ -e "/sys$DEVPATH" ]; then
		add_qmi
	fi
	if [ "$MODEMTYPE" = "2" ] && [ -e "/sys$DEVPATH" ]; then
		add_ncm
	fi
	service modemhelper start
	;;
    remove)
	if [ -e /tmp/qmi_port ]; then
	    del_qmi
	fi
	if [ -e /tmp/ncm_port ]; then
	    del_ncm
	fi
	;;
    *)
	$LOG "Opps. Unknow command '$ACTION'"
esac
