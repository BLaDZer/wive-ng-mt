#!/bin/sh

# send release to dhcp server
killall -q -SIGUSR2 udhcpc	> /dev/null 2>&1
# unload all services
wifi_unload.sh			> /dev/null 2>&1
# and reboot device
reboot
