# Copyright (c) Jess VanDerwalker <washu@sonic.net>
#
# Top level Makefile for the xtoq project
#

# May want to change prefix to local directory when testing
prefix = /usr/local
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin

export prefix
export exec_prefix
export bindir

PACKAGE = xtoq
VERSION = 0.1
APPNAME = $(PACKAGE).app
XCODESRCDIR = xtoq/xtoq
XTOQSRCDIR = src
RESDIR = resources
MACOSTARGET = 10.7

export PACKAGE
export VERSION
export APPNAME
export XCODESRCDIR
export XTOQSRCDIR
export RESDIR
export MACOSTARGET

all clean $(PACKAGE) install uninstall:
	cd $(XTOQSRCDIR) && $(MAKE) $@
	cd $(XCODESRCDIR) && $(MAKE) $@

$(APPNAME): FORCE
	mkdir -p $(APPNAME)/Contents/MacOS
	cp $(XCODESRCDIR)/$(PACKAGE) $(APPNAME)/Contents/MacOS
	mkdir -p $(APPNAME)/Contents/Resources/en.lproj
	cp -r $(XCODESRCDIR)/en.lproj $(APPNAME)/Contents/Resources/
	cp $(RESOURCEDIR)/* $(APPNAME)/Contents/Resources/
	cp $(XCODESRCDIR)/Info.plist $(APPNAME)/Contents/.
	echo 'APPL????' > $(APPNAME)/Contents/PkgInfo

FORCE:
	rm -rf $(APPNAME)

.PHONY: FORCE all clean install