#!/bin/sh

CURDIR=`pwd`

KERNELHDRS=kernel-headers
BINUTILVER=binutils-2.24
UCLIBCVER=uClibc-0.9.33.2
GCCVER=gcc-4.8.5
HOSTGCCVER=`gcc -dumpversion | cut -f -2 -d .`

INSTALL_DEP=NO
UNPACK=YES
HEADERS=YES
BINUTILS=YES
GCC=YES
UCLIB=YES
GCCCPP=YES

CPU_OVERLOAD=4

export LANGUAGE=en_EN.UTF-8:en

export LC_PAPER=en_EN.UTF-8
export LC_ADDRESS=en_EN.UTF-8
export LC_MONETARY=en_EN.UTF-8
export LC_TELEPHONE=en_EN.UTF-8
export LC_IDENTIFICATION=en_EN.UTF-8
export LC_MEASUREMENT=en_EN.UTF-8
export LC_NAME=en_EN.UTF-8

export LANG=C
export LC_COLLATE=C
export LC_MESSAGES=C
export LC_ALL=C

export LC_NUMERIC=
export LC_CTYPE=
export LC_TIME=

export WCURDIR=$CURDIR/tmp
export TARGET=mipsel-linux-uclibc
export PREFIX=$CURDIR
export ROOTCURDIR=$CURDIR

export TARGET_CURDIR=$WCURDIR/$TARGET-toolchain
export KERNEL_HEADERS=$TARGET_CURDIR/include
export PATH="${PATH}:${PREFIX}/bin:${PREFIX}/lib"
export CC=gcc

#install need lib`s and headers
if [ "$INSTALL_DEP" = "YES" ]; then
    ISOPENMANDRIVA=`grep OpenMandriva -i -c < /etc/issue`
    ISROSA=`grep ROSA -i -c < /etc/issue`
    ISMAGEIA=`grep Mageia -i -c < /etc/issue`
    ISUBUNTU=`grep Ubuntu -i -c < /etc/issue`
    if [ "$ISOPENMANDRIVA" = "1" ] || [ "$ISROSA" = "1" ]; then
	urpmi --auto -a flex --download-all --allow-force
	urpmi --auto -a make --download-all --allow-force
	urpmi --auto -ay gcc --download-all --allow-force
	urpmi --auto -ay glibc --download-all --allow-force
	urpmi --auto -ay bison --download-all --allow-force
	urpmi --auto -ay libtool --download-all --allow-force
	urpmi --auto -ay libgmpxx-devel --download-all --allow-force
	urpmi --auto -ay libmpc --download-all --allow-force
	urpmi --auto -ay mpfr --download-all --allow-force
	urpmi --auto -ay gcc-gfortran --download-all --allow-force
	urpmi --auto -ay texinfo --download-all --allow-force
	urpmi --auto -ay intltool --download-all --allow-force
	urpmi --auto -ay fakeroot --download-all --allow-force
	urpmi --auto -ay gperf --download-all --allow-force
	urpmi --auto -ay gengetopt --download-all --allow-force
	urpmi --auto -ay vim-enhanced --download-all --allow-force
	urpmi --auto -ay zip --download-all --allow-force
	urpmi --auto -ay pkg-config --download-all --allow-force
    elif [ "$ISMAGEIA" = "1" ]; then
	urpmi --auto bc --download-all
	urpmi --auto flex --download-all
	urpmi --auto make --download-all
	urpmi --auto gcc --download-all
	urpmi --auto libmpc3 --download-all
	urpmi --auto glibc-devel --download-all
	urpmi --auto binutils-devel --download-all
	urpmi --auto bison --download-all
	urpmi --auto libtool-devel --download-all
	urpmi --auto autoconf --download-all
	urpmi --auto automake --download-all
	urpmi --auto libgmpxx-devel --download-all
	urpmi --auto libmpc-devel --download-all
	urpmi --auto gcc-cpp --download-all
	urpmi --auto gcc-c++ --download-all
	urpmi --auto gcc-gfortran --download-all
	urpmi --auto gcc-plugins --download-all
	urpmi --auto colorgcc --download-all
	urpmi --auto texinfo --download-all
	urpmi --auto gettext --download-all
	urpmi --auto intltool --download-all
	urpmi --auto fakeroot --download-all
	urpmi --auto gperf --download-all
	urpmi --auto gengetopt --download-all
	urpmi --auto vim-enhanced --download-all
	urpmi --auto zip --download-all
	urpmi --auto pkg-config --download-all
    elif [ "$ISUBUNTU" = "1" ]; then
	sudo apt-get update
	sudo apt-get -y install build-essential
	sudo apt-get -y install flex
	sudo apt-get -y install gcc
	sudo apt-get -y install libc6
	sudo apt-get -y install bison
	sudo apt-get -y install libtool
	sudo apt-get -y install libgmp-dev
	sudo apt-get -y install libmpc-dev
	sudo apt-get -y install libmpfr-dev
	sudo apt-get -y install texinfo
	sudo apt-get -y install intltool
	sudo apt-get -y install fakeroot
	sudo apt-get -y install gperf
	sudo apt-get -y install zlib1g-dev	# for build tools
	sudo apt-get -y install gcc-multilib	# for build tools
	sudo apt-get -y install gawk		# for build tools
	sudo apt-get -y install autopoint	# for build davfs
	sudo apt-get -y install gengetopt	# for build chillispot
	sudo apt-get -y install vim-enhanced	# for build uboot
	sudo apt-get -y install zip		# for build tools
	sudo apt-get -y install pkg-config	# for build tools
    else
	urpmi --auto -a flex --download-all --allow-force
	urpmi --auto -a make --download-all --allow-force
	urpmi --auto -a gcc- --download-all --allow-force
	urpmi --auto -a glibc- --download-all --allow-force
	urpmi --auto -a bison- --download-all --allow-force
	urpmi --auto -a libtool- --download-all --allow-force
	urpmi --auto -a libgmpxx-devel --download-all --allow-force
	urpmi --auto -a libmpc- --download-all --allow-force
	urpmi --auto -a mpfr- --download-all --allow-force
	urpmi --auto -a gcc-gfortran --download-all --allow-force
	urpmi --auto -a texinfo- --download-all --allow-force
	urpmi --auto -a intltool- --download-all --allow-force
	urpmi --auto -a fakeroot- --download-all --allow-force
	urpmi --auto -a gperf- --download-all --allow-force
	urpmi --auto -a gengetopt- --download-all --allow-force
	urpmi --auto -a vim-enhanced- --download-all --allow-force
	urpmi --auto -a zip- --download-all --allow-force
	urpmi --auto -a pkg-config- --download-all --allow-force
    fi
