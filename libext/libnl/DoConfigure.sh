#!/bin/bash

echo "=====================CONFIGURE-LIBNL========================"
APROOTDIR=`pwd`

HBUILD=`uname -m`-pc-linux-gnu
HTARGET=mipsel-linux

if [ ! -f $APROOTDIR/configure ]; then
    libtoolize -c --force
    aclocal
    autoreconf -fi
    autoconf
fi

#arch options
CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem CFLAGS=-D__linux__ libnet_cv_have_packet_socket=y"

./configure $CONFOPTS
