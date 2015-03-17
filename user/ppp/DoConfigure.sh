#!/bin/bash

echo "==================CONFIGURE-PPPD==============================="
APROOTDIR=`pwd`

HBUILD=`uname -m`-pc-linux-gnu
HTARGET=mipsel-linux

#arch options
CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD --prefix=$APROOTDIR/filesystem"

./configure $CONFOPTS
