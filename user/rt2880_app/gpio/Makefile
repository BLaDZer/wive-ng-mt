EXEC = gpio

all: $(EXEC)

$(EXEC): $(EXEC).c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $@.c $(LDLIBS)

romfs:
	$(ROMFSINST) -S /bin/gpio

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
