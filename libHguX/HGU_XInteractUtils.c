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
* File        :    HGU_XInteractUtils.c					     *
* $Revision$
******************************************************************************
* Author Name :    Richard Baldock					     *
* Author Login:    richard@hgu.mrc.ac.uk				     *
* Date        :    Mon Sep 19 17:46:46 1994				     *
* Synopsis    :    X interaction utility routines			     *
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#ifndef __EXTENSIONS__
#define __EXTENSIONS__
#endif

#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <memory.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>

#include <HGU_XInteract.h>
#include <HGU_XCursors.h>

/* timer routines and variables */
struct itimerval	L_value, S_value, DC_value;
int 			HGU_XTimerExpired;

static struct itimerval	value;

static void HGU_XSetExpiredFlag(
int val)
{
    if( val ){ /* to satisfy the IRIX646 compiler */ }

    HGU_XTimerExpired = 1;
    return;
}

int HGU_XSetUpTimers()
{
        /* set up the interval timer structures */
        L_value.it_interval.tv_sec = 0;
        L_value.it_value.tv_sec = 0;
        L_value.it_interval.tv_usec = 0;
        L_value.it_value.tv_usec = HGU_XLONGDELAY;
        S_value.it_interval.tv_sec = 0;
        S_value.it_value.tv_sec = 0;
        S_value.it_interval.tv_usec = 0;
        S_value.it_value.tv_usec = HGU_XSHORTDELAY;
        DC_value.it_interval.tv_sec = 0;
        DC_value.it_value.tv_sec = 0;
        DC_value.it_interval.tv_usec = 0;
        DC_value.it_value.tv_usec = HGU_XDOUBLECLICKDELAY;

        /* catch return signal from the timer */
        signal(SIGALRM, HGU_XSetExpiredFlag);

	HGU_XTimerExpired = 0;
	return( 0 );
}

int HGU_XSetTimer(
struct itimerval *val)
{
	setitimer(ITIMER_REAL, val, &value);
	HGU_XTimerExpired = 0;
	return( 0 );
}
int HGU_XSetWindowColormap(
Display		*dpy,
Window		win,
Colormap	cmap)
{
	unsigned int		nchildren;
	Window			root, parent, *children;

	/* set the new colormap */
	XSetWindowColormap(dpy, win, cmap);

	/* get the parent window */
	XQueryTree(dpy, win, &root, &parent, &children, &nchildren);
	XFree( children );

	/* set parent colormap unless root */
	if( parent != root )
		HGU_XSetWindowColormap(dpy, parent, cmap);

	return( 0 );
}

int HGU_XDestroyWindow(
Display		*dpy,
Window		win)
{
	unsigned int		nchildren;
	Window			root, parent, *children;
	XEvent			event;

	/* get the parent window */
	XQueryTree(dpy, win, &root, &parent, &children, &nchildren);
	XFree( children );

	/* clear any pending events before destroying window */
	XSync(dpy, False);
	while( XCheckWindowEvent(dpy, win, ~0x0, &event) == True )
	    continue;

	/* destroy parent unless root */
	if( parent != root )
		HGU_XDestroyWindow(dpy, parent);
	else
		XDestroyWindow(dpy, win);

	return( 0 );
}

int HGU_XIsAscendent(
    Display	*dpy,
    Window	win,
    Window	testwin)
{
    unsigned int	nchildren;
    Window		root, parent, *children;

    /* get the parent window */
    XQueryTree(dpy, win, &root, &parent, &children, &nchildren);
    XFree( children );

    if( parent == testwin ){
	return( 1 );
    }

    if( parent == root ){
	return( 0 );
    }

    return( HGU_XIsAscendent(dpy, parent, testwin) );
}

