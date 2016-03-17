SUBDIRS = libpool/src libxmlet/src libcwmp/src cwmpd/src
DIST_SUBDIRS = libpool/src libxmlet/src libcwmp/src cwmpd/src
ACLOCAL_AMFLAGS = -I m4

install-data-local:
	-install conf/cwmp.conf  $(prefix)/etc/cwmp.conf;
	-install conf/device.xml  $(prefix)/etc/device.xml;

romfs:
	$(ROMFSINST) -S conf/cwmp.conf /etc/cwmp.conf
	$(ROMFSINST) -S conf/device.xml /etc/device.xml
	$(ROMFSINST) -S cwmpd/src/cwmpd /bin/cwmpd

	$(ROMFSINST) -S ../init.d/W86cwmpd /etc/rc.d/W86cwmpd
	$(ROMFSINST) -s /etc/rc.d/W86cwmpd /etc/init.d/cwmpd

	rm -rf filesystem
