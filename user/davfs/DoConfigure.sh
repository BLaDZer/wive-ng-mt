#!/bin/bash

echo "==================CONFIGURE-DAVFS2==========================="
APROOTDIR=`pwd`

if [ ! -f $APROOTDIR/configure ]; then
    aclocal
    autoconf
fi

if [ ! -f $APROOTDIR/Makefile.in ] || [ ! -f $APROOTDIR/Makefile ]; then
    autoreconf -fi
    automake -c --add-missing
    automake
fi

HBUILD=`uname -m`-pc-linux-gnu
HTARGET=mipsel-linux

#arch options
CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD"

CONFOPTS="$CONFOPTS"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem"

./configure $CONFOPTS
