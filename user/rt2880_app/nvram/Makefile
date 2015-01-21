NVDIR	 = $(ROOTDIR)/libnvram

CFLAGS	+= -I$(NVDIR)
CFLAGS	+= -I$(LINUXDIR) -I$(ROOTDIR)/lib/shared/include
LDFLAGS	+= -lnvram

all: ralink_init

ralink_init: ralink_init.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -f *.o ralink_init

romfs:
	$(ROMFSINST) -S /bin/ralink_init
	$(ROMFSINST) -s /bin/ralink_init /bin/nvram_get
	$(ROMFSINST) -s /bin/ralink_init /bin/nvram_buf_get
	$(ROMFSINST) -s /bin/ralink_init /bin/nvram_set
	$(ROMFSINST) -s /bin/ralink_init /bin/nvram_show
	$(ROMFSINST) -s /bin/ralink_init /bin/nvram_default
	$(ROMFSINST) -s /bin/ralink_init /bin/nvram_clear
