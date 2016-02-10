CFLAGS += -pipe -fpack-struct
CFLAGS += -ffunction-sections -fdata-sections
LDFLAGS += -Wl,--gc-sections

all: cdp-send

cdp-send: cdp-send.o

clean:
	rm -f cdp-send.o cdp-send

romfs:
	$(ROMFSINST) -S init.d/W82cdp /etc/rc.d/W82cdp
	$(ROMFSINST) -s /etc/rc.d/W82cdp /etc/init.d/cdp
	$(ROMFSINST) -S /bin/cdp-send
