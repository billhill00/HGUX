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
* File        :    HGU_XGetVtx.c					     *
* $Revision$
******************************************************************************
* Author Name :    Richard Baldock					     *
* Author Login:    richard@hgu.mrc.ac.uk				     *
* Date        :    Mon Sep 19 19:03:52 1994				     *
* Synopsis    :    X interaction routine to modify / define a vertex	     *
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>

#include <HGU_XInteract.h>

static char *help_str = 
"HGU_XGetVtxs: interactively define a number of vertices\n\
    (points) using the mouse or arrow keys. Vertices can be\n\
    defined and deleted before finally returning to the calling\n\
    procedure.\n\
\n\
Define: press and release buttons 1 or 2. Button 2 can be dragged.\n\
    Press spacebar. The position of the current point is the\n\
    position of the cursor which can be moved using the mouse\n\
    or arrow keys. Ctrl<arrow-key> will move the cursor faster.\n\
\n\
Delete: defined vertices are deleted by selecting the vertex then\n\
    pressing delete or backspace. Vertices are selected singly by\n\
    presing button 1 or 2 with the cursor over the vertex. If the\n\
    shift key is pressed then multiple vertices can be selected.\n\
\n\
Confirm: the currently defined vertices will be returned on pressing\n\
    button 3 or return.";


/* local routines */

#define NEW_POINT(X,Y)\
	if( n )\
	    HGU_XDrawCross(dpy, win, gc, x, y);\
	else\
	    n = 1;\
	x = X;\
	y = Y;\
	HGU_XDrawCross(dpy, win, gc, x, y);\
	cb_struct.x = X;\
	cb_struct.y = Y;\
        finished = HGU_XInteractWindowEvent(dpy, win, mode, callbacks,\
					    (caddr_t) &cb_struct);

#define REMOVE_POINT(X,Y)\
	if( n )\
	    HGU_XDrawCross(dpy, win, gc, x, y);\
	n = 0;

#define MOVE_POINTER(DX,DY,MAG) \
            dx = DX;\
            dy = DY;\
            if( MAG ){\
                dx *= 10;\
                dy *= 10;\
	    }\
            switch( n ){\
	    case 0:\
                XWarpPointer( dpy, None, win, 0, 0, 0, 0, x+dx, y+dy );\
                break;\
	    case 1:\
                NEW_POINT(x+dx,y+dy);\
                XWarpPointer( dpy, None, win, 0, 0, 0, 0, x, y );\
                break;\
	    }

typedef struct {
    HGU_XInteractCallback	window_func;
    caddr_t		window_data;
    int			x_centre, y_centre;
    unsigned int	width, height;
} MagVtxWindowData;

static int mag_window_func(
Display			*dpy,
Window			win,
caddr_t			client_data,
caddr_t			call_data)
{
    HGU_XGetVtxCallbackStruct	*cbs = (HGU_XGetVtxCallbackStruct *) call_data;
    MagVtxWindowData		*data = (MagVtxWindowData *) client_data;
    HGU_XGetVtxCallbackStruct	cb_struct;

    cb_struct.x = data->x_centre + (cbs->x - data->width) / 2;
    cb_struct.y = data->y_centre + (cbs->y - data->height) / 2;
    if( data->window_func != NULL )
	return( (*(data->window_func))
	       (dpy, win, data->window_data, (caddr_t) &cb_struct) );
    else
	return( 0 );
}

/* public routines */

