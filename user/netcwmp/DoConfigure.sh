#!/bin/bash

echo "==================CONFIGURE-NETWCMP=============================="
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

CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem --disable-dependency-tracking"
CONFOPTS="$CONFOPTS --enable-shared=no --enable-static=no --with-openssl=no"

# build with support ssl only if project configured and builded with openssl
if [ -e ../../lib/shared/lib/libssl.so ]; then
    CONFOPTS="$CONFOPTS --with-openssl=${ROOTDIR}/lib/shared"
else
    CONFOPTS="$CONFOPTS --with-openssl=no"
fi

./configure $CONFOPTS
