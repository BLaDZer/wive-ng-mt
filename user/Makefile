#
#	Makefile -- Build instructions for user level apps
#

.EXPORT_ALL_VARIABLES:
.PHONY: all romfs clean

#
# Include architecture specific build rules.
#

ifndef ROOTDIR
ROOTDIR=..
endif

UCLINUX_BUILD_USER=1
-include $(LINUX_CONFIG)
-include $(CONFIG_CONFIG)
-include $(ARCH_CONFIG)

ifdef CONFIG_RALINK_MT7620
ifdef CONFIG_RAETH_ESW
CFLAGS += -DCONFIG_RAETH_ESW -DCONFIG_RALINK_MT7620
endif
endif

ifdef CONFIG_RALINK_MT7621
ifdef CONFIG_MT7530_GSW
CFLAGS += -DCONFIG_MT7530_GSW -DCONFIG_RALINK_MT7621
endif
endif

ifdef CONFIG_RALINK_MT7628
ifdef CONFIG_RAETH_ESW
CFLAGS += -DCONFIG_RAETH_ESW -DCONFIG_RALINK_MT7628
endif
endif

#for all cleanap
SUBDIRS_APP = $(shell LC_ALL=C; ls)

#
# must run the vendor build first
#
VEND=$(ROOTDIR)/vendors
dir_v = $(VEND)/$(CONFIG_VENDOR)/.

dir_y =
dir_n =
dir_  =

#base system application
dir_$(CONFIG_USER_WIRELESS_TOOLS)           += wireless_tools
dir_$(CONFIG_USER_WIRELESS_IW_TOOL)         += iw
dir_$(CONFIG_USER_IPTABLES_IPTABLES)        += iptables
dir_$(CONFIG_USER_IPTABLES_IP6TABLES)       += iptables
dir_$(CONFIG_USER_CDP)			    += cdp-tools
dir_$(CONFIG_USER_LLTD)			    += lldt
dir_$(CONFIG_USER_LLDPD)		    += lldpd
dir_$(CONFIG_USER_INADYN)                   += inadyn
dir_$(CONFIG_USER_WPA_SUPPLICANT)	    += wpa_supplicant
dir_$(CONFIG_USER_DNSMASQ)                  += dnsmasq
dir_$(CONFIG_USER_DROPBEAR)                 += dropbear
dir_$(CONFIG_USER_IPROUTE2)                 += iproute2
dir_$(CONFIG_USER_PPPD)                	    += ppp
dir_$(CONFIG_USER_XL2TPD)		    += xl2tpd
dir_$(CONFIG_USER_PARPROUTED)		    += parprouted
dir_$(CONFIG_USER_IGMP_PROXY)               += igmpproxy
dir_$(CONFIG_USER_ETHTOOL)                  += ethtool
dir_$(CONFIG_USER_UDPXY)                    += udpxy
dir_$(CONFIG_USER_WIVE)			    += wive-utils
dir_$(CONFIG_USER_KABINET)		    += kabinet
dir_$(CONFIG_USER_MINIUPNPD)                += miniupnp
dir_$(CONFIG_USER_XUPNPD)		    += xupnpd
dir_$(CONFIG_USER_RADVD)               	    += radvd
dir_$(CONFIG_USER_DHCP6)                    += dhcp6
dir_$(CONFIG_USER_ZEBRA)		    += quagga
dir_$(CONFIG_USER_SNMPD)                    += snmpd
dir_$(CONFIG_USER_TCPDUMP)		    += tcpdump

#for usb support
dir_$(CONFIG_USB_MODESWITCH)                += usb_modeswitch
dir_$(CONFIG_USER_P910ND)		    += p910nd
dir_$(CONFIG_USER_NTFS3G)                   += ntfs-3g
dir_$(CONFIG_USER_DAVFS)		    += davfs
dir_$(CONFIG_USER_SAMBA)		    += samba
dir_$(CONFIG_USER_TRANSMISSION)		    += transmission
dir_$(CONFIG_USER_STORAGE)		    += usb

#only ralink apps and scripts
dir_$(CONFIG_RT2880_APP)		    += rt2880_app

#need last busybox and goahead
dir_$(CONFIG_USER_BUSYBOX_BUSYBOX)          += busybox
dir_$(CONFIG_USER_GOAHEAD_HTTPD)	    += goahead

# multicore tools
ifdef CONFIG_RALINK_MT7621
dir_$(CONFIG_USER_IRQBALANCE)		    += irqbalance
endif

all:
	# ">>>>>>>>>>>>>> CONFIGURE AND BUILD USERAPPS  <<<<<<<<<<<<<<<<<<<"
	for i in $(dir_y) ; do \
	    if [ -f "$$i/DoConfigure.sh" ]; then ( cd $$i; ./DoConfigure.sh || exit 1 ; cd .. ) || exit 1 ; fi ; \
	    if [ -d "$$i" ]; then ( cd $$i; $(MAKE) -j$(HOST_NCPU) || exit 1 ; cd .. ) || exit 1 ; fi  \
	done

romfs:
	# ">>>>>>>>>>>>>>>>>>>>> INSTALL USERAPPS  <<<<<<<<<<<<<<<<<<<<<<<<"
	for i in $(sort $(dir_y)) ; do \
		[ ! -d $$i ] || $(MAKE) -C $$i romfs || exit $$? ; \
	done

clean:
	# ">>>>>>>>>>>>>>>>>>>>>>> CLEAN USERAPPS  <<<<<<<<<<<<<<<<<<<<<<<<"
	-for i in $(SUBDIRS_APP); do $(MAKE) -C $$i clean; $(RM) -rf $(ROOTDIR)/user/$$i/filesystem/*; done
	-for i in $(SUBDIRS_APP); do $(MAKE) -C $$i distclean; $(RM) -rf $(ROOTDIR)/user/$$i/filesystem/*; done
	-for i in $(SUBDIRS_APP); do $(MAKE) -C $$i mrproper; $(RM) -rf $(ROOTDIR)/user/$$i/filesystem/*; done

web:
	$(MAKE) -C goahead all
