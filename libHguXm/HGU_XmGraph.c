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
* File        :    HGU_XmGraph.c					     *
* $Revision$
******************************************************************************
* Author Name :     Richard Baldock					     *
* Author Login:    richard@hgu.mrc.ac.uk				     *
* Date        :    Thu Aug 25 09:51:33 1994				     *
* Synopsis    : Defines a graph, graphical interface element - not a 	     *
*		widget.							     *
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <X11/cursorfont.h>

#include <Xm/DrawingA.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/LabelG.h>

#include <HGU_XmUtils.h>

typedef struct {
  /* resources */
  String	x_str;		/* string to display on y-axis		*/
  String	y_str;		/* string to display on y-axis		*/
  float	x_min;		/* minimum x value			*/
  float	x_max;		/* maximum x value			*/
  float	y_min;		/* minimum y value			*/
  float	y_max;		/* maximum y value			*/
  String	x_format;	/* value label format			*/
  String	y_format;	/* value label format			*/
  XtCallbackList	x_position;	/* callback on change of x	*/
  XtCallbackList	y_position;	/* callback on change of y	*/
  int		markers;	/* to distinguish different lines:
					NONE|DASHES|COLOURS|MARKERS	*/
  int		style;		/* graph style - GRAPH|HISTOGRAM	*/
  Pixel	cols[6];       	/* line colours	*/

    /* private data */
  Widget	x_strW;		/* widget to show x_str			*/
  Widget	y_strW;		/* widget to show y_str			*/
  Widget	x_valW;		/* widget to show current x value	*/
  Widget	y_valW;		/* widget to show current y value	*/
  Widget	x_minW;		/* widget to show min x value		*/
  Widget	x_maxW;		/* widget to show max x value		*/
  Widget	y_minW;		/* widget to show min y value		*/
  Widget	y_maxW;		/* widget to show max y value		*/
  Widget	graphicsW;	/* simple widget for the graphics	*/
  WlzPolygonDomain **polys;	/* polyline list		*/
  int		npolys;		/* number of polylines			*/
  int		x;		/* current x-hair pos */
  float		X;		/* current x-hair pos */
  int		y;		/* current y-hair pos */
  float		Y;		/* current y-hair pos */
} GraphResources;
    
#define Offset(field) XtOffsetOf(GraphResources, field)

static float	x_min = 0.0;
static float	y_min = 0.0;
static float	x_max = 100.0;
static float	y_max = 100.0;

static XtResource resources[] = {
  {HGU_Nx_str, XtCLabel, XtRString, sizeof(String),
     Offset(x_str), XtRString, "x"},
  {HGU_Ny_str, XtCLabel, XtRString, sizeof(String),
     Offset(y_str), XtRString, "y"},
  {HGU_Nx_min, XtCValue, XtRFloat, sizeof(float),
     Offset(x_min), XtRFloat, (caddr_t) &x_min},
  {HGU_Nx_max, XtCValue, XtRFloat, sizeof(float),
     Offset(x_max), XtRFloat, (caddr_t) &x_max},
  {HGU_Ny_min, XtCValue, XtRFloat, sizeof(float),
     Offset(y_min), XtRFloat, (caddr_t) &y_min},
  {HGU_Ny_max, XtCValue, XtRFloat, sizeof(float),
     Offset(y_max), XtRFloat, (caddr_t) &y_max},
  {HGU_Nx_format, HGU_CFormat, XtRString, sizeof(String),
     Offset(x_format), XtRString, "%4.0f"},
  {HGU_Ny_format, HGU_CFormat, XtRString, sizeof(String),
     Offset(y_format), XtRString, "%4.0f"},
  {HGU_Nx_position, XtCCallback, XtRCallback, sizeof(caddr_t),
     Offset(x_position), XtRCallback, NULL},
  {HGU_Ny_position, XtCCallback, XtRCallback, sizeof(caddr_t),
     Offset(y_position), XtRCallback, NULL},
  {HGU_Nmarkers, XtCValue, XtRInt, sizeof(int),
     Offset(markers), XtRImmediate, (caddr_t) HGU_XmNONE},
  {HGU_Nstyle, XtCValue, XtRInt, sizeof(int),
     Offset(style), XtRImmediate, (caddr_t) HGU_XmGRAPH},
  {HGU_Ncol1, XtCColor, XtRPixel, sizeof(Pixel),
     Offset(cols[0]), XtRString, "yellow"},
  {HGU_Ncol2, XtCColor, XtRPixel, sizeof(Pixel),
     Offset(cols[1]), XtRString, "cyan"},
  {HGU_Ncol3, XtCColor, XtRPixel, sizeof(Pixel),
     Offset(cols[2]), XtRString, "magenta"},
  {HGU_Ncol4, XtCColor, XtRPixel, sizeof(Pixel),
     Offset(cols[3]), XtRString, "red"},
  {HGU_Ncol5, XtCColor, XtRPixel, sizeof(Pixel),
     Offset(cols[4]), XtRString, "green"},
  {HGU_Ncol6, XtCColor, XtRPixel, sizeof(Pixel),
     Offset(cols[5]), XtRString, "blue"},
};

