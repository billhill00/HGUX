#ifndef HGUGL_TEXT_H
#define HGUGL_TEXT_H
#pragma ident "MRC HGU $Id"
/************************************************************************
* Project:	Mouse Atlas
* Title:	HGUglText.h
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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct
{
  Font		xFont;
  GLuint	bitmapBase;
} HGUglTextFont;

HGUglTextFont	*HGUglTextFontCreate(Font);
void		HGUglTextFontChar(HGUglTextFont *, char),
		HGUglTextFontDestroy(HGUglTextFont *),
		HGUglTextFontStr(HGUglTextFont *, char *);


#ifdef __cplusplus 
}
#endif /* __cplusplus */

#endif /* HGUGL_TEXT_H */
