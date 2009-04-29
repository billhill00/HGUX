#if defined(__GNUC__)
#ident "MRC HGU $Id:"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id:"
#else static char _HGU_XmMenuUtils_c[] = "MRC HGU $Id:";
#endif
#endif
/*!
* \file         HGU_XmMenuUtils.c
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Wed Apr 29 09:16:50 2009
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
* \brief        Convenience routine for creating menus, copied		     *
*		from Heller - Motif Programming Manual, with some	     *
*		modifications, Heller's	comments:			     *
*                                                                            *
* Build popup, option and pulldown menus, depending on the menu type.        *
* It may be XmMENU_PULLDOWN, XmMENU_OPTION, or XmMENU_POPUP. Pulldowns       *
* return the CascadeButton that pops up the menu. Popups return the menu.    *
* Option menus are created, but the RowColumn that acts as the option        *
* "area" is returned unmanaged. (The user must manage it).                   *
* Pulldown menus are built from cascade buttons, so this function also       *
* builds pullright menus. The function also adds the right callback for      *
* PushButton or ToggleButton menu items                                      *
*               
* \todo         -
* \bug          None known
*
* Maintenance log with most recent changes at top of list.
*/

#include <stdio.h>
#include <stdlib.h>

#include <Xm/CascadeB.h>
#include <Xm/CascadeBG.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>

#include <HGU_XmUtils.h>

Widget HGU_XmBuildMenu(Widget		parent,
		       int		menu_type,
		       char		*menu_name,
		       char		menu_mnemonic,
		       unsigned char	tear_off_model,
		       MenuItem	*items)
{
    return( HGU_XmBuildRadioMenu(parent, menu_type, menu_name,
				 menu_mnemonic, tear_off_model,
				 False, False, items) );
}

Widget HGU_XmBuildPulldownMenu(
  Widget	menu_parent,
  unsigned char	tear_off_model,
  Boolean	radio_behavior,
  Boolean	always_one,
  MenuItem	*items)
{
  Widget	menu, widget;
  int		i, toggle;
  Boolean	one_toggle_set=False;
  Visual	*visual;
  Arg		arg[4];
    
  /* get the visual explicitly */
  visual = HGU_XmWidgetToVisual(menu_parent);
  XtSetArg(arg[0], XmNvisual, visual);
  XtSetArg(arg[1], XmNtearOffModel, tear_off_model);
  XtSetArg(arg[2], XmNradioBehavior, radio_behavior);
  XtSetArg(arg[3], XmNradioAlwaysOne, always_one);

  /* create the menu */
  menu = XmCreatePulldownMenu( menu_parent, "_pulldown", arg, 4 );
  XtVaSetValues(menu_parent, XmNsubMenuId, menu, NULL);

  /* now the menu items */
  for( i=0; items[i].name != NULL; i++ ){

    /* if subitems then build the new menu, this will return
       a cascade button as required. */
    if( items[i].subitems ){
      widget =
	HGU_XmBuildRadioMenu(menu, XmMENU_PULLDOWN,
			     items[i].name, items[i].mnemonic,
			     items[i].tear_off_model,
			     items[i].radio_behavior,
			     items[i].always_one,
			     items[i].subitems);
    } else {
      widget = XtVaCreateManagedWidget(items[i].name, *items[i].wclass,
				       menu, NULL);
    }

    /* any item can have accelerators except cascade buttons */
    if( items[i].accelerator
       && items[i].wclass != &xmCascadeButtonWidgetClass
       && items[i].wclass != &xmCascadeButtonGadgetClass )
    {
      XmString	str = XmStringCreateSimple( items[i].accel_text );
      XtVaSetValues( widget,
		    XmNaccelerator,	items[i].accelerator,
		    XmNacceleratorText,	str,
		    NULL);
      XmStringFree( str );
    }

    /* set the toggle if required but only one if radio_behavior == True */
    toggle = ((items[i].wclass == &xmToggleButtonWidgetClass) ||
	      (items[i].wclass == &xmToggleButtonGadgetClass));
    if( toggle && items[i].item_set == True ){
      if( !(radio_behavior && one_toggle_set) ){
	XtVaSetValues(widget, XmNset, True, NULL);
	XtVaSetValues(menu, XmNmenuHistory, widget, NULL);
	one_toggle_set = True;
      }
    }

    /* add callbacks */
    if( items[i].callback )
      XtAddCallback( widget, toggle ?
		    XmNvalueChangedCallback : XmNactivateCallback,
		    items[i].callback, items[i].callback_data );

    if( items[i].help_callback )
      XtAddCallback(widget, XmNhelpCallback,
		    items[i].help_callback, items[i].help_callback_data);


  }

  return( menu );
}

