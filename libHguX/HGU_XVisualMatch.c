#if defined(__GNUC__)
#ident "MRC HGU $Id:"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id:"
#else static char _HGU_XVisualMa_ch_c[] = "MRC HGU $Id:";
#endif
#endif
/*!
* \file         HGU_XVisualMatch.c
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Wed Apr 29 08:15:27 2009
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
* \ingroup      HGU_X
* \brief        Provides a function for finding visuals.
*               
* \todo         -
* \bug          None known
*
* Maintenance log with most recent changes at top of list.
*/
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

 
/************************************************************************
* Function:	HGU_XVisualMatch					*
* Returns:	Visual *:			Matched visual (maybe	*
*						NULL).			*
* Purpose:	Given a visual class (eg: PseudoColor, TrueColor, ...)	*
*		and a visual depth (eg: 8, 24, ...) an attempt is made	*
*		to find a matching visual for the given display and	*
*		and screen.						*
* Global refs:	-							*
* Parameters:	int *visualClassRtn:	Return pointer for the matched	*
*					visual class.			*
*		int *visualDepthRtn:	Return pointer for the matched	*
*					visual depth.			*
*		int visualClass:	Requested visual class.		*
*		int visualDepth:	Requested visualDepth depth.	*
*		Display *display:	X11 display pointer.		*
*		int screenNum:		X11 screen number (NOT screen	*
*					pointer).			*
************************************************************************/
Visual		*HGU_XVisualMatch(int *visualClassRtn, int *visualDepthRtn,
				  int visualClass, int visualDepth,
				  Display *display, int screenNum)
{
  Visual	*visual;
  XVisualInfo	vInfoTemplate;
  XVisualInfo	*vInfo = NULL;
  int		numVisuals = 0;

  vInfoTemplate.class = visualClass;
  vInfoTemplate.depth = visualDepth;
  vInfo = XGetVisualInfo(display, (long )((VisualDepthMask)|(VisualClassMask)),
  			 &vInfoTemplate, &numVisuals);
  if(vInfo)
  {
    visual = vInfo->visual;
    XFree(vInfo);
  }
  else
  {
    visual = DefaultVisual(display, screenNum);
    if(visualClassRtn || visualDepthRtn)
    {
      vInfoTemplate.visualid = XVisualIDFromVisual(visual);
      vInfo = XGetVisualInfo(display, (long )(VisualIDMask),
      		  	     &vInfoTemplate, &numVisuals);
    }
    if(vInfo)
    {
      visualClass = vInfo->class;
      visualDepth = vInfo->depth;
      XFree(vInfo);
    }
    else
    {
      visualClass = 0;
      visualDepth = 0;
    }
  }
  if(visualClassRtn)
  {
    *visualClassRtn = visualClass;
  }
  if(visualDepthRtn)
  {
    *visualDepthRtn = visualDepth;
  }
  return(visual);
}
