#if defined(__GNUC__)
#ident "MRC HGU $Id:"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id:"
#else static char _HGU_XmSetInt_c[] = "MRC HGU $Id:";
#endif
#endif
/*!
* \file         HGU_XmSetInt.c
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Wed Apr 29 09:14:49 2009
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
* \brief        
*               
* \todo         -
* \bug          None known
*
* Maintenance log with most recent changes at top of list.
*/

#include <stdio.h>
#include <stdlib.h>

#include <Xm/XmAll.h>

#include <HGU_XmUtils.h>
#include <Alc.h>

static void set_new_val(
  int				new_val,
  HGU_XmSetIntControlStruct	*controlStr)
{
  char	str[20];
  int	cbFlg;

  if( new_val > controlStr->maxval ){
    new_val = controlStr->maxval;
    XBell(XtDisplayOfObject(controlStr->text), 100);
  }
  if( new_val < controlStr->minval ){
    new_val = controlStr->minval;
    XBell(XtDisplayOfObject(controlStr->text), 100);
  }

  cbFlg = (controlStr->val != new_val);

  controlStr->val = new_val;
  (void) sprintf(str, "%d", new_val);
  XtVaSetValues(controlStr->text,
		XmNvalue,	str,
		XmNcursorPosition,	0,
		NULL);

  if( cbFlg && controlStr->cb_func ){
    (*(controlStr->cb_func))(XtParent(controlStr->text), controlStr->cb_data,
			     (XtPointer) new_val);
  }

  return;
}

static void increment_val_cb(
  Widget	w,
  XtPointer	user_data,
  XtPointer	call_data)
{
  XmArrowButtonCallbackStruct	*arrowCbs;
  HGU_XmSetIntControlStruct	*controlStr;
  int	  			incr = (int) user_data, new_val;

  XtVaGetValues(XtParent(w), XmNuserData, &controlStr, NULL);
  arrowCbs = (XmArrowButtonCallbackStruct *) call_data;
  new_val = controlStr->val + arrowCbs->click_count * incr;

  set_new_val(new_val, controlStr);
  
  return;
}

static void set_from_text_cb(
  Widget	w,
  XtPointer	user_data,
  XtPointer	call_data)
{
  HGU_XmSetIntControlStruct	*controlStr;
  String			strVal;
  int				new_val;

  XtVaGetValues(XtParent(w), XmNuserData, &controlStr, NULL);
  XtVaGetValues(controlStr->text, XmNvalue, &strVal, NULL);

  if( strVal == NULL ){
    XBell(XtDisplayOfObject(w), 100);
    set_new_val(controlStr->val, controlStr);
    return;
  }
  if( sscanf(strVal, "%d", &new_val) < 1 ){
    XBell(XtDisplayOfObject(w), 100);
    set_new_val(controlStr->val, controlStr);
  }
  else {
    set_new_val(new_val, controlStr);
  }
  
  return;
}

static void SetIntDestroyCb(
  Widget	w,
  XtPointer	user_data,
  XtPointer	call_data)
{
  HGU_XmSetIntControlStruct	*controlStr;

  XtVaGetValues(w, XmNuserData, &controlStr, NULL);
  if( controlStr ){
    AlcFree( (void *) controlStr );
  }
  return;
}

