#!/bin/sh


##########################################################################################################
#
# Entware install helper (Wive-NG-Project)
#
##########################################################################################################

optmount=`mount | grep "opt" -c`
if [ "$optmount" ]; then
    cd /opt
    wget -O - http://entware.wl500g.info/binaries/entware/installer/entware_install.sh | sh
    echo "!!!Need relogin or reboot device before use!!!"
    # reload params and profile
    . /etc/scripts/global.sh
else
    echo "/opt not mounted."
fi
