#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _HGU_XmMiscCallbacks_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         HGU_XmMiscCallbacks.c
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
* \brief        Miscellaneous useful callbacks
*/

#include <stdio.h>
#include <stdlib.h>

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <Xm/FileSB.h>

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

void FSBPopupCallback(
Widget		w,
XtPointer       client_data,
XtPointer	call_data)
{
  XmString	dirSpec;

  /* confirm we have been given a file selection box widget */
  if( XmIsFileSelectionBox(w) ){
    /* get the current value so it can be reset */
    XtVaGetValues(w, XmNdirSpec, &dirSpec, NULL);

    /* update the selection */
    XmFileSelectionDoSearch( w, NULL );

    /* reset the value */
    XtVaSetValues(w, XmNdirSpec, dirSpec, NULL);
  }

  return;
}
