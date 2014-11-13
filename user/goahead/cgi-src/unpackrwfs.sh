#!/bin/sh

echo "Unpack user RWFS part"				> /dev/console
bzcat $1  | tar xf - -C / || exit 1			> /dev/console
echo "RWFS LOAD OK!!!! $1" > /etc/rwfsloadbyuser

if [ -f /etc/scripts/userrwfs.sh ]; then
    echo "Start postrwfs load script"			> /dev/console
    sh /etc/scripts/userrwfs.sh				> /dev/console
fi

fs backup_nvram						> /dev/console
fs save                                                 > /dev/console
