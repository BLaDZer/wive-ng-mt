EXEC = switch

-include $(LINUX_CONFIG)

CFLAGS += -I$(LINUXDIR) -I$(LINUXDIR)/include -I$(ROOTDIR)/lib/shared/include
CFLAGS += -I$(LINUXDIR)/drivers/net/raeth
CONF_H  = $(ROOTDIR)/lib/shared/include/linux/autoconf.h

ifdef CONFIG_RALINK_MT7620
ifdef CONFIG_RAETH_ESW
CFLAGS += -DCONFIG_RAETH_ESW -DCONFIG_RALINK_MT7620
endif
endif

ifdef CONFIG_RALINK_MT7621
ifdef CONFIG_MT7530_GSW
CFLAGS += -DCONFIG_MT7530_GSW -DCONFIG_RALINK_MT7621
endif
endif

ifdef CONFIG_RALINK_MT7628
ifdef CONFIG_MT7530_GSW
CFLAGS += -DCONFIG_MT7530_GSW -DCONFIG_RALINK_MT7628
endif
endif

SRC=switch.c

all: $(EXEC)
switch: $(SRC) $(CONF_H)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRC) $(LDLIBS)

$(EXEC): $(EXEC).c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $@.c $(LDLIBS)

romfs:
	$(ROMFSINST) -S /bin/$(EXEC)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
