CFLAGS    += -ffunction-sections -fdata-sections -Wno-maybe-uninitialized -fvisibility=hidden -I$(ROOTDIR)/libwive/ -I$(ROOTDIR)/libnvram/
CXXFLAGS  += -ffunction-sections -fdata-sections -Wno-maybe-uninitialized -fvisibility-inlines-hidden -I$(ROOTDIR)/libwive/ -I$(ROOTDIR)/libnvram/
LDFLAGS   += -Wl,--gc-sections -L$(ROOTDIR)/libwive/ -L$(ROOTDIR)/libnvram/
LDLIBS    += -lwive -lnvram -lcrypt

DIRS = snmplib modules snmpd

all romfs test:
	for i in $(DIRS) ; do make -C $$i $@ || exit $?; done

clean:
	for i in $(DIRS) ; do make -C $$i clean ; done
