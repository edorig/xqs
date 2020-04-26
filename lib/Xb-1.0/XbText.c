/****
 * XmbText.c
 ****
 * Written By: brett adams( bretta@jove.acs.unt.edu )
 * Description: helper and utility funtions for use with an XmText widget,
 * 	these funtions are for use with motif widgets only
 ****
 ***/

#include "Xb.h"

void XmTextPrintf( Widget wid, ... )
{  
  char *format;
  /* Was 2048, too short for CDE's dtwm properties */ 
  char str[16384];
  va_list args;
  int n;

  va_start(args, wid);

  if( XtClass(wid) != xmTextWidgetClass )
       XtError( "XmTextPrintf() --  must use an XmText widget\n" );

  format = va_arg(args, char * );
  n = vsprintf( str, format, args );
  if( n == -1 )
       XtError( "XmTextPrintf() --  could not create string\n" );
  
  str[ n ] = '\0';
  XmTextAppend( wid, str );
  
  va_end(args);

}

void XmStringPrintf( XmString xmstr, ... )
{
  char *format;
  char str[2048];
  va_list args;

  va_start(args, xmstr);

  format = va_arg(args, char * );
  vsprintf( str, format, args );

  xmstr = XmStringCreateLocalized( str );
  va_end(args);
}


/* append text to the end of an XmText widget */
void XmTextAppend( Widget widget, char *buffer )
{  
  int last = XmTextGetLastPosition( widget );  
  XmTextReplace( widget, last, last, buffer );  
} 

/* append a newline to the end of an XmText widget */
void XmTextNewLine( Widget w ){ XmTextAppend( w, "\n" ); }
void XmTextClear( Widget w ){ XmTextSetString( w, "" ); }
