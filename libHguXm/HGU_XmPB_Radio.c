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
* File        :    HGU_XmPB_Radio.c					     *
* $Revision$
******************************************************************************
* Author Name :     Richard Baldock					     *
* Author Login:    richard@hgu.mrc.ac.uk				     *
* Date        :    Thu Aug 25 09:56:07 1994				     *
* Synopsis:	Implement a push-button radio box with label. This	     *
*		is a convenience routine to provide a simple interface	     *
*		for a push button style interface. If toggle buttons	     *
*		with the indicators visible are required then either	     *
*		reset the appropriate resource or usr the Motif		     *
*		procedures.						     *
* Functions:								     *
*****************************************************************************/
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
