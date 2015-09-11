all:

clean:

romfs:
	mkdir -p $(ROMFSDIR)/etc/Wireless
	mkdir -p $(ROMFSDIR)/etc/default
	mkdir -p $(ROMFSDIR)/etc/Wireless/RT2860
# internal WiFi module
ifdef CONFIG_RALINK_MT7620
	$(ROMFSINST) -S	defaults/default-2T2R						/etc/default/nvram_default
	rm -f MT7620_AP_2T2R-4L_V15.BIN
ifeq ($(CONFIG_RT_FIRST_IF_INTERNAL_PA_INTERNAL_LNA),y)
	cp -f MT7620_AP_2T2R-4L_internal_LNA_internal_PA_V15.BIN			MT7620_AP_2T2R-4L_V15.BIN
else ifeq ($(CONFIG_RT_FIRST_IF_INTERNAL_PA_EXTERNAL_LNA),y)
	cp -f MT7620_AP_2T2R-4L_external_LNA_internal_PA_V15.BIN			MT7620_AP_2T2R-4L_V15.BIN
else ifeq ($(CONFIG_RT_FIRST_IF_PA_EXTERNAL_LNA),y)
	cp -f MT7620_AP_2T2R-4L_external_LNA_external_PA_V15.BIN			MT7620_AP_2T2R-4L_V15.BIN
else
	cp -f MT7620_AP_2T2R-4L_internal_LNA_internal_PA_V15.BIN			MT7620_AP_2T2R-4L_V15.BIN
endif
	$(ROMFSINST) -S	MT7620_AP_2T2R-4L_V15.BIN					/etc/Wireless/MT7620_AP_2T2R-4L_V15.BIN
	$(ROMFSINST) -S	MT7620_AP_2T2R-4L_V15.BIN                         		/etc/Wireless/rfbackup.bin
	rm -f MT7620_AP_2T2R-4L_V15.BIN
	rm -f MT7620_AP_2T2R-4L_BACKUP.BIN
endif
# external PCI-E 7610 WiFi module (ALWAYS EXTPA/LNA CONFIG FOR THIS MODULE)
ifdef CONFIG_MT7610_AP
ifeq ($(CONFIG_RT_FIRST_IF_EXTERNAL_PA_EXTERNAL_LNA),y)
	# black sample
	$(ROMFSINST) -S	defaults/default-2T2R-7620-7610-EXTPA-EXTLNA			/etc/default/nvram_default
else
	# white/final sample
	$(ROMFSINST) -S	defaults/default-2T2R-7620-7610					/etc/default/nvram_default
endif
	mkdir -p $(ROMFSDIR)/etc/Wireless/iNIC
	$(ROMFSINST) -S MT7610E-V10-FEM-EXTPALNA.bin					/etc/Wireless/iNIC/MT7610E-V10-FEM.bin
ifdef CONFIG_RALINK_MT7620
ifeq ($(CONFIG_RT_FIRST_IF_INTERNAL_PA_INTERNAL_LNA),y)
	$(ROMFSINST) -S MT7620_7610_AP_2T2R-4L_internal_LNA_internal_PA_BACKUP.BIN	/etc/Wireless/rfbackup.bin
else ifeq ($(CONFIG_RT_FIRST_IF_INTERNAL_PA_EXTERNAL_LNA),y)
	$(ROMFSINST) -S MT7620_7610_AP_2T2R-4L_external_LNA_internal_PA_BACKUP.BIN	/etc/Wireless/rfbackup.bin
else ifeq ($(CONFIG_RT_FIRST_IF_EXTERNAL_PA_EXTERNAL_LNA),y)
	$(ROMFSINST) -S MT7620_7610_AP_2T2R-4L_external_LNA_external_PA_BACKUP.BIN	/etc/Wireless/rfbackup.bin
endif
endif
endif
# external PCI-E 76x2 WiFi module
ifdef CONFIG_MT76X2_AP
	mkdir -p $(ROMFSDIR)/etc/Wireless/iNIC
