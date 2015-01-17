EXEC = mtd_write
OBJS = led.o mtd.o

CFLAGS := -Os -I$(LINUXDIR) -I$(ROOTDIR)/lib/shared/include -I$(ROOTDIR)/lib/shared/include/mtd
CFLAGS += -fno-caller-saves -fno-reorder-blocks -fno-unwind-tables -fno-asynchronous-unwind-tables
LDFLAGS := -static

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

romfs:
	$(ROMFSINST) -S /bin/$(EXEC)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
