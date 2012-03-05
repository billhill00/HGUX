#ifndef _HGU_DrawingAP_h
#define _HGU_DrawingAP_h
#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _HGU_DrawingAP_h[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         HGU_DrawingAP.h
* \author	Richard Baldock
* \date		April 2009
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
* \ingroup      HGU_Xm
* \brief        Private header file for the HGU_DrawingArea widget
*/

#include <HGU_DrawingA.h>
#include <Xm/DrawingAP.h>

typedef struct {int empty;} HGU_DrawingAreaClassPart;

typedef struct {
    CoreClassPart		core_class;
    CompositeClassPart		composite_class;
    ConstraintClassPart		constraint_class;
    XmManagerClassPart      	manager_class;
    XmDrawingAreaClassPart  	drawing_area_class;
    HGU_DrawingAreaClassPart	hgu_drawing_area_class;
} HGU_DrawingAreaClassRec;

extern HGU_DrawingAreaClassRec hgu_DrawingAreaClassRec;

typedef struct {
    /* resources */
    Visual		*visual;
    String		visual_str;
} HGU_DrawingAreaPart;

typedef struct _HGU_DrawingAreaRec {
    CorePart			core;
    CompositePart		composite;
    ConstraintPart		constraint;
    XmManagerPart           	manager;
    XmDrawingAreaPart       	drawing_area;
    HGU_DrawingAreaPart		hgu_drawing_area;
} HGU_DrawingAreaRec;

#endif /* _HGU_DrawingAP_h */
