-include $(LINUX_CONFIG)
-include $(CONFIG_CONFIG)

PWD	  := $(shell pwd)
DIR	  := $(PWD)/coova-chilli-1.3.0

CONFOPTS  := --host=mipsel-linux --enable-shared --disable-static --prefix=$(DIR)/filesystem --with-poll
CONFOPTS  += --disable-tap--disable-largelimits --disable-miniportal --disable-json --enable-debug  --enable-debug2
CONFOPTS  += --enable-dhcpopt -enable-sessionstate --enable-sessionid --enable-apsessionid
CONFOPTS  += --enable-miniconfig --enable-chilliredir --enable-chilliproxy

ifndef CONFIG_IPV6
CONFOPTS  += --without-ipv6
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
	cp -fva $(PWD)/chilli.conf $(ROMFSDIR)/etc

clean:
	-make clean -C $(DIR)
	rm -rf $(DIR)/filesystem

distclean:
	-make distclean -C $(DIR)
	rm -rf $(DIR)/filesystem
