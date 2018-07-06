###########################################################################
#
# Makefile -- Top level uClinux makefile.
#
# Copyright (c) 2001-2004, SnapGear (www.snapgear.com)
# Copyright (c) 2001, Lineo
# Copyright (c) 2010-2016, Wive-NG (wive-ng.sf.net)
#

############################################################################
#
# Lets work out what the user wants, and if they have configured us yet
#

.EXPORT_ALL_VARIABLES:

ROOTDIR	:= $(shell pwd)

all: oldconfig tools prep_romfs linux libs user_only romfs image

CONFIG_VENDOR	:= Mediatek
CONFIG_LINUXDIR := linux
CONFIG_LIBCDIR	:= lib
CONFIG_LANGUAGE	:= "en_EN.UTF-8:en"

#########################################################################################################
# Tune locale to eng
#
LANG		:= C
LC_COLLATE	:= C
LC_MESSAGES	:= C
LC_ALL		:= C

#########################################################################################################
# Cleanup CFLAGS/CPPFLAGS/LDFALGS
#
CFLAGS		:=
CPPFLAGS	:=
CXXFLAGS	:=
LDFLAGS		:=

############################################################################
#
# Get the core stuff worked out
#

MAKE		:= make
HOSTCC       	:= gcc
HOSTCXX      	:= g++
HOSTCFLAGS   	:= -Wall -Wmissing-prototypes -Wstrict-prototypes -O2 -fomit-frame-pointer -std=gnu89 -fno-delete-null-pointer-checks
HOSTCXXFLAGS 	:= -O2

ROMFSINST	:= romfs-inst.sh

FIRMROOT	:= $(ROOTDIR)
LINUXDIR	:= $(ROOTDIR)/$(CONFIG_LINUXDIR)
LIBCDIR		:= $(ROOTDIR)/$(CONFIG_LIBCDIR)
LIBCDIRSHARED	:= $(ROOTDIR)/$(CONFIG_LIBCDIR)/shared
IMAGEDIR	:= $(ROOTDIR)/images
ROMFSDIR	:= $(ROOTDIR)/romfs
SCRIPTSDIR	:= $(ROOTDIR)/config/scripts
LINUX_CONFIG	:= $(LINUXDIR)/.config
ARCH_CONFIG	:= $(ROOTDIR)/vendors/config/mips/config.arch
CONFIG_CONFIG	:= $(ROOTDIR)/config/.config
VERSION_CONFIG	:= $(ROOTDIR)/version
PATH		:= $(PATH):$(ROOTDIR):$(ROOTDIR)/tools:$(ROOTDIR)/toolchain/bin:$(ROOTDIR)/lib/lib:$(ROOTDIR)/lib/include:$(LINUXDIR):$(LIBCDIR)

# May use a different compiler
CONFIG_CROSS_COMPILER_PATH	:= $(ROOTDIR)/toolchain/bin/
CROSS_COMPILE			:= $(CONFIG_CROSS_COMPILER_PATH)mipsel-linux-uclibc-
KERNEL_CROSS_COMPILE		:= $(CROSS_COMPILE)
CROSS_COMPILER_PREFIX		:= $(CROSS_COMPILE)

# NUM MAKE PROCESS = CPU NUMBER IN THE SYSTEM * CPU_OVERLOAD
CPU_OVERLOAD		:= 4
HOST_NCPU		:= $(shell if [ -f /proc/cpuinfo ]; then n=`grep -c processor /proc/cpuinfo`; if [ $$n -gt 1 ];then expr $$n \* ${CPU_OVERLOAD}; else echo $$n; fi; else echo 1; fi)
CONFIG_SHELL		:= $(shell if [ -x "$$BASH" ]; then echo $$BASH; else if [ -x /bin/bash ]; then echo /bin/bash; else echo sh; fi ; fi)

# Include configs
-include $(VERSION_CONFIG)
-include $(ARCH_CONFIG)

