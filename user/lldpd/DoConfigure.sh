#!/bin/bash

echo "==================CONFIGURE-LLDPD==============================="
APROOTDIR=`pwd`

if [ ! -f $APROOTDIR/Makefile.in ]; then
    ./autogen.sh
fi
if [ ! -f $APROOTDIR/configure ]; then
    aclocal
    autoconf
fi

HBUILD=`uname -m`-pc-linux-gnu
HTARGET=mipsel-linux

#arch options
CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD --with-readline=no --with-embedded-libevent=no"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem --disable-privsep"
CONFOPTS="$CONFOPTS --disable-cdp --enable-fdp --enable-edp --enable-lldpmed --enable-dot1 --enable-dot3"
CONFOPTS="$CONFOPTS --with-lldpd-pid-file=/var/run/lldpd.pid --with-lldpd-ctl-socket=/var/run/lldpd.socket"

ac_cv_lib_nl_nl_connect=no

./configure $CONFOPTS
