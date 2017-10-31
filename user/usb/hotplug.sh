#!/bin/sh

##########################################################################################################
#
# MDEV hotplug helper (Wive-NG-Project)
#
##########################################################################################################

if [ -f /var/run/syslogd.pid ]; then
    LOG="logger -t hotplug($$)"
else
    LOG="echo hotplug($$)"
fi

$LOG "mdev helper started as '$0' in '$( pwd )', MDEV '$MDEV', ACTION '$ACTION', DEVPATH '$DEVPATH', SUBSYSTEM '$SUBSYSTEM', SEQNUM '$SEQNUM'"
