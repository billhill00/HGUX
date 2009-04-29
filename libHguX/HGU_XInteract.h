#ifndef HGU_XINTERACT_H
#define HGU_XINTERACT_H

#if defined(__GNUC__)
#ident "MRC HGU $Id:"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id:"
#else static char _HGU_XInter_ct.h[] = "MRC HGU $Id:";
#endif
#endif
/*!
* \file         HGU_XInteract.h
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Wed Apr 29 08:26:09 2009
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
* \brief        Header file for HGU_X - interactive programs
*               
* \todo         -
* \bug          None known
*
* Maintenance log with most recent changes at top of list.
*/

#include <sys/types.h>
#include <sys/time.h>

#include <X11/Xlib.h>
#include <Wlz.h>
#include <HGU_XUtils.h>

#ifdef  __cplusplus
extern "C" {
#endif

#define MAGWINSIZE	128
#define MAGOFFSET	32

/* timer defines and variables */
#define HGU_XSHORTDELAY		50000           /* microseconds */
#define HGU_XLONGDELAY		500000          /* microseconds */
#define HGU_XDOUBLECLICKDELAY	250000		/* microseconds */

extern struct itimerval	L_value, S_value, DC_value;
extern int 		HGU_XTimerExpired;

/* mode masks */
typedef enum  {
	HGU_XConfineMask		= 1<<0,
	HGU_XNoConfirmMask		= 1<<1,
	HGU_XDomainPaintMode		= 1<<2,
	HGU_XMagWindowMask		= 1<<3,
	HGU_XCentreRectMode		= 1<<4,
	HGU_XSubWindowMode		= 1<<5,
	HGU_XPrimaryApplicationModal	= 1<<6,
	HGU_XFullApplicationModal	= 1<<7,
	HGU_XSystemApplicationModal	= 1<<8
} HGU_Xmode;

#define HGU_XPixelDomainMode HGU_XDomainPaintMode

/* cursor types */
typedef enum {
    HGU_XCURSOR_NONE		= 0,
    HGU_XCURSOR_DEFAULT,
    HGU_XCURSOR_POINTER,
    HGU_XCURSOR_TOPLEFTCORNER,
    HGU_XCURSOR_TOPRIGHTCORNER,
    HGU_XCURSOR_BOTTOMLEFTCORNER,
    HGU_XCURSOR_BOTTOMRIGHTCORNER,
    HGU_XCURSOR_CROSS,
    HGU_XCURSOR_TRANSLATE,
    HGU_XCURSOR_SCALE,
    HGU_XCURSOR_ROTATE,
    HGU_XCURSOR_SHEAR,
    HGU_XCURSOR_BLOB,
    HGU_XCURSOR_WAITING,
    HGU_XCURSOR_OBJECT,
    HGU_XCURSOR_POINT,
    HGU_XCURSOR_LAST		/* this must be the last in the list */
} HGU_XCursorType;

/* GC types */
typedef enum {
    HGU_XGC_INTERACT		= 0,
    HGU_XGC_CONFIRMED,
    HGU_XGC_PLANE,
    HGU_XGC_DRAW,
    HGU_XGC_DELETE,
    HGU_XGC_COPY,
    HGU_XGC_HIGHLIGHT,
    HGU_XGC_LAST		/* this must be the last in the list */
} HGU_XGCType;

/* interact callback structures */
#ifdef __STDC__ /* [ */
typedef int (*HGU_XInteractCallback)(Display *dpy, Window win,
			      caddr_t client_data, caddr_t call_data);
#else /* ! __STDC__ ][ */
typedef int (*HGU_XInteractCallback)();
#endif /* __STDC__ ] */

typedef struct {
    int				type;
    HGU_XInteractCallback	window_func;
    caddr_t			window_data;
    HGU_XInteractCallback	non_window_func;
    caddr_t			non_window_data;
    HGU_XInteractCallback	help_func;
    caddr_t			help_data;
} HGU_XInteractCallbacks;

typedef struct {
    XEvent	*event;
    float	x;
    float	y;
} HGU_XGetVtxCallbackStruct;
	
typedef struct {
    XEvent	*event;
    float	x;
    float	y;
    WlzFVertex2	*vtxs;
    int		nvtxs;
} HGU_XGetVtxsCallbackStruct;
	
typedef struct {
    XEvent	*event;
    float	x;
    float	y;
    float	width;
    float	height;
} HGU_XGetRectCallbackStruct;
	
typedef struct {
    XEvent	*event;
    float	x;
    float	y;
    int		increment;
    WlzObject	*obj_incr;
    WlzObject	*obj;
} HGU_XGetDomainCallbackStruct;

/* cursor types */
typedef enum {
    HGU_XBLOBTYPE_CIRCLE	= 0,
    HGU_XBLOBTYPE_SQUARE,
    HGU_XBLOBTYPE_LAST		/* this must be the last in the list */
} HGU_XBlobType;

typedef struct {
    int			blob_radius;
    HGU_XBlobType	blob_type;
    int			blob_border;
    int			colour;
    unsigned int	plane_mask;
    Pixmap		stipple;
    char		*stipple_file;
    int			thresh_width;
} HGU_XDomainStyleStruct;
	
typedef struct {
    int		type;
    float	sx;
    float	sy;
    float	tx;
    float	ty;
} HGU_XGridConstraint;

/* undo object list */
typedef struct _UndoObjectRec {
    WlzObject			*obj;
    struct _UndoObjectRec	*next;
} UndoObjectRec, *UndoObjectList;


/* routines useful for interact programs */
extern int	HGU_XSetUpTimers	(void);
extern int	HGU_XSetTimer		(struct itimerval *val);
extern int	HGU_XSetWindowColormap	(Display	*dpy,
					 Window		win,
					 Colormap	cmap);
extern int	HGU_XDestroyWindow	(Display	*dpy,
					 Window		win);
extern int	HGU_XIsAscendent	(Display	*dpy,
					 Window		win,
					 Window		testwin);
extern int	HGU_XCheckCloseEvent	(Display	*dpy,
					 Window		win,
					 XEvent		*event);
extern int	HGU_XGrabButton		(Display	*dpy,
					 Window		win,
					 int		confine);
extern int	HGU_XUngrabButton	(Display	*dpy,
					 Window		win);
extern int	HGU_XGrabPointer	(Display	*dpy,
					 Window		win,
					 int		confine);
extern int	HGU_XUngrabPointer	(Display	*dpy,
					 Window		win);
extern int	HGU_XDrawCross		(Display	*dpy,
					 Window		win,
					 GC		gc,
					 int		x,
					 int		y);
extern int	HGU_XDrawRectangle	(Display	*dpy,
					 Window		win,
					 GC		gc,
					 int		x,
					 int		y,
					 int		w,
					 int		h);
extern Cursor	HGU_XCreateObjectCursor	(Display	*dpy,
					 Window		win,
					 WlzObject	*obj,
					 int		border);
extern void	HGU_XSetCursor		(Display		*dpy,
					 Window			win,
					 HGU_XCursorType	cursor_type);
extern GC	HGU_XGetGC		(Display	*dpy,
					 Window		win,
					 HGU_XGCType	gc_type);
extern Window	HGU_XCreateMagWin	(Display	*dpy,
					 Window		win,
					 int		x,
					 int		y,
					 int		xsize,
					 int		ysize,
					 int		mag_factor,
					 int		interp);
extern Window	HGU_XCreateMag2Win	(Display	*dpy,
					 Window		win,
					 int		x,
					 int		y,
					 int		xsize,
					 int		ysize);
extern int	HGU_XInteractWindowEvent(
    Display			*dpy,
    Window			win,
    int				mode,
    HGU_XInteractCallbacks	*callbacks,
    caddr_t			cb_struct);
extern int	HGU_XInteractNonwindowEvent(
    Display			*dpy,
    Window			win,
    int				mode,
    HGU_XInteractCallbacks	*callbacks,
    XEvent			*event);
extern int	HGU_XInteractHelpEvent(
    Display			*dpy,
    Window			win,
    HGU_XInteractCallbacks	*callbacks,
    char 			*help_str);
extern void	HGU_XGetButtonRelease	(Display	*dpy,
					 Window		win,
					 XEvent		*event);
extern void	HGU_XGetMagSize		(Display	*dpy,
					 Window		win,
					 int		x_centre,
					 int		y_centre,
					 unsigned int	*width,
					 unsigned int	*height);
extern Window	HGU_XGetMag2Win		(Display	*dpy,
					 Window		win,
					 int		x_centre,
					 int		y_centre,
					 unsigned int	*width,
					 unsigned int	*height);

/* convenience routines for interact procedures using domains */
extern void	HGU_XDispDomain		(Display	*dpy,
					 Window		win,
					 GC		gc,
					 WlzObject	*obj);
extern void	HGU_XUndispDomain	(Display	*dpy,
					 Window		win,
					 Pixmap		pixmap,
					 GC		gc,
					 WlzObject	*obj);
extern WlzObject 	*HGU_XIncrementDomain(Display		*dpy,
					      Window		win,
					      Pixmap		pixmap,
					      GC		gc,
					      WlzObject		*obj,
					      WlzObject		*obj_incr);
extern WlzObject	*HGU_XDecrementDomain(Display		*dpy,
					      Window		win,
					      Pixmap		pixmap,
					      GC		gc,
					      WlzObject		*obj,
					      WlzObject		*obj_incr);
extern GC	HGU_XCreateDomainGC(Display			*dpy,
				    Window			win,
				    HGU_XDomainStyleStruct	*style,
				    Pixmap			*stipple);

extern WlzObject 	*HGU_XDoubleObj	(WlzObject *obj);
extern WlzObject 	*HGU_XHalveObj	(WlzObject *obj);
extern WlzObject 	*HGU_XMag2Object(WlzObject	*obj,
					 int		x_centre,
					 int		y_centre,
					 unsigned int	width,
					 unsigned int	height,
					 int		magnify);
extern WlzObject 	*HGU_XCheckConstraints(
    WlzObject	 	*obj,
    HGU_XGridConstraint *constraint);

/* routines to maintain an undo list */
extern UndoObjectList	push_undo_object(
    UndoObjectList	obj_list,
    WlzObject		*obj);
extern UndoObjectList	pop_undo_object(
    UndoObjectList	obj_list,
    WlzObject		**obj);
extern void		clear_undo_object(
    UndoObjectList 	obj_list);

/* interact routines */
extern WlzObject	*HGU_XGetDomain(
    Display			*dpy,
    Window			win,
    int				mode,
    HGU_XInteractCallbacks	*callbacks,
    HGU_XGridConstraint		*constraint,
    HGU_XDomainStyleStruct	*style,
    WlzObject			*start);

extern WlzPolygonDomain	*HGU_XGetPolydmn(
    Display			*dpy,
    Window			win,
    int				mode,
    HGU_XInteractCallbacks	*callbacks,
    WlzPolygonDomain		*start);

extern WlzPolygonDomain	*HGU_XGetPolyDomain(
    Display			*dpy,
    Window			win,
    int				mode,
    HGU_XInteractCallbacks	*callbacks,
    WlzPolygonDomain		*start);

extern WlzObject	*HGU_XGetThresholdDomain(
    Display			*dpy,
    Window			win,
    int				mode,
    HGU_XInteractCallbacks	*callbacks,
    HGU_XGridConstraint		*constraint,
    HGU_XDomainStyleStruct	*style,
    WlzObject			*start,
    WlzObject			*ref_obj);
    
extern WlzFVertex2	*HGU_XGetRect	(Display	*dpy,
					 Window		win,
					 int		mode,
					 HGU_XInteractCallbacks	*callbacks,
					 WlzFVertex2	*start);
extern WlzFVertex2	*HGU_XGetVtx	(Display	*dpy,
					 Window		win,
					 int		mode,
					 HGU_XInteractCallbacks	*callbacks,
					 WlzFVertex2	*start);
extern int		HGU_XGetVtxs	(Display	*dpy,
					 Window		win,
					 int		mode,
					 HGU_XInteractCallbacks	*callbacks,
					 WlzFVertex2	*start,
					 int		n_start,
					 WlzFVertex2	**return_vtxs);

#ifdef  __cplusplus
}
#endif

#endif  /* HGU_XINTERACT_H */
/* don't put anything after this line */
