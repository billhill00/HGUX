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
* File        :    HGU_XmFileProcs.c					     *
* $Revision$
******************************************************************************
* Author Name :     Richard Baldock					     *
* Author Login:    richard@hgu.mrc.ac.uk				     *
* Date        :    Thu Aug 25 09:50:45 1994				     *
* Synopsis    : Miscellaneous procedures to check and return FILE 	     *
*		pointers.    					             *
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <Xm/FileSB.h>

#include <HGU_XmUtils.h>

String HGU_XmGetFileStr(
Widget		w,
XmString	filestr,
XmString	dirstr)
{
    String	file;

    /* check the compound string */
    if( !XmStringGetLtoR( filestr, XmSTRING_DEFAULT_CHARSET, &file ) ){
	HGU_XmUserError( w, "HGU_XmGetFilePointer:\n    no filename string",
			  XmDIALOG_FULL_APPLICATION_MODAL );
	return( NULL );
    }

    if( !(*file) ){
	HGU_XmUserError( w, "HGU_XmGetFilePointer:\n    no file selected",
			  XmDIALOG_FULL_APPLICATION_MODAL );
	XtFree( file );
	return( NULL );
    }

    /* create full path */
    if( *file != '/' ){
	String dir, newfile;
	if( XmStringGetLtoR( dirstr, XmSTRING_DEFAULT_CHARSET, &dir ) ){
	    if( (newfile = XtMalloc( (unsigned int)
			(strlen(dir) + strlen(file) + 2) ) ) == NULL ){
		XtFree( dir );
		return( NULL );
	    }
	    (void) sprintf( newfile, "%s/%s", dir, file );
	    XtFree( file );
	    XtFree( dir );
	    file = newfile;
	}
    }
    return( file );
}

FILE *HGU_XmGetFilePointerBck(
Widget		w,
XmString	filestr,
XmString	dirstr,
char		*type,
char		*extension)
{
    String	file;
    FILE	*fp;
    struct stat	buf;
    String	filebak;
    int		was_bak;

    if( (file = HGU_XmGetFileStr( w, filestr, dirstr )) == NULL )
	return( NULL );
    was_bak = 0;
    /* check status if regular file */
    /* don't bother with this check if write permission is requested
       because the file may not exist */
    if( stat( file, &buf ) == -1 ){
	if( *type != 'w' && *type != 'a' ){
	    HGU_XmUserError( w, "HGU_XmGetFilePointer:\n"
			        "    can't get status of selected\n"
			        "    file - please check selection",
			      XmDIALOG_FULL_APPLICATION_MODAL );
	    XtFree( file );
	    return( NULL );
	}
    } else if( !S_ISREG( buf.st_mode ) ){
	HGU_XmUserError( w, "HGU_XmGetFilePointer:\n"
			    "    selection is not a regular file\n"
			    "    (eg a directory) - please check selection",
			  XmDIALOG_FULL_APPLICATION_MODAL );
	XtFree( file );
	return( NULL );
    } else if( (*type == 'w' || *type == 'a') ){

	if( !HGU_XmUserConfirm(w,"Overwrite existing file?","yes","no",0) ){
	    XtFree( file ); 
	    return( NULL );
	}

	/* check for backup */
	if( extension != NULL ){
	    if( ( filebak = (String) malloc( (unsigned int) (strlen(file) +
					strlen(extension) + 1 ) ) ) != NULL )
		(void) sprintf( filebak, "%s%s", file, extension );

	    if( ( filebak == NULL ) || rename( file, filebak ) ){
		if( !HGU_XmUserConfirm( w,"Backup failed, write anyway?",
				       "yes","no",0 ) ){
		    free( filebak );
		    XtFree( file ); 
		    return( NULL );
		}
	    was_bak++;
            free( filebak );
	    }
	}
    }

    /* open the file */
    if( (fp = fopen( file, type )) == NULL ){
	HGU_XmUserError( w, "HGU_XmGetFilePointer:\n"
			    "    unable to open file - please\n"
			    "    check that you have appropriate\n"
			    "    access permission (read/write/execute)",
			  XmDIALOG_FULL_APPLICATION_MODAL );
	XtFree( file );
	return( NULL );
    }

    /* If an original file was backed up, try to change the permissions and
       group of the new file to be the same as the original. */
    if( was_bak )
    {
	(void) chmod( file, buf.st_mode );
	(void) chown( file, (uid_t) -1, buf.st_gid );
    }

    XtFree( (char *) file );
    return( fp );
}

FILE *HGU_XmGetFilePointer(
    Widget	w,
    XmString	filestr,
    XmString	dirstr,
    char	*type)
{
    return( HGU_XmGetFilePointerBck(w, filestr, dirstr, type, NULL) );
}