static void	X_Hair(Widget		widget,
		       XtPointer	client_data,
		       XtPointer	call_data);

static void	Y_Hair(Widget		widget,
		       XtPointer	client_data,
		       XtPointer	call_data);

static void	GraphUpdate(Widget	widget,
			    XtPointer	client_data,
			    XtPointer	call_data);

static int	get_event_incr(XEvent *event);
static int	get_event_x(XEvent *event);
static int	get_event_y(XEvent *event);

static GC	hair_gc=(GC) 0,
		line_gc=(GC) 0;

static void HGU_XmCreateGraphGCs(
  Display	*dpy,
  Drawable	drawable)
{
  XGCValues       	gcvalues;
  unsigned int    	valuemask;

  /* create the hair-line and graph graphics contexts */
  /* use the passed window for the drawable  */
  gcvalues.function = GXinvert;
  valuemask = GCFunction;
  hair_gc = XCreateGC(dpy, drawable,
		      valuemask, &gcvalues);
  gcvalues.function = GXcopy;
  line_gc = XCreateGC(dpy, drawable,
		      valuemask, &gcvalues);

  return;
}

Widget HGU_XmCreateGraph(
String		name,
Widget		parent,
XtPointer	data)
{
  Drawable	drawable;

  drawable = RootWindowOfScreen(XtScreen(parent));
  return HGU_XmCreateGraphD(name, parent, data, drawable);
}

