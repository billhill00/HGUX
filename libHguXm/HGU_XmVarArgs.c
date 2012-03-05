#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _HGU_XmVarArgs_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         HGU_XmVarArgs.c
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
* \brief        Varargs wrappers for various libhguXm functions. These
*		*are* wrappers and *not* re-implementations, this has a
*		small cost in terms of efficiency but should make code
*		maintenance much easier in the long run.	
*/

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

/*!
* \return	Action area composite widget.
* \ingroup	HGU_Xm
* \brief	A varargs wrapper for HGU_XmCreateActionArea() which    *
*               dynamically builds up an argument list and then passes  *
*               it on to HGU_XmCreateActionArea().
* \param	parentW			The composite widget's parent.
* \param	 ...                    A NULL terminated varargs argument
* 					list consisting of argument triples:
* 					  char *name,
* 					  XtCallbackProc callback and
* 					  XtPointer client_data.
*/
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

/*!
* \return	Action area composite widget.
* \ingroup	HGU_Xm
* \brief	A varargs wrapper for HGU_XmCreateStdDialog() which
*		dynamically builds up an argument list and then passes
*		it on to HGU_XmCreateStdDialog().
* \param	parentW			The composite widget's parent.
* \param	name:			The composite widget's name.
* \param	controlWidgetClass: 	Manager widget for the composite widget.
* \param	...			A NULL terminated varargs argument list
* 					consisting of argument triples:
*				          char *name,
*				          XtCallbackProc callback and
*				          XtPointer client_data.
*/
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
