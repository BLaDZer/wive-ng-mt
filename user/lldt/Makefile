-include $(LINUX_CONFIG)

VPATH = src

CFLAGS += -Os -pipe -fpack-struct -Wall -Wno-unused -fno-caller-saves -fno-reorder-blocks -fno-aggressive-loop-optimizations
CFLAGS += -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -I$(LINUXDIR) -I$(ROOTDIR)/lib/shared/include

LDFLAGS += -Wl,--gc-sections

ifdef CONFIG_IPV6
CFLAGS += -DUSE_IPV6
endif

LD= $(CC)

CONF_H = $(ROOTDIR)/lib/shared/include/linux/autoconf.h
OS_LAYER = osl-linux.c

all: lld2d lld2test

include src/common.mk

romfs:
	$(ROMFSINST) -S init.d/W80lld2d /etc/rc.d/W80lld2d
	$(ROMFSINST) -s /etc/rc.d/W80lld2d /etc/init.d/lld2d
	$(ROMFSINST) -S -e CONFIG_USER_LLTD	/bin/lld2d
	$(ROMFSINST) -S -e CONFIG_USER_LLTD	/bin/icon.ico
	$(ROMFSINST) -S -e CONFIG_USER_LLTD	/bin/icon.large.ico
	$(ROMFSINST) -e CONFIG_USER_LLTD lld2d.conf /etc/lld2d.conf
