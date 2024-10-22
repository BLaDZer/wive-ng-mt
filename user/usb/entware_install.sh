#!/bin/sh


##########################################################################################################
#
# Entware install helper (Wive-NG-Project)
#
##########################################################################################################

optmount=`mount | grep opt -c`
if [ "$optmount" != "0" ]; then
    cd /opt
    mkdir -p /opt/home
    wget -O - http://bin.entware.net/mipselsf-k3.4/installer/generic.sh | sh
    # reload params and profile
    . /etc/scripts/global.sh
    echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
    echo "!!!NEED REBOOT DEVICE BEFORE USE!!!"
    echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
else
    echo "/opt not mounted! Check you SD Card/USB Flash or usb drives inserts and check correct part table."
fi
