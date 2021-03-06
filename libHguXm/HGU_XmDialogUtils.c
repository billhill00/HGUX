#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _HGU_XmDialogUtils_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         HGU_XmDialogUtils.c
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
* \brief        Convenience routines for creating dialogs, some copied	     *
*		from Heller - Motif Programming Manual.	Also some	     *
*		routines for handling modal dialogs.			     *
*/

#include <stdio.h>
#include <stdlib.h>

#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/Separator.h>

#include <HGU_XmUtils.h>

#define TIGHTNESS	15

Widget HGU_XmCreateActionArea(
Widget		parent,
ActionAreaItem	*actions,
int		num_actions)
{
    Widget	action_area, widget;
    int		i;

    action_area =
	XtVaCreateWidget("action", xmFormWidgetClass, parent,
			 XmNfractionBase,	(TIGHTNESS+1)*num_actions + 1,
			 XmNverticalSpacing,	4,
			 XmNskipAdjust,		True,
			 NULL);

    for( i=0; i < num_actions; i++ ){
	widget =
	    XtVaCreateManagedWidget(actions[i].name,
				    xmPushButtonWidgetClass,	action_area,
				    XmNleftAttachment,	XmATTACH_POSITION,
				    XmNleftPosition,	TIGHTNESS*i + i+1,
				    XmNtopAttachment,		XmATTACH_FORM,
				    XmNbottomAttachment,	XmATTACH_FORM,
				    XmNrightAttachment,	XmATTACH_POSITION,
				    XmNrightPosition,	TIGHTNESS*(i+1) + i+1,
				    XmNshadowThickness, 2,
				    XmNborderWidth,	0,
				    XmNmarginHeight,	3,
				    XmNshowAsDefault,	i==0,
				    XmNdefaultButtonShadowThickness,	1,
				    NULL);

	if( actions[i].callback )
	    XtAddCallback( widget, XmNactivateCallback, actions[i].callback,
			  actions[i].client_data );

    }
    XtManageChild( action_area );

    return( action_area );
}

Widget HGU_XmCreateWActionArea(
Widget		parent,
ActionAreaItem	*actions,
int		num_actions,
WidgetClass	widgetClass)
{
    Widget	action_area, widget;
    int		i;

    action_area =
	XtVaCreateWidget("action", xmFormWidgetClass, parent,
			 XmNfractionBase,	(TIGHTNESS+1)*num_actions + 1,
			 XmNverticalSpacing,	4,
			 XmNskipAdjust,		True,
			 NULL);

    for( i=0; i < num_actions; i++ ){
	widget =
	    XtVaCreateManagedWidget(actions[i].name,
				    widgetClass,	action_area,
				    XmNleftAttachment,	XmATTACH_POSITION,
				    XmNleftPosition,	TIGHTNESS*i + i+1,
				    XmNtopAttachment,	XmATTACH_FORM,
				    XmNbottomAttachment,XmATTACH_FORM,
				    XmNrightAttachment,	XmATTACH_POSITION,
				    XmNrightPosition,	TIGHTNESS*(i+1) + i+1,
				    XmNshadowThickness, 2,
				    XmNborderWidth,	0,
				    XmNmarginHeight,	3,
				    XmNshowAsDefault,	i==0,
				    NULL);

	if( actions[i].callback )
	    XtAddCallback( widget, XmNactivateCallback, actions[i].callback,
			  actions[i].client_data );

    }
    XtManageChild( action_area );

    return( action_area );
}

static ActionAreaItem	default_actions[] = {
{ "Ok",		NULL,			NULL},
{ "Cancel",	PopdownCallback,	NULL},
{ "Help",	NULL,			NULL},
};