Widget HGU_XmCreateGraphD(
String		name,
Widget		parent,
XtPointer	data,
Drawable	drawable)
{
    Widget		graph, form_x, form_y, filler, frame;
    GraphResources	*graph_res;
    char		str[128];
    XmString		xmstr;
    XGCValues       	gcvalues;
    unsigned int    	valuemask;
    Dimension		width, height;

    if( data ){/* to satisfy the IRIX646 compiler */}

    /* create the base widget - XmForm and get resources */
    graph = XtVaCreateWidget(name, xmFormWidgetClass, parent,
			     XmNhorizontalSpacing,	(Dimension) 3,
			     XmNverticalSpacing,	(Dimension) 3,
			     NULL);
    graph_res = (GraphResources *) malloc(sizeof(GraphResources));
    XtGetApplicationResources(graph, graph_res, resources,
			      XtNumber(resources), NULL, 0);
    XtVaSetValues(graph, XmNuserData, graph_res, NULL);

    /* create the sub_part manager widgets */
    form_x = XtVaCreateWidget("x_form", xmFormWidgetClass, graph,
			    XmNfractionBase,	100,
			    NULL);
    form_y = XtVaCreateWidget("y_form", xmFormWidgetClass, graph,
			    XmNfractionBase,	100,
			    NULL);
    frame = XtVaCreateWidget("frame", xmFrameWidgetClass, graph, NULL);

    XtVaSetValues(form_x,
		  XmNbottomAttachment,	XmATTACH_FORM,
		  XmNleftAttachment,	XmATTACH_WIDGET,
		  XmNrightAttachment,	XmATTACH_FORM,
		  XmNleftWidget,	form_y,
		  XmNleftOffset,	-10,
		  XmNborderWidth,	0,
		  NULL);

    XtVaSetValues(form_y,
		  XmNtopAttachment,	XmATTACH_FORM,
		  XmNbottomAttachment,	XmATTACH_FORM,
		  XmNleftAttachment,	XmATTACH_FORM,
		  XmNborderWidth,	0,
		  NULL);

    XtVaSetValues(frame,
		  XmNtopAttachment,	XmATTACH_FORM,
		  XmNbottomAttachment,	XmATTACH_WIDGET,
		  XmNleftAttachment,	XmATTACH_WIDGET,
		  XmNrightAttachment,	XmATTACH_FORM,
		  XmNbottomWidget,	form_x,
		  XmNleftWidget,	form_y,
		  XmNborderWidth,	0,
		  NULL);

    /* create the label widgets */
    (void) sprintf(str,graph_res->x_format, graph_res->x_min);
    xmstr = XmStringCreateSimple( str );
    graph_res->x_minW =
	XtVaCreateManagedWidget("x_min",
				xmLabelGadgetClass, form_x,
				XmNlabelString,	xmstr,
				XmNborderWidth,	0,
				XmNleftAttachment,	XmATTACH_POSITION,
				XmNleftPosition,	0,
				NULL);
    XmStringFree( xmstr );

    (void) sprintf(str,graph_res->x_format, graph_res->x_min);
    xmstr = XmStringCreateSimple( str );
    graph_res->x_valW = 
	XtVaCreateManagedWidget("x_val",
				xmLabelGadgetClass, form_x,
				XmNlabelString,	xmstr,
				XmNborderWidth,	0,
				XmNleftAttachment,	XmATTACH_POSITION,
				XmNleftPosition,	20,
				NULL);
    XmStringFree( xmstr );

    xmstr = XmStringCreateLtoR( graph_res->x_str, XmSTRING_DEFAULT_CHARSET );
    graph_res->x_strW =
	XtVaCreateManagedWidget("x_str",
				xmLabelGadgetClass, form_x,
				XmNlabelString,	xmstr,
				XmNborderWidth,	0,
				XmNleftAttachment,	XmATTACH_POSITION,
				XmNleftPosition,	60,
				NULL);
    XmStringFree( xmstr );

    (void) sprintf(str,graph_res->x_format, graph_res->x_max);
    xmstr = XmStringCreateSimple( str );
    graph_res->x_maxW =
	XtVaCreateManagedWidget("x_max",
				xmLabelGadgetClass, form_x,
				XmNlabelString,	xmstr,
				XmNborderWidth,	0,
				XmNrightAttachment,	XmATTACH_POSITION,
				XmNrightPosition,	100,
				NULL);
    XmStringFree( xmstr );

    (void) sprintf(str,graph_res->y_format, graph_res->y_max);
    xmstr = XmStringCreateSimple( str );
    graph_res->y_maxW =
	XtVaCreateManagedWidget("y_max",
				xmLabelGadgetClass, form_y,
				XmNlabelString,	xmstr,
				XmNborderWidth,	0,
				XmNtopAttachment,	XmATTACH_POSITION,
				XmNtopPosition,		0,
				XmNleftAttachment,	XmATTACH_FORM,
				XmNrightAttachment,	XmATTACH_FORM,
				XmNalignment,		XmALIGNMENT_END,
				NULL);
    XmStringFree( xmstr );

    xmstr = XmStringCreateLtoR( graph_res->y_str, XmSTRING_DEFAULT_CHARSET );
    graph_res->y_strW =
	XtVaCreateManagedWidget("y_str",
				xmLabelGadgetClass, form_y,
				XmNlabelString,	xmstr,
				XmNborderWidth,	0,
				XmNtopAttachment,	XmATTACH_POSITION,
				XmNtopPosition,		20,
				XmNleftAttachment,	XmATTACH_FORM,
				XmNrightAttachment,	XmATTACH_FORM,
				XmNalignment,		XmALIGNMENT_END,
				NULL);
    XmStringFree( xmstr );

    (void) sprintf(str,graph_res->y_format, graph_res->y_min);
    xmstr = XmStringCreateSimple( str );
    graph_res->y_valW =
	XtVaCreateManagedWidget("y_val",
				xmLabelGadgetClass, form_y,
				XmNlabelString,	xmstr,
				XmNborderWidth,	0,
				XmNtopAttachment,	XmATTACH_POSITION,
				XmNtopPosition,		45,
				XmNleftAttachment,	XmATTACH_FORM,
				XmNrightAttachment,	XmATTACH_FORM,
				XmNalignment,		XmALIGNMENT_END,
				NULL);
    XmStringFree( xmstr );

    filler =
	XtVaCreateManagedWidget("  ",
				xmLabelGadgetClass, form_y,
				XmNborderWidth,	0,
				XmNbottomAttachment,	XmATTACH_POSITION,
				XmNbottomPosition,	100,
				XmNleftAttachment,	XmATTACH_FORM,
				XmNrightAttachment,	XmATTACH_FORM,
				NULL);

    (void) sprintf(str,graph_res->y_format, graph_res->y_min);
    xmstr = XmStringCreateSimple( str );
    graph_res->y_minW =
	XtVaCreateManagedWidget("y_min",
				xmLabelGadgetClass, form_y,
				XmNlabelString,	xmstr,
				XmNborderWidth,	0,
				XmNbottomAttachment,	XmATTACH_WIDGET,
				XmNbottomWidget,	filler,
				XmNleftAttachment,	XmATTACH_FORM,
				XmNrightAttachment,	XmATTACH_FORM,
				XmNalignment,		XmALIGNMENT_END,
				NULL);
    XmStringFree( xmstr );

    /* create the graphics widget */
    graph_res->graphicsW =
	XtVaCreateManagedWidget("graphics",
				xmDrawingAreaWidgetClass,	frame,
				XmNborderWidth, 		0,
				NULL);
    XtAddCallback(graph_res->graphicsW, XmNexposeCallback, GraphUpdate,
		  graph_res);
    XtAddCallback(graph_res->graphicsW, XmNresizeCallback, GraphUpdate,
      graph_res);
    XtAddCallback(graph_res->graphicsW, XmNinputCallback, X_Hair, graph_res);
    XtAddCallback(graph_res->graphicsW, XmNinputCallback, Y_Hair, graph_res);
    graph_res->x = 0;
    graph_res->X = graph_res->x_min;
    graph_res->y = 0;
    graph_res->Y = graph_res->y_min;

    /* create the hair-line and graph graphics contexts */
    /* use the passed window for the drawable  */
    if( drawable && (hair_gc == (GC) 0) ){
      HGU_XmCreateGraphGCs(XtDisplay(graph), drawable);
    }

    /* initialise the polyline list */
    graph_res->polys  = NULL;
    graph_res->npolys = 0;

    /* manage the widgets */
    XtManageChild( form_x );
    XtManageChild( form_y );
    XtManageChild( frame );

    XtManageChild( graph );
    return( graph );
}


