#!/bin/bash

echo "==================CONFIGURE-QUAGGA=============================="
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

if [ ! -f $APROOTDIR/configure ]; then
    aclocal
    autoreconf -fi -I lib
    autoconf
fi
if [ ! -f $APROOTDIR/Makefile.in ] || [ ! -f $APROOTDIR/missing -a ! -f $APROOTDIR/build-aux/missing ]; then
    automake --add-missing --force-missing --copy
fi

CONFOPTS="--host=$HTARGET --build=$HBUILD"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem"
CONFOPTS="$CONFOPTS --enable-shared --disable-static"
CONFOPTS="$CONFOPTS --disable-vtysh --disable-watchquagga --disable-doc"
CONFOPTS="$CONFOPTS --disable-bgpd --disable-ripngd"
CONFOPTS="$CONFOPTS --disable-ospfapi --disable-ospfclient --disable-ospfd --disable-ospf6d --disable-nhrpd"
CONFOPTS="$CONFOPTS --disable-dependency-tracking"

./configure $CONFOPTS
