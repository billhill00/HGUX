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
* File        :    HGU_XmSlider.c					     *
* $Revision$
******************************************************************************
* Author Name :     Richard Baldock					     *
* Author Login:    richard@hgu.mrc.ac.uk				     *
* Date        :    Thu Aug 25 09:58:03 1994				     *
* Synopsis:	Implement a slider user interface element. This is not	     *
*		a widget implementation but a few convenience routines	     *
*		to return a composite widget (Form) with Label, Scale	     *
*		and Text widget children set up to behave as a slider	     *
*		The top-level and label widget have the widget name	     *
*		given as argument to CreateHorizontalSlider, the other	     *
*		children widgets have the names "scale" and "text"	     *
*		respectively. The slider values can be changed using	     *
*		all the usual Scale widget actions plus the setting	     *
*		of the text. The text value will update the scale	     *
*		value only on pressing <return>.			     *
*		The cb_func is added to the XmNvalueChangedCallback	     *
*		only. If you want to get drag changes then install the	     *
*		callback on the scale XmNdragCallback list explicitly.	     *
* Functions:								     *
*	HGU_XmCreateHorizontalSliderWidget() - horizontal slider	     *
*	HGU_XmCreateHorizontalLogSliderWidget() - log slider		     *
*	HGU_XmCreateHorizontalExpSliderWidget() - exp slider		     *
*	HGU_XmCreateHorizontalFuncSliderWidget() - arbitrary function	     *
*		slider.							     *
*	HGU_XmSetSliderValue - set the slider value, this will update	     *
*	 	the display but will not call the callbacks.		     *
*	HGU_XmGetSliderValue - returns the slider value. Note in all         *
*		these procedures the values are floats although the	     *
*		scale itself does everything in int. The precision is	     *
*		controlled by the argument decimalPoints.		     *
*									     *
*		See the Motif manual for more details on the Scale and	     *
*		Text widgets.						     *
*****************************************************************************/
#include <stdio.h>

#if defined SUNOS5 || defined IRIX5 || defined IRIX646
#include <stdlib.h>
#endif /* SUNOS5 || IRIX5 || IRIX646 */

#include <math.h>

#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/Scale.h>
#include <Xm/Text.h>

#include <HGU_XmUtils.h>

#if defined SUNOS5 || defined IRIX5 || defined IRIX646
#define nint(a) ((a<0)?(int)(a-0.5):(int)(a+0.5))
#endif /* SUNOS5 || IRIX5 || IRIX646 */
 
