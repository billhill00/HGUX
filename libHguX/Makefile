#########################################################################
# Project:      Mouse Atlas						#
# Title:        Makefile		                               	#
# Date:         September 1994	                                    	#
# Author:       Richard Baldock				    		#
# $Revision$
# Purpose:      Makefile for building a library				#
# Maintenance:	Log changes below, with most recent at top of list.	#
# 01-05-96 bill	Use HGU_INC_DIR.					#
# 16-09-94 richard	Copied and modified version of libbibfiles	#
#########################################################################

# Include the configuration makefile (modify as required).
include			../../Makefile.conf

# Name of archive (library) to be built (modify as required).
ARCHIVE			= libHguX.a

# Names of executables to be built (modify as required).
EXECUTABLES		=

# List of all 'C' source files (modify as required).
CSOURCES		= \
			HGU_XError.c \
                        HGU_XGC.c \
			HGU_XGetDomain.c \
			HGU_XGetDomainUtils.c \
			HGU_XGetPolydmn.c \
			HGU_XGetPolyDomain.c \
			HGU_XGetRect.c \
			HGU_XGetThresholdDomain.c \
			HGU_XGetVtx.c \
			HGU_XGetVtxs.c \
			HGU_XInteractUtils.c \
                        HGU_XLut.c \
			HGU_XVisualMatch.c \
                        HGU_XMiscProcs.c

# List of all header files that are available outside of either this archive
# or these executables (modify as required).
INCLUDES_PUB		= HGU_XCursors.h HGU_XInteract.h HGU_XUtils.h

# List of all header files that are required by this archive or these
# executables (modify as required).
INCLUDES_PRV		= 

# List of all troff sources for manual pages, these should have the appropriate
# extension (eg cat.1) (modify as required).
MANPAGES		=  \
                        HGU_XGC.3 \
                        HGU_XLut.3

# List of all header file directories to be searched (modify as required).
INCDIRS			= .  \
			../../Core/libWlz \
			../../Core/libAlg \
			../../Core/libAlc \
			$(HGU_INC_DIR) \
			$(WOOLZHOME)/include \
			$(X11HOME)/include

# List of all sub directories that contain makefiles (modify as required).
SUBSYSTEMS		= 

# List of files (should be top level directories) to be put used in creating
# a compressed tar file for distribution.
TARSOURCES		=

# List of all header files, ie the union of the private and public header
# files (should not need modifying).
INCLUDES_ALL		= $(sort $(INCLUDES_PUB) $(INCLUDES_PRV))

# List of all object files required for archive (should not need modifying).
OBJECTS			= $(CSOURCES:%.c=%.o)

# Basic flags for controlling compilation (modify as required).
DEFINES			=
CPPFLAGS		= $(INCDIRS:%=-I%) $(DEFINES) $(UNIXFLAGS)
CFLAGS			= $(CDEBUG) $(COPTIMISE) $(ANSI_CONFORMANCE)

ifeq 		($(UNIXTYPE), SUNOS5)
CFLAGS_SPECIAL		= $(CDEBUG) -xcg92 -xO1 $(ANSI_CONFORMANCE)
else
CFLAGS_SPECIAL		= $(CFLAGS)
endif

# Default target which builds everything (should not need modifying).
all:			archive executables includes manpages 
ifneq ($(strip $(SUBSYSTEMS)),)
			$(subsystems)
endif

# Target which installs everything after it has been built (should not need
# modifying).
install:		install_includes install_manpages install_archive \
			install_executables

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

$(OBJECTS):             %.o: %.c $(INCLUDES_ALL) 
ifeq ($(UNIXTYPE), SUNOS5)
			$(CC) -v  -c -o $@ $(CPPFLAGS) $(CFLAGS) $<
else
			$(CC) -c -o $@ $(CPPFLAGS) $(CFLAGS) $<
endif

# Targets to create executables, just a dependencies (modify as required).

# Target for Purified executables (modify as required).
purify:

%:                      %.o
			$(CC) -o $@ $< $(COPTIMISE) $(LDFLAGS) $(LDLIBS)
 
%.pure:                 %.o
			$(PURIFY) $(CC) -o $@ $< $(COPTIMISE) $(LDFLAGS) \
                                        $(LDLIBS)
