#!/bin/bash

echo "=====================CONFIGURE-LIBEVENT===================="
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

if [ ! -f $APROOTDIR/configure ]; then
    autoreconf -fi -I include
    sh ./autogen.sh
fi
if [ ! -f $APROOTDIR/Makefile.in ] || [ ! -f $APROOTDIR/missing -a ! -f $APROOTDIR/build-aux/missing ]; then
    automake --add-missing --force-missing --copy
fi

CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD"
CONFOPTS="$CONFOPTS --enable-shared --disable-static"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem --disable-debug-mode --disable-dependency-tracking --disable-openssl"

./configure $CONFOPTS
