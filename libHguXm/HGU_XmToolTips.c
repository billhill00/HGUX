#pragma ident "MRC HGU $Id$"
/*!
* \file         HGU_XmToolTips.c
* \author       richard <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Tue Dec  9 08:13:25 2003
* \version      MRC HGU $Id$
*               $Revision$
*               $Name$
* \par Copyright:
*               1994-2002 Medical Research Council, UK.
*               All rights reserved.
* \par Address:
*               MRC Human Genetics Unit,
*               Western General Hospital,
*               Edinburgh, EH4 2XU, UK.
* \ingroup      XmUtils
* \brief        Functions to support tool tips in Motif GUI applications.
*               
* \todo         -
* \bug          None known
*
* Maintenance log with most recent changes at top of list.
*/

#include <stdio.h>
#include <stdlib.h>

#include <XM/XmAll.h>

/* default delays in milli-secs before tip popup and removal*/
#define HGU_XmTOOLTIP_DELAY 500
#define HGU_XmTOOLTIP_LONG_DELAY 3000

/* default num characters before first break */
#define HGU_XmTOOTIP_TEXT_BREAK 20

/* tooltip help taken from qtip example */
/* modified to provide alternative placement
   so the tip is not always bottom-middle of the widget */
typedef struct _HGU_XmToolTipData {
  Widget	widget;
  XmString	helpStr;
  Position	x;
  Position	y;
} HGU_XmToolTipData;

/* some private statics */
static XtIntervalId HGU_XmToolTipTimerId=0L;
static int	HGU_XmToolTipTimout;
static Widget	HGU_XmToolTipShellW=(Widget) NULL;
static Widget	HGU_XmToolTipLabelW=(Widget) NULL;
static XmFontList HGU_XmToolTipFList=NULL;
static int	HGU_XmToolTipDelay=HGU_XmTOOLTIP_DELAY;
static int	HGU_XmToolTipLongDelay=HGU_XmTOOLTIP_LONG_DELAY;
static int	HGU_XmToolTipTextBreakWidth=HGU_XmTOOTIP_TEXT_BREAK;


/*
** from Dan H's Motif book from O'Rielly
*/
Widget HGU_XmGetTopShell(
  Widget 	w)
{
  while ((w != (Widget) NULL) && (XtIsWMShell(w) == False)){
    w=XtParent(w);
  }

  return w;
}

void HGU_XmToolTipSetDelays(
  int	delay,
  int	longDelay)
{
  HGU_XmToolTipDelay = delay;
  HGU_XmToolTipLongDelay = longDelay;
  return;
}

void HGU_XmToolTipSetTextBreak(
  int	textBreak)
{
  HGU_XmToolTipTextBreakWidth = textBreak;
  return;
}

String HGU_XmToolTipStringBreak(
  String	str)
{
  String	rtnStr=NULL;
  int		i, j, k;

  if( str ){
    i = strlen(str) + strlen(str)/HGU_XmToolTipTextBreakWidth + 4;
    rtnStr = (String) AlcCalloc(i, sizeof(char));
    for(i=0, j=0, k=0; i < strlen(str); i++){
      if((k > HGU_XmToolTipTextBreakWidth) && (str[i] == ' ')){
	rtnStr[j++] = '\n';
	k = 0;
      }
      else {
	rtnStr[j++] = str[i];
	k++;
      }
    }
    rtnStr[j] = '\0';
  }

  return rtnStr;
}

void HGU_XmDropToolTip(
  XtPointer	client_data,
  XtIntervalId	*id)
{
  HGU_XmToolTipTimout = 0;
  HGU_XmToolTipTimerId = 0L;
  if( HGU_XmToolTipShellW ){
    if( XtIsManaged(HGU_XmToolTipShellW) ){
      XtUnmanageChild(HGU_XmToolTipShellW);
    }
  }

  return;
}

void HGU_XmPopToolTip(
  XtPointer	client_data,
  XtIntervalId	*id)
{
  HGU_XmToolTipData *data=(HGU_XmToolTipData *) client_data;
  XtAppContext	appCon;

  HGU_XmToolTipTimerId = 0L;
  HGU_XmToolTipTimout = 0;

  /*
  ** it's possible, a time consuming call was made and mouse moved
  ** out of the window, but the event is lost, so the help window
  ** might still be up..bring it down, # Oct-22-96 
  */
  if (HGU_XmToolTipShellW != (Widget) NULL){
    if (XtIsManaged(HGU_XmToolTipShellW)){
      XtUnmanageChild(HGU_XmToolTipShellW);
    }
  }

  if (HGU_XmToolTipFList == NULL){
    XtVaGetValues(HGU_XmToolTipLabelW,
		  XmNfontList, &HGU_XmToolTipFList,
		  NULL);
  }

  if( data->helpStr && HGU_XmToolTipFList){
    Dimension	lab_w, lab_h;

    lab_w = XmStringWidth(HGU_XmToolTipFList, data->helpStr)+6;
    lab_h = XmStringHeight(HGU_XmToolTipFList, data->helpStr)+6;

    /* reset x position if outside the screen */
    if((data->x + lab_w) > WidthOfScreen(XtScreen(HGU_XmToolTipShellW))){
      data->x = WidthOfScreen(XtScreen(HGU_XmToolTipShellW)) - lab_w;
    }

    XtVaSetValues(HGU_XmToolTipShellW,
		  XmNx,data->x,
		  XmNy,data->y,
		  XmNwidth,lab_w,
		  XmNheight,lab_h,
		  NULL);
    XtVaSetValues(HGU_XmToolTipLabelW,
		  XmNlabelString,data->helpStr,
		  NULL);

    XtManageChild(HGU_XmToolTipShellW);
    XRaiseWindow(XtDisplay(HGU_XmToolTipShellW),XtWindow(HGU_XmToolTipShellW));

    /* add a timeout to remove it */
    appCon = XtWidgetToApplicationContext(HGU_XmToolTipShellW);
    HGU_XmToolTipTimerId=XtAppAddTimeOut(appCon,
					 HGU_XmToolTipLongDelay,
					 HGU_XmDropToolTip,
					 NULL);
  }

  return;
}

