#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _HGU_XmMiscProcs_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         HGU_XmMiscProcs.c
* \author	Richard Baldock
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
* \ingroup      HGU_Xm
* \brief        Miscellaneous procedures, too small to split up:
*	HGU_XmIsViewable:	test if a widget is currently viewable
*	HGU_XmGetShellOffsets:	get offset positions of a shell widget
*				within the WM window
*/

#include <stdio.h>

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/cursorfont.h>
#include <Xm/XmAll.h>

Boolean HGU_XmIsViewable(
Widget	w)
{
    XWindowAttributes	win_att;

    if( w == NULL || !XtIsRealized( w ) )
	return( False );

    if( !XGetWindowAttributes( XtDisplay(w), XtWindow(w), &win_att ) )
	return( False );

    if( win_att.map_state == IsViewable )
	return( True );
    return( False );
}

static int HGU_XmQueryRootPosition(
Display         *dpy,
Window          win,
int             *x,
int		*y)
{
        int                     X=0, Y=0;
	unsigned int		nchildren;
        XWindowAttributes       win_att;
        Window                  root, parent, *children;
 
        /* get position within parent */
        if( XGetWindowAttributes(dpy, win, &win_att) == 0 )
                return( 1 );
 
        if( win == win_att.root ){ /* stopping condition */
                *x = 0;
                *y = 0;
                return( 0 );
        }
 
        *x = win_att.x;
        *y = win_att.y;
 
        /* get the parent window */
        XQueryTree(dpy, win, &root, &parent, &children, &nchildren);
        XFree( children );
 
        /* get parent position */
        HGU_XmQueryRootPosition(dpy, parent, &X, &Y);
        *x += X;
        *y += Y;
 
        return( 0 );
}

int HGU_XmGetShellOffsets(
Widget		w,
Position	*x_off,
Position	*y_off)
{
    Display		*dpy;
    Window		win;
    Window		root, parent, *children;
    unsigned int	nchildren;
    int			x=0, y=0, x0=0, y0=0;

    /* only bother if this is a shell widget */
    if( !XtIsShell( w ) )
	return( -1 );

    /* check XWindow hierarchy for reparenting */
    dpy = XtDisplay( w );
    win = XtWindow( w );
    HGU_XmQueryRootPosition( dpy, win, &x, &y);
    if( !XQueryTree( dpy, win, &root, &parent, &children, &nchildren ) )
	return( -1 );

    /* check if reparented */
    while( parent != root ){
	win = parent;
	if( !XQueryTree( dpy, win, &root, &parent, &children, &nchildren ) )
	    return( -1 );
    }
    HGU_XmQueryRootPosition( dpy, win, &x0, &y0 );

    *x_off = x - x0;
    *y_off = y - y0;
    return( 0 );
}

Visual *HGU_XGetVisual(
  Display	*dpy,
  int		screen,
  int		class,
  unsigned int	depth)
{
  Visual	*visual=NULL;
  XVisualInfo	visualTemplate, *visualList;
  int		numVisuals;
  int		i, numCells;

  /* set up the template */
  visualTemplate.screen = screen;
  visualTemplate.class = class;
  visualTemplate.depth = depth;

  /* get matching visuals */
  visualList = XGetVisualInfo(dpy,
			      VisualScreenMask|VisualDepthMask|VisualClassMask,
			      &visualTemplate, &numVisuals);
  if( numVisuals < 1 ){
    return NULL;
  }

  /* select visual with the maximum number of color cells */
  for(i=0, numCells=0; i < numVisuals; i++){
    if( numCells < visualList[i].visual->map_entries ){
      visual = visualList[i].visual;
      numCells = visualList[i].visual->map_entries;
    }
  }
  XFree(visualList);
  
  return visual;
}

XVisualInfo HGU_XGetVisualInfo(
  Display	*dpy,
  int		screen,
  int		class,
  unsigned int	depth)
{
  XVisualInfo	visualTemplate, *visualList;
  int		numVisuals;
  int		i, numCells;

  /* set up the template */
  visualTemplate.screen = screen;
  visualTemplate.class = class;
  visualTemplate.depth = depth;

  /* get matching visuals */
  visualList = XGetVisualInfo(dpy,
			      VisualScreenMask|VisualDepthMask|VisualClassMask,
			      &visualTemplate, &numVisuals);
  if( numVisuals < 1 ){
    visualTemplate.visual = NULL;
    visualTemplate.depth = -1;
    return visualTemplate;
  }

  /* select visual with the maximum number of color cells */
  for(i=0, numCells=0; i < numVisuals; i++){
    if( numCells < visualList[i].visual->map_entries ){
      visualTemplate = visualList[i];
    }
  }
  XFree(visualList);
  
  return visualTemplate;
}

Visual *HGU_XmWidgetToVisual(Widget w)
{
  Visual	*visual;

  if( XtIsShell(w) ){
    XtVaGetValues(w, XtNvisual, &visual, NULL);
  }
  else {
    return HGU_XmWidgetToVisual(XtParent(w));
  }

  return visual;
}

void HGU_XmSetHourGlassCursor(
  Widget	w)
{
  static Cursor	cursor;

  if( !XtIsRealized(w) ){
    return;
  }

  /* create the cursor */
  if( !cursor ){
    cursor = XCreateFontCursor( XtDisplayOfObject(w), XC_watch );
  }

  /* if a shell widget, set the cursor */
  if( XtIsShell( w ) ){
    XDefineCursor(XtDisplay(w), XtWindow(w), cursor);
  }

  /* get the children, call function on each */
  if( XtIsComposite(w) ){
    Cardinal	numChildren;
    WidgetList	children;

    XtVaGetValues(w,
		  XmNchildren, &children,
		  XmNnumChildren, &numChildren,
		  NULL);
    while( numChildren ){
      numChildren--;
      HGU_XmSetHourGlassCursor(children[numChildren]);
    }
  }
  XFlush(XtDisplayOfObject(w));

  return;
}

void HGU_XmUnsetHourGlassCursor(
  Widget	w)
{
  XEvent	event;

  if( !XtIsRealized(w) ){
    return;
  }

  /* if a shell widget, unset the cursor */
  if( XtIsShell( w ) ){
    XUndefineCursor(XtDisplay(w), XtWindow(w));
  }

  /* get the children, call function on each */
  if( XtIsComposite(w) ){
    Cardinal	numChildren=0;
    WidgetList	children=NULL;

    XtVaGetValues(w,
		  XmNchildren, &children,
		  XmNnumChildren, &numChildren,
		  NULL);
    while( numChildren ){
      numChildren--;
      HGU_XmUnsetHourGlassCursor(children[numChildren]);
    }
  }
  XFlush(XtDisplayOfObject(w));

  /* clear pending events */
  while( XCheckMaskEvent(XtDisplayOfObject(w),
			 ButtonPressMask|ButtonReleaseMask|
			 ButtonMotionMask|PointerMotionMask|
			 KeyPressMask, &event) )
  {
    /* do nothing */
  }

  return;
}
