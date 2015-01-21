EXEC = mii_mgr
OBJS = mii_mgr.o

CFLAGS += -I$(LINUXDIR) -I$(ROOTDIR)/lib/shared/include
CFLAGS += -I$(LINUXDIR)/drivers/net/raeth

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

romfs:
	$(ROMFSINST) -S /bin/$(EXEC)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
