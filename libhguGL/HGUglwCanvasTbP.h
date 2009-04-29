#ifndef HGUGL_GLWCANVASTBP_H
#define HGUGL_GLWCANVASTBP_H
#if defined(__GNUC__)
#ident "MRC HGU $Id:"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id:"
#else static char _HGUgl_canvasTbP.h[] = "MRC HGU $Id:";
#endif
#endif
/*!
* \file         HGUglwCanvasTbP.h
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Wed Apr 29 11:08:35 2009
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
* \ingroup      HGU_GL
* \brief        Private include file for the HGUglCanvasTb MRC HGU OpenGL
*		widget. The HGUglCanvasTb widget provides both a canvas
*		drawing area and a 3D virtual trackball, it is descended
*		from the HGUglCanvas OpenGL widget. See the manual page
*		HGUglCanvasTb(3) for a description of the widget.
*               
*
* Maintenance log with most recent changes at top of list.
*/

#include <HGUglwCanvasTb.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct _HGUglwCanvasTbClassPart
{
  caddr_t			extension;
} HGUglwCanvasTbClassPart;

typedef struct
{
  CoreClassPart			core_class;
  XmPrimitiveClassPart		primitive_class;
  HGUglwCanvasClassPart		hguGLwCanvas_class;
  HGUglwCanvasTbClassPart 	hguGLwCanvasTb_class;
} HGUglwCanvasTbClassRec;

extern HGUglwCanvasTbClassRec	hguGLwCanvasTbClassRec;

typedef struct
{
  int				animateInterval; 	/* ms between frames */
  XtIntervalId			animateFnId;
  XtCallbackList		trackballCallback;
  HGUglwTrackballMode		trackballMode;
  int				trackballSize;     /* Percent of window size */
  int				motionInterval; /* Motion intervl limit (ms) */
  Time				motionTime;     /* Time of prev motion event */
  WlzIVertex2			motionPos;  /* Position of prev motion event */
  HGUglQuaternion		rotateDelta;
  HGUglQuaternion		rotateCurrent;
  WlzDVertex3			translateDelta;
  WlzDVertex3			translateCurrent;
  XEvent			tbEvent;		   /* Last X11 event */
} HGUglwCanvasTbPart;

typedef struct _HGUglwCanvasTbRec
{
  CorePart			core;
  XmPrimitivePart		primitive;
  HGUglwCanvasPart		hguGLwCanvas;
  HGUglwCanvasTbPart		hguGLwCanvasTb;
} HGUglwCanvasTbRec;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HGUGL_GLWCANVASTBP_H */