/* callback procedures */
#define MAXGRPTS	256
static XPoint points[MAXGRPTS];

static Cursor	cursor = (Cursor) 0;

static void SetCrossCursor(
Display         *dpy,
Window          win)
{
        if( !cursor  )
                cursor = XCreateFontCursor(dpy, XC_crosshair);
        XDefineCursor( dpy, win, cursor );
}

static void GraphUpdate(
Widget		w,
XtPointer	client_data,
XtPointer	call_data)
{
    GraphResources	*graph_res = (GraphResources *) client_data;
    int			i, j, k=0;
    float		x_scale, x_offset, y_scale, y_offset;
    Dimension		width, height;

    if( call_data ){ /* to satisfy IRIX646 */ }

    /* check if realized */
    if( XtIsRealized(graph_res->graphicsW) != True ){
	return;
    }
    SetCrossCursor( XtDisplay(w), XtWindow(graph_res->graphicsW) );
    if( hair_gc == (GC) 0 ){
      HGU_XmCreateGraphGCs(XtDisplay(graph_res->graphicsW),
			   XtWindow(graph_res->graphicsW));
    }

    /* clear the graphics window */
    XClearWindow( XtDisplay(w), XtWindow(graph_res->graphicsW) );
    X_Hair(w, graph_res, NULL);
    Y_Hair(w, graph_res, NULL);

    /* display the graphs */
    XtVaGetValues(graph_res->graphicsW, XmNwidth, &width,
		  XmNheight, &height, NULL);
    x_scale = (int) width / (graph_res->x_max - graph_res->x_min);
    x_offset = (-1.0) * x_scale * graph_res->x_min;
    y_scale = (int) height / (graph_res->y_min - graph_res->y_max);
    y_offset = (-1.0) * y_scale * graph_res->y_max;
    for(i=0; i < graph_res->npolys; i++){
	XSetForeground(XtDisplay(w), line_gc, graph_res->cols[i%6]);
	if( graph_res->polys[i]->type == 1 ){
	    for(j=0, k=0; j < graph_res->polys[i]->nvertices; j++){
		points[k].x = (float)graph_res->polys[i]->vtx[j].vtX *
		    x_scale + x_offset;
		points[k].y = (float)graph_res->polys[i]->vtx[j].vtY *
		    y_scale + y_offset;
		k++;
		if( k == MAXGRPTS ){
		    XDrawLines(XtDisplay(w), XtWindow(graph_res->graphicsW),
			       line_gc, points, k, CoordModeOrigin);
		    points[0] = points[k-1];
		    k = 1;
		}
	    }
	}
	else if( graph_res->polys[i]->type == 2 ){
	    for(j=0, k=0; j < graph_res->polys[i]->nvertices; j++){
		points[k].x = ((WlzFVertex2 *)
			       (graph_res->polys[i]->vtx))[j].vtX *
				   x_scale + x_offset;
		points[k].y = ((WlzFVertex2 *)
			       (graph_res->polys[i]->vtx))[j].vtY *
				   y_scale + y_offset;
		k++;
		if( k == MAXGRPTS ){
		    XDrawLines(XtDisplay(w), XtWindow(graph_res->graphicsW),
			       line_gc, points, k, CoordModeOrigin);
		    points[0] = points[k-1];
		    k = 1;
		}
	    }
	}
	else
	    return;

	if( k )
	    XDrawLines(XtDisplay(w), XtWindow(graph_res->graphicsW),
		       line_gc, points, k, CoordModeOrigin);
    }
    XFlush( XtDisplay(w) );
}

