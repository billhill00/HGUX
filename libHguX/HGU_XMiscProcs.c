#pragma ident "MRC HGU $Id$"
/*****************************************************************************
* Copyright   :    1994 Medical Research Council, UK.                        *
*                  All rights reserved.                                      *
******************************************************************************
* Address     :    MRC Human Genetics Unit,                                  *
*                  Western General Hospital,                                 *
*                  Edinburgh, EH4 2XU, UK.                                   *
******************************************************************************
* Project     :    libhguX - MRC HGU X11 Utilities			     *
* File        :    HGU_XMiscProcs.c					     *
* $Revision$
******************************************************************************
* Author Name :    Richard Baldock					     *
* Author Login:    richard@hgu.mrc.ac.uk				     *
* Date        :    Wed Sep 14 18:51:27 1994				     *
* Synopsis    :    X miscellaneous procedures				     *
*****************************************************************************/
#include <stdio.h>

#include <X11/Xproto.h>
#include <X11/Xlib.h>

int QueryInWindow(
Display		*dpy,
Window		win)
{
	XWindowAttributes	win_att;
	Window			root, child;
	int			X, Y, x, y;
	unsigned int		keys;

	/* get position within parent */
        if( XGetWindowAttributes(dpy, win, &win_att) == 0 )
		return( 0 );

	if( XQueryPointer(dpy, win, &root, &child, &X, &Y, &x, &y, &keys)
			== False )
		return( 0 );

	if( (x > 0) && (x < win_att.width) && (y > 0) && (y < win_att.height) )
		return( 1 );
	return( 0 );
}

int QueryRootPosition(
Display		*dpy,
Window		win,
int		*x,
int		*y)
{
	int			X, Y;
	unsigned int		nchildren;
	XWindowAttributes	win_att;
	Window			root, parent, *children;

	/* get position within parent */
        if( XGetWindowAttributes(dpy, win, &win_att) == 0 )
		return( 1 );

	if( win == win_att.root ){ /* stopping condition */
		*x = 0;
		*y = 0;
		return( 0 );
	}

	*x = win_att.x;
	*y = win_att.y;

	/* get the parent window */
	XQueryTree(dpy, win, &root, &parent, &children, &nchildren);
	XFree( children );

	/* get parent position */
	QueryRootPosition(dpy, parent, &X, &Y);
	*x += X;
	*y += Y;

	return( 0 );
}

