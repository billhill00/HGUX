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
* File        :    HGU_XMiscProcs.c					     *
* $Revision$
******************************************************************************
* Author Name :    Richard Baldock					     *
* Author Login:    richard@hgu.mrc.ac.uk				     *
* Date        :    Wed Sep 14 18:51:27 1994				     *
* Synopsis    :    X miscellaneous procedures				     *
*****************************************************************************/
#include <stdio.h>

#include <X11/Xlib.h>

int QueryInWindow(
Display		*dpy,
Window		win)
{
	XWindowAttributes	win_att;
	Window			root, child;
	int			X, Y, x, y;
	unsigned int		keys;

	/* get position within parent */
        if( XGetWindowAttributes(dpy, win, &win_att) == 0 )
		return( 0 );

	if( XQueryPointer(dpy, win, &root, &child, &X, &Y, &x, &y, &keys)
			== False )
		return( 0 );

	if( (x > 0) && (x < win_att.width) && (y > 0) && (y < win_att.height) )
		return( 1 );
	return( 0 );
}

int QueryRootPosition(
Display		*dpy,
Window		win,
int		*x,
int		*y)
{
	int			X, Y;
	unsigned int		nchildren;
	XWindowAttributes	win_att;
	Window			root, parent, *children;

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
	QueryRootPosition(dpy, parent, &X, &Y);
	*x += X;
	*y += Y;

	return( 0 );
}