static void X_Hair(
Widget		widget,
XtPointer	client_data,
XtPointer	call_data)
{
    XmDrawingAreaCallbackStruct
	*cbs = (XmDrawingAreaCallbackStruct *) call_data;
    GraphResources	*graph_res = (GraphResources *) client_data;
    Dimension		w, h;
    int			x;
    float		X;
    char		str[64];
    XmString		xmstr;

    if( widget ){ /* to satisfy IRIX646 compiler */ }

    /* check if realized */
    if( XtIsRealized(graph_res->graphicsW) != True )
	return;

    if( hair_gc == (GC) 0 ){
      HGU_XmCreateGraphGCs(XtDisplay(graph_res->graphicsW),
			   XtWindow(graph_res->graphicsW));
    }

    /* find graphics window size */
    XtVaGetValues(graph_res->graphicsW, XmNwidth, &w, XmNheight, &h, NULL);
    x = graph_res->x;
    X = graph_res->X;

    if( cbs == NULL )  
	x = (X - graph_res->x_min) * (int) w / (graph_res->x_max - graph_res->x_min);
    else {
	/* remove the old line */
	XDrawLine(XtDisplay(graph_res->graphicsW),
		  XtWindow(graph_res->graphicsW), hair_gc,
		  x, 0, x, h);

	/* find position */
	if((cbs->event->type == KeyPress)||(cbs->event->type == KeyRelease)){
	  x += get_event_incr( cbs->event );
	}
	else {
	  x = get_event_x( cbs->event );
	}
    }

    /* draw the new line */
    XDrawLine(XtDisplay(graph_res->graphicsW),
	      XtWindow(graph_res->graphicsW), hair_gc,
	      x, 0, x, h);

    /* update the value widgets */
    if( cbs ){
      X = ((float) x) / (int) w * (graph_res->x_max - graph_res->x_min) +
	graph_res->x_min;
      X =  (X < graph_res->x_min) ? graph_res->x_min : X;
      X =  (X > graph_res->x_max) ? graph_res->x_max : X;
      sprintf(str, graph_res->x_format, X);
      xmstr = XmStringCreateSimple( str );
      XtVaSetValues(graph_res->x_valW, XmNlabelString, xmstr, NULL);
      XmStringFree( xmstr );
    }
    graph_res->x = x;
    graph_res->X = X;
}

