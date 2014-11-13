#!/bin/bash

echo "==================CONFIGURE-QUAGGA=============================="
APROOTDIR=`pwd`

if [ ! -f $APROOTDIR/configure ]; then
    aclocal
    autoconf
fi
if [ ! -f $APROOTDIR/Makefile.in ]; then
    automake -c --add-missing
    automake
fi

HBUILD=`uname -m`-pc-linux-gnu
HTARGET=mipsel-linux

#arch options
CONFOPTS="--host=$HTARGET --build=$HBUILD"

CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem"
CONFOPTS="$CONFOPTS --disable-bgpd --disable-babeld --disable-ripngd"
CONFOPTS="$CONFOPTS --disable-ospfapi --disable-ospfclient --disable-ospfd --disable-ospf6d"
CONFOPTS="$CONFOPTS --disable-dependency-tracking"

./configure $CONFOPTS
