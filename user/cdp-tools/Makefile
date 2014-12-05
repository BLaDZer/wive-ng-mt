CFLAGS += -Os -pipe -fpack-struct -Wall -Wno-unused -fno-caller-saves -fno-reorder-blocks -fno-aggressive-loop-optimizations
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
