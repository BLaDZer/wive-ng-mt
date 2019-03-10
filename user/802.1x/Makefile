-include $(LINUX_CONFIG)

# sync with drivers (mt7615 and later support 16mbssid with radius)
CFLAGS += -DMAX_MBSSID_NUM=8

# If you want to support multiple radius server for multiple bssid, add following line
CFLAGS += -DMULTIPLE_RADIUS

# allways enable for compat
CFLAGS += -DRADIUS_MAC_ACL_SUPPORT

# If you want to support passpoint release 2 feature
# CFLAGS +=  -DHOTSPOT_R2

# If you want to debug daemon, add following line
CFLAGS += -DDBG -DSYSLOG

CFLAGS += -Os -fPIC -fomit-frame-pointer -ffunction-sections -fdata-sections -fvisibility=hidden
LDFLAGS += -Wl,--gc-sections

OBJS =	rtdot1x.o eloop.o eapol_sm.o radius.o md5.o  \
	config.o ieee802_1x.o  \
	sta_info.o radius_client.o \
	ieee802_11_auth.o

-include $(OBJS:%.o=%.d)

all: rtdot1xd

rtdot1xd: $(OBJS)
	$(CC) -o rt2860apd $(OBJS)

clean:
	rm -f core *.o rt2860apd

romfs:
	$(ROMFSINST) rt2860apd /bin/rt2860apd
	ln -sf rt2860apd $(ROMFSDIR)/bin/rtinicapd
	$(ROMFSINST) -S ../init.d/W65apd /etc/rc.d/W65apd
	$(ROMFSINST) -s /etc/rc.d/W65apd /etc/init.d/apd
