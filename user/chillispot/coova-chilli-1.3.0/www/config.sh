#!/bin/sh
if [ -f config-local.sh ]; then
    . ./config-local.sh 
else
    [ -f /opt/Wive-MT/user/chillispot/coova-chilli-1.3.0/filesystem/etc/chilli/defaults ] && . /opt/Wive-MT/user/chillispot/coova-chilli-1.3.0/filesystem/etc/chilli/defaults
    [ -f /opt/Wive-MT/user/chillispot/coova-chilli-1.3.0/filesystem/etc/chilli/config ]   && . /opt/Wive-MT/user/chillispot/coova-chilli-1.3.0/filesystem/etc/chilli/config
fi
