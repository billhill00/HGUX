#pragma ident "MRC HGU $Id$"
/*!
* \file         HGU_XmFileList.c
* \author       richard <Richard.Baldock@hgu.mrc.ac.uk>
* \date         Thu Apr  8 15:32:47 2004
* \version      MRC HGU $Id$
*               $Revision$
*               $Name$
* \par Copyright:
*               1994-2002 Medical Research Council, UK.
*               All rights reserved.
* \par Address:
*               MRC Human Genetics Unit,
*               Western General Hospital,
*               Edinburgh, EH4 2XU, UK.
* \ingroup      HGU_Xm
* \brief        Utilities to manage a recent file-list in Motif applications.
*               
* \todo         -
* \bug          None known
*
* Maintenance log with most recent changes at top of list.
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <time.h>

#include <Xm/XmAll.h>

#include <Wlz.h>
#include <WlzExtFF.h>
#include <HGU_XmUtils.h>

/* general procedures and structures for managing a file list
   and file-list menu items as in "recent"
*/

void HGU_XmFileListItemFree(
  void	*entry)
{
  HGU_XmFileListCallbackStruct	*cbs;

  if( entry ){
    cbs = (HGU_XmFileListCallbackStruct *) entry;
    if( cbs->file ){
      AlcFree(cbs->file);
    }
    AlcFree(cbs);
  }
  return;
}

WlzErrorNum HGU_XmFileListClearList(
  AlcDLPList	*fileList)
{
  AlcDLPItem	*item;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  if( fileList ){
    item = fileList->head;
    while( item ){
      item = AlcDLPItemUnlink(fileList, item, 1, NULL);
    }
  }
  else {
    errNum = WLZ_ERR_PARAM_NULL;
  }

  return errNum;
}

AlcDLPList *HGU_XmFileListCreateList(
  String	resourceFile,
  WlzErrorNum	*dstErr)
{
  AlcDLPList	*list=NULL;
  FILE		*fp;
  HGU_XmFileListCallbackStruct	*cbs;
  int		index;
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  AlcErrno	alcErrno;

  /* check input parameters */
  if( resourceFile == NULL ){
    errNum = WLZ_ERR_PARAM_NULL;
  }

  /* create list */
  list = AlcDLPListNew(&alcErrno);

  /* read file to get menu items */
  if( errNum == WLZ_ERR_NONE ){
    if( fp = fopen(resourceFile, "r") ){
      BibFileRecord	*bibfileRecord;
      BibFileError	bibFileErr;

      /* search for file list entry */
      bibFileErr = BibFileRecordRead(&bibfileRecord, NULL, fp);
      while( bibFileErr == BIBFILE_ER_NONE ){

	/* create items and add to list */
	if( !strncmp(bibfileRecord->name, "HGU_XmFileListFileRecord", 23) ){
	  cbs = (HGU_XmFileListCallbackStruct *)
	    AlcMalloc(sizeof(HGU_XmFileListCallbackStruct));
	  errNum = WlzEffBibParseFileRecord(bibfileRecord, &index,
					    &(cbs->file), &(cbs->format));
	  AlcDLPListEntryAppend(list, NULL, (void *) cbs,
				HGU_XmFileListItemFree);
	}

	BibFileRecordFree(&bibfileRecord);
	bibFileErr = BibFileRecordRead(&bibfileRecord, NULL, fp);
      }
    }
    else {
      errNum = WLZ_ERR_FILE_OPEN;
    }
  }

  if( dstErr ){
    *dstErr = errNum;
  }
  return list;
}

