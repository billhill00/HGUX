#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _HGU_XGetRect_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         HGU_XGetRect.c
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date		April 2009
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
* \ingroup      HGU_X
* \brief        X interaction routine to modify / define a rectangle
*/

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
#define NEXT_RECT(X,Y) \
    x = X;\
    y = Y;\
    w = x - fixedx;\
    h = y - fixedy;\
    if( mode & HGU_XCentreRectMode ){\
	w *= 2;\
	h *= 2;\
    }\
    if( w > 0 )\
        if( h > 0 )\
            HGU_XSetCursor( dpy, win, HGU_XCURSOR_BOTTOMRIGHTCORNER );\
        else\
            HGU_XSetCursor( dpy, win, HGU_XCURSOR_TOPRIGHTCORNER );\
    else\
        if( h > 0 )\
            HGU_XSetCursor( dpy, win, HGU_XCURSOR_BOTTOMLEFTCORNER );\
        else\
            HGU_XSetCursor( dpy, win, HGU_XCURSOR_TOPLEFTCORNER );\
    cb_struct.x = (w > 0) ? x - w : x;\
    cb_struct.y = (h > 0) ? y - h : y;\
    cb_struct.width = (w > 0) ? w : -w;\
    cb_struct.height = (h > 0) ? h : -h;\
    finished = HGU_XInteractWindowEvent(dpy, win, mode, callbacks,\
					(caddr_t) &cb_struct);\
    HGU_XDrawRectangle( dpy, win, gc, x, y, -w, -h );

#define NEXT_POINT(X,Y) \
            HGU_XDrawRectangle( dpy, win, gc, x, y, -w, -h );\
            switch( n ){\
	    case 0:\
		fixedx = X;\
		fixedy = Y;\
		n = 1;\
		break;\
	    case 2:\
	    case 1:\
		n = 2;\
		break;\
	    }\
            NEXT_RECT(X,Y)

#define REMOVE_POINT(X,Y) \
            switch( n ){\
	    case 1:\
		HGU_XDrawRectangle( dpy, win, gc, x, y, -w, -h );\
                w = h = 0;\
		n = 0;\
	    case 0:\
                if( mode & HGU_XCentreRectMode )\
	            HGU_XSetCursor( dpy, win, HGU_XCURSOR_CROSS );\
                else\
	            HGU_XSetCursor( dpy, win, HGU_XCURSOR_TOPLEFTCORNER );\
		break;\
	    case 2:\
                HGU_XDrawRectangle( dpy, win, gc, x, y, -w, -h );\
		n = 1;\
                NEXT_RECT(X,Y)\
		break;\
	    }

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
                XWarpPointer( dpy, None, win, 0, 0, 0, 0, x+dx, y+dy );\
		break;\
	    case 2:\
                NEXT_POINT(x+dx,y+dy);\
                break;\
	    }

/* public routines */

WlzFVertex2 *HGU_XGetRect(
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
    int			n=0, x=0, y=0, w=0, h=0, dx, dy;
    int			fixedx=0, fixedy=0;
    int			finished=0;
    HGU_XGetRectCallbackStruct	cb_struct;
    
    /* set up the callback struct */
    cb_struct.event = &event;

    /* get interact gc */
    gc = HGU_XGetGC( dpy, win, HGU_XGC_INTERACT );
    
    /* set the cursor */
    if( mode & HGU_XCentreRectMode )
	HGU_XSetCursor( dpy, win, HGU_XCURSOR_CROSS );
    else
	HGU_XSetCursor( dpy, win, HGU_XCURSOR_TOPLEFTCORNER );

    /* warp cursor to the start point if defined */
    if( start != NULL ){
	if( mode & HGU_XCentreRectMode ){
	    fixedx = start[0].vtX + start[1].vtX/2;
	    fixedy = start[0].vtY + start[1].vtY/2;
	} else {
	    fixedx = start[0].vtX;
	    fixedy = start[0].vtY;
	}
	x = start[0].vtX + start[1].vtX;
	y = start[0].vtY + start[1].vtY;
	w = start[1].vtX;
	h = start[1].vtY;
	n = 1;
	XWarpPointer( dpy, None, win, 0, 0, 0, 0, x, y );
    }
    HGU_XDrawRectangle( dpy, win, gc, x, y, -w, -h );
 
    /* select input and passive button grab */
    if( XGetWindowAttributes(dpy, win, &win_att) == 0 ){
	HGU_XError(dpy, win, "HGU_XGetRect",
		   "failed to get window attributes", 0);
	return( NULL );
    }
    XSelectInput(dpy, win, win_att.your_event_mask |
		 KeyPressMask | KeyReleaseMask |
		 EnterWindowMask | LeaveWindowMask );
    if( QueryInWindow( dpy, win ) ){
	HGU_XGrabPointer(dpy, win, mode&HGU_XConfineMask);
    }
    
    /* get the new position */
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
	    case Button1:		/* define point */
	    case Button2:
		NEXT_POINT(event.xbutton.x,event.xbutton.y);
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
                REMOVE_POINT(event.xkey.x,event.xkey.y);
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
		NEXT_POINT(event.xkey.x,event.xkey.y);
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
		NEXT_POINT(event.xmotion.x,event.xmotion.y);
	    }
	    else if( n == 1 ){
		HGU_XDrawRectangle( dpy, win, gc, x, y, -w, -h );
		NEXT_RECT(event.xmotion.x,event.xmotion.y);
	    }
	    else if( n == 0 ){
		NEXT_POINT(event.xmotion.x,event.xmotion.y);
		REMOVE_POINT(event.xmotion.x,event.xmotion.y);
	    }
	    break;
	    
	case EnterNotify:
	  finished = HGU_XInteractWindowEvent(dpy, win, mode, callbacks,
					      (caddr_t) &cb_struct);
	  HGU_XGrabPointer(dpy, win, mode&HGU_XConfineMask);
	  break;
	    
	case LeaveNotify:
	  finished = HGU_XInteractWindowEvent(dpy, win, mode, callbacks,
					      (caddr_t) &cb_struct);
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
    switch( n ){
    case 2:
    case 1:
	HGU_XDrawRectangle( dpy, win, gc, x, y, -w, -h );
	vtx = (WlzFVertex2 *) AlcMalloc(sizeof(WlzFVertex2) * 2);
	vtx[0].vtX = (w < 0) ? x : x - w;
	vtx[0].vtY = (h < 0) ? y : y - h;
	vtx[1].vtX = (w < 0) ? -w : w;
	vtx[1].vtY = (h < 0) ? -h : h;
	break;
    case 0:
    default:
	vtx = NULL;
	break;
    }
    
    return( vtx );
}
