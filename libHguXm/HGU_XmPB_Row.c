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
* File        :    HGU_XmPB_Row.c					     *
* $Revision$
******************************************************************************
* Author Name :     Richard Baldock					     *
* Author Login:    richard@hgu.mrc.ac.uk				     *
* Date        :    Thu Aug 25 09:56:48 1994				     *
* Synopsis:	Implement a push-button row with label. This		     *
*		is a convenience routine to provide a simple interface	     *
*		for a push button style interface. This is to provide	     *
*		a graphical element with a row of push buttons preceded	     *
*		by a label.						     *
* Functions:								     *
*****************************************************************************/
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
