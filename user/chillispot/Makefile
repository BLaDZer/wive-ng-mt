.EXPORT_ALL_VARIABLES:

-include $(LINUX_CONFIG)
-include $(CONFIG_CONFIG)
-include $(ARCH_CONFIG)

PWD	  := $(shell pwd)
DIR	  := $(PWD)/coova-chilli-master

CONFOPTS  := --host=mipsel-linux --enable-shared --disable-static --with-gnu-ld --with-poll --without-mmap --without-pcap --prefix=$(DIR)/filesystem
CONFOPTS  += --disable-tap --disable-largelimits --disable-json --disable-miniportal --disable-miniconfig --disable-layer3 --disable-dhcpopt --disable-leakybucket
CONFOPTS  += --disable-multilan --disable-multiroute
CONFOPTS  += --enable-chilliproxy --enable-chilliredir --enable-chilliquery
CONFOPTS  += --enable-sessionstate --enable-sessionid --enable-apsessionid
CONFOPTS  += --enable-accounting-onoff --enable-redirdnsreq --enable-redirinject
CONFOPTS  += --enable-uamanyip --enable-uamuiport

# fix packet drops
CONFOPTS  += --enable-largelimits

#DEBUG     := y
ifdef DEBUG
CONFOPTS  += --enable-debug --enable-debug2
else
CONFOPTS  += --disable-debug --disable-debug2
endif

# ipv6 very very limited and unstable suppot in coova
#ifdef CONFIG_IPV6
#CONFOPTS  += --with-ipv6
#else
CONFOPTS  += --without-ipv6
#endif

ifdef CONFIG_IP_NF_QUEUE
CONFOPTS  += --with-nfqueue
else
CONFOPTS  += --without-nfqueue
endif

ifdef CONFIG_NETFILTER_XT_MATCH_COOVA
CONFOPTS  += --with-nfcoova
else
CONFOPTS  += --without-nfcoova
endif

ifdef CONFIG_LIB_OPENSSL
CONFOPTS  += --with-openssl --enable-chilliradsec
else
CONFOPTS  += --without-openssl --disable-chilliradsec
endif

# curl not need for regular build without miniportal
#ifdef CONFIG_LIB_CURL
#CONFOPTS  += --with-curl
#else
CONFOPTS  += --without-curl
#endif

all:
	(cd $(DIR) ; if [ ! -f ./configure ]; then  aclocal; autoreconf -fi;  autoconf; fi ; \
	if [ ! -f ./Makefile ]; then  automake --add-missing --force-missing --copy; fi ; \
	ac_cv_func_malloc_0_nonnull=yes \
	ac_cv_func_realloc_0_nonnull=yes \
	ac_cv_func_memcmp_working=yes \
	ac_cv_func_setvbuf_reversed=no \
	ac_cv_lib_resolv_res_init=yes \
	./configure $(CONFOPTS) && $(MAKE) -j$(HOST_NCPU) DESTDIR=$(DIR)/filesystem && $(MAKE) install)

romfs:
	$(ROMFSINST) -S chilli.conf /etc/chilli.conf
	$(ROMFSINST) -S ../init.d/W88chillispot /etc/rc.d/W88chillispot
	$(ROMFSINST) -s /etc/rc.d/W88chillispot /etc/init.d/chillispot
	cp -fva $(DIR)/filesystem/lib/*.so*  $(ROMFSDIR)/lib
	cp -fva $(DIR)/filesystem/sbin/*  $(ROMFSDIR)/sbin

clean:
	-make clean -C $(DIR)
	rm -rf $(DIR)/filesystem

distclean:
	-make distclean -C $(DIR)
	rm -rf $(DIR)/filesystem
