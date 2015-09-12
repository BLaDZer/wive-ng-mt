all:

clean:

romfs:
	mkdir -p $(ROMFSDIR)/opt
	mkdir -p $(ROMFSDIR)/etc/scripts
#install scripts
	$(ROMFSINST) -S automount.sh /etc/scripts/automount.sh
	$(ROMFSINST) -S umount_all.sh /etc/scripts/umount_all.sh
	$(ROMFSINST) -S storage.sh /etc/scripts/storage.sh
	$(ROMFSINST) -S usbctrl.sh /etc/scripts/usbctrl.sh
	$(ROMFSINST) -S prnctrl.sh /etc/scripts/prnctrl.sh
	$(ROMFSINST) -S entware_install.sh /etc/scripts/entware_install.sh
#create symlinks
	$(ROMFSINST) -s /etc/scripts/automount.sh /bin/automount.sh
	$(ROMFSINST) -s /etc/scripts/umount_all.sh /bin/umount_all.sh
	$(ROMFSINST) -s /etc/scripts/storage.sh /bin/storage.sh
	$(ROMFSINST) -s /etc/scripts/usbctrl.sh /bin/usbctrl.sh
	$(ROMFSINST) -s /etc/scripts/prnctrl.sh /bin/prnctrl.sh
	$(ROMFSINST) -s /etc/scripts/entware_install.sh /bin/entware_install.sh
ifdef CONFIG_USB_MODESWITCH
	$(ROMFSINST) -S modems.conf /etc/modems.conf
	$(ROMFSINST) -S init.d/W85modemhelper /etc/rc.d/W85modemhelper
	$(ROMFSINST) -s /etc/rc.d/W85modemhelper /etc/init.d/modemhelper
	$(ROMFSINST) -S modem.sh /etc/scripts/modem.sh
	$(ROMFSINST) -s /etc/scripts/modem.sh /bin/modem.sh
endif
