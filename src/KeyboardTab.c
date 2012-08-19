/*****
 * KeyboardTab.c
 *****
 * Written By: brett adams (bretta@jove.acs.unt.edu)
 * Description: KeyboardTab.c -- callbacks and and functions called when the
 *	"Keyboard" tab becomes active
 *****
 *****/

#ifdef KBD_TAB

#include "xqs.h"

char *GetKeycodeString( XEvent *event );
Bool GetFocusWindow( Display *dpy, Window *focus_window_return );
void PutFocusWindow( Widget text_wid, Display *d, Window w );
extern void SetWidgetForeColor();

int ErrorCatcher( Display *d, XErrorEvent *event );
void Safe_SelectInputTree( Display *d, Window w, EventMask mask );

Widget key_out;

void KeyboardTabActive()
{
  XEvent xevent;
  EventMask event_mask;
  Window focus_window=0;

  XtManageChild( key_frame );
  if( info_dpy == NULL ) return;

  /* clear the output box */
  /*XmTextClear( key_box );*/
  
  /* mask any events that may be created if a window is destroyed or killed */
  event_mask = FocusChangeMask | KeyPressMask | SubstructureNotifyMask;
  
  SetStatus( "Masking window tree..." );
 
  /* get the window currently in focus and select the events we want */
  focus_window = DefaultRootWindow( info_dpy );
  Safe_SelectInputTree( info_dpy, focus_window, event_mask);
  GetFocusWindow( info_dpy, &focus_window );
  PutFocusWindow( key_out, info_dpy, focus_window );

  ClearStatus(); /* clear the status line */

  XtSetSensitive( stop_butt, TRUE );
  STOP = FALSE;
  while( !STOP ) {  
    XtAppNextEvent( app_context, &xevent );
    
    /* we are only interested in events from the remote display,
     * else simply dispatch them 
     */
    if( info_dpy == xevent.xany.display ) {
      switch( xevent.type ) {	
	/* if a new window a created mask its window tree so we can handle
	 * any events sent to it 
	 */
      case CreateNotify:
	focus_window = xevent.xcreatewindow.window;
	Safe_SelectInputTree( info_dpy, xevent.xcreatewindow.window, 
			      event_mask );
	break;

	/* print the keycode to the key text box */
      case KeyPress: /*case KeyRelease:*/
	XmTextAppend( key_out, GetKeycodeString( &xevent ) );
	break;
	  	  	    	   
	/* put the name of the window with the InputFocus */
      case FocusIn: case FocusOut:
	if( GetFocusWindow(info_dpy, &focus_window) )
	  PutFocusWindow( key_out, info_dpy, focus_window );
	break;	  
  	   
      default: 
	;; /* nothing */
	break;	  
      }/* end switch( xevent.type ) */     
    }
    XtDispatchEvent( &xevent );
    
  }/* end while( !STOP ) */
  

}/* end KeyboardTabActive() */

void Safe_SelectInputTree( Display *dpy, Window win, EventMask mask )
{
  int (*old_handler)() = XSetErrorHandler( &ErrorCatcher );
  SelectInputTree( dpy, win, mask);
  XSetErrorHandler( old_handler );
}

Bool 
GetFocusWindow( dpy, focus_window_return )
Display *dpy;
Window *focus_window_return;
{

  int focus_state;
  Window window;

  XGetInputFocus( dpy, &window, &focus_state );

  /* XGetInputFocus returns PointerRoot(0x1) as the window id if the
   * root window gets the focus and sets the focus_state variable to 
   * RevertToPointerRoot
   */
  focus_state = RevertToPointerRoot;
  if( window == PointerRoot && focus_state == RevertToPointerRoot )
    window = DefaultRootWindow( dpy );    
    
  /* if the focus is the same there is no need to go thru all this again */
  if( window == (*focus_window_return) ){
    /*printf( "dupe\n" );*/
    return FALSE;
  }  
    
  (*focus_window_return) = window;
  return TRUE;
}

/* get the name of the window with the current focus and format and displat it */
void 
PutFocusWindow( text_wid, d, win )
Widget text_wid;
Display *d;
Window win;
{
  int stat;
  Window root;
  char *temp;
  char out_string[ 1024 ];

  root = DefaultRootWindow( d );
  
  if( win == root )    temp = XtNewString( "root" );
  else  stat =  XFetchName( d, win, &temp );

  if( temp == NULL )  temp = XtNewString( "(no name)" );

  sprintf( out_string, "\nfocus name: 0x%x -- %s\n", (unsigned int)win, 
	   temp );
  XmTextAppend( text_wid, out_string );
  XtFree( temp );
}

