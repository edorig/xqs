/***** 
 * AccessTab.c 
 *****
 * Written By: brett adams (bretta@jove.acs.unt.edu)
 * Description: AccessTab.c -- callbacks and and functions used to control
 *	the Access tab of the xqs program
 *****
 ****/

#include "xqs.h"


Widget access_out;

static binding _family_types[] = {
  { FamilyInternet, "TCP/IP" },
  { FamilyDECnet, "DECnet" },
  { FamilyChaos, "Chaos(unknown)" },
  { 0, "(unknown)" }
};

void AccessTabActive()
{
  int 	nhosts, i;
  Bool 	access_enabled;
  XHostAddress	*xhosts;
  struct hostent *hp;
  struct in_addr in; 

  XtManageChild( access_frame );
  if( info_dpy == NULL ) return;

  XmTextClear( access_out );
  SetStatus( "Retrieving host access list..." );
  SetWidgetCursor( tab_form, XC_watch );
  /* get the access control list */
  xhosts = XListHosts( info_dpy, &nhosts, &access_enabled );
      /* On Linux, there is always one allowed host, localhost, connected by AF_UNIX socket to the server. So we we need to print the allowed network hosts only if nhosts>1.  The first one is host 0, the last one is host nhost-1. */ 
  if( access_enabled == TRUE && nhosts > 1 ) {
    for( i=0; i<nhosts-1; i++ ) {	    
	 /* need to output the net number if the hostname is not found */

      /* Obsolete, getaddrinfo(3) should be used */ 
	  hp = gethostbyaddr((void *)xhosts[i].address, (socklen_t) xhosts[i].length, AF_INET );
	 
	 if( !hp->h_name )  
	   memcpy( &in.s_addr, hp->h_addr_list[0], sizeof(in.s_addr) );
		 	  
	    XmTextPrintf( access_out, "%s      %s\n",			  
			  hp->h_name ? hp->h_name : inet_ntoa(in),
			  Lookup(xhosts[i].family, _family_types) ); 

	    
	}
  }
  else if( access_enabled == TRUE && nhosts == 1 ) {
           XmTextAppend( access_out, "\n-- No hosts in access list! --\n" );
  } else /* access_enabled == FALSE <all host can connect to display> */
       XmTextSetString( access_out,
			"Access control disabled, all hosts welcome.\n" );
  
  UnsetWidgetCursor( tab_form );
  ClearStatus();
  
  XFree(xhosts); 
}


void InitAccessTab()
{
  XmString xmstr;
  Widget host_label, family_label, rowcol, access_form;

  /*
   * widget creation and management
   */
  access_frame = XmCreateFrame( tab_form, "access_form", NULL, 0 );
  
  access_form = XmCreateForm( access_frame, "access_form", NULL , 0 );
  XtManageChild( access_form );

  rowcol = XmCreateRowColumn( access_form, "rowcol", NULL, 0 );
  XtManageChild( rowcol );

  host_label = XmCreateLabelGadget( rowcol, "host_label", NULL, 0 );
  XtManageChild( host_label );
  
  family_label = XmCreateLabelGadget( rowcol, "family_label", NULL, 0 );
  XtManageChild( family_label );

  access_out = XmCreateText( access_form, "access_out", NULL, 0 );
  XtManageChild( access_out );

  /*
   * widget resources
   */  
  XtVaSetValues( access_frame,
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

  XtVaSetValues( access_form,
		 XmNtopAttachment, XmATTACH_FORM,
		 XmNbottomAttachment, XmATTACH_FORM,
		 XmNleftAttachment, XmATTACH_FORM,	
		 XmNrightAttachment, XmATTACH_FORM,
		 NULL );

  XtVaSetValues( rowcol,
		 XmNtopAttachment, XmATTACH_FORM,
		 XmNbottomAttachment, XmATTACH_NONE,
		 XmNrightAttachment, XmATTACH_FORM,	
		 XmNleftAttachment, XmATTACH_FORM,
		 XmNnumColumns, 2,
		 XmNpacking, XmPACK_COLUMN,
		 XmNresizeHeight, TRUE,
		 XmNresizeWidth, TRUE,		 
		 NULL );

  xmstr = XmStringCreateSimple( "Host" );
  XtVaSetValues( host_label, 
		 XmNlabelString, xmstr,
		 XmNtopAttachment, XmATTACH_FORM,
		 XmNbottomAttachment, XmATTACH_NONE,
		 XmNleftAttachment, XmATTACH_FORM,	
		 XmNrightAttachment, XmATTACH_WIDGET,
		 XmNrightWidget, family_label,
		 XmNwidth, 30,
		 NULL );
  XmStringFree( xmstr );

  xmstr = XmStringCreateSimple( "Network Family" );
  XtVaSetValues( family_label, 
		 XmNlabelString, xmstr,
		 XmNtopAttachment, XmATTACH_FORM,
		 XmNbottomAttachment, XmATTACH_NONE,
		 XmNrightAttachment, XmATTACH_FORM,
		 XmNleftAttachment, XmATTACH_NONE,
		 XmNwidth, 30,
		 NULL );
  XmStringFree( xmstr );

  XtVaSetValues( access_out,
		 XmNtopAttachment, XmATTACH_WIDGET,
		 XmNtopWidget, rowcol,
		 XmNbottomAttachment, XmATTACH_FORM,
		 XmNleftAttachment, XmATTACH_FORM,
		 XmNrightAttachment, XmATTACH_FORM,		 
		 XmNshadowThickness, 0,
		 XmNeditMode, XmMULTI_LINE_EDIT,
		 XmNeditable, FALSE,  			/* can't edit */
		 XmNcursorPositionVisible, FALSE, 	/* no cursor */
		 XmNscrollingPolicy, XmAUTOMATIC,
		 XmNwordWrap, TRUE,
		 NULL );

}
