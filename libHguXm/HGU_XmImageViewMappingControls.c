#pragma ident "MRC HGU $Id$"
/*!
* \file         HGU_XmImageViewMappingControls.c
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Mon Mar  4 13:53:33 2002
* \version      MRC HGU $Id$
*               $Revision$
*               $Name$
* \par Copyright:
*               1994-2001 Medical Research Council, UK.
*               All rights reserved.
* \par Address:
*               MRC Human Genetics Unit,
*               Western General Hospital,
*               Edinburgh, EH4 2XU, UK.
* \ingroup      libHguXm
* \brief        Build a set of grey-level mapping controls plus associated functions.
*               
* \todo         -
* \bug          None known
*
* Maintenance log with most recent changes at top of list.
*/

#include <stdio.h>

#include <Xm/XmAll.h>
#include <HGU_XmUtils.h>

void HGU_XmImageViewSetLutControls(
  HGU_XmImageViewDataStruct	*data)
{
  Widget	slider, option, toggle, widget;

  /* reset the graph scale */
  HGU_XmSetGraphLimits(data->graph, (float) data->srcMin,
		       (float) data->srcMax + 1,
		       (float) 0, (float) 255);

  /* now reset the sliders */
  if( slider = XtNameToWidget(data->greyMapping, "*src_grey_min") ){
    HGU_XmSetSliderRange(slider, (float) data->srcMin,
			 (float) data->srcMax);
    HGU_XmSetSliderValue(slider, (float) data->min);
  }
  if( slider = XtNameToWidget(data->greyMapping, "*src_grey_max") ){
    HGU_XmSetSliderRange(slider, (float) data->srcMin,
			 (float) data->srcMax);
    HGU_XmSetSliderValue(slider, (float) data->max);
  }
  if( slider = XtNameToWidget(data->greyMapping, "*dst_grey_min") ){
    HGU_XmSetSliderValue(slider, (float) data->Min);
  }
  if( slider = XtNameToWidget(data->greyMapping, "*dst_grey_max") ){
    HGU_XmSetSliderValue(slider, (float) data->Max);
  }
  if( slider = XtNameToWidget(data->greyMapping, "*gamma") ){
    HGU_XmSetSliderValue(slider, (float) data->gamma);
  }
  if( slider = XtNameToWidget(data->greyMapping, "*mean") ){
    HGU_XmSetSliderRange(slider, (float) data->srcMin,
			 (float) data->srcMax);
    HGU_XmSetSliderValue(slider, (float) data->mean);
  }
  if( slider = XtNameToWidget(data->greyMapping, "*sigma") ){
    HGU_XmSetSliderRange(slider, (float) 0,
			 (float) data->srcMax);
    HGU_XmSetSliderValue(slider, (float) data->sigma);
  }

  /* reset the transform type and invert toggle */
  if( option = XtNameToWidget(data->greyMapping, "*transformType") ){
    switch( data->transType ){
    default:
    case WLZ_GREYTRANSFORMTYPE_LINEAR:
      widget = XtNameToWidget(option, "*linear");
      break;

    case WLZ_GREYTRANSFORMTYPE_GAMMA:
      widget = XtNameToWidget(option, "*gamma");
      break;

    case WLZ_GREYTRANSFORMTYPE_SIGMOID:
      widget = XtNameToWidget(option, "*sigmoid");
      break;
    }
    if( widget ){
      XtVaSetValues(option, XmNmenuHistory, widget, NULL);
    }
  }
  if( toggle = XtNameToWidget(data->greyMapping, "*invert") ){
    XtVaSetValues(toggle, XmNset, data->invert, NULL);
  }
  

  /* re-display the transform graph */
  HGU_XmImageViewSetTransformDisplay(data);

  return;
}

void HGU_XmImageViewResetLutControlValues(
  HGU_XmImageViewDataStruct	*data)
{
  if( data == NULL ){
    return;
  }
  data->transType = WLZ_GREYTRANSFORMTYPE_LINEAR;
  data->min = data->srcMin;
  data->max = data->srcMax;
  data->Min = 0;
  data->Max = 255;
  data->gamma = 1.0;
  data->mean = (data->min + data->max)/2.0;
  data->sigma = data->mean;
  data->invert = 0;

  return;
}

