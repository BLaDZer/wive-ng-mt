#!/bin/bash

echo "==================CONFIGURE-TRANSMISSION======================="
APROOTDIR=`pwd`

$APROOTDIR/update-version-h.sh

HBUILD=`uname -m`-pc-linux-gnu
HTARGET=mipsel-linux

# prefer use bash if multishell
if [ -e /bin/bash ]; then
    SHELL="/bin/bash"
else
    SHELL="/bin/sh"
fi

export SHELL

if [ ! -f $APROOTDIR/configure ]; then
    autoreconf -fi
    sh ./autogen.sh
fi
if [ ! -f $APROOTDIR/Makefile.in ] || [ ! -f $APROOTDIR/missing -a ! -f $APROOTDIR/build-aux/missing ]; then
    automake --add-missing --force-missing --copy
fi

CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD"

./configure $CONFOPTS \
	    --prefix=$APROOTDIR/filesystem \
	    --enable-shared --disable-static \
	    --disable-cli --disable-mac --disable-nls --disable-utp \
	    --without-gtk \
	    --enable-external-natpmp \
	    --enable-lightweight \
	    --with-zlib=$FIRMROOT/lib/shared/lib \
	    --with-zlib-includes=$FIRMROOT/lib/shared/include \
	    OPENSSL_CFLAGS="-I$FIRMROOT/lib/shared/include" \
	    OPENSSL_LIBS="-L$FIRMROOT/lib/shared/lib -lcrypto -lssl" \
	    LIBCURL_CFLAGS="-I$FIRMROOT/lib/shared/include" \
	    LIBCURL_LIBS="-L$FIRMROOT/lib/shared/lib -lcurl" \
	    LIBEVENT_CFLAGS="-I$FIRMROOT/lib/shared/include" \
	    LIBEVENT_LIBS="-L$FIRMROOT/lib/shared/lib -levent" \
	    --with-systemd-daemon=no \
	    --disable-dependency-tracking
