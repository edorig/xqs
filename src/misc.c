/*****
 * misc.c 
 *****
 * Written By: brett adams (bretta@jove.acs.unt.edu)
 * Description: misc.c -- misc. functions for the xqs prog
 *****
 *****/

#include "xqs.h"

int ErrorCatcher( d, event )
  Display *d;
  XErrorEvent *event;
{
  char err[64], req[64], min[64];;
  
  XGetErrorText( d, event->error_code, err, 64 );
  XGetErrorText( d, event->request_code, req, 64 );
  XGetErrorText( d, event->minor_code, min, 64 );
  printf( "resource id: %lu (0x%x)\n" 
	  "serial: %lu\n"
	  "error: %u, %s\n"
	  "request_code: %u, %s\n"
	  "minor_code: %u, %s\n\n", (unsigned long)event->resourceid, 
	  (unsigned int)event->resourceid, event->serial, event->error_code,
	  err, event->request_code, req, event->minor_code, min);
  /*exit(1);*/
  return 0;
}

int ErrorsToWidget( d, event )
  Display *d;
  XErrorEvent *event;
{
  char err[64], req[64], min[64];;
  
  XGetErrorText( d, event->error_code, err, 64 );
  XGetErrorText( d, event->request_code, req, 64 );
  XGetErrorText( d, event->minor_code, min, 64 );
  XmTextPrintf( err_out, "resource id: %lu (0x%x)\n" 
		"serial: %lu\n"
		"error: %u, %s\n"
		"request_code: %u, %s\n"
		"minor_code: %u, %s\n\n", (unsigned long)event->resourceid, 
		(unsigned int)event->resourceid, event->serial, 
		event->error_code, err, event->request_code, req, 
		event->minor_code, min);
  /*exit(1);*/
  XtManageChild( err_pop );
  return 0;


}
/* taken from dsimple.c which is distributed with several of the X clients */
void Fatal_Error(msg, arg0,arg1,arg2,arg3,arg4,arg5,arg6)
char *msg;
char *arg0, *arg1, *arg2, *arg3, *arg4, *arg5, *arg6;
{
	fflush(stdout);
	fflush(stderr);
	fprintf(stderr, "%s: error: ", "xqs");
	fprintf(stderr, msg, arg0, arg1, arg2, arg3, arg4, arg5, arg6);
	fprintf(stderr, "\n");
	/* exit(1); */
}

/* set the message displayed in the status bar */
void SetStatus( String string )
{
  XmString xmstr;

  xmstr = XmStringCreateSimple( string );
  XtVaSetValues( status_bar, 
		 XmNlabelString, xmstr,
		 NULL );
  XmStringFree( xmstr );
  XmUpdateDisplay( status_bar );
  return;
}

/* clear the status bar */
void ClearStatus(){ SetStatus( "  " ); }
