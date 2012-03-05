#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _HGU_XmMacroMenu_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         HGU_XmMacroMenu.c
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
* \brief        
*/

#include <stdio.h>
#include <stdlib.h>

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <Xm/PushBG.h>

#include <HGU_XmUtils.h>

int	HGU_XmMacroReplayLoopFlag;
void 	HGU_XmEndRecordMacro(void);
void 	HGU_XmReplayMacro(void);

/* menu item structures */

MenuItem HGU_XmMacroMenuItemsP[] = {		/* macro_menu items */
{"start_record", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
     HGU_XmMacroCb, (XtPointer) ((unsigned long) 0), NULL, NULL,
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"end_record", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
     HGU_XmMacroCb, (XtPointer) ((unsigned long) 1), NULL, NULL,
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"replay", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
     HGU_XmMacroCb, (XtPointer) ((unsigned long) 2), NULL, NULL,
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"loop", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
     HGU_XmMacroCb, (XtPointer) ((unsigned long) 3), NULL, NULL,
     XmTEAR_OFF_DISABLED, False, False, NULL},
{"quit_loop", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
     HGU_XmMacroCb, (XtPointer) ((unsigned long) 4), NULL, NULL,
     XmTEAR_OFF_DISABLED, False, False, NULL},
{NULL},
};

MenuItem	*HGU_XmMacroMenuItems = &(HGU_XmMacroMenuItemsP[0]);

/* action and callback procedures */
void HGU_XmMacroCb(
Widget 		w,
XtPointer 	client_data,
XtPointer 	call_data)
{
    unsigned long	item_no = (unsigned long) client_data;
    XtAppContext	app_con = XtWidgetToApplicationContext( w );

    if( call_data ){/* to satisfy IRIX646 compiler */}

    switch( item_no ){
    case 0:             /* Start record */
	XtSetSensitive(XtNameToWidget(XtParent(w), "*replay"), False);
	XtSetSensitive(XtNameToWidget(XtParent(w), "*loop"), False);
	XtSetSensitive(XtNameToWidget(XtParent(w), "*quit_loop"), False);
        HGU_XmStartRecordMacro( app_con );
        return;
    case 1:             /* End record */
        HGU_XmEndRecordMacro( );
	XtSetSensitive(XtNameToWidget(XtParent(w), "*replay"), True);
	XtSetSensitive(XtNameToWidget(XtParent(w), "*loop"), True);
	XtSetSensitive(XtNameToWidget(XtParent(w), "*quit_loop"), True);
        return;
    case 2:             /* Replay */
        HGU_XmReplayMacro( );
        return;
    case 3:             /* Continuous replay loop */
	XtSetSensitive(XtNameToWidget(XtParent(w), "*start_record"), False);
	XtSetSensitive(XtNameToWidget(XtParent(w), "*end_record"), False);
	XtSetSensitive(XtNameToWidget(XtParent(w), "*replay"), False);
	XtSetSensitive(w, False);
	HGU_XmMacroReplayLoopFlag = 1;
	while( HGU_XmMacroReplayLoopFlag ){
	    HGU_XmReplayMacro( );
	    while( XtAppPending(app_con) )
		XtAppProcessEvent(app_con, XtIMAll);
	}
	XtSetSensitive(XtNameToWidget(XtParent(w), "*start_record"), True);
	XtSetSensitive(XtNameToWidget(XtParent(w), "*end_record"), True);
	XtSetSensitive(XtNameToWidget(XtParent(w), "*replay"), True);
	XtSetSensitive(w, True);
        return;
    case 4:		/* stop the continuous loop */
	HGU_XmMacroReplayLoopFlag = 0;
	return;
    default:
        return;
    }
}

/* macro procedures */

typedef struct {
    Widget	w;
    String	action_name;
    XEvent	event;
    String	*params;
    Cardinal	num_params;
} MacroRecord;

static XtActionHookId	hook_id;
static MacroRecord	*macrorecords=NULL;
static int		num_records;
static int		max_numrecords;
static int		recording_macro=0;

static void delete_widget_from_macro(Widget 	w,
				     XtPointer	client_data,
				     XtPointer	call_data)
{
    int		i;

    for(i=0; i < num_records; i++){
	if( macrorecords[i].w == w ){
	    if( (num_records - i) > 1 )
		memcpy(&(macrorecords[i]), &(macrorecords[i+1]),
		       sizeof(MacroRecord) * (num_records - i - 1));
	    num_records--;
	    break;
	}
    }

    if( i < num_records )
	delete_widget_from_macro(w, client_data, call_data);

    return;
}

static void macro_hook_proc(Widget	w,
			    XtPointer	client_data,
			    String	action_name, 
			    XEvent	*event,
			    String	*params,
			    Cardinal	*num_params)
{
    if( client_data ){/* to satisfy IRIX646 compiler */}

    if( num_records >= max_numrecords ){
	max_numrecords += 512;
	macrorecords = (MacroRecord *) 
	    realloc(macrorecords, sizeof(MacroRecord) * max_numrecords);
    }

    macrorecords[num_records].w 	  = w;
    macrorecords[num_records].action_name = action_name;
    macrorecords[num_records].event 	  = *event;
    macrorecords[num_records].params 	  = params;
    macrorecords[num_records].num_params  = *num_params;
    num_records++;

    /* put in a destroy callback */
    XtAddCallback(w, XtNdestroyCallback, delete_widget_from_macro, NULL);

    return;
}

void HGU_XmStartRecordMacro(XtAppContext	app_con)
{
    if( macrorecords == NULL ){
	macrorecords = (MacroRecord *) malloc(sizeof(MacroRecord) * 512);
	max_numrecords = 512;
    }

    if( recording_macro )
	return;

    recording_macro = 1;
    num_records = 0;
    hook_id = XtAppAddActionHook( app_con, macro_hook_proc, NULL );
    return;
}

void HGU_XmEndRecordMacro(void)
{
    if( !recording_macro )
	return;

    recording_macro = 0;
    XtRemoveActionHook( hook_id );
}

void HGU_XmReplayMacro(void)
{
    int i;

    for(i=0; i < num_records; i++)
	XtCallActionProc(macrorecords[i].w,
			 macrorecords[i].action_name,
			 &(macrorecords[i].event),
			 macrorecords[i].params,
			 macrorecords[i].num_params );
    return;
}





