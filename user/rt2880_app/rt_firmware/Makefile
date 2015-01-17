all:

clean:

romfs:
	mkdir -p $(ROMFSDIR)/etc/Wireless
	mkdir -p $(ROMFSDIR)/etc/default
	mkdir -p $(ROMFSDIR)/etc/Wireless/RT2860
# 2T2R MODE 7620 with internal LNA
	$(ROMFSINST) -S	defaults/default-2T2R				/etc/default/nvram_default
ifdef CONFIG_RALINK_MT7620
	$(ROMFSINST) -S	MT7620_AP_2T2R-4L_V15.BIN			/etc/Wireless/MT7620_AP_2T2R-4L_V15.BIN
	$(ROMFSINST) -s /etc/Wireless/MT7620_AP_2T2R-4L_V15.BIN 	/etc/Wireless/rf.bin
endif
# external PCI-E modules
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
