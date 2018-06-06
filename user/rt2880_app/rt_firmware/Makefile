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
ifeq ($(MODEL_NAME),SNR-CPE-MD1.2-5GHZ-MT)
	$(ROMFSINST) -S eeproms-defaults/MT7610E-V10-FEMSKY85717-EXTPALNA.bin				/etc/Wireless/MT7610E-V10-FEM.bin
else ifeq ($(MODEL_NAME),SNR-CPE-ME1-5GHZ-MT)
	$(ROMFSINST) -S eeproms-defaults/MT7610E-V10-FEMSKYSE5003L-EXTPALNA.bin				/etc/Wireless/MT7610E-V10-FEM.bin
else
	$(ROMFSINST) -S eeproms-defaults/MT7610E-V10-FEM-EXTPALNA.bin					/etc/Wireless/MT7610E-V10-FEM.bin
endif
endif
###################################################################################################################################################
# external PCI-E 76x2 WiFi module
###################################################################################################################################################
# 7602 module
ifdef CONFIG_RT_FIRST_IF_MT7602E
ifeq ($(CONFIG_RT_FIRST_IF_INTERNAL_PA_INTERNAL_LNA),y)
	$(ROMFSINST) -S eeproms-defaults/MT7602E_EEPROM-FEM-INTERNALPALNA.bin				/etc/Wireless/MT7602E_EEPROM.bin
else ifeq ($(CONFIG_RT_FIRST_IF_INTERNAL_PA_EXTERNAL_LNA),y)
	$(ROMFSINST) -S eeproms-defaults/MT7602E_EEPROM-FEM-INTERNALPAEXERNALLNA.bin			/etc/Wireless/MT7602E_EEPROM.bin
else ifeq ($(CONFIG_RT_FIRST_IF_EXTERNAL_PA_EXTERNAL_LNA),y)
	$(ROMFSINST) -S eeproms-defaults/MT7602E_EEPROM-FEM-EXTERNALPALNA.bin				/etc/Wireless/MT7602E_EEPROM.bin
endif
endif
# 7612 module
ifdef CONFIG_RT_SECOND_IF_MT7612E
ifeq ($(CONFIG_RT_SECOND_IF_INTERNAL_PA_INTERNAL_LNA),y)
	$(ROMFSINST) -S eeproms-defaults/MT7612E_EEPROM-FEM-INTERNALPALNA.bin				/etc/Wireless/MT7612E_EEPROM.bin
else ifeq ($(CONFIG_RT_SECOND_IF_INTERNAL_PA_EXTERNAL_LNA),y)
	$(ROMFSINST) -S eeproms-defaults/MT7612E_EEPROM-FEM-INTERNALPAEXTERNALLNA.bin			/etc/Wireless/MT7612E_EEPROM.bin
else ifeq ($(CONFIG_RT_SECOND_IF_EXTERNAL_PA_EXTERNAL_LNA),y)
	$(ROMFSINST) -S eeproms-defaults/MT7612E_EEPROM-FEM-EXTERNALPALNA.bin				/etc/Wireless/MT7612E_EEPROM.bin
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
# external PCI-E 7615 WiFi module
###################################################################################################################################################
# 7615 module
ifdef CONFIG_RT_FIRST_IF_MT7615E
	$(ROMFSINST) -S eeproms-backups/MT7621_MT7615N_external_LNA_external_PA_BACKUP.BIN		/etc/Wireless/rfbackup.bin
	$(ROMFSINST) -S eeproms-defaults/MT7615N_EEPROM1.bin						/etc/Wireless/MT7615E_EEPROM1.bin
	$(ROMFSINST) -S sku/7615_SingleSKU.dat								/etc/Wireless/SingleSKU.dat
	$(ROMFSINST) -S sku/7615_SingleSKU_BF.dat							/etc/Wireless/SingleSKU_BF.dat
ifdef CONFIG_RT_SECOND_IF_MT7615E
	$(ROMFSINST) -S eeproms-defaults/MT7615N_EEPROM2.bin						/etc/Wireless/MT7615E_EEPROM2.bin
endif
	$(ROMFSINST) -S	defaults/default-4t4r-3t3r-wan0							/etc/default/nvram_default
