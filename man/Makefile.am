libmandir = $(LIB_MAN_DIR)

# List your man pages here
libman_PRE = 

libman_DATA = $(libman_PRE:man=@LIB_MAN_SUFFIX@)

EXTRA_DIST = $(libman_PRE)
CLEANFILES = $(libman_DATA)
SUFFIXES = .$(LIB_MAN_SUFFIX) .man

# List your sed substitutions here:
MAN_SUBSTS += 	-e 's|__bundle_id_prefix__|$(BUNDLE_ID_PREFIX)|g'

.man.$(APP_MAN_SUFFIX):
	$(AM_V_GEN)$(SED) $(MAN_SUBSTS) < $< > $@

