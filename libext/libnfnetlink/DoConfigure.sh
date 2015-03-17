#!/bin/bash

echo "=====================CONFIGURE-LIBNFNETLINK===================="
APROOTDIR=`pwd`

if [ ! -f $APROOTDIR/configure ]; then
    libtoolize -c --force
    aclocal
    autoreconf -fi
    autoconf
fi
if [ ! -f $APROOTDIR/Makefile.in ]; then
    automake -c --add-missing
    automake --foreign
fi

HBUILD=`uname -m`-pc-linux-gnu
HTARGET=mipsel-linux

#arch options
CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD"

CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem \
	  --disable-dependency-tracking --enable-shared --disable-static \
	  CFLAGS=-D__linux__ libnet_cv_have_packet_socket=y"

./configure $CONFOPTS
