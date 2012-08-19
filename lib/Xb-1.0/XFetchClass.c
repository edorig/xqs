
#include "Xb.h"
#include <X11/Xatom.h>

Bool
XFetchClass( Display *display, 
	     Window w, 
	     char **class_name_return )
{
  int format;
  unsigned long n_items, bytes_after_return;
  unsigned char *data;
  Atom actual_type;

  /*printf( "XFetchClass() called\n" );*/

  XGetWindowProperty( display, w, XA_WM_CLASS, 0, 2, FALSE, XA_STRING,
		      &actual_type, &format, &n_items, 
		      &bytes_after_return, &data );
		      		    
  if( format == 8 && actual_type == XA_STRING )
    (char *)class_name_return = XtNewString( (char *)data );
  else 
    class_name_return = NULL;
  
  return TRUE;


}/* end XFetchClass() */