/************************************************************************
* Function:	int slider_scale_value()				*
* Purpose:	Calculate the integer value corrsponding to the given 	*
*		string and number of decimal points which can be used	*
*		to update the scale position.				*
* Returns:	the value or 0 if scanf fails.				*
* Arguments:
************************************************************************/
static int slider_scale_value(
String			stringValue,
int			points,
int			min,
int			max,
HGU_XmSliderFunc	func)
{
    float	value=0.0;
    int		val;

    sscanf( stringValue, "%f", &value );
    while( points-- > 0 )
	value *= 10;

    if( func != NULL )
	value = min + (max - min) *
	    (*func)( (float)(value - min)/(max-min), 1 );
    val = nint((double) value);

    return( val );
}
/************************************************************************
* Function:	String slider_text_string()				*
* Purpose:	Form the string corresponding to a particular scale	*
*		value and number of decimal points.			*
* Returns:	the given string pointer - as in sprintf etc.		*
* Arguments:
************************************************************************/
static String slider_text_string(
String			str,
int			val,
int			points,
int			min,
int			max,
HGU_XmSliderFunc	func)
{
    char	format[16];
    float	value=(float) val;
    
    sprintf(format, "%%-.%df", points);

    if( func != NULL )
	value = min + (max - min) *
	    (*func)((float)(value - min)/(max - min), 0);

    while( points-- > 0 )
	value /= 10.0;

    sprintf(str, format, value);
    return( str );
}
/************************************************************************
* Function:	void update_slider_value()				*
* Purpose:	Callback function used to update the the scale value	*
*		in response to a value input in the text field		*
*		All the XmNvalueChangedCallbacks are called		*
* Returns:								*
* Arguments:
************************************************************************/
static void update_slider_value(Widget		text,
				Widget		scale,
				XmAnyCallbackStruct	*cbs)
{
    short	points;
    int		val, min, max, oldval;
    HGU_XmSliderFunc	func;
    String	string;
    XmScaleCallbackStruct	scale_cbs;

    XtVaGetValues(text, XmNvalue, &string, NULL);
    XtVaGetValues(scale, XmNdecimalPoints, &points,
		  XmNminimum, &min, XmNmaximum, &max,
		  XmNuserData, &func, NULL);

    val = slider_scale_value(string, points, min, max, func);
    XtVaGetValues(scale, XmNvalue, &oldval, NULL);
    if( val == oldval ){
      return;
    }

    val = (val > min) ? val : min;
    val = (val < max) ? val : max;
    XtVaSetValues(scale, XmNvalue, val, NULL);

    scale_cbs.reason = XmCR_VALUE_CHANGED;
    scale_cbs.event  = (cbs == NULL) ? NULL : cbs->event;
    scale_cbs.value  = val;
    XtCallCallbacks( scale, XmNvalueChangedCallback, &scale_cbs );
}
/************************************************************************
* Function:	void update_slider_text()				*
* Purpose:	Callback function used to update the the text value	*
*		in response to a scale change either dragged or		*
*		otherwise. No callbacks are called.			*
* Returns:								*
* Arguments:
************************************************************************/
static void update_slider_text(Widget		scale,
			       Widget		text,
			       XmScaleCallbackStruct	*cbs)
{
    short	decimalPoints;
    char	string[128];
    int		position;
    int		min, max;
    HGU_XmSliderFunc	func;

    XtVaGetValues(scale, XmNdecimalPoints, &decimalPoints,
		  XmNminimum, &min, XmNmaximum, &max,
		  XmNuserData, &func, NULL);

    (void) slider_text_string( string, cbs->value, decimalPoints,
				   min, max, func);
    position = (int) strlen( string ) + 1;
    XtVaSetValues(text, XmNvalue, string, XmNcursorPosition, position, NULL);
}
/************************************************************************
* Function:	log_func(), exp_func()					*
* Purpose:	example functions for the non-linear slider.		*
*		In this case the functions are simple log or exp	*
*		such that the functions returns a value in the range	*
*		0-1 given an argument in the range 0-1.			*
* Returns:	float value						*
************************************************************************/
static float log_func(float	x,
		      int	inverse)
{
    float value;

    if( inverse )
	value = exp((double) x * log((double) 2.0)) - 1.0;
    else
	value = log(1.0 + (double) x) / log((double) 2.0);

    return(value);
}

static float exp_func(float	x,
		      int	inverse)
{
    float value;

    if( inverse )
	value = log(1.0 + (double) x * (exp((double) 1.0) - 1.0));
    else
	value = (exp((double) x) - 1.0) / (exp((double) 1.0) - 1.0);

    return(value);
}
/************************************************************************
* Function:	Widget HGU_XmCreateHorizontalSlider()			*
* Purpose:	Procedure to put together a Form widget with a Label,	*
*		Scale and Text children set up to act as a slider	*
*		graphical element. It is NOT a widget proper and setting*
*		specific resource to change the default appearence	*
*		will require knowing the widget names (see above).	*
* Returns:	The pointer to the parent (Form) widget			*
* Arguments:								*
*	name		the top-level and label widget name		*
*	parent		the parent widget				*
*	fvalue		the initial value				*
*	fminval		the minimum allowed value			*
*	fmaxval		the maximum allowed value			*
*	decimalPoints	the number of decimal points to be displayed	*
*			this also determines the precision to which the	*
*			value can be set.				*
*	cb_func		a callback function to be put on the valueChanged
*			callback list.					*
*	cb_data		the client data for the callback		*
************************************************************************/
Widget HGU_XmCreateHorizontalSlider(String		name,
				    Widget		parent,
				    float		fvalue, 
				    float        	fminval, 
				    float        	fmaxval,
				    int			decimalPoints,
				    XtCallbackProc	cb_func,
				    XtPointer		cb_data)
{
    return( HGU_XmCreateHorizontalFuncSlider(name, parent, fvalue, fminval,
					     fmaxval, decimalPoints, cb_func,
					     cb_data, NULL) );
}
/************************************************************************
* Function:	Widget HGU_XmCreateHorizontalLogSlider()		*
* Purpose:	Same as HGU_XmCreateHorizontalSlider() but the scale	*
*		is logarithmic.						*
************************************************************************/
Widget HGU_XmCreateHorizontalLogSlider(String		name,
				       Widget		parent,
				       float		fvalue, 
				       float    	fminval,
				       float    	fmaxval,
				       int		decimalPoints,
				       XtCallbackProc	cb_func,
				       XtPointer	cb_data)
{
    return( HGU_XmCreateHorizontalFuncSlider(name, parent, fvalue, fminval,
					     fmaxval, decimalPoints, cb_func,
					     cb_data, log_func) );
}
/************************************************************************
* Function:	Widget HGU_XmCreateHorizontalExpSlider()		*
* Purpose:	Same as HGU_XmCreateHorizontalSlider() but the scale	*
*		is exponential.						*
************************************************************************/
Widget HGU_XmCreateHorizontalExpSlider(String		name,
				       Widget		parent,
				       float		fvalue, 
				       float 		fminval, 
				       float 		fmaxval,
				       int		decimalPoints,
				       XtCallbackProc	cb_func,
				       XtPointer	cb_data)
{
    return( HGU_XmCreateHorizontalFuncSlider(name, parent, fvalue, fminval,
					     fmaxval, decimalPoints, cb_func,
					     cb_data, exp_func) );
}
/************************************************************************
* Function:	Widget HGU_XmCreateHorizontalFuncSlider()		*
* Purpose:	Same as HGU_XmCreateHorizontalSlider() except that the	*
*		scale is determined by the input function. The only	*
*		constraint on the function is that it must accept	*
*		an argument in the range 0-1 and return a float value	*
*		in the range 0-1. A second argument is a flag to request*
*		the direct function or its inverse. The direct function *
*		maps the scale proportional value ie:			*
*		(scale_val-minval) / (maxval-minval) on to the		*
*		required proportional times 10**decimalPoints ie.	*
*		(true_val*10**decimalPoints - minval)/(maxval-minval).	*
************************************************************************/
static void UpdateFromText(
  Widget	w,
  XEvent	*event,
  String	*params,
  Cardinal	*num_params)
{
  XtCallCallbacks(w, XmNactivateCallback, NULL);
  return;
}
static XtActionsRec actions[] = {
  {"HGU_SliderUpdateFromText",	UpdateFromText},
  {NULL,		NULL},
};

