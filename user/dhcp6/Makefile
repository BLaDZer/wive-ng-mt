####################################################
#	workaround for build wide dhcp		   #
#	this fedore patched version 2014-01-16     #
####################################################
CFLAGS	+= -Os -pipe -fpack-struct -Wall -Wno-unused -fno-caller-saves -fno-reorder-blocks -fno-aggressive-loop-optimizations
CFLAGS	+= -ffunction-sections -fdata-sections
LDFLAGS += -Wl,--gc-sections

all:
	@set -e;
	cd wide-dhcpv6 && make -j1

clean:
	@set -e;
	cd wide-dhcpv6 && make clean &&	make distclean

romfs:
	@set -e;
	cd wide-dhcpv6 && make romfs
