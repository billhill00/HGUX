#ifndef HGUGL_GLWCANVAS_H
#define HGUGL_GLWCANVAS_H
#pragma ident "MRC HGU $Id"
/************************************************************************
* Project:	Mouse Atlas
* Title:	HGUglwCanvas.h
* Date: 	April 1999
* Author:	Bill Hill
* Copyright:	1999 Medical Research Council, UK.
*		All rights reserved.
* Address:	MRC Human Genetics Unit,
*		Western General Hospital,
*		Edinburgh, EH4 2XU, UK.
* Purpose:	Public include file for the HGUglCanvas MRC HGU OpenGL
*		widget. The HGUglCanvas widget is descended from the
*		Motif primitive widget. See the manual page
*		HGUglCanvas(3) for a description of the widget.
* $Revision$
* Maintenance:	Log changes below, with most recent at top of list.
************************************************************************/
#include <GL/gl.h>
#include <GL/glx.h>
#include <HGUglwCallbacks.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Resources:
 *
 * Name			Class			RepType		Default Value
 * ----                -----              	-------         -------------
 * attribList          	AttribList         	int *           NULL
 * visualInfo          	VisualInfo         	VisualInfo      NULL
 * installColormap     	InstallColormap    	Boolean         TRUE
 * allocateBackground  	AllocateColors     	Boolean         FALSE
 * allocateOtherColors 	AllocateColors     	Boolean         FALSE
 * installBackground   	InstallBackground  	Boolean         TRUE
 * exposeCallback      	Callback           	Pointer         NULL
 * initCallback       	Callback           	Pointer         NULL
 * inputCallback       	Callback           	Pointer         NULL
 * resizeCallback      	Callback           	Pointer         NULL
 * bufferSize          	BufferSize              int             0
 * level               	Level            	int             0
 * rgba                	Rgba             	Boolean         FALSE
 * doublebuffer        	Doublebuffer     	Boolean         FALSE
 * stereo              	Stereo           	Boolean         FALSE
 * auxBuffers          	AuxBuffers              int             0
 * redSize             	ColorSize               int             1
 * greenSize           	ColorSize               int             1
 * blueSize            	ColorSize               int             1
 * alphaSize           	AlphaSize               int             0
 * depthSize           	DepthSize               int             0
 * stencilSize         	StencilSize      	int             0
 * accumRedSize        	AccumColorSize   	int             0
 * accumGreenSize      	AccumColorSize   	int             0
 * accumBlueSize       	AccumColorSize   	int             0
 * accumAlphaSize      	AccumAlphaSize		int             0
 * glxCtx		GlxCtx			GLXContext	NULL
 * directRender		DirectRender		Boolean		FALSE
 */

#define HGUglwNattribList         	"attribList"
#define HGUglwCAttribList         	"AttribList"
#define HGUglwNvisualInfo         	"visualInfo"
#define HGUglwCVisualInfo         	"VisualInfo"
#define HGUglwRVisualInfo         	"VisualInfo"

#define HGUglwNinstallColormap    	"installColormap"
#define HGUglwCInstallColormap    	"InstallColormap"
#define HGUglwNallocateBackground 	"allocateBackground"
#define HGUglwNallocateOtherColors	"allocateOtherColors"
#define HGUglwCAllocateColors     	"AllocateColors"
#define HGUglwNinstallBackground  	"installBackground"
#define HGUglwCInstallBackground  	"InstallBackground"

#define HGUglwNbufferSize         	"bufferSize"
#define HGUglwCBufferSize         	"BufferSize"
#define HGUglwNlevel              	"level"
#define HGUglwCLevel              	"Level"
#define HGUglwNrgba               	"rgba"
#define HGUglwCRgba               	"Rgba"
#define HGUglwNdoublebuffer       	"doublebuffer"
#define HGUglwCDoublebuffer       	"Doublebuffer"
#define HGUglwNstereo             	"stereo"
#define HGUglwCStereo             	"Stereo"
#define HGUglwNauxBuffers         	"auxBuffers"
#define HGUglwCAuxBuffers         	"AuxBuffers"
#define HGUglwNredSize            	"redSize"
#define HGUglwNgreenSize          	"greenSize"
#define HGUglwNblueSize           	"blueSize"
#define HGUglwCColorSize          	"ColorSize"
#define HGUglwNalphaSize          	"alphaSize"
#define HGUglwCAlphaSize          	"AlphaSize"
#define HGUglwNdepthSize          	"depthSize"
#define HGUglwCDepthSize          	"DepthSize"
#define HGUglwNstencilSize        	"stencilSize"
#define HGUglwCStencilSize        	"StencilSize"
#define HGUglwNaccumRedSize       	"accumRedSize"
#define HGUglwNaccumGreenSize     	"accumGreenSize"
#define HGUglwNaccumBlueSize      	"accumBlueSize"
#define HGUglwCAccumColorSize     	"AccumColorSize"
#define HGUglwNaccumAlphaSize     	"accumAlphaSize"
#define HGUglwCAccumAlphaSize     	"AccumAlphaSize"

#define HGUglwNglxCtx			"glxCtx"
#define HGUglwCGlxCtx			"GlxCtx"
#define HGUglwNdirectRender		"directRender"
#define HGUglwCDirectRender		"DirectRender"

/*
 * Class record constants.
 */
externalref WidgetClass hguGLwCanvasWidgetClass;

typedef struct _HGUglwCanvasClassRec	*HGUglwCanvasWidgetClass;
typedef struct _HGUglwCanvasRec		*HGUglwCanvasWidget;

extern Widget 	HGUglwCreateCanvas(Widget, char *, ArgList, Cardinal);
extern void	HGUglwSetCanvasGlxContext(Widget, GLXContext),
		HGUglwCanvasInput(Widget, XEvent *, String *, Cardinal *),
		HGUglwCanvasSwapBuffers(Widget);
extern GLXContext HGUglwCreateCanvasGlxContext(Widget, GLXContext),
		HGUglwGetCanvasGlxContext(Widget);
extern Bool	HGUglwCanvasGlxMakeCurrent(Widget, GLXContext);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HGUGL_GLWCANVAS_H */
