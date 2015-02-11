#!/bin/bash

echo "=====================CONFIGURE-LIBNL========================"
APROOTDIR=`pwd`

if [ ! -f $APROOTDIR/configure ]; then
    libtoolize -c --force
    aclocal
    autoconf
fi

HBUILD=`uname -m`-pc-linux-gnu
HTARGET=mipsel-linux

#arch options
CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD"

CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem \
	  CFLAGS=-D__linux__ libnet_cv_have_packet_socket=y"

./configure $CONFOPTS
