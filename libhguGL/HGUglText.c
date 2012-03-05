#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _libhguGL/HGUglText_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         HGUglText.c
* \author       Bill Hill
* \date         Wed Apr 29 11:14:56 2009
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
* \ingroup      HGU_GL
* \brief        Functions for handling text within the MRC HGU OpenGL
*		library.
*/

#include <stdlib.h>
#include <math.h>
#include <Xm/XmAll.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <Wlz.h>
#include <HGUgl.h>

/*!
* \ingroup	HGU_GL
* \brief	Creates an HGUgl text font instance using the given X11
* 		font.
* \param	xFont			Given X11 font.
*/
HGUglTextFont	*HGUglTextFontCreate(Font xFont)
{
  HGUglTextFont	*newFnt;

  newFnt = (HGUglTextFont *)malloc(sizeof(HGUglTextFont));
  if(newFnt)
  {
    newFnt->xFont = xFont;
    newFnt->bitmapBase = glGenLists(256);
    glXUseXFont(xFont, 0, 256, newFnt->bitmapBase);
  }
  return(newFnt);
}

/*!
* \ingroup	HGU_GL
* \brief	Destroys the given HGUgl text font instance.
* \param	fnt			Given HGUgl text font instance.
*/
void		HGUglTextFontDestroy(HGUglTextFont *fnt)
{
  if(fnt)
  {
    glDeleteLists(fnt->bitmapBase, 256);
    free(fnt);
  }
}

/*!
* \ingroup	HGU_GL
* \brief	Draws the pixmap for the given character using the
* 		given HGUgl text font instance.
* \param	fnt			Given HGUgl text font instance.
* \param	charCode		Given character.
*/
void		HGUglTextFontChar(HGUglTextFont *fnt, char charCode)
{
  if(fnt)
  {
    glCallList(fnt->bitmapBase + (unsigned int)charCode);
  }
}

/*!
* \ingroup	HGU_GL
* \brief	Draws the pixmaps for the given character string using
* 		the given HGUgl text font instance.
* \param	fnt			Given HGUgl text font instance.
* \param	str			Given character string.
*/
void		HGUglTextFontStr(HGUglTextFont *fnt, char *str)
{
  if(fnt && str)
  {
    while(*str)
    {
      glCallList(fnt->bitmapBase + (unsigned int)*str);
      ++str;
    }
  }
}
