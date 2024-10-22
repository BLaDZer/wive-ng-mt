#!/bin/bash

echo "==================CONFIGURE-DHCPv6=============================="

cd wide-dhcpv6

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
    autoconf
fi

CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem"

./configure $CONFOPTS