void HGU_XmToolTipHelpEventHandler(
  Widget        w,
  XtPointer     client_data,
  XEvent        *event,
  Boolean       *continue_to_dispatch)
{
  XtAppContext	appCon = XtWidgetToApplicationContext(w);
  HGU_XmToolTipData *data=(HGU_XmToolTipData *) client_data;
  Dimension	width, height;

  switch( event->type ){
  case EnterNotify: /* set position to mid-widget, bottom */
    if( !HGU_XmToolTipTimerId ){
      HGU_XmToolTipTimout=1;
      XtVaGetValues(data->widget,
		    XmNwidth,&width,
		    XmNheight,&height,
		    NULL);
      XtTranslateCoords(data->widget,
			(Position) (width/2),
			(Position) (height+2),
			&(data->x),&(data->y));
      
      HGU_XmToolTipTimerId=XtAppAddTimeOut(appCon,
					   HGU_XmToolTipDelay,
					   HGU_XmPopToolTip,
					   client_data);
    }
    break;

  case MotionNotify:
    /* first clear it then reset */
    if( HGU_XmToolTipTimout ){
      XtRemoveTimeOut(HGU_XmToolTipTimerId);
      HGU_XmDropToolTip(client_data, &HGU_XmToolTipTimerId);
    }
    /* and now reset */
    HGU_XmToolTipTimout=1;
    if( XtIsSubclass(w, xmLabelWidgetClass) ){
      XtVaGetValues(data->widget,
		    XmNwidth,&width,
		    XmNheight,&height,
		    NULL);
      XtTranslateCoords(data->widget,
			(Position) (width/2),
			(Position) (height+2),
			&(data->x),&(data->y));
    }
    else {
      data->x = event->xmotion.x_root+2;
      data->y = event->xmotion.y_root+2;
    }
    HGU_XmToolTipTimerId=XtAppAddTimeOut(appCon,
					 HGU_XmToolTipDelay,
					 HGU_XmPopToolTip,
					 client_data);
    break;

  case LeaveNotify:
    if( HGU_XmToolTipTimout ){
      XtRemoveTimeOut(HGU_XmToolTipTimerId);
      HGU_XmDropToolTip(client_data, &HGU_XmToolTipTimerId);
    }
    break;

  default:
    break;
  }
    
  return;
}

void HGU_XmAddToolTip(
  Widget	topl,
  Widget	w,
  String	helpStr)
{
  HGU_XmToolTipData	*data;

  /* check inputs, must be a non-NULL string and
     a sub-class of widget */
  if( helpStr == NULL ){
    return;
  }
  if( XtIsWidget(w) == False ){
    return;
  }

  /* if this is the first call then create the tip shell and
     label widgets */
  if( HGU_XmToolTipShellW == NULL ){
    /* note overrideRedirect cuts out window manager
       interventioan and decoration */
    HGU_XmToolTipShellW = 
      XtVaCreatePopupShell("toolTipShell",
			   topLevelShellWidgetClass,
			   HGU_XmGetTopShell(topl),
			   XmNoverrideRedirect, True,
			   XmNresizePolicy, XmRESIZE_ANY,
			   NULL);

    HGU_XmToolTipLabelW = 
      XtVaCreateManagedWidget("toolTipLabel",
			      xmLabelWidgetClass, HGU_XmToolTipShellW,
			      NULL);
  }

  /* create a structure to pass to the event handler */
  if( data = (HGU_XmToolTipData *) XtNew(HGU_XmToolTipData) ){
    String	tmpStr;
    data->widget = w;
    tmpStr = HGU_XmToolTipStringBreak(helpStr);
    data->helpStr = XmStringGenerate(tmpStr, NULL, XmMULTIBYTE_TEXT, NULL);
    AlcFree(tmpStr);

    /* add an event handler */
    if( XtIsSubclass(w, xmLabelWidgetClass) ){
      XtAddEventHandler(w,PointerMotionMask|
			EnterWindowMask|LeaveWindowMask,
			False, HGU_XmToolTipHelpEventHandler,
			(XtPointer) data);
    }
    else {
      XtAddEventHandler(w,PointerMotionMask|
			EnterWindowMask|LeaveWindowMask,
			False, HGU_XmToolTipHelpEventHandler,
			(XtPointer) data);
    }
  }

  return;
}

