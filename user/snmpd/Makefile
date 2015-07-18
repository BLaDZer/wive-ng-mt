CFLAGS    += -ffunction-sections -fdata-sections -Wno-maybe-uninitialized
CXXFLAGS  += -ffunction-sections -fdata-sections -Wno-maybe-uninitialized
LDFLAGS   += -Wl,--gc-sections

DIRS = snmplib modules snmpd

all romfs test:
	for i in $(DIRS) ; do make -C $$i $@ || exit $?; done

clean:
	for i in $(DIRS) ; do make -C $$i clean ; done
