#!/bin/bash

echo "==================CONFIGURE-HAVEGED============================"
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

if [ ! -f $APROOTDIR/configure ] || [ ! -f $APROOTDIR/Makefile.in ]; then
    autoreconf -fi
fi

CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem --enable-daemon=yes --enable-diagnostic=no \
	    --enable-olt=no --enable-threads=no --enable-tune=yes --enable-shared=no --enable-static=no"

./configure $CONFOPTS
