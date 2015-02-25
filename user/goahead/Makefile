.EXPORT_ALL_VARIABLES:

DIRS = src cgi-src
ROOT_DIRECTORY = /web

all:
	for i in $(DIRS) ; do make -j$(HOST_NCPU) -C $$i $@ || exit $?; done

romfs:
	$(ROMFSINST) $(ROOT_DIRECTORY)
ifeq ("$(CONFIG_RT2860V2_STA)", "")
	rm -rf $(ROMFSDIR)$(ROOT_DIRECTORY)/station
endif
ifeq ("$(CONFIG_RT2860V2_RT3XXX_AP_ANTENNA_DIVERSITY)", "")
ifeq ("$(CONFIG_RT2860V2_RT3XXX_STA_ANTENNA_DIVERSITY)", "")
	rm -f $(ROMFSDIR)$(ROOT_DIRECTORY)/wireless/ant_diversity.asp
endif
endif
ifeq ("$(CONFIG_USER_CHILLISPOT)", "")
	rm -rf $(ROMFSDIR)$(ROOT_DIRECTORY)/internet/hotspot.asp
endif
ifeq ("$(CONFIG_USB)", "")
	rm -rf $(ROMFSDIR)$(ROOT_DIRECTORY)/usb
endif
ifeq ("$(CONFIG_USER_STORAGE)", "")
	rm -rf $(ROMFSDIR)$(ROOT_DIRECTORY)/storage
endif
ifeq ("$(CONFIG_USER_P910ND)", "")
	rm -rf $(ROMFSDIR)$(ROOT_DIRECTORY)/usb/P910NDprintersrv.asp
endif
ifeq ("$(CONFIG_USB_MODESWITCH)", "")
	rm -rf $(ROMFSDIR)$(ROOT_DIRECTORY)/usb/USBmodem.asp
endif
	for i in $(DIRS) ; do make -C $$i $@ ; done
	rm -f $(ROMFSDIR)$(ROOT_DIRECTORY)/cgi-bin/.keep_me

clean:
	for i in $(DIRS) ; do make -C $$i clean ; done
