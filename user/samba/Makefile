.EXPORT_ALL_VARIABLES:

-include $(CONFIG_CONFIG)

CFLAGS    	+= -ffunction-sections -fdata-sections
ifndef CONFIG_USER_STORAGE
CFLAGS    	+= -Os -fno-caller-saves -fno-unwind-tables -fno-asynchronous-unwind-tables
endif

CXXFLAGS  	+= $(CFLAGS)
LDFLAGS   	+= -Wl,--gc-sections

SMBCFLAGS	+= -Wall $(CFLAGS) -DMAX_DEBUG_LEVEL="0"
SMBLDFLAGS	+= $(LDFLAGS)

DIRS = source

all:
	for i in $(DIRS) ; do make -C $$i || exit $? ; done

install:
	#DO NOT MAKE INSTALL NEED!!! Run make install may be crashed you system!

romfs:
	for i in $(DIRS) ; do make -C $$i romfs || exit $? ; done
	$(ROMFSINST) smb.conf.template /etc/smb.conf.template
	$(ROMFSINST) -s /bin/smbd /bin/nmbd

clean:
	-for i in $(DIRS) ; do make -C $$i clean ; done