ifneq ($(SUBARCH),)
# Using UML, so make the kernel and non-kernel with different ARCHs
MAKEARCH := $(MAKE) ARCH=$(SUBARCH) CROSS_COMPILE=$(CROSS_COMPILE)
MAKEARCH_KERNEL := $(MAKE) ARCH=$(ARCH) SUBARCH=$(SUBARCH) CROSS_COMPILE=$(KERNEL_CROSS_COMPILE)
else
MAKEARCH := $(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE)
MAKEARCH_KERNEL := $(MAKEARCH)  ARCH=$(ARCH) CROSS_COMPILE=$(KERNEL_CROSS_COMPILE)
endif

KCONFIG_NOTIMESTAMP := 1

DIRS :=  $(ROOTDIR)/vendors $(ROOTDIR)/lib $(ROOTDIR)/user

export LANG LC_COLLATE LC_MESSAGES LC_ALL
export HOSTCC HOSTCXX HOSTCFLAGS HOSTCXXFLAGS CFLAGS CPPFLAGS CXXFLAGS LDFLAGS
export KCONFIG_NOTIMESTAMP
export CONFIG_VENDOR CONFIG_LINUXDIR CONFIG_LIBCDIR LIBCDIR LIBCDIRSHARED CONFIG_LANGUAGE VENDOR PRODUCT CONFIG_SHELL
export CONFIG_CROSS_COMPILER_PATH CROSS_COMPILE KERNEL_CROSS_COMPILE CROSS_COMPILER_PREFIX
export CONFIG_CONFIG LINUX_CONFIG ARCH_CONFIG
export DEVNAME REALNAME VERSIONPKG VERSIONSTR
export FIRMROOT DIRS ROOTDIR LINUXDIR ROMFSDIR SCRIPTSDIR IMAGEDIR ROMFSINST
export HOST_NCPU PATH

############################################################################
#
# normal make targets
#

.PHONY: oldconfig
oldconfig:
	if [ ! -f $(LINUXDIR)/.config ] ; then \
		echo "ERROR: you need to do a 'make config' first" ; \
		exit 1 ; \
	fi
	$(MAKEARCH) -C config oldconfig || exit 1
	$(MAKEARCH_KERNEL) -C $(LINUXDIR) oldconfig || exit 1
	$(MAKEARCH_KERNEL) -C $(LINUXDIR) prepare || exit 1
	$(MAKEARCH_KERNEL) -C $(LINUXDIR) dep || exit 1

.PHONY: libs
libs: lib_configure lib_only lib_install libnvram_only libwive_only libext_only

.PHONY: lib_configure
lib_configure:
	##########################UPDATE CONFIGS#############################
	$(MAKEARCH) -C $(LIBCDIR) oldconfig || exit 1

