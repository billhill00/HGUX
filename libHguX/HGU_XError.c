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
* File        :    HGU_XError.c						     *
* $Revision$
******************************************************************************
* Author Name :    Richard Baldock					     *
* Author Login:    richard@hgu.mrc.ac.uk				     *
* Date        :    Mon Sep 19 17:19:56 1994				     *
* Synopsis    :    X error handling routines to allow callbacks to a GUI     *
*****************************************************************************/
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
int	errno)
{
    HGU_XErrorStruct	errstruct;

    if( error_func == NULL ){
	fprintf(stderr, "%s: %s\n\terrno = %d\n", srcstr, errstr, errno);
	XBell( dpy, 100 );
	return( 0 );
    }

    errstruct.srcstr = srcstr;
    errstruct.errstr = errstr;
    errstruct.errno  = errno;
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
