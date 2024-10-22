# Before including this in a build directory
# define OS_LAYER and CC, CFLAGS, etc.

.PHONY: clean all

# Files for the Daemon and the Test-Tool
DCFILES = $(OS_LAYER) main.c event.c util.c packetio.c band.c \
	state.c sessionmgr.c enumeration.c mapping.c seeslist.c \
	tlv.c qospktio.c

TCFILES = $(OS_LAYER) ctmain.c event.c ctpacketio.c util.c ctstate.c

DOBJFILES = $(patsubst %c,%o,$(DCFILES))
TOBJFILES = $(patsubst %c,%o,$(TCFILES))

# Note we do not define all here; that happens where included
# depending in which targets to build

clean:
	rm -f -- *~ lld2d lld2test $(DOBJFILES) $(TOBJFILES)

lld2d: $(DOBJFILES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(DOBJFILES)

lld2test: $(TOBJFILES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(TOBJFILES)

# End
