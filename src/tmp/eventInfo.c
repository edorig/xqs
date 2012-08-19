/*****
 * eventInfo.c
 ****
 * Written By: brett adams (bretta@jove.acs.unt.edu)
 * Description: eventInfo.c 
 *****
 ****/

#include "xqs.h"

void PutEventInfo( Widget w, XEvent ev);
void PutEventCommonInfo();
void PutButtonEventInfo();
void PutCirculateEventInfo();
void PutCirculateRequestEventInfo();
void PutClientMessageEventInfo();
void PutColormapEventInfo();
void PutConfigureEventInfo();
void PutConfigureRequestEventInfo();
void PutCreateWindowEventInfo();
void PutDestroyWindowEventInfo();
void PutCrossingEventInfo();
void PutExposeEventInfo();
void PutFocusChangeEventInfo();
void PutGraphicsExposeEventInfo();
void PutNoExposeEventInfo();
void PutGravityEventInfo();
void PutKeymapEventInfo();
void PutKeyEventInfo();
void PutMapEventInfo();
void PutMapRequestEventInfo();
void PutMotionEventInfo();
void PutPropertyEventInfo();
void PutReparentEventInfo();
void PutResizeRequestEventInfo();
void PutSelectionClearEventInfo();
void PutSelectionEventInfo();
void PutSelectionRequestEventInfo();
void PutUnmapEventInfo();
void PutVisibilityEventInfo();

extern char *Lookup(int code, binding *table);

static binding _circulate_place[] = {
  { PlaceOnTop, "PlaceOnTop" },
  { PlaceOnBottom, "PlaceOnBottom" },
  { 0, 0 }
};

void PutEventCommonInfo( Widget w, XAnyEvent event )
{

  XmTextPrintf( w, "%s: \n"
		"serial # : %lu\n"
		"send event: %s\n"
		"display name: %s\n", EventTypeToString(event.type),
		event.serial, event.send_event ? "TRUE" : "FALSE",
		DisplayString(event.display));

  /* put window id if not a GraphicsExpose, NoExpose, SelecionNotfiy/Request*/ 

}


void PutButtonEventInfo( Widget w, XButtonEvent event )
{
  XmTextPrintf( w, "event window: 0x%x\n"
		"root window: 0x%x\n"
		"child window: 0x%x\n"
		"time: %x\n"
		"coordinates relative to window; (%d,%d)\n"
		"coordinates relative to root: (%d,%d)\n"
		"state: 0x%x\n"
		"button: %d\n"
		"same screen: %s\n", (unsigned int)event.window,
		(unsigned int)event.root, (unsigned int)event.subwindow,
		(unsigned int)event.time, event.x, event.y, event.x_root,
		event.y_root, event.state, (int)event.button,
		event.same_screen ? "TRUE" : "FALSE" );

  
}

void PutCirculateEventInfo( Widget w, XCirculateEvent event )
{  
  XmTextPrintf( w, "event window: 0x%x\n"
		"window: 0x%x\n"
		"place: ", (unsigned int)event.event,
		(unsigned int)event.window,
		Lookup(event.place,_circulate_place)  );
}

void PutCirculateRequestEventInfo( Widget w, XCirculateRequestEvent event )
{  
  XmTextPrintf( w, "parent: 0x%x\n"
		"window: 0x%x\n"
		"place: ", (unsigned int)event.parent,
		(unsigned int)event.window,
		Lookup(event.place,_circulate_place)  );
}

void PutClientMessageEventInfo( Widget w, XClientMessageEvent event )
{
  
}

void PutColormapEventInfo( Widget w, XColormapEvent event )
{
  
}

