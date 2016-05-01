#!/bin/sh

RO_ROOT=romfs
TOOLSPREFIX=./toolchain/bin/mipsel-linux-uclibc
STRIPOPT="-R .note -R .comment -R .pdr -R .mdebug.abi32 -R .note.gnu.build-id -R .gnu.attributes -R .reginfo -g --strip-unneeded"
STRIP="$TOOLSPREFIX-strip $STRIPOPT"
OBJCOPY="$TOOLSPREFIX-objcopy $STRIPOPT"
SSTRIP=./tools/sstrip/sstrip

echo --------------------------------GENERATE CONFIG-----------------------------
. linux/.config
. config/.config

SCONFIG_SH=$RO_ROOT/etc/scripts/config.sh
:>"$SCONFIG_SH"

echo "Search for parameters used"
PARAMS=
tmp=
for dir in $RO_ROOT/etc/rc.d $RO_ROOT/etc/scripts
do
	tmp="$( grep -r -o "\$CONFIG_[^ \t\"]\+" "$dir"/* | sed 's/^.*$CONFIG_\(.\+\)$/\1/' | sort | uniq )"
	PARAMS="$PARAMS $tmp"
done

echo "Need variables: $PARAMS"

for i in $PARAMS; do
    var=$( eval "echo \$CONFIG_$i" )
    if [ $var ]; then
	    echo "Found CONFIG_$i"
	    echo "CONFIG_$i=$var" >> "$SCONFIG_SH"
    fi
done
chmod 777 "$SCONFIG_SH"

echo -------------------------------FIND FILES TO STRIP-----------------------------
NON_STRIPS_BIN=`find $RO_ROOT/bin -type f -print -exec file {} \; | grep -vE "modules|icon|start|rc|\.sh|\.dat" | cut -d":" -f1`
NON_STRIPS_LIB=`find $RO_ROOT/lib -type f -print -exec file {} \; | grep -vE "modules|libc.so|libpthread.so|\.a|\.la|\.pc|\.dat" | cut -d":" -f1`
echo -----------------------------------STRIP BIN----------------------------------
for i in $NON_STRIPS_BIN; do
    echo $i;
    $OBJCOPY $i $i
done
if [ -n "$NON_STRIPS_BIN" ]; then
    echo BIN: $NON_STRIPS_BIN
    $STRIP $NON_STRIPS_BIN
    $SSTRIP $NON_STRIPS_BIN
fi
echo -----------------------------------STRIP LIB----------------------------------
for i in $NON_STRIPS_LIB; do
    echo $i;
    $OBJCOPY $i $i
done
if [ "$NON_STRIPS_LIB" != "" ]; then
    echo LIB: $NON_STRIPS_LIB
    $STRIP $NON_STRIPS_LIB
    $SSTRIP $NON_STRIPS_LIB
fi
echo -----------------------------------STRIP MODULES----------------------------------
find $RO_ROOT/lib/modules -type f -name '*.ko' | xargs -r $STRIP $STRIPOPT
find $RO_ROOT/lib/modules -type f -name '*.ko' -print -print | xargs -n2 -r $OBJCOPY $STRIPOPT
echo -----------------------------------SYNC!!-------------------------------------
sync
echo ----------------------------APP STRIP AND COPY OK-----------------------------