Widget HGU_XmCreateStdDialog(
Widget		parent,
String		name,
WidgetClass	controlWidgetClass,
ActionAreaItem	*actions,
int		num_actions)
{
    Widget	dialog, base, action, separator;
    String	parent_name;
    Visual	*visual;

    parent_name = (String) malloc( sizeof(char) * (strlen(name) + 8) );
    (void) sprintf( parent_name, "%s_popup", name );
    visual = HGU_XmWidgetToVisual(parent);

    dialog = XtVaCreatePopupShell(parent_name,
				  xmDialogShellWidgetClass, parent,
				  XmNdeleteResponse,	XmUNMAP,
				  XmNvisual,		visual,
                                  NULL);
    free( parent_name );

    base = XtVaCreateWidget(name, xmFormWidgetClass, dialog, NULL);

    if( actions != NULL )
	action = HGU_XmCreateActionArea( base, actions, num_actions );
    else
	action = HGU_XmCreateActionArea( base, default_actions,
					XtNumber(default_actions) );
    XtVaSetValues( action,
		  XmNleftAttachment,	XmATTACH_FORM,
		  XmNrightAttachment,	XmATTACH_FORM,
		  XmNbottomAttachment,	XmATTACH_FORM,
		  XmNborderWidth,	0,
		  NULL);

    separator = XtVaCreateManagedWidget("separator", xmSeparatorWidgetClass,
                                        base,
                                        XmNbottomAttachment,   XmATTACH_WIDGET,
                                        XmNbottomWidget,        action,
                                        XmNleftAttachment,      XmATTACH_FORM,
                                        XmNrightAttachment,     XmATTACH_FORM,
                                        XmNborderWidth,         0,
                                        NULL);

    (void) XtVaCreateManagedWidget("control", controlWidgetClass, base,
				   XmNbottomAttachment,	XmATTACH_WIDGET,
				   XmNbottomWidget,    	separator,
				   XmNtopAttachment,  	XmATTACH_FORM,
				   XmNleftAttachment,  	XmATTACH_FORM,
				   XmNrightAttachment,  XmATTACH_FORM,
				   XmNborderWidth,      0,
				   NULL);

    return( base );
}

/************************************************************************
* Function:     confirm_callback                                        *
* Returns:      void:                                                   *
* Global Refs:  -                                                       *
* Purpose:      Private callback for HGU_XmDialogConfirm                  *
************************************************************************/
static void confirm_callback(
Widget  	w,
XtPointer	client_data,
XtPointer	call_data)
{
    int	*answer = (int *) client_data;

    if( w || call_data){/* to satisfy IRIX646 compiler */}

    *answer = 1;
}

/************************************************************************
* Function:     cancel_callback	                                        *
* Returns:      void:                                                   *
* Global Refs:  -                                                       *
* Purpose:      Private callback for HGU_XmDialogConfirm                  *
************************************************************************/
static void cancel_callback(
Widget  	w,
XtPointer	client_data,
XtPointer	call_data)
{
    int	*answer = (int *) client_data;

    if( w || call_data){/* to satisfy IRIX646 compiler */}

    *answer = 0;
}

int HGU_XmDialogConfirm(
Widget		dialog,
Widget		confirm,
Widget		cancel,
unsigned char	modality)
{
    XtAppContext        app_con = XtWidgetToApplicationContext( dialog );
    int			answer = -1;

    /* get confirm and cancel widgets if required */
    if( confirm == NULL ){
	if( (confirm = XtNameToWidget(dialog, "*Ok")) == NULL )
	    return( 0 );
    }
    if( cancel == NULL ){
	if( (cancel = XtNameToWidget(dialog, "*Cancel")) == NULL )
	    return( 0 );
    }

    /* set callbacks */
    XtAddCallback(confirm, XmNactivateCallback, confirm_callback,
		  (XtPointer) &answer);
    XtAddCallback(cancel, XmNactivateCallback, cancel_callback,
		  (XtPointer) &answer);

    /* set modal behavior */
    XtVaSetValues( dialog, XmNdialogStyle, modality, NULL);

    /* popup widget and process events */
    PopupCallback( XtParent(dialog), (XtPointer) XtParent(dialog), NULL );
    while( answer < 0 ){
        XtAppProcessEvent( app_con, XtIMAll );
        XFlush( XtDisplay(dialog) );
    }

    /* remove dialog, flush display and destroy resources */
    PopdownCallback( XtParent(dialog), (XtPointer)XtParent(dialog), NULL );
    XmUpdateDisplay( dialog );

    /* remove callbacks */
    XtRemoveCallback(confirm, XmNactivateCallback, confirm_callback,
		     (XtPointer) &answer);
    XtRemoveCallback(cancel, XmNactivateCallback, cancel_callback,
		     (XtPointer) &answer);

    return( answer );
}
