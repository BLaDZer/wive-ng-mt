#!/bin/bash

echo "=====================CONFIGURE-LIBTALLOC===================="
APROOTDIR=`pwd`
BACKUPCFLAGS=$CFLAGS
BACKUPCPPFLAGS=$CPPFLAGS
BACKUPLDFLAGS=$LDFLAGS
LIBDIR=$FIRMROOT/lib/shared
LIBS=$LIBDIR/lib
INCLUDES=$LIBDIR/include

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

if [ ! -f Makefile ]; then
    cp -f Makefile.template Makefile
fi
if [ ! -f configure ]; then
    cp -f configure.template configure
fi

CONFOPTS="--hostcc=gcc --cross-compile --cross-answers=mips.txt"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem --without-gettext --disable-python --disable-rpath"

./configure $CONFOPTS
