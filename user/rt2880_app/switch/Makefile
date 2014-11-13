EXEC = switch

-include $(LINUX_CONFIG)

CFLAGS += -I$(LINUXDIR) -I$(LINUXDIR)/include
CFLAGS += -I$(LINUXDIR)/drivers/net/raeth
CONF_H  = $(LINUXDIR)/include/linux/autoconf.h

ifdef CONFIG_RALINK_MT7620
CFLAGS += -DCONFIG_RALINK_MT7620
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