static String translations_table =
"<LeaveWindow>: HGU_SliderUpdateFromText()";

Widget HGU_XmCreateHorizontalFuncSlider(String		name,
					Widget		parent,
					float		fvalue, 
					float 		fminval, 
					float 		fmaxval,
					int		decimalPoints,
					XtCallbackProc	cb_func,
					XtPointer	cb_data,
					HGU_XmSliderFunc func)
{
    Widget	base, label, scale, text;
    char	str[40];
    int		len=decimalPoints;
    int		value, minval, maxval;
    XtTranslations	translations;

    while( len-- > 0 ){
	fvalue *= 10.0;
	fminval *= 10.0;
	fmaxval *= 10.0;
    }
    minval = (int) fminval;
    maxval = (int) fmaxval;
    if( func != NULL )
	fvalue = minval + (maxval - minval) *
	    (*func)((float) (fvalue - minval) / (maxval - minval), 1);
    value  = nint((double) fvalue);

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

    len = (abs(minval) > abs(maxval)) ? abs(minval) : abs(maxval);
    (void) sprintf(str, "%d", len);
    len = (int) strlen(str) + ((minval<0)?1:0) + (decimalPoints?1:0);
    (void) slider_text_string( str, value, decimalPoints,
				   minval, maxval, func );

    text = XtVaCreateManagedWidget("text", xmTextWidgetClass, base,
				   XmNtopAttachment,	XmATTACH_FORM,
				   XmNbottomAttachment, XmATTACH_FORM,
				   XmNrightAttachment,	XmATTACH_FORM,
				   XmNborderWidth,	0,
				   XmNshadowThickness,	0,
				   XmNvalue,		str,
				   XmNcursorPosition,	len,
				   XmNcolumns,		len,
				   NULL);

    /* add an action to update when the cursor leaves the text window */
    XtAppAddActions(XtWidgetToApplicationContext(base), actions, XtNumber(actions));
    translations = XtParseTranslationTable( translations_table );
    XtOverrideTranslations( text, translations );

    /* note the function pointer func is not cast to XtPointer
       because this throws up a compiler warning on the grounds
       that a pointer-to-function may not be equivalent to a
       pointer-to-object. This could be a portability problem */
    scale = XtVaCreateManagedWidget("scale", xmScaleWidgetClass, base,
				    XmNtopAttachment,	XmATTACH_FORM,
				    XmNleftAttachment,	XmATTACH_WIDGET,
				    XmNrightAttachment,	XmATTACH_WIDGET,
				    XmNleftWidget,	label,
				    XmNrightWidget,	text,
				    XmNvalue,		value,
				    XmNmaximum,		maxval,
				    XmNminimum,		minval,
				    XmNdecimalPoints,	decimalPoints,
				    XmNorientation,	XmHORIZONTAL,
				    XmNborderWidth,	0,
				    XmNtopOffset,	8,
				    XmNuserData,	func,
				    NULL);

    XtAddCallback(scale, XmNdragCallback, 
		  (XtCallbackProc) update_slider_text, 
		  text);
    XtAddCallback(scale, XmNvalueChangedCallback, 
		  (XtCallbackProc) update_slider_text, 
		  text);
    XtAddCallback(text, XmNactivateCallback, 
		  (XtCallbackProc) update_slider_value, 
		  scale);

    if( cb_func != NULL )
	XtAddCallback(scale, XmNvalueChangedCallback, cb_func, cb_data);

    XtManageChild( base );
    return( base );
}

