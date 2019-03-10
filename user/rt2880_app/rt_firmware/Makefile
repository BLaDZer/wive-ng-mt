.EXPORT_ALL_VARIABLES:

-include $(LINUX_CONFIG)
-include $(CONFIG_CONFIG)
-include $(ARCH_CONFIG)
-include $(VERSION_CONFIG)

CHIP7620 :=
ifdef CONFIG_RALINK_MT7620
ifeq ($(CONFIG_RT_FIRST_CARD),7620)
CHIP7620 := y
endif
endif

all:

clean:

romfs:
	mkdir -p $(ROMFSDIR)/etc/Wireless
	mkdir -p $(ROMFSDIR)/etc/default
	mkdir -p $(ROMFSDIR)/etc/Wireless/RT2860
ifndef CONFIG_RT_SECOND_IF_NONE
	mkdir -p $(ROMFSDIR)/etc/Wireless/iNIC
endif
###################################################################################################################################################
# internal MT7620 WiFi module (2860 - old stable driver, 7620 new driver)
###################################################################################################################################################
ifdef CHIP7620
	rm -f MT7620_AP_2T2R-4L_V15.BIN
ifeq ($(CONFIG_RT_FIRST_IF_INTERNAL_PA_INTERNAL_LNA),y)
	cp -f eeproms-backups/MT7620_AP_2T2R-4L_internal_LNA_internal_PA_V15.BIN			MT7620_AP_2T2R-4L_V15.BIN
else ifeq ($(CONFIG_RT_FIRST_IF_INTERNAL_PA_EXTERNAL_LNA),y)
	cp -f eeproms-backups/MT7620_AP_2T2R-4L_external_LNA_internal_PA_V15.BIN			MT7620_AP_2T2R-4L_V15.BIN
else ifeq ($(CONFIG_RT_FIRST_IF_EXTERNAL_PA_EXTERNAL_LNA),y)
	cp -f eeproms-backups/MT7620_AP_2T2R-4L_external_LNA_external_PA_V15.BIN			MT7620_AP_2T2R-4L_V15.BIN
endif
	$(ROMFSINST) -S	MT7620_AP_2T2R-4L_V15.BIN							/etc/Wireless/MT7620_AP_2T2R-4L_V15.BIN
	$(ROMFSINST) -S	MT7620_AP_2T2R-4L_V15.BIN                         				/etc/Wireless/rfbackup.bin
	rm -f MT7620_AP_2T2R-4L_V15.BIN
# install defaults (commulative for SNR W4N and MD1 production routers)
	$(ROMFSINST) -S	defaults/default-2t2r-1t1r-wan4							/etc/default/nvram_default
endif
###################################################################################################################################################
# external PCI-E 7610 WiFi module (ALWAYS EXTPA/LNA CONFIG FOR THIS MODULE)
###################################################################################################################################################
ifdef CONFIG_RT_SECOND_IF_MT7610E
ifdef CONFIG_RALINK_MT7621
	$(ROMFSINST) -S eeproms-defaults/MT7610E-V10-FEMSKYSE5003L-EXTPALNA.bin				/etc/Wireless/MT7610E-V10-FEM.bin
else
	$(ROMFSINST) -S eeproms-defaults/MT7610E-V10-FEM-EXTPALNA.bin					/etc/Wireless/MT7610E-V10-FEM.bin
endif
endif
###################################################################################################################################################
# external PCI-E 76x3 WiFi module
###################################################################################################################################################
# 7603 module
ifdef CONFIG_RT_FIRST_IF_MT7603E
ifeq ($(CONFIG_RT_FIRST_IF_INTERNAL_PA_INTERNAL_LNA),y)
	$(ROMFSINST) -S eeproms-defaults/MT7603E_EEPROM-FEM-INTERNALPALNA.bin				/etc/Wireless/MT7603E_EEPROM.bin
else ifeq ($(CONFIG_RT_FIRST_IF_EXTERNAL_PA_EXTERNAL_LNA),y)
	$(ROMFSINST) -S eeproms-defaults/MT7603E_EEPROM-FEM-EXTERNALPALNA.bin				/etc/Wireless/MT7603E_EEPROM.bin
endif
endif
###################################################################################################################################################
# full backups dualband devices
###################################################################################################################################################
ifdef CONFIG_RT_SECOND_IF_MT7610E
ifdef CONFIG_RALINK_MT7621
# replace defaults and backup calibs for 7603+7610
	$(ROMFSINST) -S eeproms-backups/MT7621_MT7603_MT7610_external_LNA_external_PA_BACKUP.BIN	/etc/Wireless/rfbackup.bin
	$(ROMFSINST) -S	defaults/default-2t2r-1t1r-wan0							/etc/default/nvram_default
else
ifeq ($(CONFIG_RT_FIRST_IF_INTERNAL_PA_INTERNAL_LNA),y)
	$(ROMFSINST) -S eeproms-backups/MT7620_7610_AP_2T2R-4L_internal_LNA_internal_PA_BACKUP.BIN	/etc/Wireless/rfbackup.bin
else ifeq ($(CONFIG_RT_FIRST_IF_INTERNAL_PA_EXTERNAL_LNA),y)
	$(ROMFSINST) -S eeproms-backups/MT7620_7610_AP_2T2R-4L_external_LNA_internal_PA_BACKUP.BIN	/etc/Wireless/rfbackup.bin
else ifeq ($(CONFIG_RT_FIRST_IF_EXTERNAL_PA_EXTERNAL_LNA),y)
	$(ROMFSINST) -S eeproms-backups/MT7620_7610_AP_2T2R-4L_external_LNA_external_PA_BACKUP.BIN	/etc/Wireless/rfbackup.bin
endif
endif
# REPLACE DEFAULTS AND CALIBS FOR SUBMODELS
ifeq ($(MODEL_NAME),SNR-CPE-MD1.1-5GHZ-MT)
# replace defaults for white horizontal 7610 MD1.1
	$(ROMFSINST) -S	defaults/default-2t2r-1t1r-wan0							/etc/default/nvram_default
endif
endif