int HGU_XCheckCloseEvent(
    Display	*dpy,
    Window	win,
    XEvent	*event)
{
    Atom	property, type;

    property = XInternAtom(dpy, "WM_PROTOCOLS", True);
    type = XInternAtom(dpy, "WM_DELETE_WINDOW", True);

    switch( event->type ){
    case ClientMessage:
	if( (event->xclient.message_type == property) &&
	    ((Atom) event->xclient.data.l[0] == type) ){
	    if( (event->xclient.window == win) ||
		HGU_XIsAscendent(dpy, win, event->xclient.window) ){
		return( 1 );
	    }
	}
	break;
    default:
	break;
    }

    return( 0 );
}

int HGU_XGrabButton(
Display		*dpy,
Window		win,
int		confine)
{
    int		grab_result;

    if( confine )
	return( HGU_XGrabPointer(dpy, win, confine) );
    
    grab_result = XGrabButton(dpy, AnyButton, AnyModifier,
			      win, True, ButtonPressMask|
			      ButtonReleaseMask|ButtonMotionMask|
			      PointerMotionMask|
			      EnterWindowMask|LeaveWindowMask,
			      GrabModeAsync, GrabModeAsync,
			      (confine ? win : None), None);
    XFlush( dpy );
    return( grab_result );
}

int HGU_XUngrabButton(
Display		*dpy,
Window		win)
{
	XUngrabButton(dpy, AnyButton, AnyModifier, win);
	XFlush( dpy );
	return( 0 );
}

int HGU_XGrabPointer(
Display		*dpy,
Window		win,
int		confine)
{
	int grab_result;

	grab_result = XGrabPointer(dpy, win, True, ButtonPressMask|
		ButtonReleaseMask|ButtonMotionMask|
		PointerMotionMask|
		EnterWindowMask|LeaveWindowMask,
		GrabModeAsync, GrabModeAsync,
		(confine ? win : None), None, CurrentTime);

	XFlush( dpy );
	return( grab_result );
}

int HGU_XUngrabPointer(
Display		*dpy,
Window		win)
{
    if( win ){ /* to satify the IRIX646 compilre */ }

    XUngrabPointer(dpy, CurrentTime);
    XFlush( dpy );
    return( 0 );
}

int HGU_XDrawCross(
Display 	*dpy,
Window  	win,
GC      	gc,
int		x,
int		y)
{
    XDrawLine(dpy, win, gc, x+5, y, x+9, y);
    XDrawLine(dpy, win, gc, x-5, y, x-9, y);
    XDrawLine(dpy, win, gc, x, y+5, x, y+9);
    XDrawLine(dpy, win, gc, x, y-5, x, y-9);
    XDrawArc(dpy, win, gc, x-4, y-4, 8, 8, 0, 360*64);
    XFlush( dpy );
    return( 0 );
}

int HGU_XDrawRectangle(
Display		*dpy,
Window		win,
GC		gc,
int		x,
int		y,
int		w,
int		h)
{
    if( w < 0 ){
	x += w;
	w *= -1;
    }

    if( h < 0 ){
	y += h;
	h *= -1;
    }

    (void) XDrawRectangle( dpy, win, gc, x, y, w, h );
    (void) XFlush( dpy );
    return( 0 );
}

/* static variables for efficiency */
static Cursor		cursors[HGU_XCURSOR_LAST];
static int		cursors_initialised = 0;

static Cursor create_bits_cursor(
Display		*dpy,
Window		win,
unsigned char	*bits,
unsigned char	*Bits,
int		w,
int		h,
int		x,
int		y)
{
    Pixmap		source, mask;
    XColor		fg, bckg;

    source = XCreatePixmapFromBitmapData(dpy, win, (char *) bits,
					 w, h, 1, 0, 1);
    mask   = XCreatePixmapFromBitmapData(dpy, win, (char *) Bits,
					 w, h, 1, 0, 1);

    fg.pixel = 1;
    bckg.pixel = 0;
    fg.red = fg.green = fg.blue = 0;
    bckg.red = bckg.green = bckg.blue = 255*256;
    fg.flags = bckg.flags = DoRed|DoGreen|DoBlue;

    return( XCreatePixmapCursor(dpy, source, mask, &fg, &bckg, x, y) );
}

