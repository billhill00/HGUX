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
* File        :    HGU_XLut.c						     *
* $Revision$
******************************************************************************
* Author Name :    Richard Baldock					     *
* Author Login:    richard@hgu.mrc.ac.uk				     *
* Date        :    Wed Sep 14 18:52:48 1994				     *
* Synopsis    :    Convenience routines to manipulate look-up tables	     *
*		   using the grey-transform associated with a PframeX	     *
*		   structure.						     *
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include <X11/Xlib.h>

#include <HGU_XUtils.h>

static int	       	ncells;
static unsigned long	pix_vals[256];
static int	       	LUT_allocated = 0;

/* --------------------- HGU_XAllocLut ----------------------------
 *
 *	Allocates but does not set colour cells.
 *	Sets local globals pix_vals[] and ncells.
 * ---------------------------------------------------------------- */

int HGU_XAllocLut (Display     	*dpy,
		   Window      	win)
{
  unsigned long		plane_masks[1];
  XWindowAttributes	win_att;
  XColor		new_cols[256];
  int			i, j, k;

  if (LUT_allocated == 1)
    return (0);			/* is this an error ?? */

  /* set up the window attributes  */
  if (XGetWindowAttributes (dpy, win, &win_att) == 0 )
    {
      fprintf (stderr, "HGU_XAllocLut: failed to get window attributes\n");
      exit (1);
    }

  /* check for monochrome */
  if (win_att.depth == 1)
    return (0);

  /* if the colormap seems funny, put in the default */
  if (!win_att.colormap)
    win_att.colormap = DefaultColormap (dpy, DefaultScreen (dpy));

  /* get ownership of all remaining colorcells */
  ncells = 0;
  while (XAllocColorCells (dpy, win_att.colormap,
			   False, plane_masks, 0, &pix_vals[ncells], 1))
    {
      ncells++;
      if (ncells > 255)
	break;
    }

  LUT_allocated = 1;

  return (0);
}

/* ---------------------- HGU_XStoreLut -------------------------------
 *
 *	Stores given colour map in previously allocated colour cells
 *	if possible.
 * -------------------------------------------------------------------- */

int HGU_XStoreLut (Display     	*dpy,
		   Window      	win,
		   XColor      	*cols,
		   int		ncols)
{
    XWindowAttributes	win_att;
    XColor		new_cols[256];
    int			i, j, k;

  if (LUT_allocated == 0)
    {
      fprintf (stderr, "HGU_XStoreLut: No LUT allocated\n");
      return (1);
    }

  /* set up the window attributes  */
  if (XGetWindowAttributes (dpy, win, &win_att) == 0 )
    {
      fprintf (stderr, "HGU_XStoreLut: failed to get window attributes\n");
      exit (1);
    }

    /* discard colors we can't set */
    for (i = 0, k = 0; i < ncols; i++)
      {
	for (j = 0; j < ncells; j++)
	  {
	    if (cols[i].pixel == pix_vals[j])
	      {
		new_cols[k] = cols[i];
		k++;
		break;
	      }
	  }
      }

    /* set the colours */
    if (k > 0)
	XStoreColors (dpy, win_att.colormap, new_cols, k);

    XFlush (dpy);

    return (0);
}

/* --------------- HGU_XFreeLut -----------------------------------
 *
 *	Frees previously allocated colour cells.
 * ---------------------------------------------------------------- */

int	HGU_XFreeLut (Display      *dpy,
		      Window       win)
{
  XWindowAttributes	win_att;

  if (LUT_allocated == 0)
    {
      fprintf (stderr, "HGU_XFreeLut: No LUT allocated\n");
      return (1);
    }

  /* set up the window attributes  */
  if (XGetWindowAttributes (dpy, win, &win_att) == 0 )
    {
      fprintf (stderr, "HGU_XFreeLut: failed to get window attributes\n");
      exit (1);
    }

  /* free the colorcells */
      if (ncells > 0)
     	XFreeColors (dpy, win_att.colormap, pix_vals, ncells, (long) 0);
     
    XFlush (dpy);

    LUT_allocated = 0;

    return (0);
}

/* --------------- HGU_XSetLut -----------------------------------
 *
 *	Allocates, stores and frees look up table.
 * ---------------------------------------------------------------- */

