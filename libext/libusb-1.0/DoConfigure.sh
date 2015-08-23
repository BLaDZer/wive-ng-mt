#!/bin/bash

echo "=====================CONFIGURE-LIBUSB===================="
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

if [ ! -f $APROOTDIR/configure ]; then
    autoreconf -fi
    autoconf
fi
if [ ! -f $APROOTDIR/Makefile.in ]; then
    automake --add-missing --force-missing --copy
fi

CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD"
CONFOPTS="$CONFOPTS --disable-dependency-tracking"
CONFOPTS="$CONFOPTS --enable-shared --enable-static"
CONFOPTS="$CONFOPTS --disable-udev --disable-timerfd --disable-examples-build --disable-tests-build --disable-static"
CONFOPTS="$CONFOPTS --enable-log --enable-debug-log"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem"

./configure $CONFOPTS
