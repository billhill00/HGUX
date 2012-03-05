#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _HGU_XError_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         HGU_XError.c
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
* \brief         X error handling routines to allow callbacks to a GUI
*/

#include <stdio.h>

#include <X11/Xlib.h>

#include <HGU_XUtils.h>

/* private variables */
static caddr_t		error_data = NULL;
static HGU_XErrorFunc	error_func;

/* public routines */
int HGU_XError(
Display	*dpy,
Window	win,
char	*srcstr,
char	*errstr,
int	errnum)
{
    HGU_XErrorStruct	errstruct;

    if( error_func == NULL ){
	fprintf(stderr, "%s: %s\n\terrnum = %d\n", srcstr, errstr, errnum);
	XBell( dpy, 100 );
	return( 0 );
    }

    errstruct.srcstr = srcstr;
    errstruct.errstr = errstr;
    errstruct.errnum  = errnum;
    return( (*error_func)(dpy, win, error_data, (caddr_t) &errstruct) );
}

int HGU_XErrorSetFunction(
HGU_XErrorFunc	err_func,
caddr_t		err_data)
{
    error_func = err_func;
    error_data = err_data;
    return( 0 );
}

int HGU_XErrorGetFunction(
HGU_XErrorFunc	*err_func,
caddr_t		*err_data)
{
    *err_func = error_func;
    *err_data = error_data;
    return( 0 );
}
