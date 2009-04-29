#if defined(__GNUC__)
#ident "MRC HGU $Id:"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id:"
#else static char _HGU_XGetDomainUtils_c[] = "MRC HGU $Id:";
#endif
#endif
/*!
* \file         HGU_XGetDomainUtils.c
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Wed Apr 29 08:23:14 2009
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
* \brief        Additional utilitie functions for interact routines
*               manipulating domains.
* \todo         -
* \bug          None known
*
* Maintenance log with most recent changes at top of list.
*/

#include <stdio.h>
#include <stdlib.h>

#include <math.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <HGU_XInteract.h>

#define stipple_width 8
#define stipple_height 8
static unsigned char stipple_bits[] = {
    0x49, 0x22, 0x94, 0x01, 0x92, 0x48, 0x01, 0x24};
/*    0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa};*/
/*   0x11, 0x00, 0x42, 0x00, 0x24, 0x00, 0x88, 0x02};*/

void HGU_XDispDomain(
  Display		*dpy,
  Window		win,
  GC		gc,
  WlzObject	*obj)
{
  WlzIntervalWSpace	iwsp;

  /* check object */
  if( obj == NULL || obj->type != WLZ_2D_DOMAINOBJ || obj->domain.i == NULL )
    return;

  /* display each interval */
  WlzInitRasterScan(obj, &iwsp, WLZ_RASTERDIR_ILIC);
  while( WlzNextInterval( &iwsp ) == WLZ_ERR_NONE ){
    XFillRectangle(dpy, win, gc, iwsp.lftpos, iwsp.linpos,
		   iwsp.rgtpos-iwsp.lftpos+1, 1);
  }
  XFlush( dpy );
  return;
}

void HGU_XUndispDomain(
  Display	*dpy,
  Window	win,
  Pixmap	pixmap,
  GC		gc,
  WlzObject	*obj)
{
  WlzIntervalWSpace	iwsp;

  /* check object */
  if( obj == NULL || obj->type != WLZ_2D_DOMAINOBJ || obj->domain.i == NULL )
    return;

  /* display each interval */
  WlzInitRasterScan(obj, &iwsp, WLZ_RASTERDIR_ILIC);
  while( WlzNextInterval( &iwsp ) == WLZ_ERR_NONE ){
    XCopyArea(dpy, pixmap, win, gc, iwsp.lftpos, iwsp.linpos,
	      iwsp.rgtpos - iwsp.lftpos + 1, 1, iwsp.lftpos, iwsp.linpos);
  }
  XFlush( dpy );
  return;
}

WlzObject *HGU_XIncrementDomain(
  Display	*dpy,
  Window	win,
  Pixmap	pixmap,
  GC		gc,
  WlzObject	*obj,
  WlzObject	*obj_incr)
{
  WlzObject	*newobj;
  WlzValues	values;

  values.core = NULL;

  /* check increment object */
  if( obj_incr == NULL || WlzArea(obj_incr, NULL) == 0 ){
    if( obj != NULL )
      return( WlzMakeMain(WLZ_2D_DOMAINOBJ, obj->domain, values,
			  NULL, NULL, NULL) );
    return( NULL );
  }

  if( obj == NULL || WlzArea(obj, NULL) == 0 ){
    newobj = WlzMakeMain(WLZ_2D_DOMAINOBJ, obj_incr->domain,
			 values, NULL, NULL, NULL);
    HGU_XDispDomain(dpy, win, gc, newobj);
  } else {
    newobj = WlzDiffDomain(obj_incr, obj, NULL);
    HGU_XDispDomain(dpy, win, gc, newobj);
    if( newobj != NULL )
      WlzFreeObj(newobj);
    newobj = WlzUnion2( obj, obj_incr, NULL );
  }

  if( newobj )
  {
    (void) WlzStandardIntervalDomain( newobj->domain.i );
  }
  return( newobj );
}

