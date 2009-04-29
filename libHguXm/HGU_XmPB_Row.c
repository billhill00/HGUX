#if defined(__GNUC__)
#ident "MRC HGU $Id:"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id:"
#else static char _HGU_XmPB_Row_c[] = "MRC HGU $Id:";
#endif
#endif
/*!
* \file         HGU_XmPB_Row.c
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Wed Apr 29 09:15:22 2009
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
* \brief        Implement a push-button row with label. This		     *
*		is a convenience routine to provide a simple interface	     *
*		for a push button style interface. This is to provide	     *
*		a graphical element with a row of push buttons preceded	     *
*		by a label.						     *
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
#include <Xm/PushB.h>
#include <Xm/PushBG.h>

#include <HGU_XmUtils.h>

/************************************************************************
* Function:	Widget HGU_XmCreatePB_Row()				*
* Purpose:	Procedure to put together a Form widget with a Label,	*
*	       	and push button interface - RowColumn + PushButtons	*
*		It is NOT a widget proper and setting			*
*		specific resource to change the default appearence	*
*		will require knowing the widget names (see above).	*
* Returns:	The pointer to the parent (Form) widget			*
* Arguments:								*
*	name		the top-level and label widget name		*
*	parent		the parent widget				*
*	items		the toggle button items				*
************************************************************************/
Widget HGU_XmCreatePB_Row(
String		name,
Widget		parent,
MenuItem	*items)
{
    Widget	base, label, row, widget;
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

    row = XtVaCreateWidget("row", xmRowColumnWidgetClass, base,
			   XmNtopAttachment,	XmATTACH_FORM,
			   XmNbottomAttachment,	XmATTACH_FORM,
			   XmNrightAttachment,	XmATTACH_FORM,
			   XmNleftAttachment,	XmATTACH_WIDGET,
			   XmNleftWidget,	label,
			   XmNborderWidth,	0,
			   XmNorientation,	XmHORIZONTAL,
			   NULL);

    i = 0;
    while( items[i].name != NULL ){
	widget = XtVaCreateManagedWidget(items[i].name,
					 xmPushButtonWidgetClass, row,
					 XmNborderWidth,	0,
					 XmNshadowThickness,	4,
					 NULL);

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
	    XtAddCallback( widget, XmNactivateCallback,
			  items[i].callback, items[i].callback_data );
	}
	
	i++;
    }
	
    
    XtManageChild( row );
    XtManageChild( base );
    return( base );
}
