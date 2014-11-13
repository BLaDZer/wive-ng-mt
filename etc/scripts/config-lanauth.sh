#!/bin/sh

LOG="logger -t lanauth"

start() {
    vpnEnabled=`nvram_get 2860 vpnEnabled`
    if [ -x /bin/lanauth ] && [ "$vpnEnabled" = "on" ]; then
	    reload
    fi
}

reload() {
    get_param
    if  [ "$vpnPassword"  != "" ] && [ "$LANAUTH_LVL" != "" ]; then
	if [ "$pid" != "" ]; then
	    if [ "$LANAUTH_LVL" = "1" ]; then
		killall -q -USR1 lanauth
	    elif [ "$LANAUTH_LVL" = "2" ]; then
		killall -q -USR2 lanauth
	    else
		stop
	    fi
	else
    	    $LOG "Starting lanauth mode $LANAUTH_LVL"
    	    lanauth -v 2 -l $LANAUTH_LVL -p $vpnPassword -A 0 &
	fi
    fi
}

get_param() {
    eval `nvram_buf_get 2860 vpnPassword LANAUTH_LVL`
    pid=`pidof lanauth`
}

stop() {
    pid=`pidof lanauth`
    if [ "$pid" != "" ]; then
	$LOG "Stopping lanauth "
	killall -q lanauth
	killall -q -SIGKILL lanauth
    fi
}

case "$1" in
       start)
           start
           ;;

       stop)
           stop
           ;;

       restart)
           stop
           start
           ;;

	reload)
	   reload
	   ;;

       *)
           echo $"Usage: $0 {start|stop|restart|reload}"
           exit 1
esac