static WlzIVertex2     	hist_vtx[1024];
static WlzPolygonDomain	*hist_polydmn=NULL;
void HGU_XmImageViewSetHistogramDisplayCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  HGU_XmImageViewDataStruct *data=(HGU_XmImageViewDataStruct *) client_data;
  int		i, min, max, nBins, nMax;
  double	binOrigin, binSize;
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  Widget	widget;

  if( (data->obj == NULL) || data->objHistogram ){
    return;
  }

  /* get max and min from data */
  min = data->srcMin;
  max = data->srcMax;
  nBins = WLZ_MIN((max-min+1), 1024);
  binOrigin = min;
  binSize = ((double)(max - min)) / nBins;

  /* calculate the histogram & reset the display */
  if( hist_polydmn == NULL ){
    hist_polydmn = WlzMakePolyDmn(WLZ_POLYGON_INT, hist_vtx, 1024, 1024, 0, NULL);
  }
  if( data->objHistogram = WlzHistogramObj(data->obj, nBins,
					     binOrigin, binSize,
					     &errNum) ){
    data->objHistogram = WlzAssignObject(data->objHistogram, NULL);
    switch( data->objHistogram->domain.hist->type ){
    case WLZ_HISTOGRAMDOMAIN_INT:
      for(i=0, nMax=0; i < nBins; i++){
	hist_vtx[i].vtX = binOrigin + i * binSize;
	hist_vtx[i].vtY = data->objHistogram->domain.hist->binValues.inp[i];
	nMax = WLZ_MAX(nMax, hist_vtx[i].vtY);
      }
      break;

    case WLZ_HISTOGRAMDOMAIN_FLOAT:
      for(i=0, nMax=0; i < nBins; i++){
	hist_vtx[i].vtX = binOrigin + i * binSize;
	hist_vtx[i].vtY = data->objHistogram->domain.hist->binValues.flp[i];
	nMax = WLZ_MAX(nMax, hist_vtx[i].vtY);
      }
      break;
    }
  }
  HGU_XmSetGraphLimits(data->histogram, (float) min, (float) max,
		       (float) 0, (float) nMax);
  HGU_XmSetGraphPolyline(data->histogram, hist_polydmn, 1);

  return;
}


static WlzIVertex2	win_vtx[5], lut_vtx[256];
static WlzPolygonDomain	*win_polydmn=NULL, *lut_polydmn=NULL;
void HGU_XmImageViewSetTransformDisplay(
  HGU_XmImageViewDataStruct *data)
{
  int		i;
  double	g, A, B, fmin, fmax, mu, sig;

  /* display the transform window */
  win_vtx[0].vtX = win_vtx[3].vtX = win_vtx[4].vtX = data->min;
  win_vtx[0].vtY = win_vtx[1].vtY = win_vtx[4].vtY = data->Min;
  win_vtx[1].vtX = win_vtx[2].vtX = data->max;
  win_vtx[2].vtY = win_vtx[3].vtY = data->Max;
  if( win_polydmn == NULL ){
    win_polydmn = WlzMakePolyDmn(WLZ_POLYGON_INT, win_vtx, 5, 5, 0, NULL);
  }
  HGU_XmSetGraphPolyline(data->graph, win_polydmn, 1);

  /* display the transform */
  if( lut_polydmn == NULL ){
    lut_polydmn = WlzMakePolyDmn(WLZ_POLYGON_INT, lut_vtx, 256, 256, 0, NULL);
  }

  lut_vtx[0].vtX = data->srcMin;
  lut_vtx[0].vtY = data->Min;
  lut_vtx[1].vtX = data->min;
  lut_vtx[1].vtY = data->Min;
  for(i=2; i < 200; i++){
    g = data->min + (i-2) * (data->max - data->min + 1) / 198;
    lut_vtx[i].vtX = (int) g;
    if( lut_vtx[i].vtX < data->srcMin ){
      lut_vtx[0].vtY = data->Min;
    }
    else if( lut_vtx[i].vtX > data->srcMax ){
      lut_vtx[i].vtY = data->Max;
    }
    else {
      lut_vtx[i].vtY = data->lut[lut_vtx[i].vtX - data->srcMin];
    }
  }
  lut_vtx[i].vtX = data->max;
  lut_vtx[i++].vtY = data->Max;
  lut_vtx[i].vtX = data->max;
  lut_vtx[i++].vtY = data->Max;
  lut_polydmn->nvertices = i;
  
  HGU_XmSetGraphPolyline(data->graph, lut_polydmn, 2);

  return;
}

void HGU_XmImageViewSetTransformDisplayCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  HGU_XmImageViewDataStruct *data=(HGU_XmImageViewDataStruct *) client_data;

  HGU_XmImageViewSetLutTransform(data);
  HGU_XmImageViewSetTransformDisplay(data);

  /* reset the transform lut and re-display image */
  if( data->obj ){
    if( data->ximage ){
      AlcFree(data->ximage->data);
      data->ximage->data = NULL;
      XDestroyImage(data->ximage);
      data->ximage = NULL;    
    }

    /* calculate the expose event and call expose callback */
    XtCallCallbacks(data->canvas, XmNexposeCallback, NULL);
  }

  return;
}

void HGU_XmImageViewHistMagPlusCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  HGU_XmImageViewDataStruct *data=(HGU_XmImageViewDataStruct *) client_data;
  float		xl, xu, yl, yu, X, Y;

  if( data->objHistogram == NULL ){
    return;
  }

  /* get the current graph scale limits and cursor position */
  HGU_XmGetGraphLimits(data->histogram, &xl, &xu, &yl, &yu);
  HGU_XmGetHairCursor(data->histogram, &X, &Y);

  /* reset scale */
  xl = (xl + X) / 2.0;
  xu = (xu + X) / 2.0;
  yl = (yl + Y) / 2.0;
  yu = (yu + Y) / 2.0;
  HGU_XmSetGraphLimits(data->histogram, xl, xu, yl, yu);

  return;
}

void HGU_XmImageViewHistMagMinusCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  HGU_XmImageViewDataStruct *data=(HGU_XmImageViewDataStruct *) client_data;
  float		xl, xu, yl, yu, X, Y;

  if( data->objHistogram == NULL ){
    return;
  }

  /* get the current graph scale limits and cursor position */
  HGU_XmGetGraphLimits(data->histogram, &xl, &xu, &yl, &yu);
  HGU_XmGetHairCursor(data->histogram, &X, &Y);

  /* reset scale */
  xl = 2.0 * xl - X;
  xu = 2.0 * xu - X;
  yl = 2.0 * yl - Y;
  yu = 2.0 * yu - Y;
  HGU_XmSetGraphLimits(data->histogram, xl, xu, yl, yu);

  return;
}

void HGU_XmImageViewHistMagResetCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  HGU_XmImageViewDataStruct *data=(HGU_XmImageViewDataStruct *) client_data;
  float		xl, xu, yl, yu;
  int		i;
  WlzHistogramDomain	*histDmn;

  if( data->objHistogram == NULL ){
    return;
  }

  /* reset scale */
  xl = data->srcMin;
  xu = data->srcMax;
  yl = 0.0;
  yu = 0.0;
  histDmn = data->objHistogram->domain.hist;
  switch( histDmn->type ){
  case WLZ_HISTOGRAMDOMAIN_INT:
    for(i=0; i < histDmn->nBins; i++){
      if( histDmn->binValues.inp[i] > yu ){
	yu = histDmn->binValues.inp[i];
      }
    }
    break;

  case WLZ_HISTOGRAMDOMAIN_FLOAT:
    for(i=0; i < histDmn->nBins; i++){
      if( histDmn->binValues.flp[i] > yu ){
	yu = histDmn->binValues.inp[i];
      }
    }
    break;
  }
  HGU_XmSetGraphLimits(data->histogram, xl, xu, yl, yu);

  return;
}

void HGU_XmImageViewInvertTransCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  HGU_XmImageViewDataStruct *data=(HGU_XmImageViewDataStruct *) client_data;
  XmToggleButtonCallbackStruct
    *cbs = (XmToggleButtonCallbackStruct *) call_data;

  data->invert = cbs->set;
  HGU_XmImageViewSetTransformDisplayCb(w, client_data, call_data);
  return;
}

void HGU_XmImageViewTransformTypeLinearCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  HGU_XmImageViewDataStruct *data=(HGU_XmImageViewDataStruct *) client_data;

  data->transType = WLZ_GREYTRANSFORMTYPE_LINEAR;
  HGU_XmImageViewSetTransformDisplayCb(w, client_data, call_data);
  return;
}

void HGU_XmImageViewTransformTypeGammaCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  HGU_XmImageViewDataStruct *data=(HGU_XmImageViewDataStruct *) client_data;

  data->transType = WLZ_GREYTRANSFORMTYPE_GAMMA;
  HGU_XmImageViewSetTransformDisplayCb(w, client_data, call_data);
  return;
}

void HGU_XmImageViewTransformTypeSigmoidCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  HGU_XmImageViewDataStruct *data=(HGU_XmImageViewDataStruct *) client_data;

  data->transType = WLZ_GREYTRANSFORMTYPE_SIGMOID;
  HGU_XmImageViewSetTransformDisplayCb(w, client_data, call_data);
  return;
}

