EXEC = gpio butcheck

CFLAGS += -I$(LINUXDIR) -I$(ROOTDIR)/lib/shared/include
CONF_H  = -I$(ROOTDIR)/lib/shared/include/linux/autoconf.h

all: $(EXEC)

$(EXEC): $(EXEC).c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $@.c $(LDLIBS)

romfs:
	$(ROMFSINST) -S /bin/gpio
	$(ROMFSINST) -S /bin/butcheck

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
