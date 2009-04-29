#if defined(__GNUC__)
#ident "MRC HGU $Id:"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id:"
#else static char _HGUglShape_c[] = "MRC HGU $Id:";
#endif
#endif
/*!
* \file         HGUglShape.c
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Wed Apr 29 11:15:18 2009
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
* \brief        Functions for drawing simple OpenGL shapes in
*		the MRC HGU OpenGL library.
*               
*
* Maintenance log with most recent changes at top of list.
*/

#include <stdlib.h>
#include <math.h>
#include <Xm/XmAll.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <Wlz.h>
#include <HGUgl.h>

/************************************************************************
* Function:	HGUglShapeWireAxes				
* Returns:	void						
* Purpose:	Draws a set of wire axes of length 1.0 * scale factor,
*		center (0,0,0),	each with a wire arrow tip and optional
*		x,y,z label.					
* Global refs:	-						
* Parameters:	HGUglTextFont *fnt:	HGU gl text font instance.
*		WlzDVertex3 scale:	Scale factor for axes.	
*		char *xLabel:		X axis label if non NULL.
*		char *yLabel:		Y axis label if non NULL.
*		char *zLabel:		Z axis label if non NULL.
************************************************************************/
void		HGUglShapeWireAxes(HGUglTextFont *fnt,
				   WlzDVertex3 scale,
			           char *xLabel, char *yLabel, char *zLabel)
{
  glBegin(GL_LINES);
  glVertex3d(0.00, 0.00, 0.00);
  glVertex3d(0.90 * scale.vtX, 0.00, 0.00);
  glEnd();
  glBegin(GL_LINE_LOOP);
  glVertex3d(scale.vtX, 0.00, 0.00);
  glVertex3d(0.90 * scale.vtX, -0.05 * scale.vtY, 0.00);
  glVertex3d(0.90 * scale.vtX, 0.05 * scale.vtY, 0.00);
  glEnd();
  glBegin(GL_LINE_LOOP);
  glVertex3d(scale.vtX, 0.00, 0.00);
  glVertex3d(0.90 * scale.vtX, 0.00, -0.05 * scale.vtZ);
  glVertex3d(0.90 * scale.vtX, 0.00, 0.05 * scale.vtZ);
  glEnd();
  if(fnt && xLabel)
  {
    glRasterPos3d(scale.vtX, 0.0, 0.0);
    HGUglTextFontStr(fnt, xLabel);
  }
  glBegin(GL_LINES);
  glVertex3d(0.00, 0.00, 0.00);
  glVertex3d(0.00, 0.90 * scale.vtY, 0.00);
  glEnd();
  glBegin(GL_LINE_LOOP);
  glVertex3d(0.00, scale.vtY, 0.00);
  glVertex3d(0.00, 0.90 * scale.vtY, -0.05 * scale.vtZ);
  glVertex3d(0.00, 0.90 * scale.vtY, 0.05 * scale.vtZ);
  glEnd();
  glBegin(GL_LINE_LOOP);
  glVertex3d(0.00, scale.vtY, 0.00);
  glVertex3d(-0.05 * scale.vtX, 0.90 * scale.vtY, 0.00);
  glVertex3d(0.05 * scale.vtX, 0.90 * scale.vtY, 0.00);
  glEnd();
  if(fnt && yLabel)
  {
    glRasterPos3d(0.0, scale.vtY, 0.0);
    HGUglTextFontStr(fnt, yLabel);
  }
  glBegin(GL_LINES);
  glVertex3d(0.00, 0.00, 0.00);
  glVertex3d(0.00, 0.00, 0.90 * scale.vtZ);
  glEnd();
  glBegin(GL_LINE_LOOP);
  glVertex3d(0.00, 0.00, scale.vtZ);
  glVertex3d(-0.05 * scale.vtX, 0.00, 0.90 * scale.vtZ);
  glVertex3d(0.05 * scale.vtX, 0.00, 0.90 * scale.vtZ);
  glEnd();
  glBegin(GL_LINE_LOOP);
  glVertex3d(0.00, 0.00, scale.vtZ);
  glVertex3d(0.00, -0.05 * scale.vtY, 0.90 * scale.vtZ);
  glVertex3d(0.00, 0.05 * scale.vtY, 0.90 * scale.vtZ);
  glEnd();
  if(fnt && zLabel)
  {
    glRasterPos3d(0.0, 0.0, scale.vtZ);
    HGUglTextFontStr(fnt, zLabel);
  }
}

