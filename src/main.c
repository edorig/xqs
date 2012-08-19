/****
 * main.c
 *****
 * Written By: brett adams (bretta@jove.acs.unt.edu)
 * Description: main.c -- the main() procedure for the xqs prog, reports 
 *	information about an X display, the majority of the information 
 *	retrieval code is derived from the standard X clients xdpyinfo, 
 *	xwininfo, xlsclients, and xprop
 *****
 ****/

#include "xqs.h"
#include "Prefs.h"

/* intialize the widgets for the tabs */
extern void InitPreferenceDB();
void InitMainWindow();
extern void InitMenuBar();
void InitMainTab();
extern void InitDisplayTab();
extern void InitClientsTab();
extern void InitWinTab();
extern void InitPropertiesTab();
extern void InitResourcesTab();
extern void InitAccessTab();
extern void InitWinTree();
extern void InitOptionsPopup();
/* void InitErrorCatcher(); */

extern void DisplayTabActive();
extern void ClientsTabActive();
extern void WindowTabActive();
extern void PropertiesTabActive();
extern void ResourcesTabActive();
extern void AccessTabActive();

#ifdef KBD_TAB
extern void InitKeyboardTab();
extern void KeyboardTabActive();
#endif

void changeTabCB();
Widget GetActiveTabWidget();
int GetActiveTabWidgetNum();
void stopCB();

extern int UpdateWinTree(
#if NeedFunctionPrototypes
			 Display *d,
			 Window toplevel,
			 Widget listree
#endif			 
);

String tab_labels[] = {
  "Display",
  "Clients",
  "Windows",
  "Properties",
  "Resources",
  "Access",
#ifdef KBD_TAB
  "Keyboard"
#endif
};

typedef struct {
  Boolean updateWinTreeOnStartUp;  
  char *infoDisplayName;
} ApplicationData, *ApplicationDataPtr;

XrmOptionDescRec options[] = {
  { "-t", "*updateWintreeOnStartUp", XrmoptionNoArg, "FALSE" },
  { "+t", "*updateWintreeOnStartUp", XrmoptionNoArg, "TRUE" },
  { "-r", "*infoDisplayName", XrmoptionSepArg, NULL },
};

XtResource app_resources[] = {
  { "updateWintreeOnStartUp", "UpdateWintreeOnStartUp", XtRBoolean, 
    sizeof(Boolean), XtOffset( ApplicationDataPtr, updateWinTreeOnStartUp), 
    XtRString, "TRUE"  },
  { "infoDisplayName", "InfoDisplayName", XtRString, sizeof(String),
    XtOffset( ApplicationDataPtr, infoDisplayName), XtRString, NULL },
};



void Usage( char *progname )
{
  fprintf( stderr, "Usage: %s [-th] [-r remote_display]\n", progname );
  exit(1);
}

void longUsage( char *progname )
{
  fprintf( stderr, "Usage: %s [-th] [-r remote_display]\n", progname );
  fprintf( stderr, 
	   "  -r, remote display to get info from if diff't from $DISPLAY\n" );
  fprintf( stderr, "  -t, don't retrieve the window tree on startup\n" );
  fprintf( stderr, "  -h. print this help screen\n" );
  exit(1);	  
}


