UCLINUX_BUILD_USER=1
-include $(LINUX_CONFIG)
-include $(CONFIG_CONFIG)
-include $(ARCH_CONFIG)

CFLAGS += $(SSP_ALL_CFLAGS) -fPIC -D_LARGEFILE64_SOURCE=1

LIBWIVE=libwive.a
LIBWIVE_SHARED=libwive.so

OBJS	:= libwive_wireless.o libwive_ethernet.o libwive_network.o libwive_gpio.o libwive_procps.o libwive_sysstat.o libwive_helpers.o
HEADERS := $(ROOTDIR)/lib/shared/include/linux/autoconf.h
LDLIBS  :=

all: shared

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LDLIBS)

$(LIBWIVE) ar-target: $(OBJS)
	$(AR) $(ARFLAGS) $(LIBWIVE) $(OBJS)

shared: $(LIBWIVE)
	$(LD) -shared -soname=$(LIBWIVE_SHARED) -o $(LIBWIVE_SHARED) --whole-archive $(LIBWIVE) --no-whole-archive -lc;
	cp -frv $(LIBWIVE_SHARED) $(LIBCDIRSHARED)/lib

$(OBJS): $(HEADERS) Makefile

clean:
	$(RM) *.[oa] *.so
