#!/bin/bash

echo "==================CONFIGURE-IRQBALANCE==============================="
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
CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD"
CONFOPTS="$CONFOPTS --host=$HTARGET --target=$HTARGET --build=$HBUILD"

./configure $CONFOPTS
