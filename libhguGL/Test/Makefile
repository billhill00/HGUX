#########################################################################
# Project:      Mouse Atlas
# Title:        Makefile
# Date:         April 1999
# Author:       Bill Hill
# Copyright:    1999 Medical Research Council, UK.
#               All rights reserved.
#               MRC Human Genetics Unit,
#               Western General Hospital,
#               Edinburgh, EH4 2XU, UK.
# Purpose:      Makefile for the MRC HGU OpenGL library test binaries.
# $Revision$
# $Id$
# Maintenance:  Log changes below, with most recent at top of list.
#########################################################################

# Include the configuration makefile (modify as required).
include			../../../Makefile.conf

# Name of archive (library) to be built (modify as required).
LIBNAME			=

# archive (library) to be built (Should not need modifying).
ARCHIVE			= $(LIBNAME:%=%.a)

# Names of executables to be built (modify as required).
EXECUTABLES		= \
			  HGUGL-TestAxes \
			  HGUGL-TestCube

# List of all 'C' source files (modify as required).
CSOURCES		= \
			  HGUGL-TestAxes.c \
			  HGUGL-TestCube.c \

# List of all header files that are available outside of either this archive
# or these executables (modify as required).
INCLUDES_PUB		= 

# List of all header files that are required by this archive or these
# executables (modify as required).
INCLUDES_PRV		= trackball.h

# List of all troff sources for manual pages, these should have the appropriate
# extension (eg cat.1) (modify as required).
MANPAGES		=

# List of all directories to be searched before the standard include
# and lib directories (modify as required).
DEPENDDIRS		= \
			  . \
			  .. \
			  ../../Core/libAlc \
			  ../../Core/libAlg \
			  ../../Core/libWlz

# List of all header file directories to be searched (modify as required).
INCDIRS			= \
			  $(DEPENDDIRS) \
			  $(HGU_INC_DIR) \
			  $(OPENGL_INC_DIR) \
			  $(MOTIF_INC_DIR) \
			  $(X11_INC_DIR)

# List of library search paths (modify as required).
LIBDIRS			= \
			  $(DEPENDDIRS) \
			  $(HGU_LIB_DIR) \
			  $(OPENGL_LIB_DIR) \
			  $(MOTIF_LIB_DIR) \
			  $(X11_LIB_DIR)

# List of all sub directories that contain makefiles. The subsystems will be
# built in the order in which they are given here (modify as required).
SUBSYSTEMS		= 

# Name of distribution tar file (modify as required).

# List of files (should be top level directories) to be put used in creating
# a compressed tar file for distribution (modify as required).
TARSOURCES		= 

# List of all header files, ie the union of the private and public header
# files (should not need modifying).
INCLUDES_ALL		= $(sort $(INCLUDES_PUB) $(INCLUDES_PRV))

# List of all object files required for archive (should not need modifying).
# C++ edit the next line
OBJECTS			= $(CSOURCES:%.c=%.o)

# List of libraries to link (modify as required).
ifeq 		($(UNIXTYPE), SUNOS5)
EXTRA_LIBS		= gen socket nsl m
endif
ifeq		($(UNIXTYPE), IRIX646)
EXTRA_LIBS		= m
SYSLIB			= /usr/lib64
endif

OPENGLLIBS		= GLU GL
MOTIFLIBS       	= Xm 
X11LIBS         	= Xt Xmu X11 Xext

LIBRARIES		= hguGL \
			  hguXm \
			  hguX \
			  wlz \
			  nalgs \
			  typealloc \
			  $(OPENGLLIBS) $(MOTIFLIBS) $(X11LIBS) \
			  $(EXTRA_LIBS)

# Basic flags for controlling compilation (modify as required).
#CDEBUG 	= -g -DDEBUG 
#COPTIMISE	= -xchip=ultra -KPIC
#CXXDEBUG	= -g -DDEBUG
#CXXOPTIMISE	= -xchip=ultra -KPIC
DEFINES 	= -D$(UNIXTYPE) $(UNIXFLAGS) -DHGUHOME=\"$(HGUHOME)\"
CPPFLAGS	= $(INCDIRS:%=-I%) $(DEFINES)
CFLAGS		= $(CDEBUG) $(COPTIMISE) $(ANSI_CONFORMANCE)
CXXFLAGS	=$(CXXDEBUG) $(CXXOPTIMISE) $(CXXANSI_CONFORMANCE)

# Default traget which builds everything (should not need modifying).
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
tar:			$(TARSOURCES) clobber
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

# Targets to create archives, dependencies and rules (modify as required).
# C++ uncommment the (INSTANCIATE_TEMPLATES) macro
ifneq ($(strip $(ARCHIVE)),)
$(ARCHIVE):		$(OBJECTS)
			$(AR) r $(ARCHIVE) $?
			$(RANLIB) $(ARCHIVE)
endif


# Targets to create executables, just a dependencies (modify as required).
HGUGL-TestAxes:		HGUGL-TestAxes.o
HGUGL-TestCube:		HGUGL-TestCube.o

# Target for Purified executables (modify as required).
purify:
