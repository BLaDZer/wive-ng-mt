#!/bin/bash

echo "==================CONFIGURE-TRANSMISSION======================="
APROOTDIR=`pwd`
BACKUPCFLAGS=$CFLAGS
BACKUPCPPFLAGS=$CPPFLAGS
BACKUPLDFLAGS=$LDFLAGS
LIBDIR=$FIRMROOT/lib/shared
LIBS=$LIBDIR/lib
INCLUDES=$LIBDIR/include

export PKG_CONFIG=/bin/true

HBUILD=`uname -m`-pc-linux-gnu
HTARGET=mipsel-linux

CFLAGS="$BACKUPCFLAGS -I$LIBDIR -I$INCLUDES -I$INCLUDES/openssl"
CPPFLAGS="$BACKUPCFLAGS -I$LIBDIR -I$INCLUDES -I$INCLUDES/openssl"
LDFLAGS="$BACKUPLDFLAGS -L$LIBDIR -L$LIBS"

# prefer use bash if multishell
if [ -e /bin/bash ]; then
    SHELL="/bin/bash"
else
    SHELL="/bin/sh"
fi

export CFLAGS LDFLAGS CPPFLAGS SHELL

$APROOTDIR/update-version-h.sh

if [ ! -f $APROOTDIR/configure ]; then
    libtoolize -c -f -i
    aclocal --force
    autoreconf -fi
    autoconf --force
    sh ./autogen.sh
fi
if [ ! -f $APROOTDIR/Makefile.in ] || [ ! -f $APROOTDIR/missing -a ! -f $APROOTDIR/build-aux/missing ]; then
    automake --add-missing --force-missing --copy
fi

CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD"

./configure $CONFOPTS \
	    --prefix=$APROOTDIR/filesystem \
	    --enable-shared --disable-static \
	    --disable-cli --disable-mac --disable-nls \
	    --without-gtk \
	    --enable-external-natpmp \
	    --enable-largefile \
	    --enable-lightweight \
	    OPENSSL_CFLAGS="-I$INCLUDES" \
	    OPENSSL_LIBS="-L$LIBS -lcrypto -lssl" \
	    LIBCURL_CFLAGS="-I$INCLUDES" \
	    LIBCURL_LIBS="-L$LIBS -lcurl" \
	    LIBEVENT_CFLAGS="-I$INCLUDES" \
	    LIBEVENT_LIBS="-L$LIBS -levent" \
	    ZLIB_CFLAGS="-L$LIBS -lz" \
	    ZLIB_LIBS="-I$INCLUDES" \
	    --with-crypto=openssl \
	    --with-inotify=yes \
	    --with-systemd-daemon=no \
	    --with-kqueue=no