# 7602 module
ifdef CONFIG_RT_FIRST_IF_MT7602E
ifeq ($(CONFIG_RT_FIRST_IF_INTERNAL_PA_INTERNAL_LNA),y)
	$(ROMFSINST) -S MT7602E_EEPROM-FEM-INTERNALPALNA.bin				/etc/Wireless/MT7602E_EEPROM.bin
else ifeq ($(CONFIG_RT_FIRST_IF_INTERNAL_PA_EXTERNAL_LNA),y)
	$(ROMFSINST) -S MT7602E_EEPROM-FEM-INTERNALPAEXERNALLNA.bin			/etc/Wireless/MT7602E_EEPROM.bin
else ifeq ($(CONFIG_RT_FIRST_IF_EXTERNAL_PA_EXTERNAL_LNA),y)
	$(ROMFSINST) -S MT7602E_EEPROM-FEM-EXTERNALPALNA.bin				/etc/Wireless/MT7602E_EEPROM.bin
endif
	$(ROMFSINST) -s	/etc/Wireless/MT7602E_EEPROM.bin				/etc/Wireless/iNIC/MT7602E_EEPROM.bin
endif
# 7612 module
ifdef CONFIG_RT_SECOND_IF_MT7612E
ifeq ($(CONFIG_RT_SECOND_IF_INTERNAL_PA_INTERNAL_LNA),y)
	$(ROMFSINST) -S MT7612E_EEPROM-FEM-INTERNALPALNA.bin				/etc/Wireless/MT7612E_EEPROM.bin
else ifeq ($(CONFIG_RT_SECOND_IF_INTERNAL_PA_EXTERNAL_LNA),y)
	$(ROMFSINST) -S MT7612E_EEPROM-FEM-INTERNALPAEXTERNALLNA.bin			/etc/Wireless/MT7612E_EEPROM.bin
else ifeq ($(CONFIG_RT_SECOND_IF_EXTERNAL_PA_EXTERNAL_LNA),y)
	$(ROMFSINST) -S MT7612E_EEPROM-FEM-EXTERNALPALNA.bin				/etc/Wireless/MT7612E_EEPROM.bin
endif
	$(ROMFSINST) -s	/etc/Wireless/MT7612E_EEPROM.bin				/etc/Wireless/iNIC/MT7612E_EEPROM.bin
endif
# full backups per devices
ifdef CONFIG_RALINK_MT7620
ifdef CONFIG_RT_SECOND_IF_INTERNAL_PA_INTERNAL_LNA
	$(ROMFSINST) -S MT7620_7612_AP_2T2R-4L_internal_LNA_internal_PA_BACKUP.BIN	/etc/Wireless/rfbackup.bin
else
	$(ROMFSINST) -S MT7620_7612_AP_2T2R-4L_external_LNA_external_PA_BACKUP.BIN	/etc/Wireless/rfbackup.bin
endif
	$(ROMFSINST) -S	defaults/default-2T2R-7620-7612					/etc/default/nvram_default
endif
ifdef CONFIG_RALINK_MT7621
	$(ROMFSINST) -S MT7621_MT7602_MT7612_BACKUP.BIN					/etc/Wireless/rfbackup.bin
	$(ROMFSINST) -S	defaults/default-2T2R-7621-7602-7612				/etc/default/nvram_default
endif
endif

#install profiles symlinks for all chips
	mkdir -p $(ROMFSDIR)/etc/Wireless/RT2860AP
	mkdir -p $(ROMFSDIR)/etc/Wireless/RT2860STA
	$(ROMFSINST) -s /etc/Wireless/RT2860/RT2860.dat /etc/Wireless/RT2860AP/RT2860AP.dat
	$(ROMFSINST) -s /etc/Wireless/RT2860/RT2860.dat /etc/Wireless/RT2860STA/RT2860STA.dat
