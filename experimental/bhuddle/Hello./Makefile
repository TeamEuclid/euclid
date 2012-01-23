# Top level Makefile for imageflip.app (which is using the Hello
# project from Xcode)

PACKAGE = imageflip
XCODEPROJNAME = Hello
VERSION = 0.1
APPNAME = $(PACKAGE).app
SOURCEDIR = Hello
RESOURCEDIR = Resources
MACOSTARGET = 10.7

export PACKAGE
export XCODEPROJNAME
export VERSION
export APPNAME
export SOURCEDIR
export RESOURCEDIR
export MACOSTARGET

all clean imageflip:
	cd $(SOURCEDIR) && $(MAKE) $@

$(APPNAME): FORCE
	mkdir -p $(APPNAME)/Contents/MacOS
	cp $(SOURCEDIR)/$(PACKAGE) $(APPNAME)/Contents/MacOS
	mkdir -p $(APPNAME)/Contents/Resources/en.lproj
	cp -r $(SOURCEDIR)/en.lproj $(APPNAME)/Contents/Resources/
	cp $(RESOURCEDIR)/* $(APPNAME)/Contents/Resources/
	cp $(SOURCEDIR)/Info.plist $(APPNAME)/Contents/.
	echo 'APPL????' > $(APPNAME)/Contents/PkgInfo

FORCE:
	rm -rf $(APPNAME)

.PHONY: FORCE all clean