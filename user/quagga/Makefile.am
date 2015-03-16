## Process this file with automake to produce Makefile.in.

CFLAGS	+= -ffunction-sections -fdata-sections
LDFLAGS	+= -Wl,--gc-sections

SUBDIRS = lib @ZEBRA@ @BGPD@ @RIPD@ @RIPNGD@ @OSPFD@ @OSPF6D@ @BABELD@ \
         @ISISD@ @PIMD@ @WATCHQUAGGA@ @VTYSH@ @OSPFCLIENT@ @DOC@ m4 @pkgsrcdir@ \
         redhat @SOLARIS@ tests

DIST_SUBDIRS = lib zebra bgpd ripd ripngd ospfd ospf6d babeld \
	  isisd watchquagga vtysh ospfclient doc m4 pkgsrc redhat tests \
	  solaris pimd

EXTRA_DIST = aclocal.m4 SERVICES TODO REPORTING-BUGS INSTALL.quagga.txt \
	update-autotools \
	vtysh/Makefile.in vtysh/Makefile.am \
	tools/rrcheck.pl tools/rrlookup.pl tools/zc.pl \
	tools/zebra.el tools/multiple-bgpd.sh \
	fpm/fpm.h

if HAVE_LATEX

HACKING.pdf: HACKING.tex
	$(LATEXMK) -pdf $<

clean-local:
	-$(LATEXMK) -C HACKING.tex

endif

ACLOCAL_AMFLAGS = -I m4

romfs: install
	$(ROMFSINST) -S init.d/W65ripd /etc/rc.d/W65ripd
	$(ROMFSINST) -s /etc/rc.d/W65ripd /etc/init.d/ripd
	$(ROMFSINST) -S init.d/W60zebra /etc/rc.d/W60zebra
	$(ROMFSINST) -s /etc/rc.d/W60zebra /etc/init.d/zebra
	$(ROMFSINST) -S filesystem/sbin/zebra /bin/zebra
	$(ROMFSINST) -S filesystem/sbin/ripd /bin/ripd
	cp -rfva filesystem/lib/*.so* $(ROMFSDIR)/lib/
	rm -rf filesystem