Widget HGU_XmBuildRadioMenu(
  Widget	parent,
  int		menu_type,
  char		*menu_name,
  char		menu_mnemonic,
  unsigned char	tear_off_model,
  Boolean	radio_behavior,
  Boolean	always_one,
  MenuItem	*items)
{
    Widget	menu, cascade=NULL, widget;
    int		i, toggle;
    Boolean	one_toggle_set=False;
    Visual	*visual;
    Arg		arg[4];
    

    /* pulldown menus require a cascade button to be made */
    if( menu_type == XmMENU_PULLDOWN ){
	cascade = XtVaCreateManagedWidget(menu_name,
					  xmCascadeButtonWidgetClass, parent,
					  NULL);
	if( menu_mnemonic )
	    XtVaSetValues( cascade, XmNmnemonic, menu_mnemonic, NULL );
    }
    else if( menu_type == XmMENU_OPTION ) {
      cascade = XmCreateOptionMenu( parent, menu_name, NULL, 0 );
    }

    /* get the visual explicitly */
    visual = HGU_XmWidgetToVisual(parent);
    XtSetArg(arg[0], XmNvisual, visual);
    XtSetArg(arg[1], XmNtearOffModel, tear_off_model);
    XtSetArg(arg[2], XmNradioBehavior, radio_behavior);
    XtSetArg(arg[3], XmNradioAlwaysOne, always_one);

    /* create the menu */
    if( menu_type == XmMENU_PULLDOWN || menu_type == XmMENU_OPTION )
      menu = XmCreatePulldownMenu( cascade, "_pulldown", arg, 4 );
    else if( menu_type == XmMENU_POPUP ){
	String	name_buf = (String) calloc(strlen(menu_name)+16, sizeof(char));
	(void) sprintf(name_buf, "%s_popup", menu_name);
	menu = XmCreatePopupMenu( parent, name_buf, arg, 4 );
	free(name_buf);
    } else {
	XtWarning("Invalid menu type passed to BuildMenu()");
	return( NULL );
    }

    if( cascade != NULL )
	XtVaSetValues(cascade, XmNsubMenuId, menu, NULL);

    /* now the menu items */
    for( i=0; items[i].name != NULL; i++ ){

	/* if subitems then build the new menu, this will return
	   a cascade button as required. */
	if( items[i].subitems ){
	    if( menu_type == XmMENU_OPTION ){
		XtWarning("You can't have submenus from option menu items");
		continue;
	    } else
		widget =
		    HGU_XmBuildRadioMenu(menu, XmMENU_PULLDOWN,
					 items[i].name, items[i].mnemonic,
					 items[i].tear_off_model,
					 items[i].radio_behavior,
					 items[i].always_one,
					 items[i].subitems);
	} else
	    widget = XtVaCreateManagedWidget(items[i].name, *items[i].wclass,
					     menu, NULL);

	/* if item or cascade button, may have a mnemonic */
	if( items[i].mnemonic )
	    XtVaSetValues( widget, XmNmnemonic, items[i].mnemonic, NULL );

	/* any item can have accelerators except cascade buttons */
	if( items[i].accelerator
	   && items[i].wclass != &xmCascadeButtonWidgetClass
	   && items[i].wclass != &xmCascadeButtonGadgetClass )
	{
	    XmString	str = XmStringCreateSimple( items[i].accel_text );
	    XtVaSetValues( widget,
			  XmNaccelerator,	items[i].accelerator,
			  XmNacceleratorText,	str,
			  NULL);
	    XmStringFree( str );
	}

	/* set the toggle if required but only one if radio_behavior == True */
	toggle = ((items[i].wclass == &xmToggleButtonWidgetClass) ||
		 (items[i].wclass == &xmToggleButtonGadgetClass));
	if( toggle && items[i].item_set == True ){
	    if( !(radio_behavior && one_toggle_set) ){
		XtVaSetValues(widget, XmNset, True, NULL);
		XtVaSetValues(menu, XmNmenuHistory, widget, NULL);
		one_toggle_set = True;
	    }
	}

	/* add callbacks */
	if( items[i].callback )
	    XtAddCallback( widget, toggle ?
			  XmNvalueChangedCallback : XmNactivateCallback,
			  items[i].callback, items[i].callback_data );

	if( items[i].help_callback )
	    XtAddCallback(widget, XmNhelpCallback,
			  items[i].help_callback, items[i].help_callback_data);


    }

    /* for popup menus, just return the menu; pulldown menus, return
       the cascade button; option menus, return the widget returned from
       XmCreateOptionMenu(). This is really a RowColumn widget. */

    return( (menu_type == XmMENU_POPUP) ? menu : cascade );
}
