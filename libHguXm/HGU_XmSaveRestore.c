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
* File        :    HGU_XmSaveRestore.c					     *
* $Revision$
******************************************************************************
* Author Name :     Richard Baldock					     *
* Author Login:    richard@hgu.mrc.ac.uk				     *
* Date        :    Thu Aug 25 09:57:29 1994				     *
* Synopsis:	Save and restore facility for X11/Xt window based system.    *
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <unistd.h>

#include <string.h>
#include <time.h>
#include <pwd.h>

#include <Xm/FileSB.h>

#include <HGU_XmUtils.h>

/* private global variables */
static Widget	save_state_dialog, restore_state_dialog;
static char	argv_0[128];

/* action and callback procedures */

void SaveStateCallback(
Widget		w,
XtPointer	client_data,
XtPointer	call_data)
{
    if( w || client_data || call_data ){/* to satisfy the IRIX646 compiler */}

    XmFileSelectionDoSearch( save_state_dialog, NULL );
    XtManageChild( save_state_dialog );
}

void RestoreStateCallback(
Widget		w,
XtPointer	client_data,
XtPointer	call_data)
{
    if( w || client_data || call_data ){/* to satisfy the IRIX646 compiler */}

    XmFileSelectionDoSearch( restore_state_dialog, NULL );
    XtManageChild( restore_state_dialog );
}

/* save and restore list control */
typedef struct _SaveRestoreList {
    Widget			widget;
    SaveRestoreProc		save_func;
    XtPointer			save_data;
    SaveRestoreProc		restore_func;
    XtPointer			restore_data;
    struct _SaveRestoreList	*next_data;
} SaveRestoreList;

static SaveRestoreList		*save_list=NULL;
static SaveRestoreProc		default_save_func;
static XtPointer		default_save_data;

void HGU_XmSaveRestoreChangeDefault(
SaveRestoreProc	func,
XtPointer	data)
{
    if( func != NULL ){
	default_save_func = func;
	default_save_data = data;
    }	
    return;
}

void HGU_XmSaveRestoreAddWidget(
Widget		w,
SaveRestoreProc	save_func,
XtPointer	save_data,
SaveRestoreProc	restore_func,
XtPointer	restore_data)
{
    SaveRestoreList	*svl;

    HGU_XmSaveRestoreRemoveWidget( w );

    /* allocate space and add to list */
    if( (svl = (SaveRestoreList *) malloc(sizeof(SaveRestoreList))) == NULL )
	return;
    svl->widget = w;
    svl->save_func = save_func;
    svl->save_data = save_data;
    svl->restore_func = restore_func;
    svl->restore_data = restore_data;
    svl->next_data = save_list;
    save_list = svl;
}
void HGU_XmSaveRestoreRemoveWidget(
Widget		w)
{
    SaveRestoreList	*svl=save_list, *svl1;

    if( svl == NULL || w == NULL )
	return;

    if( svl->widget == w ){
	save_list = svl->next_data;
	free( svl );
	return;
    }

    while( svl->next_data != NULL && svl->next_data->widget != w )
	svl = svl->next_data;

    if( svl->next_data == NULL )
	return;
    svl1 = svl->next_data->next_data;
    free( svl->next_data );
    svl->next_data = svl1;
    return;
}

static void default_save(
Widget		w,
XtPointer	data,
FILE		*fp)
{
    Position	x, y;
    Position	x_off=0, y_off=0;
    Dimension	width, height;
    String	name = XtName( w ), topl_name = (String) data;

    fprintf(fp, "!\n! %s widget geometry\n!\n", name);

    if( XtIsTransientShell( w ) )
    {
	HGU_XmGetShellOffsets( w, &x_off, &y_off );
	if( HGU_XmIsViewable( w ) )
	    fprintf(fp, "%s*%s.mappedWhenManaged: True\n",
		    topl_name, XtName(XtParent(w)));
	else
	    fprintf(fp, "%s*%s.mappedWhenManaged: False\n",
		    topl_name, XtName(XtParent(w)));
    } 
    else if( XtIsTransientShell( XtParent( w )) )
    {
	HGU_XmGetShellOffsets( XtParent( w ), &x_off, &y_off );
	if( HGU_XmIsViewable( w ) )
	    fprintf(fp, "%s*%s.mappedWhenManaged: True\n",
		    topl_name, XtName(XtParent(w)));
	else
	    fprintf(fp, "%s*%s.mappedWhenManaged: False\n",
		    topl_name, XtName(XtParent(w)));
    }

    if( XtIsSubclass( w, xmBulletinBoardWidgetClass ) ){
	if( HGU_XmIsViewable( w ) )
	    fprintf(fp, "%s*%s.defaultPosition: \tFalse\n", topl_name, name);
	else
	    fprintf(fp, "%s*%s.defaultPosition: \tTrue\n", topl_name, name);
    }
	    
    XtVaGetValues(w, XmNx, &x, XmNy, &y,
		  XmNwidth, &width, XmNheight, &height, 
		  NULL);
    fprintf(fp, "%s*%s.x:     \t%d\n", topl_name, name, x - x_off);
    fprintf(fp, "%s*%s.y:     \t%d\n", topl_name, name, y - y_off);
    fprintf(fp, "%s*%s.width: \t%d\n", topl_name, name, width);
    fprintf(fp, "%s*%s.height:\t%d\n", topl_name, name, height);
    return;
}

