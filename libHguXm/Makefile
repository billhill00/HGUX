#########################################################################
# Project:      Mouse Atlas						#
# Title:        Makefile		                               	#
# $Id$
# Date:         September 1994	                                    	#
# Author:       Bill Hill 				    		#
# $Revision$
# Purpose:      Makefile for building a library which provides		#
#		functions for the registration of woolz objects by	#
#		Mouse Atlas Project applications.			#
# Maintenance:	Log changes below, with most recent at top of list.	#
# 25-10-94 bill	Added C source file HGU_XmVarArgs.c.			#
# 01-09-94 bill	Created from template makefile.				#
#########################################################################

# Include the configuration makefile (modify as required).
include			../../Makefile.conf

LIBNAME			= libHguXm

# Name of archive (library) to be built (modify as required).
ARCHIVE			= $(LIBNAME).a

# Names of executables to be built (modify as required).
EXECUTABLES		=

# List of all 'C' source files (modify as required).
CSOURCES		= \
			HGU_XmDialogUtils.c \
			HGU_DrawingA.c \
			HGU_XmExtFF_FSBUtils.c \
			HGU_XmFileList.c \
			HGU_XmFileProcs.c \
			HGU_XmGraph.c \
			HGU_XmImageView.c \
			HGU_XmImageViewMappingControls.c \
			HGU_XmMenuUtils.c \
			HGU_XmMiscCallbacks.c \
			HGU_XmMiscProcs.c \
			HGU_XmPB_Radio.c \
			HGU_XmPB_Row.c \
			HGU_XmSaveRestore.c \
			HGU_XmSetInt.c \
			HGU_XmSlider.c \
			HGU_XmTextLine.c \
			HGU_XmUserPrompt.c \
			HGU_XmHelpMenu.c \
			HGU_XmMacroMenu.c \
			HGU_XmToolTips.c \
			HGU_XmVarArgs.c

# List of all header files that are available outside of either this archive
# or these executables (modify as required).
INCLUDES_PUB		=  \
			HGU_XmUtils.h \
			HGU_DrawingA.h \
			HGU_DrawingAP.h \
			HGU_XmStrings.h

# List of all header files that are required by this archive or these
# executables (modify as required).
INCLUDES_PRV		=  \
                  HGU_XmUtils.h   \
                  HGU_DrawingA.h \
                  HGU_DrawingAP.h \
                  HGU_XmStrings.h

# List of all troff sources for manual pages, these should have the appropriate
# extension (eg cat.1) (modify as required).
MANPAGES		=                  \
                   HGU_XmDialogUtils.3   \
                   HGU_XmFileProcs.3     \
                   HGU_XmMenuUtils.3     \
                   HGU_XmMiscCallbacks.3 \
                   HGU_XmMiscProcs.3     \
                   HGU_XmPB_Radio.3      \
                   HGU_XmPB_Row.3        \
                   HGU_XmSaveRestore.3   \
                   HGU_XmSlider.3        \
                   HGU_XmTextLine.3      \
                   HGU_XmUserPrompt.3


# List of all header file directories to be searched (modify as required).
INCDIRS			= .  \
			../libHguX \
			../../Core/libWlz \
			../../Core/libWlzExtFF \
			../../Core/libAlg \
			../../Core/libAlc \
			../../Core/libbibfile \
			$(X11_INC_DIR) \
			$(MOTIF_INC_DIR) \
			$(HGU_INC_DIR)

# List of all sub directories that contain makefiles (modify as required).
SUBSYSTEMS		= 

WWW_DIRS		= 

# List of files (should be top level directories) to be put used in creating
# a compressed tar file for distribution.
TARSOURCES		=

# List of all header files, ie the union of the private and public header
# files (should not need modifying).
INCLUDES_ALL		= $(sort $(INCLUDES_PUB) $(INCLUDES_PRV))

# List of all object files required for archive (should not need modifying).
OBJECTS			= $(CSOURCES:%.c=%.o)

# Basic flags for controlling compilation (modify as required).
#CDEBUG			= -g
#COPTIMISE		=

# un-comment these lines to make a 32-bit library for the IRIX 6
DEFINES			= -D$(UNIXTYPE) $(UNIXFLAGS) -DXTHREADS
CPPFLAGS		= $(INCDIRS:%=-I%) $(DEFINES)
CFLAGS			= $(CDEBUG) $(COPTIMISE) $(ANSI_CONFORMANCE)

