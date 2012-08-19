/***** 
 * EventTab.c 
 *****
 * Written By: brett adams (bretta@jove.acs.unt.edu)
 * Description: EventTab.c -- callbacks and and functions called when the
 *	"Events" tab becomes active 
 *****
 ****/

#include "xqs.h"

#define MAXITEMS 300

void popupEventSelectCB();
void okEventSelectCB();

void PutEventInfo( Widget w, XEvent ev);
static Bool togg_set[ LASTEvent ] = { FALSE };

EventMask BuildEventMask( Bool ev_toggle[] );
static EventMask mask = 0;

static XEvent event_items[MAXITEMS];
static int offset = 0;
/* int ErrorCatcher( Display *d, XErrorEvent *event ); */

Widget event_list, eventpop, event_togg[LASTEvent], event_out;

void 
EventTabActive()
{
  Window root;
  XEvent xevent;
  XmString item;
  int nitems;
  
  int (*old_handler)() = XSetErrorHandler( &ErrorsToWidget );

  XtManageChild( event_frame );

  if( info_dpy == NULL ) {
    XSetErrorHandler( old_handler );
    return;
  }

  /*QuestionPopup( "Are you a wanker?" );*/

  /* only mask those events selected in the event popup */
  /*if( event_mask == 0 )  SetStatus( "No Events Selected" );*/
  if( mask == 0 )  SetStatus( "No Events Selected" );
  else {
    SetStatus( "Masking root window tree..." );
    root = DefaultRootWindow( info_dpy );
    SelectInputTree( info_dpy, root, mask );
    ClearStatus();
    STOP = FALSE;
    XtSetSensitive( stop_butt, TRUE );
  }
  
  while( !STOP ) {
    XtAppNextEvent( app_context, &xevent );
    if( togg_set[ xevent.type ] ){
      XtVaGetValues( event_list, XmNitemCount, &nitems, NULL );
      if( nitems > MAXITEMS )  XmListDeletePos( event_list, MAXITEMS );
      
      if( xevent.xany.display == info_dpy ) {
	item = XmStringCreateLocalized( EventTypeToString(xevent.type) );
	XmListAddItemUnselected( event_list, item, 1);
	event_items[nitems] =  xevent;
	XmStringFree( item );
      }
    }
    XtDispatchEvent( &xevent );

  }  /* end while( !STOP ) */

  mask = 0;
/*   if( mask != 0 ){ */
/*     mask = 0; */
/*     root = DefaultRootWindow( info_dpy ); */
/*     SelectInputTree( info_dpy, root, 0 ); */
/*   } */

  XSetErrorHandler( old_handler );

}/* end EventTabActive() */





void 
eventListSelectCB( w, client, cb_struct)/*, XtPointer call )*/
  Widget w;
  XtPointer client;
  XmListCallbackStruct *cb_struct;
{
  int nitems;

  /*printf( "%s\n", XgConvertXmStringToString( cb_struct->item ) );*/

  XtVaGetValues( w, XmNitemCount, &nitems, NULL );

  if( cb_struct->item_position < offset )
    PutEventInfo( event_out, event_items[offset - cb_struct->item_position] );
  else
    PutEventInfo( event_out, 
		  event_items[nitems - cb_struct->item_position + offset] );

  XmTextNewLine(event_out);

}


void
popupEventSelectCB( w, client, call )
  Widget w;
  XtPointer client;
  XtPointer call;
{
  int i;

  /*printf( "popupEventSelectCB called\n" );*/
  
  for( i=2; i<LASTEvent; i++ )
    XtVaSetValues( event_togg[ i ],
		   XmNset, togg_set[ i ],
		   NULL );

  XtManageChild( eventpop );
  
}

void
popupWindowSelectCB( Widget w, XtPointer client, XtPointer call )
{

}

void 
cancelEventSelectCB( Widget w, XtPointer client, XtPointer call )
{
  XtUnmanageChild( eventpop );
}

void
applyEventSelectCB( Widget w, XtPointer client, XtPointer call )
{
  int i;
  
  /* from X.h KeyPress = 2 */
  for( i=KeyPress; i<LASTEvent; i++ )
    XtVaGetValues( event_togg[i], 
		   XmNset, &togg_set[ i ],
		   NULL );
  STOP = TRUE;
  mask = BuildEventMask( togg_set );
  EventTabActive();
  
}

