#if defined(__GNUC__)
#ident "MRC HGU $Id:"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id:"
#else static char _HGUgl_canvasTb_c[] = "MRC HGU $Id:";
#endif
#endif
/*!
* \file         HGUglwCanvasTb.c
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Wed Apr 29 11:11:31 2009
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
* \brief        Methods for the HGUglCanvasTb MRC HGU OpenGL
*		widget. The HGUglCanvasTb widget provides both a canvas
*		drawing area and a 3D virtual trackball, it is descended
*		from the HGUglCanvas OpenGL widget. See the manual page
*		HGUglCanvasTb(3) for a description of the widget.
*               
*
* Maintenance log with most recent changes at top of list.
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
static void	Initialize(Widget, Widget, ArgList, Cardinal *),
		Destroy(Widget);
static Boolean	SetValues(Widget,  Widget, Widget, ArgList, Cardinal *);
/* Action procedures: */
void		HGUglwCanvasTbMotionBeginRotate(Widget, XEvent *, String *,
						Cardinal *),
		HGUglwCanvasTbMotionBeginXY(Widget, XEvent *, String *,
					    Cardinal *),
		HGUglwCanvasTbMotionBeginZ(Widget, XEvent *, String *,
					   Cardinal *),
		HGUglwCanvasTbMotionRotate(Widget, XEvent *, String *, 
					   Cardinal *),
		HGUglwCanvasTbMotionXY(Widget, XEvent *, String *,
				       Cardinal *),
		HGUglwCanvasTbMotionZ(Widget, XEvent *, String *,
				      Cardinal *),
		HGUglwCanvasTbMotionEnd(Widget, XEvent *, String *,
				        Cardinal *);
/* Timeout procedures: */
static void	HGUglwCanvasTbAnimateFn(XtPointer, XtIntervalId *);
/* And the rest: */
static void	HGUglwCanvasTbMotionBeginFn(HGUglwCanvasTbWidget,
					    XEvent *, HGUglwTrackballMode),
		HGUglwCanvasTbCalculateRotate(HGUglwCanvasTbWidget,
					      WlzIVertex2),
		HGUglwCanvasTbCalculateXY(HGUglwCanvasTbWidget, 
					  WlzIVertex2),
		HGUglwCanvasTbCalculateZ(HGUglwCanvasTbWidget,
					 WlzIVertex2);
static double	HGUglwCanvasTbProjectToZ(double, WlzDVertex2);

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

/************************************************************************
* Function:     Initialize                                             
* Returns:      void                                                   
* Purpose:      Widget Method: Initialises the trackball.	
* Global refs:  -                                                      
* Parameters:   Widget reqW:		NOT USED
*               Widget newW:
*               ArgList args:		NOT USED.		
*               Cardinal *numArgs:	NOT USED.		
************************************************************************/
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

/************************************************************************
* Function:	Destroy						
* Returns:      void                                                   
* Purpose:      Destroy Method: Destroys/frees trackball stuff.	
* Global refs:  -                                                      
* Parameters:   Widget givenW:
************************************************************************/
static void     Destroy(Widget givenW)
{
  XtRemoveAllCallbacks(givenW, HGUglwNtrackballCallback);
}

/************************************************************************
* Function:     SetValues					
* Returns:      Boolean:		TRUE if an expose event is
*					required (always FALSE).
* Purpose:      Widget SetValues: Checks validity of some of the
*		trackball resources.				
* Global refs:  -                                                      
* Parameters:   Widget curW:
*		Widget reqW:		NOT USED.		
*               Widget newW:		NOT USED.		
*               ArgList args:		NOT USED.		
*               Cardinal *numArgs:	NOT USED.		
************************************************************************/
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

/************************************************************************
* Function:     HGUglwCanvasTbMotionBeginRotate			
* Returns:      void                                                   
* Purpose:      Action Procedure: Start of trackball rotation.	
* Global refs:  -                                                      
* Parameters:   Widget givenW:		Widget instance.	
*		XEvent *event:		Event that caused the action.
*		String *args:		NOT USED.		
*		cardinal *argCount:	NOT USED.		
************************************************************************/
void		HGUglwCanvasTbMotionBeginRotate(Widget givenW, XEvent *event,
				  		String *args,
						Cardinal *argCount)
{
  HGUglwCanvasTbMotionBeginFn((HGUglwCanvasTbWidget )givenW, event,
  		      HGUglwCANVASTB_MODE_ROTATE);
}

/************************************************************************
* Function:     HGUglwCanvasTbMotionBeginXY			
* Returns:      void                                                   
* Purpose:      Action Procedure: Start of trackball x-y translation.
* Global refs:  -                                                      
* Parameters:   Widget givenW:		Widget instance.	
*		XEvent *event:		Event that caused the action.
*		String *args:		NOT USED.		
*		cardinal *argCount:	NOT USED.		
************************************************************************/
void		HGUglwCanvasTbMotionBeginXY(Widget givenW, XEvent *event,
				    	    String *args, Cardinal *argCount)
{
  HGUglwCanvasTbMotionBeginFn((HGUglwCanvasTbWidget )givenW, event,
  		      HGUglwCANVASTB_MODE_XY);
}

