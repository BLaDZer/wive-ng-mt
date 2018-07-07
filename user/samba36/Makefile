.EXPORT_ALL_VARIABLES:

-include $(CONFIG_CONFIG)

SRC_NAME 	:= source3
THISDIR  	:= $(shell pwd)

CFLAGS    	+= -ffunction-sections -fdata-sections -I$(THISDIR)/source3/include

CXXFLAGS  	+= $(CFLAGS)
LDFLAGS   	+= -Wl,--gc-sections

SMBCFLAGS	+= -Wall $(CFLAGS) -DMAX_DEBUG_LEVEL="0"
SMBLDFLAGS	+= $(LDFLAGS)

all:
	cd $(SRC_NAME) && make -j1

install:
	#DO NOT MAKE INSTALL NEED!!! Run make install may be crashed you system!

clean:
	make -C $(SRC_NAME) clean ; \
	if [ -f $(SRC_NAME)/Makefile ] ; then \
		make -C $(SRC_NAME) distclean ; \
	fi ;

distclean: clean
	@find $(SRC_NAME) -name config.h | xargs rm -f
	@find $(SRC_NAME) -name Makefile | xargs rm -f
	@find $(SRC_NAME) -name configure | xargs rm -f
	@find $(SRC_NAME) -name config.status | xargs rm -f
	@find $(SRC_NAME) -name config.cache | xargs rm -f

romfs:
	$(ROMFSINST) $(SRC_NAME)/bin/samba_multicall /sbin/samba_multicall
	$(ROMFSINST) -s samba_multicall /sbin/smbd
	$(ROMFSINST) -s samba_multicall /sbin/nmbd
	$(ROMFSINST) -s samba_multicall /sbin/smbpasswd
	mkdir -p $(ROMFSDIR)/usr/lib
	mkdir -p $(ROMFSDIR)/usr/lib/samba
	cp -f codepages/*.dat $(ROMFSDIR)/usr/lib/samba
	$(ROMFSINST) -S smb.conf.template       /etc/smb.conf.template