void PutConfigureEventInfo( Widget w, XConfigureEvent event )
{
}
void PutConfigureRequestEventInfo( Widget w, XConfigureRequestEvent event )
{
}
void PutCreateWindowEventInfo( Widget w, XCreateWindowEvent event )
{
}
void PutDestroyWindowEventInfo( Widget w, XDestroyWindowEvent event )
{
}
void PutCrossingEventInfo( Widget w, XCrossingEvent event )
{
}
void PutExposeEventInfo( Widget w, XExposeEvent event )
{
}
void PutFocusChangeEventInfo( Widget w, XFocusChangeEvent event )
{
}
void PutGraphicsExposeEventInfo( Widget w, XGraphicsExposeEvent event )
{
}
void PutNoExposeEventInfo( Widget w, XNoExposeEvent event )
{
}
void PutGravityEventInfo( Widget w, XGravityEvent event )
{
}
void PutKeymapEventInfo( Widget w, XKeymapEvent event )
{
}
void PutKeyEventInfo( Widget w, XKeyEvent event )
{
}
void PutMapEventInfo( Widget w, XMapEvent event )
{
}
void PutMapRequestEventInfo( Widget w, XMapRequestEvent event )
{
}
void PutMotionEventInfo( Widget w, XMotionEvent event )
{
}
void PutPropertyEventInfo( Widget w, XPropertyEvent event )
{
}
void PutReparentEventInfo( Widget w, XReparentEvent event )
{
}
void PutResizeRequestEventInfo( Widget w, XResizeRequestEvent event )
{
}
void PutSelectionClearEventInfo( Widget w, XSelectionClearEvent event )
{
}
void PutSelectionEventInfo( Widget w, XSelectionEvent event )
{
}
 void PutSelectionRequestEventInfo( Widget w, XSelectionRequestEvent event )
{
}
void PutUnmapEventInfo( Widget w, XUnmapEvent event )
{
}
void PutVisibilityEventInfo( Widget w, XVisibilityEvent event )
{
}


void PutEventInfo( Widget wid, XEvent event )
{
  
  PutEventCommonInfo( wid, event.xany );

  switch( event.type ){
  case ButtonPress: case ButtonRelease:
    PutButtonEventInfo( wid, event.xbutton );
    break;
  case CirculateNotify:
    PutCirculateEventInfo( wid, event.xcirculate );
    break;
  case CirculateRequest:
    PutCirculateRequestEventInfo( wid, event.xcirculaterequest );
    break;
  case ClientMessage:
    PutClientMessageEventInfo( wid, event.xclient );
    break;
  case ColormapNotify:
    PutColormapEventInfo( wid, event.xcolormap );
    break;
  case ConfigureNotify:
    PutConfigureEventInfo( wid, event.xconfigure );
    break;
  case ConfigureRequest:
    PutConfigureRequestEventInfo( wid, event.xconfigurerequest );
    break;
  case CreateNotify:
    PutCreateWindowEventInfo( wid, event.xcreatewindow );
    break;
  case DestroyNotify:
    PutDestroyWindowEventInfo( wid, event.xdestroywindow );
    break;
  case EnterNotify: case LeaveNotify:
    PutCrossingEventInfo( wid, event.xcrossing );
    break;
  case Expose:
    PutExposeEventInfo( wid, event.xexpose );
    break;
  case FocusIn: case FocusOut:
    PutFocusChangeEventInfo( wid, event.xfocus );
    break;
  case GraphicsExpose:
    PutGraphicsExposeEventInfo( wid, event.xgraphicsexpose );
    break;
  case NoExpose:
    PutNoExposeEventInfo( wid, event.xnoexpose );
    break;
  case GravityNotify:
    PutGravityEventInfo( wid, event.xgravity );
    break;
  case KeymapNotify:
    PutKeymapEventInfo( wid, event.xkeymap );
    break;
  case KeyPress: case KeyRelease:
    PutKeyEventInfo( wid, event.xkey );
    break;
  case MapNotify: 
    PutMapEventInfo( wid, event.xmap );
    break;
  case MapRequest: 
    PutMapRequestEventInfo( wid, event.xmaprequest );
    break;
  case MotionNotify:
    PutMotionEventInfo( wid, event.xmotion );
    break;
  case PropertyNotify:
    PutPropertyEventInfo( wid, event.xproperty );
    break;
  case ReparentNotify:
    PutReparentEventInfo( wid, event.xreparent );
    break;
  case ResizeRequest:
    PutResizeRequestEventInfo( wid, event.xresizerequest );
    break;
  case SelectionClear:
    PutSelectionClearEventInfo( wid, event.xselectionclear );
    break;
  case SelectionNotify:
    PutSelectionEventInfo( wid, event.xselection );
    break;
  case SelectionRequest:
    PutSelectionRequestEventInfo( wid, event.xselectionrequest );
    break;
  case UnmapNotify:
    PutUnmapEventInfo( wid, event.xunmap );
    break;
  case VisibilityNotify:
    PutVisibilityEventInfo( wid, event.xvisibility );
    break;
  default:
    XmTextAppend( wid, "\n** Unknown Event Type **\n" );
    /*SetStatus( "Unknow Event Type" );*/
    /*Beep(); */
    break;
  }

}/* end PutEventInfo() */