void 
main( argc, argv )
  int argc;
  char **argv;
{  
  int i;
  char *progname = argv[0];
  ApplicationData app_data;

  appshell = XtAppInitialize( &app_context, progname, options,
			      XtNumber(options), &argc, argv, NULL, NULL, 0);

  /* parse args not used by Xt */
  for( i = 1; i < argc; i++ ){
    char *arg = argv[i];
    if( arg[0] == '-' ){      
      switch( arg[1] ){		
      case 'h':				/* -h, -help */
	longUsage( progname );	    
	continue;
      default:
	Usage( progname );	    	  
      }
    }/* end if() */
    else{ printf( "%s, %c\n", arg, (*arg) ); Usage( progname ); }
  }

  /* get any resources that may have been set on the commnad line */
  XtGetApplicationResources( appshell, &app_data, app_resources, 
			     XtNumber(app_resources), NULL, 0 );

  /* open the display we are getting the information from, could
   * be the one we are already connected to
   */
  info_dpy = XtOpenDisplay( app_context, app_data.infoDisplayName, progname,
			    NULL, options, XtNumber(options), &argc, argv );
  if( !info_dpy ){
    fprintf( stderr, "Could not open display: %s\n", app_data.infoDisplayName);
    exit(EXIT_FAILURE);
  }

  /* initialize each of the main components of the interface */
  InitMainWindow(); 
  InitPreferenceDB();
  InitMenuBar();
  InitWinTree();
  InitMainTab();
  InitDisplayTab();
  InitClientsTab();
  InitPropertiesTab();
  InitResourcesTab();
  InitAccessTab();
#ifdef KBD_TAB
  InitKeyboardTab();
#endif
  InitWinTab();
  InitOptionsPopup(); 
  /* InitErrorCatcher(); */

  XtRealizeWidget( appshell );

  DisplayTabActive();
  pref_db.treePrefs[0].value = app_data.updateWinTreeOnStartUp;
  if( app_data.updateWinTreeOnStartUp == TRUE )
       UpdateWinTree( info_dpy, DefaultRootWindow(info_dpy), wintree );

  XtAppMainLoop( app_context );
}


void 
stopCB(  w, client, call )
  Widget w;
  XtPointer client;
  XtPointer call; 
{
  XtSetSensitive( w, FALSE );
  STOP = TRUE;
}


Widget GetActiveTabWidget()
{
  if( XtIsManaged(display_frame) ) return display_frame;
  else if( XtIsManaged(clients_frame) ) return clients_frame;
  else if( XtIsManaged(win_frame) ) return win_frame;
  else if( XtIsManaged(prop_frame) ) return prop_frame;
  else if( XtIsManaged(access_frame) ) return access_frame;
#ifdef KBD_TAB
  else if( XtIsManaged(key_frame) ) return key_frame;
#endif
  else return (Widget)NULL;
}

int GetActiveTabNum()
{
  if( XtIsManaged(display_frame) ) return DISPLAY_T;
  else if( XtIsManaged(clients_frame) ) return CLIENTS_T;
  else if( XtIsManaged(win_frame) ) return WINDOWS_T;
  else if( XtIsManaged(prop_frame) ) return PROP_T;
  else if( XtIsManaged(access_frame) ) return ACCESS_T;
#ifdef KBD_TAB
  else if( XtIsManaged(key_frame) ) return KEYBOARD_T;
#endif
  else return -1;
}


void InitErroratcher()
{
  XmString xmstr;
  Widget cancel;

  err_pop = XmCreateTemplateDialog( main_window, "err_pop", NULL, 0 );
  err_out = XmCreateScrolledText( err_pop, "err_out", NULL, 0 );
  XtManageChild( err_out );


  cancel = XmCreatePushButtonGadget( err_pop, "cancel", NULL, 0 );
  XtManageChild( cancel );

  xmstr = XmStringCreateLocalized( "Errors" );
  XtVaSetValues( err_pop,
		 XmNdialogTitle, xmstr,
		 NULL );
  XmStringFree( xmstr );  

  XtVaSetValues( err_out,
		 XmNshadowThickness, 0,		 
		 XmNwordWrap, TRUE,
		 XmNeditMode, XmMULTI_LINE_EDIT,
		 XmNeditable, FALSE,  			/* can't edit */
		 XmNcursorPositionVisible, FALSE, 	/* no cursor */
		 XmNautoShowCursorPosition, FALSE,
		 XmNscrollingPolicy, XmAUTOMATIC,
		 NULL );



  xmstr = XmStringCreateLocalized( "Cancel" );
  
}