WlzFVertex2 *HGU_XGetVtx(
Display			*dpy,
Window			win,
int			mode,
HGU_XInteractCallbacks	*callbacks,
WlzFVertex2		*start)
{
    XEvent		event;
    GC			gc;
    XWindowAttributes	win_att;
    WlzFVertex2		*vtx;
    HGU_XGetVtxCallbackStruct	cb_struct;
    int			n=0, x, y, dx, dy;
    int			finished=0;
    
    /* set up the callback struct */
    cb_struct.event = &event;

    /* get interact gc */
    gc = HGU_XGetGC( dpy, win, HGU_XGC_INTERACT );
    
    /* set the cursor */
    HGU_XSetCursor( dpy, win, HGU_XCURSOR_CROSS );
    
    /* warp cursor to the start point if defined */
    if( start != NULL )
    {
    	x = (int) (start->vtX + 0.5);
    	y = (int) (start->vtY + 0.5);
	XWarpPointer( dpy, None, win, 0, 0, 0, 0, x, y );
	NEW_POINT(x,y)
    }
    
    /* select input and passive button grab */
    if( XGetWindowAttributes(dpy, win, &win_att) == 0 ){
	HGU_XError(dpy, win, "HGU_XGetVtx",
		   "failed to get window attributes", 0);
	return( NULL );
    }
    XSelectInput(dpy, win, win_att.your_event_mask |
		 KeyPressMask | KeyReleaseMask |
		 EnterWindowMask | LeaveWindowMask );
    if( QueryInWindow( dpy, win ) ){
	HGU_XGrabPointer(dpy, win, mode&HGU_XConfineMask);
    }
    
    /* get the new vertex */
    /* wait for finished button (button 3) */
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
	    case Button1:		/* define vertex */
	    case Button2:
		/* check for magnify option */
		if( event.xbutton.state & ShiftMask ){
		    Window 			mag_win;
		    WlzFVertex2	 		mag_vtx;
		    HGU_XInteractCallbacks	mag_callbacks;
		    MagVtxWindowData		mag_window_data;
		    unsigned int		w, h;

		    /* set up start vertex if defined */
		    if( n ){
			mag_vtx.vtX = cb_struct.x;
			mag_vtx.vtY = cb_struct.y;
			vtx = &mag_vtx;
		    } else {
			vtx = NULL;
		    }
		    REMOVE_POINT(x,y);

		    /* get the magnify window */
		    mag_win= HGU_XGetMag2Win
			(dpy, win, event.xbutton.x, event.xbutton.y, &w, &h);

		    /* transform the start vertex */
		    if( vtx != NULL ){
			vtx->vtX = 2 * (vtx->vtX - event.xbutton.x) + w;
			vtx->vtY = 2 * (vtx->vtY - event.xbutton.y) + h;
		    }

		    /* set up the callbacks structure */
		    mag_callbacks = *callbacks;
		    mag_callbacks.window_func = mag_window_func;
		    mag_callbacks.window_data = (caddr_t) &mag_window_data;
		    mag_window_data.x_centre = event.xbutton.x;
		    mag_window_data.y_centre = event.xbutton.y;
		    mag_window_data.width    = w;
		    mag_window_data.height   = h;
		    mag_window_data.window_func = callbacks->window_func;
		    mag_window_data.window_data = callbacks->window_data;

		    /* get a vertex from the mag window */
		    HGU_XUngrabPointer(dpy, win);
		    vtx = HGU_XGetVtx(dpy, mag_win, mode, &mag_callbacks, vtx);
		    (void) HGU_XDestroyWindow(dpy, mag_win);
		    if( QueryInWindow( dpy, win ) ){
			HGU_XGrabPointer(dpy, win, mode&HGU_XConfineMask);
		    }

		    /* clean up and transform returned vertex */
		    if( vtx != NULL ){
			vtx->vtX = event.xbutton.x + (vtx->vtX - w) / 2;
			vtx->vtY = event.xbutton.y + (vtx->vtY - h) / 2;
			NEW_POINT(vtx->vtX, vtx->vtY);
			free( vtx );
		    }
		} else {
		    NEW_POINT(event.xbutton.x,event.xbutton.y);
		}
		break;
	    case Button3:		/* confirm */
		finished = 1;
		break;
	    default:
	        break;
	    }
	    break;
	    
	case ButtonRelease:
	    switch( event.xbutton.button ){
	    case Button1:
	    case Button2:
		finished = mode & HGU_XNoConfirmMask;
		break;
	    default:
	        break;
	    }
	    break;
	    
        case KeyPress:
            switch( XLookupKeysym(&event.xkey, 0) ){
            case XK_Delete:
	    case XK_BackSpace:
                REMOVE_POINT(x,y)
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
		NEW_POINT(event.xkey.x,event.xkey.y);
                break;
	    case XK_Return:
		finished = 1;
		break;
	    case XK_Help:
		HGU_XInteractHelpEvent(dpy, win, callbacks, help_str);
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
	    if( event.xmotion.state & Button2Mask ){
	        NEW_POINT(event.xmotion.x,event.xmotion.y);
	    }
	    else if( n == 0 ){
		x = event.xmotion.x;
		y = event.xmotion.y;
		cb_struct.x = x;
		cb_struct.y = y;
		finished = HGU_XInteractWindowEvent(dpy, win, mode, callbacks,
						    (caddr_t) &cb_struct);
	    }
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
    
    /* ungrab the button */
    HGU_XUngrabPointer(dpy, win);
    
    /* undefine the cross cursor */
    HGU_XSetCursor( dpy, win, HGU_XCURSOR_NONE );
    
    /* reset input mask */
    XSelectInput(dpy, win, win_att.your_event_mask );

    /* make a vertex to return */
    if( n ){
    	REMOVE_POINT(x,y)
	vtx = (WlzFVertex2 *) AlcMalloc(sizeof(WlzFVertex2));
	vtx->vtX = cb_struct.x;
	vtx->vtY = cb_struct.y;
    }
    else
	vtx = NULL;
    
    return( vtx );
}
