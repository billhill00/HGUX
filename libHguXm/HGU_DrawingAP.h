#pragma ident "MRC HGU $Id$"

/************************************************************************
* Project:	MRC HGU image processing utilities - X library		*
* Title:	HGU_DrawingAP.h						*
* Author:	Richard Baldock, MRC Human Genetics Unit		*
* Copyright:	Medical Research Council Human Genetics Unit		*
* Date:		Thu May 12 09:52:22 BST 1994				*
* Synopsis:	Private header file for the HGU_DrawingArea widget	*
************************************************************************/

#ifndef _HGU_DrawingAP_h
#define _HGU_DrawingAP_h

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