void InitMainTab()
{  
  /*printf( "called InitMainTab()\n" );*/

  tab_form = XtVaCreateWidget( "tab_form", xmFormWidgetClass, work_area, 
			       XmNtopAttachment, XmATTACH_FORM,	
			       XmNbottomAttachment, XmATTACH_FORM,
			       XmNleftAttachment, XmATTACH_WIDGET,
			       XmNleftWidget, tree_form,
			       XmNrightAttachment, XmATTACH_FORM, 
			       XmNmarginWidth, 5,
			       XmNmarginHeight, 5,
			       XmNorientation, XmVERTICAL,
			       XmNpacking, XmPACK_TIGHT,
			       NULL);

  main_tab = XtVaCreateManagedWidget( "main_tab", xgTabsWidgetClass, tab_form, 
				      XmNtopAttachment, XmATTACH_FORM,	
				      XmNbottomAttachment, XmATTACH_NONE,
				      XmNleftAttachment, XmATTACH_FORM,
				      XmNrightAttachment, XmATTACH_FORM, 
				      XgNtabLabels, tab_labels,
				      XgNtabCount, XtNumber(tab_labels),
				      XgNtabShape, XgCHAMFERRED,
				      NULL );
  XtAddCallback( main_tab, XmNactivateCallback, changeTabCB, NULL );
				        
  XtManageChild( tab_form );

}

/* 
 * initialize the main manager form to hold interface and the status bar 
 * at the bottom 
 */
void
InitMainWindow()
{
  XmString xmstr;
  Widget status_frame, msg_form;
  
  /* printf( "InitMainWindow() called\n" ); */

  main_window = XmCreateMainWindow( appshell, "main_window", NULL, 0 );
  XtManageChild( main_window );

  menu_bar = XmCreateMenuBar( main_window, "menu_bar", NULL, 0 );
  XtManageChild( menu_bar );

  work_area = XmCreateForm( main_window, "work_area", NULL, 0 );
  XtManageChild( work_area );

  msg_form = XmCreateForm( main_window, "msg_form", NULL, 0 );
  XtManageChild( msg_form );

  status_frame = XmCreateFrame( msg_form, "status_frame", NULL, 0 );
  XtManageChild( status_frame );

  status_bar = XmCreateLabelGadget(status_frame, "status_bar", NULL, 0 );
  XtManageChild( status_bar );

/* #ifdef KBD_TAB */
  stop_butt = XmCreatePushButtonGadget( msg_form, "stop_butt", NULL, 0 );
  XtManageChild( stop_butt );
  XtAddCallback( stop_butt, XmNactivateCallback, stopCB, NULL );
/* #endif */
  
  /* 
   * resources for main window components
   */
  XtVaSetValues( main_window, 
		 XmNmenuBar, menu_bar,
		 XmNworkWindow, work_area,
		 XmNmessageWindow, msg_form,
		 XmNresizable, TRUE,
 		 XmNheight, 350, 
 		 XmNwidth, 600, 
		 XmNshadowThickness, 0,		  
		 XmNtranslations, NULL,
		 NULL );      

  XtVaSetValues( work_area,
		 XmNtopAttachment, XmATTACH_FORM,
		 XmNbottomAttachment, XmATTACH_WIDGET,
		 XmNbottomWidget, status_frame,
		 XmNrightAttachment, XmATTACH_FORM,
		 XmNleftAttachment, XmATTACH_FORM,
		 /*XmNmarginWidth, 3,*/
		 NULL );

  XtVaSetValues( msg_form,
		 XmNtopAttachment, XmATTACH_NONE,
		 XmNbottomAttachment, XmATTACH_FORM,
		 XmNrightAttachment, XmATTACH_FORM,
		 XmNleftAttachment, XmATTACH_FORM,
		 NULL );

#ifdef KBD_TAB
  xmstr = XmStringCreateLocalized( "stop" );
  XtVaSetValues( stop_butt,
		 XmNlabelString, xmstr,
		 XmNtopAttachment, XmATTACH_FORM,
		 XmNbottomAttachment, XmATTACH_FORM,
		 XmNrightAttachment, XmATTACH_FORM,
		 XmNleftAttachment, XmATTACH_NONE,
		 XmNsensitive, FALSE,
		 NULL );
  XmStringFree(xmstr);
#endif
  
  XtVaSetValues( status_frame,
		 XmNtopAttachment, XmATTACH_FORM,
		 XmNbottomAttachment, XmATTACH_FORM,
		 XmNleftAttachment, XmATTACH_FORM,
		 XmNrightAttachment, XmATTACH_FORM,
/* #ifdef KBD_TAB */
/*    		 XmNrightAttachment, XmATTACH_WIDGET, */
/*    		 XmNrightWidget, stop_butt, */
/* #else */
/*  		 XmNrightAttachment, XmATTACH_FORM, */
/* #endif  */
		 NULL );

  xmstr = XmStringCreateSimple( "  " );
  XtVaSetValues( status_bar,
		 XmNlabelString, xmstr,
		 XmNalignment, XmALIGNMENT_BEGINNING,
		 XmNshadowThickness, 0,
		 XmNmarginHeight, 3,
		 XmNmarginWidth, 5,
		 NULL );

  XmStringFree( xmstr );

}


