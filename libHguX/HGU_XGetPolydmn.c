#pragma ident "MRC HGU $Id$"
/*****************************************************************************
* Copyright   :    1994 Medical Research Council, UK.                        *
*                  All rights reserved.                                      *
******************************************************************************
* Address     :    MRC Human Genetics Unit,                                  *
*                  Western General Hospital,                                 *
*                  Edinburgh, EH4 2XU, UK.                                   *
******************************************************************************
* Project     :    libhguX - MRC HGU X11 Utilities			     *
* File        :    HGU_XGetPolydmn.c					     *
* $Revision$
******************************************************************************
* Author Name :    Richard Baldock					     *
* Author Login:    richard@hgu.mrc.ac.uk				     *
* Date        :    Mon Sep 19 19:29:39 1994				     *
* Synopsis    :    Interaction routine for use with X11	to define a polygon  *
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include <math.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>

#include <HGU_XInteract.h>

static char *help_str = 
"HGU_XGetPolydmn: interactively define a polyline using the \n\
    mouse or arrow keys. A polyline is an ordered set of points\n\
    which can be defined or deleted in sequence.\n\
\n\
Define: press and release buttons 1 or 2. Button 2 can be dragged.\n\
    Press spacebar. The position of the current point is the\n\
    position of the cursor which can be moved using the mouse\n\
    or arrow keys. Ctrl<arrow-key> will move the cursor faster.\n\
\n\
Delete: press delete or backspace, holding the key pressed will delete\n\
    many points.\n\
\n\
Confirm: the currently defined polyline will be returned on pressing\n\
    button 3 or return.";

/* private routines */
static void MyDrawLine(
  Display 	*dpy,
  Window  	win,
  GC      	gc,
  WlzFVertex2	*v1,
  WlzFVertex2	*v2)
{
  int	x1 = v1->vtX,
    y1 = v1->vtY,
    x2 = v2->vtX,
    y2 = v2->vtY;
    
  XDrawLine(dpy, win, gc, x1, y1, x1, y1);
  XDrawLine(dpy, win, gc, x1, y1, x2, y2);
  XFlush( dpy );
  return;
}

#define NEW_VTX(VTX) \
if( !nvtxs || vtxs[nvtxs-1].vtX != VTX->vtX ||\
              vtxs[nvtxs-1].vtY != VTX->vtY ) {\
    if( nvtxs >= max_nvtxs ){\
        max_nvtxs += 256;\
        vtxs = (WlzFVertex2 *)\
	    AlcRealloc(vtxs, sizeof(WlzFVertex2)*max_nvtxs);\
    }\
    vtxs[nvtxs] = *(VTX);\
    if( nvtxs ){\
        MyDrawLine(dpy, win, gc, &(vtxs[nvtxs-1]), &vtxp);\
        MyDrawLine(dpy, win, gc, &(vtxs[nvtxs-1]), &(vtxs[nvtxs]));\
    }\
    nvtxs++;\
    vtxp = *(VTX);\
    MyDrawLine(dpy, win, gc, &(vtxs[nvtxs-1]), &vtxp);\
}


#define MOVE_VTX(VTX) \
if( nvtxs ){\
    MyDrawLine(dpy, win, gc, &(vtxs[nvtxs-1]), &vtxp);\
    vtxp = *(VTX);\
    MyDrawLine(dpy, win, gc, &(vtxs[nvtxs-1]), &vtxp);\
}

#define REMOVE_VTX(VTX) \
switch( nvtxs ){\
case 0:\
    break;\
case 1:\
    MyDrawLine(dpy, win, gc, &(vtxs[nvtxs-1]), &vtxp);\
    nvtxs = 0;\
    break;\
default:\
    MyDrawLine(dpy, win, gc, &(vtxs[nvtxs-1]), &vtxp);\
    nvtxs--;\
    vtxp = vtxs[nvtxs];\
    XWarpPointer( dpy, None, win, 0, 0, 0, 0, (int) vtxp.vtX,\
	     (int) vtxp.vtY );\
    break;\
}
    

