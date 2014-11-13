DIRLIST_ETC=playlists
DIRLIST_SHR=ui www profiles plugins

all:
	make -C src embedded

clean:
	make -C src clean

install:
	#OK

romfs:
	$(ROMFSINST) -S init.d/W75xupnpd /etc/rc.d/W75xupnpd
	$(ROMFSINST) -s /etc/rc.d/W75xupnpd /etc/init.d/xupnpd
	cp -fv src/xupnpd $(ROMFSDIR)/bin/xupnpd
	rm -rf $(ROMFSDIR)/etc/xupnpd
	mkdir -p $(ROMFSDIR)/etc/xupnpd
	cp -fv src/*.lua $(ROMFSDIR)/etc/xupnpd
	mkdir -p $(ROMFSDIR)/etc/xupnpd/config
	for i in $(DIRLIST_ETC); do cp -apr src/$$i $(ROMFSDIR)/etc/xupnpd/$$i; done
	mkdir -p $(ROMFSDIR)/share/xupnpd
	for i in $(DIRLIST_SHR); do cp -apr src/$$i $(ROMFSDIR)/share/xupnpd/$$i; done
