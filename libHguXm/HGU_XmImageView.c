#pragma ident "MRC HGU $Id$"
/*!
* \file         HGU_XmImageView.c
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Mon Feb 25 14:25:22 2002
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
* \ingroup      HGU_Xm
* \brief        Procedures to create a widget for image viewing
 including grey-level re-mapping.
*               
* \todo         -
* \bug          None known
*
* Maintenance log with most recent changes at top of list.
*/

#include <stdio.h>

#include <Xm/XmAll.h>
#include <HGU_XmUtils.h>

/* menu item structure */
static MenuItem file_type_menu_itemsP[] = {   /* file_menu items */
  {"woolz", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, (XtPointer) WLZEFF_FORMAT_WLZ,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"pgm", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, (XtPointer) WLZEFF_FORMAT_PNM,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"bmp", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, (XtPointer) WLZEFF_FORMAT_BMP,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"tiff", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, (XtPointer) WLZEFF_FORMAT_TIFF,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"raw", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   NULL, (XtPointer) WLZEFF_FORMAT_RAW,
   HGU_XmHelpStandardCb, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  NULL,
};

int HGU_XmGetColorIndexFromMask24(
  unsigned long mask,
  int		order)
{
  int index;

  switch( mask ){
  case 0xff:
    index = 0;
    break;
  case 0xff00:
    index = 1;
    break;
  case 0xff0000:
    index = 2;
    break;
  case 0xff000000:
    index = 3;
    break;
  default:
    index = 0;
    break;
  }
  if( order == MSBFirst ){
    index = 3 - index;
  }

  return index;
}

XImage *HGU_XmObjToXImageLut2D(
  XWindowAttributes	*win_att,
  WlzObject	*obj,
  HGU_XmLut	lut,
  WlzErrorNum	*dstErr)
{
  XImage		*rtnImage=NULL;
  Dimension		width, height;
  UBYTE			*data, *dst_data;
  WlzGreyValueWSpace	*gVWSp = NULL;
  WlzErrorNum		errNum=WLZ_ERR_NONE;
  int			i, j;
  int			rIndx, gIndx, bIndx, aIndx;
  UINT			r, g, b, a;

  /* allocate space for the data */
  width = obj->domain.i->lastkl - obj->domain.i->kol1 + 1;
  height = obj->domain.i->lastln - obj->domain.i->line1 + 1;
  if( gVWSp = WlzGreyValueMakeWSp(obj, &errNum) ){
    if( data = (UBYTE *) AlcMalloc(((win_att->depth == 8)?1:4)
				   *width*height*sizeof(char)) ){
      dst_data = data;
      if( rtnImage = XCreateImage(DisplayOfScreen(win_att->screen),
				  win_att->visual, win_att->depth,
				  ZPixmap, 0, (char *) dst_data,
				  width, height, 8, 0) ){

	/* establish rgb index values if 24 bit */
	if( win_att->depth == 24 ){
	  rIndx = HGU_XmGetColorIndexFromMask24(win_att->visual->red_mask,
					       rtnImage->byte_order);
	  gIndx = HGU_XmGetColorIndexFromMask24(win_att->visual->green_mask,
					       rtnImage->byte_order);
	  bIndx = HGU_XmGetColorIndexFromMask24(win_att->visual->blue_mask,
					       rtnImage->byte_order);
	  aIndx = HGU_XmGetColorIndexFromMask24(~(win_att->visual->red_mask|
						 win_att->visual->green_mask|
						 win_att->visual->blue_mask),
					       rtnImage->byte_order);
	}

	/* fill in the values */
	a = 0xff;
	for(j=0; j < height; j++){
	  for(i=0; i < width; i++, data++){
	    WlzGreyValueGet(gVWSp, 0, j + obj->domain.i->line1,
			    i + obj->domain.i->kol1);
	    switch( gVWSp->gType ){
	    default:
	    case WLZ_GREY_INT:
	      r = *(gVWSp->gPtr[0].inp);
	      g = b = r;
	      break;
	    case WLZ_GREY_SHORT:
	      r = *(gVWSp->gPtr[0].shp);
	      g = b = r;
	      break;
	    case WLZ_GREY_UBYTE:
	      r = *(gVWSp->gPtr[0].ubp);
	      g = b = r;
	      break;
	    case WLZ_GREY_FLOAT:
	      r = *(gVWSp->gPtr[0].flp);
	      g = b = r;
	      break;
	    case WLZ_GREY_DOUBLE:
	      r = *(gVWSp->gPtr[0].dbp);
	      g = b = r;
	      break;
	    case WLZ_GREY_RGBA:
	      b = *(gVWSp->gPtr[0].rgbp);
	      r = WLZ_RGBA_RED_GET(b);
	      r = WLZ_RGBA_GREEN_GET(b);
	      r = WLZ_RGBA_BLUE_GET(b);
	      break;
	    }

	    switch( lut.core->type ){
	    case HGU_XmLUT_GREY:
	      r = lut.g->lut[r - lut.g->min];
	      g = lut.g->lut[g - lut.g->min];
	      b = lut.g->lut[b - lut.g->min];
	      break;

	    case HGU_XmLUT_RGB:
	      r = lut.rgb->lut[0][r - lut.rgb->min[0]];
	      g = lut.rgb->lut[1][g - lut.rgb->min[1]];
	      b = lut.rgb->lut[2][b - lut.rgb->min[2]];
	      break;

	    case HGU_XmLUT_COMPOUND:
	      break;
	    }
	    switch( win_att->depth ){
	    case 24:
	      data[rIndx] = r;
	      data[gIndx] = g;
	      data[bIndx] = b;
	      data[aIndx] = a;
	      data += 3;
	      break;

	    case 8:
	      data[0] = r;
	      break;
	    }
	  }
	}
      }
      else {
	errNum = WLZ_ERR_UNSPECIFIED;
	AlcFree((void *) dst_data);
      }
    }	  
    else {
      errNum = WLZ_ERR_MEM_ALLOC;
    }
    WlzGreyValueFreeWSp(gVWSp);
  }

  if( dstErr ){
    *dstErr = errNum;
  }
  return rtnImage;
}

/* misc routines */
/* assumption here that on mapping to an XImage then UBYTE values are
   sufficient. Should really define a LUT object which could be of many
   types including a function.

   As a minimum the types should include an array for any pixel type,
   the min and max of the table and an option for compound objects.

   The simplest is for each pixel value the lut will return a grey or
   rgb value. In addition need a compound version, i.e. a lut for each
   element of a compound object. Must be fast.
*/

XImage *HGU_XmObjToXImageLut(
  Widget	w,
  WlzObject	*obj,
  HGU_XmLut	lut,
  WlzErrorNum	*dstErr)
{
  XImage		*rtnImage=NULL;
  XWindowAttributes	win_att;
  WlzCompoundArray	*cobj;
  WlzErrorNum		errNum=WLZ_ERR_NONE;

  /* check inputs */
  if( w == NULL ){
    errNum = WLZ_ERR_PARAM_NULL;
  }
  else if( XGetWindowAttributes(XtDisplay(w), XtWindow(w), &win_att) == 0 ){
    errNum = WLZ_ERR_UNSPECIFIED;
  }
  else if( obj == NULL ){
    errNum = WLZ_ERR_OBJECT_NULL;
  }
  else if( lut.core == NULL ){
    errNum = WLZ_ERR_PARAM_DATA;
  }
  else switch( obj->type ){
  case WLZ_2D_DOMAINOBJ:
    if( obj->domain.core == NULL ){
      errNum = WLZ_ERR_DOMAIN_NULL;
    }
    else if( obj->values.core == NULL ){
      errNum = WLZ_ERR_VALUES_NULL;
    }
    else {
      rtnImage = HGU_XmObjToXImageLut2D(&win_att, obj, lut, &errNum);
    }
    break;

  case WLZ_EMPTY_OBJ:
    break;

  case WLZ_COMPOUND_ARR_1:
  case WLZ_COMPOUND_ARR_2:
    cobj = (WlzCompoundArray *) obj;
    if( cobj->n < 1 ){
      errNum = WLZ_ERR_OBJECT_DATA;
    }
    else {
      switch(cobj->o[0]->type){
      case WLZ_2D_DOMAINOBJ:
	break;

      default:
	errNum = WLZ_ERR_OBJECT_TYPE;
	break;
      }
    }
    break;

  default:
    errNum = WLZ_ERR_OBJECT_TYPE;
    break;
  }
    

  if( dstErr ){
    *dstErr = errNum;
  }
  return rtnImage;
}

void HGU_XmMapDialogCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  Widget	dialog=(Widget) client_data;

  if( dialog ){
    XtManageChild(dialog);
    XtPopup(XtParent(dialog), XtGrabNone);
    XtMapWidget(XtParent(dialog));
    if( XmIsFileSelectionBox(dialog) == True ){
      XmFileSelectionDoSearch(dialog,  NULL);
    }
  }
  return;
}


