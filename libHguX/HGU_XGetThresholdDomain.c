#pragma ident "MRC HGU $Id$"
/*****************************************************************************
* Copyright   :    1994 Medical Research Council, UK.                        *
*                  All rights reserved.                                      *
******************************************************************************
* Address     :    MRC Human Genetics Unit,                                  *
*                  Western General Hospital,                                 *
*                  Edinburgh, EH4 2XU, UK.                                   *
******************************************************************************
* Project     :    libhguX - MRC HGU X11 Utilities			     *
* File        :    HGU_XGetThresholdDomain.c				     *
* $Revision$
******************************************************************************
* Author Name :    Richard Baldock					     *
* Author Login:    richard@hgu.mrc.ac.uk				     *
* Date        :    Mon Nov 21 10:26:09 1994				     *
* Synopsis    :    Use thresholding of the reference object modify the	     * 
*		   given start domain.					     *
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <HGU_XInteract.h>

static char *help_str = 
"HGU_XGetThresholdDomain: ";

/* local routines */
static WlzObject *get_thresh_obj(
    WlzObject	*ref_obj,
    int		x,
    int		y,
    int		thresh_up,
    int		thresh_down)
{
    WlzObject	*obj1, *obj2, **obj_list;
    WlzGreyP	greyp;
    WlzPixelV	threshV;
    WlzGreyType	gtype;
    WlzGreyValueWSpace	*gVWSp = NULL;
    int		i, num_obj;

    /* get the selected grey-value */
    gtype = WlzGreyTableTypeToGreyType(ref_obj->values.core->type, NULL);
    gVWSp = WlzGreyValueMakeWSp(ref_obj, NULL);
    threshV.type = WLZ_GREY_INT;
    WlzGreyValueGet(gVWSp, 0, (double) y, (double) x);
    switch(gtype)
    {
    case WLZ_GREY_INT: 
      threshV.v.inv = (int) ((*(gVWSp->gVal)).inv);
      break;
    case WLZ_GREY_SHORT:
      threshV.v.inv = (int) ((*(gVWSp->gVal)).shv);
      break;
    case WLZ_GREY_UBYTE:
      threshV.v.inv = (int) ((*(gVWSp->gVal)).ubv);
      break;
    case WLZ_GREY_FLOAT:
      threshV.v.inv = (int) ((*(gVWSp->gVal)).flv);
      break;
    case WLZ_GREY_DOUBLE:
      threshV.v.inv = (int) ((*(gVWSp->gVal)).dbv);
      break;
    }
    WlzGreyValueFreeWSp(gVWSp);

    /* threshold within the required range */
    threshV.v.inv -= thresh_down;
    if( (obj1 = WlzThreshold(ref_obj, threshV, WLZ_THRESH_HIGH,
			     NULL)) == NULL)
    {
	return( NULL );
    }
    threshV.v.inv += thresh_down + thresh_up;
    if( (obj2 = WlzThreshold(obj1, threshV, WLZ_THRESH_HIGH,
			     NULL)) == NULL )
    {
	WlzFreeObj( obj1 );
	return( NULL );
    }
    WlzFreeObj( obj1 );
    obj1 = WlzMakeMain(WLZ_2D_DOMAINOBJ, obj2->domain, obj2->values,
		       NULL, NULL, NULL);

    /* find the object at the test point */
    WlzLabel(obj1, &num_obj, &obj_list, 2047, 1, WLZ_8_CONNECTED);
    WlzFreeObj( obj1 );
    WlzFreeObj( obj2 );
    obj2 = NULL;

    for(i=0; i < num_obj; i++){
	if( WlzInsideDomain( obj_list[i], 0.0, 
			    (double) y, (double) x, NULL ) ){
	    obj2 = obj_list[i];
	} else
	    WlzFreeObj( obj_list[i] );
    }
    AlcFree((void *) obj_list);

    return( obj2 );
}