static void Y_Hair(
Widget		widget,
XtPointer	client_data,
XtPointer	call_data)
{
    XmDrawingAreaCallbackStruct
	*cbs = (XmDrawingAreaCallbackStruct *) call_data;
    GraphResources	*graph_res = (GraphResources *) client_data;
    Dimension		w, h;
    int			y;
    float		Y;
    char		str[64];
    XmString		xmstr;

    if( widget ){ /* to satisfy IRIX646 compiler */ }

    /* check if realized */
    if( XtIsRealized(graph_res->graphicsW) != True )
	return;

    if( hair_gc == (GC) 0 ){
      HGU_XmCreateGraphGCs(XtDisplay(graph_res->graphicsW),
			   XtWindow(graph_res->graphicsW));
    }

    /* find graphics window size */
    XtVaGetValues(graph_res->graphicsW, XmNwidth, &w, XmNheight, &h, NULL);
    y = graph_res->y;
    Y = graph_res->Y;

    if( cbs == NULL )
	y = (Y - graph_res->y_max) * (int) h / (graph_res->y_min - graph_res->y_max);
    else {
	/* remove the old line */
	XDrawLine(XtDisplay(graph_res->graphicsW),
		  XtWindow(graph_res->graphicsW), hair_gc,
		  0, y, w, y);

	/* find position */
	y = get_event_y( cbs->event );
    }

    /* draw the new line */
    XDrawLine(XtDisplay(graph_res->graphicsW),
	      XtWindow(graph_res->graphicsW), hair_gc,
	      0, y, w, y);

    /* update the value widgets */
    if( cbs ){
      Y = ((float) y) / (int) h * (graph_res->y_min - graph_res->y_max) +
	graph_res->y_max;
      Y =  (Y < graph_res->y_min) ? graph_res->y_min : Y;
      Y =  (Y > graph_res->y_max) ? graph_res->y_max : Y;
      sprintf(str, graph_res->y_format, Y);
      xmstr = XmStringCreateSimple( str );
      XtVaSetValues(graph_res->y_valW, XmNlabelString, xmstr, NULL);
      XmStringFree( xmstr );
    }
    graph_res->y = y;
    graph_res->Y = Y;
}

/* miscellaneous procedures */
static int get_event_incr(XEvent *event)
{
  int 	incr = 0;

  if( event ){
    switch( event->type ){
    case KeyPress:
      switch(  XLookupKeysym(&(event->xkey), 0) ){

      case XK_Right:
      case XK_KP_Right:
      case XK_f:
      case XK_Up:
      case XK_KP_Up:
      case XK_p:
	incr = 1;
	break;

      case XK_Left:
      case XK_KP_Left:
      case XK_b:
      case XK_Down:
      case XK_KP_Down:
      case XK_n:
	incr = -1;
	break;

      default:
	incr = 0;
	break;
      }
      break;
	  
    default:
      incr = 0;
    }
  }

  return incr;
}
static int get_event_x(XEvent *event)
{
        switch( event->type ){
        case ButtonPress:
        case ButtonRelease:
                return( event->xbutton.x );
        case KeyPress:
        case KeyRelease:
                return( event->xkey.x );
        case MotionNotify:
                return( event->xmotion.x );
        case EnterNotify:
                return( event->xcrossing.x );
        case LeaveNotify:
                return( -1 );
        default:
                return( 0 );
        }
}
static int get_event_y(XEvent *event)
{
        switch( event->type ){
        case ButtonPress:
        case ButtonRelease:
                return( event->xbutton.y );
        case KeyPress:
        case KeyRelease:
                return( event->xkey.y );
        case MotionNotify:
                return( event->xmotion.y );
        case EnterNotify:
                return( event->xcrossing.y );
        case LeaveNotify:
                return( -1 );
        default:
                return( 0 );
        }
}

/* public access procedures */
/* Public Procedures */
WlzPolygonDomain *HGU_XmGetGraphPolyline(
Widget  w,
int     n)
{
    GraphResources	*graph_res;

    XtVaGetValues(w, XmNuserData, &graph_res, NULL);
 
    /* check if n valid */
    if( (n < 1) || (n > graph_res->npolys) )
	return( NULL );
    
    return( graph_res->polys[n-1] );
}

