#!/bin/bash

echo "=====================CONFIGURE-LIBPCAP===================="
APROOTDIR=`pwd`
BACKUPCFLAGS=$CFLAGS
BACKUPCPPFLAGS=$CPPFLAGS
BACKUPLDFLAGS=$LDFLAGS

LIBDIR=$FIRMROOT/lib/shared
LIBS=$LIBDIR/lib
INCLUDES=$LIBDIR/include

HBUILD=`uname -m`-pc-linux-gnu
HTARGET=mipsel-linux

CFLAGS="$BACKUPCFLAGS -I$LIBDIR -I$INCLUDES"
CPPFLAGS="$BACKUPCPPFLAGS -I$LIBDIR -I$INCLUDES"
LDFLAGS="$BACKUPLDFLAGS -L$LIBDIR -L$LIBS"

# prefer use bash if multishell
if [ -e /bin/bash ]; then
    SHELL="/bin/bash"
else
    SHELL="/bin/sh"
fi

export CFLAGS LDFLAGS CPPFLAGS SHELL

if [ ! -f $APROOTDIR/Makefile.in ]; then
    cp -f $APROOTDIR/Makefile.in.tmpl $APROOTDIR/Makefile.in
fi
if [ ! -f $APROOTDIR/configure ]; then
    libtoolize -c -f -i
    aclocal --force
    autoreconf -fi
    autoconf --force
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
CONFOPTS="$CONFOPTS --enable-shared"
CONFOPTS="$CONFOPTS --with-pcap=linux"
CONFOPTS="$CONFOPTS --disable-bluetooth --without-libnl $IPV6"

./configure $CONFOPTS
