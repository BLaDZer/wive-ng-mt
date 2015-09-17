#!/bin/bash

echo "==================CONFIGURE-NTFS-3G==========================="
APROOTDIR=`pwd`

HBUILD=`uname -m`-pc-linux-gnu
HTARGET=mipsel-linux

if [ ! -f $APROOTDIR/configure ]; then
    aclocal
    autoreconf -fi
    autoconf
fi
if [ ! -f $APROOTDIR/Makefile ]; then
    automake --add-missing --force-missing --copy
fi

CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD"
CONFOPTS="$CONFOPTS --enable-shared --disable-static"
CONFOPTS="$CONFOPTS --disable-mtab --disable-debug"
CONFOPTS="$CONFOPTS --disable-library --disable-ldconfig --disable-mount-helper --with-fuse=internal"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem"

./configure $CONFOPTS