/* public routines */
unsigned long HGU_XColourFromName(
    Display	*dpy,
    Window	win,
    char	*name)
{
    XColor		xcolor;
    XWindowAttributes	win_att;

    if( !XGetWindowAttributes(dpy, win, &win_att) )
    {
	return( ~0x0 );
    }

    if( !XParseColor(dpy, win_att.colormap, name, &xcolor) )
    {
	return( ~0x0 );
    }

    XAllocColor(dpy, win_att.colormap, &xcolor);
    return( xcolor.pixel );
}

WlzObject *HGU_XGetThresholdDomain(
Display			*dpy,
Window			win,
int			mode,
HGU_XInteractCallbacks	*callbacks,
HGU_XGridConstraint	*constraint,
HGU_XDomainStyleStruct	*style,
WlzObject		*start,
WlzObject		*ref_obj)
{
    XEvent			event;
    GC				gc, gc_copy, gc_bound = NULL;
    Pixmap			pixmap, pixmap1, stipple;
    XWindowAttributes		win_att;
    UndoObjectList		undo_list;
    WlzObject			*obj, *obj1=NULL, *obj2=NULL, *thresh_obj=NULL;
    WlzObject			*new_ref_obj;
    WlzGreyP			greyp;
    int				finished=0;
    int				x, y;
    static int			x1=0, y1=0;
    static int			thresh = -1;
    int				add_domain = 1;

    /* check reference object */
    if( ref_obj == NULL || ref_obj->type != WLZ_2D_DOMAINOBJ )
	return( WlzMakeMain(start->type, start->domain, start->values,
			    NULL, NULL, NULL) );
    new_ref_obj = WlzMakeMain(WLZ_2D_DOMAINOBJ, ref_obj->domain,
			      ref_obj->values, NULL, NULL, NULL);

    /* get domain gc and copy gc */
    gc = HGU_XCreateDomainGC(dpy, win, style, &stipple);
    gc_copy = HGU_XGetGC( dpy, win, HGU_XGC_COPY );
    
    /* copy the window into a pixmap */
    if( XGetWindowAttributes(dpy, win, &win_att) == 0 ){
	HGU_XError(dpy, win, "HGU_XGetThresholdDomain",
		   "failed to get window attributes", 0);
	XFreeGC( dpy, gc );
	return( NULL );
    }
    pixmap = XCreatePixmap(dpy, win, win_att.width, win_att.height,
			   win_att.depth);
    XCopyArea(dpy, win, pixmap, gc_copy, 0, 0, win_att.width, win_att.height,
	      0, 0);
    
    /* check starting domain */
    if( start != NULL ){
	obj = HGU_XIncrementDomain(dpy, win, pixmap, gc, NULL, start);
    } else {
	obj = NULL;
    }

    /* initialise the undo list */
    undo_list = push_undo_object( NULL, obj );

    /* select input and passive button grab */
    XSelectInput(dpy, win, win_att.your_event_mask |
		 KeyPressMask | KeyReleaseMask |
		 EnterWindowMask | LeaveWindowMask );
    if( QueryInWindow( dpy, win ) ){
	HGU_XGrabPointer(dpy, win, mode&HGU_XConfineMask);
    }

    /* save an area for the grey-value display */
    pixmap1 = XCreatePixmap(dpy, win, 64, 32,
			   win_att.depth);
    XCopyArea(dpy, win, pixmap1, gc_copy, x1+8, y1+4, 64, 32,
	      0, 0);

    /* get the new vertex */
    /* wait for finished button (button 3) */
    while( !finished ){
	
	XNextEvent(dpy, &event);
	if( event.xany.window != win ){
	    finished = HGU_XInteractNonwindowEvent(dpy, win, mode, callbacks,
						   &event);
	    continue;
        }

	switch( event.type ){

	case ButtonPress:

	    switch( event.xbutton.button ){
	    case Button1:	/* first selected point */
	    case Button2:	/* change mode */

		x = event.xbutton.x;
		y = event.xbutton.y;

		/* starting a new domain therefore reset thresh */
		thresh = -1;

		/* check for magnify option */
		if( event.xbutton.state & ShiftMask ){
		    break;
		}

		/* clear grey-value string */
		XCopyArea(dpy, pixmap1, win, gc_copy, 0, 0, 64, 32,
			  x1+8, y1+4);
		x1 = x;
		y1 = y;

		/* check for constraint domain option */
		if( event.xbutton.state & ControlMask ){
		    HGU_XDomainStyleStruct	domain_style;
		    Pixmap			stipple_bound;

		    /* set up callbacks and style structures */
		    domain_style.blob_radius  = 6;
		    domain_style.blob_type    = HGU_XBLOBTYPE_CIRCLE;
		    domain_style.blob_border  = 0;
		    domain_style.colour       =
			(int) HGU_XColourFromName(dpy, win, "green");
		    domain_style.plane_mask   = 255;
		    domain_style.stipple      = 0;
		    domain_style.stipple_file = NULL;

		    /* undisplay the constraint region complement*/
		    if( gc_bound == NULL ){
			gc_bound = HGU_XCreateDomainGC(dpy, win,
						       &domain_style,
						       &stipple_bound);
		    }
		    obj2 = WlzDiffDomain(ref_obj, new_ref_obj, NULL);
		    HGU_XUndispDomain(dpy, win, pixmap, gc_bound, obj2);
		    if( obj2 )
			WlzFreeObj( obj2 );
		    HGU_XDispDomain(dpy, win, gc, obj);

		    /* get the new constraint region */
		    HGU_XUngrabPointer(dpy, win);
		    obj1 = HGU_XGetDomain(dpy, win, mode, callbacks,
					  constraint, &domain_style, NULL);
		    if( obj1 ){
			if( (obj2 = WlzIntersect2(ref_obj, obj1, NULL)) ){
			    if( new_ref_obj )
				WlzFreeObj(new_ref_obj);
			    new_ref_obj = WlzMakeMain(WLZ_2D_DOMAINOBJ,
						      obj2->domain,
						      ref_obj->values,
						      NULL, NULL, NULL);
			    WlzFreeObj( obj2 );
			}
			WlzFreeObj( obj1 );
		    } else {
			if( new_ref_obj )
			    WlzFreeObj(new_ref_obj);
			new_ref_obj = WlzMakeMain(WLZ_2D_DOMAINOBJ,
						  ref_obj->domain,
						  ref_obj->values,
						  NULL, NULL, NULL);
		    }

		    /* display the constraint region complement */
		    obj2 = WlzDiffDomain(ref_obj, new_ref_obj, NULL);
		    HGU_XDispDomain(dpy, win, gc_bound, obj2);
		    if( obj2 )
			WlzFreeObj( obj2 );

		    if( QueryInWindow( dpy, win ) ){
			HGU_XGrabPointer(dpy, win, mode&HGU_XConfineMask);
		    }
		    HGU_XDispDomain(dpy, win, gc, obj);
		    XCopyArea(dpy, win, pixmap1, gc_copy, x1+8, y1+4, 64, 32,
			      0, 0);

		    break;
		}

		/* get the current grey-value and threshold domain */
		if( thresh_obj = get_thresh_obj(new_ref_obj, x, y,
						style->thresh_width,
						style->thresh_width) ){
		    if( event.xbutton.state&Mod1Mask ){
			HGU_XUndispDomain(dpy, win, pixmap, gc_copy,
					  thresh_obj);
			add_domain = 0;
		    } else {
			HGU_XDispDomain(dpy, win, gc, thresh_obj);
			add_domain = 1;
		    }
		}
		break;

	    case Button3:	/* confirm */
		finished = 1;
		break;
	    }

		break;

	case ButtonRelease:

	    switch( event.xbutton.button ){
	    case Button1:	/* first selected point */
	    case Button2:	/* change mode */

		if( thresh_obj ){
		    /* save the previous object */
		    if( obj )
			undo_list = push_undo_object( undo_list, obj );

		    /* check add or subtract */
		    if( add_domain ){
			if( obj ){
			    obj1 = WlzUnion2(obj, thresh_obj, NULL);
			    WlzFreeObj( obj );
			    obj = obj1;
			} else {
			    obj = WlzMakeMain(WLZ_2D_DOMAINOBJ,
					      thresh_obj->domain,
					      thresh_obj->values,
					      NULL, NULL, NULL);
			}
		    } else {
			if( obj ){
			    obj1 = WlzDiffDomain(obj, thresh_obj, NULL);
			    WlzFreeObj( obj );
			    obj = obj1;
			}
		    }
		    WlzFreeObj( thresh_obj );
		    thresh_obj = NULL;
		    HGU_XDispDomain(dpy, win, gc, obj);

		    XCopyArea(dpy, win, pixmap1, gc_copy, x1+8, y1+4, 64, 32,
			      0, 0);
		}

		break;
	    }

		break;

        case KeyPress:
            switch( XLookupKeysym(&event.xkey, 0) ){
            case XK_Delete:
	    case XK_BackSpace:
                break;
	    case XK_Right:
		break;
	    case XK_Left:
		break;
	    case XK_Up:		/* make threshold range bigger */
		if( event.xkey.state & ControlMask ){
		    ;
		}
		break;
	    case XK_Down:	/* make threshold range smaller */
		if( event.xkey.state & ControlMask ){
		    ;
		}
		break;
	    case XK_space:
                break;
	    case XK_Return:
		finished = 1;
		break;
	    case XK_Help:
		HGU_XInteractHelpEvent(dpy, win, callbacks, help_str);
		break;
	    case XK_u:	/* undo linked to following case */
	    case XK_U:
		if( !(event.xkey.state & ControlMask) )
		    break;
		/* don't add additional cases here */
	    case XK_Undo:
	    case XK_F14:
		/* clear grey-value string */
		XCopyArea(dpy, pixmap1, win, gc_copy, 0, 0, 64, 32,
			  x1+8, y1+4);

		if( obj != NULL ){
		    HGU_XUndispDomain(dpy, win, pixmap, gc_copy, obj);
		    WlzFreeObj( obj );
		    obj = NULL;
		}
		undo_list = pop_undo_object( undo_list, &obj );
		if( obj != NULL );
		    HGU_XDispDomain(dpy, win, gc, obj);

		XCopyArea(dpy, win, pixmap1, gc_copy, x1+8, y1+4, 64, 32,
			  0, 0);

		break;
            default:
                break;
            }
            break;
	    
        case KeyRelease:
            switch( XLookupKeysym(&event.xkey, 0) ){
            case XK_Delete:
	    case XK_BackSpace:
		break;
            default:
                break;
            }
            break;

	case MotionNotify:
	    /* check for button1 and pixel mode */
	    x = event.xmotion.x;
	    y = event.xmotion.y;
		
	    /* show grey-value from reference image */
	    if( (event.xmotion.state & (Button1Mask|Button2Mask))
		&& thresh_obj ){
		int	t;
		XEvent	tmp_event;

		/* check for further motion events */
		if( XCheckTypedWindowEvent(dpy, win,
					   MotionNotify, &tmp_event) )
		    break;

		t = style->thresh_width + (x - x1)/4;
		t = (t < 1) ? 1 : t;

		if( t == thresh )
		    break;

		/* update the display - don't delete previous to avoid
		   flashing */
		if( obj1 = get_thresh_obj(new_ref_obj, x1, y1, t, t) ){
		    WlzObject	*obj3;

		    if( (t < thresh) && 
		       (obj2 = WlzDiffDomain(thresh_obj, obj1, NULL)) ){
			if( !add_domain ){
			    if( obj3 = WlzIntersect2(obj2, obj, NULL) ){
				HGU_XDispDomain(dpy, win, gc, obj3);
				WlzFreeObj( obj3 );
			    }
			} else {
			    HGU_XUndispDomain(dpy, win, pixmap, gc_copy,
					      obj2);
			    HGU_XDispDomain(dpy, win, gc, obj);
			}
			WlzFreeObj(obj2);
		    }

		    if( thresh_obj )
			WlzFreeObj( thresh_obj );
		    thresh_obj = obj1;

		    if( !add_domain )
			HGU_XUndispDomain(dpy, win, pixmap, gc_copy,
					  thresh_obj);
		    else
			HGU_XDispDomain(dpy, win, gc, thresh_obj);

		    thresh = t;
		}

	    } else {
		/* display grey-value string */
		char	g_str[16];
		WlzGreyValueWSpace	*gVWSp = NULL;

		gVWSp = WlzGreyValueMakeWSp(ref_obj, NULL);
		WlzGreyValueGet(gVWSp, 0, (double) y, (double) x);
		switch( WlzGreyTableTypeToGreyType(
		  ref_obj->values.core->type, NULL) )
		{
		case WLZ_GREY_INT: 
		  (void) sprintf(&g_str[0], " %d ",
				 (int) ((*(gVWSp->gVal)).inv));
		  break;
		case WLZ_GREY_SHORT:
		  (void) sprintf(&g_str[0], " %d ",
				 (int) ((*(gVWSp->gVal)).shv));
		  break;
		case WLZ_GREY_UBYTE:
		  (void) sprintf(&g_str[0], " %d ",
				 (int) ((*(gVWSp->gVal)).ubv));
		  break;
		case WLZ_GREY_FLOAT:
		  (void) sprintf(&g_str[0], " %g ",
				 (float) ((*(gVWSp->gVal)).flv));
		  break;
		case WLZ_GREY_DOUBLE:
		  (void) sprintf(&g_str[0], " %g ",
				 (double) ((*(gVWSp->gVal)).dbv));
		  break;
		}
		WlzGreyValueFreeWSp(gVWSp);

		/* clear previous */
		XCopyArea(dpy, pixmap1, win, gc_copy, 0, 0, 64, 32,
			  x1+8, y1+4);

		/* save small area and display the new value */
		XCopyArea(dpy, win, pixmap1, gc_copy, x+8, y+4, 64, 32,
			  0, 0);
		XDrawImageString(dpy, win, gc_copy, x+8, y+24, &g_str[0],
				 (int) strlen(&g_str[0]));
		XFlush( dpy );
		x1 = x;
		y1 = y;
	    }
	    break;

	case EnterNotify:
	    HGU_XGrabPointer(dpy, win, mode&HGU_XConfineMask);
	    break;

	case LeaveNotify:
	    if( !(event.xcrossing.state & (Button1Mask|Button2Mask)) ){
		XCopyArea(dpy, pixmap1, win, gc_copy, 0, 0, 64, 32,
			  x1+8, y1+4);
		XFlush( dpy );
		HGU_XUngrabPointer(dpy, win);
	    }
	    break;

	case Expose:
	    XCopyArea(dpy, pixmap, win, gc_copy, 0, 0,
		      win_att.width, win_att.height, 0, 0);
	    HGU_XDispDomain(dpy, win, gc, obj);
	    break;

	default:
	    break;
	}
    }

    /* ungrab the button */
    HGU_XUngrabPointer(dpy, win);

    /* undefine the cross cursor */
    XUndefineCursor( dpy, win );

    /* reset input mask */
    XSelectInput(dpy, win, win_att.your_event_mask );

    /* clear pending events */
    while( XCheckWindowEvent(dpy, win, ~0x0, &event) == True )
	continue;
    
    /* clean up and return */
    XCopyArea(dpy, pixmap1, win, gc_copy, 0, 0, 64, 32,
	      x1+8, y1+4);
    HGU_XUndispDomain(dpy, win, pixmap, gc_copy, obj);
    if( stipple != (Pixmap) 0 )
	XFreePixmap( dpy, stipple );
    XFreeGC( dpy, gc );
    XFreePixmap( dpy, pixmap );
    if( gc_bound != NULL ){
	XFreeGC( dpy, gc_bound );
    }

    /* clear undo objects */
    clear_undo_object( undo_list );

    /* clear temporary reference object */
    if( new_ref_obj )
	WlzFreeObj( new_ref_obj );

    /* check size */
    if( obj != NULL && WlzArea( obj, NULL ) <= 0 ){
	WlzFreeObj( obj );
	obj = NULL;
    }

    return( obj );
}