int HGU_XmSetGraphPolyline(
Widget  		w,
WlzPolygonDomain 	*poly,
int     		n)
{
    GraphResources	*graph_res;
    WlzPolygonDomain    **newpolys;
    int             i;
 

    XtVaGetValues(w, XmNuserData, &graph_res, NULL);
 
    /* check polyline number */
    if( n < 1 )
	return( 0 );
 
    /* make space for the new polyline if required */
    if( n > graph_res->npolys ){
	graph_res->npolys++;
	n = graph_res->npolys;
	newpolys = (WlzPolygonDomain **)
	    malloc(sizeof(WlzPolygonDomain *) * n);
	for(i=0; i < n-1; i++)
	    newpolys[i] = graph_res->polys[i];
	newpolys[n-1] = NULL;
	if( graph_res->polys != NULL )
	    free( graph_res->polys );
	graph_res->polys = newpolys;
    }
 
    /* free off the old polyline */
    if( graph_res->polys[n-1] != NULL )
	free( graph_res->polys[n-1] );
 
    /* copy the new */
    i = (int) (sizeof(WlzPolygonDomain) + (sizeof(WlzIVertex2) *
					       poly->nvertices));
    graph_res->polys[n-1] = (WlzPolygonDomain *) malloc( i );
    *graph_res->polys[n-1] = *poly;
    graph_res->polys[n-1]->vtx = (WlzIVertex2 *) 
	(graph_res->polys[n-1] + 1);
    for(i=0; i < poly->nvertices; i++)
	graph_res->polys[n-1]->vtx[i] = poly->vtx[i];
 
    GraphUpdate(w, (XtPointer) graph_res, NULL);
    return( n );
}

int HGU_XmSetGraphLimits(
Widget  	w,
float   	xl, 
float 		xu, 
float 		yl, 
float 		yu)
{
    GraphResources	*graph_res;
    char            	str[64];
    XmString		xmstr;
 
    XtVaGetValues(w, XmNuserData, &graph_res, NULL);
 
    graph_res->x_min = xl;
    graph_res->x_max = xu;
    graph_res->y_min = yl;
    graph_res->y_max = yu;
 
    sprintf(str, graph_res->x_format, graph_res->x_min);
    xmstr = XmStringCreateSimple( str );
    XtVaSetValues(graph_res->x_minW, XmNlabelString, xmstr, NULL);
    XmStringFree( xmstr );

    sprintf(str, graph_res->x_format, graph_res->x_max);
    xmstr = XmStringCreateSimple( str );
    XtVaSetValues(graph_res->x_maxW, XmNlabelString, xmstr, NULL);
    XmStringFree( xmstr );

    sprintf(str, graph_res->y_format, graph_res->y_min);
    xmstr = XmStringCreateSimple( str );
    XtVaSetValues(graph_res->y_minW, XmNlabelString, xmstr, NULL);
    XmStringFree( xmstr );

    sprintf(str, graph_res->y_format, graph_res->y_max);
    xmstr = XmStringCreateSimple( str );
    XtVaSetValues(graph_res->y_maxW, XmNlabelString, xmstr, NULL);
    XmStringFree( xmstr );
    XFlush(XtDisplay(w));

    GraphUpdate(w, (XtPointer) graph_res, NULL);
    return( 0 );
}

int HGU_XmGetGraphLimits(
  Widget  	w,
  float   	*xl, 
  float 		*xu, 
  float 		*yl, 
  float 		*yu)
{
    GraphResources	*graph_res;
 
    XtVaGetValues(w, XmNuserData, &graph_res, NULL);
 
    if( graph_res == NULL ){
      return 1;
    }

    *xl = graph_res->x_min;
    *xu = graph_res->x_max;
    *yl = graph_res->y_min;
    *yu = graph_res->y_max;

    return 0;
}

int HGU_XmSetHairCursor(
  Widget  	w,
  float		X,
  float		Y)
{
    GraphResources	*graph_res;
 
    XtVaGetValues(w, XmNuserData, &graph_res, NULL);
 
    if( graph_res == NULL ){
      return 1;
    }

    graph_res->X = X;
    graph_res->Y = Y;
    GraphUpdate(w, (XtPointer) graph_res, NULL);

    return 0;
}

int HGU_XmGetHairCursor(
  Widget  	w,
  float		*X,
  float		*Y)
{
    GraphResources	*graph_res;
 
    XtVaGetValues(w, XmNuserData, &graph_res, NULL);
 
    if( graph_res == NULL ){
      return 1;
    }

    *X = graph_res->X;
    *Y = graph_res->Y;
    GraphUpdate(w, (XtPointer) graph_res, NULL);

    return 0;
}