void HGU_XmImageViewResetGreyRange(
  HGU_XmImageViewDataStruct	*data)
{
  WlzPixelV	greyMin, greyMax;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  if((data == NULL) || (data->obj == NULL)){
    return;
  }

  /* calculate the grey-range & reset the scale */
  errNum = WlzGreyRange(data->obj, &greyMin, &greyMax);
  switch( greyMin.type ){
  case WLZ_GREY_LONG:
    data->srcMin = greyMin.v.lnv;
    data->srcMax = greyMax.v.lnv;
    break;

  case WLZ_GREY_INT:
    data->srcMin = greyMin.v.inv;
    data->srcMax = greyMax.v.inv;
    break;

  case WLZ_GREY_SHORT:
    data->srcMin = greyMin.v.shv;
    data->srcMax = greyMax.v.shv;
    break;

  case WLZ_GREY_UBYTE:
    data->srcMin = greyMin.v.ubv;
    data->srcMax = greyMax.v.ubv;
    break;

  case WLZ_GREY_FLOAT:
    data->srcMin = greyMin.v.flv;
    data->srcMax = greyMax.v.flv;
    break;

  case WLZ_GREY_DOUBLE:
    data->srcMin = greyMin.v.dbv;
    data->srcMax = greyMax.v.dbv;
    break;

  case WLZ_GREY_BIT:
  case WLZ_GREY_RGBA:
  default:
    return;
  }

  return;
}

