EXEC = mtd_write
OBJS = led.o mtd.o

# mtd_write must be static include libgcc
LDFLAGS += -static -static-libgcc

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

romfs:
	$(ROMFSINST) -S /bin/$(EXEC)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
