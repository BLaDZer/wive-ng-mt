EXEC = mtd_write
OBJS = led.o mtd.o

CFLAGS := -Os -I$(LINUXDIR) -I$(LINUXDIR)/include
CFLAGS += -fno-caller-saves -fno-unwind-tables -fno-asynchronous-unwind-tables
LDFLAGS := -static

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

romfs:
	$(ROMFSINST) -S /bin/$(EXEC)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
