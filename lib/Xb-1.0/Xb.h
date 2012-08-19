/****
 * Xb.h 
 *****
 * Written By: brett adams(bretta@jove.acs.unt.edu)
 * Description: utility, convenience and misc. function for X programming
 *****
 *****/

#ifndef _XB_H
#define _XB_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/Text.h>
#include <stdarg.h>

#if NeedFunctionPrototypes || defined(__cplusplus)
  extern void SelectInputTree( Display *dpy, Window w, EventMask mask );
  extern char * EventTypeToString( int event_type );
  extern char *NotifyDetailToString( int notify_detail );
  extern char * NotifyModeToString( int notify_mode );
  /*  Bool XFetchClass( Display *dpy, Window w, char **class_name_return ); */

  extern void XmTextPrintf( Widget w, ... );
  extern void XmTextAppend( Widget w, char *buffer );
  extern void XmTextNewLine( Widget w );
  extern void XmTextClear( Widget w );

  extern void SetWidgetCursor( Widget w, int cursor_shape );
  extern void UnsetWidgetCursor( Widget w );
  extern void SetWindowCursor( Display *dpy, Window w, int cursor_shape );
  extern void UnsetWindowCursor( Display *dpy, Window w ) ;

#else
  extern void SelectInputTree();
  extern char * EventTypeToString();
  extern char *NotifyDetailToString();
  extern char * NotifyModeToString();
  /* Bool XFetchClass( Display *dpy, Window w, char **class_name_return ); */
  
  extern void XmTextPrintf();
  extern void XmTextAppend();
  extern void XmTextNewLine();
  extern void XmTextClear();
  
  extern void SetWidgetCursor();
  extern void UnsetWidgetCursor();
  extern void SetWindowCursor();
  extern void UnsetWindowCursor();
#endif


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _XB_H */
