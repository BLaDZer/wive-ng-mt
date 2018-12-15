#!/bin/sh

###################################################################
# config-cwmp opt43 - configure lanauth client helper for Wive-NG #
###################################################################

#include global config
. /etc/scripts/global.sh

LOG="logger -t autocwmphelper"

vendorspecific="$1"

# extract url form option string
# get first value on option 43 (format id|value_length|value)
extract_url() {
    # get and check len
    length=`printf "%d" "'${vendorspecific:1:1}"`
    if [ "$length" = "" ] || [ "$length" = "0" ] || [ "255" -lt "$length" ]; then
	$LOG "ACS URL $url is incorrect length."
	url=""
	return
    fi

    # extract first url
    url=${vendorspecific:2:$length}
    if [ "$url" = "" ]; then
	$LOG "ACS URL is null."
	return
    fi

    # base url check
    isurl=`echo "$url" | grep -iEc "^http://|^https://"`
    if [ "$isurl" = "0" ]; then
	$LOG "ACS URL $url is not http/https url."
	url=""
	return
    fi
}

get_addresses() {
    # get adress from url
    addr=`echo "${url}" | awk -F/ '{print $3}' | sed 's/:.*//'`
    if [ "$addr" = "" ]; then
	$LOG "Error adress get for URL $url."
	return
    fi

    # check is ip or not
    is_ip=`ipcalc -sn "$addr"`
    if [ "$is_ip" = "" ]; then
	# get ip adresses for build security ruleset
	ipget "$addr" > /tmp/acsaddrip
    else
	echo "$addr" > /tmp/acsaddrip
    fi
}

generate_netfilter_rules() {
    # generate netfilter rules for all acs adresses
    get_addresses

    if [ -e /tmp/acsaddrip ]; then
	ipaddrlist=`cat /tmp/acsaddrip`
	if [ "$cwmpdEnabled" = "2" ] && [ "$cwmpd_httpd_port" != "" ] && [ "$ipaddrlist" != "" ]; then
	    # create iptables user scrips dir if not exist
	    mkdir -p /etc/iptables.d
	    echo "#!/bin/sh" > /etc/iptables.d/cwmpdfw
	    for oneaddr in $ipaddrlist ; do
		if [ "$oneaddr" != "" ]; then
		    echo "iptables -A servicelimit ! -i $lan_if -s $oneaddr $STATE_NEW -p tcp --dport $cwmpd_httpd_port -j ACCEPT" >> /etc/iptables.d/cwmpdfw
		fi
	    done
	    # allow execute
	    chmod 755 /etc/iptables.d/cwmpdfw
	else
	    $LOG "Error adress list not exist or trunclated"
	fi
    else
	$LOG "Error adress list nit exist or trunclated"
    fi
}

# option is not exist - exit with ok exit code
if [ "$vendorspecific" = "" ]; then
    if [ "$cwmpdEnabled" = "2" ]; then
	curuptime=`cat /proc/uptime | awk '{printf "%0.f", $1}'`
	# if uptime more than 1 hour
	# avoid false positive result at first renew
	# only if work or more than one hour without CWMP configure get
	if [ "$curuptime" -lt "3600" ]; then
	    $LOG "ISP not support TR case - disable now."
	    nvram_set 2860 cwmpdEnabled 0
	fi
    fi
    exit 0
fi

# parse, check and normalize acs url from option string
extract_url
if [ "$url" = "" ]; then
    $LOG "Extract URL error..."
    exit 1
fi

# check url changes
# if not changed - exit with ok exit code
if [ -e /tmp/acsurl ]; then
    oldurl=`tail -qn1 /tmp/acsurl`
    if [ "$url" = "$oldurl" ]; then
	exit 0
    fi
fi
echo "$url" > /tmp/acsurl

# generate ruleset for security reason
generate_netfilter_rules
