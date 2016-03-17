#!/bin/bash

echo "==================CONFIGURE-QUAGGA=============================="
APROOTDIR=`pwd`

HBUILD=`uname -m`-pc-linux-gnu
HTARGET=mipsel-linux

if [ ! -f $APROOTDIR/configure ]; then
    aclocal
    autoreconf -fi
    autoconf
fi
if [ ! -f $APROOTDIR/Makefile.in ]; then
    automake --add-missing --force-missing --copy
fi

CONFOPTS="--host=$HTARGET --build=$HBUILD"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem"
CONFOPTS="$CONFOPTS --enable-shared --disable-static"
CONFOPTS="$CONFOPTS --disable-vtysh --disable-watchquagga --disable-doc"
CONFOPTS="$CONFOPTS --disable-bgpd --disable-ripngd"
CONFOPTS="$CONFOPTS --disable-ospfapi --disable-ospfclient --disable-ospfd --disable-ospf6d"
CONFOPTS="$CONFOPTS --disable-dependency-tracking"

./configure $CONFOPTS
