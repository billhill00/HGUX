#if defined(__GNUC__)
#ident "MRC HGU $Id:"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id:"
#else static char _HGU_XGetVtxs_c[] = "MRC HGU $Id:";
#endif
#endif
/*!
* \file         HGU_XGetVtxs.c
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Wed Apr 29 08:20:54 2009
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
* \ingroup      HGU_X
* \brief         X interaction routine to modify / define a vertex list
*               
* \todo         -
* \bug          None known
*
* Maintenance log with most recent changes at top of list.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
static int get_vtx_index(
WlzFVertex2 	*vtxs,
int		nvtxs,
WlzFVertex2	*vtx)
{
    int		i;

    if( vtxs == NULL )
	return( -1 );

    for(i=0; i < nvtxs; i++){
	if(fabs(vtxs[i].vtX - vtx->vtX) < 3.0 &&
	   fabs(vtxs[i].vtY - vtx->vtY) < 3.0)
	    return( i );
    }
    return( -1 );
}

#define CLEAR_SELECTED_VTXS() \
for(clr_vtx_i=0; clr_vtx_i < nvtxs_sel; clr_vtx_i++){\
    HGU_XDrawCross(dpy, win, gc_sel, (int) (vtxs_sel[clr_vtx_i].vtX),\
                    (int) (vtxs_sel[clr_vtx_i].vtY));\
}\
nvtxs_sel = 0;

#define REMOVE_SELECTED_VTXS() \
for(rem_vtx_i=0; rem_vtx_i < nvtxs_sel; rem_vtx_i++){\
    int j = get_vtx_index(vtxs, nvtxs, &vtxs_sel[rem_vtx_i]);\
    if( j >= 0 ){\
        HGU_XDrawCross(dpy, win, gc, (int) (vtxs[j].vtX),\
		       (int) (vtxs[j].vtY));\
        for(;j < nvtxs-1; j++)\
            vtxs[j] = vtxs[j+1];\
        nvtxs--;\
    }\
}\
CLEAR_SELECTED_VTXS()

#define SELECT_VTX(VTX) \
if( (sel_vtx_i = get_vtx_index(vtxs_sel, nvtxs_sel, VTX)) < 0 ){\
    if( (sel_vtx_i = get_vtx_index(vtxs, nvtxs, VTX)) >= 0 ){\
        if( nvtxs_sel >= max_nvtxs_sel ){\
	    max_nvtxs_sel += 32;\
	    vtxs_sel = (WlzFVertex2 *)\
                AlcRealloc(vtxs_sel, sizeof(WlzFVertex2)*max_nvtxs_sel);\
        }\
        vtxs_sel[nvtxs_sel].vtX = vtxs[sel_vtx_i].vtX;\
        vtxs_sel[nvtxs_sel].vtY = vtxs[sel_vtx_i].vtY;\
        HGU_XDrawCross(dpy, win, gc_sel, (int) (vtxs_sel[nvtxs_sel].vtX),\
	               (int) (vtxs_sel[nvtxs_sel].vtY));\
        nvtxs_sel++;\
    }\
}


#define NEW_VTX(VTX) \
if( (new_vtx_i = get_vtx_index(vtxs, nvtxs, VTX)) >= 0 ){\
    SELECT_VTX((&(vtxs[new_vtx_i])))\
} else {\
    if( nvtxs >= max_nvtxs ){\
	max_nvtxs += 32;\
	vtxs = (WlzFVertex2 *)\
            AlcRealloc(vtxs, sizeof(WlzFVertex2)*max_nvtxs);\
    }\
    vtxs[nvtxs].vtX = VTX->vtX;\
    vtxs[nvtxs].vtY = VTX->vtY;\
    HGU_XDrawCross(dpy, win, gc, (int) (vtxs[nvtxs].vtX),\
		   (int) (vtxs[nvtxs].vtY));\
    nvtxs++;\
}

#define MOVE_POINTER(DX,DY,MAG) \
dx = DX;\
dy = DY;\
if( MAG ){\
    dx *= 10;\
    dy *= 10;\
}\
XWarpPointer( dpy, None, win, 0, 0, 0, 0, x+dx, y+dy );

typedef struct {
    HGU_XInteractCallback	window_func;
    caddr_t			window_data;
    HGU_XGetVtxsCallbackStruct	*cb_struct;
} GetVtxWindowData;

int getvtx_window_func(
Display		*dpy,
Window		win,
caddr_t		client_data,
caddr_t		call_data)
{
    HGU_XGetVtxCallbackStruct	*cbs = (HGU_XGetVtxCallbackStruct *) call_data;
    GetVtxWindowData	*getvtx_window_data = (GetVtxWindowData *) client_data;
    HGU_XGetVtxsCallbackStruct	*cb_struct = getvtx_window_data->cb_struct;

    cb_struct->x = cbs->x;
    cb_struct->y = cbs->y;
    if( getvtx_window_data->window_func != NULL )
	return( (*(getvtx_window_data->window_func))
	       (dpy, win, getvtx_window_data->window_data,
		(caddr_t) cb_struct) );
    else
	return( 0 );
}
    

/* public routines */