/************************************************************************
* Function:     HGUglwCanvasTbMotionBeginZ			
* Returns:      void                                                   
* Purpose:      Action Procedure: Start of trackball z translation.
* Global refs:  -                                                      
* Parameters:   Widget givenW:		Widget instance.	
*		XEvent *event:		Event that caused the action.
*		String *args:		NOT USED.		
*		cardinal *argCount:	NOT USED.		
************************************************************************/
void		HGUglwCanvasTbMotionBeginZ(Widget givenW, XEvent *event,
				    	   String *args, Cardinal *argCount)
{
  HGUglwCanvasTbMotionBeginFn((HGUglwCanvasTbWidget )givenW, event,
  		      HGUglwCANVASTB_MODE_Z);
}

/************************************************************************
* Function:     HGUglwCanvasTbMotionBeginFn			
* Returns:      void                                                   
* Purpose:      Function used by the HGUglwCanvasTbMotionBegin.*()'s to
*		do most things.					
* Global refs:  -                                                      
* Parameters:   HGUglwCanvasTbWidget tbW: Canvas / trackball widget
*					instance.		
*		XEvent *event:		Event to be processed.	
*		HGUglwTrackballMode newMode: New mode for trackball.
************************************************************************/
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

/************************************************************************
* Function:     HGUglwCanvasTbMotionRotate			
* Returns:      void                                                   
* Purpose:      Action Procedure: Rotate trackball.		
* Global refs:  -                                                      
* Parameters:   Widget givenW:		Widget instance.	
*		XEvent *event:		Event that caused the action.
*		String *args:		NOT USED.		
*		cardinal *argCount:	NOT USED.		
************************************************************************/
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

/************************************************************************
* Function:	HGUglwCanvasTbCalculateRotate			
* Returns:	void						
* Purpose:	Update the trackball rotation for the given current
*		position. The current and last positions are then used
*		two calculate two normalised points (ie [-1.0, +1.0]).
*		The axis of rotation is	then the cross product of the
*		two vectors through the	normalised points and the
*		trackball's origin.				
* Global refs:	-						
* Parameters:   HGUglwCanvasTbWidget tbW: Canvas / trackball widget
*					instance.		
*		WlzIVertex2 curPos:	New position.		
************************************************************************/
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

/************************************************************************
* Function:     HGUglwCanvasTbMotionXY				
* Returns:      void                                                   
* Purpose:      Action Procedure: Translate trackball in x-y plane.
* Global refs:  -                                                      
* Parameters:   Widget givenW:		Widget instance.	
*		XEvent *event:		Event that caused the action.
*		String *args:		NOT USED.		
*		cardinal *argCount:	NOT USED.		
************************************************************************/
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

/************************************************************************
* Function:	HGUglwCanvasTbCalculateXY			
* Returns:	void						
* Purpose:	Update the trackball xy translation for the given
*		current pointer position. 			
* Global refs:	-						
* Parameters:   HGUglwCanvasTbWidget tbW: Canvas / trackball widget
*					instance.		
*		WlzIVertex2 curPos:	New position.		
************************************************************************/
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

/************************************************************************
* Function:     HGUglwCanvasTbMotionZ				
* Returns:      void                                                   
* Purpose:      Action Procedure: Translate trackball along z axis.
* Global refs:  -                                                      
* Parameters:   Widget givenW:		Widget instance.	
*		XEvent *event:		Event that caused the action.
*		String *args:		NOT USED.		
*		cardinal *argCount:	NOT USED.		
************************************************************************/
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

/************************************************************************
* Function:	HGUglwCanvasTbCalculateZ			
* Returns:	void						
* Purpose:	Update the trackball z translation for the given
*		current pointer position. 			
* Global refs:	-						
* Parameters:   HGUglwCanvasTbWidget tbW: Canvas / trackball widget
*					instance.		
*		WlzIVertex2 curPos:	New position.		
************************************************************************/
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

/************************************************************************
* Function:     HGUglwCanvasTbMotionEnd				
* Returns:      void                                                   
* Purpose:      Action Procedure: End of non-free trackball motion.
* Global refs:  -                                                      
* Parameters:   Widget givenW:		Widget instance.	
*		XEvent *event:		Event that caused the action.
*		String *args:		NOT USED.		
*		cardinal *argCount:	NOT USED.		
************************************************************************/
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

/************************************************************************
* Function:     HGUglwCanvasTbReset				
* Returns:      void                                                   
* Purpose:      Reset trackball to identity, ie no rotation, 	
*		no translation and no animation.		
* Global refs:  -                                                      
* Parameters:    Widget givenW:		Widget instance.	
************************************************************************/
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