static void save_state(
Widget		w,
XtPointer	client_data,
XtPointer	call_data)
{
    Widget		topl = (Widget) client_data;
    XmFileSelectionBoxCallbackStruct
	*cbs = (XmFileSelectionBoxCallbackStruct *) call_data;
    FILE		*fp;
    time_t		daytime = time(NULL);
    struct passwd	*pwp;
    String		str1, str2;
    Position		x, y, x_off, y_off;
    Dimension		width, height;
    String		topl_name;
    SaveRestoreList	*svl=save_list;

    /* get the resource file */
    if( (fp = HGU_XmGetFilePointer( w, cbs->value, cbs->dir, "w" )) == NULL )
	return;
    str1 = ctime( (const time_t *) &daytime );
    pwp  = getpwuid( getuid() );
    str2 = pwp->pw_gecos;
    fprintf(fp, "!\n! saved state resource file from %s\n", argv_0);
    fprintf(fp, "! date: %s! user: %s\n!\n", str1, str2); /* no \n after date:
							     because ctime
							     includes the \n
							     */

    /* save top-level geometry */
    topl_name = XtName( topl );
    XtVaGetValues(topl, XmNx, &x, XmNy, &y,
		  XmNwidth, &width, XmNheight, &height, 
		  NULL);
    HGU_XmGetShellOffsets( topl, &x_off, &y_off );

    fprintf(fp, "!\n! top-level widget geometry\n!\n");
    fprintf(fp, "%s.x:       \t%d\n", topl_name, x - x_off);
    fprintf(fp, "%s.y:       \t%d\n", topl_name, y - y_off);
    fprintf(fp, "%s.width:   \t%d\n", topl_name, width);
    fprintf(fp, "%s.height:  \t%d\n", topl_name, height);
    fprintf(fp, "%s.geometry:\t%dx%d+%d+%d\n", topl_name, width, height,
	    x-5, y-21);

    /* call registered save_state procedures */
    while( svl != NULL ){

	if( svl->save_func != NULL )
	    (*svl->save_func)( svl->widget, svl->save_data, fp );
	else
	    (*default_save_func)( svl->widget, default_save_data, fp );

	svl = svl->next_data;
    }
	    
    fclose( fp );
}

