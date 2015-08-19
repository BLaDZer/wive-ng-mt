#!/bin/bash

echo "=====================CONFIGURE-LIBPCAP===================="
APROOTDIR=`pwd`
BACKUPCFLAGS=$CFLAGS
BACKUPLDFLAGS=$LDFLAGS
LIBDIR=$FIRMROOT/lib/shared
LIBS=$LIBDIR/lib
INCLUDES=$LIBDIR/include

HBUILD=`uname -m`-pc-linux-gnu
HTARGET=mipsel-linux

CFLAGS="$BACKUPCFLAGS -I$INCLUDES"
CPPFLAGS="$BACKUPCFLAGS -I$INCLUDES"
LDFLAGS="$BACKUPLDFLAGS -L$LIBS"

export CFLAGS LDFLAGS CPPFLAGS

if [ ! -f $APROOTDIR/Makefile.in ]; then
    cp -f $APROOTDIR/Makefile.in.tmpl $APROOTDIR/Makefile.in
fi
if [ ! -f $APROOTDIR/configure ]; then
    autoreconf -fi
    autoconf
fi
if [ -f ../../linux/.config ]; then
    IPV6=`cat ../../linux/.config | grep "CONFIG_IPV6=y" -c`
    if [ "$IPV6" != "0" ]; then
	IPV6="--enable-ipv6"
    else
	IPV6=""
    fi
fi

CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem"
CONFOPTS="$CONFOPTS --enable-shared --with-pcap=linux"
CONFOPTS="$CONFOPTS --disable-bluetooth --disable-canusb --disable-can --without-flex --without-bison --without-libnl $IPV6"

./configure $CONFOPTS
