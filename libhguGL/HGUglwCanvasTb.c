#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _libhguGL/HGUglwCanvasTb_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         HGUglwCanvasTb.c
* \author       Bill Hill
* \date         Wed Apr 29 11:11:31 2009
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
* \brief        Methods for the HGUglCanvasTb MRC HGU OpenGL
*		widget. The HGUglCanvasTb widget provides both a canvas
*		drawing area and a 3D virtual trackball, it is descended
*		from the HGUglCanvas OpenGL widget. See the manual page
*		HGUglCanvasTb(3) for a description of the widget.
*/

#include <stdio.h>
#include <math.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <Xm/PrimitiveP.h>
#include <GL/glx.h>
#include <GL/gl.h>
#include <Wlz.h>
#include <HGUglQuaternion.h>
#include <HGUglwCallbacks.h>
#include <HGUglwCanvasP.h>
#include <HGUglwCanvasTbP.h>

/* Widget methods: */
static void			Initialize(
				  Widget,
				  Widget,
				  ArgList,
				  Cardinal *);
static void			Destroy(
				  Widget);
static Boolean			SetValues(
				  Widget,
				  Widget,
				  Widget,
				  ArgList,
				  Cardinal *);
/* Action procedures: */
void				HGUglwCanvasTbMotionBeginRotate(
				  Widget,
				  XEvent *,
				  String *,
				  Cardinal *);
void				HGUglwCanvasTbMotionBeginXY(
				  Widget,
				  XEvent *,
				  String *,
				  Cardinal *);
void				HGUglwCanvasTbMotionBeginZ(
				  Widget,
				  XEvent *,
				  String *,
				  Cardinal *);
void				HGUglwCanvasTbMotionRotate(
				  Widget,
				  XEvent *,
				  String *,
				  Cardinal *);
void				HGUglwCanvasTbMotionXY(
				  Widget,
				  XEvent *,
				  String *,
				  Cardinal *);
void				HGUglwCanvasTbMotionZ(
				  Widget,
				  XEvent *,
				  String *,
				  Cardinal *);
void				HGUglwCanvasTbMotionEnd(
				  Widget,
				  XEvent *,
				  String *,
				  Cardinal *);
/* Timeout procedures: */
static void			HGUglwCanvasTbAnimateFn(
				  XtPointer,
				  XtIntervalId *);
/* And the rest: */
static void			HGUglwCanvasTbMotionBeginFn(
				  HGUglwCanvasTbWidget,
				  XEvent *,
				  HGUglwTrackballMode);
static void			HGUglwCanvasTbCalculateRotate(
				  HGUglwCanvasTbWidget,
				  WlzIVertex2);
static void			HGUglwCanvasTbCalculateXY(
				  HGUglwCanvasTbWidget,
				  WlzIVertex2);
static void			HGUglwCanvasTbCalculateZ(
				  HGUglwCanvasTbWidget,
				  WlzIVertex2);
static double			HGUglwCanvasTbProjectToZ(
				  double, WlzDVertex2);

static char		defaultTranslations[] =
  "Shift<Btn1Down>:			hguGLw-motion-begin-z()\n"
  "<Btn1Down>:				hguGLw-motion-begin-rotate()\n"
  "<Btn2Down>:				hguGLw-motion-begin-xy()\n"
  "Shift<Btn1Motion>:			hguGLw-motion-translate-z()\n"
  "<Btn1Motion>:			hguGLw-motion-rotate()\n"
  "<Btn2Motion>:			hguGLw-motion-translate-xy()\n"
  "<Btn1Up>:				hguGLw-motion-end()\n"
  "<Btn2Up>:				hguGLw-motion-end()\n"
  "<KeyDown>:    			hguGLw-input()\n"
  "<KeyUp>:      			hguGLw-input()\n"
  "<BtnDown>:    			hguGLw-input()\n"
  "<BtnUp>:      			hguGLw-input()\n"
  "<BtnMotion>:  			hguGLw-input()";

static XtActionsRec	actionsList[] =
{
  {"hguGLw-motion-begin-rotate", 	HGUglwCanvasTbMotionBeginRotate},
  {"hguGLw-motion-begin-xy", 		HGUglwCanvasTbMotionBeginXY},
  {"hguGLw-motion-begin-z", 		HGUglwCanvasTbMotionBeginZ},
  {"hguGLw-motion-rotate", 		HGUglwCanvasTbMotionRotate},
  {"hguGLw-motion-translate-xy", 	HGUglwCanvasTbMotionXY},
  {"hguGLw-motion-translate-z", 	HGUglwCanvasTbMotionZ},
  {"hguGLw-motion-end", 		HGUglwCanvasTbMotionEnd},
  {"hguGLw-input", 			HGUglwCanvasInput},
};

