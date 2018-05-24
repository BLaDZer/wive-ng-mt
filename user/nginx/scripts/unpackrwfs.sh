#!/bin/sh

echo "Unpack user RWFS part"				> /dev/console 2>&1
bzcat $1  | tar xf - -C / || exit 1			> /dev/console 2>&1
echo "RWFS LOAD OK!!!! $1" > /etc/rwfsloadbyuser

if [ -f /etc/scripts/userrwfs.sh ]; then
    echo "Start postrwfs load script"			> /dev/console 2>&1
    sh /etc/scripts/userrwfs.sh				> /dev/console 2>&1
fi

fs save                                                 > /dev/console 2>&1
