#allways agressive optimize for size
CFLAGS	+= -ffunction-sections -fdata-sections -fvisibility=hidden
LDFLAGS	+= -Wl,--gc-sections

all: eoip

eoip: eoipcr.c libnetlink.o
	@set -e;
	$(CC) $(CFLAGS) $(LDFLAGS) -Wall -Os -o eoip eoipcr.c libnetlink.o

libnetlink.o: libnetlink.c libnetlink.h
	@set -e;
	$(CC) $(CFLAGS) $(LDFLAGS) -Wall -Os -c libnetlink.c

clean:
	rm -f eoip libnetlink.o

romfs:
	@set -e;
	$(ROMFSINST) -S eoip /bin/eoip