static void restore_state_from_file(
Widget	topl,
FILE	*fp)
{
    char		buf[512];
    String		t_name = XtName( topl );
    Boolean		resize_topl=False;

    /* read and set resources */
    while( fgets( buf, 512, fp ) != NULL ){

	char		w_name[128], r_name[64];
	Widget		widget;
	XtResourceList	rsc_list;
	Cardinal	num_rsc;
	int		i;

	/* check for empty string or comment line */
	if( strlen(buf) == 0 || buf[0] == '!' )
	    continue;

	/* extract widget name, resource name and value */
	if( sscanf(buf, " %[^,:,\n]: %[^,\n]", w_name, buf) < 2 )
	    continue;
	if( (int) strlen(w_name) < 1 )
	    continue;
	for(i=(int)strlen(w_name)-1; i > 0 && w_name[i] != '.'
	    && w_name[i] != '*'; i--){}
	w_name[i] = '\0';
	strcpy( &r_name[0], &w_name[i+1] );

	/* get the widget - check for topl widget name */
	switch( w_name[0] ){
	case '\0':
	    widget = topl;
	    break;
	case '.':
	case '*':
	    if( strcmp(&(w_name[1]), t_name) == 0 )
		widget = topl;
	    else if( strncmp(&(w_name[1]), t_name, strlen(t_name)) == 0 ){
		String str = (String) &(w_name[strlen(t_name)+1]);
		if( (widget = XtNameToWidget(topl, str)) == NULL )
		    continue;
	    }
	    else if( (widget = XtNameToWidget(topl, w_name)) == NULL )
		continue;
	    break;
	default:
	    if( strcmp(&w_name[0], t_name) == 0 )
		widget = topl;
	    else if( strncmp(&w_name[0], t_name, strlen(t_name)) == 0 ){
		String str = (String) &(w_name[strlen(t_name)]);
		if( (widget = XtNameToWidget(topl, str)) == NULL )
		    continue;
	    }
	    else if( (widget = XtNameToWidget(topl, w_name)) == NULL )
		continue;
	    break;
	}

	/* convert and set the value */
	XtGetResourceList( XtClass(widget), &rsc_list, &num_rsc );
	for(i=0; i < num_rsc; i++){
	    if( strcmp( r_name, rsc_list[i].resource_name ) == 0 ){
		XrmValue	from, to;

		from.size = (int) strlen( buf );
		from.addr = (caddr_t) buf;

		to.addr = NULL;

		/* some string resources are not copied, eg geometry,
		   therefore allocate space for each string. This is
		   probably a small memory leak */
		if( strcmp(rsc_list[i].resource_type, XtRString) == 0 ){
		    String	str = (String) malloc(strlen(buf) + 4);
		    strcpy(str, buf);
		    XtVaSetValues( widget, r_name, str, NULL );
		}
		else if( XtConvertAndStore( widget, XtRString, &from,
					   rsc_list[i].resource_type, &to) ){

		    /* check for resize of top-level widget */
		    if( widget == topl &&
		       (!strcmp(r_name, XmNheight) ||
			!strcmp(r_name, XmNwidth)) )
		    {
			resize_topl = True;
		    }

		    /* set the resource value */
		    switch( rsc_list[i].resource_size ){
		    case 1:
			XtVaSetValues( widget, r_name,
				      *((unsigned char *) to.addr), NULL );
			break;
		    case 2:
			XtVaSetValues( widget, r_name,
				      *((unsigned short *) to.addr), NULL );
			break;
		    case 4:
			XtVaSetValues( widget, r_name,
				      *((unsigned int *) to.addr), NULL );
			break;
		    }
		}
		break;
	    }
	}
	XtFree( (char *)rsc_list );
    }

    /* check top-level */
    if( resize_topl == True ){
	WidgetList	children;
	Cardinal	numChildren;
	XtVaGetValues( topl, XmNchildren, &children,
		      XmNnumChildren, &numChildren, NULL);
	XtUnmanageChildren(children, numChildren);
	XtManageChildren(children, numChildren);
    }
    return;
}
static void restore_state(
Widget		w,
XtPointer	client_data,
XtPointer	call_data)
{
    Widget		topl = (Widget) client_data;
    XmFileSelectionBoxCallbackStruct
	*cbs = (XmFileSelectionBoxCallbackStruct *) call_data;
    FILE		*fp;
    String		filestr;
    XrmDatabase 	database;
    SaveRestoreList	*svl=save_list;

    /* get the resource file name */
    if( (fp = HGU_XmGetFilePointer( w, cbs->value, cbs->dir, "r" )) == NULL )
	return;

    /* merge the file database with the application database */
    if( filestr = HGU_XmGetFileStr( w, cbs->value, cbs->dir ) ){
	database = XtDatabase(XtDisplay(topl));
	XrmCombineFileDatabase(filestr, &database, True);
	XtFree( filestr );
    }

    /* read and set resources */
    restore_state_from_file( topl, fp );

    /* call registered restore_state procedures */
    while( svl != NULL ){

	if( svl->restore_func != NULL )
	    (*svl->restore_func)( svl->widget, svl->restore_data, fp );

	svl = svl->next_data;
    }
	    
    fclose( fp );
    return;
}

void HGU_XmSaveRestoreHelpCallback(
Widget		topl,
XtCallbackProc	help_cb,
XtPointer	help_data)
{
    if( topl ){/* to satisfy the IRIX646 compiler */}

    XtAddCallback(save_state_dialog, XmNhelpCallback, help_cb, help_data );
    XtAddCallback(restore_state_dialog, XmNhelpCallback, help_cb, help_data );
    return;
}


