#pragma ident "MRC HGU $Id"
/************************************************************************
* Project:	Mouse Atlas
* Title:	HGUGL-TestAxes.c
* Date: 	April 1999
* Author:	Bill Hill
* Copyright:	1999 Medical Research Council, UK.
*		All rights reserved.
* Address:	MRC Human Genetics Unit,
*		Western General Hospital,
*		Edinburgh, EH4 2XU, UK.
* Purpose:	Test binary for the MRC HGU OpenGL library.
* $Revision$
* Maintenance:	Log changes below, with most recent at top of list.
************************************************************************/
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <X11/keysym.h>
#include <Xm/XmAll.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <Wlz.h>
#include <HGUgl.h>

static void 	AxesDrawScene(Widget),
		AxesDrawSceneCb(Widget, XtPointer, XtPointer),
		AxesResizeCb(Widget, XtPointer, XtPointer),
		AxesInitCb(Widget, XtPointer, XtPointer),
		AxesInputCb(Widget, XtPointer, XtPointer);

static HGUglTextFont *axesFont = NULL;

int		main(int argc, char *argv[])
{
  Widget	canvas,
  		toplevel,
		form,
		frame;
  XtAppContext	appCtx;
  static String axesFallbackRes[] =
  {
    "*frame*shadowType: SHADOW_IN",
    "*canvas*width: 256",
    "*canvas*height: 256",
    "*canvas*doublebuffer: TRUE",
    "*canvas*rgba: TRUE",
    "*canvas*redSize: 2",
    "*canvas*greenSize: 2",
    "*canvas*blueSize: 2",
    "*canvas*alphaSize: 0",
    "*canvas*allocateBackground: TRUE",
    "*canvas*allocateotherColors: TRUE",
    "*canvas*installColormap: TRUE",
    NULL
  };

  toplevel = XtVaAppInitialize(&appCtx, argv[0],
			       (XrmOptionDescList) NULL, 
			       (Cardinal)0,
			       (int *)&argc, 
			       (String*)argv, 
			       axesFallbackRes,
			       NULL);
  form = XtVaCreateManagedWidget("form", xmFormWidgetClass, toplevel,
				 NULL);
  frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, form,
    				XtNx, 30,
				XtNy, 30,
				XmNbottomAttachment, XmATTACH_FORM,
				XmNtopAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_FORM,
				XmNrightAttachment, XmATTACH_FORM,
				XmNleftOffset, 30,
				XmNtopOffset, 30,
				XmNbottomOffset, 30,
				XmNrightOffset, 30,
				NULL);
  canvas = XtVaCreateManagedWidget("canvas", hguGLwCanvasTbWidgetClass, frame,
			           NULL);
  XtAddCallback(canvas, HGUglwNexposeCallback, AxesDrawSceneCb, NULL);
  XtAddCallback(canvas, HGUglwNresizeCallback, AxesResizeCb, NULL);
  XtAddCallback(canvas, HGUglwNinitCallback, AxesInitCb, NULL);
  XtAddCallback(canvas, HGUglwNinputCallback, AxesInputCb, NULL);
  XtAddCallback(canvas, HGUglwNtrackballCallback, AxesDrawSceneCb, NULL);
  XtRealizeWidget(toplevel);
  XtAppMainLoop(appCtx);
  return(0);
}

static void 	AxesInitCb(Widget w, XtPointer clientData,
			     XtPointer callData)
{
  static GLfloat fogColor[4] = {0.0, 0.0, 0.0, 1.0};
  static String xFontStr = "-*-courier-*-*-*-*-14-*-*-*-*-*-*-*";
  Font		xFont;

  (void )HGUglwCreateCanvasGlxContext(w, NULL);
  HGUglwCanvasGlxMakeCurrent(w, NULL);
  xFont = XLoadFont(XtDisplay(w), xFontStr);
  if(xFont)
  {
    axesFont = HGUglTextFontCreate(xFont);
  }
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
  glLineWidth(1.5);
  glEnable(GL_FOG);
  glFogi (GL_FOG_MODE, GL_LINEAR);
  glHint (GL_FOG_HINT, GL_NICEST);
  glFogf (GL_FOG_START, 1.0);
  glFogf (GL_FOG_END, -1.0);
  glFogfv (GL_FOG_COLOR, fogColor);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glDepthFunc(GL_LESS);
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_FLAT);
}


static void	AxesDrawSceneCb(Widget w, XtPointer clientData,
			          XtPointer callData)
{
  AxesDrawScene(w);
}

static void	AxesResizeCb(Widget w, XtPointer clientData,
		               XtPointer callData)
{
  HGUglwCanvasCallbackStruct *glwCbs;

  glwCbs = (HGUglwCanvasCallbackStruct *)callData;
  glViewport(0, 0, (GLint)(glwCbs->width) - 1, (GLint)(glwCbs->height) - 1);
}

static void AxesDrawScene(Widget canvasW)
{
  GLdouble	rot[4][4];
  WlzDVertex3	scale,
  		trans;
  Dimension	width,
  		height;
  GLfloat	aspect;

  XtVaGetValues(canvasW, XmNwidth, &width, XmNheight, &height, NULL);
  if((width > 0) && (height > 0))
  {
    HGUglwCanvasGlxMakeCurrent(canvasW, NULL);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if(height > width)
    {
      aspect = (GLfloat )height/(GLfloat )width;
      glOrtho(-1.0, 1.0, -(aspect), aspect, -1.0, +1.0);
    }
    else
    {
      aspect = (GLfloat )width/(GLfloat )height;
      glOrtho(-(aspect), aspect, -1.0, 1.0, -1.0, +1.0);
    }
    HGUglwCanvasTbGetTranslate(canvasW, &trans);
    HGUglwCanvasTbGetRotateMatrixGL(canvasW, rot);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(trans.vtX, trans.vtY, trans.vtZ);
    glMultMatrixd(&rot[0][0]);
    WLZ_VTX_3_SET(scale, 0.75, 0.75, 0.75);
    HGUglShapeWireAxes(axesFont, scale, "X-axis", "Y-axis", "Z-axis");
    HGUglwCanvasSwapBuffers(canvasW);
  }
}

static void	AxesInputCb(Widget w, XtPointer clientData,
		  	      XtPointer callData)
{
  char		buffer[1];
  KeySym	keysym;
  HGUglwCanvasCallbackStruct *glwCbs;

  glwCbs = (HGUglwCanvasCallbackStruct *)callData;
  switch(glwCbs->event->type)
  {
    case KeyRelease:
      if(XLookupString((XKeyEvent *)glwCbs->event, buffer, 1,
      		       &keysym, NULL) == 1)
      {
	switch(keysym)
	{
	  case XK_Escape:
	  case XK_Q:
	  case XK_q:
	    exit(0);
	    break;
	}
      }
      break;
  }
}
