#ifndef HGUGL_SHAPE_H
#define HGUGL_SHAPE_H
#pragma ident "MRC HGU $Id"
/************************************************************************
* Project:	Mouse Atlas
* Title:	HGUglShape.h
* Date: 	April 1999
* Author:	Bill Hill
* Copyright:	1999 Medical Research Council, UK.
*		All rights reserved.
* Address:	MRC Human Genetics Unit,
*		Western General Hospital,
*		Edinburgh, EH4 2XU, UK.
* Purpose:	Functions for drawing simple OpenGL shapes in
*		the MRC HGU OpenGL library.
* $Revision$
* Maintenance:	Log changes below, with most recent at top of list.
************************************************************************/

extern void	HGUglShapeSolidCube(WlzDVertex3, WlzDVertex3),
		HGUglShapeWireAxes(HGUglTextFont *, WlzDVertex3,
				   char *, char *, char *),
		HGUglShapeWireCube(WlzDVertex3, WlzDVertex3);

#endif /* HGUGL_SHAPE_H */
