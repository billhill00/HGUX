#ifndef HGUGL_QUATERNION_H
#define HGUGL_QUATERNION_H
#pragma ident "MRC HGU $Id"
/************************************************************************
* Project:	Mouse Atlas
* Title:	HGUglQuaternion.h
* Date: 	April 1999
* Author:	Bill Hill
* Copyright:	1999 Medical Research Council, UK.
*		All rights reserved.
* Address:	MRC Human Genetics Unit,
*		Western General Hospital,
*		Edinburgh, EH4 2XU, UK.
* Purpose:	Data structures, macros and function prototypes for
*		manipulating quaternions in the MRC HGU OpenGL
*		library.
* $Revision$
* Maintenance:	Log changes below, with most recent at top of list.
************************************************************************/
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
