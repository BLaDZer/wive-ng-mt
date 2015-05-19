# use host target
CC        = gcc
GCC       = gcc
CXX       = g++
AS        = as
AR        = ar
LD        = ld
NM        = nm
OBJCOPY   = objcopy
RANLIB    = ranlib
STRIP     = strip
STRIPTOOL = $(STRIP)

CFLAGS    = -Wall -O2
LDFLAGS   =

#NUM MAKE PROCESS = CPU NUMBER IN THE SYSTEM * CPU_OVERLOAD
CPU_OVERLOAD	:= 4
HOST_NCPU	:= $(shell if [ -f /proc/cpuinfo ]; then n=`grep -c processor /proc/cpuinfo`; if [ $$n -gt 1 ];then expr $$n \* ${CPU_OVERLOAD}; else echo $$n; fi; else echo 1; fi)

-include $(LINUX_CONFIG)

all: install

install:
	mkdir -p bin
	make -j$(HOST_NCPU) -C mkimage
	make -j$(HOST_NCPU) -C sstrip
	make -j$(HOST_NCPU) -C mksquash_lzma-3.2 install
	make -j1 -C mksquash_xz-4.0 install

clean:
	rm -rf ./bin
	rm -rf ./headers
	make -C sstrip clean
	make -C mkimage clean
	make -C mksquash_lzma-3.2 clean
	make -C mksquash_xz-4.0 clean
	if [ ! -f xz/Makefile ]; then \
	    if [ -f xz/configure ]; then cd xz && ./configure && cd .. ; fi ; \
	    make -C xz distclean ; \
	fi ;

mrproper:
	echo "OK"
