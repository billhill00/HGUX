#ifndef _HGU_DrawingA_h
#define _HGU_DrawingA_h

#if defined(__GNUC__)
#ident "MRC HGU $Id:"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id:"
#else static char _HGU_DrawingA.h[] = "MRC HGU $Id:";
#endif
#endif
/*!
* \file         HGU_DrawingA.h
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Wed Apr 29 09:11:39 2009
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
* \brief        Public header file for HGU_DrawingArea widget
*               
* \todo         -
* \bug          None known
*
* Maintenance log with most recent changes at top of list.
*/

/* Additional resources:

 Name	     	   Class		RepType		Default Value
 ----	     	   -----		-------		-------------
 XtNvisual         XtCVisual            XtRVisual       CopyFromParent
*/

typedef struct _HGU_DrawingAreaClassRec	*HGU_DrawingAreaWidgetClass;
typedef struct _HGU_DrawingAreaRec	*HGU_DrawingAreaWidget;

extern WidgetClass hgu_DrawingAreaWidgetClass;

#endif /* _HGU_DrawingA_h */
/* DON'T ADD STUFF AFTER THIS #endif */