void
okEventSelectCB( Widget w, XtPointer client, XtPointer call )
{
  int i;

  /* from X.h KeyPress = 2 */
  for( i=KeyPress; i<LASTEvent; i++ )
    XtVaGetValues( event_togg[ i ], 
		   XmNset, &togg_set[i],
		   NULL );
  XtUnmanageChild( eventpop );
  STOP = TRUE;
  mask = BuildEventMask( togg_set );
  EventTabActive();

}

void
allEventSelectCB( Widget w, XtPointer client, XtPointer call )
{
  int i;

  /*printf( "allEventSelectCB called\n" );*/
  
  for( i=2; i<LASTEvent; i++ )
    XtVaSetValues( event_togg[i], 
		   XmNset, TRUE,
		   NULL );
}

void
unallEventSelectCB( Widget w, XtPointer client, XtPointer call )
{
  int i;

  /*printf( "unallEventSelectCB called\n" );*/
  
  for( i=2; i<LASTEvent; i++ )
    XtVaSetValues( event_togg[ i ], 
		   XmNset, FALSE,
		   NULL );
}

EventMask BuildEventMask(  Bool ev_togg[] )
{
  EventMask mask = 0;
  int i;
  for( i=2; i<LASTEvent; i++ ){
    switch( i ){
    case ButtonPress: 
      if( ev_togg[i] == TRUE ) mask = mask | ButtonPressMask;
      break;
    case ButtonRelease:
      if( ev_togg[i] == TRUE ) mask = mask | ButtonReleaseMask;
      break;
    case MotionNotify:
      if( ev_togg[i] == TRUE ) 
	mask = mask | PointerMotionMask | PointerMotionHintMask | 
	  ButtonMotionMask | Button1MotionMask | Button2MotionMask |
	  Button3MotionMask | Button4MotionMask  | Button5MotionMask;
      break;
    case ColormapNotify:
      if( ev_togg[i] == TRUE )  mask = mask | ColormapChangeMask;
      break;
    case CreateNotify:
      if( ev_togg[i] == TRUE ) mask = mask | SubstructureNotifyMask;
      break;
    case EnterNotify:
      if( ev_togg[i] == TRUE ) mask = mask | EnterWindowMask;
      break;
    case LeaveNotify:
      if( ev_togg[i] == TRUE ) mask = mask | LeaveWindowMask;
      break;
    case FocusIn: case FocusOut:
      if( ev_togg[i] == TRUE ) mask = mask | FocusChangeMask;
      break;
    case GraphicsExpose: /* no mask for GraphicsExpose event */
      break;
    case Expose:
      if( ev_togg[i] == TRUE ) mask =mask | ExposureMask;
      break;
    case VisibilityNotify:
      if( ev_togg[i] == TRUE ) mask = mask | VisibilityChangeMask;
      break;
    case KeyPress:
      if( ev_togg[i] == TRUE ) mask = mask | KeyPressMask;
      break;
    case KeyRelease:
      if( ev_togg[i] == TRUE ) mask = mask | KeyReleaseMask;
      break;
    case KeymapNotify:
      if( ev_togg[i] == TRUE ) mask = mask | KeymapStateMask;
      break;
    case NoExpose: /* no mask for NoExpose events */
      break;
    case PropertyNotify:
      if( ev_togg[i] == TRUE ) mask = mask | PropertyChangeMask;
      break;
    case ResizeRequest:
      if( ev_togg[i] == TRUE ) mask = mask | ResizeRedirectMask;
      break;
    case MappingNotify: case SelectionClear: case ClientMessage: 
    case SelectionNotify: case SelectionRequest:
      break;    /* these events are always sent */
    case DestroyNotify:
      if( ev_togg[i] == TRUE ) mask = mask | SubstructureNotifyMask;
      break;
    case CirculateNotify: case ConfigureNotify: case GravityNotify:   
    case MapNotify: case ReparentNotify: case UnmapNotify:
      if( ev_togg[i] == TRUE ) mask = mask | SubstructureNotifyMask |
				 StructureNotifyMask;
      break;
    case CirculateRequest: case ConfigureRequest: case MapRequest:
      if( ev_togg[i] == TRUE ) mask = mask | SubstructureRedirectMask;
      break;
    default:
      /* should never get here */
      fprintf( stderr, "ERROR -- BuildMask(): Reached switch default\n" );
      fprintf( stderr, "event.type: %d\n", i );
      break;
    }
  }

  return mask;

}/* end BuildMask() */



