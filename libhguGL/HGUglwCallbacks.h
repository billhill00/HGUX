#ifndef HGUGL_GLWCALLBACKS_H
#define HGUGL_GLWCALLBACKS_H
#pragma ident "MRC HGU $Id"
/************************************************************************
* Project:	Mouse Atlas
* Title:	HGUglwCallbacks.h
* Date: 	April 1999
* Author:	Bill Hill
* Copyright:	1999 Medical Research Council, UK.
*		All rights reserved.
* Address:	MRC Human Genetics Unit,
*		Western General Hospital,
*		Edinburgh, EH4 2XU, UK.
* Purpose:	Definitions for the MRC HGU OpenGL library HGUglwCanvas
*		and HGUglwCanvasTb widgets.
* $Revision$
* Maintenance:	Log changes below, with most recent at top of list.
************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define HGUglwCCallback           	"Callback"
#define HGUglwNexposeCallback     	"exposeCallback"
#define HGUglwNinitCallback      	"initCallback"
#define HGUglwNresizeCallback     	"resizeCallback"
#define HGUglwNinputCallback      	"inputCallback"
#define HGUglwNtrackballCallback      	"trackballCallback"

#define HGUglw_CR_BASE		((('H'<<8)|'G'<<8)|'U'<<8)
#define HGUglw_CR_EXPOSE    	XmCR_EXPOSE
#define HGUglw_CR_RESIZE    	XmCR_RESIZE
#define HGUglw_CR_INPUT     	XmCR_INPUT
#define HGUglw_CR_INIT  	((HGUglw_CR_BASE)+1)
#define HGUglw_CR_TRACKBALL  	((HGUglw_CR_BASE)+2)

typedef struct
{
  int		reason;
  XEvent	*event;
  Dimension	width;
  Dimension	height;
  GLXContext	glxCtx;
} HGUglwCanvasCallbackStruct;

#ifdef __cplusplus 
}
#endif /* __cplusplus */

#endif /* HGUGL_GLWCALLBACKS_H */