Widget HGU_XmCreateSetIntControl(
  String		name,
  Widget		parent,
  int			val,
  int			minval,
  int			maxval,
  XtCallbackProc	cb_func,
  XtPointer		cb_data)
{
    Widget	base, label, arrowup, arrowdn, text;
    char	str[40];
    int		len;
    HGU_XmSetIntControlStruct	*controlStr;

    if( minval >= maxval ){
      fprintf(stderr, "HGU_XmCreateSetIntControl: minval >= maxval - defaults set\n");
      minval = 0;
      maxval = 100;
      val = 50;
    }
    if( val < minval ){
      fprintf(stderr, "HGU_XmCreateSetIntControl: val < minval\n");
      val = minval;
    }
    if( val > maxval ){
      fprintf(stderr, "HGU_XmCreateSetIntControl: val > maxval\n");
      val = maxval;
    }

    base = XtVaCreateWidget(name, xmFormWidgetClass, parent,
			    XmNborderWidth,	0,
			    NULL);

    label = XtVaCreateManagedWidget(name, xmLabelWidgetClass, base,
				    XmNtopAttachment,	XmATTACH_FORM,
				    XmNbottomAttachment,XmATTACH_FORM,
				    XmNleftAttachment,	XmATTACH_FORM,
				    XmNborderWidth,	0,
				    NULL);

    len = (abs(minval) > abs(maxval)) ? abs(minval) : abs(maxval);
    (void) sprintf(str, "%d", len);
    len = (int) strlen(str) + (minval<0)?1:0;
    (void) sprintf(str, "%d", val);

    arrowdn = XtVaCreateManagedWidget("arrowdn", xmArrowButtonWidgetClass,
				      base,
				      XmNarrowDirection,	XmARROW_DOWN,
				      XmNtopAttachment,		XmATTACH_FORM,
				      XmNrightAttachment,	XmATTACH_FORM,
				      XmNbottomAttachment,	XmATTACH_FORM,
				      XmNborderWidth,	0,
				      XmNshadowThickness,	0,
				      NULL);
    arrowup = XtVaCreateManagedWidget("arrowup", xmArrowButtonWidgetClass,
				      base,
				      XmNarrowDirection,	XmARROW_UP,
				      XmNtopAttachment,		XmATTACH_FORM,
				      XmNbottomAttachment,	XmATTACH_FORM,
				      XmNrightAttachment,	XmATTACH_WIDGET,
				      XmNrightWidget,		arrowdn,
				      XmNborderWidth,		0,
				      XmNshadowThickness,	0,
				      NULL);
    text = XtVaCreateManagedWidget("text", xmTextWidgetClass, base,
				   XmNtopAttachment,	XmATTACH_FORM,
				   XmNbottomAttachment, XmATTACH_FORM,
				   XmNleftAttachment,	XmATTACH_WIDGET,
				   XmNleftWidget,	label,
				   XmNrightAttachment,	XmATTACH_WIDGET,
				   XmNrightWidget,	arrowup,
				   XmNborderWidth,	0,
				   XmNshadowThickness,	0,
				   XmNvalue,		str,
				   XmNcursorPosition,	0,
				   XmNcolumns,		len+1,
				   NULL);

    XtAddCallback(arrowup, XmNactivateCallback, increment_val_cb,(XtPointer) 1);
    XtAddCallback(arrowdn, XmNactivateCallback, increment_val_cb,(XtPointer) -1);
    XtAddCallback(text, XmNactivateCallback, set_from_text_cb, NULL);
    XtAddCallback(text, XmNlosingFocusCallback, set_from_text_cb, NULL);
    XtAddCallback(base, XmNdestroyCallback, SetIntDestroyCb, NULL);

    controlStr = (HGU_XmSetIntControlStruct *)
      AlcMalloc(sizeof(HGU_XmSetIntControlStruct));
    controlStr->val = val;
    controlStr->minval = minval;
    controlStr->maxval = maxval;
    controlStr->text = text;
    controlStr->cb_func = cb_func;
    controlStr->cb_data = cb_data;
    XtVaSetValues(base, XmNuserData, (XtPointer *) controlStr, NULL);

    XtManageChild( base );
    return( base );
}

int HGU_XmGetSetIntValue(
  Widget	w)
{
  HGU_XmSetIntControlStruct	*controlStr;

  XtVaGetValues(w, XmNuserData, &controlStr, NULL);

  if( controlStr == NULL ){
    fprintf(stderr, "HGU_XmGetSetIntValue: NULL control data\n");
    return( 0 );
  }

  return( controlStr->val );
}

int HGU_XmSetSetIntValue(
  Widget	w,
  int		val)
{
  HGU_XmSetIntControlStruct	*controlStr;
  XtCallbackProc		cb_func;

  XtVaGetValues(w, XmNuserData, &controlStr, NULL);

  if( controlStr == NULL ){
    fprintf(stderr, "HGU_XmGetSetIntValue: NULL control data\n");
    return( 0 );
  }

  cb_func = controlStr->cb_func;
  controlStr->cb_func = NULL;
  set_new_val(val, controlStr);
  controlStr->cb_func = cb_func;

  return( 1 );
}
