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
# internal MT7620 WiFi module
###################################################################################################################################################
ifeq ($(CONFIG_RT_FIRST_CARD),7620)
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
	$(ROMFSINST) -S eeproms-defaults/MT7610E-V10-FEM-EXTPALNA.bin					/etc/Wireless/MT7610E-V10-FEM.bin
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
	$(ROMFSINST) -S eeproms-defaults/MT7603E_EEPROM-FEM-INTERNALPALNA.bin				/etc/Wireless/MT7603E_EEPROM.bin
endif
###################################################################################################################################################
# full backups dualband devices
###################################################################################################################################################
ifdef CONFIG_RT_SECOND_IF_MT7610E
ifeq ($(CONFIG_RT_FIRST_IF_INTERNAL_PA_INTERNAL_LNA),y)
	$(ROMFSINST) -S eeproms-backups/MT7620_7610_AP_2T2R-4L_internal_LNA_internal_PA_BACKUP.BIN	/etc/Wireless/rfbackup.bin
else ifeq ($(CONFIG_RT_FIRST_IF_INTERNAL_PA_EXTERNAL_LNA),y)
	$(ROMFSINST) -S eeproms-backups/MT7620_7610_AP_2T2R-4L_external_LNA_internal_PA_BACKUP.BIN	/etc/Wireless/rfbackup.bin
else ifeq ($(CONFIG_RT_FIRST_IF_EXTERNAL_PA_EXTERNAL_LNA),y)
	$(ROMFSINST) -S eeproms-backups/MT7620_7610_AP_2T2R-4L_external_LNA_external_PA_BACKUP.BIN	/etc/Wireless/rfbackup.bin
# replace defaults for black horiontal 7610 not production sample
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
