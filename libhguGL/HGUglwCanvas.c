#pragma ident "MRC HGU $Id"
/************************************************************************
* Project:	Mouse Atlas
* Title:	HGUglwCanvas.c
* Date: 	April 1999
* Author:	Bill Hill
* Copyright:	1999 Medical Research Council, UK.
*		All rights reserved.
* Address:	MRC Human Genetics Unit,
*		Western General Hospital,
*		Edinburgh, EH4 2XU, UK.
* Purpose:	Methods for the HGUglCanvas MRC HGU OpenGL
*		widget. The HGUglCanvas widget is descended from the
*		Motif primitive widget. See the manual page
*		HGUglCanvas(3) for a description of the widget.
* $Revision$
* Maintenance:	Log changes below, with most recent at top of list.
************************************************************************/
#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <GL/glx.h>
#include <GL/gl.h>
#include <Xm/PrimitiveP.h>
#include <Wlz.h>
#include <HGUglwCallbacks.h>
#include <HGUglwCanvasP.h>

#define HGUglw_OFFSET(field) XtOffset(HGUglwCanvasWidget, field)

static void	HGUglwCreateColormap(HGUglwCanvasWidget, int, XrmValue *);

static void 	Initialize(Widget, Widget, ArgList, Cardinal *),
		Realize(Widget, Mask *, XSetWindowAttributes *),
		Redraw(Widget, XEvent *, Region),
		Resize(Widget),
		Destroy(Widget);

static char defaultTranslations[] = "<Key>osfHelp: PrimitiveHelp() \n"
				    "<KeyDown>:    HGUglwCanvasInput() \n"
				    "<KeyUp>:      HGUglwCanvasInput() \n"
				    "<BtnDown>:    HGUglwCanvasInput() \n"
				    "<BtnUp>:      HGUglwCanvasInput() \n"
				    "<BtnMotion>:  HGUglwCanvasInput() ";

static XtActionsRec actions[] =
{
  {
    "HGUglwCanvasInput", (XtActionProc )HGUglwCanvasInput
  },
};