WlzObject *HGU_XDecrementDomain(
  Display	*dpy,
  Window	win,
  Pixmap	pixmap,
  GC		gc,
  WlzObject	*obj,
  WlzObject	*obj_incr)
{
  WlzObject	*newobj;
  WlzValues	values;

  values.core = NULL;

  /* check increment object */
  if( obj_incr == NULL || WlzArea(obj_incr, NULL) == 0 ){
    if( obj != NULL )
      return( WlzMakeMain(WLZ_2D_DOMAINOBJ, obj_incr->domain,
			  values, NULL, NULL, NULL) );
    return( NULL );
  }

  if( obj == NULL || WlzArea(obj, NULL) == 0 ){
    return( NULL );
  }
  newobj = WlzIntersect2(obj, obj_incr, NULL);
  HGU_XUndispDomain(dpy, win, pixmap, gc, newobj);
  if( newobj != NULL )
    WlzFreeObj(newobj);
  newobj = WlzDiffDomain(obj, obj_incr, NULL);

  if( newobj )
  {
    (void) WlzStandardIntervalDomain( newobj->domain.i );
  }
  return( newobj );
}

GC HGU_XCreateDomainGC(
  Display			*dpy,
  Window			win,
  HGU_XDomainStyleStruct	*style,
  Pixmap			*stipple)
{
  XGCValues        	gcvalues;
  unsigned int	width, height;
  int			x_hot, y_hot;

  /* get domain gc */
  gcvalues.function       = GXcopy;
  gcvalues.foreground     = style->colour;
  gcvalues.background     = 0;
  gcvalues.plane_mask	    = style->plane_mask;
  gcvalues.subwindow_mode = IncludeInferiors;
  gcvalues.line_width	    = 0;
  gcvalues.line_style	    = LineOnOffDash;
  gcvalues.dashes         = 2;
  gcvalues.fill_style     = FillStippled;
  gcvalues.stipple        = (Pixmap) 0;

  if( style->stipple != (Pixmap) 0 )
  {
    gcvalues.stipple = style->stipple;
    *stipple = (Pixmap) 0;
  }
  else if( style->stipple_file != NULL )
  {
    switch( XReadBitmapFile(dpy, win, style->stipple_file,
			    &width, &height, stipple, &x_hot, &y_hot) ){
    default:
      HGU_XError(dpy, win, "HGU_XGetDomain",
		 "can't read or invalid stipple bitmap file", 0);
      *stipple = (Pixmap) 0;
      break;
    case BitmapSuccess:
      break;
    }
    gcvalues.stipple = *stipple;
  }
    
  if( gcvalues.stipple == (Pixmap) 0 )
  {
    *stipple = XCreateBitmapFromData(dpy, win, (char *) stipple_bits,
				     stipple_height, stipple_width);
    gcvalues.stipple = *stipple;
  }

  return( XCreateGC(dpy, win, (GCFunction | GCForeground | GCBackground |
			       GCPlaneMask |
			       GCSubwindowMode | GCLineWidth | GCLineStyle |
			       GCDashList | GCFillStyle | GCStipple),
		    &gcvalues) );
}

WlzObject *HGU_XDoubleObj(
  WlzObject *obj)
{
  return WlzIntRescaleObj(obj, 2, 1, NULL);
}

WlzObject *HGU_XHalveObj(
  WlzObject *obj)
{
  return WlzIntRescaleObj(obj, 2, 0, NULL);
}

WlzObject *HGU_XMag2Object(
  WlzObject	*obj,
  int		x_centre,
  int		y_centre,
  unsigned int	width,
  unsigned int	height,
  int		magnify)
{
  WlzObject	*mag_obj;
  int		line_del, kol_del;

  /* check input object */
  if( obj == NULL || obj->type != WLZ_2D_DOMAINOBJ )
    return( NULL );

  /* double or halve the object */
  if( magnify ){
    if( (mag_obj = HGU_XDoubleObj( obj )) == NULL )
      return( NULL );

    /* calculate the required shift */
    kol_del = (2 * (obj->domain.i->kol1   - x_centre) + (int) width) -
      mag_obj->domain.i->kol1;
    line_del = (2 * (obj->domain.i->line1  - y_centre) + (int) height) -
      mag_obj->domain.i->line1;
  } else {
    if( (mag_obj = HGU_XHalveObj( obj )) == NULL )
      return( NULL );

    /* calculate the required shift */
    kol_del = (2 * x_centre + obj->domain.i->kol1 - (int) width) / 2 -
      mag_obj->domain.i->kol1;
    line_del = (2 * y_centre + obj->domain.i->line1 - (int) height) / 2 - 
      mag_obj->domain.i->line1;
  }
  mag_obj->domain.i->kol1   += kol_del;
  mag_obj->domain.i->lastkl += kol_del;
  mag_obj->domain.i->line1  += line_del;
  mag_obj->domain.i->lastln += line_del;

  return( mag_obj );
}