void HGU_XmImageViewSetLutTransform(
  HGU_XmImageViewDataStruct	*data)
{
  int		i, gi;
  double	g, A, B, fmin, fmax, mu, sig;

  /* check and clear existing */
  if( data == NULL ){
    return;
  }
  if( data->lut.g->lut ){
    AlcFree(data->lut.g->lut);
  }
  data->lut.g->lut = (UBYTE *) AlcMalloc(sizeof(char)*
				(data->srcMax - data->srcMin + 1));

  /* set new lut */
  /* calculate the transform */
  switch( data->transType ){
  default:
  case WLZ_GREYTRANSFORMTYPE_LINEAR:
    for(i=0, gi=data->srcMin; gi <= data->min; i++, gi++){
      data->lut.g->lut[i] = data->Min;
    }
    A = ((double) (data->Max - data->Min)) / (data->max - data->min + 1);
    B = data->Min - A*data->min;
    for(; gi <= data->srcMax; i++, gi++){
      data->lut.g->lut[i] = (int) (A*gi + B);
      if( gi == data->max ){
	break;
      }
    }
    for(; gi <= data->srcMax; i++, gi++){
      data->lut.g->lut[i] = data->Max;
    }
    break;

  case WLZ_GREYTRANSFORMTYPE_GAMMA:
    for(i=0, gi=data->srcMin; gi <= data->min; i++, gi++){
      data->lut.g->lut[i] = data->Min;
    }
    A = (data->Max - data->Min) / 
      pow((data->max - data->min + 1), data->gamma);
    B = data->Min;
    for(; gi <= data->srcMax; i++, gi++){
      g = gi;
      data->lut.g->lut[i] = (int) (A * pow(g - data->min, data->gamma) +
			      B);
      if( gi == data->max ){
	break;
      }
    }
    for(; gi <= data->srcMax; i++, gi++){
      data->lut.g->lut[i] = data->Max;
    }
    break;

  case WLZ_GREYTRANSFORMTYPE_SIGMOID:
    for(i=0, gi=data->srcMin; gi <= data->min; i++, gi++){
      data->lut.g->lut[i] = data->Min;
    }
    mu = data->mean + data->min;
    sig = data->sigma;
    fmin = 1.0/(1.0 + exp(-(data->min - mu)/sig));
    fmax = 1.0/(1.0 + exp(-(data->max + 1 - mu)/sig));
    A = (data->Max - data->Min) / (fmax - fmin);
    B = data->Min - A * fmin;
    for(; gi <= data->srcMax; i++, gi++){
      g = gi;
      data->lut.g->lut[i] = (int) (A / (1.0 + exp(-(g - mu)/sig)) + B);
      if( gi == data->max ){
	break;
      }
    }
    for(; gi < data->srcMax; i++, gi++){
      data->lut.g->lut[i] = data->Max;
    }
    break;

  }

  if( data->invert ){
    for(; i >0;){
      i--;
      data->lut.g->lut[i] = (data->Max + data->Min - data->lut.g->lut[i]);
    }
  }

  return;
}

static void HGU_XmImageViewCanvasInputCb(
  Widget          w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  HGU_XmImageViewDataStruct
    *data=(HGU_XmImageViewDataStruct *) client_data;
  XmAnyCallbackStruct	*cbs = (XmAnyCallbackStruct *) call_data;
  int			x, y;

  /* switch on event type */
  switch( cbs->event->type ){

  case ButtonPress:
    switch( cbs->event->xbutton.button ){

    case Button1:
      break;

    case Button2:
      break;

    case Button3:		/* unused */
    default:
      break;

    }
    break;

  case ButtonRelease:
    switch( cbs->event->xbutton.button ){

    case Button1:
      break;

    case Button2:
      break;

    default:
      break;

    }
    break;

  case MotionNotify: /* always display feedback on position and domain */
    x = cbs->event->xmotion.x;
    y = cbs->event->xmotion.y;
    /*display_pointer_feedback_information(x, y);*/
    break;

  case KeyPress:
    switch( XLookupKeysym(&(cbs->event->xkey), 0) ){
	
    case XK_Right:
    case XK_f:
      break;

    case XK_Up:
    case XK_p:
      break;

    case XK_Left:
    case XK_b:
      break;

    case XK_Down:
    case XK_n:
      break;

    case XK_w:
      /* get a filename for the view image */
      break;

    }
    break;

  default:
    break;
  }

  return;
}
 
static void HGU_XmImageViewCanvasMotionEventHandler(
  Widget        w,
  XtPointer     client_data,
  XEvent        *event,
  Boolean       *continue_to_dispatch)
{
  XmDrawingAreaCallbackStruct cbs;

  /* call the canvas input callbacks */
  cbs.reason = XmCR_INPUT;
  cbs.event = event;
  cbs.window = XtWindow(w);
  XtCallCallbacks(w, XmNinputCallback, (XtPointer) &cbs);

  return;
}

void HGU_XmImageViewCanvasExposeCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  HGU_XmImageViewDataStruct
    *viewData=(HGU_XmImageViewDataStruct *) client_data;
  XmDrawingAreaCallbackStruct
    *cbs=(XmDrawingAreaCallbackStruct *) call_data;
  int	xExp, yExp, wExp, hExp;
  int	xImg, yImg, wImg, hImg;
  Dimension	width, height, rW, rH;
  XWindowAttributes	win_att;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  /* check object */
  if((viewData == NULL) || (viewData->obj == NULL)){
    return;
  }

  /* check ximage re-generate as required */
  if( viewData->ximage == NULL ){
    if( (viewData->ximage = 
	 HGU_XmObjToXImageLut(w, viewData->obj, viewData->lut,
			      &errNum)) == NULL ){
      return;
    }
  }

  /* check graphics context */
  if( viewData->gc == (GC) 0 ){
    XGCValues	gc_values;
    viewData->gc = XCreateGC(XtDisplay(viewData->canvas),
			     XtWindow(viewData->canvas), 0, &gc_values);
  }

  /* check event to get expose region */
  XtVaGetValues(viewData->canvas, XmNwidth, &width,
		XmNheight, &height, NULL);
  if( (cbs == NULL) || (cbs->event == NULL) || (cbs->event->type != Expose) ){
    Widget	scrolled_window, x_bar, y_bar;
    int 	minimum, maximum, value, size;
    
    /* the whole visible region */
    scrolled_window = XtParent(viewData->canvas);
    while( !XtIsSubclass(scrolled_window, xmScrolledWindowWidgetClass) ){
      scrolled_window = XtParent(scrolled_window);
    }

    /* get the scrollbars */
    XtVaGetValues(scrolled_window,
		  XmNhorizontalScrollBar, &x_bar,
		  XmNverticalScrollBar, &y_bar,
		  NULL);

    /* set x expose region */
    XtVaGetValues(x_bar,
		  XmNminimum, &minimum,
		  XmNmaximum, &maximum,
		  XmNvalue,   &value,
		  XmNsliderSize, &size, NULL);
    xExp = value * width / (maximum - minimum);
    wExp = size * width / (maximum - minimum);
      
    /* set y expose region */
    XtVaGetValues(y_bar,
		  XmNminimum, &minimum,
		  XmNmaximum, &maximum,
		  XmNvalue,   &value,
		  XmNsliderSize, &size, NULL);
    yExp = value * height / (maximum - minimum);
    hExp = size * height / (maximum - minimum);
  }
  else {
    xExp = cbs->event->xexpose.x;
    yExp = cbs->event->xexpose.y;
    wExp = cbs->event->xexpose.width;
    hExp = cbs->event->xexpose.height;
  }

  /* find matching region in the ximage */
  xImg = (xExp) / viewData->magVal;
  yImg = (yExp) / viewData->magVal;
  wImg = xImg + wExp / viewData->magVal + 2;
  hImg = yImg + hExp / viewData->magVal + 2;

  if( wImg < 0 ){
    return;
  }
  else if( xImg > viewData->ximage->width ){
    return;
  }
  else if( xImg < 0 ){
    xImg = 0;
  }
  if( wImg > viewData->ximage->width ){
    wImg = viewData->ximage->width - xImg + 1;
  }
  else {
    wImg = wImg - xImg + 1;
  }

  if( hImg < 0 ){
    return;
  }
  else if( yImg > viewData->ximage->height ){
    return;
  }
  else if( yImg < 0 ){
    yImg = 0;
  }
  if( hImg > viewData->ximage->height ){
    hImg = viewData->ximage->height - yImg + 1;
  }
  else {
    hImg = hImg - yImg + 1;
  }

  xExp = (xImg) * viewData->magVal;
  yExp = (yImg) * viewData->magVal;

  /* display ximage region */
  if( viewData->magVal > 1.0 ){
    /* create a temporary ximage  - assume 24 bit */
    unsigned int	*data, *orig_data, *toPtr, *fromPtr;
    int		intMag = WLZ_NINT(viewData->magVal);
    int		i, j, k;
    XImage	*tmpImage;

    data = (unsigned int *) AlcMalloc(sizeof(unsigned int)
				      * wImg * hImg * intMag * intMag);
    toPtr = data;
    orig_data = (unsigned int *) viewData->ximage->data;
    for(j=0; j < hImg; j++){
      fromPtr = orig_data + (j + yImg) * viewData->ximage->width + xImg;
      for(i=0; i < wImg; i++, fromPtr++){
	for(k=0; k < intMag; k++, toPtr++){
	  *toPtr = *fromPtr;
	}
      }
      for(k=1; k < intMag; k++){
	memcpy(toPtr, toPtr - wImg*intMag, sizeof(int)*wImg*intMag);
	toPtr += wImg*intMag;
      }
    }
    if( XGetWindowAttributes(XtDisplay(viewData->canvas), XtWindow(viewData->canvas),
			     &win_att) ){
      tmpImage = XCreateImage(XtDisplay(viewData->canvas),
			      win_att.visual, win_att.depth,
			      ZPixmap, 0, (char *) data,
			      wImg*intMag, hImg*intMag, 8, 0);
      XPutImage(XtDisplay(viewData->canvas), XtWindow(viewData->canvas),
		viewData->gc, tmpImage,
		0, 0, xExp, yExp, wImg*intMag, hImg*intMag);
      AlcFree(data);
      tmpImage->data = NULL;
      XDestroyImage(tmpImage);
    }
  }
  else if( viewData->magVal < 1.0 ){
    /* create a temporary ximage  - assume 24 bit */
    unsigned int	*data, *orig_data, *toPtr, *fromPtr;
    int		intMag = WLZ_NINT(1.0/viewData->magVal);
    int		i, j, k, w, h;
    XImage	*tmpImage;

    w = wImg / intMag;
    h = hImg / intMag;
    data = (unsigned int *) AlcMalloc(sizeof(unsigned int) * w *h);
    toPtr = data;
    orig_data = (unsigned int *) viewData->ximage->data;
    for(j=0; j < h; j++){
      fromPtr = orig_data + (j*intMag + yImg) * viewData->ximage->width + xImg;
      for(i=0; i < w; i++, toPtr++, fromPtr += intMag){
	*toPtr = *fromPtr;
      }
    }
    if( XGetWindowAttributes(XtDisplay(viewData->canvas), XtWindow(viewData->canvas),
			     &win_att) ){
      tmpImage = XCreateImage(XtDisplay(viewData->canvas),
			      win_att.visual, win_att.depth,
			      ZPixmap, 0, (char *) data,
			      w, h, 8, 0);
      XPutImage(XtDisplay(viewData->canvas), XtWindow(viewData->canvas),
		viewData->gc, tmpImage,
		0, 0, xExp, yExp, w, h);
      AlcFree(data);
      tmpImage->data = NULL;
      XDestroyImage(tmpImage);
    }
  }
  else {
    XPutImage(XtDisplay(viewData->canvas), XtWindow(viewData->canvas),
	      viewData->gc, viewData->ximage,
	      xImg, yImg, xExp, yExp, wImg, hImg);
  }

  return;
}