Cursor HGU_XCreateObjectCursor(
Display		*dpy,
Window		win,
WlzObject	*obj,
int		border)
{
    Pixmap		source, mask;
    unsigned int	width, height, rw, rh;
    WlzObject		*obj1;
    XGCValues		gcvalues;
    WlzIntervalWSpace	iwsp;
    XColor		fg, bckg;
    GC			gc;
    Cursor		cursor;

    if( obj == NULL || obj->type != WLZ_2D_DOMAINOBJ)
	return( None );

    width = obj->domain.i->lastkl - obj->domain.i->kol1 + 3;
    height = obj->domain.i->lastln - obj->domain.i->line1 + 3;
    if( !XQueryBestCursor(dpy, win, width, height, &rw, &rh) ){
	XBell(dpy, 100);
	HGU_XError(dpy, win, "HGU_XCreateObjectCursor",
		   "invalid object size - nearest used", 0);
	width = rw;
	height = rh;
    }

    source = XCreatePixmap( dpy, win, width, height, 1 );
    mask   = XCreatePixmap( dpy, win, width, height, 1 );

    gc = XCreateGC(dpy, source, 0, &gcvalues);
    XFillRectangle(dpy, source, gc, 0, 0, width, height);
    XFillRectangle(dpy, mask, gc, 0, 0, width, height);
    XSetForeground(dpy, gc, 1);
    XSetBackground(dpy, gc, 0);

    WlzInitRasterScan(obj, &iwsp, WLZ_RASTERDIR_ILIC);
    while( WlzNextInterval(&iwsp) == WLZ_ERR_NONE ){
	XDrawLine(dpy, source, gc, iwsp.lftpos - iwsp.intdmn->kol1 + 1,
		  iwsp.linpos - iwsp.intdmn->line1 + 1,
		  iwsp.rgtpos - iwsp.intdmn->kol1 + 1,
		  iwsp.linpos - iwsp.intdmn->line1 + 1);
    }

    obj1 = WlzDilation( obj, WLZ_8_CONNECTED, NULL );
    WlzInitRasterScan(obj1, &iwsp, WLZ_RASTERDIR_ILIC);
    while( WlzNextInterval(&iwsp) == WLZ_ERR_NONE ){
	XDrawLine(dpy, mask, gc, iwsp.lftpos - iwsp.intdmn->kol1,
		  iwsp.linpos - iwsp.intdmn->line1,
		  iwsp.rgtpos - iwsp.intdmn->kol1,
		  iwsp.linpos - iwsp.intdmn->line1);
    }

    /* create the cursor */
    fg.pixel = 1;
    bckg.pixel = 0;
    fg.red = fg.green = fg.blue = 0;
    bckg.red = bckg.green = bckg.blue = 255*256;
    fg.flags = bckg.flags = DoRed|DoGreen|DoBlue;

    if( border )
	cursor = XCreatePixmapCursor(dpy, source, mask, &fg, &bckg,
				     width/2 + 1, height/2 +1);
    else
	cursor = XCreatePixmapCursor(dpy, source, source, &fg, &bckg,
				     width/2 + 1, height/2 +1);

    WlzFreeObj( obj1 );
    XFreeGC( dpy, gc );
    XFreePixmap( dpy, source );
    XFreePixmap( dpy, mask );
    return( cursor );
}


