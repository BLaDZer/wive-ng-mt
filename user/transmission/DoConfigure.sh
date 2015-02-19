#!/bin/bash

echo "==================CONFIGURE-TRANSMISSION======================="
APROOTDIR=`pwd`

$APROOTDIR/update-version-h.sh

if [ ! -f $APROOTDIR/configure ]; then
    sh ./autogen.sh
fi
if [ ! -f $APROOTDIR/Makefile.in ]; then
    autoreconf -fi
    automake -c --add-missing
    automake
fi

HBUILD=`uname -m`-pc-linux-gnu
HTARGET=mipsel-linux

#arch options
CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD"

./configure $CONFOPTS \
	    --prefix=$APROOTDIR/filesystem \
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
	    --disable-dependency-tracking
