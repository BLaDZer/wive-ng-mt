SUBDIRS = libpool/src libxmlet/src libcwmp/src cwmpd/src
install-data-local:
	-install conf/cwmp.conf  $(prefix)/etc/cwmp.conf;
	-install conf/device.xml  $(prefix)/etc/device.xml;

romfs:
	$(ROMFSINST) -S conf/cwmp.conf /etc/cwmp.conf
	$(ROMFSINST) -S conf/device.xml /etc/device.xml;
	$(ROMFSINST) -S cwmpd/src/cwmpd /bin/cwmpd
	rm -rf filesystem
