EXEC = reg
OBJS = reg.o

CFLAGS += -I$(LINUXDIR)/drivers/net/rt_rdm

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

romfs:
	$(ROMFSINST) -S /bin/$(EXEC)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o *.map
