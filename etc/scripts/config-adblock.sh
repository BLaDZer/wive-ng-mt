#!/bin/sh

#############################################################
# config-adblock.sh - configure ads DNS filter for Wive-NG  #
#############################################################

# prevent double start adblock configure in one time
while [ -e /tmp/adblock_runing ]; do
    # Sleep until file does exists/is created
    usleep 500000
done
echo $$ > /tmp/adblock_runing

get_param()
{
    eval `nvram_buf_get 2860 dns_adblock dns_adblock_skip`

    LOG="logger -t adblock"

    list='/tmp/list'
    templist="$list.tmp"

    # RUSSIAN BLOCK LIST
    blocklists="http://cdn.raletag.gq/rueasyhosts.txt"
    # GLOBAL BLOCK LISTS
    blocklists="$blocklists http://winhelp2002.mvps.org/hosts.txt"
    blocklists="$blocklists https://pgl.yoyo.org/adservers/serverlist.php?hostformat=hosts&showintro=0&mimetype=plaintext"
    blocklists="$blocklists http://www.malwaredomainlist.com/hostslist/hosts.txt"
    # skip some counters - prevent break sites view and allow small analitycs
    unblocklist="liveinternet.ru|counter.yadro.ru|^yadro.ru|top100.ru|mc.yandex.ru|metrika|openstat.net"
    unblocklist="$unblocklist|google-analytics.com|googletagmanager.com|^stats.g.doubleclick.net|clustrmaps.com"

    # add user unblock list
    if [ "$dns_adblock_skip" != "" ]; then
	unblocklist="$unblocklist|$dns_adblock_skip"
    fi

}

get_and_parse_lists()
{
    rm -f $templist
    for url in $blocklists ; do
	$LOG "Get ad hosts lists from $url"
	wget "$url" -O - -q -c >> $templist
    done

    if [ -e $templist ]; then
	$LOG "Filter and remove duplicated records."
	dos2unix -u $templist > /dev/null 2>&1
	grep -vE 'localhost|\|#|^\\|\\$' $templist | grep '^[0-9a-zA-Z]' | awk '{print $2}' | sed $'s/\r$//' | sed '/^$/d' | sort -u > $list
    fi
    mv -f "$list" "$templist"
    if [ -e $templist ]; then
	# allow user domains and baisc counters
	if [ "$unblocklist" != "" ]; then
	    domains=`cat $templist | grep -vE "$unblocklist"`
	else
	    domains=`cat $templist`
	fi
	count=0
	rm -f "$list"
	for url in $domains ; do
	    if [ "$url" != "" ]; then
		echo "address=/${url}/#" >> $list
		count="$(($count+1))"
	    fi
	done
	if [ -e $list ]; then
	    mv -f "$list" /etc/dnsmasq.d/ads.conf
	    $LOG "$count domains blocked by DNS."
	    service dnsserver restart
	    $LOG "Next adblock update after 24h."
	    needsleep="86400"
	else
	    $LOG "Lists get error. Network problem? Retry by 60 seconds."
	    needsleep="60"
	fi
	rm -f "$templist"
    else
	$LOG "Lists get error. Network problem? Retry by 60 seconds."
	needsleep="60"
    fi
}

get_addresses() {
    # get adress from url
    addr=`echo "${url}" | awk -F/ '{print $3}' | sed 's/:.*//'`
    if [ "$addr" = "" ]; then
        $LOG "Error adress get from URL $url."
        return
    fi
}

get_param
while [ $dns_adblock == "1" ]; do
	# renew param
	get_param
	# check sources
	reachable=0
	for url in $blocklists ; do
	    get_addresses
	    ping -q -c 1 "$addr" > /dev/null 2>&1
	    if [ "$?" -eq 0 ]; then
		reachable=1
		break
	    fi
	done
	# if one or more blocklist source ping ok - try get/parse and apply
	# if not - wait 15 seconds for new try, may me network not aviable
	if [ "$reachable" = "1" ]; then
		get_and_parse_lists
	else
	        $LOG "Server unreachable (internet connection not ready?) wait 15 sec and repeat try."
		needsleep="15"
	fi
	sleep $needsleep
done

# remove ext dnsmasq option with block lists
rm -f /etc/dnsmasq.d/ads.conf

# remove running flag
rm -f /tmp/adblock_runing