void ActivateTabNum( int tabn )
{
  extern Widget display_out;
  
  switch( tabn ){
  case DISPLAY_T:    
    XtSetKeyboardFocus( display_frame, display_out );
    DisplayTabActive();
    break;
  case CLIENTS_T:
    ClientsTabActive();
    break;
  case WINDOWS_T:
    WindowTabActive();
    break;
  case PROP_T:
    PropertiesTabActive();
    break;
  case RES_T:
    ResourcesTabActive();
    break;      
  case ACCESS_T:
    AccessTabActive();
    break;
#ifdef KBD_TAB
  case KEYBOARD_T:
    KeyboardTabActive();
    break;
#endif
  default:
    fprintf( stderr, "Error --  ActivateTabNum(): Unknown Tab number (%d)\n",
	     tabn );
    /* exit( EXIT_FAILURE ); */
    break;
  }
}


void
changeTabCB( w, client, call_data )
  Widget w;
  XtPointer client;
  XgTabsCallbackStruct *call_data;
{
  
  ClearStatus();
  STOP = TRUE;  /* stop any indefinite loops a tab may be running */
  
  /* only unmanage if the tab doesnt change */
  if( call_data->tab != call_data->old_tab ) {
       switch( call_data->old_tab ){
       case DISPLAY_T:
	 XtUnmanageChild( display_frame );
	 break;
       case CLIENTS_T:
	 XtUnmanageChild( clients_frame );
	 break;
       case WINDOWS_T:
	 XtUnmanageChild( win_frame );
	 break;      
       case PROP_T:
	 XtUnmanageChild( prop_frame );
	 break;
       case RES_T:
	 XtUnmanageChild( res_frame );
	 break;      
       case ACCESS_T:
	 XtUnmanageChild( access_frame );
	 break;      
#ifdef KBD_TAB
       case KEYBOARD_T:
	 XtUnmanageChild( key_frame );
	 break;
#endif
       default:
	 fprintf( stderr, "Error -- changeTabCB(): Unknown Tab number (%d_\n",
		  call_data->tab );
	 exit( EXIT_FAILURE );
	 break;      
       }
  }

  if( info_dpy == NULL ){
       SetStatus( "No Display Open" );
       /* return; */
  }

  /* activate the new tab */
  ActivateTabNum( call_data->tab );

}


void 
ActivateCurrentTab()
{
  int tabn = GetActiveTabNum();
  ActivateTabNum( tabn );
}
