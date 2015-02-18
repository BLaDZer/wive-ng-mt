all:

clean:

romfs:
	mkdir -p $(ROMFSDIR)/etc/Wireless
	mkdir -p $(ROMFSDIR)/etc/default
	mkdir -p $(ROMFSDIR)/etc/Wireless/RT2860
# internal WiFi module
ifdef CONFIG_RALINK_MT7620
	# Install defaults
	$(ROMFSINST) -S	defaults/default-2T2R						/etc/default/nvram_default
	rm -f MT7620_AP_2T2R-4L_V15.BIN
ifeq ($(CONFIG_INTERNAL_PA_INTERNAL_LNA),y)
	# Handle several power design for MT7620 - iPA + iLNA
	cp -f MT7620_AP_2T2R-4L_internal_LNA_internal_PA_V15.BIN			MT7620_AP_2T2R-4L_V15.BIN
else ifeq ($(CONFIG_INTERNAL_PA_EXTERNAL_LNA),y)
	# Handle several power design for MT7620 - iPA + eLNA
	cp -f MT7620_AP_2T2R-4L_external_LNA_internal_PA_V15.BIN			MT7620_AP_2T2R-4L_V15.BIN
else ifeq ($(CONFIG_EXTERNAL_PA_EXTERNAL_LNA),y)
	# Handle several power design for MT7620 - ePA + eLNA
	cp -f MT7620_AP_2T2R-4L_external_LNA_external_PA_V15.BIN			MT7620_AP_2T2R-4L_V15.BIN
else
	# Handle several power design for MT7620 - iLAN+iPA as default.
	cp -f MT7620_AP_2T2R-4L_internal_LNA_internal_PA_V15.BIN			MT7620_AP_2T2R-4L_V15.BIN
endif
	$(ROMFSINST) -S	MT7620_AP_2T2R-4L_V15.BIN					/etc/Wireless/MT7620_AP_2T2R-4L_V15.BIN
	$(ROMFSINST) -s /etc/Wireless/MT7620_AP_2T2R-4L_V15.BIN 			/etc/Wireless/rf.bin
	rm -f MT7620_AP_2T2R-4L_V15.BIN
	rm -f MT7620_AP_2T2R-4L_BACKUP.BIN
endif
# external PCI-E WiFi module
ifdef CONFIG_MT7610_AP
	# Install defaults
	$(ROMFSINST) -S	defaults/default-2T2R-7620-7610					/etc/default/nvram_default
	mkdir -p $(ROMFSDIR)/etc/Wireless/iNIC
	$(ROMFSINST) -S MT7610E-V10-FEM.bin						/etc/Wireless/iNIC/MT7610E-V10-FEM.bin
	#install fabric calibrations backup
ifeq ($(CONFIG_INTERNAL_PA_INTERNAL_LNA),y)
	$(ROMFSINST) -S MT7620_7610_AP_2T2R-4L_internal_LNA_internal_PA_BACKUP.BIN	/etc/Wireless/rfbackup.bin
else ifeq ($(CONFIG_INTERNAL_PA_EXTERNAL_LNA),y)
	$(ROMFSINST) -S MT7620_7610_AP_2T2R-4L_external_LNA_internal_PA_BACKUP.BIN	/etc/Wireless/rfbackup.bin
else ifeq ($(CONFIG_EXTERNAL_PA_EXTERNAL_LNA),y)
	$(ROMFSINST) -S MT7620_7610_AP_2T2R-4L_external_LNA_external_PA_BACKUP.BIN	/etc/Wireless/rfbackup.bin
endif
endif
ifdef CONFIG_MT76X2_AP
	$(ROMFSINST) -S	defaults/default-2T2R-7621-7602-7612				/etc/default/nvram_default
	mkdir -p $(ROMFSDIR)/etc/Wireless/iNIC
	$(ROMFSINST) -S MT7602E_EEPROM.bin						/etc/Wireless/MT7602E_EEPROM.bin
	$(ROMFSINST) -S MT7612E_EEPROM.bin						/etc/Wireless/MT7612E_EEPROM.bin
	$(ROMFSINST) -s	/etc/Wireless/MT7602E_EEPROM.bin				/etc/Wireless/iNIC/MT7602E_EEPROM.bin
	$(ROMFSINST) -s	/etc/Wireless/MT7612E_EEPROM.bin				/etc/Wireless/iNIC/MT7612E_EEPROM.bin
ifdef CONFIG_RALINK_MT7621
	$(ROMFSINST) -S MT7621_MT7602_MT7612_BACKUP.BIN					/etc/Wireless/rfbackup.bin
endif
endif

#install profiles symlinks for all chips
	mkdir -p $(ROMFSDIR)/etc/Wireless/RT2860AP
	mkdir -p $(ROMFSDIR)/etc/Wireless/RT2860STA
	$(ROMFSINST) -s /etc/Wireless/RT2860/RT2860.dat /etc/Wireless/RT2860AP/RT2860AP.dat
	$(ROMFSINST) -s /etc/Wireless/RT2860/RT2860.dat /etc/Wireless/RT2860STA/RT2860STA.dat