/*
 * return a printable string for the keycode in an XKeyEvent structure
 */
char *GetKeycodeString( XEvent *event )
{
  int count;
  char *temp;
  KeySym ks;
  const int KEY_BUFF_SIZE  = 256;
  char *key_buff, *special_buff;
  
  key_buff = (char *)XtMalloc( KEY_BUFF_SIZE );

  /* if event != NULL then get then lookup up the string and the keysysm 
   * in the  XKeyEventStructure
   */
  if( event ){  
    count = XLookupString( (XKeyEvent *)event, key_buff, KEY_BUFF_SIZE,
			   &ks, NULL);
    key_buff[count] = '\0';
    
    /* if the length of the string is zero attempt to convert the 
     * keysym to a string 
     */   
    if( count == 0){    
      temp = XKeysymToString(ks);
      if( temp )  strcpy(key_buff, temp);    
      else  strcpy(key_buff, "(NoSymbol)");
    }

    /* handle the special cases */
    switch( ks ){
    case XK_Linefeed: case XK_Return:
      sprintf( key_buff, " \\n \n" );
      break;
      /*
	case XK_Shift_L: case XK_Shift_R:
	break;
      */
    case XK_BackSpace:
      sprintf( key_buff, "(BckSpc)" );
      break;
    case XK_Delete:
      sprintf( key_buff, "(Delete)" );
    case XK_Escape:
      sprintf( key_buff, "(Esc)" );
    default:
      /* if a "special" key then put it in parenthesis before returning */
      if( IsCursorKey(ks) || IsFunctionKey(ks) || IsKeypadKey(ks) || \
	  IsMiscFunctionKey(ks) || IsModifierKey(ks) || IsPFKey(ks) ){
	special_buff = (char *)XtMalloc( KEY_BUFF_SIZE );
	sprintf( special_buff, "(%s)", key_buff );
	XtFree(key_buff); 
	return special_buff;
      }
      break;
    } 

    return key_buff;
  }
  else  return NULL;
}


void
InitKeyboardTab()
{
  Widget key_form, key_scroll, vscroll;
  
  /*printf( "InitOutBox() called\n" );*/

  key_frame = XtVaCreateWidget( "key_frame",
				       xmFrameWidgetClass, tab_form,
				       XmNtopAttachment, XmATTACH_WIDGET,
				       XmNtopWidget, main_tab,
				       XmNbottomAttachment, XmATTACH_FORM,
				       XmNleftAttachment, XmATTACH_FORM,
				       XmNrightAttachment, XmATTACH_FORM,
				       XmNchildType, XmFRAME_WORKAREA_CHILD,
				       XmNshadowType, XmSHADOW_ETCHED_OUT,
				       NULL );

  key_form = XtVaCreateManagedWidget( "key_form",
				      xmFormWidgetClass, key_frame,
				      XmNtopAttachment, XmATTACH_FORM,
				      XmNbottomAttachment, XmATTACH_FORM,
				      XmNleftAttachment, XmATTACH_FORM,
				      XmNrightAttachment, XmATTACH_FORM,
				      NULL, 0 );

  key_scroll = XtVaCreateManagedWidget( "key_scroll",
					xmScrolledWindowWidgetClass, key_form,
					XmNtopAttachment, XmATTACH_FORM,
					XmNrightAttachment, XmATTACH_FORM,
					XmNleftAttachment, XmATTACH_FORM,
					XmNbottomAttachment, XmATTACH_FORM,
					XmNscrollBarPlacement, XmBOTTOM_RIGHT,
					XmNscrollHorizontal, FALSE,
					XmNhorizontalScrollBar, NULL,
					/* XmNscrollingPolicy, XmAUTOMATIC, */
					NULL );
  
  key_out = XtVaCreateManagedWidget( "key_out",
				     xmTextWidgetClass, key_scroll,
				     XmNshadowThickness, 0,
				     XmNeditMode, XmMULTI_LINE_EDIT,
				     XmNeditable, FALSE,
				     XmNcursorPositionVisible, FALSE, 	
				     XmNscrollHorizontal, FALSE,
				     /* XmNscrollingPolicy, XmAUTOMATIC, */
				     XmNwordWrap, TRUE,
				     NULL );

  XtVaGetValues( key_scroll, XmNverticalScrollBar, &vscroll, NULL );
  XtVaSetValues( vscroll, XmNshadowThickness, 2, XmNwidth, 15, NULL );


} /* end InitKeyboardTab */

#endif /* KBD_TAB */
