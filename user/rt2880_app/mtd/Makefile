-include $(LINUX_CONFIG)
-include $(CONFIG_CONFIG)
-include $(ARCH_CONFIG)
-include $(VERSION_CONFIG)

ifdef CONFIG_USER_CWMPD
CFLAGS += -DCWMPDSP
endif

EXEC = mtd_write butcheck

# mtd_write must be static include libgcc
LDFLAGS += -static -static-libgcc

all: $(EXEC)

$(EXEC): $(EXEC).c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $@.c led.c $(LDLIBS)

romfs:
	$(ROMFSINST) -S /bin/mtd_write
	$(ROMFSINST) -S /bin/butcheck

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
