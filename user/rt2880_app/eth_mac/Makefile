EXEC += eth_mac
OBJS += eth_mac.o

CFLAGS += -I$(LINUXDIR) -I$(ROOTDIR)/lib/shared/include -I$(ROOTDIR)/lib/shared/include/mtd

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

romfs:
	$(ROMFSINST) -S /bin/$(EXEC)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
