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
* File        :    HGU_XmUserPrompt.c					     *
* $Revision$
******************************************************************************
* Author Name :     Richard Baldock					     *
* Author Login:    richard@hgu.mrc.ac.uk				     *
* Date        :    Thu Aug 25 10:01:23 1994				     *
* Synopsis:	Motif versions of HGU_notice procedures plus convenience     *
*		procedures to the standard Motif dialogs		     *
*									     *
*	HGU_XmUserConfirm():	prompts for a yes/no answer and returns	     *
*				1/0 respectively.			     *
*	HGU_XmUserContinue():	prompts for a continue (ie no switch)	     *
*				modeless and prcesses events.		     *
*	HGU_XmUserGetstr():	prompts for an input string and returns	     *
*				either a string pointer or NULL.	     *
*	HGU_XmUserInfo():	put up an infomation dialog with	     *
*				settable modality. Returns immediately.	     *
*	HGU_XmUserMessage():	put up a message dialog with settable	     *
*				modality. Not very different from	     *
*				HGU_XmUserInfo(). Returns immediately.	     *
*****************************************************************************/
#include <stdio.h>

#include <Xm/FileSB.h>
#include <Xm/MessageB.h>
#include <Xm/SelectioB.h>
#include <Xm/PushBG.h>

#include <HGU_XmUtils.h>

/************************************************************************
* Function:     confirm_callback					*
* Returns:      void:		                    			*
* Global Refs:  -                                                       *
* Purpose:      Private callback for HGU_XmUserConfirm			*
************************************************************************/
static void confirm_callback(
Widget		w,
XtPointer	client_data,
XtPointer	call_data)
{
    XmAnyCallbackStruct	*cbs = (XmAnyCallbackStruct *) call_data;
    int			*answer = (int *) client_data;

    if( w ){/* to satisfy IRIX646 compiler */}

    if( cbs->reason == XmCR_OK )
	*answer = 1;
    else if( cbs->reason == XmCR_CANCEL )
	*answer = 0;
}

static void confirm3_callback(
  Widget		w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  XmAnyCallbackStruct	*cbs = (XmAnyCallbackStruct *) call_data;
  int			*answer = (int *) client_data;

  if( w ){/* to satisfy IRIX646 compiler */}

  if( !strcmp(XtName(w), "button2") ){
    *answer = 1;
  }
  else if( cbs->reason == XmCR_OK ){
    *answer = 0;
  }
  else if( cbs->reason == XmCR_CANCEL ){
    *answer = 2;
  }

  return;
}

