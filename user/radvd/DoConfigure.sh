#!/bin/bash

echo "==================CONFIGURE-RADVD=============================="
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

CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem --disable-dependency-tracking"

./configure $CONFOPTS
