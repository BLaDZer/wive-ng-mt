-include $(LINUX_CONFIG)
-include $(CONFIG_CONFIG)
-include $(ARCH_CONFIG)

PWD	  := $(shell pwd)
DIR	  := $(PWD)/coova-chilli-master

CONFOPTS  := --host=mipsel-linux --enable-shared --disable-static --with-poll --without-mmap --without-nfcoova --prefix=$(DIR)/filesystem
CONFOPTS  += --disable-tap --disable-largelimits --disable-miniportal --disable-json
CONFOPTS  += -enable-sessionstate --enable-sessionid --enable-apsessionid --enable-miniconfig --enable-chilliredir --enable-chilliproxy

#DEBUG     := y
ifdef DEBUG
CONFOPTS  +=  --enable-debug --enable-debug2
else
CONFOPTS  +=  --disable-debug --disable-debug2
endif

ifndef CONFIG_IPV6
CONFOPTS  += --without-ipv6
endif

ifdef CONFIG_IP_NF_QUEUE
CONFOPTS  += --with-nfqueue
endif

ifdef CONFIG_LIB_OPENSSL
CONFOPTS  += --with-openssl -enable-chilliradsec
endif

ifdef CONFIG_LIB_PCAP
CONFOPTS  += --with-pcap
endif

ifdef CONFIG_LIB_CURL
CONFOPTS  += --with-curl
endif

all:
	(cd $(DIR) ; if [ ! -f ./configure ]; then  aclocal; autoreconf -fi;  autoconf; fi ; \
	if [ ! -f ./Makefile ]; then  automake --add-missing --force-missing --copy; fi ; \
	./configure $(CONFOPTS) && $(MAKE) -j$(HOST_NCPU) DESTDIR=$(DIR)/filesystem && $(MAKE) install)

romfs:
	$(ROMFSINST) -S init.d/W88chillispot /etc/rc.d/W88chillispot
	$(ROMFSINST) -s /etc/rc.d/W88chillispot /etc/init.d/chillispot
	cp -fva $(DIR)/filesystem/lib/*.so*  $(ROMFSDIR)/lib
	cp -fva $(DIR)/filesystem/sbin/*  $(ROMFSDIR)/sbin

clean:
	-make clean -C $(DIR)
	rm -rf $(DIR)/filesystem

distclean:
	-make distclean -C $(DIR)
	rm -rf $(DIR)/filesystem
