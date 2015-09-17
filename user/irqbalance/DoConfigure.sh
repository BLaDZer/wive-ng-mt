#!/bin/bash

echo "==================CONFIGURE-IRQBALANCE==============================="
APROOTDIR=`pwd`

HBUILD=`uname -m`-pc-linux-gnu
HTARGET=mipsel-linux

if [ ! -f $APROOTDIR/Makefile.in ]; then
    ./autogen.sh
fi
if [ ! -f $APROOTDIR/configure ]; then
    aclocal
    autoconf
fi

CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD"
CONFOPTS="$CONFOPTS --without-glib2 --without-systemd --enable-shared --disable-static"

./configure $CONFOPTS
