#allways agressive optimize for size
CFLAGS	+= -ffunction-sections -fdata-sections
LDFLAGS	+= -Wl,--gc-sections

UTILSLIST := sfppi-generic sfppi-vendor

#Uncomment for Beaglebone
BEAGLEBONE =aa

default: sfppi-generic sfppi-vendor

sfppi-generic:
ifndef BEAGLEBONE
	$(CC) $(CFLAGS) $(LDFLAGS) -o sfppi-generic sfppi-generic.c -lwiringPi -lm
else
	$(CC) $(CFLAGS) $(LDFLAGS) -DBEAGLEBONE -o sfppi-generic sfppi-generic.c -lm
endif

sfppi-vendor:
ifndef BEAGLEBONE
	$(CC) $(CFLAGS) $(LDFLAGS) -o sfppi-vendor sfppi-vendor.c -lwiringPi -lcrypto -lz -lm
else
	$(CC) $(CFLAGS) $(LDFLAGS) -DBEAGLEBONE -o sfppi-vendor sfppi-vendor.c -lcrypto -lz -lm
endif

clean:
	$(RM) sfppi-generic sfppi-vendor 

romfs:
	for i in $(UTILSLIST); do $(ROMFSINST) -S $$i /bin/$$i; done
