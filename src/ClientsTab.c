/***** 
 * ClientsTab.c 
 *****
 * Written By: brett adams (bretta@jove.acs.unt.edu)
 * Description: ClientsTab.c -- callbacks and and functions called when the
 *	"Clients" tab becomes active, most of the code here was incorporated
 *	from adapted from xlsclients, in most case i just redirected the output
 *	to an XmText widget
 *****
 ****/
/*
 *==================
 * xlsclients 
 *=================
 */
/*
 * $XConsortium: xlsclients.c,v 1.6 90/12/18 15:01:56 gildea Exp $
 *
 * Copyright 1989 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Jim Fulton, MIT X Consortium
 */

#include "xqs.h"
#include "Prefs.h"

void xlsclients( Display *dpy, Bool all_screens, Bool verbose, int maxcmdlen );
void lookat( Display *dpy, Window w, Bool verbose, int maxcmdlen );
void print_client_properties( Display *dpy, Window w, Bool verbose, 
			      int maxcmdlen);
void print_text_field( Display *dpy, char *s, XTextProperty *tp);
int print_quoted_word( char *s, int maxlen );
void unknown( Display *dpy, Atom actual_type, int actual_format );
int isascii(char);

Widget clients_out;

void
ClientsTabActive()
{  
  int maxcmdlen = 15000;
  Bool all_screens, more;

  
  XtManageChild( clients_frame );
  if( info_dpy == NULL ) return;

  XmTextClear( clients_out );

  SetWidgetCursor( tab_form, XC_watch );
  SetStatus( "Retrieving client info..." );

  all_screens = pref_db.cliPrefs[0].value;
  more = pref_db.cliPrefs[1].value;

  xlsclients( info_dpy, all_screens, more, maxcmdlen );
  
  if( XmTextGetLastPosition(clients_out) == 0 )
       SetStatus( "No clients found" );
  else {
       ClearStatus();
       XtVaSetValues( clients_out, XmNcursorPosition, 0, NULL );
  }
    
  UnsetWidgetCursor( tab_form );

}