#define MOVE_POINTER(DX,DY,MAG) \
dx = DX;\
dy = DY;\
if( MAG ){\
    dx *= 10;\
    dy *= 10;\
}\
XWarpPointer( dpy, None, win, 0, 0, 0, 0, (int) vtxp.vtX+dx,\
	     (int) vtxp.vtY+dy );

/* public routines */

WlzPolygonDomain *HGU_XGetPolydmn(
  Display		*dpy,
  Window		win,
  int			mode,
  HGU_XInteractCallbacks	*callbacks,
  WlzPolygonDomain	*start)
{
  XEvent		event;
  GC			gc;
  XWindowAttributes	win_att;
  WlzFVertex2		*vtxs=NULL, vtx, vtxp;
  int			nvtxs=0, max_nvtxs=0;
  int			finished = 0;
  HGU_XGetVtxsCallbackStruct	cb_struct;
  int			i;
  int			dx, dy;
  WlzPolygonDomain	*return_polydmn;
  static int		current_cursor=2;
    
  /* set up the callback struct */
  cb_struct.event = &event;

  /* get interact gc */
  gc = HGU_XGetGC( dpy, win, HGU_XGC_INTERACT );
    
  /* set the cursor */
  HGU_XSetCursor( dpy, win, HGU_XCURSOR_POINT );
    
  /* copy the given polygon */
  if( (start != NULL) && (start->nvertices > 0) ){

    for(i=0; i < start->nvertices; i++){
	    
      switch( start->type ){
      case WLZ_POLYGON_INT:
	vtx.vtX = start->vtx[i].vtX;
	vtx.vtY = start->vtx[i].vtY;
	break;
      case WLZ_POLYGON_FLOAT:
	vtx.vtX = ((WlzFVertex2 *) start->vtx)[i].vtX;
	vtx.vtY = ((WlzFVertex2 *) start->vtx)[i].vtY;
	break;
      case WLZ_POLYGON_DOUBLE:
	vtx.vtX = ((WlzDVertex2 *) start->vtx)[i].vtX;
	vtx.vtY = ((WlzDVertex2 *) start->vtx)[i].vtY;
	break;
      }
	    
      NEW_VTX((&vtx));
    }
	
    /* warp pointer */
    XWarpPointer(dpy,None,win,0,0,0,0,(int) (vtx.vtX), (int) (vtx.vtY));
  }
    
  /* select input */
  if( XGetWindowAttributes(dpy, win, &win_att) == 0 ){
    HGU_XError(dpy, win, "HGU_XGetPolydmn",
	       "failed to get window attributes", 0);
    return( NULL );
  }
  XSelectInput(dpy, win, win_att.your_event_mask |
	       KeyPressMask | KeyReleaseMask |
	       EnterWindowMask | LeaveWindowMask );
  if(  QueryInWindow( dpy, win ) ){
    HGU_XGrabPointer(dpy, win, mode&HGU_XConfineMask);
  }
    
  /* get the polyline */
  while( !finished ){
	
    XNextEvent(dpy, &event);
    if( event.xany.window != win ){
      finished = HGU_XInteractNonwindowEvent(dpy, win, mode, callbacks,
					     &event);
      continue;
    }
	
    switch( event.type ){
	    
    case ButtonPress:
      switch( event.xbutton.button ){
      case Button1:		/* new vertex or select */
      case Button2:
	vtx.vtX = event.xbutton.x;
	vtx.vtY = event.xbutton.y;
	NEW_VTX((&vtx));
	break;
      case Button3:
	finished = 1;
	break;
      default:
	break;
      }
      break;

    case ButtonRelease:
      switch( event.xbutton.button ){
      case Button1:		/*  */
      case Button2:
	/* CHECK NOCONFIRM */
	break;
      case Button3:
      default:
	break;
      }
      break;

    case KeyPress:
      switch( XLookupKeysym(&event.xkey, 0) ){
      case XK_Delete:
      case XK_BackSpace:
	vtx.vtX = event.xkey.x;
	vtx.vtY = event.xkey.y;
	REMOVE_VTX((&vtx));
	break;
      case XK_Right:
	MOVE_POINTER(1,0,event.xkey.state&ControlMask);
	break;
      case XK_Left:
	MOVE_POINTER(-1,0,event.xkey.state&ControlMask);
	break;
      case XK_Up:
	MOVE_POINTER(0,-1,event.xkey.state&ControlMask);
	break;
      case XK_Down:
	MOVE_POINTER(0,1,event.xkey.state&ControlMask);
	break;
      case XK_space:
	vtx.vtX = event.xkey.x;
	vtx.vtY = event.xkey.y;
	NEW_VTX((&vtx));
	break;
      case XK_Return:
	finished = 1;
	break;
      case XK_Help:
	HGU_XInteractHelpEvent(dpy, win, callbacks, help_str);
	break;
      case XK_C:
      case XK_c:		/* <ctrl> c toggle cursor */
	if( event.xkey.state & ControlMask ){
	  current_cursor++;
	  switch( current_cursor%3 ){
	  case 0:			/* normal cross cursor */
	    HGU_XSetCursor( dpy, win, HGU_XCURSOR_CROSS );
	    break;
	  case 1:			/* pointer cursor */
	    HGU_XSetCursor( dpy, win, HGU_XCURSOR_POINTER );
	    break;
	  case 2:			/* single point cursor */
	    HGU_XSetCursor( dpy, win, HGU_XCURSOR_POINT );
	    break;
	  }
	}
	break;
      default:
	break;
      }
      break;
	    
    case KeyRelease:
      switch( XLookupKeysym(&event.xkey, 0) ){
      case XK_Delete:
      case XK_BackSpace:
	finished = mode & HGU_XNoConfirmMask;
	break;
      default:
	break;
      }
      break;

    case MotionNotify:
      /* check button masks */
      vtx.vtX = event.xmotion.x;
      vtx.vtY = event.xmotion.y;
      if( event.xmotion.state & (Button1Mask|Button2Mask) ){
	NEW_VTX((&vtx));
      } else {
	MOVE_VTX((&vtx));
      }
      cb_struct.x = event.xmotion.x;
      cb_struct.y = event.xmotion.y;
      cb_struct.vtxs  = vtxs;
      cb_struct.nvtxs = nvtxs;
      finished = HGU_XInteractWindowEvent(dpy, win, mode, callbacks,
					  (caddr_t) &cb_struct);
      break;
	    
    case EnterNotify:
      HGU_XGrabPointer(dpy, win, mode&HGU_XConfineMask);
      break;
	    
    case LeaveNotify:
      HGU_XUngrabPointer(dpy, win);
      break;
	    
    default:
      break;
    }
  }
    
  /* remove the line */
  for(i=1; i < nvtxs; i++)
    MyDrawLine(dpy, win, gc, &vtxs[i-1], &vtxs[i]);
  if( nvtxs )
    MyDrawLine(dpy, win, gc, &vtxs[nvtxs-1], &vtxp);
    
  /* ungrab the pointer */
  HGU_XUngrabPointer(dpy, win);
    
  /* undefine the cross cursor */
  XUndefineCursor( dpy, win );

  /* reset input mask */
  XSelectInput(dpy, win, win_att.your_event_mask );
    
  /* check for defined points */
  if( nvtxs == 0 ){
    if( vtxs != NULL )
      AlcFree( (void *) vtxs );
    return( NULL );
  }
   
  /* make a polygondomain to return values */
  vtxs = (WlzFVertex2 *) AlcRealloc(vtxs, sizeof(WlzFVertex2)*(nvtxs+2));
  vtxs[nvtxs] = vtxp;
  vtxs[nvtxs+1] = vtxs[0];

  /* return the new polygon */
  return_polydmn = WlzMakePolygonDomain(WLZ_POLYGON_FLOAT, nvtxs+1,
					(WlzIVertex2 *) vtxs,
					nvtxs+2, 1, NULL);
  AlcFree( (void *) vtxs );
  return return_polydmn;
}
