EXEC = ralinkiappd
OBJS = rtmpiapp.o

# If you want to debug daemon, add following line
CFLAGS += -I./ -DDBG -DSYSLOG -ffunction-sections -fdata-sections
LDFLAGS += -Wl,--gc-sections

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

romfs:
	$(ROMFSINST) /sbin/$(EXEC)
	$(ROMFSINST) -S init.d/W68iappd /etc/rc.d/W68iappd
	$(ROMFSINST) -s /etc/rc.d/W68iappd /etc/init.d/iappd

clean:
	rm -f core *.o ralinkiappd