fi

mkdir -p $WCURDIR

cd $WCURDIR
mkdir -p ${TARGET}-toolchain  && cd ${TARGET}-toolchain

#################################################TUNE FOR CURRENT VERSION GCC BUILD##########################################################
# supress some warnings
WARN_OPTS="-Wno-pointer-sign -Wno-unused-but-set-variable -Wno-trigraphs -Wno-format-security -Wno-long-long -Wno-sizeof-pointer-memaccess"
# disable some optimizations
GCC_OPTS="-fno-strict-overflow -fno-delete-null-pointer-checks -fno-var-tracking-assignments"
if [ "$HOSTGCCVER" = "4.8" ] ||  [ "$HOSTGCCVER" = "4.9" ]; then
    GCC_OPTS="$GCC_OPTS -fno-aggressive-loop-optimizations -fno-tree-slsr -fno-var-tracking-assignments"
fi
export CFLAGS="-O2 $GCC_OPTS $WARN_OPTS"
# configure toolchain
EXT_OPT="--disable-lto --disable-gold --enable-ld=yes --enable-gold=no --disable-sanity-checks --disable-werror --disable-nls"
EXT_OPT="$EXT_OPT --disable-biendian --disable-softfloat --disable-libquadmath --disable-libquadmath-support --disable-libmudflap"
EXT_OPT="$EXT_OPT --disable-libssp --disable-libgomp --disable-threads --with-gnu-ld --with-gnu-as"
if [ "$GCCVER" = "gcc-4.8.5" ] || [ "$GCCVER" = "gcc-4.9.2" ]; then
    EXT_OPT="$EXT_OPT --disable-libatomic --with-pic"
fi
# for gcc < 4.9.2 need tls disable
if [ "$GCCVER" != "gcc-4.9.2" ]; then
    EXT_OPT="$EXT_OPT --disable-tls"
fi
export EXT_OPT="$EXT_OPT"
##############################################################################################################################################

