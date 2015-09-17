#!/bin/bash

echo "=====================CONFIGURE-LIBNEON===================="
APROOTDIR=`pwd`
BACKUPCFLAGS=$CFLAGS
BACKUPLDFLAGS=$LDFLAGS
# fix for find xml2-config
BACKUPPATH=$PATH
PATH=$PATH:$APROOTDIR/../libxml2:$APROOTDIR/../libiconv/lib
LIBDIR=$FIRMROOT/lib/shared
LIBS=$LIBDIR/lib
INCLUDES=$LIBDIR/include

HBUILD=`uname -m`-pc-linux-gnu
HTARGET=mipsel-linux

CFLAGS="$BACKUPCFLAGS -I$INCLUDES"
CPPFLAGS="$BACKUPCFLAGS -I$INCLUDES"
LDFLAGS="$BACKUPLDFLAGS -L$LIBS"

export CFLAGS LDFLAGS CPPFLAGS

if [ ! -f $APROOTDIR/configure ]; then
    autoreconf -fi
    sh ./autogen.sh
fi

CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD"
CONFOPTS="$CONFOPTS --enable-shared --disable-static"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem --disable-debug --enable-webdav"
CONFOPTS="$CONFOPTS --without-pakchois --without-ca-bundle --without-gssapi --without-libproxy --with-ssl --with-libs=$LIBDIR"

./configure $CONFOPTS

PATH=$BACKUPPATH
