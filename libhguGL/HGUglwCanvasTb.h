#ifndef HGUGL_GLWCANVASTB_H
#define HGUGL_GLWCANVASTB_H

#if defined(__GNUC__)
#ident "MRC HGU $Id:"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id:"
#else static char _HGUgl_canvasTb.h[] = "MRC HGU $Id:";
#endif
#endif
/*!
* \file         HGUglwCanvasTb.h
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Wed Apr 29 11:07:39 2009
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
* \brief        Public include file for the HGUglCanvasTb MRC HGU OpenGL
*		widget. The HGUglCanvasTb widget provides both a canvas
*		drawing area and a 3D virtual trackball, it is descended
*		from the HGUglCanvas OpenGL widget. See the manual page
*		HGUglCanvasTb(3) for a description of the widget.
*               
*
* Maintenance log with most recent changes at top of list.
*/

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
