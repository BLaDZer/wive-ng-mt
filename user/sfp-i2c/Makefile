#allways agressive optimize for size
CFLAGS	+= -ffunction-sections -fdata-sections
LDFLAGS	+= -Wl,--gc-sections


all:
	$(CC) $(CFLAGS) $(LDFLAGS) -Wall --std c99 sfp-i2c.c -o sfp-i2c

install:
	#ok

clean:
	rm -f sfp-i2c

romfs:
	$(ROMFSINST) -S sfp-i2c /bin/sfp-i2c