void HGU_XSetCursor(
Display		*dpy,
Window		win,
HGU_XCursorType	cursor_type)
{
    /* check initialisation */
    if( !cursors_initialised ){
	int	i;
	for(i=0; i < (int) HGU_XCURSOR_LAST; i++)
	    cursors[i] = (Cursor) 0;
	cursors_initialised = 1;
    }

    if( cursors[(int) cursor_type] ){
	XDefineCursor( dpy, win, cursors[(int) cursor_type] );
	return;
    }

    /* create cursor if necessary */
    switch( cursor_type ){
    case HGU_XCURSOR_NONE:
    case HGU_XCURSOR_DEFAULT:
	XUndefineCursor( dpy, win );
	return;
    case HGU_XCURSOR_POINTER:
	cursors[(int) cursor_type] =
	    XCreateFontCursor(dpy, XC_top_left_arrow);
	break;
    case HGU_XCURSOR_TOPLEFTCORNER:
	cursors[(int) cursor_type] =
	    XCreateFontCursor(dpy, XC_top_left_corner);
	break;
    case HGU_XCURSOR_TOPRIGHTCORNER:
	cursors[(int) cursor_type] =
	    XCreateFontCursor(dpy, XC_top_right_corner);
	break;
    case HGU_XCURSOR_BOTTOMLEFTCORNER:
	cursors[(int) cursor_type] =
	    XCreateFontCursor(dpy, XC_bottom_left_corner);
	break;
    case HGU_XCURSOR_BOTTOMRIGHTCORNER:
	cursors[(int) cursor_type] =
	    XCreateFontCursor(dpy, XC_bottom_right_corner);
	break;
    case HGU_XCURSOR_CROSS:
	cursors[(int) cursor_type] =
	    create_bits_cursor(dpy, win,
			       cross_cursor_bits, Cross_Cursor_bits,
			       cross_cursor_width, cross_cursor_height,
			       cross_cursor_x_hot, cross_cursor_y_hot);
	break;
    case HGU_XCURSOR_TRANSLATE:
	cursors[(int) cursor_type] =
	    create_bits_cursor(dpy, win, 
			       translate_cursor_bits, Translate_Cursor_bits,
			       translate_cursor_width, translate_cursor_height,
			       translate_cursor_x_hot, translate_cursor_y_hot);
	break;
    case HGU_XCURSOR_SCALE:
	cursors[(int) cursor_type] =
	    create_bits_cursor(dpy, win,
			       scale_cursor_bits, Scale_Cursor_bits,
			       scale_cursor_width, scale_cursor_height,
			       scale_cursor_x_hot, scale_cursor_y_hot);
	break;
    case HGU_XCURSOR_ROTATE:
	cursors[(int) cursor_type] =
	    create_bits_cursor(dpy, win,
			       rotate_cursor_bits, Rotate_Cursor_bits,
			       rotate_cursor_width, rotate_cursor_height,
			       rotate_cursor_x_hot, rotate_cursor_y_hot);
	break;
    case HGU_XCURSOR_SHEAR:
	cursors[(int) cursor_type] =
	    create_bits_cursor(dpy, win, 
			       shear_cursor_bits, Shear_Cursor_bits,
			       shear_cursor_width, shear_cursor_height,
			       shear_cursor_x_hot, shear_cursor_y_hot);
	break;
    case HGU_XCURSOR_BLOB:
	cursors[(int) cursor_type] =
	    create_bits_cursor(dpy, win,
			       blob_cursor_bits, Blob_Cursor_bits,
			       blob_cursor_width, blob_cursor_height,
			       blob_cursor_x_hot, blob_cursor_y_hot);
	break;
    case HGU_XCURSOR_WAITING:
	cursors[(int) cursor_type] =
	    XCreateFontCursor(dpy, XC_watch);
	break;
    case HGU_XCURSOR_POINT:
	cursors[(int) cursor_type] =
	    create_bits_cursor(dpy, win,
			       point_cursor_bits, Point_Cursor_bits,
			       point_cursor_width, point_cursor_height,
			       point_cursor_x_hot, point_cursor_y_hot);
	break;
    case HGU_XCURSOR_OBJECT:
    case HGU_XCURSOR_LAST:
    default:
	XUndefineCursor( dpy, win );
	return;
    }

    XDefineCursor( dpy, win, cursors[(int) cursor_type] );
    return;
}

static GC		gcs[(int) HGU_XGC_LAST];
static int		gcs_initialised=0;

