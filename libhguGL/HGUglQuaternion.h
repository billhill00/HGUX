#ifndef HGUGL_QUATERNION_H
#define HGUGL_QUATERNION_H

#if defined(__GNUC__)
#ident "MRC HGU $Id:"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id:"
#else static char _HGUglQuaternion.h[] = "MRC HGU $Id:";
#endif
#endif
/*!
* \file         HGUglQuaternion.h
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Wed Apr 29 11:10:51 2009
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
* \ingroup      HGU_GL
* \brief        Data structures, macros and function prototypes for
*		manipulating quaternions in the MRC HGU OpenGL
*               
*
* Maintenance log with most recent changes at top of list.
*/

#include <float.h>
#include <Wlz.h>
#include <HGUglQuaternion.h>

typedef struct 
{
  double	qW;
  WlzDVertex3	qV;
} HGUglQuaternion;

#define HGUgl_QUAT_SET(Q,W,V) \
		(Q).qW = (W), \
		(Q).qV = (V)

#define HGUgl_QUAT_SET4(Q,W,X,Y,Z) \
		(Q).qW = (W), \
		(Q).qV.vtX = (X), \
		(Q).qV.vtY = (Y), \
		(Q).qV.vtZ = (Z)

#define HGUgl_QUAT_ADD(Q,O,P) \
		(Q).qW = (O).qW + (P).qW, \
		WLZ_VTX_3_ADD((Q).qV, (O).qV, (P).qV)

#define HGUgl_QUAT_SUB(Q,O,P) \
		(Q).qW = (O).qW - (P).qW, \
		WLZ_VTX_3_SUB((Q).qV, (O).qV, (P).qV)

#define HGUgl_QUAT_SCALE(Q,P,S) \
		(Q).qW = (P).qW * (S), \
		WLZ_VTX_3_SCALE((Q).qV, (P).qV, S)

extern void	HGUglQuatToMatrixGL(double [4][4], HGUglQuaternion);
extern HGUglQuaternion	HGUglQuatProduct(HGUglQuaternion, HGUglQuaternion),
		HGUglQuatNormalise(HGUglQuaternion quat),
		HGUglQuatFromAxis(WlzDVertex3 axis, double phi);

#endif /* HGUGL_QUATERNION_H */
