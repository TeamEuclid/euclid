#
# A GNUmakefile
#

# Check that the GNUSTEP_MAKEFILES environment variable is set
ifeq ($(GNUSTEP_MAKEFILES),)
GNUSTEP_MAKEFILES := $(shell gnustep-config --variable=GNUSTEP_MAKEFILES 2>/dev/null)
 ifeq ($(GNUSTEP_MAKEFILES),)
   $(error You need to set GNUSTEP_MAKEFILES before compiling!)
 endif
endif

# Include the common variables
include $(GNUSTEP_MAKEFILES)/common.make

# Build an Objective-C program
TOOL_NAME = Moder

# Objective-C files requiring compilation
Moder_OBJC_FILES =  ModUtil.m main.m

-include GNUmakefile.preamble

# Include in the rules for making command line tools
include $(GNUSTEP_MAKEFILES)/tool.make

-include GNUmakefile.postamble