GC HGU_XGetGC(
Display		*dpy,
Window		win,
HGU_XGCType	gc_type)
{
    XGCValues        	gcvalues;
    unsigned long	mask;

    /* check initialisation */
    if( !gcs_initialised ){
	int	i;
	for(i=0; i < (int) HGU_XGC_LAST; i++)
	    gcs[i] = (GC) NULL;
	gcs_initialised = 1;
    }

    if( gcs[(int) gc_type] != NULL )
	return( gcs[(int) gc_type] );

    /* create gc */
    switch( gc_type ){
    case HGU_XGC_INTERACT:
        gcvalues.function       = GXxor;
        gcvalues.foreground     = (unsigned long) 0277;
        gcvalues.background     = (unsigned long) 0;
        gcvalues.plane_mask     = (unsigned long) 255;
        gcvalues.subwindow_mode = IncludeInferiors;
	mask = (GCFunction | GCPlaneMask | GCSubwindowMode |
		GCForeground|GCBackground);
	break;
    case HGU_XGC_CONFIRMED:
        gcvalues.function       = GXxor;
        gcvalues.foreground     = (unsigned long) 0123;
        gcvalues.background     = (unsigned long) 0;
        gcvalues.plane_mask     = (unsigned long) 255;
        gcvalues.subwindow_mode = IncludeInferiors;
	gcvalues.line_width	= 1;
	gcvalues.join_style	= JoinMiter;
	mask = (GCFunction | GCPlaneMask | GCSubwindowMode |
		GCForeground | GCBackground | GCLineWidth | GCJoinStyle);
	break;
    case HGU_XGC_PLANE:
        gcvalues.function       = GXcopy;
	gcvalues.foreground	= (unsigned long) 1;
	gcvalues.background	= (unsigned long) 0;
	gcvalues.plane_mask	= (unsigned long) 1;
	mask = (GCFunction | GCForeground | GCBackground | GCPlaneMask);
	break;
    case HGU_XGC_DRAW:
        gcvalues.function       = GXxor;
	gcvalues.foreground	= (unsigned long) 1;
	gcvalues.plane_mask	= (unsigned long) 1;
	mask = (GCFunction | GCForeground | GCPlaneMask);
	break;
    case HGU_XGC_DELETE:
        gcvalues.function       = GXclear;
	gcvalues.plane_mask	= 0x1;
	mask = (GCFunction | GCPlaneMask);
	break;
    case HGU_XGC_COPY:
        gcvalues.foreground	= (unsigned long) 0;
        gcvalues.background	= (unsigned long) 1;
	mask = (GCForeground | GCBackground);
	break;
    case HGU_XGC_HIGHLIGHT:
        gcvalues.function       = GXxor;
        gcvalues.foreground     = (unsigned long) 0277;
        gcvalues.background     = (unsigned long) 0;
        gcvalues.plane_mask     = (unsigned long) 255;
	gcvalues.line_width	= 3;
        gcvalues.subwindow_mode = IncludeInferiors;
	mask = (GCFunction | GCPlaneMask | GCSubwindowMode |
		GCForeground| GCBackground | GCLineWidth);
	break;
    case HGU_XGC_LAST:
    default:
	return( (GC) NULL );
    }

    gcs[(int) gc_type] = XCreateGC(dpy, win, mask, &gcvalues);
    return( gcs[(int) gc_type] );
}

void HGU_XSetWindowGroup(
Display		*dpy,
Window		win,
Window		parent)
{
    unsigned int	nchildren;
    Window		root, top_parent, *children;
    XWMHints		*xwm_hints;

    /* set the WMHints */
    if( (xwm_hints = XGetWMHints( dpy, parent )) != NULL ){
	xwm_hints->flags = WindowGroupHint;
	XSetWMProperties(dpy, win, NULL, NULL, NULL, 0, NULL, xwm_hints, NULL);
	XFree( xwm_hints );
	return;
    }

    /* get the top-level window of the parent window */
    XQueryTree(dpy, parent, &root, &top_parent, &children, &nchildren);
    XFree( children );

    if( top_parent != root ){
	HGU_XSetWindowGroup(dpy, win, top_parent);
    }

    return;
}