/************************************************************************
* Function:     HGU_XmUserConfirm3	                                *
* Returns:      int:                    0, 1, 2				*
* Global Refs:  -                                                       *
* Purpose:      Popup a window with the text of "question" and three	*
*		buttons with labels "ans1", "ans2 and "ans2".		*
*		The window is destroyed and the routine returns when	*
*		either button is pressed with values 			*
*		 0 for ans1 and 1 for ans2 and 2 for ans3. This		*
*		procedure uses a standard Motif question dialog.	*
* Parameters:   Widget	w		a valid widget pointer		*
*               String	question	the question text		*
*			ans1		answer 1 button text		*
*		       	ans2		answer 2 button text		*
*			ans3		answer 3 button text		*
*		int	default_ans	!=0 then ans1 default else ans1	*
************************************************************************/
int HGU_XmUserConfirm3(
  Widget	w,
  String	question,
  String	ans1,
  String	ans2,
  String	ans3,
  int		default_ans)
{
  XtAppContext	app_con = XtWidgetToApplicationContext( w );
  Widget	dialog, button;
  XmString	text, str1, str2, str3, title;
  int		answer = -1;
  Visual	*visual;
  Arg		arg[1];

  /* get the visual explicitly */
  visual = HGU_XmWidgetToVisual(w);
  XtSetArg(arg[0], XmNvisual, visual);

    /* create the dialog widget */
  dialog = XmCreateQuestionDialog(w, "HGU_XmUserConfirm", arg, 1);
  XtVaSetValues(dialog,
		XmNdialogStyle,	XmDIALOG_FULL_APPLICATION_MODAL,
		NULL);
  XtSetSensitive(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON),
		 False);
  XtAddCallback(dialog, XmNokCallback, confirm3_callback, &answer);
  XtAddCallback(dialog, XmNcancelCallback, confirm3_callback, &answer);

  /* add an extra button */
  button = XtVaCreateManagedWidget("button2", xmPushButtonGadgetClass,
				   dialog, NULL);

  /* set the resources */
  text = XmStringCreateLtoR( question, XmSTRING_DEFAULT_CHARSET );
  str1 = XmStringCreateSimple( ans1 );
  str2 = XmStringCreateSimple( ans2 );
  str3 = XmStringCreateSimple( ans3 );
  title = XmStringCreateSimple( "Confirm Dialog" );

  XtVaSetValues(dialog,
		XmNmessageString,	text,
		XmNokLabelString,	str1,
		XmNcancelLabelString,	str3,
		XmNdialogTitle,	title,
		XmNdefaultButtonType,	default_ans?
		XmDIALOG_OK_BUTTON : XmDIALOG_CANCEL_BUTTON,
		NULL);
  XtVaSetValues(button,
		XmNlabelString,	str2,
		NULL);
  XtAddCallback(button, XmNactivateCallback, confirm3_callback, &answer);

  XmStringFree( text );
  XmStringFree( str1 );
  XmStringFree( str2 );
  XmStringFree( str3 );
  XmStringFree( title );

  /* popup widget and process events */
  XtManageChild( dialog );
  while( answer < 0 ){
    XtAppProcessEvent( app_con, XtIMAll );
    XSync( XtDisplay(dialog), 0 );
  }

  /* remove dialog, flush display and destroy resources */
  XtUnmanageChild( dialog );
  XSync( XtDisplay(dialog) , 0 );
  XmUpdateDisplay( dialog );
  XtDestroyWidget( dialog );

  return( answer );
}
/************************************************************************
* Function:     HGU_XmUserConfirm	                                *
* Returns:      int:                    ONE or ZERO			*
* Global Refs:  -                                                       *
* Purpose:      Popup a window with the text of "question" and two	*
*		buttons with labels "ans1" and "ans2". The window is	*
*		destroyed and the routine returns when either button is	*
*		pressed with values 1 for ans1 and 0 for ans2. This	*
*		procedure uses a standard Motif question dialog.	*
* Parameters:   Widget	w		a valid widget pointer		*
*               String	question	the question text		*
*			ans1		OK button text			*
*		       	ans2		CANCEL button text		*
*		int	default_ans	!=0 then ans1 default else ans1	*
************************************************************************/
int HGU_XmUserConfirm(
Widget	w,
String	question,
String	ans1,
String	ans2,
int	default_ans)
{
    XtAppContext	app_con = XtWidgetToApplicationContext( w );
    Widget		dialog;
    XmString		text, yes, no, title;
    int			answer = -1;
    Visual		*visual;
    Arg			arg[1];

    /* get the visual explicitly */
    visual = HGU_XmWidgetToVisual(w);
    XtSetArg(arg[0], XmNvisual, visual);

    /* create the dialog widget */
    dialog = XmCreateQuestionDialog(w, "HGU_XmUserConfirm", arg, 1);
    XtVaSetValues(dialog,
		  XmNdialogStyle,	XmDIALOG_FULL_APPLICATION_MODAL,
		  NULL);
    XtSetSensitive( XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON),
		   False);
    XtAddCallback(dialog, XmNokCallback, confirm_callback, &answer);
    XtAddCallback(dialog, XmNcancelCallback, confirm_callback, &answer);

    /* set the resources */
    text = XmStringCreateLtoR( question, XmSTRING_DEFAULT_CHARSET );
    yes = XmStringCreateSimple( ans1 );
    no = XmStringCreateSimple( ans2 );
    title = XmStringCreateSimple( "Confirm Dialog" );

    XtVaSetValues(dialog,
		  XmNmessageString,	text,
		  XmNokLabelString,	yes,
		  XmNcancelLabelString,	no,
		  XmNdialogTitle,	title,
		  XmNdefaultButtonType,	default_ans?
		  XmDIALOG_OK_BUTTON : XmDIALOG_CANCEL_BUTTON,
		  NULL);

    XmStringFree( text );
    XmStringFree( yes );
    XmStringFree( no );
    XmStringFree( title );

    /* popup widget and process events */
    XtManageChild( dialog );
    while( answer < 0 ){
	XtAppProcessEvent( app_con, XtIMAll );
	XSync( XtDisplay(dialog), 0 );
    }

    /* remove dialog, flush display and destroy resources */
    XtUnmanageChild( dialog );
    XSync( XtDisplay(dialog) , 0 );
    XmUpdateDisplay( dialog );
    XtDestroyWidget( dialog );

    return( answer );
}