static XtResource resources[] = 
{
  {
    HGUglwNbufferSize, HGUglwCBufferSize,
    XtRInt, sizeof(int),
    HGUglw_OFFSET(hguGLwCanvas.bufferSize),
    XtRImmediate, (XtPointer )0
  },
  {
    HGUglwNlevel, HGUglwCLevel,
    XtRInt, sizeof(int),
    HGUglw_OFFSET(hguGLwCanvas.level),
    XtRImmediate, (XtPointer )0
  },
  {
    HGUglwNrgba, HGUglwCRgba,
    XtRBoolean, sizeof(Boolean),
    HGUglw_OFFSET(hguGLwCanvas.rgba),
    XtRImmediate, (XtPointer )FALSE
  },
  {
    HGUglwNdoublebuffer, HGUglwCDoublebuffer,
    XtRBoolean, sizeof(Boolean),
    HGUglw_OFFSET(hguGLwCanvas.doublebuffer),
    XtRImmediate, (XtPointer )FALSE
  },
  {
    HGUglwNstereo, HGUglwCStereo,
    XtRBoolean, sizeof(Boolean),
    HGUglw_OFFSET(hguGLwCanvas.stereo),
    XtRImmediate, (XtPointer )FALSE
  },
  {
    HGUglwNauxBuffers, HGUglwCAuxBuffers,
    XtRInt, sizeof(int),
    HGUglw_OFFSET(hguGLwCanvas.auxBuffers),
    XtRImmediate, (XtPointer )0
  },
  {
    HGUglwNredSize, HGUglwCColorSize,
    XtRInt, sizeof(int),
    HGUglw_OFFSET(hguGLwCanvas.redSize),
    XtRImmediate, (XtPointer )1
  },
  {
    HGUglwNgreenSize, HGUglwCColorSize,
    XtRInt, sizeof(int),
    HGUglw_OFFSET(hguGLwCanvas.greenSize),
    XtRImmediate, (XtPointer )1
  },
  {
    HGUglwNblueSize, HGUglwCColorSize,
    XtRInt, sizeof(int),
    HGUglw_OFFSET(hguGLwCanvas.blueSize),
    XtRImmediate, (XtPointer )1
  },
  {
    HGUglwNalphaSize, HGUglwCAlphaSize,
    XtRInt, sizeof(int),
    HGUglw_OFFSET(hguGLwCanvas.alphaSize),
    XtRImmediate, (XtPointer )0
  },
  {
    HGUglwNdepthSize, HGUglwCDepthSize,
    XtRInt, sizeof(int),
    HGUglw_OFFSET(hguGLwCanvas.depthSize),
    XtRImmediate, (XtPointer )0
  },
  {
    HGUglwNstencilSize, HGUglwCStencilSize,
    XtRInt, sizeof(int),
    HGUglw_OFFSET(hguGLwCanvas.stencilSize),
    XtRImmediate, (XtPointer )0
  },
  {
    HGUglwNaccumRedSize, HGUglwCAccumColorSize,
    XtRInt, sizeof(int),
    HGUglw_OFFSET(hguGLwCanvas.accumRedSize),
    XtRImmediate, (XtPointer )0
  },
  {
    HGUglwNaccumGreenSize, HGUglwCAccumColorSize,
    XtRInt, sizeof(int),
    HGUglw_OFFSET(hguGLwCanvas.accumGreenSize),
    XtRImmediate, (XtPointer )0
  },
  {
    HGUglwNaccumBlueSize, HGUglwCAccumColorSize,
    XtRInt, sizeof(int),
    HGUglw_OFFSET(hguGLwCanvas.accumBlueSize),
    XtRImmediate, (XtPointer )0
  },
  {
    HGUglwNaccumAlphaSize, HGUglwCAccumAlphaSize,
    XtRInt, sizeof(int),
    HGUglw_OFFSET(hguGLwCanvas.accumAlphaSize),
    XtRImmediate, (XtPointer )0
  },
  {
    HGUglwNattribList, HGUglwCAttribList,
    XtRPointer, sizeof(int *),
    HGUglw_OFFSET(hguGLwCanvas.attribList),
    XtRImmediate, (XtPointer )NULL
  },
  {
    HGUglwNvisualInfo, HGUglwCVisualInfo,
    HGUglwRVisualInfo, sizeof(XVisualInfo *),
    HGUglw_OFFSET(hguGLwCanvas.visualInfo),
    XtRImmediate, (XtPointer )NULL
  },
  {
    HGUglwNinstallColormap, HGUglwCInstallColormap,
    XtRBoolean, sizeof(Boolean),
    HGUglw_OFFSET(hguGLwCanvas.installColormap),
    XtRImmediate, (XtPointer )TRUE
  },
  {
    HGUglwNallocateBackground, HGUglwCAllocateColors,
    XtRBoolean, sizeof(Boolean),
    HGUglw_OFFSET(hguGLwCanvas.allocateBackground),
    XtRImmediate, (XtPointer )FALSE
  },
  {
    HGUglwNallocateOtherColors, HGUglwCAllocateColors,
    XtRBoolean, sizeof(Boolean),
    HGUglw_OFFSET(hguGLwCanvas.allocateOtherColors),
    XtRImmediate, (XtPointer )FALSE
  },
  {
    HGUglwNinstallBackground, HGUglwCInstallBackground,
    XtRBoolean, sizeof(Boolean),
    HGUglw_OFFSET(hguGLwCanvas.installBackground),
    XtRImmediate, (XtPointer )TRUE
  },
  {
    HGUglwNinitCallback, HGUglwCCallback,
    XtRCallback, sizeof(XtCallbackList),
    HGUglw_OFFSET(hguGLwCanvas.initCallback),
    XtRImmediate, (XtPointer )NULL
  },
  {
    HGUglwNinputCallback, HGUglwCCallback,
    XtRCallback, sizeof(XtCallbackList),
    HGUglw_OFFSET(hguGLwCanvas.inputCallback),
    XtRImmediate, (XtPointer )NULL
  },
  {
    HGUglwNresizeCallback, HGUglwCCallback,
    XtRCallback, sizeof(XtCallbackList),
    HGUglw_OFFSET(hguGLwCanvas.resizeCallback),
    XtRImmediate, (XtPointer )NULL
  },
  {
    HGUglwNglxCtx, HGUglwCGlxCtx,
    XtRPointer, sizeof(GLXContext),
    HGUglw_OFFSET(hguGLwCanvas.glxCtx),
    XtRImmediate, (XtPointer )NULL
  },
  {
    HGUglwNdirectRender, HGUglwCDirectRender,
    XtRBoolean, sizeof(Boolean),
    HGUglw_OFFSET(hguGLwCanvas.directRender),
    XtRImmediate, (XtPointer )FALSE
  },
  {
    HGUglwNexposeCallback, HGUglwCCallback,
    XtRCallback, sizeof(XtCallbackList),
    HGUglw_OFFSET(hguGLwCanvas.exposeCallback),
    XtRImmediate, (XtPointer )NULL
  },
  {
    XmNtraversalOn, XmCTraversalOn,
    XmRBoolean, sizeof(Boolean),
    HGUglw_OFFSET(primitive.traversal_on),
    XmRImmediate, (XtPointer )FALSE
  },
  {
    XmNhighlightOnEnter, XmCHighlightOnEnter,
    XmRBoolean, sizeof(Boolean),
    HGUglw_OFFSET(primitive.highlight_on_enter),
    XmRImmediate, (XtPointer )FALSE
  },
  {
    XmNhighlightThickness, XmCHighlightThickness,
    XmRHorizontalDimension, sizeof(Dimension),
    HGUglw_OFFSET(primitive.highlight_thickness),
    XmRImmediate, (XtPointer )0
  },
};

