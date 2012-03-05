#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _HGU_XVisualMatch_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         HGU_XVisualMatch.c
* \author       Bill Hill
* \date		April 2009
* \version      $Id$
* \par
* Address:
*               MRC Human Genetics Unit,
*               MRC Institute of Genetics and Molecular Medicine,
*               University of Edinburgh,
*               Western General Hospital,
*               Edinburgh, EH4 2XU, UK.
* \par
* Copyright (C), [2012],
* The University Court of the University of Edinburgh,
* Old College, Edinburgh, UK.
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
*/
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

 
/*!
* \return	Matched visual (maybe 
* \ingroup	HGU_X
* \brief	Given a visual class (eg: PseudoColor, TrueColor, ...)
*		and a visual depth (eg: 8, 24, ...) an attempt is made
*		to find a matching visual for the given display and
*		and screen.
* \param	visualClassRtn		Return pointer for the matched visual
* 					class.
* \param	visualDepthRtn		Return pointer for the matched visual
* 					depth.
* \param	visualClass		Requested visual class.
* \param	visualDepth		Requested visualDepth depth.
* \param	display			X11 display pointer.
* \param	screenNum		X11 screen number (NOT screen pointer).
*/
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
