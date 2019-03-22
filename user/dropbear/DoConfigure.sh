#!/bin/bash

echo "==================CONFIGURE-DROPBEAR==========================="
APROOTDIR=`pwd`

# prefer use bash if multishell
if [ -e /bin/bash ]; then
    SHELL="/bin/bash"
else
    SHELL="/bin/sh"
fi

export SHELL

HBUILD=`uname -m`-pc-linux-gnu
HTARGET=mipsel-linux

if [ ! -f $APROOTDIR/configure ]; then
    aclocal
    autoreconf -fi
    autoconf
fi

CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD"
CONFOPTS="$CONFOPTS --disable-zlib --disable-openpty"
CONFOPTS="$CONFOPTS --disable-shadow --disable-pam --disable-harden"
CONFOPTS="$CONFOPTS --disable-lastlog --disable-utmp --disable-utmpx"
CONFOPTS="$CONFOPTS --disable-wtmp --disable-wtmpx --disable-loginfunc"
CONFOPTS="$CONFOPTS --disable-pututline --disable-pututxline"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem ac_cv_type_socklen_t=yes"

./configure $CONFOPTS
