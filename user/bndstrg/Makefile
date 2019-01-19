-include $(LINUX_CONFIG)

BNDSTRG_EXEC = bndstrg

ifndef CONFIG_MT7615_AP
LEGACYMODE := -DLEGACYMODE
endif

CFLAGS += -I./ -Wall -g -lrt -ffunction-sections -fdata-sections -fvisibility=hidden -DSYSLOG -DBND_STRG_DBG -DBND_STRG_QA $(LEGACYMODE)
LDFLAGS += -lpthread -lnvram -Wl,--gc-sections

ifdef LEGACYMODE
BNDSTRG_OBJS = bndstrg_legacy.o driver_wext_legacy.o eloop.o main_legacy.o netlink.o os_internal.o util.o debug.o
else
BNDSTRG_OBJS = bndstrg.o driver_wext.o eloop.o main.o netlink.o os_internal.o util.o debug.o
endif

all: $(BNDSTRG_EXEC)

$(BNDSTRG_EXEC): $(BNDSTRG_OBJS)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(LDFLAGS) -o $(BNDSTRG_EXEC) $(BNDSTRG_OBJS)

install:
	echo ok

romfs:
	$(ROMFSINST) /bin/$(BNDSTRG_EXEC)
	$(ROMFSINST) -S ../init.d/W66bndstr /etc/rc.d/W66bndstr
	$(ROMFSINST) -s /etc/rc.d/W66bndstr /etc/init.d/bndstr
clean:
	rm -f *.o $(BNDSTRG_EXEC)
