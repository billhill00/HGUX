#pragma ident "MRC HGU $Id$"
/************************************************************************
*   Copyright  :   1994 Medical Research Council, UK.                   *
*                  All rights reserved.                                 *
*************************************************************************
*   Address    :   MRC Human Genetics Unit,                             *
*                  Western General Hospital,                            *
*                  Edinburgh, EH4 2XU, UK.                              *
*************************************************************************
*   Project    :   MRC HGU Image Processing Utilities			*
*   File       :   HGU_XGetPolyDomain.c					*
* $Revision$
*************************************************************************
*   Author Name :  Richard Baldock					*
*   Author Login:  richard@hgu.mrc.ac.uk				*
*   Date        :  Tue Mar 10 22:47:21 1998				*
*   Synopsis    : 							*
*************************************************************************
*   Maintenance :  date - name - comments (Last changes at the top)	*
************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>

#include <HGU_XInteract.h>

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

WlzPolygonDomain *HGU_XGetPolyDomain(
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
    
  /* set up the callback struct */
  cb_struct.event = &event;

  /* get interact gc */
  gc = HGU_XGetGC( dpy, win, HGU_XGC_INTERACT );
    
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
  return_polydmn = WlzMakePolyDmn(WLZ_POLYGON_FLOAT,
				  (WlzIVertex2 *) vtxs,
				  nvtxs+1, nvtxs+2, 1, NULL);
  AlcFree( (void *) vtxs );
  return return_polydmn;
}
