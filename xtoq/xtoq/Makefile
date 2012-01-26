
CFLAGS = -Wall -g -O0
INCLUDES = -I../../src -I/opt/X11/include
LDFLAGS = -L/opt/X11/lib -lxcb-util -lxcb-damage -lxcb-composite -lxcb-xfixes -lxcb-render -lxcb-shape -lxcb-image -lxcb-shm -lxcb
FRAMEWORKS = Foundation
IBTOOL = ibtool

SOURCES = main.m AppController.m AppDelegate.m DisplayNumberController.m XtoqImageRep.m XtoqView.m
XIBDIR = en.lproj

all: $(PACKAGE)

$(PACKAGE): $(SOURCES) $(XIBDIR)/MainMenu.nib Info.plist
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o $@ $(SOURCES) -framework $(FRAMEWORKS)

$(XIBDIR)/MainMenu.nib: $(XIBDIR)/MainMenu.xib
	$(IBTOOL) $(XIBDIR)/MainMenu.xib --compile $(XIBDIR)/MainMenu.nib

$(XIBDIR)/DisplayNumberDialog.nib: DisplayNumberDialog.xib
	$(IBTOOL) DisplayNumberDialog.xib --compile $(XIBDIR)/DisplayNumberDialog.nib

Info.plist: $(PACKAGE)-Info.plist
	cp $(PACKAGE)-Info.plist Info.plist
	sed -e 's/$${EXECUTABLE_NAME}/$(PACKAGE)/' \
        -e 's/$${PRODUCT_NAME.*}/$(PACKAGE)/' \
        -e 's/$${MACOSX_DEPLOYMENT_TARGET}/$(MACOSXTARGET)/' \
        -e 's/$${NETSURF_VERSION}/$(VERSION)/' \
        < $(PACKAGE)-Info.plist > Info.plist

clean:
	rm -f $(PACKAGE)
	rm -rf $(PACKAGE).dSYM
	rm -f Info.plist

.PHONY: all clean