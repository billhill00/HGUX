#ifndef HGUGL_GLWCANVASP_H
#define HGUGL_GLWCANVASP_H
#pragma ident "MRC HGU $Id"
/************************************************************************
* Project:	Mouse Atlas
* Title:	HGUglwCanvasP.h
* Date: 	April 1999
* Author:	Bill Hill
* Copyright:	1999 Medical Research Council, UK.
*		All rights reserved.
* Address:	MRC Human Genetics Unit,
*		Western General Hospital,
*		Edinburgh, EH4 2XU, UK.
* Purpose:	Private include file for the HGUglCanvas MRC HGU OpenGL
*		widget. The HGUglCanvas widget is descended from the
*		Motif primitive widget. See the manual page
*		HGUglCanvas(3) for a description of the widget.
* $Revision$
* Maintenance:	Log changes below, with most recent at top of list.
************************************************************************/
#include <HGUglwCanvas.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct _HGUglwCanvasClassPart
{
  caddr_t		extension;
} HGUglwCanvasClassPart;

typedef struct _HGUglwCanvasClassRec
{
  CoreClassPart		core_class;
  XmPrimitiveClassPart	primitive_class;
  HGUglwCanvasClassPart	hguGLwCanvas_class;
} HGUglwCanvasClassRec;

extern HGUglwCanvasClassRec hguGLwCanvasClassRec;

typedef struct
{
  int			*attribList;				/* resources */
  XVisualInfo		*visualInfo;
  Boolean		myList;	       /* TRUE if we malloced the attribList */
  Boolean		myVisual;	/* TRUE if we created the visualInfo */
  Boolean		installColormap;
  Boolean		allocateBackground;
  Boolean		allocateOtherColors;
  Boolean		installBackground;
  XtCallbackList	initCallback;
  XtCallbackList	resizeCallback;
  XtCallbackList	exposeCallback;
  XtCallbackList	inputCallback;
  int			bufferSize;		      /* specific attributes */
  int			level;
  Boolean		rgba;
  Boolean		doublebuffer;
  Boolean		stereo;
  int			auxBuffers;
  int			redSize;
  int			greenSize;
  int			blueSize;
  int			alphaSize;
  int			depthSize;
  int			stencilSize;
  int			accumRedSize;
  int			accumGreenSize;
  int			accumBlueSize;
  int			accumAlphaSize;
  GLXContext		glxCtx;
  Boolean		directRender;
} HGUglwCanvasPart;

typedef struct _HGUglwCanvasRec
{
  CorePart		core;
  XmPrimitivePart	primitive;
  HGUglwCanvasPart	hguGLwCanvas;
} HGUglwCanvasRec;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HGUGL_GLWCANVASP_H */
