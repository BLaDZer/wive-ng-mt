#!/bin/bash

echo "==================CONFIGURE-DAVFS2==========================="
APROOTDIR=`pwd`

HBUILD=`uname -m`-pc-linux-gnu
HTARGET=mipsel-linux

if [ ! -f $APROOTDIR/configure ]; then
    aclocal
    autoreconf -fi
    autoconf
fi

if [ ! -f $APROOTDIR/Makefile.in ] || [ ! -f $APROOTDIR/Makefile ]; then
    automake -c --add-missing
    automake
fi

CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD"
CONFOPTS="$CONFOPTS"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem"

./configure $CONFOPTS
