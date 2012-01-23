# Makefile for /Hello folder


CFLAGS = -g -O0 --verbose
FRAMEWORKS = -framework Cocoa
IBTOOL = ibtool

SOURCES = main.m AppDelegate.m HelloView.m
XIBDIR = en.lproj
# XIBS = $(XIBDIR)MainMenu.xib
# NIBS = $(XIBDIR) $(XIBS:.xib=.nib)

all: imageflip

imageflip: $(SOURCES) $(XIBDIR)/MainMenu.nib Info.plist
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $(SOURCES) $(FRAMEWORKS)

$(XIBDIR)/MainMenu.nib: $(XIBDIR)/MainMenu.xib
	$(IBTOOL) $(XIBDIR)/MainMenu.xib --compile $(XIBDIR)/MainMenu.nib

Info.plist: $(XCODEPROJNAME)-Info.plist
	cp $(XCODEPROJNAME)-Info.plist Info.plist
	sed -e 's/$${EXECUTABLE_NAME}/$(PACKAGE)/' \
        -e 's/$${PRODUCT_NAME.*}/$(PACKAGE)/' \
        -e 's/$${MACOSX_DEPLOYMENT_TARGET}/$(MACOSXTARGET)/' \
        -e 's/$${NETSURF_VERSION}/$(VERSION)/' \
        < $(XCODEPROJNAME)-Info.plist > Info.plist

clean:
	rm -f $(PACKAGE)
	rm -rf $(PACKAGE).dSYM
	rm -f Info.plist

.PHONY: all clean