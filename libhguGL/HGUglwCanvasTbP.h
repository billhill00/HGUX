#ifndef HGUGL_GLWCANVASTBP_H
#define HGUGL_GLWCANVASTBP_H
#pragma ident "MRC HGU $Id"
/************************************************************************
* Project:	Mouse Atlas
* Title:	HGUglwCanvasTbP.h
* Date: 	April 1999
* Author:	Bill Hill
* Copyright:	1999 Medical Research Council, UK.
*		All rights reserved.
* Address:	MRC Human Genetics Unit,
*		Western General Hospital,
*		Edinburgh, EH4 2XU, UK.
* Purpose:	Private include file for the HGUglCanvasTb MRC HGU OpenGL
*		widget. The HGUglCanvasTb widget provides both a canvas
*		drawing area and a 3D virtual trackball, it is descended
*		from the HGUglCanvas OpenGL widget. See the manual page
*		HGUglCanvasTb(3) for a description of the widget.
* $Revision$
* Maintenance:	Log changes below, with most recent at top of list.
************************************************************************/
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