void InitEventTab()
{
  Widget event_form, list_scroll, event_butt, 
    out_scroll, eventrc, cancel_butt, ok_butt, all_butt, 
    apply_butt, unall_butt;
  XmString xmstr, cancel, ok, apply, all, unall; 
  int i;
  Arg arg[1];


  event_frame = XmCreateFrame( tab_form, "event_frame", NULL, 0 );

  /* create the main form for the event tab */
  event_form = XmCreateForm( event_frame, "event_form", NULL , 0 );
  XtManageChild( event_form );

  /* create the list to hold the reported events */
  event_list = XmCreateScrolledList( event_form, "event_list", NULL, 0 );
  XtManageChild( event_list );
  XtAddCallback( event_list, XmNbrowseSelectionCallback, eventListSelectCB, 
		 NULL);
  
  /* get the scrollbar for the event_list widget */
  list_scroll = XtParent( event_list );

  event_out = XmCreateScrolledText( event_form, "event_out", NULL, 0 );
  XtManageChild( event_out );

  /* get scrollbar for the event_out widget */
  out_scroll = XtParent( event_out );

  /**********/

  event_butt = XmCreatePushButtonGadget( event_form, "event_butt", NULL, 0 );
  XtManageChild( event_butt );
  XtAddCallback( event_butt, XtNactivateCallback, popupEventSelectCB, NULL );

  i = 0;
  XtSetArg( arg[i], XmNautoUnmanage, FALSE); i++;
  eventpop = XmCreateTemplateDialog( main_window, "eventpop", arg, i );

  eventrc = XmCreateRowColumn( eventpop, "eventrc", NULL, 0 );
  XtManageChild( eventrc );
  
  ok_butt = XmCreatePushButtonGadget( eventpop, "ok_butt", NULL, 0 );
  XtManageChild( ok_butt );
  XtAddCallback( ok_butt, XtNactivateCallback, okEventSelectCB, NULL );

  apply_butt = XmCreatePushButtonGadget( eventpop, "apply_butt", NULL, 0 );
  XtManageChild( apply_butt );
  XtAddCallback( apply_butt, XtNactivateCallback, applyEventSelectCB, NULL );

  cancel_butt = XmCreatePushButtonGadget( eventpop, "cancel_butt", NULL, 0 );
  XtManageChild( cancel_butt );
  XtAddCallback( cancel_butt, XtNactivateCallback, cancelEventSelectCB, NULL );

  all_butt = XmCreatePushButtonGadget( eventpop, "all_butt", NULL, 0 );
  XtManageChild( all_butt );
  XtAddCallback( all_butt, XtNactivateCallback, allEventSelectCB, NULL );
  
  unall_butt = XmCreatePushButtonGadget( eventpop, "unall_butt", NULL, 0 );
  XtManageChild( unall_butt );
  XtAddCallback( unall_butt, XtNactivateCallback, unallEventSelectCB, NULL );
  

  /*********/


  /*************
   * set default resources 
   *********/

  XtVaSetValues( event_frame,
		 XmNtopAttachment, XmATTACH_WIDGET,
		 XmNtopWidget, main_tab,
		 XmNbottomAttachment, XmATTACH_FORM,
		 XmNleftAttachment, XmATTACH_FORM,	
		 XmNrightAttachment, XmATTACH_FORM,
		 XmNchildType, XmFRAME_WORKAREA_CHILD,
		 XmNshadowType, XmSHADOW_ETCHED_OUT,
		 XmNmarginHeight, 3,
		 XmNmarginWidth, 3,
		 NULL );

  XtVaSetValues( event_form,
		 XmNtopAttachment, XmATTACH_WIDGET,
		 XmNtopWidget, main_tab,
		 XmNbottomAttachment, XmATTACH_FORM,
		 XmNleftAttachment, XmATTACH_FORM,	
		 XmNrightAttachment, XmATTACH_FORM,
		 XmNmarginWidth, 3,
		 XmNmarginHeight, 3,
		 NULL );

  XtVaSetValues( event_list,
		 XmNwidth, 100,
		 XmNselectionPolicy, XmBROWSE_SELECT,
		 XmNlistSizePolicy, XmVARIABLE,
		 XmNscrollingPolicy, XmAUTOMATIC,
                 XmNscrollBarDisplayPolicy, XmAS_NEEDED,
                 XmNscrollBarPlacement, XmBOTTOM_RIGHT, 
		 NULL );

  XtVaSetValues( list_scroll,
		 XmNtopAttachment, XmATTACH_FORM,
		 XmNbottomAttachment, XmATTACH_WIDGET,
		 XmNbottomWidget, event_butt,
		 XmNleftAttachment, XmATTACH_FORM,	
		 XmNrightAttachment, XmATTACH_NONE,
		 NULL );

  XtVaSetValues( event_out,
		 XmNshadowThickness, 1,
		 XmNeditMode, XmMULTI_LINE_EDIT,
		 XmNeditable, FALSE,  /* can't edit */
		 XmNcursorPositionVisible, FALSE, /* no cursor */
		 XmNscrollingPolicy, XmAUTOMATIC,
		 XmNwordWrap, TRUE,
		 NULL );

  XtVaSetValues( out_scroll,
		 XmNtopAttachment, XmATTACH_FORM,
		 XmNbottomAttachment, XmATTACH_FORM,		 
		 XmNleftAttachment, XmATTACH_WIDGET,	
		 XmNleftWidget, list_scroll,		 
		 /*XmNleftAttachment, XmATTACH_FORM.*/
		 XmNrightAttachment, XmATTACH_FORM,
		 NULL );


  xmstr = XmStringCreateLocalized( "Events" );
  XtVaSetValues( event_butt,
		 XmNtopAttachment, XmATTACH_NONE,
		 XmNbottomAttachment, XmATTACH_FORM,
		 XmNrightAttachment, XmATTACH_NONE,	
		 XmNleftAttachment, XmATTACH_FORM,
		 XmNlabelString, xmstr,
		 NULL );

  XtVaSetValues( eventpop,
		 XmNdialogTitle, xmstr,
		 XmNautoUnmanage, FALSE,		 
		 NULL );
  XmStringFree( xmstr );
		 
  XtVaSetValues( eventrc,
		 XmNtopAttachment, XmATTACH_FORM,
		 XmNbottomAttachment, XmATTACH_NONE,
		 XmNrightAttachment, XmATTACH_FORM,	
		 XmNleftAttachment, XmATTACH_FORM,
		 XmNspacing, 0,
		 XmNnumColumns, 3,
		 XmNpacking, XmPACK_COLUMN,
		 XmNresizeHeight, TRUE,
		 XmNresizeWidth, TRUE,
		 NULL );

  
  ok = XmStringCreateSimple( "OK" );
  apply = XmStringCreateSimple( "Apply" );    
  cancel = XmStringCreateSimple( "Cancel" );  
  all = XmStringCreateSimple( "Select All" );
  unall = XmStringCreateSimple( "UnSelect All" );
  XtVaSetValues( ok_butt,
		 XmNlabelString, ok,
		 NULL );
  XtVaSetValues( apply_butt,
		 XmNlabelString, apply,
		 NULL );
  XtVaSetValues( cancel_butt,
		 XmNlabelString, cancel,
		 NULL );
  XtVaSetValues( all_butt, 
		 XmNlabelString, all,
		 NULL );
  XtVaSetValues( unall_butt, 
		 XmNlabelString, unall,
		 NULL );

  /* create a toggle for each event type */
  for( i=2; i<LASTEvent; i++ ){
    xmstr = XmStringCreateSimple( EventTypeToString(i) );
    event_togg[i] = XmCreateToggleButtonGadget(eventrc,"event_button",NULL,0 );
    XtVaSetValues( event_togg[i],
    		   XmNlabelString, xmstr,
		   XmNvisibleWhenOff, FALSE,
		   NULL );
    XmStringFree( xmstr );
    XtManageChild( event_togg[i] );
  }   
  
}
