#!/bin/bash

echo "=====================CONFIGURE-LIBNFNETLINK===================="
APROOTDIR=`pwd`

HBUILD=`uname -m`-pc-linux-gnu
HTARGET=mipsel-linux

if [ ! -f $APROOTDIR/configure ]; then
    libtoolize -c --force
    aclocal
    autoreconf -fi
    autoconf
fi
if [ ! -f $APROOTDIR/Makefile.in ]; then
    automake --add-missing --force-missing --copy --foreign
fi

CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem \
	  --disable-dependency-tracking --enable-shared --disable-static \
	  CFLAGS=-D__linux__ libnet_cv_have_packet_socket=y"

./configure $CONFOPTS