void HGU_XmImageViewControlledInstallImage(
  WlzObject			*obj,
  HGU_XmImageViewDataStruct	*data,
  unsigned int			cntrlMask)
{
  if( data == NULL ){
    return;
  }
  if( obj == NULL || (obj->type != WLZ_2D_DOMAINOBJ) ){
    if( cntrlMask & HGU_XmIMAGEVIEW_CLEARONINVALID ){
      obj = NULL;
    }
    else {
      return;
    }
  }

  /* clear old object and ximage */
  if( data->obj ){
    WlzFreeObj(data->obj);
    data->obj = NULL;
  }
  if( data->ximage ){
    if( data->ximage->data ){
      AlcFree(data->ximage->data);
      data->ximage->data = NULL;
    }
    XDestroyImage(data->ximage);
    data->ximage = NULL;
  }

  if( obj ){
    Dimension	width, height;
    data->obj = WlzAssignObject(obj, NULL);

    /* reset the LUT */
    HGU_XmImageViewResetGreyRange(data);
    if( !(cntrlMask & HGU_XmIMAGEVIEW_KEEPLUTCONTROLVALUES) ){
      HGU_XmImageViewResetLutControlValues(data);
    }
    HGU_XmImageViewSetLutTransform(data);
    HGU_XmImageViewSetLutControls(data);

    /* call canvas expose to provoke display */
    if( !(cntrlMask & HGU_XmIMAGEVIEW_KEEPMAGVALUE) ){
      data->magVal = 1.0;
    }
    data->width = data->obj->domain.i->lastkl - data->obj->domain.i->kol1 + 1;
    data->height = data->obj->domain.i->lastln - data->obj->domain.i->line1 + 1;
    width = data->width * data->magVal;
    height = data->height * data->magVal;
    XtVaSetValues(data->canvas, XmNwidth, width,
		  XmNheight, height, NULL);
    XtCallCallbacks(data->canvas, XmNexposeCallback, NULL);
  }
  else {
    /* clear the canvas, leave data etc unchanged */
    XClearWindow(XtDisplay(data->canvas), XtWindow(data->canvas));
  }

  return;
}

void HGU_XmImageViewInstallImage(
  WlzObject			*obj,
  HGU_XmImageViewDataStruct	*data)
{
  HGU_XmImageViewControlledInstallImage(obj, data, 0x0);
  return;
}

void HGU_XmImageViewReadImageFromParamsCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  HGU_XmImageViewDataStruct
    *data=(HGU_XmImageViewDataStruct *) client_data;
  XmFileSelectionBoxCallbackStruct
    *cbs=(XmFileSelectionBoxCallbackStruct *) call_data;
  int			intType;
  String		strBuf, tmpBuf;
  FILE			*fp;
  WlzObject		*obj;

  /* check image file, read image */
  if( data->file == NULL ){
    /* report error */
    return;
  }

  switch( data->type ){
  case WLZEFF_FORMAT_WLZ:
  case WLZEFF_FORMAT_PNM:
  case WLZEFF_FORMAT_BMP:
  case WLZEFF_FORMAT_TIFF:
    obj = WlzAssignObject(WlzEffReadObj(NULL, data->file,
					data->type, 0, NULL), NULL);
    break;
    
  case WLZEFF_FORMAT_RAW:
    strBuf = AlcMalloc(sizeof(char) *
		       (strlen(data->file) + 48));
    switch( data->depth ){
    case 8:
      intType = 3;
      break;
    case 12:
      intType = 2;
      break;
    case 16:
      intType = 7;
      break;
    case 32:
      intType = 1;
      break;
    }
    sprintf(strBuf, "WlzRawToWlz -%s %d %d %d %s", data->byteOrder?"b":"l",
	    data->width, data->height, intType, data->file);
    if( fp = popen(strBuf, "r") ){
      obj = WlzAssignObject(WlzEffReadObj(fp, NULL,
					  WLZEFF_FORMAT_WLZ, 0, NULL), NULL);
      pclose(fp);
    }
    AlcFree(strBuf);
    break;

  default:
    /* report error */
    return;
  }

  if( obj == NULL ){
    HGU_XmUserError(data->canvas,
		    "Failed to read the image, please\n"
		    "check the file, permissions and the\n"
		    "image format and try again.\n",
		    XmDIALOG_FULL_APPLICATION_MODAL);
    return;
  }
  if( obj->type != WLZ_2D_DOMAINOBJ ){
    WlzFreeObj(obj);
    return;
  }

  /* install the object */
  HGU_XmImageViewInstallImage(obj, data);
  WlzFreeObj(obj);

  return;
}

void HGU_XmImageViewReadImageCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  HGU_XmImageViewDataStruct
    *data=(HGU_XmImageViewDataStruct *) client_data;
  XmFileSelectionBoxCallbackStruct
    *cbs=(XmFileSelectionBoxCallbackStruct *) call_data;
  Widget		option;
  int			intType;
  String		strBuf, tmpBuf;
  FILE			*fp;
  WlzObject		*obj;

  /* get image type and file */
  XtVaGetValues(data->typeMenu, XmNmenuHistory, &option, NULL);
  if(WlzStringMatchValue(&intType, XtName(option),
			 "woolz", WLZEFF_FORMAT_WLZ,
			 "pgm", WLZEFF_FORMAT_PNM,
			 "bmp", WLZEFF_FORMAT_BMP,
			 "tiff", WLZEFF_FORMAT_TIFF,
			 "raw", WLZEFF_FORMAT_RAW,
			 NULL)){
    data->type = (WlzEffFormat) intType;
  }
  else {
    data->type = WLZEFF_FORMAT_RAW;
  }
  data->file = HGU_XmGetFileStr(data->imageForm, cbs->value, cbs->dir);

  /* read the image */
  switch( data->type ){
  case WLZEFF_FORMAT_WLZ:
    if( (fp = HGU_XmGetFilePointer(data->imageForm, cbs->value,
				   cbs->dir, "r")) == NULL )
    {
      return;
    }
    obj = WlzAssignObject(WlzEffReadObj(fp, NULL,
					      data->type, 0, NULL), NULL);
    fclose(fp);
    break;

  case WLZEFF_FORMAT_PNM:
  case WLZEFF_FORMAT_BMP:
  case WLZEFF_FORMAT_TIFF:
    if( data->file == NULL )
    {
      return;
    }
    obj = WlzAssignObject(WlzEffReadObj(NULL, data->file,
					      data->type, 0, NULL), NULL);
    break;

  case WLZEFF_FORMAT_RAW:
    if( data->file == NULL )
    {
      return;
    }
    /* get the data size and byte ordering */
    tmpBuf = (String) AlcMalloc(sizeof(char) * 32);
    sprintf(tmpBuf, "%d,%d,%d,%d", data->width, data->height,
	    data->depth, data->byteOrder? 1 : 0);
    if( strBuf = HGU_XmUserGetstr(w,
				  "Please type in: w, h, d, o where:\n"
				  "\n"
				  "\tw = image width in pixels\n"
				  "\th = image height in pixels\n"
				  "\td = number of bits per pixel\n"
				  "\to = byte order:\n"
				  "      1 - big-endian\n"
				  "      0 - small-endian",
				  "Ok", "Cancel", tmpBuf) ){
      if( sscanf(strBuf, "%d,%d,%d,%d", &(data->width), &(data->height),
		 &(data->depth), &(data->byteOrder)) < 4 ){
	AlcFree(strBuf);
	AlcFree(tmpBuf);
	return;
      }
      AlcFree(strBuf);
      AlcFree(tmpBuf);
    }
    else {
      return;
    }
    strBuf = AlcMalloc(sizeof(char) *
		       (strlen(data->file) + 48));
    switch( data->depth ){
    case 8:
      intType = 3;
      break;
    case 12:
      intType = 2;
      break;
    case 16:
      intType = 7;
      break;
    case 32:
      intType = 1;
      break;
    }
    sprintf(strBuf, "WlzRawToWlz -%s %d %d %d %s", data->byteOrder?"b":"l",
	    data->width, data->height, intType, data->file);
    if( fp = popen(strBuf, "r") ){
      obj = WlzAssignObject(WlzEffReadObj(fp, NULL,
					  WLZEFF_FORMAT_WLZ, 0, NULL), NULL);
      pclose(fp);
    }
    AlcFree(strBuf);
    break;
  }
  if( obj == NULL ){
    HGU_XmUserError(data->canvas,
		    "Failed to read the image, please\n"
		    "check the file, permissions and the\n"
		    "image format and try again.\n",
		    XmDIALOG_FULL_APPLICATION_MODAL);
    return;
  }
  if( obj->type != WLZ_2D_DOMAINOBJ ){
    WlzFreeObj(obj);
    return;
  }

  /* install the object */
  HGU_XmImageViewInstallImage(obj, data);
  WlzFreeObj(obj);

  return;
}