/************************************************************************
* Function:	HGUglShapeWireCube				
* Returns:	void						
* Purpose:	Draws a wire cube with the given scale and center.
* Global refs:	-						
* Parameters:	WlzDVertex3 side:	Length of sides.	
*		WlzDVertex3 center:	Center of the cube.	
************************************************************************/
void		HGUglShapeWireCube(WlzDVertex3 side, WlzDVertex3 center)
{
  WlzDVertex3	min,
  		max;

  WLZ_VTX_3_SCALE(side, side, 0.5);
  WLZ_VTX_3_SUB(min, center, side);
  WLZ_VTX_3_ADD(max, center, side);
  glBegin(GL_LINE_LOOP);
  glVertex3d(min.vtX, max.vtY, min.vtZ);
  glVertex3d(max.vtX, max.vtY, min.vtZ);
  glVertex3d(max.vtX, min.vtY, min.vtZ);
  glVertex3d(min.vtX, min.vtY, min.vtZ);
  glEnd();
  glBegin(GL_LINE_LOOP);
  glVertex3d(min.vtX, max.vtY, max.vtZ);
  glVertex3d(max.vtX, max.vtY, max.vtZ);
  glVertex3d(max.vtX, min.vtY, max.vtZ);
  glVertex3d(min.vtX, min.vtY, max.vtZ);
  glEnd();
  glBegin(GL_LINES);
  glVertex3d(min.vtX, max.vtY, max.vtZ);
  glVertex3d(min.vtX, max.vtY, min.vtZ);
  glVertex3d(max.vtX, max.vtY, max.vtZ);
  glVertex3d(max.vtX, max.vtY, min.vtZ);
  glVertex3d(max.vtX, min.vtY, max.vtZ);
  glVertex3d(max.vtX, min.vtY, min.vtZ);
  glVertex3d(min.vtX, min.vtY, max.vtZ);
  glVertex3d(min.vtX, min.vtY, min.vtZ);
  glEnd();
}

/************************************************************************
* Function:	HGUglShapeSolidCube				
* Returns:	void						
* Purpose:	Draws a solid cube of side using the given side length
*		and center.					
* Global refs:	-						
* Parameters:	WlzDVertex3 side:	Length of sides.	
*		WlzDVertex3 center:	Center of the cube.	
************************************************************************/
void		HGUglShapeSolidCube(WlzDVertex3 side, WlzDVertex3 center)
{
  WlzDVertex3	min,
  		max;

  WLZ_VTX_3_SCALE(side, side, 0.5);
  WLZ_VTX_3_SUB(min, center, side);
  WLZ_VTX_3_ADD(max, center, side);
  glBegin(GL_QUADS);
  glVertex3d(min.vtX, max.vtY, min.vtZ);
  glVertex3d(max.vtX, max.vtY, min.vtZ);
  glVertex3d(max.vtX, min.vtY, min.vtZ);
  glVertex3d(min.vtX, min.vtY, min.vtZ);
  glVertex3d(min.vtX, max.vtY, max.vtZ);
  glVertex3d(max.vtX, max.vtY, max.vtZ);
  glVertex3d(max.vtX, min.vtY, max.vtZ);
  glVertex3d(min.vtX, min.vtY, max.vtZ);
  glEnd();
  glBegin(GL_QUAD_STRIP);
  glVertex3d(min.vtX, min.vtY, max.vtZ);
  glVertex3d(min.vtX, min.vtY, min.vtZ);
  glVertex3d(min.vtX, max.vtY, max.vtZ);
  glVertex3d(min.vtX, max.vtY, min.vtZ);
  glVertex3d(max.vtX, max.vtY, max.vtZ);
  glVertex3d(max.vtX, max.vtY, min.vtZ);
  glVertex3d(max.vtX, min.vtY, max.vtZ);
  glVertex3d(max.vtX, min.vtY, min.vtZ);
  glVertex3d(min.vtX, min.vtY, max.vtZ);
  glVertex3d(min.vtX, min.vtY, min.vtZ);
  glEnd();
}
