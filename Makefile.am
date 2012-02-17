SUBDIRS = include src man

MAINTAINERCLEANFILES = ChangeLog INSTALL

INSTALL:
	$(INSTALL_CMD)

ChangeLog:
	$(CHANGELOG_CMD)

dist-hook: ChangeLog INSTALL

.PHONY: ChangeLog INSTALL

