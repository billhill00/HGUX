#ifndef HGU_XMUTILS_H
#define HGU_XMUTILS_H

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
* File        :    HGU_XmUtils.h					     *
* $Revision$
******************************************************************************
* Author Name :     Richard Baldock					     *
* Author Login:    richard@hgu.mrc.ac.uk				     *
* Date        :    Thu Aug 25 10:02:02 1994				     *
* Synopsis:	Procedure definitions for the HGU Motif Utilities	     *
*		library.						     *
*****************************************************************************/

#include <stdio.h>

#include <X11/Intrinsic.h>
#include <Xm/BulletinB.h>

#include <HGU_XmStrings.h>
#include <Wlz.h>

#ifdef  __cplusplus
extern "C" {
#endif

/* structure for action_area convenience routine - copied from Heller */
typedef struct _ActionAreaItem {
    char		*name;
    XtCallbackProc	callback;
    XtPointer		client_data;
} ActionAreaItem;

/* structure for BuildMenu() convenience routine - copied from Heller */
typedef struct _MenuItem {
    String		name;
    WidgetClass		*wclass;
    char		mnemonic;
    String		accelerator;
    String		accel_text;
    Boolean		item_set;
    XtCallbackProc	callback;
    XtPointer		callback_data;
    XtCallbackProc	help_callback;
    XtPointer		help_callback_data;
    unsigned char	tear_off_model;
    Boolean		radio_behavior;
    Boolean		always_one;
    struct _MenuItem	*subitems;
} MenuItem;

extern Widget HGU_XmBuildMenu(Widget		parent,
			      int		menu_type,
			      char		*menu_name,
			      char		menu_mnemonic,
			      unsigned char	tear_off_model,
			      MenuItem		*items);

extern Widget HGU_XmBuildPulldownMenu(Widget	menu_parent,
				      unsigned char	tear_off_model,
				      Boolean	radio_behavior,
				      Boolean	always_one,
				      MenuItem	*items);

extern Widget HGU_XmBuildRadioMenu(Widget		parent,
				   int			menu_type,
				   char			*menu_name,
				   char			menu_mnemonic,
				   unsigned char	menu_tear_off,
				   Boolean		radio_behavior,
				   Boolean		always_one,
				   MenuItem		*items);

extern Widget HGU_XmCreateActionArea(Widget		parent,
				     ActionAreaItem	*actions,
				     int		num_actions);

extern Widget HGU_XmCreateWActionArea(Widget		parent,
				      ActionAreaItem	*actions,
				      int		num_actions,
				      WidgetClass	widgetClass);

extern Widget HGU_XmCreateStdDialog(Widget		parent,
				    String		name,
				    WidgetClass	controlWidgetClass,
				    ActionAreaItem	*actions,
				    int		num_actions);

extern int HGU_XmDialogConfirm( Widget		dialog,
			       Widget		confirm,
			       Widget		cancel,
			       unsigned char	modality);

#ifndef _HGU_XmVarArgs_c /* [ */
extern Widget HGU_XmVaCreateActionArea(Widget 	parentW,
				...);

extern Widget HGU_XmVaCreateStdDialog(Widget	parentW,
				String		name,
				WidgetClass	controlWidgetClass,
				...);
#endif /* ! _HGU_XmVarArgs_c ] */


/* miscellaneous callback functions */
extern void Quit(Widget		w,
		 XtPointer	client_data,
		 XtPointer	call_data);

extern void PopupCallback(Widget		w,
			  XtPointer     client_data,
			  XtPointer	call_data);

extern void PopdownCallback(Widget		w,
			    XtPointer   client_data,
			    XtPointer	call_data);
extern void DestroyWidgetCallback(Widget		w,
				  XtPointer     client_data,
				  XtPointer	call_data);

extern void SetSensitiveCallback(Widget		w,
				 XtPointer      client_data,
				 XtPointer	call_data);

extern void FSBPopupCallback(Widget		w,
			     XtPointer     client_data,
			     XtPointer	call_data);

extern void UninstallTranslationsCallback( Widget		w,
					  XtPointer    client_data,
					  XtPointer	call_data);

extern int HGU_XmUserConfirm(Widget	w,
			     String	question,
			     String     ans1,
			     String     ans2,
			     int	default_ans);
extern String HGU_XmUserGetstr(  Widget	w,
			       String	question, 
			       String ans1, 
			       String ans2, 
			       String default_ans);
extern String HGU_XmUserGetFilename(Widget	w,
				    String	question,
				    String	ans1,
				    String	ans2,
				    String	default_ans,
				    String	dir_str,
				    String	pattern_str);

extern void HGU_XmUserInfo(Widget		w,
			   String		str,
			   unsigned char	modality);

extern void HGU_XmUserMessage(Widget		w,
			      String		str,
			      unsigned char	modality);

extern void HGU_XmUserError(Widget		w,
			    String		str,
			    unsigned char	modality);

extern int HGU_XmUserWarning(Widget	w,
			     String	warning,
			     int	default_ans);

/* extern Widget		HGU_XmVaCreatePB_Radio(); */

/* HGU_XmSlider functions */
typedef float (*HGU_XmSliderFunc)(float val, int invert);

extern Widget HGU_XmCreateHorizontalSlider(String	name,
					   Widget	parent,
					   float	fvalue, 
					   float        fminval, 
					   float        fmaxval,
					   int		decimalPoints,
					   XtCallbackProc	cb_func,
					   XtPointer	cb_data);

extern Widget HGU_XmCreateHorizontalLogSlider(String		name,
					      Widget		parent,
					      float		fvalue, 
					      float    fminval,
					      float    fmaxval,
					      int		decimalPoints,
					      XtCallbackProc	cb_func,
					      XtPointer	cb_data);
extern Widget HGU_XmCreateHorizontalExpSlider(String		name,
					      Widget		parent,
					      float		fvalue, 
					      float fminval, 
					      float fmaxval,
					      int		decimalPoints,
					      XtCallbackProc	cb_func,
					      XtPointer	cb_data);

extern Widget HGU_XmCreateHorizontalFuncSlider(String		name,
					       Widget		parent,
					       float		fvalue, 
					       float fminval, 
					       float fmaxval,
					       int		decimalPoints,
					       XtCallbackProc	cb_func,
					       XtPointer	cb_data,
					       HGU_XmSliderFunc	func);

extern void HGU_XmSetSliderValue(Widget	slider,
				 float	value);

extern float HGU_XmGetSliderValue(Widget	slider);

extern void HGU_XmSetSliderRange(Widget	slider,
				 float	minval, 
				 float maxval);
extern void HGU_XmGetSliderRange(Widget	slider,
				 float	*minval, 
				 float *maxval);

extern void GE_SliderSetProc(Widget	slider,
			     String	value);

/* HGU_XmTextLine functions */
extern Widget HGU_XmCreateTextLine(String		name,
				   Widget		parent,
				   String		value,
				   XtCallbackProc	cb_func,
				   XtPointer	cb_data);
extern void HGU_XmSetTextLineValue(Widget	text_line,
				   String	value);

extern String HGU_XmGetTextLineValue(Widget	text_line);

extern void GE_TextLineSetProc(Widget	text_line,
			       String	value);

extern String GE_Text_LineGetProc(Widget	text_line);

/* HGU_XmPB_Radio functions */
extern Widget HGU_XmCreatePB_Radio(String		name,
				   Widget		parent,
				   MenuItem	*items,
				   int		def_item);

/* HGU_XmPB_Radio functions */
Widget HGU_XmCreatePB_Row(String		name,
			  Widget		parent,
			  MenuItem	*items);

/* HGU_XmGraph functions, string definitions and types */
extern Widget HGU_XmCreateGraph(String		name,
				Widget		parent,
				XtPointer	data);

extern WlzPolygonDomain *HGU_XmGetGraphPolyline(Widget  w,
						    int     n);

extern int HGU_XmSetGraphPolyline(Widget  w,
				  WlzPolygonDomain *poly,
				  int     n);
extern int HGU_XmSetGraphLimits(Widget  w,
				float   xl, 
				float xu, 
				float yl, 
				float yu);

typedef enum {
    HGU_XmGRAPH,
    HGU_XmHISTOGRAM
} HGU_XmGraphStyleType;

typedef enum {
    HGU_XmNONE,
    HGU_XmDASHES,
    HGU_XmCOLOURS,
    HGU_XmMARKERS
} HGU_XmGraphMarkerType;

/* HGU_XmSaveRestore structures and functions */

typedef void (*SaveRestoreProc)(Widget		w,
				XtPointer	data,
				FILE 		*fp);

extern void SaveStateCallback(Widget		w,
			      XtPointer		client_data,
			      XtPointer		call_data);
extern void RestoreStateCallback(Widget		w,
				 XtPointer	client_data,
				 XtPointer	call_data);
extern void HGU_XmSaveRestoreInit(Widget	topl,
				  int	*argc,
				  char	**argv);
extern void HGU_XmSaveRestoreHelpCallback(Widget		topl,
					  XtCallbackProc	help_cb,
					  XtPointer	help_data);
extern void HGU_XmSaveRestoreAddWidget(Widget		w,
				       SaveRestoreProc	save_func,
				       XtPointer	save_data,
				       SaveRestoreProc	restore_func,
				       XtPointer	restore_data);
extern void HGU_XmSaveRestoreRemoveWidget(Widget		w);
extern void HGU_XmSaveRestoreChangeDefault( SaveRestoreProc	func,
					   XtPointer	data);
extern void HGU_XmFSD_SaveFunc(Widget		w,
			       XtPointer	data,
			       FILE		*fp);

/* HGU_XmFileProcs functions */
extern String HGU_XmGetFileStr( Widget		w,
			       XmString	filestr,
			       XmString	dirstr);
extern FILE *HGU_XmGetFilePointer( Widget		w,
				  XmString	filestr,
				  XmString	dirstr,
				  char		*type);

extern FILE *HGU_XmGetFilePointerBck( Widget	w,
				      XmString	filestr,
				      XmString	dirstr,
				      char	*type,
				      char	*extension);

/* HGU_XmMiscProcs functions */
extern Boolean HGU_XmIsViewable(Widget	w);
extern int HGU_XmGetShellOffsets(Widget		w,
				 Position	*x_off,
				 Position	*y_off);

/* the help menu routines */
extern  MenuItem		*HGU_XmHelpMenuItems;
extern  MenuItem		HGU_XmHelpMenuItemsP[];

extern void	HGU_XmHelpMenuInit(Widget	topl);
extern void	HGU_XmHelpOnProgramCb(Widget	w,
				      XtPointer	client_data,
				      XtPointer call_data);
extern void	HGU_XmHelpOnContextCb(Widget	w,
				      XtPointer	client_data,
				      XtPointer call_data);
extern void	HGU_XmHelpOnWindowsCb(Widget	w,
				      XtPointer	client_data,
				      XtPointer call_data);
extern void	HGU_XmHelpOnKeysCb(Widget	w,
				   XtPointer	client_data,
				   XtPointer 	call_data);
extern void	HGU_XmHelpIndexCb(Widget	w,
				  XtPointer	client_data,
				  XtPointer 	call_data);
extern void	HGU_XmHelpOnHelpCb(Widget	w,
				   XtPointer	client_data,
				   XtPointer 	call_data);
extern void	HGU_XmHelpTutorialCb(Widget	w,
				     XtPointer	client_data,
				     XtPointer 	call_data);
extern void	HGU_XmHelpVersionCb(Widget	w,
				    XtPointer	client_data,
				    XtPointer 	call_data);
extern void	HGU_XmHelpDestroyCb(Widget	w,
				    XtPointer	client_data,
				    XtPointer 	call_data);
extern void	HGU_XmHelpStandardCb(Widget	w,
				     XtPointer	client_data,
				     XtPointer 	call_data);
extern void	HGU_XmHelpStandardStringCb(Widget	w,
					   XtPointer	client_data,
					   XtPointer 	call_data);
extern int	HGU_XmHelpCreateViewer(Widget	w,
				       char	*url);
extern void     HGU_XmHelpDestroyViewer(void);
extern void	HGU_XmHelpShowUrl(Widget	w,
				  char	*url);
extern void	HGU_XmHelpShowString(Widget	w,
				     char	*title,
				     char 	*str);

/* the macros menu routines */
extern  MenuItem		*HGU_XmMacroMenuItems;
extern  MenuItem		HGU_XmMacroMenuItemsP[];

extern void	HGU_XmMacroCb(Widget	w,
			      XtPointer	client_data,
			      XtPointer	call_data);

extern void	HGU_XmStartRecordMacro(XtAppContext	app_con);

extern void	HGU_XmEndRecordMacro();
extern void	HGU_XmReplayMacro();

extern int	HGU_XmMacroReplayLoopFlag;

/* utility functions for non-default visuals */
extern Visual *HGU_XmWidgetToVisual(Widget w);

extern Visual *HGU_XGetVisual(
  Display     *dpy,
  int         screen,
  int         class,
  unsigned int        depth);

/* HGU_XmSetInt.c structures and functions */

typedef struct {
  int		val;
  int		maxval;
  int		minval;
  Widget	text;
  XtCallbackProc	cb_func;
  XtPointer		cb_data;
} HGU_XmSetIntControlStruct;

extern Widget HGU_XmCreateSetIntControl(
  String		name,
  Widget		parent,
  int			val,
  int			minval,
  int			maxval,
  XtCallbackProc	cb_func,
  XtPointer		cb_data);

extern int HGU_XmGetSetIntValue(
  Widget	w);

extern int HGU_XmSetSetIntValue(
  Widget	w,
  int		val);

#ifdef  __cplusplus
}
#endif

/* do not add anything after this line */
#endif /* HGU_XMUTILS_H */