/* save-restore initialisation procedure */
void HGU_XmSaveRestoreInit(
Widget	topl,
int	*argc,
char	**argv)
{
    XmString		xmstr;
    Visual		*visual;
    Arg			arg[1];
    static int		initialised=0;

    /* check initialisation */
    if( initialised )
	return;
    initialised = 1;

    /* check command line for -state option */
    if( *argc > 2 && argv != NULL ){
	int 	i;
	FILE	*fp;
	for(i=0; i < *argc; i++){
	    if( strcmp(argv[i], "-state") == 0 && i < (*argc-1) ){
		int	j;

		XrmDatabase database = XtDatabase(XtDisplay(topl));

		/* merge the file database with the application database */
		XrmCombineFileDatabase(argv[i+1], &database, True);

		/* set any topl widget resources */
		if( (fp = fopen(argv[i+1], "r")) != NULL ){
		    restore_state_from_file( topl, fp );
		    fclose( fp );
		} else {
		    fprintf(stderr,
			    "HGU_XmSaveRestoreInit: can't open file %s\n",
			    argv[i+1]);
		}

		/* remove the -state option from the list */
		for(j=i+2; j < *argc; j++)
		    argv[j-2] = argv[j];
		*argc -= 2;
		i--;
	    }
	}
    }


    /* default save procedure */
    default_save_func = default_save;
    default_save_data = XtName( topl );

    /* create save_state and restore_state file selection dialogs */
    /* get the visual explicitly */
    visual = HGU_XmWidgetToVisual( topl );
    XtSetArg(arg[0], XmNvisual, visual);
    if( argv == NULL )
	XtVaGetValues(topl, XtNargv, &argv, NULL);
    save_state_dialog =
	XmCreateFileSelectionDialog( topl, "save_state_dialog", arg, 1);
    sprintf(argv_0, "%s - save state", argv[0]);
    xmstr = XmStringCreateSimple( argv_0 );
    XtVaSetValues( save_state_dialog, XmNdialogTitle, xmstr, NULL );
    XmStringFree( xmstr );

    XtAddCallback(save_state_dialog, XmNcancelCallback, 
                  (void (*)()) XtUnmanageChild, NULL);
    XtAddCallback(save_state_dialog, XmNokCallback, 
                  (void (*)()) XtUnmanageChild, NULL);
    XtAddCallback(save_state_dialog, XmNokCallback, 
                  save_state, (XtPointer) topl);

    restore_state_dialog =
	XmCreateFileSelectionDialog( topl, "restore_state_dialog", arg, 1);
    sprintf(argv_0, "%s - restore state", argv[0]);
    xmstr = XmStringCreateSimple( argv_0 );
    XtVaSetValues( restore_state_dialog, XmNdialogTitle, xmstr, NULL );
    XmStringFree( xmstr );

    XtAddCallback(restore_state_dialog, XmNcancelCallback, 
                  (void (*)()) XtUnmanageChild, NULL);
    XtAddCallback(restore_state_dialog, XmNokCallback, 
                  (void (*)()) XtUnmanageChild, NULL);
    XtAddCallback(restore_state_dialog, XmNokCallback, 
                  restore_state, (XtPointer) topl);

    sprintf( argv_0, "%s", argv[0] );
    return;
}

/* save restore functions for a selected Motif widgets */

void HGU_XmFSD_SaveFunc(
Widget		w,
XtPointer	data,
FILE		*fp)
{
    String	name = XtName( w ), topl_name = (String) data;

    /* first the default save - position etc. */
    default_save( w, data, fp );

    /* save directory and pattern resources */

    if( XtIsSubclass( w, xmFileSelectionBoxWidgetClass ) ){
	XmString	dir_xmstr, pat_xmstr;
	String		str;

	XtVaGetValues(w, XmNdirectory, &dir_xmstr, 
		      XmNpattern, &pat_xmstr, NULL);

	if( XmStringGetLtoR(dir_xmstr, XmSTRING_DEFAULT_CHARSET, &str) ){
	    fprintf(fp, "%s*%s.directory:     \t%s\n", topl_name, name, str);
	    XtFree( str );
	}

	if( XmStringGetLtoR(pat_xmstr, XmSTRING_DEFAULT_CHARSET, &str) ){
	    fprintf(fp, "%s*%s.pattern:       \t%s\n", topl_name, name, str);
	    XtFree( str );
	}

    }
    return;
}
    

