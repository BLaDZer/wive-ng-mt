#!/bin/bash

echo "==================CONFIGURE-FREERADIUS=============================="
APROOTDIR=`pwd`

HBUILD=`uname -m`-pc-linux-gnu
HTARGET=mipsel-linux

if [ ! -f Makefile ]; then
    cp -f Makefile.template Makefile
fi
if [ ! -f $APROOTDIR/configure ]; then
    aclocal
    autoreconf -fi
    autoconf
fi

CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD"
CONFOPTS="$CONFOPTS --prefix=$APROOTDIR/filesystem --disable-openssl-version-check --disable-developer --enable-shared --disable-static"
CONFOPTS="$CONFOPTS --without-system-libtool --without-system-libltdl"
CONFOPTS="$CONFOPTS --with-threads --with-rlm_eap --with-rlm_mschap  --with-rlm_always --with-rlm_attr_filter --with-rlm_files --with-udpfromto"
CONFOPTS="$CONFOPTS --without-rlm_eap_tnc --without-rlm_eap_pwd --without-rlm_eap_ikev2 --without-detail --without-rlm_pap --without-rlm_chap --without-rlm_passwd"
CONFOPTS="$CONFOPTS --without-openssl --without-pcre --without-regex --without-rlm_digest --without-rlm_expr --without-rlm_cache --without-rlm_cache_eap"
CONFOPTS="$CONFOPTS --without-rlm_linelog --without-rlm_realm --without-rlm_securid --without-rlm_sqlhpwippool --without-rlm_utf8"
CONFOPTS="$CONFOPTS --without-proto_dhcp --without-rlm_couchbase --without-rlm_dynamic_clients --without-rlm_logintime"
CONFOPTS="$CONFOPTS --without-rlm_redis --without-rlm_smsotp --without-rlm_sqlippool --without-rlm_wimax --without-proto_vmps --without-rlm_counter"
CONFOPTS="$CONFOPTS --without-rlm_idn --without-rlm_perl --without-rlm_rediswho --without-rlm_soh --without-rlm_test --without-rlm_yubikey"
CONFOPTS="$CONFOPTS --without-rlm_cram --without-rlm_example --without-rlm_ippool --without-rlm_opendirectory --without-rlm_preprocess --without-rlm_replicate"
CONFOPTS="$CONFOPTS --without-rlm_sometimes --without-rlm_unbound --without-rlm_date --without-rlm_exec --without-rlm_krb5 --without-rlm_otp"
CONFOPTS="$CONFOPTS --without-rlm_python --without-rlm_rest --without-rlm_sql --without-rlm_unix --without-rlm_expiration --without-rlm_ldap"
CONFOPTS="$CONFOPTS --without-rlm_pam --without-rlm_radutmp --without-rlm_ruby --without-rlm_sqlcounter --without-rlm_unpack"

./configure $CONFOPTS
