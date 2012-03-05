#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _HGU_XGetDomain_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         HGU_XGetDomain.c
* \author       Richard Baldock
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
* \brief        X interaction routine to modify / define a woolz domain
*/

#include <stdio.h>
#include <stdlib.h>

#include <math.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <Wlz.h>
#include <HGU_XInteract.h>

static char *help_str = 
"HGU_XGetDomain: interactively define a woolz domain using\n\
    the mouse or arrow keys. Parts of the domain can be\n\
    defined and deleted before finally returning to the calling\n\
    procedure. The domain can be defined (or deleted) in two modes,\n\
    DRAWING or PAINTING. Drawing implies defining a polyline and\n\
    the enclosed area will be added or subtracted from that already\n\
    defined. Painting will add or subtract the area defined by the\n\
    cursor which will be a circular disc of adjustable size.\n\
    The currently defined domain will show as a coloured stipple\n\
    or wash over the original window.\n\
\n\
Drawing: this is the default mode and the cursor will show as an arrow.\n\
    Pressing buttons 1 or 2 will initiate the definition of a polyline\n\
    using HGU_XGetPolydmn and allow rubber-band straight lines or\n\
    freehand drawing. When the line is confirmed (button 3 or return),\n\
    then the area enclosed (the polyline is closed by joining the first\n\
    and last points with a straight line) will be added to the existing\n\
    domain if the increment has no overlap with the original or partial \n\
    overlay and the initial buttonpress was within the original domain.\n\
    In all other cases the new domain will be subtracted from the old.\n\
\n\
Painting: in paint mode the cursor will be a circular disc and the area\n\
    under the disc can be added to the existing domain by pressing\n\
    buttons 1 or 2 with the centre of the disc within the original domain\n\
    and dragging (painting) as required. The cursor will act as an eraser\n\
    if the initial button press is outside of the existing domain.\n\
\n\
Confirm: the currently defined domain will be returned on pressing\n\
    button 3 or return.\n\
\n\
Controls: draw and paint mode are toggled by <ctrl>Button1 or\n\
    <ctrl>Button2. Paint mode is set by <ctrl>p, draw mode by <ctrl>d.\n\
    The existing domain can be temporarily removed by pressing\n\
    <meta>Button1 or <meta>Button2, the domain will be restored when the\n\
    the button is released. The paint cursor can be made bigger or\n\
    smaller by <ctrl>Up or <ctrl>Down respectively.";

/* local routines */
static WlzFVertex2 first_vertex(
  WlzPolygonDomain *pdmn)
{
  WlzFVertex2	vtx;

  switch( pdmn->type ){

  case WLZ_POLYGON_INT:
    vtx.vtX = pdmn->vtx->vtX;
    vtx.vtY = pdmn->vtx->vtY;
    break;

  case WLZ_POLYGON_FLOAT:
    vtx.vtX = ((WlzFVertex2 *) pdmn->vtx)->vtX;
    vtx.vtY = ((WlzFVertex2 *) pdmn->vtx)->vtY;
    break;

  case WLZ_POLYGON_DOUBLE:
    vtx.vtX = ((WlzDVertex2 *) pdmn->vtx)->vtX;
    vtx.vtY = ((WlzDVertex2 *) pdmn->vtx)->vtY;
    break;

  default:
    vtx.vtX = 0;
    vtx.vtY = 0;
    break;

  }

  return( vtx );
}

static WlzObject *makeblob(
  int		radius,
  HGU_XBlobType	blob_type)
{
  WlzDomain	domain;
  WlzValues	values;

  values.core = NULL;
  radius = (radius < 0) ? 0 : radius;

  switch( blob_type ){
  default:
  case HGU_XBLOBTYPE_CIRCLE:
    return WlzMakeCircleObject((double)radius, 0.0, 0.0, NULL);

  case HGU_XBLOBTYPE_SQUARE:
    domain.i = WlzMakeIntervalDomain(WLZ_INTERVALDOMAIN_RECT,
				     -radius, radius,
				     -radius, radius, NULL);
    return WlzMakeMain(WLZ_2D_DOMAINOBJ, domain, values, NULL, NULL, NULL);
  }
}

