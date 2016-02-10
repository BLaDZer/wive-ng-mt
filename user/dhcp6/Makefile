####################################################
#	workaround for build wide dhcp		   #
####################################################

CFLAGS	+= -pipe -fpack-struct
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
