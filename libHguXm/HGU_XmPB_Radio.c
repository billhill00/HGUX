#if defined(__GNUC__)
#ident "MRC HGU $Id:"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id:"
#else static char _HGU_XmPB_Radio_c[] = "MRC HGU $Id:";
#endif
#endif
/*!
* \file         HGU_XmPB_Radio.c
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Wed Apr 29 09:15:46 2009
* \version      MRC HGU $Id$
*               $Revision$
*               $Name$
* \par Address:
*               MRC Human Genetics Unit,
*               Western General Hospital,
*               Edinburgh, EH4 2XU, UK.
* \par Copyright:
* Copyright (C) 2005 Medical research Council, UK.
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
* \brief        Implement a push-button radio box with label. This	     *
*		is a convenience routine to provide a simple interface	     *
*		for a push button style interface. If toggle buttons	     *
*		with the indicators visible are required then either	     *
*		reset the appropriate resource or usr the Motif		     *
*		procedures.						     *
* Functions:								     *
*               
* \todo         -
* \bug          None known
*
* Maintenance log with most recent changes at top of list.
*/

#include <stdio.h>

#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>

#include <HGU_XmUtils.h>

static void set_bckgrnd_cb(
Widget		w,
XtPointer	client_data,
XtPointer	call_data)
{
    XmToggleButtonCallbackStruct
	*cbs = (XmToggleButtonCallbackStruct *) call_data;

    Pixel	selectColor, background;
    int		previous_state;

    if( client_data ){/* to satisfy the IRIX646 compiler */}

    XtVaGetValues(w,
		  XmNselectColor,	&selectColor,
		  XmNbackground,	&background,
		  XmNuserData,		&previous_state,
		  NULL);

    if( previous_state != cbs->set )
	XtVaSetValues(w,
		      XmNbackground,	selectColor,    
		      XmNselectColor,	background,
		      XmNuserData,	cbs->set,
		      NULL);

    return;
}


/************************************************************************
* Function:	Widget HGU_XmCreatePB_Radio()				*
* Purpose:	Procedure to put together a Form widget with a Label,	*
*	       	and radio button interface - RowColumn + Toggles	*
*		It is NOT a widget proper and setting			*
*		specific resource to change the default appearence	*
*		will require knowing the widget names (see above).	*
* Returns:	The pointer to the parent (Form) widget			*
* Arguments:								*
*	name		the top-level and label widget name		*
*	parent		the parent widget				*
*	items		the toggle button items				*
*	def_item	the default selection				*
************************************************************************/
Widget HGU_XmCreatePB_Radio(
String		name,
Widget		parent,
MenuItem	*items,
int		def_item)
{
    Widget	base, label, radio, widget;
    int		i;

    base = XtVaCreateWidget(name, xmFormWidgetClass, parent,
			    XmNborderWidth,	0,
			    XmNverticalSpacing,	3,
			    NULL);

    label = XtVaCreateManagedWidget(name, xmLabelWidgetClass, base,
				    XmNtopAttachment,	XmATTACH_FORM,
				    XmNbottomAttachment,XmATTACH_FORM,
				    XmNleftAttachment,	XmATTACH_FORM,
				    XmNborderWidth,	0,
				    NULL);

    radio = XmCreateRadioBox( base, "radio", NULL, 0 );
    XtVaSetValues( radio,
		  XmNtopAttachment,	XmATTACH_FORM,
		  XmNbottomAttachment,	XmATTACH_FORM,
		  XmNrightAttachment,	XmATTACH_FORM,
		  XmNleftAttachment,	XmATTACH_WIDGET,
		  XmNleftWidget,	label,
		  XmNborderWidth,	0,
		  XmNorientation,	XmHORIZONTAL,
		  XmNentryAlignment,	XmALIGNMENT_CENTER,
		  XmNbuttonSet,		def_item,
		  NULL);

    i = 0;
    while( items[i].name != NULL ){
	widget = XtVaCreateManagedWidget(items[i].name,
					 xmToggleButtonWidgetClass, radio,
					 XmNindicatorOn,	False,
					 XmNshadowThickness,	4,
					 XmNborderWidth,	0,
					 XmNuserData,		0,
					 NULL);

	XtAddCallback( widget, XmNvalueChangedCallback, set_bckgrnd_cb, NULL);
	if( i == def_item ){
	    XmToggleButtonCallbackStruct	cbs;
	    cbs.set = True;
	    set_bckgrnd_cb( widget, NULL, (XtPointer) &cbs );
	    XtVaSetValues(widget, XmNset, True, NULL);
	}

	/* add mnemonic */
	if( items[i].mnemonic )
	    XtVaSetValues( widget, XmNmnemonic, items[i].mnemonic, NULL );

	/* add accelerators */
	if( items[i].accelerator ){
	    XmString	str = XmStringCreateSimple( items[i].accel_text );
	    XtVaSetValues( widget,
			  XmNaccelerator,	items[i].accelerator,
			  XmNacceleratorText,	str,
			  NULL);
	    XmStringFree( str );
	}

	/* add callbacks */
	if( items[i].callback ){
	    XtAddCallback( widget, XmNvalueChangedCallback,
			  items[i].callback, items[i].callback_data );
	}
	
	i++;
    }
	
    
    XtManageChild( radio );
    XtManageChild( base );
    return( base );
}