WlzObject *HGU_XCheckConstraints(
  WlzObject 		*obj,
  HGU_XGridConstraint 	*constraint)
{
  WlzObject	*obj1, *obj2;
  WlzValues	values;
  int		kmin, kmax, lmin, lmax, width;
  int		Kmin, Kmax, Lmin, Lmax;
  int		line, kol, i, j;
  WlzUByte		*vals;
  WlzPixelV	bckgrnd;

  /* check the object */
  if( obj == NULL || obj->type != WLZ_2D_DOMAINOBJ )
    return( NULL );

  /* check the constraint */
  values.core = NULL;
  if( constraint == NULL )
    return WlzMakeMain(obj->type, obj->domain, values, NULL, NULL, NULL);

  if( constraint->sx < 0.0 )
    constraint->sx = -1.0 * constraint->sx;

  if( constraint->sy < 0.0 )
    constraint->sy = -1.0 * constraint->sy;

  if( (constraint->sx < 1.01) && (constraint->sy < 1.01) )
    return WlzMakeMain(obj->type, obj->domain, values, NULL, NULL, NULL);

  /* make a bounding rectangular object */
  lmin = obj->domain.i->line1 - constraint->sy;
  lmax = obj->domain.i->lastln + constraint->sy;
  kmin = obj->domain.i->kol1 - constraint->sx;
  kmax = obj->domain.i->lastkl + constraint->sx;
  width = kmax - kmin + 1;
  vals = (WlzUByte *) AlcCalloc((lmax - lmin + 1) * width, sizeof(char));
  bckgrnd.type = WLZ_GREY_UBYTE;
  bckgrnd.v.ubv = (WlzUByte) 0;
  obj1 = WlzMakeRect(lmin, lmax, kmin, kmax, WLZ_GREY_UBYTE,
		     (int *) vals, bckgrnd, NULL, NULL, NULL);

  /* find bounds of "true-size" object */
  Kmin = (obj->domain.i->kol1 - constraint->tx) / constraint->sx - 1;
  Kmax = (obj->domain.i->lastkl - constraint->tx) / constraint->sx + 1;
  Lmin = (obj->domain.i->line1 - constraint->ty) / constraint->sy - 1;
  Lmax = (obj->domain.i->lastln - constraint->ty) / constraint->sy + 1;

  /* scan reduced scale region */
  for(j=Lmin; j <= Lmax; j++){
    line = constraint->ty + j * constraint->sy;
    for(i=Kmin; i < Kmax; i++){
      kol = constraint->tx + i * constraint->sx;
      if( WlzInsideDomain(obj, 0.0, (double) (line+constraint->sy/2),
			  (double) (kol+constraint->sx/2), NULL) ){
	int l, k;
	for(l=0; l < WLZ_NINT(constraint->sy); l++){
	  for(k=0; k < WLZ_NINT(constraint->sx); k++){
	    vals[(kol-kmin+k) + (line-lmin+l)*width] = 10;
	  }
	}
      }
    }
  }
	
  /* threshold object */
  bckgrnd.v.ubv = (WlzUByte) 5;
  obj2 = WlzThreshold( obj1, bckgrnd, WLZ_THRESH_HIGH, NULL );
  WlzFreeObj( obj1 );
  AlcFree( (void *) vals );

  return( obj2 );
}