int notice_confirm(
Widget	w,
String	message_str,
String	confirm_str,
String	deny_str,
Boolean	grab)
{
    if( grab ){/* to satisfy IRIX646 compiler */}

    return( HGU_XmUserConfirm(w, message_str, confirm_str, deny_str, 1) );
}

int notice_confirm_X(
Display	*dpy,
Window	win,
String	message_str,
String	confirm_str,
String	deny_str,
Boolean	grab)
{
    Widget	w = XtWindowToWidget(dpy, win);

    if( grab ){/* to satisfy IRIX646 compiler */}

    return( HGU_XmUserConfirm(w, message_str, confirm_str, deny_str, 1) );
}

/************************************************************************
* Function:     HGU_XmUserContinue	                                *
* Returns:      void:							*
* Global Refs:  -                                                       *
* Purpose:      Popup a window with the text of "question" and one	*
*		buttons with label "str". The window is			*
*		destroyed and the routine returns when the button is	*
*		pressed.. This	is useful for prompting an action and	*
*		waits until the button is pressed but process events	*
*		to allow user actions.					*
* Parameters:   Widget	w		a valid widget pointer		*
*               String	question	the question text		*
*			str		continue button text		*
************************************************************************/
void HGU_XmUserContinue(
  Widget	w,
  String	question,
  String	str)
{
    XtAppContext	app_con = XtWidgetToApplicationContext( w );
    Widget		dialog;
    XmString		text, yes, title;
    int			answer = -1;
    Visual		*visual;
    Arg			arg[1];

    /* get the visual explicitly */
    visual = HGU_XmWidgetToVisual(w);
    XtSetArg(arg[0], XmNvisual, visual);

    /* create the dialog widget */
    dialog = XmCreateMessageDialog(w, "HGU_XmUserConfirm", arg, 1);
    XtSetSensitive(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON),
		   False);
    XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
    XtVaSetValues(dialog, XmNdefaultButtonType, XmDIALOG_OK_BUTTON,
		  NULL);
    XtAddCallback(dialog, XmNokCallback, confirm_callback, &answer);

    /* set the resources */
    text = XmStringCreateLtoR( question, XmSTRING_DEFAULT_CHARSET );
    yes = XmStringCreateSimple( str );
    title = XmStringCreateSimple( "Continue Dialog" );

    XtVaSetValues(dialog,
		  XmNmessageString,	text,
		  XmNokLabelString,	yes,
		  XmNdialogTitle,	title,
		  NULL);

    XmStringFree( text );
    XmStringFree( yes );
    XmStringFree( title );

    /* popup widget and process events */
    XtManageChild( dialog );
    while( answer < 0 ){
	XtAppProcessEvent( app_con, XtIMAll );
	XSync( XtDisplay(dialog), 0 );
    }

    /* remove dialog, flush display and destroy resources */
    XtUnmanageChild( dialog );
    XSync( XtDisplay(dialog) , 0 );
    XmUpdateDisplay( dialog );
    XtDestroyWidget( dialog );

    return;
}

static String return_string;

/************************************************************************
* Function:     getstr_callback						*
* Returns:      void:		                    			*
* Global Refs:  return_string                                           *
* Purpose:      Private callback for HGU_XmUserGetstr			*
************************************************************************/
static void getstr_callback(
Widget		w,
XtPointer	client_data,
XtPointer	call_data)
{
    XmSelectionBoxCallbackStruct
	*cbs = (XmSelectionBoxCallbackStruct *) call_data;
    int	*answer = (int *) client_data;

    if( w ){/* to satisfy IRIX646 compiler */}

    if( cbs->reason == XmCR_OK )
    {
	*answer = 1;
	if(!XmStringGetLtoR(cbs->value, XmSTRING_DEFAULT_CHARSET,
			    &return_string))
	    return_string = NULL;
    }
    else if( cbs->reason == XmCR_CANCEL )
    {
	*answer = 0;
	return_string = NULL;
    }

}