# Default traget which builds everything (should not need modifying).
all:			archive executables includes manpages 
ifneq ($(strip $(SUBSYSTEMS)),)
			$(subsystems)
endif

# Target which installs everything after it has been built (should not need
# modifying).
install:		install_includes install_manpages install_archive \
			install_executables install_www install_doc

# Target which installs an archive (should not need modifying).
install_archive:	$(ARCHIVE)
ifneq ($(strip $(SUBSYSTEMS)),)
			$(subsystems)
endif
			$(install_archive)

# Target which installs executables (should not need modifying).
install_executables:	$(EXECUTABLES)
ifneq ($(strip $(SUBSYSTEMS)),)
			$(subsystems)
endif
			$(install_executables)

# Target which installs the public 'C' header files (should not need 
# modifying).
install_includes:	$(INCLUDES_PUB)
ifneq ($(strip $(SUBSYSTEMS)),)
			$(subsystems)
endif
			$(install_includes)

# Target which installs the www files (should not need 
# modifying).
install_www:    	$(WWW_DIRS)
ifneq ($(strip $(SUBSYSTEMS)),)
			$(subsystems)
endif
ifneq ($(strip $(WWW_DIRS)),)
			$(install_www)
endif

# Target which installs the doc files (should not need modifying)
# modifying).
install_doc:    $(DOC_FILES)
ifneq ($(strip $(SUBSYSTEMS)),)
			$(subsystems)
endif
			$(install_doc)

# Target which installs the manual pages in the appropriate directories,
# watch out for the tabs in the sed regexp's (should not need modifying).
install_manpages:	$(MANPAGES)
ifneq ($(strip $(SUBSYSTEMS)),)
			$(subsystems)
endif
			$(install_manpages)

# Target for compressed tar files suitable for distribution (should not need
# modifying).
tar:			$(TARSOURCES) clobber allsources
ifneq ($(strip $(TARSOURCES)),)
			$(TAR) $(TARFLAGS) $(TARFILE) $(TARSOURCES)
			$(COMPRESS) $(TARFILE)
endif

# Target for the archive (library), just a dependency (should not need
# modifying).
archive:		$(ARCHIVE)

# Target for executables, just a dependency (should not need modifying).
executables:		$(EXECUTABLES)

# Target for public header files, just a dependency (should not need modifying).
includes:		$(INCLUDES_PUB)

# Target for manual pages, just a dependency (should not need modifying).
manpages:		$(MANPAGES)

# Target for lint (modify as required).
lint:			$(CSOURCES) $(INCLUDES_PRV)
			$(LINT) $(LINTFLAGS) $(CPPFLAGS) $(CSOURCES)

# Target to clean up after making (should not need modifying).
clean:
			-$(RM) core $(OBJECTS) $(EXECUTABLES) $(ARCHIVE)

# Target to clean up everything that might not be wanted (should not need
# modifying).
clobber:		clean
ifneq ($(strip $(SUBSYSTEMS)),)
			$(subsystems)
endif
			-$(RM) *.o *.a .pure *.pure *.bak *~

# Target to create editable sources from SCCS (should not need modifying).
edit:
			$(SCCS) edit $(CSOURCES) $(INCLUDES_ALL) Makefile

# Target to create read only sources from SCCS (should not need modifying).
sources:
			-$(SCCS) get $(CSOURCES) $(INCLUDES_ALL) Makefile

# making the tag file
tags:
	$(ETAGS) -f $(ETAGSFILE) $(ETAGSFLAGS) $(CSOURCES) $(CCSOURCES) $(INCLUDES_ALL)

allsources:		sources
ifneq ($(strip $(SUBSYSTEMS)),)
			$(subsystems)
endif

# Targets to create archives, dependencies and rules (modify as required).
$(ARCHIVE):		$(OBJECTS)
			$(AR) r $(ARCHIVE) $?
			$(RANLIB) $(ARCHIVE)

$(OBJECTS):             %.o: %.c $(INCLUDES_ALL) 

# Targets to create executables, just a dependencies (modify as required).

# Target for Purified executables (modify as required).
purify:

%:                      %.o
			$(CC) -o $@ $< $(COPTIMISE) $(LDFLAGS) $(LDLIBS)
 
%.pure:                 %.o
			$(PURIFY) $(CC) -o $@ $< $(COPTIMISE) $(LDFLAGS) \
                                        $(LDLIBS)