#define HGUglw_OFFSET(field) XtOffset(HGUglwCanvasTbWidget, field)

static XtResource resources[] =
{
  {
    HGUglwNanimateInterval, HGUglwCAnimateInterval,
    XtRInt, sizeof(int),
    HGUglw_OFFSET(hguGLwCanvasTb.animateInterval),
    XtRImmediate, (XtPointer )100
  },
  {
    HGUglwNtrackballCallback, HGUglwCCallback,
    XtRCallback, sizeof(XtCallbackList),
    HGUglw_OFFSET(hguGLwCanvasTb.trackballCallback),
    XtRImmediate, (XtPointer )NULL
  },
  {
    HGUglwNtrackballSize, HGUglwCTrackballSize,
    XtRInt, sizeof(int),
    HGUglw_OFFSET(hguGLwCanvasTb.trackballSize),
    XtRImmediate, (XtPointer )80
  },
  {
    HGUglwNtrackballMode, HGUglwCTrackballMode,
    XtRInt, sizeof(int),
    HGUglw_OFFSET(hguGLwCanvasTb.trackballMode),
    XtRImmediate, (XtPointer )HGUglwCANVASTB_MODE_STILL
  },
  {
    HGUglwNmotionInterval, HGUglwCMotionInterval,
    XtRInt, sizeof(int),
    HGUglw_OFFSET(hguGLwCanvasTb.motionInterval),
    XtRImmediate, (XtPointer )200
  },
};

#undef HGUglw_OFFSET

HGUglwCanvasTbClassRec hguGLwCanvasTbClassRec =
{
  {
    /* Core */
    /* superclass         	*/ (WidgetClass) &hguGLwCanvasClassRec,
    /* class_name         	*/ "HGUglwCanvasTb",
    /* widget_size        	*/ sizeof(HGUglwCanvasTbRec),
    /* class_initialize   	*/ NULL,
    /* class_part_init    	*/ NULL,
    /* class_inited       	*/ FALSE,
    /* initialize         	*/ Initialize,
    /* initialize_hook    	*/ NULL,
    /* realize            	*/ XtInheritRealize,
    /* actions            	*/ actionsList,
    /* num_actions        	*/ XtNumber(actionsList),
    /* resources          	*/ resources,
    /* num_resources      	*/ XtNumber(resources),
    /* xrm_class          	*/ NULLQUARK,
    /* compress_motion    	*/ TRUE,
    /* compress_exposure  	*/ TRUE,
    /* compress_enterleave	*/ TRUE,
    /* visible_interest   	*/ TRUE,
    /* destroy            	*/ Destroy,
    /* resize             	*/ XtInheritResize,
    /* expose             	*/ XtInheritExpose,
    /* set_values         	*/ SetValues,
    /* set_values_hook    	*/ NULL,
    /* set_values_almost  	*/ XtInheritSetValuesAlmost,
    /* get_values_hook    	*/ NULL,
    /* accept_focus       	*/ NULL,
    /* version            	*/ XtVersion,
    /* callback_private   	*/ NULL,
    /* tm_table           	*/ defaultTranslations,
    /* query_geometry     	*/ NULL,
    /* display_accelerator	*/ NULL,
    /* extension          	*/ NULL
  },
  {
    /* XmPrimitive */
    /* border_highlight         */ XmInheritBorderHighlight,
    /* border_unhighlight       */ XmInheritBorderUnhighlight,
    /* translations             */ XtInheritTranslations,
    /* arm_and_activate         */ NULL,
    /* get_resources            */ NULL,
    /* num get_resources        */ 0,
    /* extension 		*/ /* NULL */ /* SGI has multiple extensions */
  },
  {
    /* HGUglwCanvas */
    /* dummy field		*/ 0,
  },
  {
    /* HGUglwCanvasTb */
    /* dummy field 		*/ 0,
  }
};

WidgetClass     hguGLwCanvasTbWidgetClass =
					 (WidgetClass )&hguGLwCanvasTbClassRec;

