#!/bin/bash

echo "=====================CONFIGURE-LIBICONV===================="
APROOTDIR=`pwd`
BACKUPCFLAGS=$CFLAGS
BACKUPCPPFLAGS=$CPPFLAGS
BACKUPLDFLAGS=$LDFLAGS

LIBDIR=$FIRMROOT/lib/shared
LIBS=$LIBDIR/lib
INCLUDES=$LIBDIR/include

HBUILD=`uname -m`-pc-linux-gnu
HTARGET=mipsel-linux

CFLAGS="$BACKUPCFLAGS -I$INCLUDES"
CPPFLAGS="$BACKUPCPPFLAGS -I$LIBDIR -I$INCLUDES"
LDFLAGS="$BACKUPLDFLAGS -L$LIBDIR -L$LIBS"
GNULIB_TOOL="$APROOTDIR/gnulib/gnulib-tool"

# prefer use bash if multishell
if [ -e /bin/bash ]; then
    SHELL="/bin/bash"
else
    SHELL="/bin/sh"
fi

export CFLAGS LDFLAGS CPPFLAGS GNULIB_TOOL SHELL

if [ ! -f $APROOTDIR/configure ]; then
#    libtoolize -f -c
    sh ./autogen.sh
# --skip-gnulib
    autoreconf -fi
fi

#CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD"

CONFOPTS="--host=$HTARGET"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem"
CONFOPTS="$CONFOPTS --enable-shared --disable-static"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem --disable-debug-mode"

#this small workaround
cp -f $APROOTDIR/inc/*.h $APROOTDIR/lib/
./configure $CONFOPTS

echo "=====================CONFIGURE-LIBCHARSET===================="

# global root dir use for prefix
CONFOPTS="--prefix=$APROOTDIR/filesystem"

cd $APROOTDIR/libcharset
APROOTDIR=`pwd`

if [ ! -f $APROOTDIR/configure ]; then
    sh ./autogen.sh
fi
if [ ! -f $APROOTDIR/Makefile.in ]; then
    automake --add-missing
    automake
fi

CONFOPTS="$CONFOPTS --host=$HTARGET"
./configure $CONFOPTS

cd ..