/************************************************************************
* Function:     HGU_XmUserGetstr	                                *
* Returns:      String:		string input by user			*
* Global Refs:  return_string                                           *
* Purpose:      Popup a window with the text of "question", an input	*
*		text window and two buttons with labels "ans1" and	*
*		"ans2". The window is destroyed and the routine returns	*
*		when either button or Return is pressed.		*
*		If the OK button (ans1) is pressed then then the input	*
*		string (or the default) is return otherwise NULL is	*
*		returned. The return string can be freed with XFree().	*
* Parameters:   Widget	w		a valid widget pointer		*
*               String	question	the question text		*
*			ans1		OK button text			*
*		       	ans2		CANCEL button text		*
*			default_ans	!=0 then ans1 default else ans1	*
************************************************************************/
String HGU_XmUserGetstr(
Widget	w,
String	question,
String	ans1,
String	ans2,
String	default_ans)
{
    XtAppContext	app_con = XtWidgetToApplicationContext( w );
    Widget		dialog;
    XmString		text1, text2, yes, no, title;
    int			answer = -1;
    Visual		*visual;
    Arg			arg[1];

    /* get the visual explicitly */
    visual = HGU_XmWidgetToVisual(w);
    XtSetArg(arg[0], XmNvisual, visual);

    /* create the dialog widget */
    dialog = XmCreatePromptDialog(w, "HGU_XmUserGetstr", arg, 1);
    XtVaSetValues(dialog,
		  XmNdialogStyle,	XmDIALOG_FULL_APPLICATION_MODAL,
		  NULL);
    XtSetSensitive( XmSelectionBoxGetChild(dialog, XmDIALOG_HELP_BUTTON),
		   False);
    XtAddCallback(dialog, XmNokCallback, getstr_callback, &answer);
    XtAddCallback(dialog, XmNcancelCallback, getstr_callback, &answer);

    /* set the resources */ 
    text1 = XmStringCreateLtoR( question, XmSTRING_DEFAULT_CHARSET );
    text2 = XmStringCreateSimple( default_ans );
    yes = XmStringCreateSimple( ans1 );
    no = XmStringCreateSimple( ans2 );
    title = XmStringCreateSimple( "Getstr Dialog" );

    XtVaSetValues(dialog,
		  XmNselectionLabelString,	text1,
		  XmNtextString,		text2,
		  XmNokLabelString,		yes,
		  XmNcancelLabelString,		no,
		  XmNdialogTitle,		title,
		  XmNdefaultButtonType,		XmDIALOG_OK_BUTTON,
		  NULL);

    XmStringFree( text1 );
    XmStringFree( text2 );
    XmStringFree( yes );
    XmStringFree( no );
    XmStringFree( title );

    /* popup widget and process events */
    XtManageChild( dialog );
    while( answer < 0 ){
	XtAppProcessEvent( app_con, XtIMAll );
	XSync( XtDisplay(dialog), 0 );
    }

    /* remove dialog, flush display and destroy resources */
    XtUnmanageChild( dialog );
    XSync( XtDisplay(dialog) , 0 );
    XmUpdateDisplay( dialog );
    XtDestroyWidget( dialog );

    return( return_string );
}

static void popupGetFilenameCb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{
    XtManageChild( (Widget) client_data );
    return;
}

static void podownGetFilenameCb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{
    XtUnmanageChild( (Widget) client_data );
    return;
}

static void setStrGetFilenameCb(
Widget	w,
XtPointer	client_data,
XtPointer	call_data)
{
  XmFileSelectionBoxCallbackStruct
    *cbs = (XmFileSelectionBoxCallbackStruct *) call_data;
  Widget	dialog = (Widget) client_data;
  String	fileStr;
  XmString	xmFileStr;

  /* get the filename string */
  if( (fileStr = HGU_XmGetFileStr(w, cbs->value, cbs->dir)) == NULL ){
    return;
  }

  /* set the text area of the getstr dialog */
  xmFileStr = XmStringCreateSimple( fileStr );
  XtVaSetValues(dialog, XmNtextString, xmFileStr, NULL);
  XmStringFree( xmFileStr );
  XtFree(fileStr);

  return;
}

