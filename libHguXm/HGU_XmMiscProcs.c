#pragma ident "MRC HGU $Id$"
/*****************************************************************************
* Copyright   :    1994 Medical Research Council, UK.                        *
*                  All rights reserved.                                      *
******************************************************************************
* Address     :    MRC Human Genetics Unit,                                  *
*                  Western General Hospital,                                 *
*                  Edinburgh, EH4 2XU, UK.                                   *
******************************************************************************
* Project     :    libhguXm: Motif Library				     *
* File        :    HGU_XmMiscProcs.c					     *
* $Revision$
******************************************************************************
* Author Name :     Richard Baldock					     *
* Author Login:    richard@hgu.mrc.ac.uk				     *
* Date        :    Thu Aug 25 09:55:25 1994				     *
* Synopsis:	Miscellaneous procedures, too small to split up:	     *
*	HGU_XmIsViewable:	test if a widget is currently viewable	     *
*	HGU_XmGetShellOffsets:	get offset positions of a shell widget	     *
*				within the WM window			     *
*****************************************************************************/
#include <stdio.h>

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>

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
        int                     X, Y;
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
    int			x, y, x0, y0;

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
  Visual	*visual;
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