void HGU_XmImageViewGammaSetCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  HGU_XmImageViewDataStruct *data=(HGU_XmImageViewDataStruct *) client_data;
  double	newval;
  Widget	slider = w;

  
  while( strcmp(XtName(slider), "gamma") ){
    if( (slider = XtParent(slider)) == NULL ){
      return;
    }
  }

  newval = (double) HGU_XmGetSliderValue(slider);
  data->gamma = newval;

  HGU_XmImageViewSetTransformDisplayCb(w, client_data, call_data);

  return;
}

void HGU_XmImageViewMeanSetCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  HGU_XmImageViewDataStruct *data=(HGU_XmImageViewDataStruct *) client_data;
  double	newval;
  Widget	slider = w;

  
  while( strcmp(XtName(slider), "mean") ){
    if( (slider = XtParent(slider)) == NULL ){
      return;
    }
  }

  newval = (double) HGU_XmGetSliderValue(slider);
  data->mean = newval;

  HGU_XmImageViewSetTransformDisplayCb(w, client_data, call_data);

  return;
}

void HGU_XmImageViewSigmaSetCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  HGU_XmImageViewDataStruct *data=(HGU_XmImageViewDataStruct *) client_data;
  double	newval;
  Widget	slider = w;

  
  while( strcmp(XtName(slider), "sigma") ){
    if( (slider = XtParent(slider)) == NULL ){
      return;
    }
  }

  newval = (double) HGU_XmGetSliderValue(slider);
  data->sigma = newval;

  HGU_XmImageViewSetTransformDisplayCb(w, client_data, call_data);

  return;
}

void HGU_XmImageViewSrcSliderMinCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  HGU_XmImageViewDataStruct *data=(HGU_XmImageViewDataStruct *) client_data;
  int		newval;
  Widget	slider = w;

  
  while( strcmp(XtName(slider), "src_grey_min") ){
    if( (slider = XtParent(slider)) == NULL ){
      return;
    }
  }

  newval = (int) HGU_XmGetSliderValue(slider);
  if( newval > data->max ){
    newval = data->max;
    HGU_XmSetSliderValue(slider, (float) newval);
  }
  data->min = newval;

  HGU_XmImageViewSetTransformDisplayCb(w, client_data, call_data);

  return;
}

void HGU_XmImageViewSrcSliderMaxCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  HGU_XmImageViewDataStruct *data=(HGU_XmImageViewDataStruct *) client_data;
  int		newval;
  Widget	slider = w;

  
  while( strcmp(XtName(slider), "src_grey_max") ){
    if( (slider = XtParent(slider)) == NULL ){
      return;
    }
  }

  newval = (int) HGU_XmGetSliderValue(slider);
  if( newval < data->min ){
    newval = data->min;
    HGU_XmSetSliderValue(slider, (float) newval);
  }
  data->max = newval;

  HGU_XmImageViewSetTransformDisplayCb(w, client_data, call_data);

  return;
}

void HGU_XmImageViewDstSliderMinCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  HGU_XmImageViewDataStruct *data=(HGU_XmImageViewDataStruct *) client_data;
  int		newval;
  Widget	slider = w;

  
  while( strcmp(XtName(slider), "dst_grey_min") ){
    if( (slider = XtParent(slider)) == NULL ){
      return;
    }
  }

  newval = (int) HGU_XmGetSliderValue(slider);
  if( newval > data->Max ){
    newval = data->Max;
    HGU_XmSetSliderValue(slider, (float) newval);
  }
  data->Min = newval;

  HGU_XmImageViewSetTransformDisplayCb(w, client_data, call_data);

  return;
}

void HGU_XmImageViewDstSliderMaxCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  HGU_XmImageViewDataStruct *data=(HGU_XmImageViewDataStruct *) client_data;
  int		newval;
  Widget	slider = w;

  
  while( strcmp(XtName(slider), "dst_grey_max") ){
    if( (slider = XtParent(slider)) == NULL ){
      return;
    }
  }

  newval = (int) HGU_XmGetSliderValue(slider);
  if( newval < data->Min ){
    newval = data->Min;
    HGU_XmSetSliderValue(slider, (float) newval);
  }
  data->Max = newval;

  HGU_XmImageViewSetTransformDisplayCb(w, client_data, call_data);

  return;
}

void HGU_XmImageViewResetImageControlsCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  HGU_XmImageViewDataStruct *data=(HGU_XmImageViewDataStruct *) client_data;
  Widget	slider;

  /* reset all the values
     leave type selection and invert alone
  */
  HGU_XmImageViewResetLutControlValues(data);
  HGU_XmImageViewSetLutTransform(data);
  HGU_XmImageViewSetLutControls(data);

  /* reset the transform lut and re-display image */
  if( data->ximage ){
    if( data->ximage->data ){
      AlcFree(data->ximage->data);
      data->ximage->data = NULL;
    }
    XDestroyImage(data->ximage);
    data->ximage = NULL;
  }

  /* calculate the expose event and call expose callback */
  XtCallCallbacks(data->canvas, XmNexposeCallback, NULL);

  return;
}