/************************************************************************
* Function:     HGUglwCanvasTbStop                                     
* Returns:      void                                                   
* Purpose:      Stop trackball with rotation and translation delta's
*		set to identity.				
* Global refs:  -                                                      
* Parameters:    Widget givenW:		Widget instance.	
************************************************************************/
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

/************************************************************************
* Function:     HGUglwCanvasTbPause				
* Returns:      void                                                   
* Purpose:      Pause trackball without changing the rotation and
*		translation delta's.				
* Global refs:  -                                                      
* Parameters:    Widget givenW:		Widget instance.	
************************************************************************/
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

/************************************************************************
* Function:	HGUglwCanvasTbAnimate				
* Returns:	void						
* Purpose:	Start trackball animation.			
* Global refs:	-						
* Parameters:	Widget givenW:		Widget instance.	
************************************************************************/
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

/************************************************************************
* Function:	HGUglwCanvasTbUpdate				
* Returns:	void						
* Purpose:	Calls the trackball animation function without setting
*		the trackball mode to HGUglwCANVASTB_MODE_ANIMATION so
*		that a single update is done.			
* Global refs:	-						
* Parameters:	 Widget givenW:		Widget instance.	
************************************************************************/
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

/************************************************************************
* Function:	HGUglwCanvasTbAnimateFn				
* Returns:	void						
* Purpose:	Trackball animation time-out procedure which calls any 
*		trackball callbacks and then if the trackball is in
*		it's animation mode reregisters itself as a time-out
*		procedure.					
* Global refs:	-						
* Parameters:	XtPointer clientData:	Used to pass the trackball
*					instance.		
*		XtIntervalId *id:	Timeout function id.	
************************************************************************/
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

/************************************************************************
* Function:	HGUglwCanvasTbGetRotateMatrixGL			
* Returns:	void						
* Purpose:	Converts current rotation quaternion to an Open GL
*		rotation matrix, using the given Open GL matrix.
* Global refs:	-						
* Parameters:	Widget givenW:		Widget instance		
*		double matrix[4][4]:	Given rotation matrix.	
************************************************************************/
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

/************************************************************************
* Function:	HGUglwCanvasTbGetTranslate			
* Returns:	void						
* Purpose:	Gets current trackball XYZ translation.		
* Global refs:	-						
* Parameters:	Widget givenW:		Widget instance		
*		WlzDVertex3 *translate:	Destination for translation.
************************************************************************/
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

/************************************************************************
* Function:	HGUglwCanvasTbGetAnimateInterval		
* Returns:	int:			Animation interval, or 0 
*					on error.		
* Purpose:	Gets current trackball animation interval, ie the
*		minimum time between anaimation frames in milliseconds.
* Global refs:	-						
* Parameters:	Widget givenW:		Widget instance		
************************************************************************/
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

/************************************************************************
* Function:	HGUglwCanvasTbSetAnimateInterval		
* Returns:	void						
* Purpose:	Sets current trackball animation interval, ie the
*		minimum time between animation frames in milliseconds.
* Global refs:	-						
* Parameters:	Widget givenW:		Widget instance		
*		int itvl:		New animation interval.	
************************************************************************/
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

/************************************************************************
* Function:	HGUglwCanvasTbGetMotionInterval			
* Returns:	int:			Motion interval, or 0 on error.
* Purpose:	Gets current trackball motion interval, ie the	
*		minimum time between motion events for no motion.
* Global refs:	-						
* Parameters:	Widget givenW:		Widget instance		
************************************************************************/
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

/************************************************************************
* Function:	HGUglwCanvasTbSetMotionInterval			
* Returns:	void						
* Purpose:	Sets current trackball motion interval, ie the	
*		minimum time between motion events for no motion.
* Global refs:	-						
* Parameters:	Widget givenW:		Widget instance		
*		int itvl:		New motion interval.	
************************************************************************/
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

/************************************************************************
* Function:	HGUglwCanvasTbGetTrackballSize			
* Returns:	int:			Trackball size, or 0 on error.
* Purpose:	Gets current trackball size as percentage of the
*		widgets window size.				
* Global refs:	-						
* Parameters:	Widget givenW:		Widget instance		
************************************************************************/
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

/************************************************************************
* Function:	HGUglwCanvasTbSetTrackballSize			
* Returns:	void						
* Purpose:	Sets current trackball size as percentage of the
*		widgets window size.				
* Global refs:	-						
* Parameters:	Widget givenW:		Widget instance		
*		int size:		Requested size.		
************************************************************************/
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

/************************************************************************
* Function:	HGUglwCanvasTbProjectToZ			
* Returns:	double:			Z coordinate of projection.
* Purpose:	Projects the given point onto the trackball sphere with
*		the given radius, or a hyperbolic sheet if the point is
*		away from the center of the sphere.		
* Global refs:	-						
* Parameters:	double size:		The trackball size [0.0-1.0].
*		WlzDVertex2 point:	Given point.		
************************************************************************/
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