endif
###################################################################################################################################################
# full backups dualband devices
###################################################################################################################################################
ifdef CONFIG_RT_SECOND_IF_MT7610E
ifdef CONFIG_RALINK_MT7621
	$(ROMFSINST) -S eeproms-backups/MT7621_MT7603_MT7610_external_LNA_external_PA_BACKUP.BIN	/etc/Wireless/rfbackup.bin
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
ifeq ($(MODEL_NAME),SNR-CPE-CG1-5GHZ-MT)
# replace defaults for 7603+7610 sample
	$(ROMFSINST) -S	defaults/default-2t2r-1t1r-wan0							/etc/default/nvram_default
endif
ifeq ($(MODEL_NAME),SNR-CPE-ME1-5GHZ-MT)
# replace defaults for 7603+7610 sample
	$(ROMFSINST) -S	defaults/default-2t2r-1t1r-wan0							/etc/default/nvram_default
endif
ifeq ($(MODEL_NAME),SNR-CPE-MD1.1-5GHZ-MT)
# replace defaults for white horizontal 7610 MD1.1
	$(ROMFSINST) -S	defaults/default-2t2r-1t1r-wan0							/etc/default/nvram_default
	$(ROMFSINST) -S eeproms-backups/MT7620_7610_AP_2T2R-3A_external_LNA_external_PA_BACKUP.BIN	/etc/Wireless/rfbackup.bin
endif
ifeq ($(MODEL_NAME),SNR-CPE-MD1.2-5GHZ-MT)
# replace defaults for new horizontal 7610 MD1.2
	$(ROMFSINST) -S	defaults/default-2t2r-1t1r-wan0							/etc/default/nvram_default
	$(ROMFSINST) -S eeproms-backups/MT7620_7610_AP_2T2R-4A_external_LNA_external_PA_BACKUP.BIN	/etc/Wireless/rfbackup.bin
endif
ifeq ($(MODEL_NAME),SNR-CPE-MD1-5GHZ-MULTI-USB-MT)
# replace defaults for black horizontal 7610 not production sample
	$(ROMFSINST) -S	defaults/default-2t2r-1t1r-wan0							/etc/default/nvram_default
endif
endif
ifdef CONFIG_RT_SECOND_IF_MT7612E
ifeq ($(CONFIG_RT_SECOND_IF_INTERNAL_PA_INTERNAL_LNA),y)
	$(ROMFSINST) -S eeproms-backups/MT7620_7612_AP_2T2R-4L_internal_LNA_internal_PA_BACKUP.BIN	/etc/Wireless/rfbackup.bin
else ifeq ($(CONFIG_RT_SECOND_IF_INTERNAL_PA_EXTERNAL_LNA),y)
	$(ROMFSINST) -S eeproms-backups/MT7620_7612_AP_2T2R-4L_internal_LNA_external_PA_BACKUP.BIN	/etc/Wireless/rfbackup.bin
else ifeq ($(CONFIG_RT_SECOND_IF_EXTERNAL_PA_EXTERNAL_LNA),y)
	$(ROMFSINST) -S eeproms-backups/MT7620_7612_AP_2T2R-4L_external_LNA_external_PA_BACKUP.BIN	/etc/Wireless/rfbackup.bin
endif
ifdef CONFIG_RALINK_MT7621
ifdef CONFIG_RT_SECOND_IF_EXTERNAL_PA_EXTERNAL_LNA
	$(ROMFSINST) -S eeproms-backups/MT7621_MT7602_MT7612_external_LNA_external_PA_BACKUP.BIN	/etc/Wireless/rfbackup.bin
else
	$(ROMFSINST) -S eeproms-backups/MT7621_MT7602_MT7612_internal_LNA_internal_PA_BACKUP.BIN	/etc/Wireless/rfbackup.bin
endif
ifdef CONFIG_RT_FIRST_IF_MT7603E
	$(ROMFSINST) -S eeproms-backups/MT7621_MT7603_MT7612_internal_LNA_internal_PA_BACKUP.BIN	/etc/Wireless/rfbackup.bin
endif
endif
ifeq ($(CONFIG_RT_SECOND_IF_EXTERNAL_PA_EXTERNAL_LNA),y)
# replace defaults for outdor AP router config 7620+7612 with external amplifiers
	$(ROMFSINST) -S	defaults/default-2t2r-2t2r-wan4							/etc/default/nvram_default
else
# replace defaults 7621 tube and metal not production samples
	$(ROMFSINST) -S	defaults/default-2t2r-2t2r-wan0							/etc/default/nvram_default
endif
endif