void HGU_XSetTransientForHint(
Display		*dpy,
Window		win,
Window		parent)
{
    unsigned int	nchildren;
    Window		root, top_parent, *children;

    /* get the top-level window of the parent window */
    XQueryTree(dpy, parent, &root, &top_parent, &children, &nchildren);
    XFree( children );

    if( top_parent != root ){
	HGU_XSetTransientForHint(dpy, win, top_parent);
    } else {
	XSetTransientForHint(dpy, win, parent);
    }

    return;
}

void HGU_XSetWMDeleteProperty(
    Display	*dpy,
    Window	win)
{
    Atom	property, type;

    property = XInternAtom(dpy, "WM_PROTOCOLS", False);
    type = XInternAtom(dpy, "WM_DELETE_WINDOW", False);

    XChangeProperty(dpy, win, property, property, 32, PropModeAppend,
		    (unsigned char *) &type, 1);
    return;
}

Window HGU_XCreateMagWin(
Display		*dpy,
Window		parent,
int		x,
int		y,
int		xsize,
int		ysize,
int		mag_factor,
int		interp)
{
    Window			win, top_win;
    Pixmap			pixmap;
    XImage			*image1, *image2;
    XWindowAttributes		win_att;
    XSetWindowAttributes	set_win_att;
    int				X, Y, i, j, k, k1, k2;
    char			*data1, *data2;
    XSizeHints			xhints;
    XClassHint			xclasshint;
    GC				mag_gc;
    XEvent			event_report;

    if( interp ){ /* not used at present - referenced for IRIX646 */ }
    
    /* get attributes of the parent */
    if( XGetWindowAttributes(dpy, parent, &win_att) == 0 ){
	HGU_XError(dpy, parent, "HGU_XCreateMagWin",
		   "failed to create magnify window", 0);
	return( None );
    }
    
    /* check the size - if necessary set to minimum = 16 */
    xsize = (xsize < 16) ? 16 : xsize;
    ysize = (ysize < 16) ? 16 : ysize;

    /* set up a copy gc */
    mag_gc = HGU_XGetGC( dpy, parent, HGU_XGC_COPY );
    
    /* copy values from the parent */
    pixmap = XCreatePixmap(dpy, parent, xsize, ysize,
			   win_att.depth);
    XFillRectangle(dpy, pixmap, mag_gc, 0, 0, xsize, ysize);
    XCopyArea(dpy, parent, pixmap, mag_gc, x - xsize/2, y - ysize/2,
	      xsize, ysize, 0, 0);
    
    /* create the new window - calculate placement */
    QueryRootPosition(dpy, parent, &X, &Y);
    X += x - xsize/2*mag_factor + MAGOFFSET;
    if( (X + mag_factor*xsize) > WidthOfScreen(win_att.screen) )
	X = WidthOfScreen(win_att.screen) - mag_factor*xsize;
    X = (X < 0) ? 0 : X;
    
    Y += y - ysize/2*mag_factor + MAGOFFSET;
    if( (Y + mag_factor*ysize) > HeightOfScreen(win_att.screen) )
	Y = HeightOfScreen(win_att.screen) - mag_factor*ysize;
    Y = (Y < 0) ? 0 : Y;
    
    top_win =  XCreateWindow(dpy, win_att.root, X, Y,
			     xsize*mag_factor, ysize*mag_factor,
			     0, win_att.depth, InputOutput, win_att.visual,
			     0, &set_win_att);
    XSetWindowColormap(dpy, top_win, win_att.colormap);
    HGU_XSetTransientForHint(dpy, top_win, parent);
    HGU_XSetWMDeleteProperty(dpy, top_win);

    /* various hints for the window manager */
    xhints.flags = PMinSize | PMaxSize;
    xhints.min_width = xhints.max_width = xsize*mag_factor;
    xhints.min_height = xhints.max_height = ysize*mag_factor;
    XSetWMNormalHints(dpy, top_win, &xhints);
    
    xclasshint.res_name = "HGU_xmag";
    xclasshint.res_class = "HGU_Xmag";
    XSetClassHint( dpy, top_win, &xclasshint );

    set_win_att.do_not_propagate_mask = ButtonPressMask|ButtonReleaseMask|
	ButtonMotionMask | PointerMotionMask;
    set_win_att.event_mask = ExposureMask;
    set_win_att.backing_store = Always;
    set_win_att.colormap = win_att.colormap;
    
    win = XCreateWindow(dpy, top_win, 0, 0,
			xsize*mag_factor, ysize*mag_factor,
			0, CopyFromParent, CopyFromParent, CopyFromParent,
			CWBackingStore | CWDontPropagate | CWColormap |
			CWEventMask,
			&set_win_att);
    
    /* map the new window */
    HGU_XSetCursor(dpy, top_win, HGU_XCURSOR_POINTER);
    XMapWindow(dpy, top_win);
    XMapWindow(dpy, win);

    /* check for exposure event */
    XFlush(dpy);
    while( !XCheckWindowEvent(dpy, win, ExposureMask, &event_report) ){
      XSync(dpy, False);
    }
    
    /* expand values without interpolation */
    /* use many counters for efficiency to save recalculation */
    if( win_att.depth != 8 ){
	for(i=0, k1=0; i < ysize; i++ ){
	    for(j=0, k2=0; j < xsize; j++ ){
		for(k=0; k < mag_factor; k++, k2++){
		    XCopyArea(dpy,pixmap,win,mag_gc,j,i,1,1,k2,k1);
		}
	    }
	    for(k=1, k1++; k < mag_factor; k++, k1++){
		XCopyArea(dpy,win,win,mag_gc,0,k1-1,xsize*mag_factor,1,0,k1);
	    }
	}
    } else {
	int	factor = xsize*mag_factor;
	image1 = XGetImage(dpy, pixmap, 0, 0, xsize, ysize,
			   (unsigned long) 255, ZPixmap);
	data2 = (char *) malloc(sizeof(char)*xsize*ysize*
				mag_factor*mag_factor);
	image2 = XCreateImage(dpy, win_att.visual, 8, ZPixmap, 0, data2,
			      xsize*mag_factor, ysize*mag_factor, 8, 0);
	
	for( i=0; i < ysize; i++ ){
	    data1 = image1->data + image1->xoffset + i*image1->bytes_per_line;
	    for( j=0, k1=0; j < xsize; j++){
		for(k=0; k < mag_factor; k++, k1++)
		    data2[k1] = data1[j];
	    }
	    data2 += factor;
	    for( k=1; k < mag_factor; k++ ){
		(void) memcpy(data2, data2 - factor, factor);
		data2 += factor;
	    }
	}
	
	XPutImage(dpy, win, mag_gc, image2, 0, 0, 0, 0,
		  xsize*mag_factor, ysize*mag_factor);
	XDestroyImage( image1 );
	XDestroyImage( image2 );
    }
    
    /* return the new window */
    XFreePixmap( dpy, pixmap );
    return( win );
}