/*!
* \ingroup	HGU_GL
* \brief	Widget Method: Initialises the trackball.
* \param	reqW			Unused.
* \param	newW			The new widget.
* \param	args			Unused.
* \param	numArgs			Unused.
*/
static void     Initialize(Widget reqW, Widget newW,
                           ArgList args, Cardinal *numArgs)
{
  HGUglwCanvasTbWidget reqCtW;
  HGUglwCanvasTbPart *reqCtb;

  reqCtW = (HGUglwCanvasTbWidget )reqW;
  reqCtb = &(reqCtW->hguGLwCanvasTb);
  HGUglwCanvasTbSetAnimateInterval(newW, reqCtb->animateInterval);
  HGUglwCanvasTbSetTrackballSize(newW, reqCtb->trackballSize);
  HGUglwCanvasTbSetMotionInterval(newW, reqCtb->motionInterval);
  HGUglwCanvasTbReset(newW);
}

/*!
* \ingroup	HGU_GL
* \brief	Destroy Method: Destroys/frees trackball stuff.
* \param	givenW			Given widget.
*/
static void     Destroy(Widget givenW)
{
  XtRemoveAllCallbacks(givenW, HGUglwNtrackballCallback);
}

/*!
* \return	TRUE if an expose event is required (always FALSE).
* \ingroup	HGU_GL
* \brief	Widget SetValues: Checks validity of some of the trackball
* 		resources.
* \param	curW			The current widget.
* \param	reqW			Unused.
* \param	newW			Unused.
* \param	args			Unused.
* \param	numArgs			Unused.
*/
static Boolean	SetValues(Widget curW, Widget reqW, Widget newW,
                          ArgList args, Cardinal *numArgs)
{
  HGUglwCanvasTbWidget newCtW;
  HGUglwCanvasTbPart *newCtb;

  newCtW = (HGUglwCanvasTbWidget )newW;
  newCtb = &(newCtW->hguGLwCanvasTb);
  HGUglwCanvasTbSetAnimateInterval(newW, newCtb->animateInterval);
  HGUglwCanvasTbSetTrackballSize(newW, newCtb->trackballSize);
  HGUglwCanvasTbSetMotionInterval(newW, newCtb->motionInterval);
  return(FALSE);
}

/*!
* \ingroup	HGU_GL
* \brief	Action Procedure: Start of trackball rotation.
* \param	givenW			Widget instance.
* \param	event			Event that caused the action.
* \param	args			Unsed.
* \param	argCount		Unsed.
*/
void		HGUglwCanvasTbMotionBeginRotate(Widget givenW, XEvent *event,
				  		String *args,
						Cardinal *argCount)
{
  HGUglwCanvasTbMotionBeginFn((HGUglwCanvasTbWidget )givenW, event,
  		      HGUglwCANVASTB_MODE_ROTATE);
}

/*!
* \ingroup	HGU_GL
* \brief	Action Procedure: Start of trackball x-y translation.
* \param	givenW			Widget instance.
* \param	event			Event that caused the action.
* \param	args			Unused.
* \param	argCount		Unused.q
*/
void		HGUglwCanvasTbMotionBeginXY(Widget givenW, XEvent *event,
				    	    String *args, Cardinal *argCount)
{
  HGUglwCanvasTbMotionBeginFn((HGUglwCanvasTbWidget )givenW, event,
  		      HGUglwCANVASTB_MODE_XY);
}

/*!
* \ingroup	HGU_GL
* \brief	Action Procedure: Start of trackball z translation.
* \param	givenW			Widget instance.
* \param	event			Event that caused the action.
* \param	args			Unused.
* \param	argCount		Unused.
*/
void		HGUglwCanvasTbMotionBeginZ(Widget givenW, XEvent *event,
				    	   String *args, Cardinal *argCount)
{
  HGUglwCanvasTbMotionBeginFn((HGUglwCanvasTbWidget )givenW, event,
  		      HGUglwCANVASTB_MODE_Z);
}

