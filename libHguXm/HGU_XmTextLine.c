#if defined(__GNUC__)
#ident "MRC HGU $Id:"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id:"
#else static char _HGU_XmTextLine_c[] = "MRC HGU $Id:";
#endif
#endif
/*!
* \file         HGU_XmTextLine.c
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Wed Apr 29 09:13:52 2009
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
* \brief        Implement a text entry user interface element. This is	     *
*		not a widget implementation but a few convenience	     *
*		routines to return a composite widget (Form) with Label	     *
*		and Text widget children set up to allow text entry.	     *
*		The top-level and label widget have the widget name	     *
*		given as argument to CreateTextLine, the text widget	     *
*		has the name "text".					     *
*		The cb_func is added to the XmNactivateCallback		     *
* Functions:								     *
*	HGU_XmCreateTextLine() - create a text-line element		     *
*	HGU_XmSetTextLineValue - set the text-line value, this will	     *
*		update the display but will not call the callbacks.	     *
*	HGU_XmGetTextLineValue - returns the text-line value.		     *
*									     *
*		See the Motif manual for more details on the Text	     *
*		widget.							     *
*               
* \todo         -
* \bug          None known
*
* Maintenance log with most recent changes at top of list.
*/

#include <stdio.h>

#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/Text.h>

#include <HGU_XmUtils.h>

/************************************************************************
* Function:	Widget HGU_XmCreateTextLine()				*
* Purpose:	Procedure to put together a Form widget with a Label,	*
*	       	and Text children set up to act as a text-line		*
*		graphical element. It is NOT a widget proper and setting*
*		specific resource to change the default appearence	*
*		will require knowing the widget names (see above).	*
* Returns:	The pointer to the parent (Form) widget			*
* Arguments:								*
*	name		the top-level and label widget name		*
*	parent		the parent widget				*
*	value		the initial value				*
*	cb_func		a callback function to be put on the activate	*
*			callback list.					*
*	cb_data		the client data for the callback		*
************************************************************************/
Widget HGU_XmCreateTextLine(
String		name,
Widget		parent,
String		value,
XtCallbackProc	cb_func,
XtPointer	cb_data)
{
    Widget	base, label, text;

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

    text = XtVaCreateManagedWidget("text", xmTextWidgetClass, base,
				   XmNtopAttachment,	XmATTACH_FORM,
				   XmNbottomAttachment,	XmATTACH_FORM,
				   XmNrightAttachment,	XmATTACH_FORM,
				   XmNleftAttachment,	XmATTACH_WIDGET,
				   XmNleftWidget,	label,
				   XmNborderWidth,	0,
				   NULL);

    HGU_XmSetTextLineValue( base, value );

    if( cb_func != NULL )
	XtAddCallback(text, XmNactivateCallback, cb_func, cb_data);

    XtManageChild( base );
    return( base );
}

void HGU_XmSetTextLineValue(
Widget	text_line,
String	value)
{
    Widget			text;

    if( (text = XtNameToWidget(text_line, ".text")) == NULL )
	return;

    if( value != NULL ){
	int	len = (int) strlen( value ) + 1;
	XtVaSetValues(text,
		      XmNvalue,			value,
		      XmNcursorPosition,	len,
		      XmNcolumns,		len,
		      NULL);
    }

    return;
}

String HGU_XmGetTextLineValue(
Widget	text_line)
{
    Widget	text;
    String	value;

    if( (text = XtNameToWidget(text_line, ".text")) == NULL )
	return( NULL );

    XtVaGetValues(text, XmNvalue, &value, NULL);

    return( value );
}

void GE_TextLineSetProc(
Widget	text_line,
String	value)
{
    Widget		text;
    XmAnyCallbackStruct	cbs;

    if( (text = XtNameToWidget(text_line, ".text")) == NULL )
	return;
    HGU_XmSetTextLineValue( text_line, value );

    cbs.reason = XmCR_ACTIVATE;
    cbs.event  = NULL;
    XtCallCallbacks( text, XmNactivateCallback, &cbs );

    return;
}

String GE_Text_LineGetProc(
Widget	text_line)
{
    String	value;
    Widget	text;

    if( (text = XtNameToWidget(text_line, ".text")) == NULL )
	return( NULL );

    XtVaGetValues( text, XmNvalue, &value, NULL );
    return( value );
}