/* reobtain cmap with new visual */
static XtResource initializeResources[] =
{
  {
    XtNcolormap, XtCColormap,
    XtRColormap, sizeof(Colormap),
    HGUglw_OFFSET(core.colormap),
    XtRCallProc,(XtPointer )HGUglwCreateColormap
  },
};

  
/* reallocate colors needed in new cmap, background only obtained if
 * the allocateBackground resource is TRUE
 */
static XtResource backgroundResources[] =
{
  {
    XmNbackground, XmCBackground,
    XmRPixel, sizeof(Pixel),
    HGUglw_OFFSET(core.background_pixel),
    XmRCallProc, (XtPointer )_XmBackgroundColorDefault
  },
  {
    XmNbackgroundPixmap, XmCPixmap,
    XmRXmBackgroundPixmap, sizeof(Pixmap),
    HGUglw_OFFSET(core.background_pixmap),
    XmRImmediate, (XtPointer )XmUNSPECIFIED_PIXMAP
  },
};

/* Other colors such as the foreground only allocated if
 * allocateOtherColors are set.
 */
static XtResource otherColorResources[] =
{
  {
    XmNforeground, XmCForeground,
    XmRPixel, sizeof(Pixel),
    HGUglw_OFFSET(primitive.foreground),
    XmRCallProc, (XtPointer )_XmForegroundColorDefault
  },
  {
    XmNhighlightColor, XmCHighlightColor, XmRPixel, sizeof(Pixel),
    HGUglw_OFFSET(primitive.highlight_color),
    XmRCallProc, (XtPointer )_XmHighlightColorDefault
  },
  {
    XmNhighlightPixmap, XmCHighlightPixmap,
    XmRPrimHighlightPixmap, sizeof(Pixmap),
    HGUglw_OFFSET(primitive.highlight_pixmap),
    XmRCallProc, (XtPointer )_XmHighlightColorDefault
  },
};

struct attribInfo
{
  int		offset;
  int		attribute;
};

static struct attribInfo intAttribs[] =
{
  {
    HGUglw_OFFSET(hguGLwCanvas.bufferSize), GLX_BUFFER_SIZE
  },
  {
    HGUglw_OFFSET(hguGLwCanvas.level), GLX_LEVEL
  },
  {
    HGUglw_OFFSET(hguGLwCanvas.auxBuffers), GLX_AUX_BUFFERS
  },
  {
    HGUglw_OFFSET(hguGLwCanvas.redSize), GLX_RED_SIZE
  },
  {
    HGUglw_OFFSET(hguGLwCanvas.greenSize), GLX_GREEN_SIZE
  },
  {
    HGUglw_OFFSET(hguGLwCanvas.blueSize), GLX_BLUE_SIZE
  },
  {
    HGUglw_OFFSET(hguGLwCanvas.alphaSize), GLX_ALPHA_SIZE
  },
  {
    HGUglw_OFFSET(hguGLwCanvas.depthSize), GLX_DEPTH_SIZE
  },
  {
    HGUglw_OFFSET(hguGLwCanvas.stencilSize), GLX_STENCIL_SIZE
  },
  {
    HGUglw_OFFSET(hguGLwCanvas.accumRedSize), GLX_ACCUM_RED_SIZE
  },
  {
    HGUglw_OFFSET(hguGLwCanvas.accumGreenSize), GLX_ACCUM_GREEN_SIZE
  },
  {
    HGUglw_OFFSET(hguGLwCanvas.accumBlueSize), GLX_ACCUM_BLUE_SIZE
  },
  {
    HGUglw_OFFSET(hguGLwCanvas.accumAlphaSize), GLX_ACCUM_ALPHA_SIZE
  },
  {
    0, None
  },
};

