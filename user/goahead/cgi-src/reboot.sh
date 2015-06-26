#!/bin/sh

echo "Content-type: text/html"
echo ''
echo '<html>'
echo '<head><script type="text/javascript" src="/js/ajax.js"></script></head>'
echo '<body><script lang=\"JavaScript\">ajaxReloadDelayedPage(60000);</script></body>'
echo '</html>'

# send release to dhcp server
killall -q -SIGUSR2 udhcpc	> /dev/null 2>&1
# save rwfs
fs save				> /dev/console 2>&1
# unload all services
/etc/scripts/wifi_unload.sh     > /dev/console 2>&1

reboot