void HGU_XmImageViewMagPlusCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  HGU_XmImageViewDataStruct
    *data=(HGU_XmImageViewDataStruct *) client_data;
  Dimension width, height;

  if( data->magVal < 16 ){
    data->magVal *= 2.0;
  }
  
  width = data->width * data->magVal;
  height = data->height * data->magVal;
  XtVaSetValues(data->canvas, XmNwidth, width, XmNheight, height, NULL);
  XtCallCallbacks(data->canvas, XmNexposeCallback, NULL);

  return;
}

void HGU_XmImageViewMagMinusCb(
  Widget	w,
  XtPointer	client_data,
  XtPointer	call_data)
{
  HGU_XmImageViewDataStruct
    *data=(HGU_XmImageViewDataStruct *) client_data;
  Dimension width, height;

  if( data->magVal > 0.25 ){
    data->magVal /= 2.0;
  }
  
  width = data->width * data->magVal;
  height = data->height * data->magVal;
  XtVaSetValues(data->canvas, XmNwidth, width, XmNheight, height, NULL);
  XtCallCallbacks(data->canvas, XmNexposeCallback, NULL);

  return;
}

static XtResource HGU_XmGreyMappingDialogTitleResources[] =
{
  {"*grey_mapping_popup.title", XmCString, XmRString, sizeof(String),
   0, XtRString, "Grey Mapping Dialog"},
};

static XtResource HGU_XmImageViewReadTitleResources[] =
{
  {"*image_select_popup.title", XmCString, XmRString, sizeof(String),
   0, XtRString, "Image Select Dialog"},
};

static XtResource HGU_XmImageViewReadChoiceResources[] =
{
  {"*file_type.labelString", XmCXmString, XmRXmString, sizeof(XmString),
   0, XtRString, "Select image type:"},
};

