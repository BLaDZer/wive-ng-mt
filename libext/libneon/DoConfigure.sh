#!/bin/bash

echo "=====================CONFIGURE-LIBNEON===================="
APROOTDIR=`pwd`
BACKUPCFLAGS=$CFLAGS
BACKUPLDFLAGS=$LDFLAGS
LIBDIR=$FIRMROOT/lib/shared
LIBS=$LIBDIR/lib
INCLUDES=$LIBDIR/include

if [ ! -f $APROOTDIR/configure ]; then
    autoreconf -fi
    sh ./autogen.sh
fi

HBUILD=`uname -m`-pc-linux-gnu
HTARGET=mipsel-linux

CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem --disable-debug --enable-webdav --enable-shared --disable-static"
CONFOPTS="$CONFOPTS --without-pakchois --without-ca-bundle --without-gssapi --without-libproxy --with-ssl"
CFLAGS="$BACKUPCFLAGS -I$INCLUDES"
CPPFLAGS="$BACKUPCFLAGS -I$INCLUDES"
LDFLAGS="$BACKUPLDFLAGS -L$LIBS"

export CFLAGS LDFLAGS CPPFLAGS

./configure $CONFOPTS