/*!
* \ingroup	HGU_GL
* \brief	Function used by the HGUglwCanvasTbMotionBegin.*()'s to
*		do most things.
* \param	tbW			Canvas / trackball widget instance.
* \param	event			Event to be processed.
* \param	newMode			New mode for trackball.
*/
static void	HGUglwCanvasTbMotionBeginFn(HGUglwCanvasTbWidget tbW,
					    XEvent *event,
				    	    HGUglwTrackballMode newMode)
{
  HGUglwCanvasTbPart *ctb;

  ctb = &(tbW->hguGLwCanvasTb);
  HGUglwCanvasTbStop((Widget )tbW);
  ctb->trackballMode = newMode;
  switch(event->type)
  {
    case ButtonPress:
      ctb->tbEvent.xkey = event->xkey;
      ctb->motionTime = event->xbutton.time;
      WLZ_VTX_2_SET(ctb->motionPos, event->xbutton.x, event->xbutton.y);
      break;
    case KeyPress:
      ctb->tbEvent.xkey = event->xkey;
      ctb->motionTime = event->xkey.time;
      WLZ_VTX_2_SET(ctb->motionPos, event->xkey.x, event->xkey.y);
      break;
    default:
      break;
  }
}

/*!
* \ingroup	HGU_GL
* \brief	Action Procedure: Rotate trackball.
* \param	givenW			Widget instance.
* \param	event			Event that caused the action.
* \param	args			Unused.
* \param	argCount			Unused.
*/
void		HGUglwCanvasTbMotionRotate(Widget givenW, XEvent *event,
			  	   	   String *args, Cardinal *argCount)
{
  WlzIVertex2	curPos;
  HGUglwCanvasTbWidget tbW;
  HGUglwCanvasTbPart *ctb;

  tbW = (HGUglwCanvasTbWidget )givenW;
  ctb = &(tbW->hguGLwCanvasTb);
  if(ctb->trackballMode == HGUglwCANVASTB_MODE_ROTATE)
  {
    switch(event->type)
    {
      case MotionNotify:
	WLZ_VTX_2_SET(curPos, event->xmotion.x, event->xmotion.y);
	if((curPos.vtX != ctb->motionPos.vtX) ||
	   (curPos.vtY != ctb->motionPos.vtY))
	{
	  HGUglwCanvasTbCalculateRotate(tbW, curPos);
	  ctb->tbEvent.xmotion = event->xmotion;
	  ctb->motionTime = event->xmotion.time;
	  ctb->motionPos = curPos;
	  HGUglwCanvasTbUpdate(givenW);
	}
	break;
      default:
	break;
    }
  }
}

/*!
* \ingroup	HGU_GL
* \brief	Update the trackball rotation for the given current
*		position. The current and last positions are then used
*		two calculate two normalised points (ie [-1.0, +1.0]).
*		The axis of rotation is	then the cross product of the
*		two vectors through the	normalised points and the
*		trackball's origin.
* \param	tbW			Canvas / trackball widget instance.
* \param	curPos			New position.
*/
void		HGUglwCanvasTbCalculateRotate(HGUglwCanvasTbWidget tbW,
				     	      WlzIVertex2 curPos)
{
  int		width,
  		height;
  double	phi,
  		tD0;
  WlzDVertex2	point1,
  		point2;
  WlzDVertex3	axis,
  		proj1,
		proj2,
		diff;
  HGUglwCanvasTbPart *ctb;

  ctb = &(tbW->hguGLwCanvasTb);
  if(((width = tbW->core.width) > 0) && ((height = tbW->core.height) > 0))
  {
    point1.vtX = ((2.0 * ctb->motionPos.vtX) / width) - 1.0;
    point1.vtY = 1.0 - ((2.0 * ctb->motionPos.vtY ) / height);
    point2.vtX = ((2.0 * curPos.vtX) / width) - 1.0;
    point2.vtY = 1.0 - ((2.0 * curPos.vtY ) / height);
    ctb->motionPos = curPos;
    tD0 = 0.01 * ctb->trackballSize;
    WLZ_VTX_3_SET(proj1, point1.vtX, point1.vtY,
		  0.0 - HGUglwCanvasTbProjectToZ(tD0, point1));
    WLZ_VTX_3_SET(proj2, point2.vtX, point2.vtY,
		  0.0 - HGUglwCanvasTbProjectToZ(tD0, point2));
    WLZ_VTX_3_CROSS(axis, proj1, proj2);
    WLZ_VTX_3_SUB(diff, proj1, proj2);
    tD0 = WLZ_VTX_3_LENGTH(diff) / (2 * 0.01 * ctb->trackballSize);
    if(tD0 > 1.0)
    {
      tD0 = 1.0;
    }
    else if(tD0 < -1.0)
    {
      tD0 = -1.0;
    }
    phi = 2.0 * asin(tD0);
    ctb->rotateDelta = HGUglQuatFromAxis(axis, phi);
  }
}

