#pragma ident "MRC HGU $Id$"
/*****************************************************************************
* Copyright   :    1994 Medical Research Council, UK.                        *
*                  All rights reserved.                                      *
******************************************************************************
* Address     :    MRC Human Genetics Unit,                                  *
*                  Western General Hospital,                                 *
*                  Edinburgh, EH4 2XU, UK.                                   *
******************************************************************************
* Project     :    libhguXm: Motif Library				     *
* File        :    HGU_DrawingA.h					     *
* $Revision$
******************************************************************************
* Author Name :     Richard Baldock					     *
* Author Login:    richard@hgu.mrc.ac.uk				     *
* Date        :    Thu Aug 25 09:48:44 1994				     *
* Synopsis    : Public header file for HGU_DrawingArea widget		     *
*****************************************************************************/
#ifndef _HGU_DrawingA_h
#define _HGU_DrawingA_h


/* Additional resources:

 Name	     	   Class		RepType		Default Value
 ----	     	   -----		-------		-------------
 XtNvisual         XtCVisual            XtRVisual       CopyFromParent
*/

typedef struct _HGU_DrawingAreaClassRec	*HGU_DrawingAreaWidgetClass;
typedef struct _HGU_DrawingAreaRec	*HGU_DrawingAreaWidget;

extern WidgetClass hgu_DrawingAreaWidgetClass;

#endif /* _HGU_DrawingA_h */
/* DON'T ADD STUFF AFTER THIS #endif */
