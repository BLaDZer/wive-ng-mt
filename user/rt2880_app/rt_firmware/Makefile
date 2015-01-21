all:

clean:

romfs:
	mkdir -p $(ROMFSDIR)/etc/Wireless
	mkdir -p $(ROMFSDIR)/etc/default
	mkdir -p $(ROMFSDIR)/etc/Wireless/RT2860
	# Install defaults
	$(ROMFSINST) -S	defaults/default-2T2R				/etc/default/nvram_default
	# Install wifi module calibrations
# internal WiFi module
ifdef CONFIG_RALINK_MT7620
	rm -f MT7620_AP_2T2R-4L_V15.BIN
ifeq	($(CONFIG_INTERNAL_PA_INTERNAL_LNA),y)
	# Handle several power design for MT7620 - iPA + iLNA
	mv -f MT7620_AP_2T2R-4L_internal_LNA_internal_PA_V15.BIN	MT7620_AP_2T2R-4L_V15.BIN
else ifeq ($(CONFIG_INTERNAL_PA_EXTERNAL_LNA),y)
	# Handle several power design for MT7620 - iPA + eLNA
	mv -f MT7620_AP_2T2R-4L_external_LNA_internal_PA_V15.BIN	MT7620_AP_2T2R-4L_V15.BIN
else ifeq ($(CONFIG_EXTERNAL_PA_EXTERNAL_LNA),y)
	# Handle several power design for MT7620 - ePA + eLNA
	mv -f MT7620_AP_2T2R-4L_external_LNA_external_PA_V15.BIN	MT7620_AP_2T2R-4L_V15.BIN
else
	# Handle several power design for MT7620 - iLAN+iPA as default.
	mv -f MT7620_AP_2T2R-4L_internal_LNA_internal_PA_V15.BIN	MT7620_AP_2T2R-4L_V15.BIN
endif
	$(ROMFSINST) -S	MT7620_AP_2T2R-4L_V15.BIN			/etc/Wireless/MT7620_AP_2T2R-4L_V15.BIN
	$(ROMFSINST) -s /etc/Wireless/MT7620_AP_2T2R-4L_V15.BIN 	/etc/Wireless/rf.bin
	rm -f MT7620_AP_2T2R-4L_V15.BIN
endif
# external PCI-E WiFi module
ifdef CONFIG_MT7610_AP
	mkdir -p $(ROMFSDIR)/etc/Wireless/iNIC
	$(ROMFSINST) -S MT7610E-V10-FEM.bin				/etc/Wireless/iNIC/MT7610E-V10-FEM.bin
ifdef CONFIG_MT7610_AP_SINGLE_SKU
	$(ROMFSINST) -S SingleSKU_5G.dat				/etc/Wireless/iNIC/SingleSKU.dat
endif
endif
ifdef CONFIG_MT76X2_AP
	mkdir -p $(ROMFSDIR)/etc/Wireless/iNIC
	$(ROMFSINST) -S MT7612E_EEPROM.bin				/etc/Wireless/iNIC/MT7612E_EEPROM.bin
endif
#install profiles symlinks for all chips
	mkdir -p $(ROMFSDIR)/etc/Wireless/RT2860AP
	mkdir -p $(ROMFSDIR)/etc/Wireless/RT2860STA
	$(ROMFSINST) -s /etc/Wireless/RT2860/RT2860.dat /etc/Wireless/RT2860AP/RT2860AP.dat
	$(ROMFSINST) -s /etc/Wireless/RT2860/RT2860.dat /etc/Wireless/RT2860STA/RT2860STA.dat