int	HGU_XSetLut (Display     	*dpy,
		     Window      	win,
		     XColor      	*cols,
		     int		ncols)
{
  if ((HGU_XAllocLut (dpy, win) == 0)
    && (HGU_XStoreLut (dpy, win, cols, ncols) == 0)
      && (HGU_XFreeLut (dpy, win) == 0))
    {
      return (0);
    }
  else
    {
      return (1);
    }
}

/* --------------- HGU_XGtransSetLut -------------------------------- */

int HGU_XGtransSetLut(
Display		*dpy,
Window		win,
unsigned char	colormap[3][256],
GTrans		*gtrans)
{
    unsigned long	planes=0;

    return( HGU_XGtransSetGreys( dpy, win, colormap, planes, gtrans ) );
}

/* -------------- HGU_XGtransToXCols ---------------------------------
 *
 *	Transforms colour map into Lut.
 *	Pointer to return Lut and number of colours given as parameters.
 * -------------------------------------------------------------------- */

int	HGU_XGtransToXCols (Display		*dpy,
			    Window		win,
			    unsigned char	colormap[3][256],
			    unsigned long	planes,
			    GTrans		*gtrans,
			    XColor		*cols,	/* Return */
			    int			*ncols)	/* Return */
{
    int			g, G;
    XWindowAttributes	win_att;

    /* set up the window attributes  */
    if (XGetWindowAttributes (dpy, win, &win_att) == 0)
      {
	fprintf (stderr, 
		 "HGU_XGtransToXCols: failed to get window attributes\n");
	exit (1);
      }

    for (G = gtrans->basic.Gmin, *ncols = 0; G <= gtrans->basic.Gmax; G++)
      {
	g = (*gtrans->basic.transf_func)( G, (caddr_t) gtrans, 1 );
	g = (g > 255) ? 255 : g;
	g = (g < 0) ? 0 : g;

	if (g & planes)
	    continue;

	switch (win_att.depth)
	  {
	  case 1:
	    return (1);
	  case 4:
	    cols->pixel = G / 16;		/* cols[ncols].pixel */
	    break;
	  case 24:
	    cols->pixel = G + (G << 8) + (G << 16);
	    break;
	  case 8:
	  default:
	    cols->pixel = G;
	    break;
	  }

	cols->flags = DoRed | DoGreen | DoBlue;
	cols->red   = (short) colormap[0][g] * 256;
	cols->green = (short) colormap[1][g] * 256;
	cols->blue  = (short) colormap[2][g] * 256;

	(*ncols)++;
	cols++;

	if (*ncols >= 256)
	    break;
    }

    return (0);
}

/* -------------- HGU_XGtransSetGreys ---------------------------------
 *
 *	Genetates Lut and then sets it
 * -------------------------------------------------------------------- */

int	HGU_XGtransSetGreys (Display		*dpy,
			     Window		win,
			     unsigned char	colormap[3][256],
			     unsigned long	planes,
			     GTrans		*gtrans)
{
  XColor	cols[256];
  int		ncols;

  if ((HGU_XGtransToXCols (dpy, win, colormap, planes, gtrans,
			   cols, &ncols) == 0)
      && (HGU_XSetLut (dpy, win, cols, ncols) == 0))
    {
      return (0);
    }
  else
    {
      return (1);
    }
}

/* -------------- HGU_XGtransSetOverlay ------------------------------ */

int HGU_XGtransSetOverlay(
Display		*dpy,
Window		win,
unsigned long	plane,
int		r,
int		g,
int		b,
GTrans		*gtrans)
{
    XColor		cols[256];
    int			G, ncols;

    for(G = gtrans->basic.Gmin, ncols=0; G <= gtrans->basic.Gmax; G++){

	if( !(G & plane) )
	    continue;

	cols[ncols].pixel = G;
	cols[ncols].flags = DoRed | DoGreen | DoBlue;
	cols[ncols].red   = (short) r * 256;
	cols[ncols].green = (short) g * 256;
	cols[ncols].blue  = (short) b * 256;

	ncols++;
	if( ncols >= 256 )
	    break;
    }

    HGU_XSetLut( dpy, win, cols, ncols );
    return( 0 );
}

int HGU_XGtransSetDefaultLut(
Display		*dpy,
Window		win,
GTrans		*gtrans)
{
    unsigned char	colormap[3][256];
    int			i;

    for(i=0; i < 256; i++){
	colormap[0][i] = 255 - i;
	colormap[1][i] = 255 - i;
	colormap[2][i] = 255 - i;
    }

    HGU_XGtransSetLut( dpy, win, colormap, gtrans );
    return( 0 );
}
