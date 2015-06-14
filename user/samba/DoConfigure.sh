#!/bin/bash

echo "==================CONFIGURE-SAMBA3=============================="
APROOTDIR=`pwd`

cd $APROOTDIR/source
HBUILD=`uname -m`-pc-linux-gnu
HTARGET=mipsel-linux

if [ ! -f configure ]; then
    sh ./autogen.sh
fi

#arch options
CONFOPTS="--host=$HTARGET --target=$HTARGET --build=$HBUILD"
#some function direct enable
CONFOPTS="$CONFOPTS --with-syslog --enable-largefile"
#disable some function
CONFOPTS="$CONFOPTS --disable-iprint --disable-pie --disable-fam --disable-cups --disable-debug --disable-static"
CONFOPTS="$CONFOPTS --disable-dmalloc --disable-iprint"
CONFOPTS="$CONFOPTS --disable-developer --disable-krb5developer"
CONFOPTS="$CONFOPTS --without-ldap --without-ads --without-cifsmount"
CONFOPTS="$CONFOPTS --without-cifsupcall --without-sys-quotas --without-quotas"
CONFOPTS="$CONFOPTS --without-utmp --without-cluster-support"
CONFOPTS="$CONFOPTS --without-acl-support --without-automount --with-sendfile-support"
CONFOPTS="$CONFOPTS --without-libaddns"
CONFOPTS="$CONFOPTS --without-libsmbclient --without-winbind --without-libsmbsharemodes --with-included-popt=no"
#path options
CONFOPTS="$CONFOPTS --with-logfilebase=/var/log --with-piddir=/var/lock --with-lockdir=/var/lock --sysconfdir=/etc --prefix=/"

SMB_BUILD_CC_NEGATIVE_ENUM_VALUES=yes \
ac_cv_type_long_long=yes \
fu_cv_sys_stat_statvfs64=yes \
samba_cv_USE_SETEUID=yes \
samba_cv_USE_SETREUID=yes \
samba_cv_USE_SETRESUID=no \
samba_cv_have_setreuid=yes \
samba_cv_have_setresuid=yes \
samba_cv_SIZEOF_INO_T=yes \
samba_cv_SIZEOF_OFF_T=yes \
samba_cv_have_longlong=yes \
samba_cv_HAVE_C99_VSNPRINTF=yes \
samba_cv_HAVE_INO64_T=yes \
samba_cv_HAVE_OFF64_T=yes \
samba_cv_HAVE_BROKEN_FCNTL64_LOCKS=no \
samba_cv_HAVE_BROKEN_GETGROUPS=no \
samba_cv_HAVE_BROKEN_READDIR_NAME=no \
samba_cv_HAVE_BROKEN_LINUX_SENDFILE=no \
samba_cv_HAVE_SENDFILE=yes \
samba_cv_HAVE_FTRUNCATE_EXTEND=yes \
samba_cv_HAVE_IFACE_AIX=no \
samba_cv_HAVE_IFACE_IFCONF=yes \
samba_cv_HAVE_IFACE_IFREQ=yes \
samba_cv_HAVE_SECURE_MKSTEMP=yes \
samba_cv_HAVE_UNSIGNED_CHAR=yes \
samba_cv_HAVE_GETTIMEOFDAY_TZ=yes \
samba_cv_HAVE_FCNTL_LOCK=yes \
samba_cv_HAVE_STRUCT_FLOCK64=yes \
samba_cv_HAVE_MMAP=no \
samba_cv_HAVE_KERNEL_OPLOCKS_LINUX=yes \
samba_cv_HAVE_KERNEL_SHARE_MODES=yes \
samba_cv_HAVE_NATIVE_ICONV=yes \
samba_cv_REPLACE_READDIR=no \
samba_cv_REPLACE_INET_NTOA=no \
samba_cv_LINUX_LFS_SUPPORT=yes \
samba_cv_HAVE_WRFILE_KEYTAB=no \
samba_cv_zlib_1_2_3=no \
ac_cv_header_zlib_h=no \
ac_cv_path_PYTHON="" \
ac_cv_path_PYTHON_CONFIG="" \
CPPFLAGS="$CPPFLAGS -DNDEBUG -DSHMEM_SIZE=524288 -Dfcntl=fcntl64 -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE=1 -D_LARGEFILE64_SOURCE=1 -D_LARGE_FILES=1" \
./configure $CONFOPTS
