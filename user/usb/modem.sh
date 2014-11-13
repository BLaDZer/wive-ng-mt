#!/bin/sh

##########################################################################################################
#
# Autodetect 3G Modem port (Wive-NG-Project)
#
##########################################################################################################

LOG="logger -t 3G"

symlink_modem() {
    if [ ! -e /dev/modem ]; then
	ln -fs /dev/$MDEV /dev/modem
	MODEMFINDED=1
    fi
}

symlink_pcui() {
    if [ ! -e /dev/pcui ]; then
	ln -fs /dev/$MDEV /dev/pcui
    fi
}

get_param() {
    # get local param always
    eval `nvram_buf_get 2860 MODEMENABLED WMODEMPORT`
    PPPDIR="/etc/ppp"
    OPTFILE="$PPPDIR/peers/dialup"
}

[ -z "`echo $DEVPATH | grep tty/$MDEV`" ] && exit 0

get_param
if [ ! -e /etc/modems.conf ]; then
    $LOG "Error - modems.conf not found."
    exit 1
fi

if [ -z "$ACTION" ]; then
    ACTION="add"
fi

if [ "$ACTION" = "add" -a ! -e "/dev/modem" ]; then
    for path in $(find /sys/devices -name "$MDEV" | sort -r 2>/dev/null); do
        DEVPATH=${path#/sys}
    done
    [ -z "$DEVPATH" ] && $LOG "environment variable DEVPATH is unset" && exit 1
    if [ -d /sys${DEVPATH} ]; then
	cd /sys${DEVPATH}/../..
	for var in id[PV]*; do
	    [ -r $var ] && eval "$var='$(cat $var)'"
	done
	bInterfaceNumber=$(cat /sys${DEVPATH}/../bInterfaceNumber 2>/dev/null)
	bInterfaceProtocol=$(cat /sys${DEVPATH}/../bInterfaceProtocol 2>/dev/null)

	modem_conf=`cat /etc/modems.conf | awk "/^${idVendor}\:${idProduct}/ {split(\\$0,a,\":\"); print \"mVendor=\"a[1] \" mProduct=\"a[2] \" mType=\"a[3] \" mModemIf=\'0\"a[4] \"\' mUserIf=\'0\"a[5] \"\'\"}"`
	eval $modem_conf

	if [ $bInterfaceNumber = $mModemIf ]; then
	    symlink_modem
	fi

	if [ $bInterfaceNumber = $mUserIf ]; then
	    symlink_pcui
	fi
    fi

    if [ "$MDEV" = "$WMODEMPORT" ] || [ "$WMODEMPORT" = "AUTO" -a "$MODEMFINDED" = "1" ]; then
	$LOG "Modem added. Starting helper service -$MDEV-"
	service modemhelper start
    fi
elif [ "$ACTION" = "remove" ]; then
	if [ -e "/dev/modem" ]; then
	    DEVMODEMTTY=`ls -l  /dev/modem | cut -f 2 -d ">" | sed "s/ \/dev\///"`
	fi

	if [ -e "/dev/pcui" ]; then
	    DEVPCUITTY=`ls -l  /dev/pcui | cut -f 2 -d ">" | sed "s/ \/dev\///"`
	fi

	if [ -e $OPTFILE ]; then
	    PPPDTTY=`cat $OPTFILE | grep $MDEV | sed 's/^[ \t]*//;s/[ \t]*$//' `
	else
	    PPPDTTY=''
	fi

	if [ "$DEVPCUITTY" = "$MDEV" ]; then
	    $LOG "Modem removed. Deleting symlink."
	    rm -f /dev/pcui
	fi

	if [ "$DEVMODEMTTY" = "$MDEV" ]; then
	    $LOG "Modem removed. Deleting symlink."
	    rm -f /dev/modem
	    if [ -n `cat $OPTFILE | grep /dev/modem` ]; then
		NEEDSTOPHELPER=1
	    fi
	fi

	if [ "$PPPDTTY" = "/dev/$MDEV" ]; then
		$LOG "Modem removed."
		NEEDSTOPHELPER=1
	fi

	if [ "$NEEDSTOPHELPER" = "1" ]; then
	    $LOG "Stopping helper service."
	    service modemhelper stop
	fi
fi

exit 0
