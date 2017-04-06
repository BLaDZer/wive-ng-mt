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

if [ ! -f $APROOTDIR/Makefile.in ] || [ ! -f $APROOTDIR/Makefile ] || [ ! -f $APROOTDIR/missing -a ! -f $APROOTDIR/build-aux/missing ]; then
    automake --add-missing --force-missing --copy
fi

CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD"
CONFOPTS="$CONFOPTS"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem"

./configure $CONFOPTS