void HGU_XmSetSliderValue(Widget	slider,
			  float		value)
{
    Widget			scale, text;
    XmScaleCallbackStruct	cbs;
    short			points;
    int				min, max;
    HGU_XmSliderFunc		func;

    if( (scale = XtNameToWidget(slider, ".scale")) == NULL )
	return;

    if( (text = XtNameToWidget(slider, ".text")) == NULL )
	return;

    XtVaGetValues(scale, XmNdecimalPoints, &points,
		  XmNminimum, &min, XmNmaximum, &max,
		  XmNuserData, &func, NULL);

    while( points-- > 0 )
	value *= 10.0;
    if( func != NULL )
	value = min  + (max - min) *
	    (*func)((float) (value - min) / (max - min), 1);

    cbs.reason = XmCR_VALUE_CHANGED;
    cbs.event  = NULL;
    cbs.value  = nint((double) value);

    XtVaSetValues(scale, XmNvalue, cbs.value, NULL);
    update_slider_text(scale, text, &cbs);
    return;
}

float HGU_XmGetSliderValue(Widget	slider)
{
    Widget	scale;
    short	points;
    int		ivalue, min, max;
    float	value;
    HGU_XmSliderFunc	func;

    if( (scale = XtNameToWidget(slider, ".scale")) == NULL )
	return( (float) 0.0 );

    XtVaGetValues(scale, XmNvalue, &ivalue, XmNdecimalPoints, &points,
		  XmNminimum, &min, XmNmaximum, &max,
		  XmNuserData, &func, NULL);
    value = (float) ivalue;
    if( func != NULL )
	value = min + (max - min) *
	    (*func)((float) (value - min) / (max - min), 0);
    while( points-- > 0 )
	value /= 10.0;

    return( value );
}

void HGU_XmSetSliderRange(Widget	slider,
			  float		minval, 
			  float 	maxval)
{
    Widget	scale;
    short	points;
    float	value;
    int		minv, maxv;

    if( maxval <= minval )
	return;

    if( (scale = XtNameToWidget(slider, ".scale")) == NULL )
	return;

    XtVaGetValues(scale, XmNdecimalPoints, &points, NULL);
    value = HGU_XmGetSliderValue( slider );

    if( value > maxval ) value = maxval;
    if( value < minval ) value = minval;

    while( points-- > 0 ){
	minval *= 10.0;
	maxval *= 10.0;
    }
    minv = (int) minval;
    maxv = (int) maxval;

    XtVaSetValues(scale, XmNminimum, minv, XmNmaximum, maxv, NULL);
    HGU_XmSetSliderValue( slider, value );
    return;
}

void HGU_XmGetSliderRange(
Widget	slider,
float	*minval,
float	*maxval)
{
    Widget	scale;
    short	points;
    int		minv, maxv;

    if( (scale = XtNameToWidget(slider, ".scale")) == NULL )
	return;

    XtVaGetValues(scale,
		  XmNdecimalPoints,	&points,
		  XmNminimum,		&minv,
		  XmNmaximum,		&maxv,
		  NULL);

    *minval = minv;
    *maxval = maxv;
    while( points-- > 0 ){
	*minval /= 10.0;
	*maxval /= 10.0;
    }

    return;
}

void GE_SliderSetProc(
Widget	slider,
String	value)
{
    Widget	scale, text;

    if( (scale = XtNameToWidget(slider, ".scale")) == NULL )
	return;

    if( (text = XtNameToWidget(slider, ".text")) == NULL )
	return;

    XtVaSetValues(text, XmNvalue, value, NULL);
    update_slider_value(text, scale, NULL);

    return;
}

String GE_SliderGetProc(Widget slider)
{
    String	value;
    Widget	text;

    if( (text = XtNameToWidget(slider, ".text")) == NULL )
	return( NULL );

    XtVaGetValues( text, XmNvalue, &value, NULL );
    return( value );
}