.PHONY: lib_install
lib_install:
	#####################COMPOSE LIBC SHARE HEADERS######################
	mkdir -p $(LIBCDIRSHARED)
	$(MAKEARCH_KERNEL) -j$(HOST_NCPU) -C $(LIBCDIR) install_headers PREFIX=$(LIBCDIRSHARED)/ DEVEL_PREFIX= MULTILIB_DIR=lib || exit 1
	#####################COMPOSE LIBC SHARE LIBC######################
	mkdir -p $(LIBCDIRSHARED)/lib
	cp -aprfv $(LIBCDIR)/lib/*.a $(LIBCDIRSHARED)/lib
	cp -aprfv $(LIBCDIR)/lib/*.so* $(LIBCDIRSHARED)/lib

.PHONY: prep_romfs
prep_romfs:
	####################PREPARE-ROMFS####################
	$(MAKEARCH) -C vendors romfs || exit 1

.PHONY: romfs
romfs: romfs.subdirs modules_install romfs.post

.PHONY: romfs.subdirs
romfs.subdirs:
	#################INSTALL-APPS-ROMFS###################
	cd $(ROOTDIR)
	fakeroot tar -zxvf dev.tgz
	fakeroot cp -rfva dev/* $(ROMFSDIR)/dev
	$(ROMFSINST) -S version /share/version
	$(ROMFSINST) -s /share/version /etc/version
	cp -vfra $(ROOTDIR)/etc/* $(ROMFSDIR)/etc
	cp -vfa  $(ROOTDIR)/etc/rc.d/rcS $(ROMFSDIR)/bin/rcS
	cd $(ROMFSDIR)/bin && /bin/ln -fvs ../etc/scripts/* . && cd $(ROOTDIR)
	cd $(ROOTDIR)
	for dir in $(DIRS) ; do [ ! -d $$dir ] || $(MAKEARCH) -C $$dir romfs || exit 1 ; done

.PHONY: modules_install
modules_install:
	####################INSTALL_MODULES####################
	. $(LINUXDIR)/.config; if [ "$$CONFIG_MODULES" = "y" ]; then \
		[ -d $(ROMFSDIR)/lib/modules ] || mkdir -p $(ROMFSDIR)/lib/modules; \
		$(MAKEARCH_KERNEL) -C $(LINUXDIR) INSTALL_MOD_PATH=$(ROMFSDIR) DEPMOD="$(ROOTDIR)/user/busybox/examples/depmod.pl" modules_install; \
	fi

.PHONY: romfs.post
romfs.post:
	##################COPY_RUNTIME_LIBS####################
	cp -aprfv $(LIBCDIRSHARED)/lib/*.so* $(ROMFSDIR)/lib
	find $(ROOTDIR)/toolchain/lib -type f -name 'libgcc_s*so*' -exec cp -vfap {} $(ROMFSDIR)/lib/ \;
	######################CLEANUP##########################
	-find $(ROMFSDIR)/. -name CVS | xargs -r rm -rf
	-rm -f $(ROMFSDIR)/lib/modules/*/build
	-rm -f $(ROMFSDIR)/lib/modules/*/source
	-rm -fr $(ROOTDIR)/dev
	#################STRIP_APPS_LIB_ROMFS##################
	./strip.sh
	###################SET_COMPILE_DATE####################
	date +%Y%m%d%H%M > $(ROMFSDIR)/etc/compile-date
	###################APPS-INSTALLED######################
	$(MAKEARCH) -C vendors romfs.post || exit 1

.PHONY: image
image:
	mkdir -p $(IMAGEDIR)
	$(MAKEARCH) -C vendors image || exit 1

#
# fancy target that allows a vendor to have other top level
# make targets,  for example "make vendor_flash" will run the
# vendor_flash target in the vendors directory
#
vendor_%:
	$(MAKEARCH) -C vendors $@ || exit 1

# for select build mode (one/multi thread)
-include $(LINUX_CONFIG)
ifeq ($(CONFIG_MT76X2_STA),m)
# Added by Steven@Ralink FIXME!!!
# In linux-2.6, it do not support VPATH in Makefile.
# But we need to use drivers/net/wireless/* to build ap and sta driver.
# Workaround: Don't build ap and sta driver at the same time.
THREADS="-j1"
else
THREADS="-j$(HOST_NCPU)"
endif

.PHONY: linux
linux:
	########################KERNEL BUILD#################################
	$(MAKEARCH_KERNEL) $(THREADS) -C $(LINUXDIR) $(LINUXTARGET) || exit 1
	#################COMPOSE AND INSTALL SHARED HEADERS##################
	mkdir -p $(LIBCDIRSHARED)
	$(MAKEARCH_KERNEL) $(THREADS) -C $(LINUXDIR) headers_install INSTALL_HDR_PATH=$(LIBCDIRSHARED) || exit 1
	cp -aprf $(LINUXDIR)/include/generated/* $(LIBCDIRSHARED)/include/linux
	cp -aprf $(LINUXDIR)/include/linux/ralink_gpio.h $(LIBCDIRSHARED)/include/linux
	find $(LIBCDIRSHARED) -type f -name ".*" -exec rm "{}" \;
	if [ -f $(LINUXDIR)/vmlinux ]; then \
		ln -f $(LINUXDIR)/vmlinux $(LINUXDIR)/linux ; \
	fi

.PHONY: tools
tools:
	$(MAKE) -j$(HOST_NCPU) -C tools || exit 1

.PHONY: clean
clean:
	################PREPARE FOR CLEANUP############################
	touch $(ROOTDIR)/lib/.config
	touch $(ROOTDIR)/linux/.config
	touch $(ROOTDIR)/config/autoconf.h
	################CLEAN ALL SUBDIRS 1############################
	-for dir in $(DIRS) $(LINUXDIR) Uboot ; do [ ! -d $$dir ] || $(MAKEARCH) -C $$dir clean ; done
	-for dir in $(DIRS) $(LINUXDIR) Uboot ; do [ ! -d $$dir ] || $(MAKEARCH) -C $$dir distclean ; done
	################CLEAN ALL SUBDIRS 2############################
	-$(MAKE) clean -C config
	-$(MAKE) clean -C tools
	-$(MAKE) clean -C libnvram
	-$(MAKE) clean -C libwive
	-$(MAKE) clean -C libext
	-$(MAKE) mrproper -C Uboot
	-$(MAKE) mrproper -C linux
	##############REMOVE UNUSED FILES 1###########################
	rm -f $(ROOTDIR)/etc/compile-date $(ROOTDIR)/etc/scripts/config.sh $(ROOTDIR)/config.tk $(ROOTDIR)/.tmp* $(ROOTDIR)/sdk_version.h $(ROOTDIR)/version $(LINUXDIR)/linux $(LINUXDIR)/arch/mips/ramdisk/*.gz
	##############REMOVE UNUSED FILES 2###########################
	find $(ROOTDIR) -type f -a \( -name '*.*~' -o -name '*.ko' -o -name '*.log' -o -name '*.old' -o -name 'config.log' -o -name 'stamp-h1' -o -name '.sgbuilt_user' -o -name '.config.cmd' \) | xargs rm -f
	##############REMOVE UNUSED FILES 3###########################
	find $(ROOTDIR)/lib $(ROOTDIR)/libext $(ROOTDIR)/user -type f -a \( -name '*.o' -o -name '*.a' -o -name '*.so' -o -name '*.lo' -o -name '*.la' \) | xargs rm -f
	##############REMOVE UNUSED FOLDERS###########################
	find $(ROOTDIR) -type d -a \( -name 'filesystem' -o -name 'autom4te.cache' -o -name '.libs' -o -name 'cvs' -o -name 'CVS' -o -name '.dep' -o -name '.deps' \) | xargs rm -rf
	rm -rf $(LIBCDIRSHARED) $(ROOTDIR)/dev $(IMAGEDIR) $(ROMFSDIR) romfs images .config

%_only:
	case "$(@)" in \
	*/*) d=`expr $(@) : '\([^/]*\)/.*'`; \
	     t=`expr $(@) : '[^/]*/\(.*\)'`; \
	     $(MAKEARCH) -j$(HOST_NCPU) -C $$d $$t;; \
	*)   $(MAKEARCH) -j$(HOST_NCPU) -C $(@:_only=);; \
	esac || exit 1

%_clean:
	case "$(@)" in \
	*/*) d=`expr $(@) : '\([^/]*\)/.*'`; \
	     t=`expr $(@) : '[^/]*/\(.*\)'`; \
	     $(MAKEARCH) -C $$d $$t;; \
	*)   $(MAKEARCH) -C $(@:_clean=) clean;; \
	esac

############################################################################
#
# debug make targets
#

.PHONY: web_make
web_make:
	$(MAKEARCH) -j$(HOST_NCPU) -C user web || exit 1

.PHONY: web
web: web_make romfs image
