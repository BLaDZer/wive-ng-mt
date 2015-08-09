-include $(LINUX_CONFIG)
-include $(CONFIG_CONFIG)

DIR	  := $(shell pwd)/coova-chilli-1.3.0

CONFOPTS  := --host=mipsel-linux --enable-shared--disable-static --prefix=$(DIR)/filesystem --with-poll
CONFOPTS  += --disable-tap --disable-debug --disable-largelimits --disable-miniportal --disable-json
CONFOPTS  += --enable-dhcpopt --enable-multilan --enable-multiroute -enable-sessionstate --enable-sessionid --enable-apsessionid
CONFOPTS  += --enable-miniconfig --enable-chilliredir --enable-chilliproxy --enable-config=/etc/chilli.conf

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

CFLAGS    += -ffunction-sections -fdata-sections -Wno-maybe-uninitialized
CXXFLAGS  += -ffunction-sections -fdata-sections -Wno-maybe-uninitialized
LDFLAGS   += -Wl,--gc-sections

all:
	(cd $(DIR) ; if [ ! -f ./configure ]; then  aclocal; autoreconf -fi;  autoconf; fi ; \
	if [ ! -f ./Makefile ]; then  automake --add-missing --force-missing --copy; fi ; \
	./configure $(CONFOPTS) && $(MAKE) -j$(HOST_NCPU) && $(MAKE) install)

romfs:
	$(ROMFSINST) -S init.d/W88chillispot /etc/rc.d/W88chillispot
	$(ROMFSINST) -s /etc/rc.d/W88chillispot /etc/init.d/chillispot
	cp -fva $(DIR)/filesystem/lib/*.so*  $(ROMFSDIR)/lib
	cp -fva $(DIR)/filesystem/sbin/*  $(ROMFSDIR)/sbin
	cp -fva $(DIR)/filesystem/etc/chilli/defaults  $(ROMFSDIR)/etc

clean:
	-make clean -C $(DIR)
	rm -rf $(DIR)/filesystem

distclean:
	-make distclean -C $(DIR)
	rm -rf $(DIR)/filesystem
