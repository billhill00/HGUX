#ifndef HGUGL_SHAPE_H
#define HGUGL_SHAPE_H
#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _libhguGL/HGUglShape_h[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         HGUglShape.h
* \author       Bill Hill
* \date         Wed Apr 29 11:10:30 2009
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
* \ingroup      HGU_GL
* \brief       	Functions for drawing simple OpenGL shapes in
*		the MRC HGU OpenGL library.
*/

extern void			HGUglShapeSolidCube(
				  WlzDVertex3,
				  WlzDVertex3);
extern void			HGUglShapeWireAxes(
				  HGUglTextFont *,
				  WlzDVertex3,
				  char *,
				  char *,
				  char *);
extern void			HGUglShapeWireCube(
				  WlzDVertex3,
				  WlzDVertex3);

#endif /* HGUGL_SHAPE_H */