/*!
* \ingroup	HGU_GL
* \brief	Action Procedure: Translate trackball in x-y plane.
* \param	givenW			Widget instance.	
* \param	event			Event that caused the action.
* \param	args			Unused.
* \param	argCount		Unused.
*/
void		HGUglwCanvasTbMotionXY(Widget givenW, XEvent *event,
			  	       String *args, Cardinal *argCount)
{
  WlzIVertex2	curPos;
  HGUglwCanvasTbWidget tbW;
  HGUglwCanvasTbPart *ctb;

  tbW = (HGUglwCanvasTbWidget )givenW;
  ctb = &(tbW->hguGLwCanvasTb);
  if(ctb->trackballMode == HGUglwCANVASTB_MODE_XY)
  {
    switch(event->type)
    {
      case MotionNotify:
	WLZ_VTX_2_SET(curPos, event->xmotion.x, event->xmotion.y);
	if((curPos.vtX != ctb->motionPos.vtX) ||
	   (curPos.vtY != ctb->motionPos.vtY))
	{
	  HGUglwCanvasTbCalculateXY(tbW, curPos);
	  ctb->tbEvent.xmotion = event->xmotion;
	  ctb->motionTime = event->xmotion.time;
	  ctb->motionPos = curPos;
	  HGUglwCanvasTbUpdate(givenW);
	}
	break;
      default:
	break;
    }
  }
}

/*!
* \ingroup	HGU_GL
* \brief	Update the trackball xy translation for the given
*		current pointer position.
* \param	tbW			Canvas / trackball widget instance.
* \param	curPos			New position.
*/
static void	HGUglwCanvasTbCalculateXY(HGUglwCanvasTbWidget tbW,
					  WlzIVertex2 curPos)
{
  int		width,
  		height;
  HGUglwCanvasTbPart *ctb;

  ctb = &(tbW->hguGLwCanvasTb);
  if(((width = tbW->core.width) > 0) && ((height = tbW->core.height) > 0))
  {
    ctb->translateDelta.vtX = ((double )(2 * (curPos.vtX -
    					      ctb->motionPos.vtX))) /
    			      width;
    ctb->translateDelta.vtY = ((double )(2 * (ctb->motionPos.vtY -
    					      curPos.vtY))) /
    			      height;
    ctb->translateDelta.vtZ = 0.0;
  }
}

/*!
* \ingroup	HGU_GL
* \brief	Action Procedure: Translate trackball along z axis.
* \param	givenW			Widget instance.
* \param	event			Event that caused the action.
* \param	args			Unused.
* \param	argCount		Unused.
*/
void		HGUglwCanvasTbMotionZ(Widget givenW, XEvent *event,
			  	      String *args, Cardinal *argCount)
{
  WlzIVertex2	curPos;
  HGUglwCanvasTbWidget tbW;
  HGUglwCanvasTbPart *ctb;

  tbW = (HGUglwCanvasTbWidget )givenW;
  ctb = &(tbW->hguGLwCanvasTb);
  if(ctb->trackballMode == HGUglwCANVASTB_MODE_Z)
  {
    switch(event->type)
    {
      case MotionNotify:
	WLZ_VTX_2_SET(curPos, event->xmotion.x, event->xmotion.y);
	if((curPos.vtX != ctb->motionPos.vtX) ||
	   (curPos.vtY != ctb->motionPos.vtY))
	{
	  HGUglwCanvasTbCalculateZ(tbW, curPos);
	  ctb->tbEvent.xmotion = event->xmotion;
	  ctb->motionTime = event->xmotion.time;
	  ctb->motionPos = curPos;
	  HGUglwCanvasTbUpdate(givenW);
	}
	break;
      default:
	break;
    }
  }
}

/*!
* \ingroup	HGU_GL
* \brief	Update the trackball z translation for the given
*		current pointer position.
* \param	tbW			Canvas / trackball widget instance.
* \param	curPos			New position.
*/
static void	HGUglwCanvasTbCalculateZ(HGUglwCanvasTbWidget tbW,
					 WlzIVertex2 curPos)
{
  int		height;
  HGUglwCanvasTbPart *ctb;

  ctb = &(tbW->hguGLwCanvasTb);
  if((tbW->core.width > 0) && ((height = tbW->core.height) > 0))
  {
    ctb->translateDelta.vtX = 0.0;
    ctb->translateDelta.vtY = 0.0;
    ctb->translateDelta.vtZ = ((double )(2 * (ctb->motionPos.vtY -
    					      curPos.vtY))) /
    			      height;
  }
}

