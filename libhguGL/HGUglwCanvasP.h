#ifndef HGUGL_GLWCANVASP_H
#define HGUGL_GLWCANVASP_H
#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _libhguGL/HGUglwCanvasP_h[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         HGUglwCanvasP.h
* \author       Bill Hill
* \date         Wed Apr 29 11:08:55 2009
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
* \ingroup      HGU_GL
* \brief        Private include file for the HGUglCanvas MRC HGU OpenGL
*		widget. The HGUglCanvas widget is descended from the
*		Motif primitive widget. See the manual page
*		HGUglCanvas(3) for a description of the widget.
*/

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
