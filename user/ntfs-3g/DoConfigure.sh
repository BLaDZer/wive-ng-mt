#!/bin/bash

echo "==================CONFIGURE-NTFS-3G==========================="
APROOTDIR=`pwd`

if [ ! -f $APROOTDIR/configure ]; then
    aclocal
    autoconf
fi
if [ ! -f $APROOTDIR/Makefile ]; then
    automake -c --add-missing
    automake
fi

HBUILD=`uname -m`-pc-linux-gnu
HTARGET=mipsel-linux

#arch options
CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD"

		 


CONFOPTS="$CONFOPTS --enable-shared=no --enable-static=no --disable-mtab --disable-debug"
CONFOPTS="$CONFOPTS --disable-library --disable-ldconfig --disable-mount-helper --with-fuse=internal"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem ac_cv_type_socklen_t=yes"

./configure $CONFOPTS
