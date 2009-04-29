#if defined(__GNUC__)
#ident "MRC HGU $Id:"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id:"
#else static char _HGUglQuaternion_c[] = "MRC HGU $Id:";
#endif
#endif
/*!
* \file         HGUglQuaternion.c
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Wed Apr 29 11:15:40 2009
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
*		library. Quaternions are used to encode rotations
*		about an axis in the HGUglwCanvasTb widget. In
*		comparison with matricies, quaternions require one
*		quarter the data storage and are much more efficient.
*		Useful references:
*		  * Shoemake K. Animating Rotation with Quaternion
*		    Curves, SIGRAPH (1985) 19 245-254.
*		  * Birkhoff G, MacLane S. A survey of Modern
*		    Algebra, Macmillan (1966).
*               
*
* Maintenance log with most recent changes at top of list.
*/

#include <math.h>
#include <float.h>
#include <Wlz.h>
#include <HGUglQuaternion.h>


/************************************************************************
* Function:	HGUglQuatProduct				
* Returns:	HGUglQuaternion:	Product of the two given
*					quaternions.		
* Purpose:	Calculates and returns the product of the two given
*		quaternions.					
* Global refs:	-						
* Parameters:	HGUglQuaternion given1:	First quaternion.	
*		HGUglQuaternion given2:	Second quaternion.	
************************************************************************/
HGUglQuaternion	HGUglQuatProduct(HGUglQuaternion given1,
				 HGUglQuaternion given2)
{
  HGUglQuaternion prod;
  WlzDVertex3	tVtx0,
  		tVtx1,
		tVtx2;
 
  prod.qW = (given1.qW * given2.qW) - WLZ_VTX_3_DOT(given1.qV, given2.qV);
  WLZ_VTX_3_SCALE(tVtx0, given2.qV, given1.qW);
  WLZ_VTX_3_SCALE(tVtx1, given1.qV, given2.qW);
  WLZ_VTX_3_CROSS(tVtx2, given1.qV, given2.qV);
  WLZ_VTX_3_ADD3(prod.qV, tVtx0, tVtx1, tVtx2);
  return(prod);
}

/************************************************************************
* Function:	HGUglQuatToMatrixGL				
* Returns:	void						
* Purpose:	Converts the given quaternion into an (Open GL)	
*		transformation matrix.				
*		Efficiency: 14 additions, 9 multiplies.		
* Global refs:	-						
* Parameters:	double matrix[4][4]:	Given (Open GL) matrix.	
* 		HGUglQuaternion quat:	Given quaternion.	
************************************************************************/
void		HGUglQuatToMatrixGL(double matrix[4][4], HGUglQuaternion quat)
{
  double	t2X,
		t2Y,
		t2Z,
  		t2XX,
  		t2YY,
		t2ZZ,
		t2XY,
		t2XZ,
		t2YZ,
		t2XW,
		t2YW,
		t2ZW;

  t2X = quat.qV.vtX + quat.qV.vtX;
  t2Y = quat.qV.vtY + quat.qV.vtY;
  t2Z = quat.qV.vtZ + quat.qV.vtZ;
  t2XX = t2X * quat.qV.vtX;
  t2YY = t2Y * quat.qV.vtY;
  t2ZZ = t2Z * quat.qV.vtZ;
  t2XY = t2X * quat.qV.vtY;
  t2XZ = t2X * quat.qV.vtZ;
  t2YZ = t2Y * quat.qV.vtZ;
  t2XW = t2X * quat.qW;
  t2YW = t2Y * quat.qW;
  t2ZW = t2Z * quat.qW;
  matrix[0][0] = 1.0 - t2YY - t2ZZ;
  matrix[0][1] = t2XY - t2ZW;
  matrix[0][2] = t2XZ + t2YW;
  matrix[0][3] = 0.0;
  matrix[1][0] = t2XY + t2ZW;
  matrix[1][1] = 1.0 - t2XX - t2ZZ;
  matrix[1][2] = t2YZ - t2XW;
  matrix[1][3] = 0.0;
  matrix[2][0] = t2XZ - t2YW;
  matrix[2][1] = t2YZ + t2XW;
  matrix[2][2] = 1.0 - t2XX - t2YY;
  matrix[2][3] = 0.0;
  matrix[3][0] = 0.0;
  matrix[3][1] = 0.0;
  matrix[3][2] = 0.0;
  matrix[3][3] = 1.0;
}

/************************************************************************
* Function:	HGUglQuatNormalise				
* Returns:	HGUglQuaternion:	Normalised quaternion.	
* Purpose:	Normalises the given quaternion (so that it lies on the
*		unit sphere).					
* Global refs:	-						
* Parameters:	HGUglQuaternion quat:	Given quaternion.	
************************************************************************/
HGUglQuaternion	HGUglQuatNormalise(HGUglQuaternion quat)
{
  double	tNorm;

  tNorm = sqrt(WLZ_VTX_3_SQRLEN(quat.qV) + (quat.qW * quat.qW));
  if(tNorm > DBL_EPSILON)
  {
    tNorm = 1.0 / sqrt(tNorm);
    HGUgl_QUAT_SCALE(quat, quat, tNorm);
  }
  return(quat);
}

/************************************************************************
* Function:	HGUglQuatFromAxis				
* Returns:	HGUglQuaternion:	Calculated quaternion.	
* Purpose:	Calculates a rotation quaternion (which lies on the
*		unit sphere) using the given axis and rotation about
*		that axis.					
* Global refs:	-						
* Parameters:	WlzDVertex3 axis:	Axis defined by vector throgh
*					(0, 0, 0) and this vertex.
*		double phi:		Rotation about the given axis.
************************************************************************/
HGUglQuaternion	HGUglQuatFromAxis(WlzDVertex3 axis, double phi)
{
  double	halfPhi,
  		axisLen,
		tD1;
  HGUglQuaternion quat;

  halfPhi = phi / 2.0;
  axisLen = WLZ_VTX_3_LENGTH(axis);
  if(axisLen > DBL_EPSILON)
  {
    quat.qW = cos(halfPhi);
    tD1 = sin(halfPhi) / axisLen;
    WLZ_VTX_3_SCALE(quat.qV, axis, tD1);
  }
  else
  {
    HGUgl_QUAT_SET4(quat, 1.0, 0.0, 0.0, 0.0);
    
  }
  return(quat);
}
