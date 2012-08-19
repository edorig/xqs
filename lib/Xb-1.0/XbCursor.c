/****
 * XbCursor.c
 ****
 * Written By: brett adams( bretta@jove.acs.unt.edu )
 * Description: helper and utility funtions to set a defined cursor shape
 * 	for either a window or a widget and alternatively unset the cursor
 * 	for the window or widget
 ****
 ****/

#include "Xb.h"

/* set a defined cursor shape for the given widget  */
void SetWidgetCursor( Widget widget, int cursor_shape )
{
  Display *dpy;
  Window window;
  Cursor cursor;

  dpy = XtDisplay( widget );
  window = XtWindow( widget );
  
  cursor = XCreateFontCursor( dpy, cursor_shape );
  XDefineCursor( dpy, window, cursor );

  /* if( cursor ) XFree( (char *)cursor ); */
}

/* unset the cursor for a widget */
void UnsetWidgetCursor( Widget widget )
{
  Display *dpy;
  Window w;

  dpy = XtDisplay( widget );
  w = XtWindow( widget );

  XUndefineCursor( dpy, w );
}

/* set a defined cursor shape  for a window */
void SetWindowCursor( Display *dpy, Window win, int cursor_shape )
{
  Cursor cursor;
  cursor = XCreateFontCursor( dpy, cursor_shape );
  XDefineCursor( dpy, win, cursor );

  /*if( cursor ) XFree( (char *)cursor );*/
}

/* unset the cursor for q window */
void UnsetWindowCursor( Display *dpy, Window win ) 
{  XUndefineCursor( dpy, win ); }