/*!
* \ingroup	HGU_GL
* \brief	Action Procedure: End of non-free trackball motion.
* \param	givenW			Widget instance.
* \param	event			Event that caused the action.
* \param	args			Unused.
* \param	argCount		Unused.
*/
void		HGUglwCanvasTbMotionEnd(Widget givenW, XEvent *event,
			  	        String *args, Cardinal *argCount)
{
  HGUglwTrackballMode newMode;
  HGUglwCanvasTbWidget tbW;
  HGUglwCanvasTbPart *ctb;

  tbW = (HGUglwCanvasTbWidget )givenW;
  ctb = &(tbW->hguGLwCanvasTb);
  newMode = HGUglwCANVASTB_MODE_STILL;
  if((ctb->trackballMode == HGUglwCANVASTB_MODE_ROTATE) ||
     (ctb->trackballMode == HGUglwCANVASTB_MODE_XY) ||
     (ctb->trackballMode == HGUglwCANVASTB_MODE_Z))
  {
    switch(event->type)
    {
      case ButtonPress:
      case ButtonRelease:
        ctb->tbEvent.xbutton = event->xbutton;
	if((event->xbutton.time - ctb->motionTime) < ctb->motionInterval)
	{
	  newMode = HGUglwCANVASTB_MODE_ANIMATION;
	}
	break;
      case KeyPress:
        ctb->tbEvent.xkey = event->xkey;
	if((event->xkey.time - ctb->motionTime) < ctb->motionInterval)
	{
	  newMode = HGUglwCANVASTB_MODE_ANIMATION;
	}
	break;
      case MotionNotify:
        ctb->tbEvent.xmotion = event->xmotion;
	if((event->xmotion.time - ctb->motionTime) < ctb->motionInterval)
	{
	  newMode = HGUglwCANVASTB_MODE_ANIMATION;
	}
	break;
      default:
	break;
    }
  }
  if(newMode == HGUglwCANVASTB_MODE_ANIMATION)
  {
    HGUglwCanvasTbAnimate(givenW);
  }
  else
  {
    HGUglwCanvasTbStop(givenW);
  }
}

/*!
* \ingroup	HGU_GL
* \brief	Reset trackball to identity, ie no rotation, 	
*		no translation and no animation.
* \param	givenW			Widget instance.
*/
void            HGUglwCanvasTbReset(Widget givenW)
{
  HGUglwCanvasTbWidget tbW;
  HGUglwCanvasTbPart *ctb;

  if(givenW)
  {
    tbW = (HGUglwCanvasTbWidget )givenW;
    ctb = &(tbW->hguGLwCanvasTb);
    HGUgl_QUAT_SET4(ctb->rotateCurrent, 1.0, 0.0, 0.0, 0.0);
    WLZ_VTX_3_SET(ctb->translateCurrent,  0.0, 0.0, 0.0);
    HGUglwCanvasTbStop(givenW);
  }
}

/*!
* \ingroup	HGU_GL
* \brief	Stop trackball with rotation and translation delta's
*		set to identity.
* \param	givenW			Widget instance.
*/
void            HGUglwCanvasTbStop(Widget givenW)
{
  HGUglwCanvasTbWidget tbW;
  HGUglwCanvasTbPart *ctb;

  if(givenW)
  {
    tbW = (HGUglwCanvasTbWidget )givenW;
    ctb = &(tbW->hguGLwCanvasTb);
    HGUgl_QUAT_SET4(ctb->rotateDelta, 1.0, 0.0, 0.0, 0.0);
    WLZ_VTX_3_SET(ctb->translateDelta,  0.0, 0.0, 0.0);
    HGUglwCanvasTbPause(givenW);
  }
}

/*!
* \ingroup	HGU_GL
* \brief	Pause trackball without changing the rotation and
*		translation delta's.
* \param	givenW			Widget instance.
*/
void            HGUglwCanvasTbPause(Widget givenW)
{
  HGUglwCanvasTbWidget tbW;
  HGUglwCanvasTbPart *ctb;

  if(givenW)
  {
    tbW = (HGUglwCanvasTbWidget )givenW;
    ctb = &(tbW->hguGLwCanvasTb);
    ctb->trackballMode = HGUglwCANVASTB_MODE_STILL;
  }
}

