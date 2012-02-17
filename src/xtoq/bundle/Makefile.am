APPLE_APPLICATIONS_DIR=/Applications
APPLE_APPLICATION_NAME=XtoQ

EXTRA_DIST = \
	mk_bundke.sh \
	Info.plist \
	Resources/English.lproj/Credits.rtf \
	Resources/English.lproj/InfoPlist.strings

install-data-hook:
	$(srcdir)/mk_bundke.sh $(srcdir) $(builddir) $(DESTDIR)/$(APPLE_APPLICATIONS_DIR)/$(APPLE_APPLICATION_NAME).app install

uninstall-hook:
	$(RM) -rf $(DESTDIR)$(APPLE_APPLICATIONS_DIR)/$(APPLE_APPLICATION_NAME).app/Contents/Resources
	$(RM) -rf $(DESTDIR)$(APPLE_APPLICATIONS_DIR)/$(APPLE_APPLICATION_NAME).app/Contents/Info.plist
	$(RM) -rf $(DESTDIR)$(APPLE_APPLICATIONS_DIR)/$(APPLE_APPLICATION_NAME).app/Contents/PkgInfo
