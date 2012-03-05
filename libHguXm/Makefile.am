##
# \file         Makefile.am
# \author       Bill Hill
# \date         November 2007
# \version      $Id$
# \par
# Copyright (C), [2012],
# The University Court of the University of Edinburgh,
# Old College, Edinburgh, UK.
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be
# useful but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public
# License along with this program; if not, write to the Free
# Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA  02110-1301, USA.
# \brief        Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS		= \
			  -I. \
             		  $(all_includes)

lib_LTLIBRARIES 	= libHguXm.la

libHguXm_la_LDFLAGS 	= -release @PACKAGE_VERSION@

libHguXm_la_SOURCES 	= \
			  HGU_DrawingA.c \
			  HGU_XmDialogUtils.c \
			  HGU_XmExtFF_FSBUtils.c \
			  HGU_XmFileList.c \
			  HGU_XmFileProcs.c \
			  HGU_XmGraph.c \
			  HGU_XmHelpMenu.c \
			  HGU_XmImageView.c \
			  HGU_XmImageViewMappingControls.c \
			  HGU_XmMacroMenu.c \
			  HGU_XmMenuUtils.c \
			  HGU_XmMiscCallbacks.c \
			  HGU_XmMiscProcs.c \
			  HGU_XmPB_Radio.c \
			  HGU_XmPB_Row.c \
			  HGU_XmSaveRestore.c \
			  HGU_XmSetInt.c \
			  HGU_XmSlider.c \
			  HGU_XmTextLine.c \
			  HGU_XmToolTips.c \
			  HGU_XmUserPrompt.c \
			  HGU_XmVarArgs.c

include_HEADERS 	= \
			  HGU_DrawingA.h \
			  HGU_DrawingAP.h \
			  HGU_XmStrings.h \
			  HGU_XmUtils.h