MenuItem *HGU_XmFileListCreateMenuItems(
  AlcDLPList	*fileList,
  XtCallbackProc	callbackProc,
  WlzErrorNum	*dstErr)
{
  MenuItem	*items;
  int		i, p, numItems;
  AlcDLPItem	*listItem;
  
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  AlcErrno	alcErrno;
  HGU_XmFileListCallbackStruct	*cbs;
  /* check inputs */
  if( fileList == NULL ){
    errNum = WLZ_ERR_PARAM_NULL;
  }

  /* create menu */
  if( errNum == WLZ_ERR_NONE ){
    /* create menu items */
    numItems = AlcDLPListCount(fileList, &alcErrno);
    items = (MenuItem *) AlcMalloc(sizeof(MenuItem) * (numItems+4));

    /* set menu items */
    i = 0;
    listItem = fileList->head;
    while( listItem ){
      cbs = (HGU_XmFileListCallbackStruct *) listItem->entry;

      /* copy in the required fields and generate short name
	 for the menu item */
      for(p=strlen(cbs->file)-1; p >= 0; p--){
	if( cbs->file[p] == '/' ){
	  break;
	}
      }
      items[i].name = (String) AlcMalloc(strlen((&(cbs->file[p+1]))) + 8);
      sprintf(items[i].name, "  %s  ", &(cbs->file[p+1]));
      items[i].wclass = &xmPushButtonWidgetClass;
      items[i].mnemonic = '\0';
      items[i].accelerator = NULL;
      items[i].accel_text = NULL;
      items[i].item_set = False;
      items[i].callback = callbackProc;
      items[i].callback_data = (XtPointer) cbs;
      items[i].help_callback = NULL;
      items[i].help_callback_data = NULL;
      items[i].tear_off_model = XmTEAR_OFF_DISABLED;
      items[i].radio_behavior = False;
      items[i].always_one = False;
      items[i].subitems = NULL;

      i++;
      listItem = listItem->next;
      if( listItem == fileList->head ){
	listItem = NULL;
      }
    }

    /* add a separator */
    items[i].name = AlcStrDup("separator");
    items[i].wclass = &xmSeparatorWidgetClass;
    items[i].mnemonic = '\0';
    items[i].accelerator = NULL;
    items[i].accel_text = NULL;
    items[i].item_set = False;
    items[i].callback = NULL;
    items[i].callback_data = NULL;
    items[i].help_callback = NULL;
    items[i].help_callback_data = NULL;
    items[i].tear_off_model = XmTEAR_OFF_DISABLED;
    items[i].radio_behavior = False;
    items[i].always_one = False;
    items[i].subitems = NULL;
    i++;

    /* add button to clear the list */
    items[i].name = AlcStrDup("Clear list");
    items[i].wclass = &xmPushButtonWidgetClass;
    items[i].mnemonic = '\0';
    items[i].accelerator = NULL;
    items[i].accel_text = NULL;
    items[i].item_set = False;
    items[i].callback = callbackProc;
    items[i].callback_data = NULL;
    items[i].help_callback = NULL;
    items[i].help_callback_data = NULL;
    items[i].tear_off_model = XmTEAR_OFF_DISABLED;
    items[i].radio_behavior = False;
    items[i].always_one = False;
    items[i].subitems = NULL;
    i++;

    items[i].name = NULL;
  }

  /* assign error */
  if( dstErr ){
    *dstErr = errNum;
  }
  return items;
}

WlzErrorNum HGU_XmFileListDestroyMenuItems(
  MenuItem	*items)
{
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  int		i;

  /* check inputs */
  if( items == NULL ){
    errNum = WLZ_ERR_PARAM_NULL;
  }
  else {
    for(i=0; items[i].name != NULL; i++){
      AlcFree(items[i].name);
    }
    AlcFree(items);
  }

  return errNum;
}

WlzErrorNum HGU_XmFileListResetMenu(
  AlcDLPList	*fileList,
  Widget	cascade,
  XtCallbackProc	callbackProc)
{
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  Widget	menu=NULL, widget;
  MenuItem	*items;
  HGU_XmFileListCallbackStruct	*cbs;
  int		i;
  char		*strbuf;

  if( cascade ){
    XtVaGetValues(cascade, XmNsubMenuId, &menu, NULL);
    if( menu ){
      XtDestroyWidget(menu);
    }
    items = HGU_XmFileListCreateMenuItems(fileList,
					  callbackProc, NULL);
    menu = HGU_XmBuildPulldownMenu(cascade, XmTEAR_OFF_DISABLED,
				   False, False, items);
    /* add tool-tips */
    for(i=0; items[i].name != NULL; i++){
      if( cbs = (HGU_XmFileListCallbackStruct *) items[i].callback_data ){
	if(strcmp(items[i].name, "separator") &&
	   strcmp(items[i].name, "Clear list")){
	  strbuf = AlcMalloc(sizeof(char)*(strlen(cbs->file)+4));
	  sprintf(strbuf, "*%s", cbs->file);
	  if(widget = XtNameToWidget(menu, strbuf)){
	    HGU_XmAddToolTip(HGU_XmGetTopShell(cascade), widget,
			     cbs->file);
	  }
	  AlcFree(strbuf);
	}
      }
    }
    HGU_XmFileListDestroyMenuItems(items);
  }

  return errNum;
}

