.EXPORT_ALL_VARIABLES:

-include $(LINUX_CONFIG)
-include $(CONFIG_CONFIG)
-include $(ARCH_CONFIG)

#for all cleanap
SUBDIRS_APP = $(shell LC_ALL=C; ls)

#allways agressive optimize for size
CFLAGS	+= -Os -fno-caller-saves -fno-reorder-blocks -fno-unwind-tables -fno-asynchronous-unwind-tables -femit-struct-debug-baseonly -fno-var-tracking
CFLAGS	+= -ffunction-sections -fdata-sections
LDFLAGS	+= -Wl,--gc-sections

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
ifdef CONFIG_RAETH_ESW
CFLAGS += -DCONFIG_RAETH_ESW -DCONFIG_RALINK_MT7628
endif
endif

#ralink specific applications
ra_dir_$(CONFIG_RT2880_REG_RW)      += reg
ra_dir_$(CONFIG_RT2880_MII_MGR)     += mii_mgr
ra_dir_$(CONFIG_RT2880_NVRAM)       += nvram
ra_dir_$(CONFIG_RT2880_MTD)         += mtd
ra_dir_$(CONFIG_RT2880_MTD_VERF)    += mtd_verifier
ra_dir_$(CONFIG_RT2880_GPIO_TEST)   += gpio
ra_dir_$(CONFIG_RTESW_SWITCH)       += switch
ra_dir_$(CONFIG_ETHMAC_TOOL)        += eth_mac
ra_dir_$(CONFIG_RALINKAPP_HWNAT)    += hw_nat
ra_dir_$(CONFIG_RALINKAPP_ATED)     += ated
ifdef CONFIG_RAETH_QDMA
ra_dir_$(CONFIG_RALINKAPP_QDMA)     += qdma
endif

#always install radio firmware
ra_dir_y			    += rt_firmware

all romfs:
	for i in $(ra_dir_y) ; do make -C $$i $@ || exit $?; done

clean:
	-for i in $(SUBDIRS_APP) ; do make -C $$i clean ; done
