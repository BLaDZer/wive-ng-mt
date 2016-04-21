#!/bin/bash

echo "=====================CONFIGURE-LIBTALLOC===================="
APROOTDIR=`pwd`
BACKUPCFLAGS=$CFLAGS
BACKUPLDFLAGS=$LDFLAGS
LIBDIR=$FIRMROOT/lib/shared
LIBS=$LIBDIR/lib
INCLUDES=$LIBDIR/include

CFLAGS="$BACKUPCFLAGS -I$INCLUDES"
CPPFLAGS="$BACKUPCFLAGS -I$INCLUDES"
LDFLAGS="$BACKUPLDFLAGS -L$LIBS"

export CFLAGS LDFLAGS CPPFLAGS

if [ ! -f Makefile ]; then
    cp -f Makefile.template Makefile
fi
if [ ! -f configure ]; then
    cp -f configure.template configure
fi

CONFOPTS="--hostcc=gcc --cross-compile --cross-answers=mips.txt"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem --without-gettext --disable-python"

./configure $CONFOPTS