WlzErrorNum HGU_XmFileListAddFile(
  AlcDLPList	*fileList,
  String	file,
  WlzEffFormat	format)
{
  HGU_XmFileListCallbackStruct	*cbs;
  AlcDLPItem	*item;
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  AlcErrno	alcErr;

  /* check inputs */
  if((fileList == NULL) || (file == NULL)){
    errNum = WLZ_ERR_PARAM_NULL;
  }

  /* create new item and add to head of the list */
  if( errNum == WLZ_ERR_NONE ){
    /* check if already in the list, in which case bring
       it to the top */
    item = fileList->head;
    while( item ){
      cbs = (HGU_XmFileListCallbackStruct *) item->entry;
      if( !strcmp(file, cbs->file) ){
	break;
      }

      if( item->next == fileList->head ){
	item = NULL;
      }
      else {
	item = item->next;
      }
    }

    /* move or create new list item */
    if( item ){
      AlcDLPItemUnlink(fileList, item, 0, &alcErr);
      AlcDLPItemInsert(fileList, NULL, item);
    }
    else {
      cbs = (HGU_XmFileListCallbackStruct *)
	AlcMalloc(sizeof(HGU_XmFileListCallbackStruct));
      cbs->file = AlcStrDup(file);
      cbs->format = format;
      AlcDLPListEntryInsert(fileList, NULL, (void *) cbs,
			    HGU_XmFileListItemFree);
    }
  }

  while( AlcDLPListCount(fileList, &alcErr) > HGU_XMFILELIST_MAXNUMITEMS ){
    AlcDLPItemUnlink( fileList, fileList->head->prev, 1, &alcErr);
  }

  return errNum;
}

void HGU_XmFileListWriteHeader(
  FILE	*fp)
{
  BibFileRecord		*bibfileRecord;
  time_t		tmpTime;
  char			*tmpS, tmpBuf[256];
  char  		*idxS = NULL;
  char  		*dateS = NULL;
  char  		*hostS = NULL;
  char  		*userS = NULL;
  static char		unknownS[] = "unknown";

  /* write some sort of identifier */
  bibfileRecord = 
    BibFileRecordMake("Ident", "0",
		      BibFileFieldMakeVa("Text",
					 "HGU_Xm Image File List",
					 "Version",	"1.0",
					 NULL));
  BibFileRecordWrite(fp, NULL, bibfileRecord);
  BibFileRecordFree(&bibfileRecord);

  /* now a comment with user, machine, date etc. */
  tmpS = getenv("USER");
  (void )sprintf(tmpBuf, "User: %s", tmpS?tmpS:unknownS);
  userS = AlcStrDup(tmpBuf);

  tmpTime = time(NULL);
  tmpS = ctime(&tmpTime);
  *(tmpS + strlen(tmpS) - 1) = '\0';
  (void )sprintf(tmpBuf, "Date: %s", tmpS?tmpS:unknownS);
  dateS = AlcStrDup(tmpBuf);

  tmpS = getenv("HOST");
  (void )sprintf(tmpBuf, "Host: %s", tmpS?tmpS:unknownS);
  hostS = AlcStrDup(tmpBuf);

  bibfileRecord = 
    BibFileRecordMake("Comment", "0",
		      BibFileFieldMakeVa("Text", userS,
					 "Text", dateS,
					 "Text", hostS,
					 NULL));
  BibFileRecordWrite(fp, NULL, bibfileRecord);
  BibFileRecordFree(&bibfileRecord);
  AlcFree(userS);
  AlcFree(dateS);
  AlcFree(hostS);

  return;
}

WlzErrorNum HGU_XmFileListWriteResourceFile(
  AlcDLPList	*fileList,
  String	resourceFile)
{
  WlzErrorNum	errNum=WLZ_ERR_NONE;
  AlcDLPItem	*item;
  HGU_XmFileListCallbackStruct	*cbs;
  FILE		*fp;

  /* check inputs */
  if( (fileList == NULL) || (resourceFile == NULL) ){
    errNum = WLZ_ERR_PARAM_NULL;
  }

  /* open the file */
  if( errNum == WLZ_ERR_NONE ){

    /* currently assume only these entries are written to the
       resource file */
    if( fp = fopen(resourceFile, "w") ){
      /* write a file header */
      HGU_XmFileListWriteHeader(fp);
      item = fileList->head;
      while( item ){
	cbs = (HGU_XmFileListCallbackStruct *) item->entry;
	WlzEffBibWriteFileRecord(fp, "HGU_XmFileListFileRecord",
				 cbs->file, cbs->format);
	item = item->next;
	if( item == fileList->head ){
	  item = NULL;
	}
      }
      fclose( fp );
    }
    else {
      errNum = WLZ_ERR_FILE_OPEN;
    }
  }

  return errNum;
}

WlzObject *HGU_XmFileListReadObject(
  Widget			w,
  HGU_XmFileListCallbackStruct	*cbs,
  WlzErrorNum			*dstErr)
{
  WlzObject	*rtnObj=NULL;
  WlzErrorNum	errNum=WLZ_ERR_NONE;

  if((w != NULL) && (cbs != NULL)){
    rtnObj = WlzEffReadObj(NULL, cbs->file, cbs->format, 0, &errNum);
  }
  else {
    errNum = WLZ_ERR_PARAM_NULL;
  }

  if( dstErr ){
    *dstErr = errNum;
  }
  return rtnObj;
}