static int check_radius(
  Display		*dpy,
  Window		win,
  int		radius)
{
  unsigned int	w, h, rw, rh;

  w = h = 2*radius + 3;
  return( XQueryBestCursor(dpy, win, w, h, &rw, &rh) );
}

static void MySetCursor(
  Display		*dpy,
  Window		win,
  int		blob_cursor,
  Cursor		cursor)
{
  if( blob_cursor )
    XDefineCursor( dpy, win, cursor );
  else
    HGU_XSetCursor( dpy, win, HGU_XCURSOR_POINTER );

  return;
}

static int call_window_func(
  Display			*dpy,
  Window			win,
  int			mode,
  HGU_XInteractCallbacks	*callbacks,
  WlzObject		*obj,
  WlzObject		*obj_incr,
  int			incr,
  HGU_XGetDomainCallbackStruct	*cb_struct)
{
  cb_struct->increment = incr;
  cb_struct->obj_incr = obj_incr;
  cb_struct->obj = obj;
  return( HGU_XInteractWindowEvent(dpy, win, mode, callbacks,
				   (caddr_t) cb_struct) );
}

typedef struct {
  HGU_XInteractCallback		window_func;
  caddr_t				window_data;
  HGU_XGetDomainCallbackStruct	*cb_struct;
} GetPolyWindowData;

int getpoly_window_func(
  Display			*dpy,
  Window			win,
  caddr_t			client_data,
  caddr_t			call_data)
{
  HGU_XGetVtxsCallbackStruct	*cbs=(HGU_XGetVtxsCallbackStruct *) call_data;
  GetPolyWindowData	*getpoly_window_data =
    (GetPolyWindowData *) client_data;
  HGU_XGetDomainCallbackStruct	*cb_struct =
    getpoly_window_data->cb_struct;

  cb_struct->x = cbs->x;
  cb_struct->y = cbs->y;
  if( getpoly_window_data->window_func != NULL )
    return( (*(getpoly_window_data->window_func))
	   (dpy, win, getpoly_window_data->window_data,
	    (caddr_t) cb_struct) );
  else
    return( 0 );
}

typedef struct {
  HGU_XInteractCallback	window_func;
  caddr_t		window_data;
  int			x_centre, y_centre;
  unsigned int	width, height;
} MagDomainWindowData;

static int mag_window_func(
  Display			*dpy,
  Window			win,
  caddr_t			client_data,
  caddr_t			call_data)
{
  HGU_XGetDomainCallbackStruct
    *cbs = (HGU_XGetDomainCallbackStruct *) call_data;
  MagDomainWindowData		*data = (MagDomainWindowData *) client_data;
  HGU_XGetDomainCallbackStruct	cb_struct;

  cb_struct = *cbs;
  cb_struct.x = data->x_centre + (cbs->x - data->width) / 2;
  cb_struct.y = data->y_centre + (cbs->y - data->height) / 2;
  if( data->window_func != NULL )
    return( (*(data->window_func))
	   (dpy, win, data->window_data,
	    (caddr_t) &cb_struct) );
  else
    return( 0 );
}

/* public routines */
UndoObjectList push_undo_object(
  UndoObjectList	obj_list,
  WlzObject	*obj)
{
  UndoObjectList	new_obj_list;

  new_obj_list = (UndoObjectList) AlcMalloc(sizeof(UndoObjectRec));

  if( obj != NULL )
    new_obj_list->obj = WlzMakeMain(obj->type, obj->domain, obj->values,
				    NULL, NULL, NULL);
  else
    new_obj_list->obj = NULL;
  new_obj_list->next = obj_list;
  return( new_obj_list );
}

