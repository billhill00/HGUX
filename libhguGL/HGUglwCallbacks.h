#ifndef HGUGL_GLWCALLBACKS_H
#define HGUGL_GLWCALLBACKS_H
#if defined(__GNUC__)
#ident "MRC HGU $Id:"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id:"
#else static char _HGUgl_callb_cks.h[] = "MRC HGU $Id:";
#endif
#endif
/*!
* \file         HGUglwCallbacks.h
* \author       Richard Baldock <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Wed Apr 29 11:09:40 2009
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
* \ingroup      HGU_GL
* \brief        Definitions for the MRC HGU OpenGL library HGUglwCanvas
*		and HGUglwCanvasTb widgets.
*               
*
* Maintenance log with most recent changes at top of list.
*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define HGUglwCCallback           	"Callback"
#define HGUglwNexposeCallback     	"exposeCallback"
#define HGUglwNinitCallback      	"initCallback"
#define HGUglwNresizeCallback     	"resizeCallback"
#define HGUglwNinputCallback      	"inputCallback"
#define HGUglwNtrackballCallback      	"trackballCallback"

#define HGUglw_CR_BASE		((('H'<<8)|'G'<<8)|'U'<<8)
#define HGUglw_CR_EXPOSE    	XmCR_EXPOSE
#define HGUglw_CR_RESIZE    	XmCR_RESIZE
#define HGUglw_CR_INPUT     	XmCR_INPUT
#define HGUglw_CR_INIT  	((HGUglw_CR_BASE)+1)
#define HGUglw_CR_TRACKBALL  	((HGUglw_CR_BASE)+2)

typedef struct
{
  int		reason;
  XEvent	*event;
  Dimension	width;
  Dimension	height;
  GLXContext	glxCtx;
} HGUglwCanvasCallbackStruct;

#ifdef __cplusplus 
}
#endif /* __cplusplus */

#endif /* HGUGL_GLWCALLBACKS_H */
