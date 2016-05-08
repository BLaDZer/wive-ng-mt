#
#	Makefile -- Build instructions for user level libs
#

.EXPORT_ALL_VARIABLES:
.PHONY: all romfs shared clean

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
dir_$(CONFIG_LIB_OPENSSL)         += libssl
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
dir_$(CONFIG_LIB_TALLOC)          += libtalloc

all:
	# ">>>>>>>>>>>>>> CONFIGURE AND BUILD USERLIBS  <<<<<<<<<<<<<<<<<<<"
	for i in $(dir_y) ; do \
	    if [ -f "$$i/DoConfigure.sh" ]; then ( cd $$i; ./DoConfigure.sh || exit 1 ; cd .. ) || exit 1 ; fi ; \
	    if [ -d "$$i" ]; then ( cd $$i; $(MAKE) -j$(HOST_NCPU) ; $(MAKE) shared || exit 1 ; cd .. ) || exit 1 ; fi \
	done

clean:
	# ">>>>>>>>>>>>>>>>> START FULL LIB CLEANUP  <<<<<<<<<<<<<<<<<<<<<<"
	-for i in $(SUBDIRS_LIBS); do $(MAKE) -C $$i clean; done
	-for i in $(SUBDIRS_LIBS); do $(MAKE) -C $$i distclean; done
	-for i in $(SUBDIRS_LIBS); do $(RM) -rf $(ROOTDIR)/libext/$$i/filesystem/*; done

# stubs for compat
romfs:
	echo OK

shared:
	echo OK
