all:

clean:

romfs:
	mkdir -p $(ROMFSDIR)/etc/Wireless
	mkdir -p $(ROMFSDIR)/etc/default
	mkdir -p $(ROMFSDIR)/etc/Wireless/RT2860
	# 2T2R MODE 7620 with internal LNA
	$(ROMFSINST) -S	MT7620_AP_2T2R-4L_V15.BIN			/etc/Wireless/MT7620_AP_2T2R-4L_V13.BIN
	$(ROMFSINST) -S	defaults/default-2T2R				/etc/default/nvram_default
	$(ROMFSINST) -s /etc/Wireless/MT7620_AP_2T2R-4L_V13.BIN 	/etc/Wireless/rf.bin
#install profiles symlinks for all chips
	mkdir -p $(ROMFSDIR)/etc/Wireless/RT2860AP
	mkdir -p $(ROMFSDIR)/etc/Wireless/RT2860STA
	$(ROMFSINST) -s /etc/Wireless/RT2860/RT2860.dat /etc/Wireless/RT2860AP/RT2860AP.dat
	$(ROMFSINST) -s /etc/Wireless/RT2860/RT2860.dat /etc/Wireless/RT2860STA/RT2860STA.dat