/************************************************************************
* Function:     HGU_XmUserGetFilename	                                *
* Returns:      String:		string input by user			*
* Global Refs:  return_string                                           *
* Purpose:      Popup a window with the text of "question", an input	*
*		text window and two buttons with labels "ans1" and	*
*		"ans2". The window is destroyed and the routine returns	*
*		when either button or Return is pressed.		*
*		If the OK button (ans1) is pressed then then the input	*
*		string (or the default) is return otherwise NULL is	*
*		returned. The return string can be freed with XFree().	*
* Parameters:   Widget	w		a valid widget pointer		*
*               String	question	the question text		*
*			ans1		OK button text			*
*		       	ans2		CANCEL button text		*
*			default_ans	!=0 then ans1 default else ans1	*
************************************************************************/
String HGU_XmUserGetFilename(
Widget	w,
String	question,
String	ans1,
String	ans2,
String	default_ans,
String	dir_str,
String	pattern_str)
{
    XtAppContext	app_con = XtWidgetToApplicationContext( w );
    Widget		dialog, button, filename_dialog;
    XmString		text1, text2, yes, no, browse, title;
    int			answer = -1;
    Visual		*visual;
    Arg			arg[1];

    /* get the visual explicitly */
    visual = HGU_XmWidgetToVisual(w);
    XtSetArg(arg[0], XmNvisual, visual);

    /* create the dialog widget */
    dialog = XmCreatePromptDialog(w, "HGU_XmUserGetFilename", arg, 1);
    XtVaSetValues(dialog,
		  XmNdialogStyle,	XmDIALOG_FULL_APPLICATION_MODAL,
		  NULL);
    XtSetSensitive( XmSelectionBoxGetChild(dialog, XmDIALOG_HELP_BUTTON),
		   False);
    button = XmSelectionBoxGetChild(dialog, XmDIALOG_APPLY_BUTTON);
    XtManageChild(button);

    /* create the file selection dialog - unmanaged */
    filename_dialog = XmCreateFileSelectionDialog(dialog,
						  "GetFilename_dialog",
						  arg, 1);
    XtSetSensitive(XmSelectionBoxGetChild(filename_dialog, XmDIALOG_HELP_BUTTON),
		   False);
    XtAddCallback(filename_dialog, XmNcancelCallback, podownGetFilenameCb,
		  (XtPointer) filename_dialog);
    XtAddCallback(filename_dialog, XmNokCallback, setStrGetFilenameCb,
		  (XtPointer) dialog);
    XtAddCallback(filename_dialog, XmNokCallback, podownGetFilenameCb,
		  (XtPointer) filename_dialog);

    /* add callbacks */
    XtAddCallback(dialog, XmNokCallback, getstr_callback, &answer);
    XtAddCallback(dialog, XmNcancelCallback, getstr_callback, &answer);
    XtAddCallback(dialog, XmNapplyCallback, popupGetFilenameCb,
		  (XtPointer) filename_dialog);

    /* set the resources */ 
    text1 = XmStringCreateLtoR( question, XmSTRING_DEFAULT_CHARSET );
    text2 = XmStringCreateSimple( default_ans );
    yes = XmStringCreateSimple( ans1 );
    no = XmStringCreateSimple( ans2 );
    browse = XmStringCreateSimple( "Browse" );
    title = XmStringCreateSimple( "Get Filename Dialog" );

    XtVaSetValues(dialog,
		  XmNselectionLabelString,	text1,
		  XmNtextString,		text2,
		  XmNokLabelString,		yes,
		  XmNcancelLabelString,		no,
		  XmNapplyLabelString,		browse,
		  XmNdialogTitle,		title,
		  XmNdefaultButtonType,		XmDIALOG_OK_BUTTON,
		  NULL);

    XmStringFree( text1 );
    XmStringFree( text2 );
    XmStringFree( yes );
    XmStringFree( no );
    XmStringFree( browse );
    XmStringFree( title );

    /* set parameters of the file selection box */
    title = XmStringCreateSimple( "Select Filename Dialog" );
    XtVaSetValues(filename_dialog,
		  XmNdialogTitle,		title,
		  NULL);
    XmStringFree( title );
    if( dir_str ){
      text1 = XmStringCreateSimple( dir_str );
      XtVaSetValues(filename_dialog, XmNdirectory, text1, NULL);
      XmStringFree( text1 );
    }
    if( pattern_str ){
      text1 = XmStringCreateSimple( pattern_str );
      XtVaSetValues(filename_dialog, XmNpattern, text1, NULL);
      XmStringFree( text1 );
    }

    /* popup widget and process events */
    XtManageChild( dialog );
    while( answer < 0 ){
	XtAppProcessEvent( app_con, XtIMAll );
	XSync( XtDisplay(dialog), 0 );
    }

    /* remove dialog, flush display and destroy resources */
    XtUnmanageChild( dialog );
    XSync( XtDisplay(dialog) , 0 );
    XmUpdateDisplay( dialog );
    XtDestroyWidget( dialog );

    return( return_string );
}