Window HGU_XCreateMag2Win(
Display		*dpy,
Window		parent,
int		x,
int		y,
int		xsize,
int		ysize)
{
    return( HGU_XCreateMagWin(dpy, parent, x, y, xsize, ysize, 2, 0) );
}

int HGU_XInteractWindowEvent(
Display			*dpy,
Window			win,
int			mode,
HGU_XInteractCallbacks	*callbacks,
caddr_t			cb_struct)
{
    if( mode ){ /* to satisfy the IRIX646 compiler */ }

    if( callbacks == NULL )
	return( 0 );

    if( callbacks->window_func != NULL ){
	return( (*callbacks->window_func)(dpy, win, callbacks->window_data,
					  cb_struct) );
    }
    return( 0 );
}

int HGU_XInteractNonwindowEvent(
Display			*dpy,
Window			win,
int			mode,
HGU_XInteractCallbacks	*callbacks,
XEvent			*event)
{
    if( callbacks == NULL )
	return( 0 );

    if( (mode & HGU_XFullApplicationModal) ||
       (mode & HGU_XSystemApplicationModal) ){
	switch( event->type ){
	case ButtonPress:
	case KeyPress:
	    XBell( dpy, 100 );
	    break;
	case Expose:		/* allow some events through */
	case VisibilityNotify:
	case ResizeRequest:
	case ConfigureNotify:
	case MapNotify:
	case UnmapNotify:
	    if( callbacks->non_window_func != NULL ){
		return( (*callbacks->non_window_func)
		       (dpy, win, callbacks->non_window_data,
			(caddr_t) event) );
	    }
	    break;
	default:
	    break;
	}
	return( 0 );
    }
    
    if( callbacks->non_window_func != NULL ){
	return( (*callbacks->non_window_func)
		(dpy, win, callbacks->non_window_data, (caddr_t) event) );
    }
    return( 0 );
}
    