char *HGU_XRequestToString(
  char	request_code)
{
  char	*str;

  switch( request_code ){
  case X_CreateWindow:
    str = "X_CreateWindow";
    break;

  case X_ChangeWindowAttributes:
    str = "X_ChangeWindowAttributes";
    break;

  case X_GetWindowAttributes:
    str = "X_GetWindowAttributes";
    break;

  case X_DestroyWindow:
    str = "X_DestroyWindow";
    break;

  case X_DestroySubwindows:
    str = "X_DestroySubwindows";
    break;

  case X_ChangeSaveSet:
    str = "X_ChangeSaveSet";
    break;

  case X_ReparentWindow:
    str = "X_ReparentWindow";
    break;

  case X_MapWindow:
    str = "X_MapWindow";
    break;

  case X_MapSubwindows:
    str = "X_MapSubwindows";
    break;

  case X_UnmapWindow:
    str = "X_UnmapWindow";
    break;

  case X_UnmapSubwindows:
    str = "X_UnmapSubwindows";
    break;

  case X_ConfigureWindow:
    str = "X_ConfigureWindow";
    break;

  case X_CirculateWindow:
    str = "X_CirculateWindow";
    break;

  case X_GetGeometry:
    str = "X_GetGeometry";
    break;

  case X_QueryTree:
    str = "X_QueryTree";
    break;

  case X_InternAtom:
    str = "X_InternAtom";
    break;

  case X_GetAtomName:
    str = "X_GetAtomName";
    break;

  case X_ChangeProperty:
    str = "X_ChangeProperty";
    break;

  case X_DeleteProperty:
    str = "X_DeleteProperty";
    break;

  case X_GetProperty:
    str = "X_GetProperty";
    break;

  case X_ListProperties:
    str = "X_ListProperties";
    break;

  case X_SetSelectionOwner:
    str = "X_SetSelectionOwner";
    break;

  case X_GetSelectionOwner:
    str = "X_GetSelectionOwner";
    break;

  case X_ConvertSelection:
    str = "X_ConvertSelection";
    break;

  case X_SendEvent:
    str = "X_SendEvent";
    break;

  case X_GrabPointer:
    str = "X_GrabPointer";
    break;

  case X_UngrabPointer:
    str = "X_UngrabPointer";
    break;

  case X_GrabButton:
    str = "X_GrabButton";
    break;

  case X_UngrabButton:
    str = "X_UngrabButton";
    break;

  case X_ChangeActivePointerGrab:
    str = "X_ChangeActivePointerGrab";
    break;

  case X_GrabKeyboard:
    str = "X_GrabKeyboard";
    break;

  case X_UngrabKeyboard:
    str = "X_UngrabKeyboard";
    break;

  case X_GrabKey:
    str = "X_GrabKey";
    break;

  case X_UngrabKey:
    str = "X_UngrabKey";
    break;

  case X_AllowEvents:
    str = "X_AllowEvents";
    break;

  case X_GrabServer:
    str = "X_GrabServer";
    break;

  case X_UngrabServer:
    str = "X_UngrabServer";
    break;

  case X_QueryPointer:
    str = "X_QueryPointer";
    break;

  case X_GetMotionEvents:
    str = "X_GetMotionEvents";
    break;

  case X_TranslateCoords:
    str = "X_TranslateCoords";
    break;

  case X_WarpPointer:
    str = "X_WarpPointer";
    break;

  case X_SetInputFocus:
    str = "X_SetInputFocus";
    break;

  case X_GetInputFocus:
    str = "X_GetInputFocus";
    break;

  case X_QueryKeymap:
    str = "X_QueryKeymap";
    break;

  case X_OpenFont:
    str = "X_OpenFont";
    break;

  case X_CloseFont:
    str = "X_CloseFont";
    break;

  case X_QueryFont:
    str = "X_QueryFont";
    break;

  case X_QueryTextExtents:
    str = "X_QueryTextExtents";
    break;

  case X_ListFonts:
    str = "X_ListFonts";
    break;

  case X_ListFontsWithInfo:
    str = "X_ListFontsWithInfo";
    break;

  case X_SetFontPath:
    str = "X_SetFontPath";
    break;

  case X_GetFontPath:
    str = "X_GetFontPath";
    break;

  case X_CreatePixmap:
    str = "X_CreatePixmap";
    break;

  case X_FreePixmap:
    str = "X_FreePixmap";
    break;

  case X_CreateGC:
    str = "X_CreateGC";
    break;

  case X_ChangeGC:
    str = "X_ChangeGC";
    break;

  case X_CopyGC:
    str = "X_CopyGC";
    break;

  case X_SetDashes:
    str = "X_SetDashes";
    break;

  case X_SetClipRectangles:
    str = "X_SetClipRectangles";
    break;

  case X_FreeGC:
    str = "X_FreeGC";
    break;

  case X_ClearArea:
    str = "X_ClearArea";
    break;

  case X_CopyArea:
    str = "X_CopyArea";
    break;

  case X_CopyPlane:
    str = "X_CopyPlane";
    break;

  case X_PolyPoint:
    str = "X_PolyPoint";
    break;

  case X_PolyLine:
    str = "X_PolyLine";
    break;

  case X_PolySegment:
    str = "X_PolySegment";
    break;

  case X_PolyRectangle:
    str = "X_PolyRectangle";
    break;

  case X_PolyArc:
    str = "X_PolyArc";
    break;

  case X_FillPoly:
    str = "X_FillPoly";
    break;

  case X_PolyFillRectangle:
    str = "X_PolyFillRectangle";
    break;

  case X_PolyFillArc:
    str = "X_PolyFillArc";
    break;

  case X_PutImage:
    str = "X_PutImage";
    break;

  case X_GetImage:
    str = "X_GetImage";
    break;

  case X_PolyText8:
    str = "X_PolyText8";
    break;

  case X_PolyText16:
    str = "X_PolyText16";
    break;

  case X_ImageText8:
    str = "X_ImageText8";
    break;

  case X_ImageText16:
    str = "X_ImageText16";
    break;

  case X_CreateColormap:
    str = "X_CreateColormap";
    break;

  case X_FreeColormap:
    str = "X_FreeColormap";
    break;

  case X_CopyColormapAndFree:
    str = "X_CopyColormapAndFree";
    break;

  case X_InstallColormap:
    str = "X_InstallColormap";
    break;

  case X_UninstallColormap:
    str = "X_UninstallColormap";
    break;

  case X_ListInstalledColormaps:
    str = "X_ListInstalledColormaps";
    break;

  case X_AllocColor:
    str = "X_AllocColor";
    break;

  case X_AllocNamedColor:
    str = "X_AllocNamedColor";
    break;

  case X_AllocColorCells:
    str = "X_AllocColorCells";
    break;

  case X_AllocColorPlanes:
    str = "X_AllocColorPlanes";
    break;

  case X_FreeColors:
    str = "X_FreeColors";
    break;

  case X_StoreColors:
    str = "X_StoreColors";
    break;

  case X_StoreNamedColor:
    str = "X_StoreNamedColor";
    break;

  case X_QueryColors:
    str = "X_QueryColors";
    break;

  case X_LookupColor:
    str = "X_LookupColor";
    break;

  case X_CreateCursor:
    str = "X_CreateCursor";
    break;

  case X_CreateGlyphCursor:
    str = "X_CreateGlyphCursor";
    break;

  case X_FreeCursor:
    str = "X_FreeCursor";
    break;

  case X_RecolorCursor:
    str = "X_RecolorCursor";
    break;

  case X_QueryBestSize:
    str = "X_QueryBestSize";
    break;

  case X_QueryExtension:
    str = "X_QueryExtension";
    break;

  case X_ListExtensions:
    str = "X_ListExtensions";
    break;

  case X_ChangeKeyboardMapping:
    str = "X_ChangeKeyboardMapping";
    break;

  case X_GetKeyboardMapping:
    str = "X_GetKeyboardMapping";
    break;

  case X_ChangeKeyboardControl:
    str = "X_ChangeKeyboardControl";
    break;

  case X_GetKeyboardControl:
    str = "X_GetKeyboardControl";
    break;

  case X_Bell:
    str = "X_Bell";
    break;

  case X_ChangePointerControl:
    str = "X_ChangePointerControl";
    break;

  case X_GetPointerControl:
    str = "X_GetPointerControl";
    break;

  case X_SetScreenSaver:
    str = "X_SetScreenSaver";
    break;

  case X_GetScreenSaver:
    str = "X_GetScreenSaver";
    break;

  case X_ChangeHosts:
    str = "X_ChangeHosts";
    break;

  case X_ListHosts:
    str = "X_ListHosts";
    break;

  case X_SetAccessControl:
    str = "X_SetAccessControl";
    break;

  case X_SetCloseDownMode:
    str = "X_SetCloseDownMode";
    break;

  case X_KillClient:
    str = "X_KillClient";
    break;

  case X_RotateProperties:
    str = "X_RotateProperties";
    break;

  case X_ForceScreenSaver:
    str = "X_ForceScreenSaver";
    break;

  case X_SetPointerMapping:
    str = "X_SetPointerMapping";
    break;

  case X_GetPointerMapping:
    str = "X_GetPointerMapping";
    break;

  case X_SetModifierMapping:
    str = "X_SetModifierMapping";
    break;

  case X_GetModifierMapping:
    str = "X_GetModifierMapping";
    break;

  case X_NoOperation:
    str = "X_NoOperation";
    break;

  default:
    str = "unknown code";
    break;
  }

  return str;
}
