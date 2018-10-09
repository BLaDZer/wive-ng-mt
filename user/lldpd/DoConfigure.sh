#!/bin/bash

echo "==================CONFIGURE-LLDPD==============================="
APROOTDIR=`pwd`

HBUILD=`uname -m`-pc-linux-gnu
HTARGET=mipsel-linux

# prefer use bash if multishell
if [ -e /bin/bash ]; then
    SHELL="/bin/bash"
else
    SHELL="/bin/sh"
fi

export SHELL

if [ ! -f $APROOTDIR/Makefile.in ]; then
    ./autogen.sh
fi
if [ ! -f $APROOTDIR/configure ]; then
    aclocal
    autoconf
fi

CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD --prefix=$APROOTDIR/filesystem"
CONFOPTS="$CONFOPTS --enable-shared --disable-static --disable-dependency-tracking"
CONFOPTS="$CONFOPTS --with-sysusersdir=no --with-systemdsystemunitdir=no --with-launchddaemonsdir=no"
CONFOPTS="$CONFOPTS --with-readline=no --with-xml=no --with-json=no"
CONFOPTS="$CONFOPTS --disable-privsep --disable-cdp --disable-hardening"
CONFOPTS="$CONFOPTS --enable-fdp --enable-edp --enable-lldpmed --enable-dot1 --enable-dot3"
CONFOPTS="$CONFOPTS --with-lldpd-pid-file=/var/run/lldpd.pid --with-lldpd-ctl-socket=/var/run/lldpd.socket"

ac_cv_lib_nl_nl_connect=no \
LIBEVENT_CFLAGS="-I$FIRMROOT/lib/shared/include" \
LIBEVENT_LIBS="-L$FIRMROOT/lib/shared/lib -levent" \
./configure $CONFOPTS
