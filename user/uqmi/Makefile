########################################################################
# wrapper for apps correct build only in one thread
########################################################################
.EXPORT_ALL_VARIABLES:

DIRS = src

all:
	for i in $(DIRS) ; do make -j1 -C $$i $@ || exit $?; done

romfs:
	for i in $(DIRS) ; do make -j1 -C $$i $@ || exit $?; done

clean:
	for i in $(DIRS) ; do make -C $$i clean ; done