int HGU_XInteractHelpEvent(
Display			*dpy,
Window			win,
HGU_XInteractCallbacks	*callbacks,
char 			*help_str)
{
    if( callbacks == NULL )
	return( 0 );

    if( callbacks->help_func != NULL ){
	return( (*callbacks->help_func)(dpy, win, callbacks->help_data,
					(caddr_t) help_str) );
    }

    if( help_str != NULL ){
	fprintf(stderr, "%s\n", help_str);
	fflush(stderr);
    }

    return( 0 );
}

void HGU_XGetButtonRelease(
Display		*dpy,
Window		win,
XEvent		*event)
{
    int		button = event->xbutton.button;
    int		continue_loop = 1;

    if( event->type != ButtonPress ){
	HGU_XError(dpy, win, "HGU_XGetButtonRelease",
		   "invalid event - not ButtonPress", 0);
	return;
    }

    do {
	XNextEvent(dpy, event);
	switch( event->type ){
	case ButtonRelease:
	    if( event->xbutton.button == button )
		continue_loop = 0;
	    break;
	case EnterNotify:
	case LeaveNotify:
	    XSendEvent(dpy, win, True, (EnterWindowMask|LeaveWindowMask),
		       event);
	    break;
	default:
	    break;
	}
    } while( continue_loop );

    return;
}

void HGU_XGetMagSize(
Display		*dpy,
Window		win,
int		x_centre,
int		y_centre,
unsigned int	*width,
unsigned int	*height)
{
    WlzFVertex2	*rect_vtx, start[2];


    start[0].vtX = x_centre - MAGWINSIZE / 2;
    start[0].vtY = y_centre - MAGWINSIZE / 2;
    start[1].vtX = MAGWINSIZE;
    start[1].vtY = MAGWINSIZE;
    rect_vtx = HGU_XGetRect(dpy, win, HGU_XNoConfirmMask |
			    HGU_XCentreRectMode, NULL, &(start[0]));
    if( rect_vtx != NULL ){
	*width  = rect_vtx[1].vtX;
	*height = rect_vtx[1].vtY;
	free( rect_vtx );
    } else {
	*width = *height = MAGWINSIZE;
    }
    *width  += *width%2;
    *height += *height%2;

    return;
}

Window HGU_XGetMag2Win(
Display		*dpy,
Window		win,
int		x_centre,
int		y_centre,
unsigned int	*width,
unsigned int	*height)
{
    HGU_XGetMagSize(dpy, win, x_centre, y_centre, width, height);
    return( HGU_XCreateMag2Win(dpy, win, x_centre, y_centre,
			       *width, *height) );
}
