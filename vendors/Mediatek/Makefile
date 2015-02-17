#########################################################################################################################
#
.EXPORT_ALL_VARIABLES:
#########################################################################################################################
-include config.arch
-include ../../../linux/.config
-include ../../../linux/drivers/mtd/ralink/ralink-flash.h
-include ../../../sdk_version.h
-include ../../../version

#########################################################################################################################
# Tune locale to eng
#
LANG		:= C
LC_COLLATE	:= C
LC_MESSAGES	:= C
LC_ALL		:= C

export LANG LC_COLLATE LC_MESSAGES LC_ALL
#########################################################################################################################
# Dir and build system config
#
ROMFS_DIR	:= $(ROOTDIR)/romfs
IMAGEDIR	:= $(ROOTDIR)/images
MKIMAGE_DIR	:= $(ROOTDIR)/tools/mkimage
KERNELZ		:= $(IMAGEDIR)/zImage
RAMDISK		:= $(IMAGEDIR)/ramdisk
IMAGE		:= $(IMAGEDIR)/$(PRODUCTID)_$(RT288X_SDK_VERSION).bin
RAMDISK_SIZE	:= $(shell echo $(CONFIG_BLK_DEV_RAM_SIZE))
CUR_DIR		:= $(shell pwd)
COMP		:= lzma
ROMFS_DIRS 	:= dev dev/pts sys proc tmp bin home mnt var etc web usr lib xml lib/modules usr/codepages media share

export ROMFS_DIR IMAGEDIR MKIMAGE_DIR KERNELZ RAMDISK IMAGE RAMDISK_SIZE CUR_DIR COMP ROMFS_DIRS
#########################################################################################################################
# Dir per system options
#
ifdef CONFIG_RALINK_MT7621
ENTRYPOINT	:= 80001000
else
ENTRYPOINT	:= 80000000
endif
export ENTRYPOINT
#########################################################################################################################

romfs:
	####PREPARE DIRS FOR ROMFS AND IMAGES####
	[ -d $(IMAGEDIR) ] || mkdir -p $(IMAGEDIR)
	[ -d $(ROMFSDIR) ] || mkdir -p $(ROMFSDIR)
	[ -d $(ROMFSDIR)/$$i ] || mkdir -p $(ROMFSDIR)
	for i in $(ROMFS_DIRS); do \
	    [ -d $(ROMFSDIR)/$$i ] || mkdir -p $(ROMFSDIR)/$$i; \
	done
	##########CREATE NEEDED SYMLINKS#########
	$(ROMFSINST) -s ./bin /sbin
	$(ROMFSINST) -s ../bin /usr/bin
	$(ROMFSINST) -s ../bin /usr/sbin
	$(ROMFSINST) -s ../lib /usr/lib
	$(ROMFSINST) -s ../share /usr/share
	$(ROMFSINST) -s ../etc/scripts/config.sh /bin/config.sh
	$(ROMFSINST) -s /proc/self/mounts /etc/mtab

romfs.post::
	############CLEANUP IMAGES################
	rm -f $(IMAGE)
	rm -f $(RAMDISK)
	rm -f $(KERNELZ).lzma
ifeq	($(CONFIG_RT2880_ROOTFS_IN_FLASH),y)
	#############COMPRESS ROMFS################
	$(ROOTDIR)/tools/bin/mksquashfs_xz-4.0 $(ROMFSDIR) $(RAMDISK) -all-root -no-exports -noappend -nopad -no-xattrs -no-recovery -always-use-fragments
endif

image:
	##############CREATE IMAGE#################
	$(CROSS_COMPILE)objcopy -O binary -R .note -R .comment -S $(LINUXDIR)/vmlinux  $(KERNELZ)
	cd $(IMAGEDIR) ; rm -f $(KERNELZ).*; \
	$(ROOTDIR)/tools/bin/lzma_alone e -a1 -lc1 -lp2 -pb2 -d27 $(KERNELZ) $(KERNELZ).$(COMP)
ifeq    ($(CONFIG_BLK_DEV_INITRD),)
ifeq    ($(CONFIG_ROOTFS_IN_FLASH_NO_PADDING),)
	# Original Kernel Image Size
	@wc -c $(KERNELZ).$(COMP)
ifeq    ($(findstring 0x, $(CONFIG_MTD_KERNEL_PART_SIZ)),0x)
	@SIZE=`wc -c $(KERNELZ).$(COMP) | awk '{ print $$1 }'` ; \
	MTD_KRN_PART_SIZE=`printf "%d" $(CONFIG_MTD_KERNEL_PART_SIZ)` ; \
	PAD=`expr $$MTD_KRN_PART_SIZE - 64 - $$SIZE` ; \
	dd if=/dev/zero count=1 bs=$$PAD conv=sync 2> /dev/null | tr \\000 \\377 >> $(KERNELZ).$(COMP)
else
	@SIZE=`wc -c $(KERNELZ).$(COMP) | awk '{ print $$1 }'` ; \
	MTD_KRN_PART_SIZE=`printf "%d" 0x$(CONFIG_MTD_KERNEL_PART_SIZ)` ; \
	PAD=`expr $$MTD_KRN_PART_SIZE - 64 - $$SIZE` ; \
	dd if=/dev/zero count=1 bs=$$PAD conv=sync 2> /dev/null | tr \\000 \\377 >> $(KERNELZ).$(COMP)
endif
	# Padded Kernel Image Size
	  @wc -c $(KERNELZ).$(COMP)
endif
	# Original RootFs Size
	  @du -sb $(ROMFSDIR)
	# Compressed RootFs Size
	  @wc -c $(RAMDISK)
	# Padded Kernel Image + Compressed Rootfs Size
	  @cat $(RAMDISK) >> $(KERNELZ).$(COMP)
	  @wc -c $(KERNELZ).$(COMP)
	#===========================================
endif
	# Pack final image and write headers
ifeq    ($(CONFIG_ROOTFS_IN_FLASH_NO_PADDING),)
	@cd $(IMAGEDIR) ; \
	$(MKIMAGE_DIR)/mkimage -A mips -O linux -T kernel -C $(COMP) -a $(ENTRYPOINT) \
	-e $(shell readelf -h $(LINUXDIR)/vmlinux | grep "Entry" | awk '{print $$4}') \
	-n $(PRODUCTID) -d $(KERNELZ).$(COMP) $(IMAGE)
else
	# For No padded need write kernel size in image header
	# to correct mount particion in mtd drivers adress
	@cd $(IMAGEDIR) ; \
	ISIZE=`wc -c $(KERNELZ).$(COMP) | awk '{print $$1}'` ; \
	RSIZE=`wc -c $(RAMDISK) | awk '{print $$1}'` ; \
	KRN_SIZE=`expr $$ISIZE - $$RSIZE + 64` ; \
	$(MKIMAGE_DIR)/mkimage -A mips -O linux -T kernel -C $(COMP) -a $(ENTRYPOINT) \
	-e $(shell readelf -h $(LINUXDIR)/vmlinux | grep "Entry" | awk '{print $$4}') \
	-k $$KRN_SIZE -n $(PRODUCTID) -d $(KERNELZ).$(COMP) $(IMAGE)
endif

clean:
	rm -f $(RAMDISK)
