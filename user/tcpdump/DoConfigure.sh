#!/bin/bash

echo "==================CONFIGURE-TCPDUMP=============================="
APROOTDIR=`pwd`

HBUILD=`uname -m`-pc-linux-gnu
HTARGET=mipsel-linux

if [ ! -f $APROOTDIR/configure ]; then
    aclocal
    autoconf
fi
if [ ! -f $APROOTDIR/Makefile.in ]; then
    cp -f $APROOTDIR/Makefile.in.tmpl $APROOTDIR/Makefile.in
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
CONFOPTS="$CONFOPTS --without-smi --without-crypto --without-cap-ng --disable-smb $IPV6"

./configure $CONFOPTS
