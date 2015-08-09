#!/bin/sh
# Coova Chilli - David Bird <david@coova.com>
# Licensed under the GPL, see http://coova.org/
# down.sh /dev/tun0 192.168.0.10 255.255.255.0

TUNTAP=$(basename $DEV)
UNDO_FILE=/opt/Wive-MT/user/chillispot/coova-chilli-1.3.0/filesystem/var/run/chilli.$TUNTAP.sh

. /opt/Wive-MT/user/chillispot/coova-chilli-1.3.0/filesystem/etc/chilli/functions

[ -e "$UNDO_FILE" ] && sh $UNDO_FILE 2>/dev/null
rm -f $UNDO_FILE 2>/dev/null

# site specific stuff optional
[ -e /opt/Wive-MT/user/chillispot/coova-chilli-1.3.0/filesystem/etc/chilli/ipdown.sh ] && . /opt/Wive-MT/user/chillispot/coova-chilli-1.3.0/filesystem/etc/chilli/ipdown.sh
