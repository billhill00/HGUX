#pragma ident "MRC HGU $Id"
/************************************************************************
* Project:	Mouse Atlas
* Title:	HGUglText.c
* Date: 	April 1999
* Author:	Bill Hill
* Copyright:	1999 Medical Research Council, UK.
*		All rights reserved.
* Address:	MRC Human Genetics Unit,
*		Western General Hospital,
*		Edinburgh, EH4 2XU, UK.
* Purpose:	Functions for handling text within the MRC HGU OpenGL
*		library.
* $Revision$
* Maintenance:	Log changes below, with most recent at top of list.
************************************************************************/
#include <stdlib.h>
#include <math.h>
#include <Xm/XmAll.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <Wlz.h>
#include <HGUgl.h>

/************************************************************************
* Function:	HGUglTextFontCreate				
* Returns:	void						
* Purpose:	Creates an HGUgl text font instance using the given X11
*		font.						
* Global refs:	-						
* Parameters:	Font xFont:		Given X11 font.		
************************************************************************/
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

/************************************************************************
* Function:	HGUglTextFontDestroy				
* Returns:	void						
* Purpose:	Destroys the given HGUgl text font instance.	
* Global refs:	-						
* Parameters:	HGUglTextFont *fnt:	Given HGUgl text font instance.
************************************************************************/
void		HGUglTextFontDestroy(HGUglTextFont *fnt)
{
  if(fnt)
  {
    glDeleteLists(fnt->bitmapBase, 256);
    free(fnt);
  }
}

/************************************************************************
* Function:	HGUglTextFontChar				
* Returns:	void						
* Purpose:	Draws the pixmap for the given character using the
*		given HGUgl text font instance.			
* Global refs:	-						
* Parameters:	HGUglTextFont *fnt:	Given HGUgl text font instance.
*		char charCode:		Given character.	
************************************************************************/
void		HGUglTextFontChar(HGUglTextFont *fnt, char charCode)
{
  if(fnt)
  {
    glCallList(fnt->bitmapBase + (unsigned int)charCode);
  }
}

/************************************************************************
* Function:	HGUglTextFontStr				
* Returns:	void						
* Purpose:	Draws the pixmaps for the given character string using
*		the given HGUgl text font instance.		
* Global refs:	-						
* Parameters:	HGUglTextFont *fnt:	Given HGUgl text font instance.
*		char *str:		Given character string.	
************************************************************************/
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
