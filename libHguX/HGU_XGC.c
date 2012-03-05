#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _HGU_XGC_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         HGU_XGC.c
* \author       Richard Baldock
* \date         April 2009
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
* \brief        Convenience routines to manipulate graphics contexts
*/

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
    gcvalues.plane_mask = 0xffffff;
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

    gcvalues.plane_mask = 0xffffff;
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
