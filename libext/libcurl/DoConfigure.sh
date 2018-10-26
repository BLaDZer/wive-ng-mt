#!/bin/bash

echo "=====================CONFIGURE-LIBCURL===================="
APROOTDIR=`pwd`
BACKUPCFLAGS=$CFLAGS
BACKUPLDFLAGS=$LDFLAGS
LIBDIR=$FIRMROOT/lib/shared
LIBS=$LIBDIR
INCLUDES=$LIBDIR/include

HBUILD=`uname -m`-pc-linux-gnu
HTARGET=mipsel-linux

CFLAGS="$BACKUPCFLAGS -I$INCLUDES -I$INCLUDES/openssl"
CPPFLAGS="$BACKUPCFLAGS -I$INCLUDES -I$INCLUDES/openssl"
LDFLAGS="$BACKUPLDFLAGS -L$LIBS/lib"

# prefer use bash if multishell
if [ -e /bin/bash ]; then
    SHELL="/bin/bash"
else
    SHELL="/bin/sh"
fi

export CFLAGS LDFLAGS CPPFLAGS SHELL

if [ ! -f $APROOTDIR/configure ]; then
    aclocal
    autoreconf -fi
    autoconf
fi
if [ ! -f $APROOTDIR/Makefile.in ] || [ ! -f $APROOTDIR/missing -a ! -f $APROOTDIR/build-aux/missing ]; then
    automake --add-missing --force-missing --copy
fi

CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD"
CONFOPTS="$CONFOPTS --enable-shared --disable-static"
CONFOPTS="$CONFOPTS --with-ssl=$LIBS --with-zlib=$LIBS"
CONFOPTS="$CONFOPTS --without-random --without-winssl --without-darwinssl --without-gnutls --without-polarssl --without-mbedtls --without-cyassl"
CONFOPTS="$CONFOPTS --without-nss --without-axtls --without-ca-bundle --without-ca-path --without-ca-fallback --without-libpsl --without-libmetalink"
CONFOPTS="$CONFOPTS --without-libssh2 --without-librtmp --without-winidn --without-libidn --without-nghttp2 --without-zsh-functions-dir"
CONFOPTS="$CONFOPTS --disable-ldap --disable-ldaps --disable-rtsp --disable-dict --disable-telnet --disable-tftp --disable-pop3"
CONFOPTS="$CONFOPTS --disable-imap --disable-smb --disable-smtp --disable-gopher"
CONFOPTS="$CONFOPTS --disable-debug --disable-curldebug --disable-manual --disable-dependency-tracking --disable-verbose"

CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem"

./configure $CONFOPTS
