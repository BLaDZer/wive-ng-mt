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

ifdef CONFIG_RALINK_RT3052
ifdef CONFIG_RAETH_ESW
CFLAGS += -DCONFIG_RAETH_ESW -DCONFIG_RALINK_RT3052
endif
endif

ifdef CONFIG_RALINK_RT3352
ifdef CONFIG_RAETH_ESW
CFLAGS += -DCONFIG_RAETH_ESW -DCONFIG_RALINK_RT3352
endif
endif

ifdef CONFIG_RALINK_RT5350
ifdef CONFIG_RAETH_ESW
CFLAGS += -DCONFIG_RAETH_ESW -DCONFIG_RALINK_RT5350
endif
endif

ifdef CONFIG_RALINK_RT6855
ifdef CONFIG_RAETH_ESW
CFLAGS += -DCONFIG_RAETH_ESW -DCONFIG_RALINK_RT6855
endif
endif

ifdef CONFIG_RALINK_RT6855A
ifdef CONFIG_RAETH_ESW
CFLAGS += -DCONFIG_RAETH_ESW -DCONFIG_RALINK_RT6855A
endif
endif

ifdef CONFIG_RALINK_MT7620
ifdef CONFIG_RAETH_ESW
CFLAGS += -DCONFIG_RAETH_ESW -DCONFIG_RALINK_MT7620
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
dir_$(CONFIG_LIB_UPNP)            += libupnp
dir_$(CONFIG_LIB_USB1)            += libusb-1.0
dir_$(CONFIG_LIB_ICONV)           += libiconv
dir_$(CONFIG_LIB_NATPMP)          += libnatpmp
dir_$(CONFIG_LIB_NFNETLINK)       += libnfnetlink
dir_$(CONFIG_LIB_NL)		  += libnl
dir_$(CONFIG_LIB_CURL)            += libcurl

all:
	CC=$(CONFIG_CROSS_COMPILER_PATH)/mipsel-linux-uclibc-gcc ; \
	GCC=$(CONFIG_CROSS_COMPILER_PATH)/mipsel-linux-uclibc-gcc ; \
	CXX=$(CONFIG_CROSS_COMPILER_PATH)/mipsel-linux-uclibc-g++ ; \
	AR=$(CONFIG_CROSS_COMPILER_PATH)/mipsel-linux-uclibc-ar ; \
	AS=$(CONFIG_CROSS_COMPILER_PATH)/mipsel-linux-uclibc-as ; \
	LD=$(CONFIG_CROSS_COMPILER_PATH)/mipsel-linux-uclibc-ld ; \
	NM=$(CONFIG_CROSS_COMPILER_PATH)/mipsel-linux-uclibc-nm ; \
	RANLIB=$(CONFIG_CROSS_COMPILER_PATH)/mipsel-linux-uclibc-ranlib ; \
	STRIP=$(CONFIG_CROSS_COMPILER_PATH)/mipsel-linux-uclibc-strip ;
ifeq ($(CONFIG_LIB_OPENSSL),y)
	# ">>>>>>>>>>>>>>>> CONFIGURE AND BUILD SSL <<<<<<<<<<<<<<<<<<<<<<<"
	if [ ! -e openssl/libssl.so ]; then  cd openssl ; ./Configure RT2880 no-ripemd; $(MAKE) -j1 || exit 1 ; cd .. ; fi
	if [ -e openssl/libssl.so ]; then  cp -fap openssl/*.so* $(LIBCDIRSHARED)/lib ; cp -fapv openssl/*.a $(LIBCDIRSHARED)/lib ; fi
	if [ -e openssl/libssl.so ]; then  $(RM) -rf $(ROOTDIR)/lib/shared/include/openssl ; cp -faprL openssl/include/* $(ROOTDIR)/lib/shared/include ; fi
endif
	# ">>>>>>>>>>>>>>>> CONFIGURE ALL USERLIBS  <<<<<<<<<<<<<<<<<<<<<<"
	for i in $(dir_y) ; do \
	    if [ -f "$$i/DoConfigure.sh" ]; then ( cd $$i; ./DoConfigure.sh || exit 1 ; cd .. ) || exit 1 ; fi \
	done
	# ">>>>>>>>>>>>>>>> MAKE AND INSTALL USERLIBS  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"
	for i in $(dir_y) ; do \
	    if [ -d "$$i" ]; then ( cd $$i; $(MAKE) -j$(HOST_NCPU) ; $(MAKE) shared || exit 1 ; cd .. ) || exit 1 ; fi \
	done

clean:
	# ">>>>>>>>>>>>>>>> START FULL CLEANUP  <<<<<<<<<<<<<<<<<<<<<<"
	-cd openssl ; $(MAKE) clean ; cd ..
	-for i in $(SUBDIRS_LIBS); do $(MAKE) -C $$i clean; $(RM) -rf $(ROOTDIR)/user/$$i/filesystem/*; done
	-for i in $(SUBDIRS_LIBS); do $(MAKE) -C $$i distclean; $(RM) -rf $(ROOTDIR)/user/$$i/filesystem/*; done
	-for i in $(SUBDIRS_LIBS); do $(MAKE) -C $$i dist-clean; $(RM) -rf $(ROOTDIR)/user/$$i/filesystem/*; done
	-for i in $(SUBDIRS_LIBS); do $(MAKE) -C $$i mrproper; $(RM) -rf $(ROOTDIR)/user/$$i/filesystem/*; done