UndoObjectList pop_undo_object(
  UndoObjectList	obj_list,
  WlzObject	**obj)
{
  if( obj_list == NULL )
    return( NULL );

  if( obj_list->obj != NULL )
    *obj = WlzMakeMain(obj_list->obj->type, obj_list->obj->domain,
		    obj_list->obj->values, NULL, NULL, NULL);

  if( obj_list->next != NULL ){
    UndoObjectList return_obj_list = obj_list->next;
    
    if( obj_list->obj != NULL )
      WlzFreeObj( obj_list->obj );
    free( obj_list );

    obj_list = return_obj_list;
  }

  return( obj_list );
}

void clear_undo_object(
  UndoObjectList	obj_list)
{
  if( obj_list != NULL ){
    clear_undo_object( obj_list->next );
    if( obj_list->obj != NULL )
      WlzFreeObj( obj_list->obj );
    free( obj_list );
  }
  return;
}

WlzObject *HGU_XGetDomain(
  Display			*dpy,
  Window			win,
  int			mode,
  HGU_XInteractCallbacks	*callbacks,
  HGU_XGridConstraint	*constraint,
  HGU_XDomainStyleStruct	*style,
  WlzObject		*start)
{
  XEvent			event;
  GC				gc, gc_copy;
  Pixmap			pixmap, stipple;
  int				finished = 0, x, y, add_blob=0;
  WlzObject			*obj, *obj1, *blobj, *obj_incr;
  WlzDomain			old_pdmn, new_pdmn;
  WlzValues			values;
  WlzFVertex2			start_vtx, *fVtxPtr;
  XWindowAttributes		win_att;
  HGU_XGetDomainCallbackStruct	cb_struct;
  HGU_XInteractCallbacks		getpoly_cbcks;
  GetPolyWindowData			getpoly_window_data;
  int				blob_radius;
  Cursor			blob_cursor;
  UndoObjectList		undo_list;

  /* set values union for WlzMakeMain calls */
  values.core = NULL;

  /* set up the callback struct */
  cb_struct.event = &event;
  getpoly_cbcks = *callbacks;
  getpoly_cbcks.window_func = getpoly_window_func;
  getpoly_cbcks.window_data = (caddr_t) &getpoly_window_data;
  getpoly_window_data.window_func = callbacks->window_func;
  getpoly_window_data.window_data = callbacks->window_data;
  getpoly_window_data.cb_struct = &cb_struct;

  /* get domain gc and copy gc */
  gc = HGU_XCreateDomainGC(dpy, win, style, &stipple);
  gc_copy = HGU_XGetGC( dpy, win, HGU_XGC_COPY );
    
  /* create the blob-cursor domain for add/erase */
  blob_radius = style->blob_radius;
  if( blob_radius < 0 || !check_radius(dpy, win, blob_radius) )
    blob_radius = 8;
  blobj = makeblob( blob_radius, style->blob_type );
  blob_cursor = HGU_XCreateObjectCursor( dpy, win, blobj,
					style->blob_border );

  /* set the cursor */
  MySetCursor( dpy, win, mode&HGU_XPixelDomainMode, blob_cursor );

  /* copy the window into a pixmap */
  if( XGetWindowAttributes(dpy, win, &win_att) == 0 ){
    HGU_XError(dpy, win, "HGU_XGetPolydmn",
	       "failed to get window attributes", 0);
    if( stipple != (Pixmap) 0 )
      XFreePixmap( dpy, stipple );
    XFreeGC( dpy, gc );
    return( NULL );
  }
  pixmap = XCreatePixmap(dpy, win, win_att.width, win_att.height,
			 win_att.depth);
  XCopyArea(dpy, win, pixmap, gc_copy, 0, 0, win_att.width, win_att.height,
	    0, 0);
    
  /* check starting domain */
  if( start != NULL ){
    obj = HGU_XIncrementDomain(dpy, win, pixmap, gc, NULL, start);
  } else {
    obj = NULL;
  }

  /* initialise the undo list */
  undo_list = push_undo_object( NULL, obj );

  /* select input and passive button grab */
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
    if( HGU_XCheckCloseEvent(dpy, win, &event) ){
      finished = 1;
      continue;
    }
    if( event.xany.window != win ){
      finished = HGU_XInteractNonwindowEvent(dpy, win, mode, callbacks,
					     &event);
      continue;
    }

    switch( event.type ){

    case ButtonPress:

      switch( event.xbutton.button ){
      case Button1:	/* first selected point */
      case Button2:	/* change mode */

	x = event.xbutton.x;
	y = event.xbutton.y;

	/* check for magnify option */
	if( event.xbutton.state & ShiftMask ){
	  Window 			mag_win;
	  WlzObject			*mag_obj;
	  HGU_XGridConstraint		mag_constraint;
	  HGU_XDomainStyleStruct	mag_style;
	  HGU_XInteractCallbacks	mag_callbacks;
	  MagDomainWindowData		mag_window_data;
	  unsigned int		w, h;

	  /* save previous object */
	  undo_list = push_undo_object( undo_list, obj );

	  /* get the magnify window */
	  HGU_XGetMagSize(dpy, win, x, y, &w, &h);

	  HGU_XUndispDomain(dpy, win, pixmap, gc_copy, obj);
	  mag_win= HGU_XCreateMag2Win(dpy, win, x, y, w, h);

	  /* set up start object if defined */
	  obj1 = HGU_XMag2Object(obj, x, y, w, h, 1);
	  if( obj != NULL )
	    WlzFreeObj( obj );

	  /* set up the callbacks structure */
	  mag_callbacks = *callbacks;
	  mag_callbacks.window_func = mag_window_func;
	  mag_callbacks.window_data = (caddr_t) &mag_window_data;
	  mag_window_data.x_centre = x;
	  mag_window_data.y_centre = y;
	  mag_window_data.width    = w;
	  mag_window_data.height   = h;
	  mag_window_data.window_func = callbacks->window_func;
	  mag_window_data.window_data = callbacks->window_data;

	  /* set up the style */
	  mag_style = *style;
	  if( stipple != (Pixmap) 0 )
	    mag_style.stipple = stipple;

	  /* set up the constraints */
	  if( constraint == NULL ){
	    mag_constraint.sx = 2.0;
	    mag_constraint.sy = 2.0;
	    mag_constraint.tx = 0.0;
	    mag_constraint.ty = 0.0;
	  } else {
	    mag_constraint.sx = 2.0 * constraint->sx;
	    mag_constraint.sy = 2.0 * constraint->sx;
	    mag_constraint.tx = (constraint->tx - x + w/2) * 2;
	    mag_constraint.ty = (constraint->ty - y + h/2) * 2;
	  }			

	  /* get a vertex from the mag window */
	  HGU_XUngrabPointer(dpy, win);
	  mag_obj = HGU_XGetDomain(dpy, mag_win, mode,
				   &mag_callbacks, &mag_constraint,
				   &mag_style, obj1);
	  (void) HGU_XDestroyWindow(dpy, mag_win);
	  if( QueryInWindow( dpy, win ) ){
	    HGU_XGrabPointer(dpy, win, mode&HGU_XConfineMask);
	  }
	  MySetCursor( dpy, win, mode&HGU_XPixelDomainMode,
		      blob_cursor );

	  /* clean up and transform returned vertex */
	  if( obj1 != NULL )
	    WlzFreeObj( obj1 );
	  if( mag_obj != NULL ){
	    obj = HGU_XMag2Object(mag_obj, x, y, w, h, 0);
	    WlzFreeObj( mag_obj );
	    HGU_XDispDomain(dpy, win, gc, obj);
	  } else {
	    obj = NULL;
	  }
	  break;
	}

	/* check for toggle paint mode */
	if( event.xbutton.state & ControlMask ){

	  if( mode & HGU_XPixelDomainMode )
	    mode &= ~HGU_XPixelDomainMode;
	  else
	    mode |= HGU_XPixelDomainMode;

	  MySetCursor( dpy, win, mode&HGU_XPixelDomainMode,
		      blob_cursor );
	  HGU_XGetButtonRelease(dpy, win, &event);
	  break;
	}

	/* check for toggle show domain */
	if( event.xbutton.state & Mod1Mask ){
	  HGU_XUndispDomain(dpy, win, pixmap, gc_copy, obj);
	  HGU_XGetButtonRelease(dpy, win, &event);
	  HGU_XDispDomain(dpy, win, gc, obj);
	  break;
	}

	/* check add or subtract */
	if( obj == NULL || WlzInsideDomain(obj, 0.0,
					   (double) y, (double) x,
					   NULL) )
	  add_blob = 1;
	else
	  add_blob = 0;

	/* check if in paint mode */
	if( mode & HGU_XPixelDomainMode )
	  break;

	/* save previous object */
	undo_list = push_undo_object( undo_list, obj );

	/* create a start polyline */
	start_vtx.vtX = x;
	start_vtx.vtY = y;
	fVtxPtr = &start_vtx;
	old_pdmn.poly = WlzMakePolygonDomain(WLZ_POLYGON_FLOAT, 1,
					     (WlzIVertex2 *) fVtxPtr,
					     1, 1, NULL);

	/* edit or get new polyline */
	HGU_XUngrabPointer(dpy, win);
	cb_struct.increment = add_blob;
	cb_struct.obj_incr = NULL;
	cb_struct.obj = obj;
	new_pdmn.poly = HGU_XGetPolydmn(dpy, win, mode, &getpoly_cbcks,
					old_pdmn.poly);
	WlzFreePolyDmn( old_pdmn.poly );

	if( QueryInWindow( dpy, win ) ){
	  HGU_XGrabPointer(dpy, win, mode&HGU_XConfineMask);
	}
	MySetCursor( dpy, win, mode&HGU_XPixelDomainMode,
		    blob_cursor );
	if( new_pdmn.poly == NULL )
	  break;

	/* create the new domain */
	obj_incr  = WlzPolyToObj( new_pdmn.poly, WLZ_SIMPLE_FILL, NULL );
	start_vtx = first_vertex( new_pdmn.poly );
	WlzFreePolyDmn( new_pdmn.poly );
	x = start_vtx.vtX;
	y = start_vtx.vtY;

	/* filled_polytoobj can now return NULL */
	if( obj_incr == NULL ){
	  break;
	}

	/* increment domain */
	if( obj != NULL ){
	  obj1 = WlzIntersect2( obj, obj_incr, NULL );
	  if( (obj1 == NULL) || (WlzArea(obj1, NULL) == 0) )
	    add_blob = 1;
	  if( WlzArea(obj_incr, NULL) )
	    add_blob = 0;
	  if( obj1 != NULL )
	    WlzFreeObj( obj1 );
	}

	/* check constraints */
	if( obj_incr ){
	  obj1 = HGU_XCheckConstraints( obj_incr, constraint );
	  WlzFreeObj( obj_incr );
	  obj_incr = obj1;
	}

	if( add_blob )
	  obj1 = HGU_XIncrementDomain(dpy, win, pixmap, gc, obj,
				      obj_incr);
	else
	  obj1 = HGU_XDecrementDomain(dpy, win, pixmap, gc_copy, obj,
				      obj_incr);

	finished = call_window_func(dpy, win, mode, callbacks, obj,
				    obj_incr, add_blob, &cb_struct);

	WlzFreeObj( obj_incr );
	if( obj != NULL )
	  WlzFreeObj( obj );
	obj = obj1;

	break;				

      case Button3:	/* confirm */
	finished = 1;
	break;
      }
      break;

    case KeyPress:
      switch( XLookupKeysym(&event.xkey, 0) ){
      case XK_Delete:
      case XK_BackSpace:
	break;
      case XK_Right:
	break;
      case XK_Left:
	break;
      case XK_Up:		/* make cursor bigger */
	if( event.xkey.state & ControlMask &&
	   mode & HGU_XPixelDomainMode ){
	  blob_radius += 1;
	  if( !check_radius(dpy, win, blob_radius) ){
	    blob_radius -= 1;
	    XBell(dpy, 100);
	    break;
	  }
	  WlzFreeObj( blobj );
	  blobj = makeblob( blob_radius, style->blob_type );
	  XFreeCursor( dpy, blob_cursor );
	  blob_cursor = HGU_XCreateObjectCursor(dpy, win, blobj,
						style->blob_border);
	  MySetCursor( dpy, win, mode&HGU_XPixelDomainMode,
		      blob_cursor );
	}
	break;
      case XK_Down:	/* make cursor smaller */
	if( event.xkey.state & ControlMask &&
	   mode & HGU_XPixelDomainMode ){
	  blob_radius -= 1;
	  if( blob_radius < 0 ){
	    blob_radius = 0;
	    XBell(dpy, 100);
	    break;
	  }
	  WlzFreeObj( blobj );
	  blobj = makeblob( blob_radius, style->blob_type );
	  XFreeCursor( dpy, blob_cursor );
	  blob_cursor = HGU_XCreateObjectCursor(dpy, win, blobj,
						style->blob_border);
	  MySetCursor( dpy, win, mode&HGU_XPixelDomainMode,
		      blob_cursor );
	}
	break;
      case XK_space:
	break;
      case XK_Return:
	finished = 1;
	break;
      case XK_Help:
	HGU_XInteractHelpEvent(dpy, win, callbacks, help_str);
	break;
      case XK_B:
      case XK_b:		/* <ctrl> b toggle cursor border */
	if( event.xkey.state & ControlMask &&
	   mode & HGU_XPixelDomainMode ){
	  style->blob_border = style->blob_border?0:1;
	  XFreeCursor( dpy, blob_cursor );
	  blob_cursor = HGU_XCreateObjectCursor(dpy, win, blobj,
						style->blob_border);
	  MySetCursor( dpy, win, mode&HGU_XPixelDomainMode,
		      blob_cursor );
	}
	break;
      case XK_D:
      case XK_d:		/* <ctrl> d puts into draw mode */
	if( event.xkey.state & ControlMask ){
	  mode &= ~HGU_XPixelDomainMode;
	  MySetCursor( dpy, win, mode&HGU_XPixelDomainMode,
		      blob_cursor );
	}
	break;
      case XK_P:
      case XK_p:		/* <ctrl> p puts into paint mode */
	if( event.xkey.state & ControlMask ){
	  mode |= HGU_XPixelDomainMode;
	  MySetCursor( dpy, win, mode&HGU_XPixelDomainMode,
		      blob_cursor );
	}
	break;
      case XK_R:
      case XK_r:		/* <ctrl> r makes cursor round */
	if( event.xkey.state & ControlMask &&
	   mode & HGU_XPixelDomainMode ){
	  WlzFreeObj( blobj );
	  style->blob_type = HGU_XBLOBTYPE_CIRCLE;
	  blobj = makeblob( blob_radius, style->blob_type );
	  XFreeCursor( dpy, blob_cursor );
	  blob_cursor = HGU_XCreateObjectCursor(dpy, win, blobj,
						style->blob_border);
	  MySetCursor( dpy, win, mode&HGU_XPixelDomainMode,
		      blob_cursor );
	}
	break;
      case XK_S:
      case XK_s:		/* <ctrl> s makes cursor square */
	if( event.xkey.state & ControlMask &&
	   mode & HGU_XPixelDomainMode ){
	  WlzFreeObj( blobj );
	  style->blob_type = HGU_XBLOBTYPE_SQUARE;
	  blobj = makeblob( blob_radius, style->blob_type );
	  XFreeCursor( dpy, blob_cursor );
	  blob_cursor = HGU_XCreateObjectCursor(dpy, win, blobj,
						style->blob_border);
	  MySetCursor( dpy, win, mode&HGU_XPixelDomainMode,
		      blob_cursor );
	}
	break;
      case XK_u:	/* undo linked to following case */
      case XK_U:
	if( !(event.xkey.state & ControlMask) )
	  break;
	/* don't add additional cases here */
      case XK_Undo:
      case XK_F14:
	if( obj != NULL ){
	  HGU_XUndispDomain(dpy, win, pixmap, gc_copy, obj);
	  WlzFreeObj( obj );
	  obj = NULL;
	}
	undo_list = pop_undo_object( undo_list, &obj );
	if( obj != NULL );
	HGU_XDispDomain(dpy, win, gc, obj);
	break;
      default:
	break;
      }
      break;
	    
    case KeyRelease:
      switch( XLookupKeysym(&event.xkey, 0) ){
      case XK_Delete:
      case XK_BackSpace:
	break;
      default:
	break;
      }
      break;

    case MotionNotify:
      /* check for button1 and pixel mode */
      x = event.xmotion.x;
      y = event.xmotion.y;
      cb_struct.x = x;
      cb_struct.y = y;
      if( (event.xmotion.state & (Button1Mask|Button2Mask)) &&
	 (mode & HGU_XPixelDomainMode) )
      {
	/* save previous object */
	undo_list = push_undo_object( undo_list, obj );

	blobj->domain.i->kol1   = x - blob_radius;
	blobj->domain.i->lastkl = x + blob_radius;
	blobj->domain.i->line1  = y - blob_radius;
	blobj->domain.i->lastln = y + blob_radius;

	/* check constraints */
	obj_incr = HGU_XCheckConstraints( blobj, constraint );

	if( add_blob )
	  obj1 = HGU_XIncrementDomain(dpy, win, pixmap, gc, obj,
				      obj_incr);
	else
	  obj1 = HGU_XDecrementDomain(dpy, win, pixmap, gc_copy, obj,
				      obj_incr);
	finished = call_window_func(dpy, win, mode, callbacks, obj,
				    obj_incr, add_blob, &cb_struct);

	if( obj_incr )
	  WlzFreeObj( obj_incr );
	if( obj != NULL )
	  WlzFreeObj( obj );
	obj = obj1;
      }
      else
	finished = call_window_func(dpy, win, mode, callbacks, obj,
				    NULL, 0, &cb_struct);
      break;

    case EnterNotify:
      HGU_XGrabPointer(dpy, win, mode&HGU_XConfineMask);
      break;

    case LeaveNotify:
      HGU_XUngrabPointer(dpy, win);
      break;

    case Expose:
      XCopyArea(dpy, pixmap, win, gc_copy, 0, 0,
		win_att.width, win_att.height, 0, 0);
      HGU_XDispDomain(dpy, win, gc, obj);
      break;

    default:
      break;
    }
  }

  /* ungrab the button */
  HGU_XUngrabPointer(dpy, win);

  /* undefine the cross cursor */
  XUndefineCursor( dpy, win );

  /* reset input mask */
  XSelectInput(dpy, win, win_att.your_event_mask );

  /* clear pending events */
  while( XCheckWindowEvent(dpy, win, ~0x0, &event) == True )
    continue;
    
  /* clean up and return */
  HGU_XUndispDomain(dpy, win, pixmap, gc_copy, obj);
  WlzFreeObj( blobj );
  if( stipple != (Pixmap) 0 )
    XFreePixmap( dpy, stipple );
  XFreeGC( dpy, gc );
  XFreePixmap( dpy, pixmap );
  XFreeCursor( dpy, blob_cursor );

  /* clear undo objects */
  clear_undo_object( undo_list );

  /* check size */
  if( obj != NULL && WlzArea( obj, NULL ) <= 0 ){
    WlzFreeObj( obj );
    obj = NULL;
  }

  return( obj );
}
