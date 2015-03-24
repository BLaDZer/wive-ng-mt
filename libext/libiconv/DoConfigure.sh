#!/bin/bash

echo "=====================CONFIGURE-LIBICONV===================="
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
    sh ./autogen.sh --skip-gnulib
fi

CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem --disable-debug-mode --disable-dependency-tracking --disable-static"

#this small workaround
cp -f $APROOTDIR/inc/*.h $APROOTDIR/lib/
./configure $CONFOPTS

echo "=====================CONFIGURE-LIBCHARSET===================="
cd $APROOTDIR/libcharset
APROOTDIR=`pwd`

CFLAGS="$BACKUPCFLAGS -I$INCLUDES"
CPPFLAGS="$BACKUPCFLAGS -I$INCLUDES"
LDFLAGS="$BACKUPLDFLAGS -L$LIBS"

export CFLAGS LDFLAGS CPPFLAGS

if [ ! -f $APROOTDIR/configure ]; then
    sh ./autogen.sh --skip-gnulib
fi
if [ ! -f $APROOTDIR/Makefile.in ]; then
    automake --add-missing
    automake
fi

CONFOPTS="--host=mipsel-linux --prefix=$APROOTDIR/filesystem"
./configure $CONFOPTS

cd ..
