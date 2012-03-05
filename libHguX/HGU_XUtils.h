#ifndef HGU_XUTILS_H
#define HGU_XUTILS_H
#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _HGU_XUtils_h[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         HGU_XUtils.h
* \author       Richard Baldock
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
* \ingroup      HGU_X
* \brief        
*/

#include <sys/types.h>

#include <X11/Xlib.h>
#include <Wlz.h>

#ifdef  __cplusplus
extern "C" {
#endif

#define HGU_DISPLAY_SELCTN      "HGU_display_selection"

/* define Error callbacks typedefs and structures */
typedef int (*HGU_XErrorFunc)(Display *dpy, Window win,
			      caddr_t client_data, caddr_t call_data);

typedef int (*GTransTransfFunc)(int val, caddr_t gtrans, int invert);
typedef int (*WlzGTransTransfFunc)(int val, caddr_t gtrans, int invert);

typedef enum 
{
    NULL_GTRANS_TYPE,
    BASIC_GTRANS_TYPE
} GTransType, WlzGTransType;

typedef struct
{
    WlzGTransType	type;
    WlzGTransTransfFunc	transf_func;
    int			gmin, gmax;
    int			Gmin, Gmax;
} BasicGTrans, WlzBasicGTrans;

typedef union _GTrans
{
    WlzGTransType	type;
    WlzBasicGTrans	basic;
} GTrans, WlzGTrans;

typedef struct {
    char	*srcstr;
    char	*errstr;
    int		errnum;
} HGU_XErrorStruct;

/* function declarations */
extern int	HGU_XError	(Display	*dpy,
				 Window		win,
				 char		*srcstr,
				 char		*errstr,
				 int		errnum);
extern int	HGU_XErrorSetFunction	(HGU_XErrorFunc err_func,
					 caddr_t	err_data);
extern int	HGU_XErrorGetFunction	(HGU_XErrorFunc *err_func,
					 caddr_t	*err_data);
extern GC	HGU_XCreateGC	(Display	*dpy,
				 Window		win);
extern GC	HGU_XColourGC	(Display	*dpy,
				 GC		gc,
				 int		colour);
extern GC	HGU_XIntensGC	(Display	*dpy,
				 GC		gc,
				 int		intensity);
extern GC	HGU_XLwidthGC	(Display	*dpy,
				 GC		gc,
				 int		width);
extern GC	HGU_XColourFromNameGC	(Display	*dpy,
					 Colormap	cmap,
					 GC	       	gc,
					 char		*colstr);

extern GTrans	*makebasicgtrans	(int	gmin,
					 int	gmax,
					 int	Gmin,
					 int	Gmax);
extern GTrans	*makedefaultgtrans	();

extern int	HGU_XSetLut		(Display	*dpy,
					 Window		win,
					 XColor		*colors,
					 int		ncols);
extern int	HGU_XGtransSetLut	(Display	*dpy,
					 Window		win,
					 unsigned char	colormap[3][256],
					 GTrans		*gtrans);
extern int	HGU_XGtransSetGreys	(Display	*dpy,
					 Window		win,
					 unsigned char	colormap[3][256],
					 unsigned long	planes,
					 GTrans		*gtrans);
extern int	HGU_XGtransSetOverlay	(Display	*dpy,
					 Window		win,
					 unsigned long	plane,
					 int		r,
					 int		g,
					 int		b,
					 GTrans		*gtrans);
extern int	HGU_XGtransSetDefaultLut(Display	*dpy,
					 Window		win,
					 GTrans		*gtrans);
extern int 	HGU_XAllocLut		(Display     	*dpy,
					 Window      	win);
extern int 	HGU_XStoreLut		(Display     	*dpy,
					 Window      	win,
					 XColor      	*cols,
					 int		ncols);
extern int	HGU_XFreeLut		(Display	*dpy,
					 Window		win);
extern int	HGU_XGtransToXCols	(Display       	*dpy,
					 Window		win,
					 unsigned char	colormap[3][256],
					 unsigned long	planes,
					 GTrans		*gtrans,
					 XColor		*cols,	/* Return */
					 int	       	*ncols);/* Return */

extern int	clearX		(Display	*dpy,
				 Window		win,
				 GC		gc);
extern int	set_backingX	(Display	*dpy,	
				 Window		win);
extern int	dispcursorX	(Display	*dpy,
				 Window		win,
				 GC		gc,
				 int		x,
				 int		y,
				 int		scale,
				 int		style);

extern int	QueryInWindow	(Display	*dpy,
				 Window		win);
extern int 	QueryRootPosition(Display	*dpy,
				  Window       	win,
				  int		*x,
				  int		*y);

extern Window	get_HGU_display_selectionX(Display	*dpy);
extern Visual   *HGU_XVisualMatch(int 		*visualClassRtn,
				int		*visualDepthRtn,
				int		visualClass,
				int		visualDepth,
				Display		*display,
				int		screenNum);

extern char *HGU_XRequestToString(
  char	request_code); 

#ifdef  __cplusplus
}
#endif

#endif  /* HGU_XUTILS_H */
/* don't put anything after this line */