int HGU_XGetVtxs(
Display			*dpy,
Window			win,
int			mode,
HGU_XInteractCallbacks	*callbacks,
WlzFVertex2		*start,
int			n_start,
WlzFVertex2		**return_vtxs)
{
    XEvent		event;
    GC			gc, gc_sel;
    XWindowAttributes	win_att;
    WlzFVertex2		*vtx, vtx1;
    WlzFVertex2		*vtxs=NULL, *vtxs_sel=NULL;
    int			nvtxs=0, max_nvtxs=0;
    int			nvtxs_sel=0, max_nvtxs_sel=0;
    HGU_XInteractCallbacks	getvtx_cbcks;
    HGU_XGetVtxsCallbackStruct	cb_struct;
    GetVtxWindowData		getvtx_window_data;	
    int			new_vtx_i, sel_vtx_i, rem_vtx_i, clr_vtx_i, i;
    int			finished=0;
    int			x=0, y=0, dx, dy;
    
    /* set up the callback struct and HGU_XGetVtx callbacks */
    cb_struct.event = &event;
    getvtx_cbcks = *callbacks;
    getvtx_cbcks.window_func = getvtx_window_func;
    getvtx_cbcks.window_data = (caddr_t) &getvtx_window_data;
    getvtx_window_data.window_func = callbacks->window_func;
    getvtx_window_data.window_data = callbacks->window_data;
    getvtx_window_data.cb_struct = &cb_struct;

    /* get interact gc */
    gc     = HGU_XGetGC( dpy, win, HGU_XGC_INTERACT );
    gc_sel = HGU_XGetGC( dpy, win, HGU_XGC_HIGHLIGHT );
    
    /* set the cursor */
    HGU_XSetCursor( dpy, win, HGU_XCURSOR_CROSS );
    
    /* warp cursor to the start point if defined */
    while( n_start > 0 ){
	n_start--;
	NEW_VTX((&(start[n_start])));
    }
    
    /* select input and passive button grab */
    if( XGetWindowAttributes(dpy, win, &win_att) == 0 ){
	HGU_XError(dpy, win, "HGU_XGetVtxs",
		   "failed to get window attributes", 0);
	return 0;
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
	    case Button1:		/* define/select vertex */
	    case Button2:
		x = event.xbutton.x;
		y = event.xbutton.y;
		vtx1.vtX = x;
		vtx1.vtY = y;

		if( event.xbutton.state & ShiftMask ){
		    SELECT_VTX((&vtx1));
		} else {
		    CLEAR_SELECTED_VTXS();
		    cb_struct.vtxs = vtxs;
		    cb_struct.nvtxs = nvtxs;
		    if( (vtx = HGU_XGetVtx(dpy, win, mode|HGU_XNoConfirmMask,
					   &getvtx_cbcks, &vtx1)) == NULL )
			break;
		    HGU_XGrabPointer(dpy, win, mode&HGU_XConfineMask);
		    NEW_VTX(vtx);
		    x = vtx->vtX;
		    y = vtx->vtY;
		    free( vtx );
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
		REMOVE_SELECTED_VTXS();
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
		vtx1.vtX = event.xkey.x;
		vtx1.vtY = event.xkey.y;
		if( event.xkey.state&ShiftMask ){
		    SELECT_VTX((&vtx1));
		} else {
		    CLEAR_SELECTED_VTXS();
		    NEW_VTX((&vtx1));
		}
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
	    x = event.xmotion.x;
	    y = event.xmotion.y;
	    cb_struct.x = x;
	    cb_struct.y = y;
	    cb_struct.vtxs = vtxs;
	    cb_struct.nvtxs = nvtxs;
	    finished = HGU_XInteractWindowEvent(dpy, win, mode, callbacks,
						(caddr_t) &cb_struct);
	    if( event.xmotion.state & (Button1Mask|Button2Mask) ){
		for(i=0; i < nvtxs; i++){
		    if( (int) vtxs[i].vtY == y && (int) vtxs[i].vtX <= x ){
			SELECT_VTX((&(vtxs[i])));
		    }
		}
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
    
    /* remove displayed vertices */
    for(i=0; i < nvtxs; i++)
	HGU_XDrawCross(dpy, win, gc, (int) (vtxs[i].vtX), (int) (vtxs[i].vtY));
    CLEAR_SELECTED_VTXS();
    if( vtxs_sel != NULL ){
	free( vtxs_sel );
    }

    /* ungrab the button */
    HGU_XUngrabPointer(dpy, win);
    
    /* undefine the cross cursor */
    HGU_XSetCursor( dpy, win, HGU_XCURSOR_NONE );
    
    /* reset input mask */
    XSelectInput(dpy, win, win_att.your_event_mask );

    /* make a vertex array to return */
    if( vtxs == NULL )
	return( 0 );

    *return_vtxs = (WlzFVertex2 *)
	AlcRealloc(vtxs, sizeof(WlzFVertex2)*nvtxs);
    return( nvtxs );
}
