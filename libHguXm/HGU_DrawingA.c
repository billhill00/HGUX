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
* File        :    HGU_DrawingA.c					     *
* $Revision$
******************************************************************************
* Author Name :     Richard Baldock					     *
* Author Login:    richard@hgu.mrc.ac.uk				     *
* Date        :    Thu Aug 25 09:46:09 1994				     *
* Synopsis    :    Subclass the Motif DrawingAreaWidget in order to add a    *
*                  new resource - the visual. This allows the embeding	     *
*                  of a drawing window with a different depth to the	     *
*                  default.						     *
*****************************************************************************/

#include <X11/Xlib.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include <HGU_DrawingAP.h>

/* new resources */
#define Offset(field) XtOffset(HGU_DrawingAreaWidget, field)

static XtResource resources[] = {
  {XtNvisual, XtCVisual, XtRVisual, sizeof(Visual *),
     Offset(hgu_drawing_area.visual), XtRString, NULL},
};

/* methods to be redefined */
static void	Realize(HGU_DrawingAreaRec 	*w,
			XtValueMask 		*value_mask,
			XSetWindowAttributes 	*set_win_att);

static Boolean	SetValues(HGU_DrawingAreaRec	*current,
			  HGU_DrawingAreaRec	*request,
			  HGU_DrawingAreaRec	*new,
			  ArgList		args,
			  Cardinal		*num_args);

HGU_DrawingAreaClassRec hgu_DrawingAreaClassRec = {
  { /* core_class fields */
    /* superclass         */    (WidgetClass) &xmDrawingAreaClassRec,
    /* class_name         */    "HGU_DrawingArea",
    /* widget_size        */    sizeof(HGU_DrawingAreaRec),
    /* class_initialize   */    NULL,
    /* class_part_init    */    NULL,
    /* class_inited       */    FALSE,
    /* initialize         */    NULL,
    /* initialize_hook    */    NULL,
    /* realize            */    (XtRealizeProc) Realize,
    /* actions            */    NULL,
    /* num_actions        */    0,
    /* resources          */    resources,
    /* num_resources      */    XtNumber(resources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion    */    TRUE,
    /* compress_exposure  */    TRUE,
    /* compress_enterleave*/    TRUE,
    /* visible_interest   */    FALSE,
    /* destroy            */    NULL,
    /* resize             */    XtInheritResize,
    /* expose             */    XtInheritExpose,
    /* set_values         */    (XtSetValuesFunc) SetValues,
    /* set_values_hook    */    NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */    NULL,
    /* accept_focus       */    NULL,
    /* version            */    XtVersion,
    /* callback_private   */    NULL,
    /* tm_table           */    XtInheritTranslations,
    /* query_geometry     */	XtInheritQueryGeometry,
    /* display_accelerator*/	XtInheritDisplayAccelerator,
    /* extension          */	NULL
  },
  { /* composite_class fields */
    /* geometry_manager   */   XtInheritGeometryManager,
    /* change_managed     */   NULL,
    /* insert_child       */   XtInheritInsertChild,
    /* delete_child       */   XtInheritDeleteChild,
    /* extension          */   NULL
  },
  { /* constraint_class fields */
    /* subresourses       */   NULL,
    /* subresource_count  */   0,
    /* constraint_size    */   sizeof(XmManagerConstraintPart),
    /* initialize         */   NULL,
    /* destroy            */   NULL,
    /* set_values         */   NULL,
    /* extension          */   NULL
  },
  { /* manager_class fields */
    /* translations	           */	XtInheritTranslations,
    /* syn_resources               */	NULL,
    /* num_syn_resources           */	0,
    /* syn_constraint_resources    */	NULL,
    /* num_syn_constraint_resources*/	0,
    /* parent_process              */	XmInheritParentProcess,
    /* extension                   */	NULL
  },
  { /* drawing_area_class fields */
    /* mumble             */   0
  },
  { /* hgu_drawing_area_class fields */
    /* empty              */   0
  }
};

WidgetClass hgu_DrawingAreaWidgetClass = (WidgetClass)&hgu_DrawingAreaClassRec;

/* new methods */

static void Realize(
HGU_DrawingAreaRec	*w,
XtValueMask		*value_mask,
XSetWindowAttributes	*set_win_att)
{
    Display		*display = XtDisplay( w );
    Window		parent = XtWindow(w->core.parent);
    XWindowAttributes 	win_att;
    Widget		parentShW;
    Status		stat;
    int			idx, returnCount;
    Window		*returnWin;

    /* if the visual == NULL then use the parent class realize method */
    if( w->hgu_drawing_area.visual == NULL )
    {
	(*(xmDrawingAreaClassRec.core_class.realize))
	    ((Widget) w, value_mask, set_win_att);
	return;
    }

    /* check if a colormap is required */
    XGetWindowAttributes(display, parent, &win_att);
    if( w->hgu_drawing_area.visual == win_att.visual )
	w->core.colormap = win_att.colormap;
    else
	w->core.colormap = XCreateColormap(display, parent,
					   w->hgu_drawing_area.visual,
					   AllocNone);

    /* create the window */
    set_win_att->colormap          = w->core.colormap;
    set_win_att->background_pixel  = w->core.background_pixel;
    set_win_att->border_pixel      = w->core.border_pixel;
    *value_mask = CWColormap|CWBackPixel|CWBorderPixel|CWEventMask;

    w->core.window = XCreateWindow(display, parent,
				   w->core.x, w->core.y,
				   w->core.width, w->core.height,
				   w->core.border_width, w->core.depth,
				   InputOutput,
				   w->hgu_drawing_area.visual,
				   *value_mask, set_win_att);

    if( w->core.colormap != win_att.colormap ){
      parentShW = XtParent(w);
      while(parentShW && (XtIsShell(parentShW) == FALSE))
      {
	parentShW = XtParent(parentShW);
      }
      if(parentShW && XtWindow(parentShW))
      {
	stat = XGetWMColormapWindows(XtDisplay(parentShW),
				     XtWindow(parentShW),
				     &returnWin, &returnCount);
      
	if(!stat) 			  /* If no property, just create one */
	{
	  Window windows[2];
	  windows[0] = XtWindow(w);
	  windows[1] = XtWindow(parentShW);
	  XSetWMColormapWindows(XtDisplay(parentShW),
				XtWindow(parentShW),
				windows, 2);
	}
	else 		    /* There is a property, add one to the beginning */
	{
	  Window *windows = (Window *)XtMalloc((sizeof(Window))*
					       (returnCount + 1));
	  windows[0] = XtWindow(w);
	  for(idx = 0; idx < returnCount; ++idx)
	  {
	    windows[idx + 1] = returnWin[idx];
	  }
	  XSetWMColormapWindows(XtDisplay(parentShW),
				XtWindow(parentShW),
				windows, returnCount + 1);
	  XtFree((char *)windows);
	  XtFree((char *)returnWin);
	}
      }
    }

    return;
}

static Boolean SetValues(
HGU_DrawingAreaRec	*current,
HGU_DrawingAreaRec	*request,
HGU_DrawingAreaRec	*new,
ArgList			args,
Cardinal		*num_args)
{
    int             i;
 
    if( current || request ){/* to satisfy IRIX646 compiler */}

    for (i = 0; i < *num_args; i++) {
 
	/* check if visual argument */
	if( strcmp( XtNvisual, args[i].name) == 0 ){
	    new->hgu_drawing_area.visual = (Visual *)args[i].value;
	}

    }

    return( False );
}
