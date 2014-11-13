#allways agressive optimize for size
CFLAGS	+= -Os -fno-caller-saves -fno-unwind-tables -fno-asynchronous-unwind-tables -femit-struct-debug-baseonly -fno-var-tracking
CFLAGS	+= -ffunction-sections -fdata-sections
LDFLAGS	+= -Wl,--gc-sections

LIBS = md5.o rmd160.o
lanauth: lanauth.o $(LIBS)
	$(CC) $(CFLAGS) $(LDFLAGS) lanauth.o -o lanauth $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o lanauth

install:
	echo "Ok.."

romfs:
	$(ROMFSINST) -S lanauth /bin/lanauth
