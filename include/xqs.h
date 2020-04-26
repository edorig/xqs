/***
 * xqs.h
 *****
 * Written By: brett adams (bretta@jove.acs.unt.edu)
 * Description: xqs.h -- header file for the xqs program
 ******
 *****/

#ifndef _XQS_H
#define _XQS_H

/* motif and X includes */
#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/CascadeBG.h>
#include <Xm/LabelG.h>
#include <Xm/PushBG.h>
#include <Xm/ToggleBG.h>
#include <Xm/Form.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrolledW.h>
#include <Xm/List.h>
#include <Xm/MainW.h>
#include <Xm/BulletinB.h>
#include <Xm/MessageB.h>
#include <Xm/Frame.h>
#include <Xm/ScrolledW.h>

#include <X11/Xos.h>
#include <X11/Xutil.h>
#include <X11/Xmu/WinUtil.h>
#include <X11/extensions/multibuf.h>
#include <X11/extensions/shape.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>

/* local includes */
#include "Xb.h"
#include "ListTree.h"
#include "Tabs.h"
#include "ComboBox.h"
#include "XgUtils.h"

/* misc C includes */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

XtAppContext app_context;
Display *info_dpy;		      /* display we are retrieving info from */

/* global widgets */
Widget appshell,
  main_window,
  menu_bar,
  work_area,		     
  status_bar,
  stop_butt,
  err_pop,
  err_out,
  tree_form,
  wintree,
  main_tab,
  tab_form,
  d_open_text,
  q_pop,
  display_frame,
  clients_frame,
  win_frame,
  prop_frame,
  res_frame,
#ifdef KBD_TAB
   key_frame,
#endif 
  access_frame;

Bool STOP;	/* flag to stop indefinite loops */

/*** misc functions ***/
void SetStatus(				/* set message in status line */
#if NeedFunctionPrototypes
	       String string
#endif
);

extern int ErrorCatcher(		/* print errors w/o exiting */
#if NeedFunctionPrototypes
			Display *d,
			XErrorEvent *event
#endif
);

extern int ErrorsToWidget(		/* print errors to an XmText widget */
#if NeedFunctionPrototypes		/* without exiting */
			  Display *d,
			  XErrorEvent *event
#endif
);

extern void ClearStatus();			/* clear the status line */
extern int GetActiveTabNum();

typedef struct {
	long code;
	char *name;
} binding;

char *Lookup(
#if NeedFunctionPrototypes
	     int code,
	     binding *table
#endif
);


#ifdef KBD_TAB
enum{ DISPLAY_T, CLIENTS_T, WINDOWS_T, PROP_T, RES_T, ACCESS_T, KEYBOARD_T };
#else
enum{ DISPLAY_T, CLIENTS_T, WINDOWS_T, PROP_T, RES_T, ACCESS_T };
#endif

#endif	/* _XQS_H */
