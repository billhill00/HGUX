#pragma ident "MRC HGU $Id$"

#define _HGU_XmVarArgs_c
/************************************************************************
* Project:      libhguXm: A general purpose library of Motif functions	*
*		for use within HGU aplications.				*
* Title:        HGU_XmVarArgs.c		                               	*
* Date:         October 1994	                                    	*
* Author:       Bill Hill 				    		*
* $Revision$
* Purpose:      Varargs wrappers for various libhguXm functions. These	*
*		ARE wrappers and NOT re-implementations, this has a 	*
*		small cost in terms of efficiency but should make code	*
*		maintenance much easier in the long run.		*
* Maintenance:	Log changes below, with most recent at top of list.	*
************************************************************************/

#include <stdlib.h>
#ifdef __STDC__
#include <stdarg.h>
#else /* ! __STDC__ ][ */
#include <varargs.h>
#endif /* __STDC__ ] */
#include <HGU_XmUtils.h>

typedef enum	       /* Used to check for errors within this module (file) */
{
  HGU_XmVA_ER_NONE = 0,
  HGU_XmVA_ER_ARGS = 1,
  HGU_XmVA_ER_MALLOC = 2
} HGU_XmVAError;

#define HGU_XmVA_LIST_INC 64 	/* Increment for dynamically built arg lists */

/************************************************************************
* Function:	HGU_XmVaCreateActionArea				*
* Returns:	Widget:			Action area composite widget.	*
* Purpose:	A varargs wrapper for HGU_XmCreateActionArea() which	*
*		dynamically builds up an argument list and then passes	*
*		it on to HGU_XmCreateActionArea().			*
* Global refs:	-							*
* Parameters:	Widget parentW:		The composite widget's parent.	*
*		...			A NULL terminated varargs	*
*					argument list consisting of 	*
*					argument triples:		*
*					  char *name,			*
*					  XtCallbackProc callback and	*
*					  XtPointer client_data.	*
************************************************************************/
#ifdef __STDC__
Widget		HGU_XmVaCreateActionArea(Widget parentW, ...)
#else /* ! __STDC__ ][ */
Widget		HGU_XmVaCreateActionArea(parentW, va_alist)
Widget		parentW;
#endif /* __STDC__ ] */
{
  va_list	ap;
  int		actIdx,
  		actNum;
  HGU_XmVAError	errFlg = HGU_XmVA_ER_NONE;
  ActionAreaItem *actList = NULL,
  		*actListMember;
  Widget	actAreaW = NULL;

  actNum = HGU_XmVA_LIST_INC;
  if((actList = (ActionAreaItem *)malloc(actNum *
  					 sizeof(ActionAreaItem))) == NULL)
    errFlg = HGU_XmVA_ER_MALLOC;
  if(errFlg == HGU_XmVA_ER_NONE)
  {
    actIdx = 0;
    actListMember = actList + actIdx;
#ifdef __STDC__
    va_start(ap, parentW);
#else /* ! __STDC__ ][ */
    va_start(ap);
#endif /* __STDC__ ] */
    while((errFlg == HGU_XmVA_ER_NONE) &&
          ((actListMember->name = va_arg(ap, char *)) != NULL))
    {
      actListMember->callback = va_arg(ap, XtCallbackProc);
      actListMember->client_data = va_arg(ap, XtPointer);
      if(++actIdx >= actNum)
      {
	actNum += HGU_XmVA_LIST_INC;
	if((actList = (ActionAreaItem *)realloc((void *)actList,
				   actNum * sizeof(ActionAreaItem))) == NULL)
	  errFlg = HGU_XmVA_ER_MALLOC;
      }
      actListMember = actList + actIdx;
    }
    actNum = actIdx;
    va_end(ap);
  }
  if(errFlg == HGU_XmVA_ER_NONE)
    actAreaW = HGU_XmCreateActionArea(parentW, actList, actNum);
  if(actList)
    free(actList);
  return(actAreaW);
}

/************************************************************************
* Function:	HGU_XmVaCreateStdDialog					*
* Returns:	Widget:			Action area composite widget.	*
* Purpose:	A varargs wrapper for HGU_XmCreateStdDialog() which	*
*		dynamically builds up an argument list and then passes	*
*		it on to HGU_XmCreateStdDialog().			*
* Global refs:	-							*
* Parameters:	Widget parentW:		The composite widget's parent.	*
*		String name:		The composite widget's name.	*
*		WidgetClass controlWidgetClass: Manager widget for the	*
*					composite widget.		*
*		...			A NULL terminated varargs	*
*					argument list consisting of 	*
*					argument triples:		*
*					  char *name,			*
*					  XtCallbackProc callback and	*
*					  XtPointer client_data.	*
************************************************************************/
#ifdef __STDC__
Widget		HGU_XmVaCreateStdDialog(Widget parentW, String name,
					WidgetClass controlWidgetClass, ...)
#else /* ! __STDC__ ][ */
Widget		HGU_XmVaCreateStdDialog(parentW, va_alist)
Widget		parentW;
String		name;
WidgetClass	controlWidgetClass;
#endif /* __STDC__ ] */
{
  va_list	ap;
  int		actIdx,
  		actNum;
  HGU_XmVAError	errFlg = HGU_XmVA_ER_NONE;
  ActionAreaItem *actList = NULL,
  		*actListMember;
  Widget	actAreaW = NULL;

  actNum = HGU_XmVA_LIST_INC;
  if((actList = (ActionAreaItem *)malloc(actNum *
  					 sizeof(ActionAreaItem))) == NULL)
    errFlg = HGU_XmVA_ER_MALLOC;
  if(errFlg == HGU_XmVA_ER_NONE)
  {
    actIdx = 0;
    actListMember = actList + actIdx;
#ifdef __STDC__
    va_start(ap, controlWidgetClass);
#else /* ! __STDC__ ][ */
    va_start(ap);
#endif /* __STDC__ ] */
    while((errFlg == HGU_XmVA_ER_NONE) &&
          ((actListMember->name = va_arg(ap, char *)) != NULL))
    {
      actListMember->callback = va_arg(ap, XtCallbackProc);
      actListMember->client_data = va_arg(ap, XtPointer);
      if(++actIdx >= actNum)
      {
	actNum += HGU_XmVA_LIST_INC;
	if((actList = (ActionAreaItem *)realloc((void *)actList,
				   actNum * sizeof(ActionAreaItem))) == NULL)
	  errFlg = HGU_XmVA_ER_MALLOC;
      }
      actListMember = actList + actIdx;
    }
    actNum = actIdx;
    va_end(ap);
  }
  if(errFlg == HGU_XmVA_ER_NONE)
    actAreaW = HGU_XmCreateStdDialog(parentW, name, controlWidgetClass,
    				     actList, actNum);
  if(actList)
    free(actList);
  return(actAreaW);
}
