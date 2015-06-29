LDFLAGS	+= -lnvram
CFLAGS	+= -I$(ROOTDIR)/user/goahead/src

all: nvram

nvram: nvram.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -f *.o nvram

romfs:
	$(ROMFSINST) -S /bin/nvram
	$(ROMFSINST) -s /bin/nvram /bin/nvram_get
	$(ROMFSINST) -s /bin/nvram /bin/nvram_buf_get
	$(ROMFSINST) -s /bin/nvram /bin/nvram_set
	$(ROMFSINST) -s /bin/nvram /bin/nvram_show
	$(ROMFSINST) -s /bin/nvram /bin/nvram_default
	$(ROMFSINST) -s /bin/nvram /bin/nvram_clear
	$(ROMFSINST) -s /bin/nvram /bin/nvram_renew
	$(ROMFSINST) -s /bin/nvram /bin/nvram_genwlconfig
