#!/bin/sh

###################################################
# config-udhcpd.sh - configure DHCP server helper #
###################################################

fname="/etc/udhcpd.conf"
fbak="/etc/udhcpd.conf_bak"
pidfile="/var/run/udhcpd.pid"
leases="/var/udhcpd.leases"

usage () {
  echo "usage: config-udhcpd.sh [option]..."
  echo "options:"
  echo "  -h              : print this help"
  echo "  -s ipaddr       : set ipaddr as start of the IP lease block"
  echo "  -e ipaddr       : set ipaddr as end of the IP lease block"
  echo "  -i ifc          : set ifc as the interface that udhcpd will use"
  echo "  -d dns1 [dns2]  : set dns1 and dns2 as DNS"
  echo "  -D domain	  : set domain name"
  echo "  -m mask         : set mask as subnet netmask"
  echo "  -g gateway      : set gateway as router's IP address"
  echo "  -w wins	  : set wins server IP address"
  echo "  -t time         : set time seconds as the IP life time"
  echo "  -r [sleep_time] : run dhcp server"
  echo "  -k              : kill the running dhcp server"
  echo "  -S [mac ipaddr] : statically assign IP to given MAC address"
  exit
}

config () {
  case "$1" in
    "-s")
      sed -e '/start/d' $fname > $fbak
      echo "start $2" >> $fbak ;;
    "-e")
      sed -e '/end/d' $fname > $fbak
      echo "end $2" >> $fbak ;;
    "-i")
      sed -e '/interface/d' $fname > $fbak
      echo "interface $2" >> $fbak ;;
    "-d")
      sed -e '/option *dns/d' $fname > $fbak
      echo "option dns $2 $3" >> $fbak ;;
    "-D")
      sed -e '/option *domain/d' $fname > $fbak
      echo "opt domain $2" >> $fbak ;;
    "-m")
      sed -e '/option *subnet/d' $fname > $fbak
      echo "option subnet $2" >> $fbak ;;
    "-g")
      sed -e '/option *router/d' $fname > $fbak
      echo "option router $2" >> $fbak ;;
    "-w")
      sed -e '/option *wins/d' $fname > $fbak
      echo "option wins $2" >> $fbak ;;
    "-t")
      sed -e '/option *lease/d' $fname > $fbak
      echo "option lease $2" >> $fbak ;;
    "-S")
      if [ "$2" = "" ]; then
        sed -e '/static_lease/d' $fname > $fbak
      elif [ "$3" = "" ]; then
	echo "insufficient arguments.."
	usage
      else
        echo "static_lease $2 $3" >> $fname
	return
      fi
      ;;
    *) return;;
  esac
  cat $fbak > $fname
  rm -f $fbak
  return
}

# argv 1 is empty
if [ "$1" = "" ]; then
  usage
fi

# argv 2 is empty
if [ "$2" = "" ]; then
  if [ "$1" != "-r" ]; then
    if [ "$1" != "-k" ]; then
      usage
	fi
  fi
fi

touch $fname

case "$1" in
  "-h") usage;;
  "-s") config "$1" "$2";;
  "-e") config "$1" "$2";;
  "-i") config "$1" "$2";;
  "-d") config "$1" "$2" "$3";;
  "-D") config "$1" "$2";;
  "-m") config "$1" "$2";;
  "-g") config "$1" "$2";;
  "-w") config "$1" "$2";;
  "-t") config "$1" "$2";;
  "-S") config "$1" "$2" "$3";;
  "-r")

    killall -q udhcpd
    killall -q -SIGKILL udhcpd
    rm -f $pidfile
    touch $leases
    touch $fname
    echo "lease_file $leases" >> $fname
    udhcpd -S $fname &
    ;;
  "-k")
    service dhcpd stop
    rm -f $pidfile ;;
  *) usage;;
esac
