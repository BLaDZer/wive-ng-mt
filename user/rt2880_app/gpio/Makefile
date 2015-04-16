EXEC = gpio butcheck

-include $(LINUX_CONFIG)
CONF_H  = $(ROOTDIR)/lib/shared/include/linux/autoconf.h

all: $(EXEC)

$(EXEC): $(EXEC).c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $@.c $(LDLIBS)

romfs:
	$(ROMFSINST) -S /bin/gpio
	$(ROMFSINST) -S /bin/butcheck

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
