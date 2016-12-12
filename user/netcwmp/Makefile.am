SUBDIRS = libpool/src libxmlet/src libcwmp/src cwmpd/src
DIST_SUBDIRS = libpool/src libxmlet/src libcwmp/src cwmpd/src
ACLOCAL_AMFLAGS = -I m4

install-data-local:
	-install conf/cwmp.conf  $(prefix)/etc/cwmp.conf;
	-install conf/device.xml  $(prefix)/etc/device.xml;
	-install conf/device_dlink_101.xml  $(prefix)/etc/device_dlink_101.xml;
	-install conf/device_dlink_115.xml  $(prefix)/etc/device_dlink_115.xml;
	-install conf/device_tr-098-1-8-0.xml  $(prefix)/etc/device_tr-098-1-8-0.xml;

romfs:
	$(ROMFSINST) -S conf/cwmp.conf /etc/cwmp.conf
	$(ROMFSINST) -S conf/device.xml /etc/device.xml
	$(ROMFSINST) -S conf/device_dlink_101.xml /etc/device_dlink_101.xml
	$(ROMFSINST) -S conf/device_dlink_115.xml /etc/device_dlink_115.xml
	$(ROMFSINST) -S conf/device_tr-098-1-8-0.xml /etc/device_tr-098-1-8-0.xml
	$(ROMFSINST) -S cwmpd/src/cwmpd /bin/cwmpd
	$(ROMFSINST) -S ../init.d/W86cwmpd /etc/rc.d/W86cwmpd
	$(ROMFSINST) -s /etc/rc.d/W86cwmpd /etc/init.d/cwmpd
	rm -rf filesystem