void InitClientsTab()
{
  Widget clients_form, out_scroll;

  clients_frame = XtVaCreateWidget( "clients_frame", xmFrameWidgetClass, 
				    tab_form,
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

  clients_form = XtVaCreateManagedWidget( "clients_form", xmFormWidgetClass, 
					  clients_frame,
					  XmNtopAttachment, XmATTACH_FORM,
					  XmNbottomAttachment, XmATTACH_FORM,
					  XmNleftAttachment, XmATTACH_FORM,
					  XmNrightAttachment, XmATTACH_FORM,
					  NULL );
  
  clients_out = XmCreateScrolledText( clients_form, "clients_out", NULL, 0 );
  XtManageChild( clients_out );

  out_scroll = XtParent( clients_out );

  XtVaSetValues( out_scroll,
		 XmNtopAttachment, XmATTACH_FORM,
		 XmNbottomAttachment, XmATTACH_FORM,
		 XmNleftAttachment, XmATTACH_FORM,	
		 XmNrightAttachment, XmATTACH_FORM,
		 NULL );

  XtVaSetValues( clients_out,
		 XmNshadowThickness, 0,		 
		 XmNwordWrap, TRUE,
		 XmNeditMode, XmMULTI_LINE_EDIT,
		 XmNeditable, FALSE,  			/* can't edit */
		 XmNcursorPositionVisible, FALSE, 	/* no cursor */
		 XmNautoShowCursorPosition, FALSE,
		 XmNscrollingPolicy, XmAUTOMATIC,
		 NULL );

}



void xlsclients( dpy, all_screens, verbose, maxcmdlen )
  Display *dpy;
  Bool all_screens;
  Bool verbose;
  int maxcmdlen;
{
  int i;
  
  if (all_screens) {
    for (i = 0; i < ScreenCount(dpy); i++) {
      lookat (dpy, RootWindow(dpy,i), verbose, maxcmdlen);
    }
  } else {
    lookat (dpy, DefaultRootWindow(dpy), verbose, maxcmdlen);
  }

}

void lookat (dpy, root, verbose, maxcmdlen)
    Display *dpy;
    Window root;
    Bool verbose;
    int maxcmdlen;
{
    Window dummy, *children = NULL, client;
    unsigned int i, nchildren = 0;

    /*
     * clients are not allowed to stomp on the root and ICCCM doesn't yet
     * say anything about window managers putting stuff there; but, try
     * anyway.
     */
    print_client_properties (dpy, root, verbose, maxcmdlen);

    /*
     * then, get the list of windows
     */
    if (!XQueryTree (dpy, root, &dummy, &dummy, &children, &nchildren)) {
	return;
    }

    for (i = 0; i < nchildren; i++) {
	client = XmuClientWindow (dpy, children[i]);
	if (client != None)
	  print_client_properties (dpy, client, verbose, maxcmdlen);
    }
}

static char *Nil = "(nil)";

void print_client_properties (dpy, w, verbose, maxcmdlen)
    Display *dpy;
    Window w;
    Bool verbose;
    int maxcmdlen;
{
    char **cliargv = NULL;
    int i, cliargc;
    XTextProperty nametp, machtp, tp;
    int charsleft = maxcmdlen;

    /*
     * get the WM_MACHINE and WM_COMMAND list of strings
     */
    if (!XGetWMClientMachine (dpy, w, &machtp)) {
	machtp.value = NULL;
	machtp.encoding = None;
    }

    if (!XGetCommand (dpy, w, &cliargv, &cliargc)) {
	if (machtp.value) XFree ((char *) machtp.value);
	return;
    }

    /*
     * do header information
     */
   
	XmTextPrintf( clients_out, "Window 0x%lx:\n", w);
	print_text_field (dpy, "  Machine:  ", &machtp);
	if (XGetWMName (dpy, w, &nametp)) {
	    print_text_field (dpy, "  Name:  ", &nametp);
	    if (nametp.value) XFree ((char *) nametp.value);
	}

    if (machtp.value) XFree ((char *) machtp.value);



    /*
     * do the command
     */
    XmTextPrintf ( clients_out, "  Command:  ");

    for (i = 0; i < cliargc && charsleft > 0; ) {
	charsleft -= print_quoted_word (cliargv[i], charsleft);
	i++;
	if (i < cliargc  &&  charsleft > 0) {
	    /* putchar (' '); */ 
	  XmTextAppend( clients_out, " " );
	  charsleft--;
	}
    }
    /* putchar ('\n'); */
    XmTextNewLine( clients_out );
    XFreeStringList (cliargv);

    if (verbose) {
	if (XGetWMIconName (dpy, w, &tp)) {
	    print_text_field (dpy, "  Icon Name:  ", &tp);
	    if (tp.value) XFree ((char *) tp.value);
	}
    }

    /*
     * do trailer information
     */
    if (verbose) {
	XClassHint clh;
	if (XGetClassHint (dpy, w, &clh)) {
	    XmTextPrintf ( clients_out, "  Instance/Class:  %s/%s",
			   clh.res_name ? clh.res_name : Nil,
			   clh.res_class ? clh.res_class : Nil);
	    if (clh.res_name) XFree (clh.res_name);
	    if (clh.res_class) XFree (clh.res_class);
	    /* putchar ('\n'); */
	    XmTextAppend( clients_out, "\n" );
	}
    }

    XmTextNewLine(clients_out);

}


void print_text_field (dpy, s, tp)
    Display *dpy;
    char *s;
    XTextProperty *tp;
{
    if (tp->encoding == None || tp->format == 0) {
	/* printf ("''"); */
	XmTextAppend( clients_out, "''" );
	return;
    }

    if (s) XmTextPrintf ( clients_out, "%s", s);
    if (tp->encoding == XA_STRING && tp->format == 8) {
	XmTextPrintf ( clients_out, "%s", tp->value ? (char *) tp->value : Nil);
    } else {
	unknown (dpy, tp->encoding, tp->format);
    }
    if (s) /* putchar ('\n'); */ XmTextNewLine( clients_out );
}

/* returns the number of characters printed */
int
print_quoted_word (s, maxlen)
    char *s;
    int maxlen;			/* max number of chars we can print */
{
    char *cp;
    Bool need_quote = False, in_quote = False;
    char quote_char = '\'', other_quote = '"';
    int charsprinted = 0;
    char buff[64];
    

    /*
     * walk down seeing whether or not we need to quote
     */
    for (cp = s; *cp; cp++) {

	if (! ((isascii(*cp) && isalnum(*cp)) || 
	       (*cp == '-' || *cp == '_' || *cp == '.' || *cp == '+' ||
		*cp == '/' || *cp == '=' || *cp == ':' || *cp == ','))) {
	    need_quote = True;
	    break;
	}
    }

    /*
     * write out the string: if we hit a quote, then close any previous quote,
     * emit the other quote, swap quotes and continue on.
     */
    in_quote = need_quote;
    if (need_quote) {
	/* putchar (quote_char); */
      sprintf( buff, "%c", quote_char );
      XmTextAppend( clients_out, buff );
      charsprinted++; maxlen--;
    }
    for (cp = s; *cp && maxlen>0; cp++) {
	if (*cp == quote_char) {
	    if (in_quote) {
	      /*putchar (quote_char);*/
	        sprintf( buff, "%c", quote_char );
	        XmTextAppend( clients_out, buff );
		charsprinted++; maxlen--;
	    }
	    /*putchar (other_quote);*/
	    sprintf( buff, "%c", quote_char );
	    XmTextAppend( clients_out, buff );
	    charsprinted++; maxlen--;
	    { 
		char tmp = other_quote; 
		other_quote = quote_char; quote_char = tmp;
	    }
	    in_quote = True;
	}
	/* putchar (*cp); */
	sprintf( buff, "%c", *cp );
	XmTextAppend( clients_out, buff );
	charsprinted++; maxlen--;
    }
    /* close the quote if we opened one and if we printed the whole string */
    if (in_quote && maxlen>0) {
	/* putchar (quote_char); */
        sprintf( buff, "%c", quote_char );
        XmTextAppend( clients_out, buff );
	charsprinted++; maxlen--;
    }

    return charsprinted;
}

void unknown (dpy, actual_type, actual_format)
    Display *dpy;
    Atom actual_type;
    int actual_format;
{
    char *s;

    XmTextPrintf ( clients_out, "<unknown type ");
    if (actual_type == None) XmTextPrintf ( clients_out, "None");
    else if ((s = XGetAtomName (dpy, actual_type)) != NULL) {
	fputs (s, stdout);
	XFree (s);
    } else {
	fputs (Nil, stdout);
    }
    XmTextPrintf(clients_out, " (%ld) or format %d>", actual_type, actual_format);
}

