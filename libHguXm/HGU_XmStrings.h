#ifndef HGU_XMSTRINGS_H
#define HGU_XMSTRINGS_H

#if defined(__GNUC__)
#ident "MRC HGU $Id:"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id:"
#else static char _HGU_XmStrings.h[] = "MRC HGU $Id:";
#endif
#endif
/*!
* \file         HGU_XmStrings.h
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Wed Apr 29 09:09:02 2009
* \version      MRC HGU $Id$
*               $Revision$
*               $Name$
* \par Address:
*               MRC Human Genetics Unit,
*               Western General Hospital,
*               Edinburgh, EH4 2XU, UK.
* \par Copyright:
* Copyright (C) 2005 Medical research Council, UK.
* 
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be
* useful but WITHOUT ANY WARRANTY; without even the implied
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
* PURPOSE.  See the GNU General Public License for more
* details.
*
* You should have received a copy of the GNU General Public
* License along with this program; if not, write to the Free
* Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA  02110-1301, USA.
* \ingroup      HGU_Xm
* \brief        String definitions for use with Xt/Motif user interface
*               procedures.
* \todo         -
* \bug          None known
*
* Maintenance log with most recent changes at top of list.
*/


/* string definitions for variable argument convenience routines */

#define HGU_VaLABEL 	"HGU_VaLABEL"
#define HGU_VaPB	"HGU_VaPB"

/* strings for the Graph pseudo widget */
#define HGU_Nx_str	"x_str"
#define HGU_Ny_str	"y_str"
#define HGU_Nx_min	"x_min"
#define HGU_Nx_max	"x_max"
#define HGU_Ny_min	"y_min"
#define HGU_Ny_max	"y_max"
#define HGU_Nx_format	"x_format"
#define HGU_Ny_format	"y_format"
#define HGU_CFormat	"format"
#define HGU_Nx_position	"x_position"
#define HGU_Ny_position	"y_position"
#define HGU_Nmarkers	"markers"
#define HGU_Nstyle	"style"
#define HGU_Ncol1	"col1"
#define HGU_Ncol2	"col2"
#define HGU_Ncol3	"col3"
#define HGU_Ncol4	"col4"
#define HGU_Ncol5	"col5"
#define HGU_Ncol6	"col6"

/* do not add anything after this line */
#endif /* HGU_XMSTRINGS_H */
