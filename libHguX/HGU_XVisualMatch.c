#pragma ident "MRC HGU $Id$"
#define _HGU_XVisualMatch_c
/************************************************************************
* Project:	libhguX - MRC HGU X11 Utilities				*
* Title:	HGU_XVisualMatch.c					*
* Date:		February 1995						*
* Author:	Bill Hill (bill@hgu.mrc.ac.uk)				*
* $Revision$
* Purpose:	Provides a function for finding visuals.		*
* Maintenance:	Log changes below, with most recent at top of list.	*
************************************************************************/
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
