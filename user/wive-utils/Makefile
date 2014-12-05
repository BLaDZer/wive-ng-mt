#based utilites for all routers

####################################
#UTILSLIST = cpu mcprobe ipget#
####################################
UTILSLIST = cpu mcprobe ipget

STRIP	?= strip

#allways agressive optimize for size
CFLAGS	+= -Os -fno-caller-saves -fno-reorder-blocks -fno-unwind-tables -fno-asynchronous-unwind-tables -femit-struct-debug-baseonly -fno-var-tracking
CFLAGS	+= -ffunction-sections -fdata-sections
LDFLAGS	+= -Wl,--gc-sections

all:
	@set -e;
	for i in $(UTILSLIST); do $(CC) $(CFLAGS) $(LDFLAGS) $$i.c -o $$i; $(STRIP) $$i; done

clean:
	@set -e;
	for i in $(UTILSLIST); do rm -rf $$i; done
	rm -rf filesystem

install:
	rm -rf filesystem
	mkdir filesystem
	mkdir filesystem/bin
	@set -e;
	for i in $(UTILSLIST); do cp -fva $$i ./filesystem/bin/$$i; done

romfs:
	rm -rf filesystem
	mkdir filesystem
	mkdir filesystem/bin
	@set -e;
	for i in $(UTILSLIST); do $(ROMFSINST) -S $$i /bin/$$i; done
