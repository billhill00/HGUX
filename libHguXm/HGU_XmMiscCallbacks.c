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
* File        :    HGU_XmMiscCallbacks.c				     *
* $Revision$
******************************************************************************
* Author Name :     Richard Baldock					     *
* Author Login:    richard@hgu.mrc.ac.uk				     *
* Date        :    Thu Aug 25 09:54:45 1994				     *
* Synopsis:	Miscellaneous useful callbacks				     *
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>

#include <HGU_XmUtils.h>

void Quit(
Widget		w,
XtPointer       client_data,
XtPointer	call_data)
{
    Widget          topl = (Widget) client_data;

    if( w || call_data ){ /* to satisfy IRIX646 compiler */ }

    /* use a dialog widget to confirm exit */
    if( !HGU_XmUserConfirm(topl, "Really quit?", "Yes", "No", 1 ) )
	return;

    XtDestroyWidget( topl );
    exit( 0 );
}

void PopupCallback(
Widget		w,
XtPointer       client_data,
XtPointer	call_data)
{
  Widget       widget;
  Widget	*children;
  int		numChildren;

  if( w || call_data ){ /* to satisfy IRIX646 compiler */ }

  widget = (Widget) client_data;

  if( XtIsShell( widget ) ){
    XtVaGetValues(widget, XtNchildren, &children,
		  XtNnumChildren, &numChildren, NULL);
    if( numChildren ){
      XtManageChild(*children);
      XtMapWidget(*children);
    }
    XtPopup( widget, XtGrabNone );
  }
}

void PopdownCallback(
Widget		w,
XtPointer       client_data,
XtPointer	call_data)
{
  Widget		widget = (Widget) client_data;

  if( call_data ){ /* to satisfy IRIX646 compiler */ }

  if( widget == NULL ){
    if( XtIsShell( w ) == True )
      widget = w;
    else {
      PopdownCallback( XtParent( w ), NULL, NULL );
      return;
    }
  }
  XtPopdown( widget );
}

void DestroyWidgetCallback(
Widget		w,
XtPointer       client_data,
XtPointer	call_data)
{
  Widget widget = (Widget) client_data;

  if( call_data ){ /* to satisfy IRIX646 compiler */ }

  if( widget != NULL )
    XtDestroyWidget( widget );
  else
    XtDestroyWidget( w );
  return;
}

void SetSensitiveCallback(
Widget		w,
XtPointer       client_data,
XtPointer	call_data)
{
  Widget widget = (Widget) client_data;
  
  if( call_data ){ /* to satisfy IRIX646 compiler */ }

  if( widget != NULL )
    XtSetSensitive( widget, True );
  else
    XtSetSensitive( w, True );
  return;
}

void UninstallTranslationsCallback(
Widget		w,
XtPointer       client_data,
XtPointer	call_data)
{
  Widget widget = (Widget) client_data;

  if( call_data ){ /* to satisfy IRIX646 compiler */ }

  if( widget != NULL )
    XtUninstallTranslations( widget );
  else
    XtUninstallTranslations( w );
  return;
}
