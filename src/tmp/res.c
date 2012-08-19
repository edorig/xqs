#include "xqs.h"

void main( int argc, char **argv)
{
  XtAppContext app_context;
  Widget appshell;

  appshell = XtAppInitialize( &app_context, argv[0], NULL, 0, &argc, argv,
			      NULL, NULL, 0 );

  printf( "%s\n", XResourceManagerString( XtDisplay(appshell) ) );;
  
}