/*!
* \ingroup	HGU_GL
* \brief 	Start trackball animation.
* \param	givenW			Widget instance.
*/
void		HGUglwCanvasTbAnimate(Widget givenW)
{
  HGUglwCanvasTbWidget tbW;
  HGUglwCanvasTbPart *ctb;

  if(givenW)
  {
    tbW = (HGUglwCanvasTbWidget )givenW;
    ctb = &(tbW->hguGLwCanvasTb);
    ctb->trackballMode = HGUglwCANVASTB_MODE_ANIMATION;
    if(ctb->animateFnId == 0)
    {
      ctb->animateFnId = XtAppAddTimeOut(XtWidgetToApplicationContext(givenW),
      				        ctb->animateInterval,
					HGUglwCanvasTbAnimateFn,
					(XtPointer )tbW);
    }
  }
}

/*!
* \ingroup	HGU_GL
* \brief	Calls the trackball animation function without setting
*		the trackball mode to HGUglwCANVASTB_MODE_ANIMATION so
*		that a single update is done.
* \param	givenW			Widget instance.
*/
void		HGUglwCanvasTbUpdate(Widget givenW)
{
  HGUglwCanvasTbWidget tbW;

  if(givenW)
  {
    tbW = (HGUglwCanvasTbWidget )givenW;
    (void )XtAppAddTimeOut(XtWidgetToApplicationContext(givenW),
			   1,
    			   HGUglwCanvasTbAnimateFn,
    			   (XtPointer )tbW);
  }
}

/*!
* \ingroup	HGU_GL
* \brief	Trackball animation time-out procedure which calls any 
*		trackball callbacks and then if the trackball is in
*		it's animation mode reregisters itself as a time-out
*		procedure.
* \param	clientData		Used to pass the trackball instance.
* \param	id			Timeout function id.
*/
static void	HGUglwCanvasTbAnimateFn(XtPointer clientData, XtIntervalId *id)
{
  Widget		givenW;
  HGUglwCanvasTbWidget tbW;
  HGUglwCanvasTbPart *ctb;
  HGUglwCanvasCallbackStruct cb;

  givenW = (Widget )clientData;
  tbW = (HGUglwCanvasTbWidget )clientData;
  ctb = &(tbW->hguGLwCanvasTb);
  WLZ_VTX_3_ADD(ctb->translateCurrent, ctb->translateCurrent,
  		ctb->translateDelta);
  ctb->rotateCurrent = HGUglQuatProduct(ctb->rotateCurrent, ctb->rotateDelta);
  cb.reason = HGUglw_CR_TRACKBALL;
  cb.event = &(ctb->tbEvent);
  cb.width = tbW->core.width;
  cb.height = tbW->core.height;
  cb.glxCtx = tbW->hguGLwCanvas.glxCtx;
  XtCallCallbacks((Widget )tbW, HGUglwNtrackballCallback, &cb);
  if(ctb->trackballMode != HGUglwCANVASTB_MODE_ANIMATION)
  {
    ctb->animateFnId = 0;
  }
  else
  {
    ctb->animateFnId = XtAppAddTimeOut(XtWidgetToApplicationContext(givenW),
				       ctb->animateInterval,
				       HGUglwCanvasTbAnimateFn,
				       clientData);
  }
}

/*!
* \ingroup	HGU_GL
* \brief	Converts current rotation quaternion to an Open GL
*		rotation matrix, using the given Open GL matrix.
* \param	givenW			Widget instance
* \param	matrix[4][4]		Given rotation matrix.
*/
void		HGUglwCanvasTbGetRotateMatrixGL(Widget givenW,
					        double matrix[4][4])
{
  HGUglwCanvasTbWidget tbW;

  if(givenW)
  {
    tbW = (HGUglwCanvasTbWidget )givenW;
    HGUglQuatToMatrixGL(matrix, tbW->hguGLwCanvasTb.rotateCurrent);
  }
}

/*!
* \ingroup	HGU_GL
* \brief	Gets current trackball XYZ translation.
* \param	givenW			Widget instance.
* \param	translate		Destination for translation.
*/
void		HGUglwCanvasTbGetTranslate(Widget givenW,
				           WlzDVertex3 *translate)
{
  HGUglwCanvasTbWidget tbW;

  if(givenW && translate)
  {
    tbW = (HGUglwCanvasTbWidget )givenW;
    *translate = tbW->hguGLwCanvasTb.translateCurrent;
  }
}

