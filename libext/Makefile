#
#	Makefile -- Build instructions for user level apps
#

.EXPORT_ALL_VARIABLES:
.PHONY: all romfs clean

#
# Include architecture specific build rules.
#

ifndef ROOTDIR
ROOTDIR=..
endif

UCLINUX_BUILD_USER=1
-include $(LINUX_CONFIG)
-include $(CONFIG_CONFIG)
-include $(ARCH_CONFIG)

CFLAGS += -ffunction-sections -fdata-sections
LDFLAGS += -Wl,--gc-sections -Bsymbolic-functions

ifdef CONFIG_RALINK_MT7620
ifdef CONFIG_RAETH_ESW
CFLAGS += -DCONFIG_RAETH_ESW -DCONFIG_RALINK_MT7620
endif
endif

ifdef CONFIG_RALINK_MT7621
ifdef CONFIG_MT7530_GSW
CFLAGS += -DCONFIG_MT7530_GSW -DCONFIG_RALINK_MT7621
endif
endif

ifdef CONFIG_RALINK_MT7628
ifdef CONFIG_MT7530_GSW
CFLAGS += -DCONFIG_MT7530_GSW -DCONFIG_RALINK_MT7628
endif
endif

#for all cleanap
SUBDIRS_LIBS = $(shell LC_ALL=C; ls)

#
# must run the vendor build first
#

VEND=$(ROOTDIR)/vendors
dir_v = $(VEND)/$(CONFIG_VENDOR)/.

dir_y =
dir_n =
dir_  =

#base user libs
dir_$(CONFIG_LIB_ZLIB)            += zlib
dir_$(CONFIG_LIB_EVENT)           += libevent
dir_$(CONFIG_LIB_PCAP)            += libpcap
dir_$(CONFIG_LIB_USB1)            += libusb-1.0
dir_$(CONFIG_LIB_NATPMP)          += libnatpmp
dir_$(CONFIG_LIB_NFNETLINK)       += libnfnetlink
dir_$(CONFIG_LIB_NL)		  += libnl
dir_$(CONFIG_LIB_ICONV)           += libiconv
dir_$(CONFIG_LIB_XML2)            += libxml2
dir_$(CONFIG_LIB_NEON)            += libneon
dir_$(CONFIG_LIB_CURL)            += libcurl

dir_$(CONFIG_LIB_NEON)            := libneon

OPENSSL_CONFIG = RT2880 no-camellia no-capieng no-cms no-gmp no-jpake no-krb5 no-mdc2 no-rfc3779 no-seed \
                  no-ec no-ecdh no-ec2m no-ecdsa no-err no-threads no-idea no-rc2 no-rc5 \
                  no-hw no-md2 no-sse2 no-dso no-ts no-ripemd no-rmd160 no-sha0 \
                  no-smime no-ans1 no-cast no-whirlpool no-sctp no-srp no-jpake no-ssl2 no-ssl3 no-speed

all:
ifeq ($(CONFIG_LIB_OPENSSL),y)
	# ">>>>>>>>>>>>>>>> CONFIGURE AND BUILD SSL <<<<<<<<<<<<<<<<<<<<<<<"
	if [ ! -e openssl/libssl.so ]; then  cd openssl ; ./Configure $(OPENSSL_CONFIG) ; $(MAKE) -j1 || exit 1 ; cd .. ; fi
	if [ -e openssl/libssl.so ]; then  cp -fap openssl/*.so* $(LIBCDIRSHARED)/lib ; fi
	if [ -e openssl/libssl.so ]; then  $(RM) -rf $(ROOTDIR)/lib/shared/include/openssl ; cp -faprL openssl/include/* $(ROOTDIR)/lib/shared/include ; fi
endif
	# ">>>>>>>>>>>>>> CONFIGURE AND BUILD USERLIBS  <<<<<<<<<<<<<<<<<<<"
	for i in $(dir_y) ; do \
	    if [ -f "$$i/DoConfigure.sh" ]; then ( cd $$i; ./DoConfigure.sh || exit 1 ; cd .. ) || exit 1 ; fi ; \
	    if [ -d "$$i" ]; then ( cd $$i; $(MAKE) -j$(HOST_NCPU) ; $(MAKE) shared || exit 1 ; cd .. ) || exit 1 ; fi \
	done

clean:
	# ">>>>>>>>>>>>>>>>> START FULL LIB CLEANUP  <<<<<<<<<<<<<<<<<<<<<<"
	-cd openssl ; $(MAKE) clean ; $(MAKE) dclean ; cd ..
	-for i in $(SUBDIRS_LIBS); do $(MAKE) -C $$i clean; $(RM) -rf $(ROOTDIR)/user/$$i/filesystem/*; done
	-for i in $(SUBDIRS_LIBS); do $(MAKE) -C $$i distclean; $(RM) -rf $(ROOTDIR)/user/$$i/filesystem/*; done