void HGU_XmImageViewRemapImageCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  HGU_XmImageViewDataStruct *data=(HGU_XmImageViewDataStruct *) client_data;
  WlzPixelV	min, max;
  WlzPixelP	lut;
  WlzObject	*obj;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  if( data->obj == NULL ){
    return;
  }

  /* change the grey-values "in-place" */
  min.type = WLZ_GREY_INT;
  min.v.inv = data->min;
  max.type = WLZ_GREY_INT;
  max.v.inv = data->max;
  lut.type = WLZ_GREY_UBYTE;
  lut.p.ubp = data->lut;
  errNum = WlzGreySetRangeLut(data->obj, min, max, lut);

  /* convert to a UBYTE image and install */
  obj = WlzAssignObject(WlzConvertPix(data->obj, WLZ_GREY_UBYTE, &errNum),
			NULL);
  HGU_XmImageViewInstallImage(obj, data);
  WlzFreeObj(obj);

  return;
}

static ActionAreaItem   image_controls_dialog_actions[] = {
  {"Apply",	NULL,		NULL},
  {"Reset",	NULL,		NULL},
  {"Dismiss",     NULL,           NULL},
  {"Help",        NULL,           NULL},
};

static MenuItem transformTypeItems[] = {
  {"linear", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   HGU_XmImageViewTransformTypeLinearCb, NULL,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"gamma", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   HGU_XmImageViewTransformTypeGammaCb, NULL,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"sigmoid", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   HGU_XmImageViewTransformTypeSigmoidCb, NULL,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};

typedef struct _HGU_XmGreyMappingDialogResourcesStruct {
  XmString	histogram_frame_title;
  XmString	hist_magp;
  XmString	hist_magn;
  XmString	hist_reset;
  Dimension	hist_graph_height;
  XmString	transform_frame_title;
  Dimension	trans_graph_height;
  XmString	grey_bounds_frame_title;
  XmString	src_grey_min;
  XmString	src_grey_max;
  XmString	dst_grey_min;
  XmString	dst_grey_max;
  XmString	transform_params_frame_title;
  XmString	transform_type_choice;
  XmString	gamma;
  XmString	mean;
  XmString	sigma;
}HGU_XmGreyMappingDialogResourcesStruct;

static HGU_XmGreyMappingDialogResourcesStruct myResources;

#define resOffset(field)   XtOffsetOf(HGU_XmGreyMappingDialogResourcesStruct, field)

static XtResource HGU_XmGreyMappingDialogResources[] =
{
  {"*histogram_frame_title.labelString", XmCXmString, XmRXmString, sizeof(XmString),
   resOffset(histogram_frame_title), XtRString, "Object Histogram"},
  {"*hist_magp.labelString", XmCXmString, XmRXmString, sizeof(XmString),
   resOffset(hist_magp), XtRString, "+"},
  {"*hist_magn.labelString", XmCXmString, XmRXmString, sizeof(XmString),
   resOffset(hist_magn), XtRString, "-"},
  {"*hist_reset.labelString", XmCXmString, XmRXmString, sizeof(XmString),
   resOffset(hist_reset), XtRString, "><"},
  {"*histogram_graph.height", XtCHeight, XtRDimension, sizeof(Dimension),
   resOffset(hist_graph_height), XtRString, "80"},
  {"*transform_frame_title.labelString", XmCXmString, XmRXmString, sizeof(XmString),
   resOffset(transform_frame_title), XtRString, "Grey level transform"},
  {"*transform_graph.height", XtCHeight, XtRDimension, sizeof(Dimension),
   resOffset(trans_graph_height), XtRString, "130"},
  {"*grey_bounds_frame_title.labelString", XmCXmString, XmRXmString, sizeof(XmString),
   resOffset(grey_bounds_frame_title), XtRString, "Grey level bounds"},
  {"*src_grey_min.labelString", XmCXmString, XmRXmString, sizeof(XmString),
   resOffset(src_grey_min), XtRString, "Source min"},
  {"*src_grey_max.labelString", XmCXmString, XmRXmString, sizeof(XmString),
   resOffset(src_grey_max), XtRString, "Source max"},
  {"*dst_grey_min.labelString", XmCXmString, XmRXmString, sizeof(XmString),
   resOffset(dst_grey_min), XtRString, "Destination min"},
  {"*dst_grey_max.labelString", XmCXmString, XmRXmString, sizeof(XmString),
   resOffset(dst_grey_max), XtRString, "Destination max"},
  {"*transform_params_frame_title.labelString", XmCXmString, XmRXmString, sizeof(XmString),
   resOffset(transform_params_frame_title), XtRString, "Transform parameters"},
  {"*transformType.labelString", XmCXmString, XmRXmString, sizeof(XmString),
   resOffset(transform_type_choice), XtRString, "Transform type:"},
  {"*gamma.labelString", XmCXmString, XmRXmString, sizeof(XmString),
   resOffset(gamma), XtRString, "Gamma"},
  {"*mean.labelString", XmCXmString, XmRXmString, sizeof(XmString),
   resOffset(mean), XtRString, "Mean"},
  {"*sigma.labelString", XmCXmString, XmRXmString, sizeof(XmString),
   resOffset(sigma), XtRString, "Sigma"},
};

void HGU_XmCreateGreyMappingControls(
  Widget			control,
  HGU_XmImageViewDataStruct	*data)
{
  Widget	form, frame, title, title_form;
  Widget	rowcolumn, button, option, toggle, widget, rc;
  Widget	graph, graphics, slider, scale, label;
  Widget	histogram_frame, transform_frame, grey_bounds_frame;
  Widget	transform_params_frame;
  int		i;

  /* get resources before widget creation */
  XtGetApplicationResources(control, &myResources, HGU_XmGreyMappingDialogResources,
			    XtNumber(HGU_XmGreyMappingDialogResources), NULL, 0);

  /* frame for the histogram */
  frame = XtVaCreateManagedWidget("histogram_frame", xmFrameWidgetClass,
				  control,
				  XmNtopAttachment, XmATTACH_FORM,
				  XmNleftAttachment, XmATTACH_FORM,
				  XmNrightAttachment, XmATTACH_FORM,
				  NULL);
  histogram_frame = frame;
  
  rc = XtVaCreateManagedWidget("histogram_frame_rc", xmRowColumnWidgetClass,
			       frame,
			       XmNchildType, XmFRAME_TITLE_CHILD,
			       XmNorientation, XmHORIZONTAL,
			       XmNpacking, XmPACK_TIGHT,
			       NULL);
  title = XtVaCreateManagedWidget("histogram_frame_title", xmLabelWidgetClass,
				  rc,
				  XmNlabelString, myResources.histogram_frame_title,
				  NULL);
  button = XtVaCreateManagedWidget("hist_magp", xmPushButtonWidgetClass,
				   rc,
				   XmNlabelString, myResources.hist_magp,
				   NULL);
  XtAddCallback(button, XmNactivateCallback,
		HGU_XmImageViewHistMagPlusCb, data);
  button = XtVaCreateManagedWidget("hist_magn", xmPushButtonWidgetClass,
				   rc,
				   XmNlabelString, myResources.hist_magn,
				   NULL);
  XtAddCallback(button, XmNactivateCallback,
		HGU_XmImageViewHistMagMinusCb, data);
  button = XtVaCreateManagedWidget("hist_reset", xmPushButtonWidgetClass,
				   rc,
				   XmNlabelString, myResources.hist_reset,
				   NULL);
  XtAddCallback(button, XmNactivateCallback,
		HGU_XmImageViewHistMagResetCb, data);

  graph = HGU_XmCreateGraphD("histogram_graph", frame, NULL, 0);
  HGU_XmSetGraphLimits(graph, (float) data->min, (float) data->max,
		       (float) data->Min, (float) data->Max);
  data->histogram = graph;

  graphics = XtNameToWidget(graph, "*.graphics");
  XtAddCallback(graphics, XmNexposeCallback,
		HGU_XmImageViewSetHistogramDisplayCb, data);
  XtVaSetValues(graphics, XtNheight, myResources.hist_graph_height, NULL);

  /* frame for the transform parameters */
  frame = XtVaCreateManagedWidget("transform_params_frame", xmFrameWidgetClass,
				  control,
				  XmNbottomAttachment, XmATTACH_FORM,
				  XmNleftAttachment, XmATTACH_FORM,
				  XmNrightAttachment, XmATTACH_FORM,
				  NULL);
  transform_params_frame = frame;
  
  title = XtVaCreateManagedWidget("transform_params_frame_title",
				  xmLabelWidgetClass, frame,
				  XmNchildType, XmFRAME_TITLE_CHILD,
				  XmNlabelString,
				  myResources.transform_frame_title,
				  NULL);
  form = XtVaCreateManagedWidget("transform_params_form", xmFormWidgetClass,
				 frame, NULL);

  i=0;
  while(transformTypeItems[i].name != NULL){
    transformTypeItems[i].callback_data = (XtPointer) data;
    i++;
  }
  option = HGU_XmBuildMenu(form, XmMENU_OPTION, "transformType", 0,
			   XmTEAR_OFF_DISABLED, transformTypeItems);
  XtManageChild(option);
  XtVaSetValues(option,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNlabelString, myResources.transform_type_choice,
		NULL);

  button = XtVaCreateManagedWidget("invert", xmToggleButtonWidgetClass,
				   form,
				   XmNtopAttachment,	XmATTACH_FORM,
				   XmNleftAttachment,	XmATTACH_WIDGET,
				   XmNleftWidget,	option,
				   XmNbottomAttachment,	XmATTACH_OPPOSITE_WIDGET,
				   XmNbottomWidget,	option,
				   NULL);
  XtAddCallback(button, XmNvalueChangedCallback,
		HGU_XmImageViewInvertTransCb, data);

  slider = HGU_XmCreateHorizontalSlider("gamma", form,
					(float) data->gamma, (float) 0.2,
					(float) 5.0, 2,
					HGU_XmImageViewGammaSetCb, data);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		option,
		XmNleftAttachment,	XmATTACH_POSITION,
		XmNleftPosition,	1,
		XmNrightAttachment,	XmATTACH_POSITION,
		XmNrightPosition,	49,
		NULL);
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback, HGU_XmImageViewGammaSetCb, data);
  label = XtNameToWidget(slider, ".gamma");
  XtVaSetValues(label, XmNlabelString, myResources.gamma, NULL);
  widget = slider;

  slider = HGU_XmCreateHorizontalSlider("mean", form,
					(float) data->mean,
					(float) data->srcMin,
					(float) data->srcMax,
					0,
					HGU_XmImageViewMeanSetCb, data);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_POSITION,
		XmNleftPosition,	1,
		XmNrightAttachment,	XmATTACH_POSITION,
		XmNrightPosition,	49,
		NULL);
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback, HGU_XmImageViewMeanSetCb, data);
  label = XtNameToWidget(slider, ".mean");
  XtVaSetValues(label, XmNlabelString, myResources.mean, NULL);

  slider = HGU_XmCreateHorizontalSlider("sigma", form,
					(float) data->sigma, (float) 0.0,
					(float) data->srcMax,
					0,
					HGU_XmImageViewSigmaSetCb, data);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_POSITION,
		XmNleftPosition,	51,
		XmNrightAttachment,	XmATTACH_POSITION,
		XmNrightPosition,	99,
		NULL);
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback, HGU_XmImageViewSigmaSetCb, data);
  label = XtNameToWidget(slider, ".sigma");
  XtVaSetValues(label, XmNlabelString, myResources.sigma, NULL);

  /* frame for the transform grey-level bounds */
  frame = XtVaCreateManagedWidget("grey_bounds_frame", xmFrameWidgetClass,
				  control,
				  XmNbottomAttachment, XmATTACH_WIDGET,
				  XmNbottomWidget, transform_params_frame,
				  XmNleftAttachment, XmATTACH_FORM,
				  XmNrightAttachment, XmATTACH_FORM,
				  NULL);
  grey_bounds_frame = frame;
  
  title = XtVaCreateManagedWidget("grey_bounds_frame_title", xmLabelWidgetClass,
				  frame,
				  XmNchildType, XmFRAME_TITLE_CHILD,
				  XmNlabelString,
				  myResources.grey_bounds_frame_title,
				  NULL);
  form = XtVaCreateManagedWidget("grey_bounds_form", xmFormWidgetClass,
				 frame, NULL);

  slider = HGU_XmCreateHorizontalSlider("src_grey_min", form,
					(float) data->min,
					(float) data->srcMin,
					(float) data->srcMax,
					0,
					HGU_XmImageViewSrcSliderMinCb,
					data);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_POSITION,
		XmNleftPosition,	1,
		XmNrightAttachment,	XmATTACH_POSITION,
		XmNrightPosition,	49,
		NULL);
  widget = slider;
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback,
		HGU_XmImageViewSrcSliderMinCb, data);
  label = XtNameToWidget(slider, ".src_grey_min");
  XtVaSetValues(label, XmNlabelString, myResources.src_grey_min, NULL);

  slider = HGU_XmCreateHorizontalSlider("src_grey_max", form,
					(float) data->max,
					(float) 0.0,
					(float) data->srcMax,
					0,
					HGU_XmImageViewSrcSliderMaxCb, data);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_POSITION,
		XmNleftPosition,	51,
		XmNrightAttachment,	XmATTACH_POSITION,
		XmNrightPosition,	99,
		NULL);
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback,
		HGU_XmImageViewSrcSliderMaxCb, data);
  label = XtNameToWidget(slider, ".src_grey_max");
  XtVaSetValues(label, XmNlabelString, myResources.src_grey_max, NULL);

  slider = HGU_XmCreateHorizontalSlider("dst_grey_min", form,
					(float) data->Min,
					(float) 0.0,
					(float) 255.0, 0,
					HGU_XmImageViewDstSliderMinCb, data);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_POSITION,
		XmNleftPosition,	1,
		XmNrightAttachment,	XmATTACH_POSITION,
		XmNrightPosition,	49,
		NULL);
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback,
		HGU_XmImageViewDstSliderMinCb, data);
    label = XtNameToWidget(slider, ".dst_grey_min");
  XtVaSetValues(label, XmNlabelString, myResources.dst_grey_min, NULL);


  slider = HGU_XmCreateHorizontalSlider("dst_grey_max", form,
					(float) data->Max,
					(float) 0.0,
					(float) 255.0, 0,
					HGU_XmImageViewDstSliderMaxCb, data);
  XtVaSetValues(slider,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		widget,
		XmNleftAttachment,	XmATTACH_POSITION,
		XmNleftPosition,	51,
		XmNrightAttachment,	XmATTACH_POSITION,
		XmNrightPosition,	99,
		NULL);
  scale = XtNameToWidget(slider, "*scale");
  XtAddCallback(scale, XmNdragCallback,
		HGU_XmImageViewDstSliderMaxCb, data);
  label = XtNameToWidget(slider, ".dst_grey_max");
  XtVaSetValues(label, XmNlabelString, myResources.dst_grey_max, NULL);

  /* frame for the transform view */
  frame = XtVaCreateManagedWidget("transform_frame", xmFrameWidgetClass,
				  control,
				  XmNtopAttachment,	XmATTACH_WIDGET,
				  XmNtopWidget,		histogram_frame,
				  XmNbottomAttachment,	XmATTACH_WIDGET,
				  XmNbottomWidget,	grey_bounds_frame,
				  XmNleftAttachment, XmATTACH_FORM,
				  XmNrightAttachment, XmATTACH_FORM,
				  NULL);
  transform_frame = frame;
  
  title = XtVaCreateManagedWidget("transform_frame_title",
				  xmLabelWidgetClass,
				  frame,
				  XmNchildType, XmFRAME_TITLE_CHILD,
				  XmNlabelString,
				  myResources.transform_frame_title,
				  NULL);
  graph = HGU_XmCreateGraphD("transform_graph", frame, NULL, (Drawable) 0);
  HGU_XmSetGraphLimits(graph, (float) data->srcMin,
		       (float) data->srcMax + 1,
		       (float) 0, (float) 255);
  XtVaSetValues(graph, XtNheight, myResources.trans_graph_height, NULL);
  data->graph = graph;
  graphics = XtNameToWidget(graph, "*.graphics");
  XtAddCallback(graphics, XmNexposeCallback,
		HGU_XmImageViewSetTransformDisplayCb, data);
  XtVaSetValues(graphics, XtNheight, myResources.trans_graph_height, NULL);

  return;
}

Widget HGU_XmCreateGreyMappingDialog(
  String			name,
  Widget			parent,
  HGU_XmImageViewDataStruct	*data)
{
  Widget	dialog, control, widget;


  dialog = HGU_XmCreateStdDialog(parent, name, xmFormWidgetClass,
				 image_controls_dialog_actions, 4);

  if( (widget = XtNameToWidget(dialog, "*.Apply")) != NULL ){
    XtAddCallback(widget, XmNactivateCallback,
		  HGU_XmImageViewRemapImageCb, data);
  }

  if( (widget = XtNameToWidget(dialog, "*.Reset")) != NULL ){
    XtAddCallback(widget, XmNactivateCallback,
		  HGU_XmImageViewResetImageControlsCb, data);
  }

  if( (widget = XtNameToWidget(dialog, "*.Dismiss")) != NULL ){
    XtAddCallback(widget, XmNactivateCallback, PopdownCallback,
		  XtParent(dialog));
  }

  control = XtNameToWidget( dialog, "*.control" );
  data->greyMapping = control;

  /* put in the controls */
  HGU_XmCreateGreyMappingControls(control, data);


  return dialog;
}