String notice_getstr(
Widget	w,
String	message_str,
String	default_str,
Boolean	grab)
{
    if( grab ){/* to satisfy IRIX646 compiler */}

    return( HGU_XmUserGetstr(w, message_str, "OK", "Cancel", default_str) );
}

String notice_getstr_X(
Display	*dpy,
Window	win,
String	message_str,
String	default_str,
Boolean	grab)
{
    Widget	w = XtWindowToWidget(dpy, win);

    if( grab ){/* to satisfy IRIX646 compiler */}

    return( HGU_XmUserGetstr(w, message_str, "OK", "Cancel", default_str) );
}

static void HGU_XmUserStdDialog(
Widget		w,
String		str,
String		title_str,
unsigned char	dialog_type,
unsigned char	modality)
{
    Widget		dialog;
    XmString		text, title;
    Visual		*visual;
    Arg			arg[1];

    /* get the visual explicitly */
    visual = HGU_XmWidgetToVisual(w);
    XtSetArg(arg[0], XmNvisual, visual);

    /* create the dialog widget */
    switch( dialog_type ){
    case XmDIALOG_ERROR:
	dialog = XmCreateErrorDialog(w, "HGU_XmUserError", arg, 1);
	XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
	XtVaSetValues(dialog, XmNdefaultButtonType, XmDIALOG_OK_BUTTON,
		      NULL);
	break;
    case XmDIALOG_INFORMATION:
	dialog = XmCreateInformationDialog(w, "HGU_XmUserInfo", arg, 1);
	XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_OK_BUTTON));
	XtVaSetValues(dialog, XmNdefaultButtonType, XmDIALOG_CANCEL_BUTTON,
		      NULL);
	break;
    case XmDIALOG_MESSAGE:
	dialog = XmCreateMessageDialog(w, "HGU_XmUserMessage", arg, 1);
	XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
	XtVaSetValues(dialog, XmNdefaultButtonType, XmDIALOG_OK_BUTTON,
		      NULL);
	break;
    default:
	return;
    }

    XtSetSensitive( XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON),
		   False);

    XtAddCallback(dialog, XmNokCallback, PopdownCallback, NULL);
    XtAddCallback(dialog, XmNokCallback,
		  (XtCallbackProc) XtDestroyWidget, NULL);

    /* set the resources */
    text  = XmStringCreateLtoR( str, XmSTRING_DEFAULT_CHARSET );
    title = XmStringCreateSimple( title_str );

    XtVaSetValues(dialog,
		  XmNmessageString,	text,
		  XmNdialogTitle,	title,
		  XmNdialogStyle,	modality,
		  NULL);

    XmStringFree( text );
    XmStringFree( title );

    /* popup widget and process events */
    XtManageChild( dialog );
    XtPopup(XtParent(dialog), XtGrabNone);

    return;
}
/************************************************************************
* Function:     HGU_XmUserInfo		                                *
* Returns:      void							*
* Global Refs:  -                                                       *
* Purpose:      Popup an information dialog with the text of "str"	*
*		and modality "modality". The procedure returns		*
*		immediately and the dialog widget is destroyed on	*
*		pressing the Cancel button. The Help button		*
*		is insensitive and inaccessible				*
* Parameters:   Widget	w		a valid widget pointer		*
*               String	str		the information text		*
*		unsigned char modality	the required modality		*
************************************************************************/
void HGU_XmUserInfo(
Widget		w,
String		str,
unsigned char	modality)
{
    HGU_XmUserStdDialog(w, str, "Information Dialog", XmDIALOG_INFORMATION,
			modality);
    return;
}
/************************************************************************
* Function:     HGU_XmUserMessage	                                *
* Returns:      void							*
* Global Refs:  -                                                       *
* Purpose:      Popup message dialog with the text of "str"		*
*		and modality "modality". The procedure returns		*
*		immediately and the dialog widget is destroyed on	*
*		pressing the Ok button. The Help button			*
*		is insensitive and inaccessible				*
* Parameters:   Widget	w		a valid widget pointer		*
*               String	str		the information text		*
*		unsigned char modality	the required modality		*
************************************************************************/
void HGU_XmUserMessage(
Widget		w,
String		str,
unsigned char	modality)
{
    HGU_XmUserStdDialog(w, str, "Message Dialog", XmDIALOG_MESSAGE, modality);
    return;
}
/************************************************************************
* Function:     HGU_XmUserError		                                *
* Returns:      void							*
* Global Refs:  -                                                       *
* Purpose:      Popup error dialog with the text of "str"		*
*		and modality "modality". The procedure returns		*
*		immediately and the dialog widget is destroyed on	*
*		pressing the Ok button. The Help button			*
*		is insensitive and inaccessible				*
* Parameters:   Widget	w		a valid widget pointer		*
*               String	str		the information text		*
*		unsigned char modality	the required modality		*
************************************************************************/
void HGU_XmUserError(
Widget		w,
String		str,
unsigned char	modality)
{
    HGU_XmUserStdDialog(w, str, "Error Dialog", XmDIALOG_ERROR, modality);
    return;
}
/************************************************************************
* Function:     HGU_XmUserWarning	                                *
* Returns:      int:                    ONE or ZERO			*
* Global Refs:  -                                                       *
* Purpose:      Popup a window with the text of "warning".The window is	*
*		destroyed and the routine returns 1 or 0 when Ok or 	*
*		Cancel is pressed respectively.				*
* Parameters:   Widget	w		a valid widget pointer		*
*               String	warning 	the warning text		*
*		int	default_ans	!=0 then Ok is the default 	*
*					answer.				*
************************************************************************/
int HGU_XmUserWarning(
Widget	w,
String	warning,
int	default_ans)
{
    XtAppContext	app_con = XtWidgetToApplicationContext( w );
    Widget		dialog;
    XmString		text, title;
    int			answer = -1;
    Visual		*visual;
    Arg			arg[1];

    /* get the visual explicitly */
    visual = HGU_XmWidgetToVisual(w);
    XtSetArg(arg[0], XmNvisual, visual);

    /* create the dialog widget */
    dialog = XmCreateWarningDialog(w, "HGU_XmUserWarning", arg, 1);
    XtVaSetValues(dialog,
		  XmNdialogStyle,	XmDIALOG_FULL_APPLICATION_MODAL,
		  NULL);
    XtSetSensitive( XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON),
		   False);
    XtAddCallback(dialog, XmNokCallback, confirm_callback, &answer);
    XtAddCallback(dialog, XmNcancelCallback, confirm_callback, &answer);

    /* set the resources */
    text = XmStringCreateLtoR( warning, XmSTRING_DEFAULT_CHARSET );
    title = XmStringCreateSimple( "Warning Dialog" );

    XtVaSetValues(dialog,
		  XmNmessageString,	text,
		  XmNdialogTitle,	title,
		  XmNdefaultButtonType,	default_ans?
		  XmDIALOG_OK_BUTTON : XmDIALOG_CANCEL_BUTTON,
		  NULL);

    XmStringFree( text );
    XmStringFree( title );

    /* popup widget and process events */
    XtManageChild( dialog );
    while( answer < 0 ){
	XtAppProcessEvent( app_con, XtIMAll );
	XSync( XtDisplay(dialog), 0 );
    }

    /* remove dialog, flush display and destroy resources */
    XtUnmanageChild( dialog );
    XSync( XtDisplay(dialog) , 0 );
    XmUpdateDisplay( dialog );
    XtDestroyWidget( dialog );

    return( answer );
}
