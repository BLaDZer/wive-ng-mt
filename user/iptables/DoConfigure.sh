#!/bin/bash

echo "==================CONFIGURE-IPTABLES=============================="
APROOTDIR=`pwd`

HBUILD=`uname -m`-pc-linux-gnu
HTARGET=mipsel-linux

if [ ! -f $APROOTDIR/configure ]; then
    aclocal
    autoreconf -fi
    autoconf
fi
if [ ! -f $APROOTDIR/Makefile.in ]; then
    automake --add-missing --force-missing --copy
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
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem --disable-dependency-tracking --enable-static --disable-shared --with-xtlibdir=/lib $IPV6"

./configure $CONFOPTS