if [ "$UNPACK" = "YES" ]; then
    echo "=================REMOVE-OLD-BUILD-TREE=================="
    rm -rf build-*
    rm -rf $CURDIR/bin
    rm -rf $CURDIR/lib
    rm -rf $CURDIR/lib64
    rm -rf $CURDIR/usr
    rm -rf $CURDIR/share
    rm -rf $CURDIR/libexec
    rm -rf $CURDIR/include
    rm -rf $CURDIR/mipsel-linux-uclibc
fi

if [ "$UNPACK" = "YES" ]; then
    echo "=================EXTRACT-KERNEL-HEADERS================="
    tar xjf $KERNELHDRS.tar.bz2
    echo "====================EXTRACT-BINUTILS===================="
    tar xjf $BINUTILVER.tar.bz2
    echo "=====================EXTRACT-UCLIBC====================="
    tar xjf $UCLIBCVER.tar.bz2
    echo "=======================EXTRACT-GCC======================"
    tar xjf $GCCVER.tar.bz2
fi

if [ "$HEADERS" = "YES" ]; then
    echo "=====================INSTALL-C-HEADERS===================="
    mkdir -p $CURDIR/usr
    rm -rf $CURDIR/usr/include
    cp -rf $KERNEL_HEADERS $CURDIR/usr
    ln -rsf $CURDIR/usr/include $CURDIR/include
fi
export KERNEL_HEADERS=$CURDIR/usr/include

if [ -f /proc/cpuinfo ]; then
    n=`grep -c processor /proc/cpuinfo`;
    if [ $n -gt 1 ]; then
	HOST_NCPU=`expr $n \* $CPU_OVERLOAD`;
    else
	HOST_NCPU=$n;
    fi;
else
    HOST_NCPU=1;
fi

if [ "$BINUTILS" = "YES" ]; then
    echo "=====================BUILD-BINUTILS====================="
    mkdir -p build-binutils && cd build-binutils
    (../$BINUTILVER/configure --target=$TARGET --prefix=$PREFIX --includedir=$KERNEL_HEADERS \
	--with-sysroot=$PREFIX --with-build-sysroot=$PREFIX $EXT_OPT \
	--enable-deterministic-archives --enable-plugins --disable-libssp --disable-libada \
	--disable-multilib --disable-werror --disable-lto --disable-nls && \
	make -j$HOST_NCPU KERNEL_HEADERS=$KERNEL_HEADERS && \
	make install) || exit 1
    cd ..
fi

if [ "$GCC" = "YES" ]; then
    echo "=====================BUILD-GCC-C========================"
    mkdir -p build-gcc-bootstrap && cd build-gcc-bootstrap
    (../$GCCVER/configure \
	--target=$TARGET --prefix=$PREFIX \
	--disable-shared \
	--with-sysroot=$PREFIX --with-headers=$KERNEL_HEADERS \
	--enable-version-specific-runtime-libs --enable-languages=c \
	$EXT_OPT && \
	make -j$HOST_NCPU && \
	make install) || exit 1
    cd ..
fi

if [ "$UCLIB" = "YES" ]; then
    echo "=====================BUILD-C-UCLIBC===================="
    cp -fv uclibc-config $UCLIBCVER/.config
    cd $UCLIBCVER
    (make oldconfig ROOTDIR=$CURDIR PREFIX=$CURDIR CROSS_COMPILER_PREFIX=$CURDIR/bin/mipsel-linux-uclibc- && \
	make -j$HOST_NCPU ROOTDIR=$CURDIR PREFIX=$CURDIR CROSS_COMPILER_PREFIX=$CURDIR/bin/mipsel-linux-uclibc- && \
	make install ROOTDIR=$CURDIR PREFIX=$CURDIR CROSS_COMPILER_PREFIX=$CURDIR/bin/mipsel-linux-uclibc-) || exit 1
    cd ..
fi

if [ "$GCCCPP" = "YES" ]; then
    echo "====================BUILD-GCC-CPP======================="
    mkdir -p build-gcc-bootstrap-cpp && cd build-gcc-bootstrap-cpp
    (../$GCCVER/configure \
	--target=$TARGET --prefix=$PREFIX \
	--with-sysroot=$PREFIX --with-headers=$KERNEL_HEADERS \
	--enable-version-specific-runtime-libs --enable-languages=c++ \
	$EXT_OPT && \
	make -j$HOST_NCPU all-host all-target-libgcc all-target-libstdc++-v3  && \
	make install-host install-target-libgcc install-target-libstdc++-v3) || exit 1
    cd ..
fi

echo "====================All IS DONE!========================"
