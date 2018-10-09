#!/bin/bash

echo "==================CONFIGURE-PPPD==============================="
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

#arch options
CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD --prefix=$APROOTDIR/filesystem"

./configure $CONFOPTS
