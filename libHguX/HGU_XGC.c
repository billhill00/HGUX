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
* File        :    HGU_XGC.c						     *
* $Revision$
******************************************************************************
* Author Name :    Richard Baldock					     *
* Author Login:    richard@hgu.mrc.ac.uk				     *
* Date        :    Wed Sep 14 18:55:22 1994				     *
* Synopsis    :    Convenience routines to manipulate graphics contexts	     *
*****************************************************************************/
#include <stdio.h>

#include <X11/Xlib.h>

#include <HGU_XUtils.h>

static XGCValues		gcvalues;

GC HGU_XCreateGC(
Display	*dpy,
Window	win)
{
    GC 		gc;
    int		screen_num = DefaultScreen(dpy);
    Window	root = RootWindow(dpy, screen_num);

    if( win == (Window) 0 )
	win = root;

    gcvalues.function	= GXcopy;
    gcvalues.foreground	= (unsigned long) 0xffffff;
    gcvalues.plane_mask	= (unsigned long) 0xffffff;
    gcvalues.arc_mode	= ArcChord;

    gc = XCreateGC(dpy, win, (GCFunction | GCPlaneMask |
			      GCArcMode | GCForeground), &gcvalues);

    return( gc );
}

GC HGU_XColourGC(
Display	*dpy,
GC	gc,
int	col)
{
    gcvalues.plane_mask = col;
    gcvalues.foreground = col;
    XChangeGC(dpy, gc, (GCPlaneMask|GCForeground), &gcvalues);

    return( gc );
}

GC HGU_XColourFromNameGC(
Display		*dpy,
Colormap	cmap,
GC		gc,
char		*colstr)
{
    XColor	colorcell_def;
    
    if( !XParseColor( dpy, cmap, colstr, &colorcell_def ) )
	return( HGU_XColourGC(dpy, gc,
			      (int) BlackPixel(dpy, DefaultScreen(dpy))) );

    if( !XAllocColor( dpy, cmap, &colorcell_def ) )
	return( HGU_XColourGC(dpy, gc,
			      (int) BlackPixel(dpy, DefaultScreen(dpy))) );

    gcvalues.plane_mask = 255;
    gcvalues.foreground = colorcell_def.pixel;
    XChangeGC(dpy, gc, (GCPlaneMask|GCForeground), &gcvalues);

    return( gc );
}

GC HGU_XLwidthGC(
Display	*dpy,
GC	gc,
int	w)
{
    gcvalues.line_width = w;
    XChangeGC(dpy, gc, GCLineWidth, &gcvalues);

    return( gc );
}

GC HGU_XIntensGC(
Display	*dpy,
GC	gc,
int	intensity)
{
    if( intensity )
	XSetFunction(dpy, gc, GXcopy);
    else
	XSetFunction(dpy, gc, GXclear);

    return( gc );
}