/*!
* \return	Animation interval, or 0 on error.
* \ingroup	HGU_GL
* \brief	Gets current trackball animation interval, ie the
*		minimum time between anaimation frames in milliseconds.
* \param	givenW			Widget instance.
*/
int		HGUglwCanvasTbGetAnimateInterval(Widget givenW)
{
  int		itvl = -1;
  HGUglwCanvasTbWidget tbW;

  if(givenW)
  {
    tbW = (HGUglwCanvasTbWidget )givenW;
    itvl = tbW->hguGLwCanvasTb.animateInterval;
  }
  return(itvl);
}

/*!
* \ingroup	HGU_GL
* \brief	Sets current trackball animation interval, ie the
*		minimum time between animation frames in milliseconds.
* \param	givenW			Widget instance.
* \param	itvl			New animation interval.
*/
void		HGUglwCanvasTbSetAnimateInterval(Widget givenW, int itvl)
{
  HGUglwCanvasTbWidget tbW;

  if(givenW)
  {
    if(itvl < 1)
    {
      itvl = 1;
    }
    tbW = (HGUglwCanvasTbWidget )givenW;
    tbW->hguGLwCanvasTb.animateInterval = itvl;
  }
}

/*!
* \return	Motion interval, or 0 on error.
* \ingroup	HGU_GL
* \brief	Gets current trackball motion interval, ie the	
*		minimum time between motion events for no motion.
* \param	givenW			Widget instance.
*/
int		HGUglwCanvasTbGetMotionInterval(Widget givenW)
{
  int		itvl = -1;
  HGUglwCanvasTbWidget tbW;

  if(givenW)
  {
    tbW = (HGUglwCanvasTbWidget )givenW;
    itvl = tbW->hguGLwCanvasTb.motionInterval;
  }
  return(itvl);
}

/*!
* \ingroup	HGU_GL
* \brief	Sets current trackball motion interval, ie the	
*		minimum time between motion events for no motion.
* \param	givenW			Widget instance.
* \param	itvl			New motion interval.
*/
void		HGUglwCanvasTbSetMotionInterval(Widget givenW, int itvl)
{
  HGUglwCanvasTbWidget tbW;

  if(givenW)
  {
    if(itvl < 1)
    {
      itvl = 1;
    }
    tbW = (HGUglwCanvasTbWidget )givenW;
    tbW->hguGLwCanvasTb.motionInterval = itvl;
  }
}

/*!
* \return	Trackball size, or 0 on error.
* \ingroup	HGU_GL
* \brief	Gets current trackball size as percentage of the
*		widgets window size.
* \param	givenW			Widget instance.
*/
int		HGUglwCanvasTbGetTrackballSize(Widget givenW)
{
  int		size = -1;
  HGUglwCanvasTbWidget tbW;

  if(givenW)
  {
    tbW = (HGUglwCanvasTbWidget )givenW;
    size = tbW->hguGLwCanvasTb.trackballSize;
  }
  return(size);
}

/*!
* \ingroup	HGU_GL
* \brief	Sets current trackball size as percentage of the
*		widgets window size.
* \param	givenW			Widget instance.
* \param	size			Requested size.
*/
void		HGUglwCanvasTbSetTrackballSize(Widget givenW, int size)
{
  HGUglwCanvasTbWidget tbW;

  if(givenW)
  {
    if(size < 1)
    {
      size = 1;
    }
    else if(size > 100)
    {
      size = 100;
    }
    tbW = (HGUglwCanvasTbWidget )givenW;
    tbW->hguGLwCanvasTb.trackballSize = size;
  }
}

/*!
* \return	Z coordinate of projection.
* \ingroup	HGU_GL
* \brief	Projects the given point onto the trackball sphere with
*		the given radius, or a hyperbolic sheet if the point is
*		away from the center of the sphere.
* \param	size			The trackball size [0.0-1.0].
* \param	point			Given point.
*/
static double	HGUglwCanvasTbProjectToZ(double size, WlzDVertex2 point)
{
  double	sqLen,
    		projZ,
		tD0;

  sqLen = WLZ_VTX_2_SQRLEN(point);  /* Square of length from origin to point */
  size *= size;      		  /* Use square of trackball size, [0.0-1.0] */
  tD0 = size * 0.5;
  if(sqLen < tD0)					    /* Inside sphere */
  {
    projZ = sqrt(size - sqLen);
  }
  else							     /* On hyperbola */
  {
    projZ = tD0 / sqrt(sqLen);
  }
  return(projZ);
}
