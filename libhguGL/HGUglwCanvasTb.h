#ifndef HGUGL_GLWCANVASTB_H
#define HGUGL_GLWCANVASTB_H
#pragma ident "MRC HGU $Id"
/************************************************************************
* Project:	Mouse Atlas
* Title:	HGUglwCanvasTb.h
* Date: 	April 1999
* Author:	Bill Hill
* Copyright:	1999 Medical Research Council, UK.
*		All rights reserved.
* Address:	MRC Human Genetics Unit,
*		Western General Hospital,
*		Edinburgh, EH4 2XU, UK.
* Purpose:	Public include file for the HGUglCanvasTb MRC HGU OpenGL
*		widget. The HGUglCanvasTb widget provides both a canvas
*		drawing area and a 3D virtual trackball, it is descended
*		from the HGUglCanvas OpenGL widget. See the manual page
*		HGUglCanvasTb(3) for a description of the widget.
* $Revision$
* Maintenance:	Log changes below, with most recent at top of list.
************************************************************************/
#include <GL/gl.h>
#include <GL/glx.h>
#include <HGUglwCallbacks.h>
 
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Resources:
 *
 * Name			Class			RepType		Default Value
 * ----	     	   	-----			-------		-------------
 * animateInterval	AnimateInterval		int		100
 * trackballSize	TrackballSize		int		80
 * trackballMode	TrackballMode		int		*
 * motionInterval	MotionInterval		int		200
 *
 */

typedef enum
{
  HGUglwCANVASTB_MODE_STILL     = 0,
  HGUglwCANVASTB_MODE_ANIMATION,
  HGUglwCANVASTB_MODE_XY,
  HGUglwCANVASTB_MODE_Z,
  HGUglwCANVASTB_MODE_ROTATE
} HGUglwTrackballMode;

#define HGUglwNanimateInterval	"animateInterval"
#define HGUglwCAnimateInterval	"AnimateInterval"
#define HGUglwNtrackballSize	"trackballSize"
#define HGUglwCTrackballSize	"TrackballSize"
#define HGUglwNtrackballMode	"trackballMode"
#define HGUglwCTrackballMode	"TrackballMode"
#define HGUglwNmotionInterval	"motionInterval"
#define HGUglwCMotionInterval	"MotionInterval"

externalref WidgetClass	hguGLwCanvasTbWidgetClass;

typedef struct _HGUglwCanvasTbClassRec	*HGUglwCanvasTbWidgetClass;
typedef struct _HGUglwCanvasTbRec	*HGUglwCanvasTbWidget;

extern Widget		HGUglwCreateCanvasTb(Widget, String,
					     ArgList, Cardinal);

extern void		HGUglwCanvasTbReset(Widget),
			HGUglwCanvasTbStop(Widget),
			HGUglwCanvasTbPause(Widget),
			HGUglwCanvasTbAnimate(Widget),
			HGUglwCanvasTbUpdate(Widget),
			HGUglwCanvasTbGetRotateMatrixGL(Widget, double[4][4]),
			HGUglwCanvasTbGetTranslate(Widget, WlzDVertex3 *),
			HGUglwCanvasTbSetAnimateInterval(Widget, int),
			HGUglwCanvasTbSetTrackballSize(Widget, int),
			HGUglwCanvasTbSetMotionInterval(Widget, int);
extern int		HGUglwCanvasTbGetAnimateInterval(Widget),
			HGUglwCanvasTbGetTrackballSize(Widget),
			HGUglwCanvasTbGetMotionInterval(Widget);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HGUGL_GLWCANVASTB_H */
