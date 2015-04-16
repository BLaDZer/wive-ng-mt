EXEC = gpio butcheck

all: $(EXEC)

$(EXEC): $(EXEC).c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $@.c $(LDLIBS)

romfs:
	$(ROMFSINST) -S /bin/gpio
	$(ROMFSINST) -S /bin/butcheck

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