Widget HGU_XmCreateImageView(
  String	name,
  Widget	parent,
  int		mappingDialogFlg)
{
  Widget	image_form, form, button, text, scrolled_window, canvas, label;
  HGU_XmImageViewDataStruct	*data;
  String			titleStr;
  XmString			choiceStr;

  /* create the top-level form */
  image_form = XtVaCreateManagedWidget(name, xmFormWidgetClass,
				       parent,
				       XmNwidth, 400,
				       XmNheight, 400,
				       NULL);
  data = (HGU_XmImageViewDataStruct *)
    AlcCalloc(sizeof(HGU_XmImageViewDataStruct), 1);
  XtVaSetValues(image_form, XmNuserData, data, NULL);
  data->imageForm = image_form;
  data->srcMin = -10000;
  data->srcMax = 10000;
  data->min = -10000;
  data->max = 10000;
  data->Min = 0;
  data->Max = 255;    
  data->width = 696;
  data->height = 520;
  data->depth = 12;
  data->byteOrder = 1;
  data->magVal = 1.0;
  data->transType = WLZ_GREYTRANSFORMTYPE_LINEAR;
  data->gamma = 1.0;
  data->invert = 0;
  data->lut.g = (HGU_XmLutGrey *) AlcMalloc(sizeof(HGU_XmLutGrey));
  data->lut.g->lut = NULL;
  data->lut.g->min = data->Min;
  data->lut.g->max = data->Max;

  /* create file-selector dialog */
  XtGetApplicationResources(image_form, &titleStr,
			    HGU_XmImageViewReadTitleResources, 1, NULL, 0);
  data->fileSelector = XmCreateFileSelectionDialog(image_form,
						   "image_select", NULL, 0);
  XtVaSetValues(XtParent(data->fileSelector), XmNtitle, titleStr, NULL);
  XtAddCallback(data->fileSelector, XmNokCallback, 
		HGU_XmImageViewReadImageCb, (XtPointer) data);
  XtAddCallback(data->fileSelector, XmNokCallback, 
		PopdownCallback, NULL);
  XtAddCallback(data->fileSelector, XmNcancelCallback, 
		PopdownCallback, NULL);

  form = XtVaCreateManagedWidget("read_file_form", xmFormWidgetClass,
				 data->fileSelector,
				 XmNborderWidth,	0,
				 NULL);

  /* add a file type selection menu */
  XtGetApplicationResources(image_form, &choiceStr,
			    HGU_XmImageViewReadChoiceResources, 1, NULL, 0);
  data->typeMenu = HGU_XmBuildMenu(form, XmMENU_OPTION,
				   "file_type", 0, XmTEAR_OFF_DISABLED,
				   file_type_menu_itemsP);
  XtVaSetValues(data->typeMenu,
		XmNtopAttachment,	XmATTACH_FORM,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNrightAttachment,	XmATTACH_FORM,
		XmNlabelString, 	choiceStr,
		NULL);
  XtManageChild(data->typeMenu);

  /* create the mapping dialog */
  XtGetApplicationResources(parent, &titleStr,
			    HGU_XmGreyMappingDialogTitleResources, 1, NULL, 0);
  data->greyMapping = HGU_XmCreateGreyMappingDialog("grey_mapping", parent,
						    data);
  XtVaSetValues(XtParent(data->greyMapping), XmNtitle, titleStr, NULL);

  /* add the button row for mag+, mag-, I/O and re-mapping */
  form = XtVaCreateManagedWidget("image_buttons_form", xmFormWidgetClass,
				 image_form,
				 XmNleftAttachment, XmATTACH_FORM,
				 XmNrightAttachment, XmATTACH_FORM,
				 XmNbottomAttachment, XmATTACH_FORM,
				 NULL);
  button = XtVaCreateManagedWidget("Read", xmPushButtonWidgetClass, form,
				   XmNleftAttachment, XmATTACH_FORM,
				   XmNtopAttachment, XmATTACH_FORM,
				   XmNbottomAttachment, XmATTACH_FORM,
				   NULL);
  XtAddCallback(button, XmNactivateCallback, HGU_XmMapDialogCb,
		data->fileSelector);
  button = XtVaCreateManagedWidget("Mag+", xmPushButtonWidgetClass, form,
				   XmNleftAttachment, XmATTACH_WIDGET,
				   XmNleftWidget, button,
				   XmNtopAttachment, XmATTACH_FORM,
				   XmNbottomAttachment, XmATTACH_FORM,
				   NULL);
  XtAddCallback(button, XmNactivateCallback,
		HGU_XmImageViewMagPlusCb, data);
  button = XtVaCreateManagedWidget("Mag-", xmPushButtonWidgetClass, form,
				   XmNleftAttachment, XmATTACH_WIDGET,
				   XmNleftWidget, button,
				   XmNtopAttachment, XmATTACH_FORM,
				   XmNbottomAttachment, XmATTACH_FORM,
				   NULL);
  XtAddCallback(button, XmNactivateCallback,
		HGU_XmImageViewMagMinusCb, data);
  text = XtVaCreateManagedWidget("image_fb", xmTextFieldWidgetClass, form,
				 XmNleftAttachment, XmATTACH_WIDGET,
				 XmNleftWidget, button,
				 XmNtopAttachment, XmATTACH_FORM,
				 XmNbottomAttachment, XmATTACH_FORM,
				 XmNcolumns, 10,
				 NULL);
  button = XtVaCreateManagedWidget("Map greys", xmPushButtonWidgetClass,
				   form,
				   XmNrightAttachment, XmATTACH_FORM,
				   XmNtopAttachment, XmATTACH_FORM,
				   XmNbottomAttachment, XmATTACH_FORM,
				   NULL);
  XtAddCallback(button, XmNactivateCallback,
		HGU_XmMapDialogCb, data->greyMapping);

  /* add scrolling window and canvas */
  scrolled_window = XtVaCreateManagedWidget("image_sc",
					    xmScrolledWindowWidgetClass,
					    image_form,
					    XmNscrollingPolicy, XmAUTOMATIC,
					    XmNleftAttachment, XmATTACH_FORM,
					    XmNrightAttachment, XmATTACH_FORM,
					    XmNtopAttachment, XmATTACH_FORM,
					    XmNbottomAttachment, XmATTACH_WIDGET,
					    XmNbottomWidget, form,
					    NULL);
  data->width = 512;
  data->height = 512;
  canvas = XtVaCreateManagedWidget("canvas", xmDrawingAreaWidgetClass,
				   scrolled_window,
				   XmNwidth,  data->width,
				   XmNheight, data->height,
				   NULL);
  XtAddCallback(canvas, XmNexposeCallback, HGU_XmImageViewCanvasExposeCb, data);
  XtAddCallback(canvas, XmNinputCallback, HGU_XmImageViewCanvasInputCb, data);
  XtAddEventHandler(canvas, PointerMotionMask,
		    False, HGU_XmImageViewCanvasMotionEventHandler, data);
  data->canvas = canvas;


  return image_form;
}
  
