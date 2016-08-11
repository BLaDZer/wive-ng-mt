UCLINUX_BUILD_USER=1
-include $(LINUX_CONFIG)
-include $(CONFIG_CONFIG)
-include $(ARCH_CONFIG)

CFLAGS += $(SSP_ALL_CFLAGS) -fPIC -D_LARGEFILE64_SOURCE=1

# strong build
CFLAGS += -Wall -Wextra -Wshadow -Wformat=2 -Wfloat-equal -Wlogical-op -Wcast-qual -Wno-unused-parameter -Wno-sign-compare -Werror -Wfatal-errors

LIBNVRAM=libnvram.a
LIBNVRAM_SHARED=libnvram.so

OBJS	:= libnvram.o libnvram_utils.o
HEADERS := libnvram.h $(ROOTDIR)/lib/shared/include/linux/autoconf.h

all: shared

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBNVRAM) ar-target: $(OBJS)
	$(AR) $(ARFLAGS) $(LIBNVRAM) $(OBJS)

shared: $(LIBNVRAM)
	$(LD) -shared -soname=$(LIBNVRAM_SHARED) -o $(LIBNVRAM_SHARED) --whole-archive $(LIBNVRAM) --no-whole-archive -lc;
	cp -frv $(LIBNVRAM_SHARED) $(LIBCDIRSHARED)/lib

$(OBJS): $(HEADERS) Makefile

clean:
	$(RM) *.[oa] *.so