static struct attribInfo booleanAttribs[] =
{
  {
    HGUglw_OFFSET(hguGLwCanvas.rgba), GLX_RGBA
   },
  {
    HGUglw_OFFSET(hguGLwCanvas.doublebuffer), GLX_DOUBLEBUFFER
   },
  {
    HGUglw_OFFSET(hguGLwCanvas.stereo), GLX_STEREO
  },
  {
    0, None
  },
};

#undef HGUglw_OFFSET


HGUglwCanvasClassRec hguGLwCanvasClassRec =
{
  {
    /* Core */
    /* superclass		*/	(WidgetClass )&xmPrimitiveClassRec,
    /* class_name		*/	"HGUglwCanvas",
    /* widget_size		*/	sizeof(HGUglwCanvasRec),
    /* class_initialize		*/	NULL,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	(XtInitProc )Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	Realize,
    /* actions			*/	actions,
    /* num_actions		*/	XtNumber(actions),
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	TRUE,
    /* destroy			*/	(XtWidgetProc )Destroy,
    /* resize			*/	(XtWidgetProc )Resize,
    /* expose			*/	(XtExposeProc )Redraw,
    /* set_values		*/	NULL,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	defaultTranslations,
    /* query_geometry		*/	XtInheritQueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
  {
    /* XmPrimitive */
    /* border_highlight		*/	XmInheritBorderHighlight,
    /* border_unhighlight	*/	XmInheritBorderUnhighlight,
    /* translations		*/	XtInheritTranslations,
    /* arm_and_activate		*/	NULL,
    /* get_resources		*/	NULL,
    /* num get_resources	*/	0,
    /* extension		*/	NULL,				
  }
};

WidgetClass	hguGLwCanvasWidgetClass = (WidgetClass )&hguGLwCanvasClassRec;

/************************************************************************
* Function:	HGUglwCanvasError				
* Returns:	void						
* Purpose:	Passes appropriate message string to the installed
*		error handler.					
* Global refs:	-						
* Parameters:	Widget givenW:		Widget instance.	
*		String str:		Error message string.	
************************************************************************/
static void	HGUglwCanvasError(Widget givenW, String str)
{
  char		buf[256];

  (void )sprintf(buf, "HGUglwCanvas: %s\n", str);
  XtAppError(XtWidgetToApplicationContext(givenW), buf);
}

/************************************************************************
* Function:	HGUglwCanvasWarning				
* Returns:	void						
* Purpose:	Passes appropriate message string to the installed
*		non-fatal error handler.			
* Global refs:	-						
* Parameters:	Widget givenW:		Widget instance.	
*		String str:		Non-fatal error message string.
************************************************************************/
static void	HGUglwCanvasWarning(Widget givenW, String str)
{
  char		buf[256];

  (void )sprintf(buf, "HGUglwCanvas: %s\n", str);
  XtAppWarning(XtWidgetToApplicationContext(givenW), buf);
}

/************************************************************************
* Function:	HGUglwCreateAttribList				
* Returns:	void						
* Purpose:	Resource initialization method: Initializes the 
		attribute list based on the attributes.			*
* Global refs:	struct attribInfo booleanAttribs: Widget's boolean
*					attribute list.		
*		struct attribInfo intAttribs: Widget's int attribute
*					list.			
* Parameters:	HGUglwCanvasWidget cW:	Canvas widget instance.	
************************************************************************/
static void	HGUglwCreateAttribList(HGUglwCanvasWidget cW)
{
  int		listLength;
  struct attribInfo *ap;
  int		*ip;

  listLength = 1; 	    /* Find list length, includeing terminating NULL */
  for(ap = booleanAttribs; ap->attribute; ap++)
  {
    if(*(Boolean *)(((char *)cW)+ap->offset))
    {
      listLength += 1;				   /* One word for a boolean */
    }
  }
  for(ap = intAttribs; ap->attribute; ap++)
  {
    if(*(int *)(((char *)cW)+ap->offset))
    {
      listLength += 2;				      /* One word for an int */
    }
  }
  cW->hguGLwCanvas.attribList = (int *)XtMalloc(listLength*sizeof(int));
  ip = cW->hguGLwCanvas.attribList;
  for(ap = booleanAttribs; ap->attribute; ap++)
  {
    if(*(Boolean *)(((char *)cW)+ap->offset))
    {
      *ip++ = ap->attribute;
    }
  }
  for(ap = intAttribs; ap->attribute; ap++)
  {
    if(*(int *)(((char *)cW)+ap->offset))
    {
      *ip++ = ap->attribute;
      *ip++ = *(int *)(((char *)cW)+ap->offset);
    }
  }
  *ip = None;
}

/************************************************************************
* Function:	HGUglwCreateVisualInfo				
* Returns:	void						
* Purpose:	Resource initialization method: Initializes the 
		visualInfo based on the attribute list.			*
* Global refs:	-						
* Parameters:	HGUglwCanvasWidget cW:	Canvas widget instance.	
************************************************************************/
static void	HGUglwCreateVisualInfo(HGUglwCanvasWidget cW)
{
  cW->hguGLwCanvas.visualInfo = glXChooseVisual(XtDisplay(cW),
					    XScreenNumberOfScreen(XtScreen(cW)),
					        cW->hguGLwCanvas.attribList);
  if(cW->hguGLwCanvas.visualInfo == NULL)
  {
    HGUglwCanvasError((Widget )cW, "Requested visual not supported.");
  }
}

/************************************************************************
* Function:	HGUglwCreateColormap				
* Returns:	void						
* Purpose:	Resource initialization method: Initializes the 
*		colormap based on the visual info.		
*		This function maintains a cache of visual-infos to 
*		colormaps. If two widgets share the same visual info,
*		they share the same colormap. This function is called
*		by the callProc of the colormap resource entry.	
* Global refs:	-						
* Parameters:	HGUglwCanvasWidget cW:	Canvas widget instance.	
*		int offset:		NOT USED.		
*		XrmValue *value:	Resource value.		
************************************************************************/
static void	HGUglwCreateColormap(HGUglwCanvasWidget cW,
				     int offset, XrmValue *value)
{
  int		idx,
  		visualInCache;
  static struct cmapCache
  {
      Visual *visual;
      Colormap cmap;
  } *cmapCache;
  const int	cacheStep = 4;
  static int 	cacheEntries = 0;
  static int 	cacheMalloced = 0;
  
  idx = 0;
  visualInCache = 0;
  while((idx < cacheEntries) && (visualInCache == 0))
  {
    if(cmapCache[idx].visual == cW->hguGLwCanvas.visualInfo->visual)
    {
      value->addr = (XtPointer )(&cmapCache[idx].cmap);
      visualInCache = 1;
    }
  }
  if(visualInCache == 0)            /* Visual not in cache, create new entry */
  {
    if(cacheEntries >= cacheMalloced)
    {
      if(cacheMalloced == 0)
      {
	cmapCache = (struct cmapCache *)XtMalloc(sizeof(struct cmapCache) *
						 cacheStep);
	cacheMalloced = cacheStep;
      }
      else
      {
	cmapCache = (struct cmapCache *)XtRealloc((char *) cmapCache,
						  sizeof(struct cmapCache)*
						  cacheMalloced + cacheStep);
        cacheMalloced += cacheStep;
      }
    }
    cmapCache[cacheEntries].cmap  = XCreateColormap(XtDisplay(cW),
						    RootWindow(XtDisplay(cW),
					cW->hguGLwCanvas.visualInfo->screen),
					cW->hguGLwCanvas.visualInfo->visual,
						    AllocNone);
    cmapCache[cacheEntries].visual = cW->hguGLwCanvas.visualInfo->visual;
    value->addr = (XtPointer )(&cmapCache[cacheEntries++].cmap);
  }
}

/************************************************************************
* Function:	Initialize					
* Returns:	void						
* Purpose:	Widget Method: Initialises the HGU Open GL canvas 
*		widget.						
* Global refs:	-						
* Parameters:	Widget reqW:		Widget values as set by the
*					resource manager.	
*		Widget newW:		
*		ArgList args:
*		Cardinal *numArgs:
************************************************************************/
static void	Initialize(Widget reqW, Widget newW,
			   ArgList args, Cardinal *numArgs)
{
  HGUglwCanvasWidget	reqCW,
  			newCW;

  reqCW = (HGUglwCanvasWidget )reqW;
  newCW = (HGUglwCanvasWidget )newW;
  if(reqCW->core.width == 0)
  {
    newCW->core.width = 100;
  }
  if(reqCW->core.height == 0)
  {
    newCW->core.width = 100;
  }
  if(newCW->hguGLwCanvas.attribList == NULL)
  {
    newCW->hguGLwCanvas.myList = TRUE;
    HGUglwCreateAttribList(newCW);
  }
  else
  {
    newCW->hguGLwCanvas.myList = FALSE;
  }
  if(newCW->hguGLwCanvas.visualInfo == NULL)
  {
    newCW->hguGLwCanvas.myVisual = TRUE;
    HGUglwCreateVisualInfo(newCW);
  }
  else
  {
    newCW->hguGLwCanvas.myVisual = FALSE;
  }
  newCW->core.depth = newCW->hguGLwCanvas.visualInfo->depth;
  XtGetApplicationResources((Widget )newCW,   /* Reobtain cmap it's colors */
			    newCW, initializeResources,
			    XtNumber(initializeResources),
			    args, *numArgs);

  if(reqCW->hguGLwCanvas.allocateBackground)
  {
    XtGetApplicationResources((Widget )newCW, /* Obtain bkgrd cmap resources */
			      newCW,
			      backgroundResources,
			      XtNumber(backgroundResources),
			      args, *numArgs);
  }
  if(reqCW->hguGLwCanvas.allocateOtherColors)
  {
    XtGetApplicationResources((Widget )newCW, /* Obtain other cmap resources */
    			      newCW,
			      otherColorResources,
			      XtNumber(otherColorResources),
			      args, *numArgs);
  }
}

/************************************************************************
* Function:	Realize						
* Returns:	void						
* Purpose:	Initializes the HGU Open GL canvas widget.	
* Global refs:	-						
* Parameters:	Widget givenW:
*		Mask *valueMask:
*		XSetWindowAttributes *attributes:
************************************************************************/
static void	Realize(Widget givenW, Mask *valueMask,
			XSetWindowAttributes *attributes)
{
  int		idx,
  		returnCount;
  Window	*returnWin;
  Widget	parentShW;
  Status	stat;
  HGUglwCanvasWidget	givenCW;
  HGUglwCanvasCallbackStruct cb;

  givenCW = (HGUglwCanvasWidget )givenW;
  if(!(givenCW->hguGLwCanvas.installBackground &&
     givenCW->hguGLwCanvas.allocateBackground))
  {
    *valueMask &= ~(CWBackPixel);
  }
  XtCreateWindow(givenW, (unsigned int)InputOutput,
		 givenCW->hguGLwCanvas.visualInfo->visual,
		 *valueMask, attributes);
  if(givenCW->hguGLwCanvas.installColormap)  /* If appropriate, install cmap */
  {
    parentShW = XtParent(givenW);
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
	windows[0] = XtWindow(givenW);
	windows[1] = XtWindow(parentShW);
	XSetWMColormapWindows(XtDisplay(parentShW),
			      XtWindow(parentShW),
			      windows, 2);
      }
      else 		    /* There is a property, add one to the beginning */
      {
	Window *windows = (Window *)XtMalloc((sizeof(Window))*
					     (returnCount + 1));
	windows[0] = XtWindow(givenW);
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
    else
    {
      HGUglwCanvasWarning(givenW,
      			  "Could not set colormap property on parent shell");
    }
  }
  cb.reason = HGUglw_CR_INIT;
  cb.event = NULL;
  cb.width = givenCW->core.width;
  cb.height = givenCW->core.height;
  cb.glxCtx = givenCW->hguGLwCanvas.glxCtx;
  XtCallCallbackList(givenW,
  		     givenCW->hguGLwCanvas.initCallback,
		     &cb);
}

/************************************************************************
* Function:	Redraw						
* Returns:	void						
* Purpose:	Expose method which calls widgets callbacks.	
* Global refs:	-						
* Parameters:	Widget givenW:
*		XEvent *event:
*		Region region:
************************************************************************/
static void	Redraw(Widget givenW, XEvent *event, Region region)
{
  HGUglwCanvasWidget givenCW;
  HGUglwCanvasCallbackStruct cb;
  XtCallbackList cblist;
   
  givenCW = (HGUglwCanvasWidget )givenW;
  cb.reason = HGUglw_CR_EXPOSE;
  cb.event = event;
  cb.width = givenCW->core.width;
  cb.height = givenCW->core.height;
  cb.glxCtx = givenCW->hguGLwCanvas.glxCtx;
  XtCallCallbackList(givenW, givenCW->hguGLwCanvas.exposeCallback,
		     &cb);
}

/************************************************************************
* Function:	Resize						
* Returns:	void						
* Purpose:	Resize method which calls widgets callbacks.	
* Global refs:	-						
* Parameters:	Widget givenW:
************************************************************************/
static void	Resize(Widget givenW)
{
  HGUglwCanvasCallbackStruct cb;
  HGUglwCanvasWidget givenCW;

  givenCW = (HGUglwCanvasWidget )givenW;
  if(XtIsRealized((Widget)givenCW))
  {
    cb.reason = HGUglw_CR_RESIZE;
    cb.event = NULL;
    cb.width = givenCW->core.width;
    cb.height = givenCW->core.height;
    cb.glxCtx = givenCW->hguGLwCanvas.glxCtx;
    XtCallCallbackList(givenW,
    		       givenCW->hguGLwCanvas.resizeCallback,
		       &cb);
  }
}

/************************************************************************
* Function:	Destroy						
* Returns:	void						
* Purpose:	Destroy method which destroys the given widget.	
* Global refs:	-						
* Parameters:	Widget givenW:
************************************************************************/
static void	Destroy(Widget givenW)    
{
  Widget	parentShW;
  Status	stat;
  Window	*returnWin;
  int		idx,
		visualMatch,
  		returnCount;
  HGUglwCanvasWidget givenCW;

  givenCW = (HGUglwCanvasWidget )givenW;
  XtRemoveAllCallbacks(givenW, HGUglwNinitCallback);
  XtRemoveAllCallbacks(givenW, HGUglwNresizeCallback);
  XtRemoveAllCallbacks(givenW, HGUglwNinputCallback);
  XtRemoveAllCallbacks(givenW, HGUglwNexposeCallback);
  if(givenCW->hguGLwCanvas.myList && givenCW->hguGLwCanvas.attribList)
  {
    XtFree((XtPointer)givenCW->hguGLwCanvas.attribList);
  }
  if(givenCW->hguGLwCanvas.myVisual && givenCW->hguGLwCanvas.visualInfo)
  {
    XtFree((XtPointer)givenCW->hguGLwCanvas.visualInfo);
  }
  if(givenCW->hguGLwCanvas.installColormap) /* If installed cmap, remove it */
  {
    parentShW = XtParent(givenW);
    while(parentShW && !XtIsShell(parentShW))
    {
	parentShW = XtParent(parentShW);
    }
    if(parentShW && XtWindow(parentShW))
    {
      stat = XGetWMColormapWindows(XtDisplay(parentShW),
				   XtWindow(parentShW),
				   &returnWin, &returnCount);
      if(stat)
      {
	idx = 0;
	visualMatch = 0;
	while((idx < returnCount) && (visualMatch == 0))
	{
	  if(returnWin[idx] == XtWindow(givenCW))
	  {
	    for(++idx; idx < returnCount; ++idx)
	    {
	      returnWin[idx - 1] = returnWin[idx];
	    }
	    XSetWMColormapWindows(XtDisplay(parentShW),
				  XtWindow(parentShW),
				  returnWin, returnCount - 1);
	    visualMatch = 1;
	  }
	  ++idx;
	}
	XtFree((char *)returnWin);
      }
    }
  }
}

/************************************************************************
* Function:	HGUglwCanvasInput				
* Returns:	void						
* Purpose:	Action function for keyboard and mouse events.	
* Global refs:	-						
* Parameters:	Widget givenW:
*		XEvent *event:
*		String *params:
*		Cardinal *numParams:
************************************************************************/
void		HGUglwCanvasInput(Widget givenW, XEvent *event,
		        	  String *params, Cardinal *numParams)
{
  HGUglwCanvasCallbackStruct cb;
  HGUglwCanvasWidget givenCW;

  givenCW = (HGUglwCanvasWidget )givenW;
  cb.reason = HGUglw_CR_INPUT;
  cb.event = event;
  cb.width = givenCW->core.width;
  cb.height = givenCW->core.height;
  cb.glxCtx = givenCW->hguGLwCanvas.glxCtx;
  XtCallCallbackList(givenW,
  		     givenCW->hguGLwCanvas.inputCallback,
		     &cb);
}

/************************************************************************
* Function:	HGUglwCreateCanvas				
* Returns:	Widget:			New HGU Open GL canvas widget.
* Purpose:	Motif style convienience function to create an HGU Open
*		GL canvas widget.				
* Global refs:	-						
* Parameters:	Widget parent:		Parent widget.		
*		String name:		New widget name.	
*		ArgList argList:	Argument list for new widget.
*		Cardinal argCount:	Number of arguments in the
*					argument list.		
************************************************************************/
Widget 		HGUglwCreateCanvas(Widget parent, String name,
			 	   ArgList argList, Cardinal argCount)
{
  return(XtCreateWidget(name, hguGLwCanvasWidgetClass, parent, argList,
			argCount));
}

/************************************************************************
* Function:	HGUglwSetCanvasGlxContext			
* Returns:	void						
* Purpose:	Sets the Open GL rendering context associated with 
*		this canvas widget.				
* Global refs:	-						
* Parameters:	Widget thisW:		Instance of canvas widget.
*		GLXContext glxCtx:	GLX rendering context to set.
************************************************************************/
void     	HGUglwSetCanvasGlxContext(Widget thisW, GLXContext glxCtx)
{
  HGUglwCanvasWidget thisCW;

  thisCW = (HGUglwCanvasWidget )thisW;
  thisCW->hguGLwCanvas.glxCtx = glxCtx;
}

/************************************************************************
* Function:	HGUglwCreateCanvasGlxContext			
* Returns:	GLXContext:		The Open GL rendering context.
* Purpose:	Creates an Open GL rendering context associated with
*		this canvas widget.				
* Global refs:	-						
* Parameters:	Widget thisW:		Instance of canvas widget.
*		GLXContext shareList:	Context with which to share 
*					display lists, NULL indicates
*					no display list sharing.
************************************************************************/
GLXContext     	HGUglwCreateCanvasGlxContext(Widget thisW,
					     GLXContext shareList)
{
  GLXContext	glxCtx;
  HGUglwCanvasWidget thisCW;

  thisCW = (HGUglwCanvasWidget )thisW;
  glxCtx = glXCreateContext(XtDisplay(thisW),
  			    thisCW->hguGLwCanvas.visualInfo,
  			    shareList, thisCW->hguGLwCanvas.directRender);
  thisCW->hguGLwCanvas.glxCtx = glxCtx;
  return(glxCtx);
}

/************************************************************************
* Function:	HGUglwGetCanvasGlxContext			
* Returns:	GLXContext:		The Open GL rendering context.
* Purpose:	Gets the Open GL rendering context associated with
*		this canvas widget.				
* Global refs:	-						
* Parameters:	Widget thisW:		Instance of canvas widget.
************************************************************************/
GLXContext     	HGUglwGetCanvasGlxContext(Widget thisW)
{
  HGUglwCanvasWidget thisCW;

  thisCW = (HGUglwCanvasWidget )thisW;

  return(thisCW->hguGLwCanvas.glxCtx);
}

/************************************************************************
* Function:	HGUglwCanvasGlxMakeCurrent			
* Returns:	void						
* Purpose:	Convienience function to make given Open GL context the
*		current context.				
* Global refs:	-						
* Parameters:	Widget thisW:		Instance of canvas widget.
*		GLXContext glxCtx:	GLX rendering context to make
*					current for this canvs widget.
*					If NULL then the Open GL 
*					rendering context associated
*					with this canvas is made
*					current.		
************************************************************************/
Bool     	HGUglwCanvasGlxMakeCurrent(Widget thisW, GLXContext glxCtx)
{
  HGUglwCanvasWidget thisCW;

  thisCW = (HGUglwCanvasWidget )thisW;
  if(glxCtx == NULL)
  {
    glxCtx = thisCW->hguGLwCanvas.glxCtx;
  }
  return(glXMakeCurrent(XtDisplay(thisW), XtWindow(thisW), glxCtx));
}

/************************************************************************
* Function:	HGUglwCanvasSwapBuffers				
* Returns:	void						
* Purpose:	Convienience function to swap the Open GL front and
*		back buffers.					
* Global refs:	-						
* Parameters:	Widget thisW:		Instance of canvas widget.
************************************************************************/
void     	HGUglwCanvasSwapBuffers(Widget thisW)
{
  glXSwapBuffers(XtDisplay(thisW), XtWindow(thisW));
}
