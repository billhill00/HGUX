#if defined(__GNUC__)
#ident "University of Edinburgh $Id$"
#else
static char _HGU_XmHelpMenu_c[] = "University of Edinburgh $Id$";
#endif
/*!
* \file         HGU_XmHelpMenu.c
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
* \brief        
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#include <X11/cursorfont.h>
#include <X11/Xatom.h>
#include <Xm/PushBG.h>
#include <X11/Xmu/WinUtil.h>	/* for XmuClientWindow() */

#include <HGU_XmUtils.h>

MenuItem HGU_XmHelpMenuItemsP[] = {		/* help_menu items */
  {"on_program", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   HGU_XmHelpOnProgramCb, NULL, NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"index", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   HGU_XmHelpIndexCb, NULL, NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"on_context", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   HGU_XmHelpOnContextCb, NULL, NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"on_windows", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   HGU_XmHelpOnWindowsCb, NULL, NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"on_keys", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   HGU_XmHelpOnKeysCb, NULL, NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"on_help", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   HGU_XmHelpOnHelpCb, NULL, NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"tutorial", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   HGU_XmHelpTutorialCb, NULL, NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {"version", &xmPushButtonGadgetClass, 0, NULL, NULL, False,
   HGU_XmHelpVersionCb, NULL, NULL, NULL,
   XmTEAR_OFF_DISABLED, False, False, NULL},
  {NULL},
};

MenuItem	*HGU_XmHelpMenuItems = &(HGU_XmHelpMenuItemsP[0]);

/* procedures to access the netscape interface */

/* resource structures and type converters */

typedef struct 
{
  String	help_www_server;
  String	help_dir;
  String	help_default_url;
  String	help_index_url;
  String	help_version_url;
  String	help_viewer;
} HGU_HelpResources;

static HGU_HelpResources	help_resources={NULL,NULL,NULL,NULL,NULL};

#define help_res_offset(field)   XtOffsetOf(HGU_HelpResources, field)
 
static XtResource help_res[] = {
  {"helpWWWServer", "HelpWWWServer", XtRString, sizeof(String),
   help_res_offset(help_www_server),
   XtRString, "http://genex.hgu.mrc.ac.uk/"},

  {"helpDir", "HelpDir", XtRString, sizeof(String),
   help_res_offset(help_dir),
   XtRString, "MouseAtlas/HelpPages"},

  {"helpDefaultUrl", "HelpDefaultUrl", XtRString, sizeof(String),
   help_res_offset(help_default_url),
   XtRString, "libhguXm/default.html"},

  {"helpIndexUrl", "HelpIndexUrl", XtRString, sizeof(String),
   help_res_offset(help_index_url),
   XtRString, "libhguXm/default.html"},

  {"helpVersionUrl", "HelpVersionUrl", XtRString, sizeof(String),
   help_res_offset(help_version_url),
   XtRString, "libhguXm/default.html"},

  {"helpViewer", "HelpViewer", XtRString, sizeof(String),
   help_res_offset(help_viewer),
   XtRString, "/opt/netscape/netscape"},
};
 

const char *progname="HGU_XmHelpUtilities";
const char *expected_mozilla_version="4.02";

#define MOZILLA_VERSION_PROP   "_MOZILLA_VERSION"
#define MOZILLA_LOCK_PROP      "_MOZILLA_LOCK"
#define MOZILLA_COMMAND_PROP   "_MOZILLA_COMMAND"
#define MOZILLA_RESPONSE_PROP  "_MOZILLA_RESPONSE"
static Atom XA_MOZILLA_VERSION  = 0;
static Atom XA_MOZILLA_LOCK     = 0;
static Atom XA_MOZILLA_COMMAND  = 0;
static Atom XA_MOZILLA_RESPONSE = 0;

static void
mozilla_remote_init_atoms (Display *dpy)
{
  if (! XA_MOZILLA_VERSION)
    XA_MOZILLA_VERSION = XInternAtom (dpy, MOZILLA_VERSION_PROP, False);
  if (! XA_MOZILLA_LOCK)
    XA_MOZILLA_LOCK = XInternAtom (dpy, MOZILLA_LOCK_PROP, False);
  if (! XA_MOZILLA_COMMAND)
    XA_MOZILLA_COMMAND = XInternAtom (dpy, MOZILLA_COMMAND_PROP, False);
  if (! XA_MOZILLA_RESPONSE)
    XA_MOZILLA_RESPONSE = XInternAtom (dpy, MOZILLA_RESPONSE_PROP, False);
}

static Window
hgu_remote_find_window (Display *dpy)
{
  int i;
  Window root = RootWindowOfScreen (DefaultScreenOfDisplay (dpy));
  Window root2, parent, *kids;
  unsigned int nkids;
  Window result = 0;
  Window tenative = 0;
  unsigned char *tenative_version = 0;

  if (! XQueryTree (dpy, root, &root2, &parent, &kids, &nkids))
    {
      fprintf (stderr, "%s: XQueryTree failed on display %s\n", progname,
	       DisplayString (dpy));
      return( (Window) 0 );
    }

  /* root != root2 is possible with virtual root WMs. */

  if (! (kids && nkids))
    {
      fprintf (stderr, "%s: root window has no children on display %s\n",
	       progname, DisplayString (dpy));
      return( (Window) 0 );
    }

  for (i = nkids-1; i >= 0; i--)
    {
      Atom type;
      int format;
      unsigned long nitems, bytesafter;
      unsigned char *version = 0;
      Window w = XmuClientWindow (dpy, kids[i]);
      int status = XGetWindowProperty (dpy, w, XA_MOZILLA_VERSION,
				       0, (65536 / sizeof (long)),
				       False, XA_STRING,
				       &type, &format, &nitems, &bytesafter,
				       &version);
      if (! version)
	continue;
      if (strcmp ((char *) version, expected_mozilla_version) &&
	  !tenative)
	{
	  tenative = w;
	  tenative_version = version;
	  continue;
	}
      XFree (version);
      if (status == Success && type != None)
	{
	  result = w;
	  break;
	}
    }

  if (result && tenative)
    {
      fprintf (stderr,
	       "%s: warning: both version %s (0x%x) and version\n"
	       "\t%s (0x%x) are running.  Using version %s.\n",
	       progname, tenative_version, (unsigned int) tenative,
	       expected_mozilla_version, (unsigned int) result,
	       expected_mozilla_version);
      XFree (tenative_version);
      return result;
    }
  else if (tenative)
    {
      fprintf (stderr,
	       "%s: warning: expected version %s but found version\n"
	       "\t%s (0x%x) instead.\n",
	       progname, expected_mozilla_version,
	       tenative_version, (unsigned int) tenative);
      XFree (tenative_version);
      return tenative;
    }
  else if (result)
    {
      return result;
    }
  else
    {
      fprintf (stderr, "%s: not running on display %s\n", progname,
	       DisplayString (dpy));
      return( (Window) 0 );
    }
}

static int
hgu_remote_command (Display *dpy, Window window, const char *command,
		    Bool raise_p)
{
  int result;
  Bool done = False;
  char *new_command = 0;

  /* The -noraise option is implemented by passing a "noraise" argument
     to each command to which it should apply.
     */
  if (! raise_p)
    {
      char *close;
      new_command = (char *) malloc (strlen (command) + 20);
      strcpy (new_command, command);
      close = strrchr (new_command, ')');
      if (close)
	strcpy (close, ", noraise)");
      else
	strcat (new_command, "(noraise)");
      command = new_command;
    }

#ifdef DEBUG_PROPS
  fprintf (stderr, "%s: (writing " MOZILLA_COMMAND_PROP " \"%s\" to 0x%x)\n",
	   progname, command, (unsigned int) window);
#endif

  XChangeProperty (dpy, window, XA_MOZILLA_COMMAND, XA_STRING, 8,
		   PropModeReplace, (unsigned char *) command,
		   strlen (command));

  /* for some reason netscape response does not appear
     therefore for now don't check for PropertyNotify response */
  done = True;

  while (!done)
    {
      XEvent event;
      XNextEvent (dpy, &event);
      if (event.xany.type == DestroyNotify &&
	  event.xdestroywindow.window == window)
	{
	  /* Print to warn user...*/
	  fprintf (stderr, "%s: window 0x%x was destroyed.\n",
		   progname, (unsigned int) window);
	  result = 6;
	  goto DONE;
	}
      else if (event.xany.type == PropertyNotify &&
	       event.xproperty.state == PropertyNewValue &&
	       event.xproperty.window == window &&
	       event.xproperty.atom == XA_MOZILLA_RESPONSE)
	{
	  Atom actual_type;
	  int actual_format;
	  unsigned long nitems, bytes_after;
	  unsigned char *data = 0;

	  result = XGetWindowProperty (dpy, window, XA_MOZILLA_RESPONSE,
				       0, (65536 / sizeof (long)),
				       True, /* atomic delete after */
				       XA_STRING,
				       &actual_type, &actual_format,
				       &nitems, &bytes_after,
				       &data);
#ifdef DEBUG_PROPS
	  if (result == Success && data && *data)
	    {
	      fprintf (stderr, "%s: (server sent " MOZILLA_RESPONSE_PROP
		       " \"%s\" to 0x%x.)\n",
		       progname, data, (unsigned int) window);
	    }
#endif

	  if (result != Success)
	    {
	      fprintf (stderr, "%s: failed reading " MOZILLA_RESPONSE_PROP
		       " from window 0x%0x.\n",
		       progname, (unsigned int) window);
	      result = 6;
	      done = True;
	    }
	  else if (!data || strlen((char *) data) < 5)
	    {
	      fprintf (stderr, "%s: invalid data on " MOZILLA_RESPONSE_PROP
		       " property of window 0x%0x.\n",
		       progname, (unsigned int) window);
	      result = 6;
	      done = True;
	    }
	  else if (*data == '1')	/* positive preliminary reply */
	    {
	      fprintf (stderr, "%s: %s\n", progname, data + 4);
	      /* keep going */
	      done = False;
	    }
#if 1
	  else if (!strncmp ((char *)data, "200", 3)) /* positive completion */
	    {
	      result = 0;
	      done = True;
	    }
#endif
	  else if (*data == '2')		/* positive completion */
	    {
	      fprintf (stderr, "%s: %s\n", progname, data + 4);
	      result = 0;
	      done = True;
	    }
	  else if (*data == '3')	/* positive intermediate reply */
	    {
	      fprintf (stderr, "%s: internal error: "
		       "server wants more information?  (%s)\n",
		       progname, data);
	      result = 3;
	      done = True;
	    }
	  else if (*data == '4' ||	/* transient negative completion */
		   *data == '5')	/* permanent negative completion */
	    {
	      fprintf (stderr, "%s: %s\n", progname, data + 4);
	      result = (*data - '0');
	      done = True;
	    }
	  else
	    {
	      fprintf (stderr,
		       "%s: unrecognised " MOZILLA_RESPONSE_PROP
		       " from window 0x%x: %s\n",
		       progname, (unsigned int) window, data);
	      result = 6;
	      done = True;
	    }

	  if (data)
	    XFree (data);
	}
#ifdef DEBUG_PROPS
      else if (event.xany.type == PropertyNotify &&
	       event.xproperty.window == window &&
	       event.xproperty.state == PropertyDelete &&
	       event.xproperty.atom == XA_MOZILLA_COMMAND)
	{
	  fprintf (stderr, "%s: (server 0x%x has accepted "
		   MOZILLA_COMMAND_PROP ".)\n",
		   progname, (unsigned int) window);
	}
#endif /* DEBUG_PROPS */
    }

DONE:

  if (new_command)
    free (new_command);

  return result;
}

static Window	netscape_window;
static char	*help_str_file = NULL;
static char	*help_str_url = NULL;


static Widget get_window_widget(Widget	w)
{
  if( XtIsShell( w ) || XtIsShell(XtParent(w)) )
    return( w );

  return( get_window_widget( XtParent(w) ) );
}

static Widget get_topl_widget(Widget	w)
{
  Widget	parent = XtParent( w );
  if( parent )
    return( get_topl_widget( parent ) );
  return( w );
}

int HGU_XmHelpCreateViewer(
			   Widget	w,
			   char		*url)
{
  int	wait_time, time_out_period = 30;

  /* check initialisation */
  if( help_resources.help_dir == NULL ){
    HGU_XmHelpMenuInit( get_topl_widget( w ) );
  }

  /* get the viewer window */
  if( (netscape_window = hgu_remote_find_window(XtDisplayOfObject(w))) == (Window) 0 ){
    
    /* start up a netscape viewer */
    if( fork() == 0 ){

      execl(help_resources.help_viewer, "netscape", "-display", 
	    DisplayString(XtDisplayOfObject(w)), url, NULL);
      fprintf(stderr, 
	      "HGU_XmHelpUtilities: failed to start netscape viewer\n");
      exit( 1 ); 	/* if it returns the viewer can't execute */
    }

    /* wait for the netscape window */
    wait_time = 0;
    while( (netscape_window = hgu_remote_find_window(XtDisplayOfObject(w)))
	   == (Window) 0 ){
      sleep( 5 );
      wait_time += 5;
      if( wait_time >= time_out_period ){
	return( 1 );
      }
    }
  }

  return( 0 );
}

void HGU_XmHelpShowUrl(
		       Widget	w,
		       char		*url)
{
  String	str;

  /* check initialisation */
  if( help_resources.help_dir == NULL ){
    HGU_XmHelpMenuInit( get_topl_widget( w ) );
  }

  /* check the url for relative or absolute */
  if( url == NULL )
    url = help_resources.help_default_url;

  if(strncmp(url, "file:",   5) &&
     strncmp(url, "http:",   5) &&
     strncmp(url, "gopher:", 7) &&
     strncmp(url, "WAIS:",   5) &&
     strncmp(url, "news:",   5) &&
     strncmp(url, "telnet:", 7) )
    {
      str = (String) malloc(sizeof(char) *
			    (strlen(url) +
			     strlen(help_resources.help_www_server) +
			     strlen(help_resources.help_dir) +
			     32));
      if( url[0] == '/' )
	sprintf(str, "%s%s", help_resources.help_www_server, url);
      else
	sprintf(str, "%s%s/%s", help_resources.help_www_server,
		help_resources.help_dir, url);
    }
  else
    {
      str = (String) malloc(sizeof(char) * (strlen(url) + 2));
      strcpy( str, url );
    }
  url = str;

  /* check that the viewer is available */
  if( HGU_XmHelpCreateViewer(w, url) ){
    HGU_XmUserError(w,
		    "Can't find or create a Netscape viewer\n"
		    " - please check the  helpViewer resource",
		    XmDIALOG_FULL_APPLICATION_MODAL);
    return;
  }

  /* create the command for Netscape */
  str = (String) malloc(sizeof(char) * (strlen(url) + 32));
  sprintf(str, "openURL(%s)", url);

  /* send the string to Netscape */
  hgu_remote_command(XtDisplayOfObject(w), netscape_window, (const char *) str,
		     True);

  return;
}

void HGU_XmHelpShowString(
			  Widget	w,
			  char		*title,
			  char		*str)
{
  FILE	*fp;

  /* check initialisation */
  if( help_resources.help_dir == NULL ){
    HGU_XmHelpMenuInit( get_topl_widget( w ) );
  }

  /* create temporary file-name and file url */
  if( help_str_file == NULL ){
    help_str_file = (char *) malloc(sizeof(char) * 64);
    help_str_file = (char *) malloc(sizeof(char) * 72);
    sprintf(help_str_file, "/tmp/HGU_XmHelpFile_%d.html", getpid());
    sprintf(help_str_url, "file:%s", help_str_file);
  }

  /* open a temporary file */
  if( (fp = fopen(help_str_file, "w")) == NULL )
    return;

  /* output the html formatting and string */
  if( title != NULL ){
    fprintf(fp, "<TITLE>%s</TITLE>\n", title);
    fprintf(fp, "<CENTER><H1>%s</H1> </CENTER>\n", title);
  } else {
    fprintf(fp, "<TITLE>HGU_XmHelp Help Page</TITLE>\n");
    fprintf(fp, "<CENTER><H1>HGU_XmHelp Help Page</H1> </CENTER>\n");
  }

  fprintf(fp, "<HR WIDTH=\"60%%\">\n");
  fprintf(fp, "<PRE>\n%s\n</PRE>\n", str);

  fprintf(fp, "<HR WIDTH=\"60%%\">\n"
	  "<PRE>\n"
	  "Copyright:      Medical Research Council, UK, 1994.\n"
	  "Synopsis:       Html page generated by HGU_XmHelpShowString\n"
	  "</PRE>\n"
	  "<ADDRESS>Contact address - "
	  "<A HREF=\"mailto:R.Baldock@hgu.mrc.ac.uk\">"
	  "R.Baldock@hgu.mrc.ac.uk</A></ADDRESS>\n");

  /* display the text */
  (void) fclose( fp );
  HGU_XmHelpShowUrl( w, help_str_url );
  return;
}

void HGU_XmHelpDestroyViewer()
{
  return;
}

/* action and callback procedures */

void HGU_XmHelpDestroyCb(Widget		w,
			 XtPointer	client_data,
			 XtPointer	call_data)
{

  /* check initialisation */
  if( help_resources.help_dir == NULL ){
    HGU_XmHelpMenuInit( get_topl_widget( w ) );
  }

  unlink((const char *) help_str_url);

  return;    
}

void HGU_XmHelpStandardCb(Widget	w,
			  XtPointer	client_data,
			  XtPointer	call_data)
{
  /* check initialisation */
  if( help_resources.help_dir == NULL ){
    HGU_XmHelpMenuInit( get_topl_widget( w ) );
  }

  HGU_XmHelpShowUrl( w, (String) client_data );

  return;
}


void HGU_XmHelpStandardStringCb(Widget	w,
				XtPointer	client_data,
				XtPointer	call_data)
{
  /* check initialisation */
  if( help_resources.help_dir == NULL ){
    HGU_XmHelpMenuInit( get_topl_widget( w ) );
  }

  HGU_XmHelpShowString( w, NULL, (String) client_data );
  return;
}

void HGU_XmHelpOnProgramCb(Widget	w,
			   XtPointer	client_data,
			   XtPointer	call_data)
{

  /* check initialisation */
  if( help_resources.help_dir == NULL ){
    HGU_XmHelpMenuInit( get_topl_widget( w ) );
  }

  HGU_XmHelpShowUrl(w, help_resources.help_default_url);
}

void HGU_XmHelpOnContextCb(Widget	w,
			   XtPointer	client_data,
			   XtPointer	call_data)
{
  Widget		help_widget, topl;
  Cursor		cursor;
  XmAnyCallbackStruct	*cbs = (XmAnyCallbackStruct *) call_data;

  /* check initialisation */
  if( help_resources.help_dir == NULL ){
    HGU_XmHelpMenuInit( get_topl_widget( w ) );
  }

  cursor = XCreateFontCursor(XtDisplayOfObject(w), XC_hand2);
  topl = get_topl_widget( w );
  if( (help_widget = XmTrackingEvent(topl, cursor, False, cbs->event)) ){
    cbs->reason = XmCR_HELP;
    XtCallCallbacks(help_widget, XmNhelpCallback, &cbs);
  }
  XFreeCursor(XtDisplayOfObject(w), cursor);

  return;
}

void HGU_XmHelpOnWindowsCb(Widget	w,
			   XtPointer	client_data,
			   XtPointer	call_data)
{
  Widget		help_widget, topl;
  Cursor		cursor;
  XmAnyCallbackStruct	*cbs = (XmAnyCallbackStruct *) call_data;

  /* check initialisation */
  if( help_resources.help_dir == NULL ){
    HGU_XmHelpMenuInit( get_topl_widget( w ) );
  }

  cursor = XCreateFontCursor(XtDisplayOfObject(w), XC_hand2);
  topl = get_topl_widget( w );
  if( (help_widget = XmTrackingLocate(topl, cursor, False)) ){
    help_widget = get_window_widget( help_widget );
    cbs->reason = XmCR_HELP;
    XtCallCallbacks(help_widget, XmNhelpCallback, &cbs);
  }
  XFreeCursor(XtDisplayOfObject(w), cursor);

  return;
}

void HGU_XmHelpOnKeysCb(Widget		w,
			XtPointer	client_data,
			XtPointer	call_data)
{
  /* check initialisation */
  if( help_resources.help_dir == NULL ){
    HGU_XmHelpMenuInit( get_topl_widget( w ) );
  }

  return;
}

void HGU_XmHelpIndexCb(Widget		w,
		       XtPointer	client_data,
		       XtPointer	call_data)
{
  /* check initialisation */
  if( help_resources.help_dir == NULL ){
    HGU_XmHelpMenuInit( get_topl_widget( w ) );
  }

  HGU_XmHelpShowUrl(w, help_resources.help_index_url);
}

void HGU_XmHelpOnHelpCb(Widget		w,
			XtPointer	client_data,
			XtPointer	call_data)
{
  /* check initialisation */
  if( help_resources.help_dir == NULL ){
    HGU_XmHelpMenuInit( get_topl_widget( w ) );
  }

  HGU_XmHelpShowUrl(w, "libhguXm/HGU_XmHelpMenu.html");
}

void HGU_XmHelpTutorialCb(Widget	w,
			  XtPointer	client_data,
			  XtPointer	call_data)
{
  /* check initialisation */
  if( help_resources.help_dir == NULL ){
    HGU_XmHelpMenuInit( get_topl_widget( w ) );
  }

  return;
}

void HGU_XmHelpVersionCb(Widget		w,
			 XtPointer	client_data,
			 XtPointer	call_data)
{

  /* check initialisation */
  if( help_resources.help_dir == NULL ){
    HGU_XmHelpMenuInit( get_topl_widget( w ) );
  }

  HGU_XmHelpShowUrl(w, help_resources.help_version_url);
  return;
}

/* help_menu initialisation procedure */
void HGU_XmHelpMenuInit(Widget	topl)
{
  /* initialise the help resources */
  XtGetApplicationResources(topl, &help_resources, help_res,
			    XtNumber(help_res), NULL, 0);

  /* setup a temporary filename */
  help_str_file = (char *) malloc(sizeof(char) * 64);
  help_str_url = (char *) malloc(sizeof(char) * 72);
  sprintf(help_str_file, "/tmp/HGU_XmHelpMenu_%d.html", getpid());
  sprintf(help_str_url, "file:%s", help_str_file);

  /* setup the required atoms */
  mozilla_remote_init_atoms(XtDisplayOfObject(topl));

  return;
}
 
