#!/bin/bash

echo "==================CONFIGURE-IRQBALANCE==============================="
APROOTDIR=`pwd`

HBUILD=`uname -m`-pc-linux-gnu
HTARGET=mipsel-linux

# prefer use bash if multishell
if [ -e /bin/bash ]; then
    SHELL="/bin/bash"
else
    SHELL="/bin/sh"
fi

export SHELL

if [ ! -f $APROOTDIR/Makefile.in ] || [ ! -f $APROOTDIR/missing -a ! -f $APROOTDIR/build-aux/missing ]; then
    automake --add-missing --force-missing --copy
    ./autogen.sh
fi
if [ ! -f $APROOTDIR/configure ]; then
    aclocal
    autoconf
fi

CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD"
CONFOPTS="$CONFOPTS --without-glib2 --without-systemd --enable-shared --disable-static --with-libcap-ng=no"

./configure $CONFOPTS
