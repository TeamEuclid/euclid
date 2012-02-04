# Copyright (c) Jess VanDerwalker <washu@sonic.net>
#
# Cocoa/Obj-C makefile
#

CC = clang
CFLAGS = -Wall -g -O0
INCLUDES = -I../../src -I/opt/X11/include
LDFLAGS = -L/opt/X11/lib -lxcb-util -lxcb-damage -lxcb-composite -lxcb-xfixes -lxcb-render -lxcb-shape -lxcb-image -lxcb-shm -lxcb
FRAMEWORKS = Cocoa AppKit
IBTOOL = ibtool

SOURCES = main.m XtoqApplication.m XtoqController.m XtoqWindow.m DisplayNumberController.m XtoqImageRep.m XtoqView.m
CSOURCES = ../../src/xtoq.c ../../src/util.c
COBJS = $(CSOURCES:.c=.o)
Xibdir = en.lproj

# Build the list of frameworks
FRAMEWORKFLAGS = $(addprefix -framework , $(FRAMEWORKS))
all: $(PACKAGE)

$(PACKAGE): $(SOURCES) $(SOURCES) Info.plist
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o $@ $(SOURCES) $(COBJS) $(FRAMEWORKFLAGS)

$CSOURCES:
	cd ../../src && $(MAKE)

$(XIBDIR)/DisplayNumberDialog.nib: DisplayNumberDialog.xib
	$(IBTOOL) DisplayNumberDialog.xib --compile $(XIBDIR)/DisplayNumberDialog.nib

Info.plist: $(PACKAGE)-Info.plist
	cp $(PACKAGE)-Info.plist Info.plist
	sed -e 's/$${EXECUTABLE_NAME}/$(PACKAGE)/' \
        -e 's/$${PRODUCT_NAME.*}/$(PACKAGE)/' \
        -e 's/$${MACOSX_DEPLOYMENT_TARGET}/$(MACOSXTARGET)/' \
        -e 's/$${NETSURF_VERSION}/$(VERSION)/' \
        < $(PACKAGE)-Info.plist > Info.plist

install:
	install -d $(bindir)/$(APPNAME)
	install -m 0755 xtoq $(bindir)/$(APPNAME)
	install -m 0655 xtoq-Info.plist $(bindir)/bin/$(APPNAME)
	install -d $(bindir)/$(APPNAME)/en.lproj
	install -m 0655 en.lproj/* $(bindir)/$(APPNAME)/en.lproj

uninstall:
	-rm $(bindir)/$(APPNAME)

clean:
	rm -f $(PACKAGE)
	rm -rf $(PACKAGE).dSYM
	rm -f Info.plist

.PHONY: all clean install uninstall