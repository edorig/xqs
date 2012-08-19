/***** 
 * DisplayTab.c 
 *****
 * Written By: brett adams (bretta@jove.acs.unt.edu)
 * Description: DisplayTab.c --  callbacks and functions used to control the
 *	Display tab of the xqs program, most of the code here was derived
 *	xdpyinfo, in most case i just redirected the output to an
 *	XmText widget
 *****
 ****/

/*
 *======================
 *  xdpyinfo
 *=====================
 */
/*
 * Copyright 1988 by the Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided 
 * that the above copyright notice appear in all copies and that both that 
 * copyright notice and this permission notice appear in supporting 
 * documentation, and that the name of M.I.T. not be used in advertising
 * or publicity pertaining to distribution of the software without specific, 
 * written prior permission. M.I.T. makes no representations about the 
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * xdpyinfo Author:  Jim Fulton, MIT X Consortium
 */


#include "xqs.h"
#include "Prefs.h"

Widget display_out;

void xdpyinfo( Display *dpy, Bool, Bool, Bool );
void print_display_info( Display *dpy );
void print_extension_info( Display *dpy );
void print_screen_info( Display *dpy, int scr);
void print_multibuf_info( Display *dpy, int scr);
void print_visual_info ( XVisualInfo *vip);
int print_event_mask( char *buf, int lastcol, int indent, long mask);


void DisplayTabActive()
{
  Bool general, all_screens, multibuf;

  XtManageChild( display_frame );
  if( info_dpy == NULL )  return;

  XmTextClear( display_out );
  
  SetStatus( "Retrieving Display information..." );
	       
  general = pref_db.dpyPrefs[0].value;
  all_screens = pref_db.dpyPrefs[1].value;
  multibuf = pref_db.dpyPrefs[2].value;

  XmTextPrintf( display_out, "name of display:    %s\n",
		DisplayString (info_dpy)); 

  xdpyinfo( info_dpy, general, all_screens, multibuf );
  
  /* set the cursor to the top of the text box */
  XtVaSetValues( display_out, XmNcursorPosition, 0, NULL );
  ClearStatus();

  XtSetKeyboardFocus( display_out, None );

}

void
InitDisplayTab()
{
  Widget display_scroll;
  
  /*printf( "called InitDisplayTab()\n" );*/

  display_frame = XmCreateFrame( tab_form, "display_frame", NULL, 0 );

  /* create the text out box */
  display_out = XmCreateScrolledText( display_frame, "display_out", NULL, 0 );
  XtManageChild( display_out );
  
  /* get the widget id of the scroll bar */
  display_scroll = XtParent( display_out );

  XtVaSetValues( display_frame,
		 XmNtopAttachment, XmATTACH_WIDGET,
		 XmNtopWidget, main_tab,
		 XmNbottomAttachment, XmATTACH_FORM,
		 XmNleftAttachment, XmATTACH_FORM,	
		 XmNrightAttachment, XmATTACH_FORM,
		 XmNshadowType, XmSHADOW_ETCHED_OUT,
		 XmNmarginHeight, 3,
		 XmNmarginWidth, 3,
		 NULL );

  XtVaSetValues( display_scroll,
		 XmNtopAttachment, XmATTACH_FORM,
		 XmNrightAttachment, XmATTACH_FORM,
		 XmNleftAttachment, XmATTACH_FORM,
		 XmNbottomAttachment, XmATTACH_FORM,
		 XmNscrollBarPlacement, XmBOTTOM_RIGHT,
		 XmNchildType, XmFRAME_WORKAREA_CHILD,
		 NULL );

  XtVaSetValues( display_out,
		 XmNshadowThickness, 0,		 
		 XmNeditMode, XmMULTI_LINE_EDIT,
		 XmNeditable, FALSE,  			/* can't edit */
		 XmNcursorPositionVisible, FALSE, 	/* no cursor */
		 XmNscrollingPolicy, XmAUTOMATIC,
		 XmNwordWrap, TRUE,
		 NULL );
  
}

void xdpyinfo( dpy, dpy_info, screen_info, multibuf_info )
  Display *dpy;
  Bool dpy_info;
  Bool screen_info;
  Bool multibuf_info;
{
  int i;
  Bool multibuf = False;
  int mbuf_event_base, mbuf_error_base;

  if (XmbufQueryExtension (info_dpy, &mbuf_event_base, &mbuf_error_base))
    multibuf = True;

  if( dpy_info )
    print_display_info (dpy);
  for (i = 0; i < ScreenCount (dpy); i++) {
    if( screen_info )
      print_screen_info (dpy, i);
    if (multibuf && multibuf_info)
      print_multibuf_info (dpy, i);
  }

}


void print_display_info( dpy )
  Display *dpy;
{  
  char dummybuf[40];
  char *cp;
  int minkeycode, maxkeycode;
  int i, n;
  XPixmapFormatValues *pmf;
  Window focuswin;
  int focusrevert;

  /* 
  XmTextPrintf( display_out, "name of display:    %s\n", DisplayString (dpy)); 
  */
  XmTextPrintf( display_out, "version number:    %d.%d\n",
		ProtocolVersion (dpy), ProtocolRevision (dpy));
  XmTextPrintf( display_out, "vendor string:    %s\n", ServerVendor (dpy));
  XmTextPrintf( display_out, "vendor release number:    %d\n", 
		VendorRelease (dpy));
  XmTextPrintf( display_out, "maximum request size:  %ld bytes\n", 
		XMaxRequestSize (dpy) * 4);
  XmTextPrintf( display_out, "motion buffer size:  %lu\n", 
		XDisplayMotionBufferSize (dpy));

  switch (BitmapBitOrder (dpy)) {
  case LSBFirst:    cp = "LSBFirst"; break;
  case MSBFirst:    cp = "MSBFirst"; break;
  default:    
    sprintf (dummybuf, "unknown order %d", BitmapBitOrder (dpy));
    cp = dummybuf;
    break;
  }
  XmTextPrintf( display_out,"bitmap unit, bit order, padding:    %d, %s, %d\n",
		BitmapUnit (dpy), cp, BitmapPad (dpy));

  switch (ImageByteOrder (dpy)) {
  case LSBFirst:    cp = "LSBFirst"; break;
  case MSBFirst:    cp = "MSBFirst"; break;
  default:    
    sprintf (dummybuf, "unknown order %d", ImageByteOrder (dpy));
    cp = dummybuf;
    break;
  }
  XmTextPrintf( display_out, "image byte order:    %s\n", cp);

  pmf = XListPixmapFormats (dpy, &n);  
  XmTextPrintf( display_out, "number of supported pixmap formats:    %d\n", n);
  if (pmf) {
    XmTextPrintf( display_out, "supported pixmap formats:\n");
    for (i = 0; i < n; i++) {
      XmTextPrintf( display_out,
		    "    depth %d, bits_per_pixel %d, scanline_pad %d\n",
		    pmf[i].depth, pmf[i].bits_per_pixel, pmf[i].scanline_pad);
    }
    XFree ((char *) pmf);
  }

  /*
   * when we get interfaces to the PixmapFormat stuff, insert code here
   */

  XDisplayKeycodes (dpy, &minkeycode, &maxkeycode);
  XmTextPrintf( display_out, "keycode range:    minimum %d, maximum %d\n",
		minkeycode, maxkeycode);

  XGetInputFocus (dpy, &focuswin, &focusrevert);
  XmTextPrintf( display_out, "focus:  ");
  switch (focuswin) {
  case PointerRoot:
    XmTextPrintf( display_out, "PointerRoot\n");
    break;
  case None:
    XmTextPrintf( display_out, "None\n");
    break;
  default:
    XmTextPrintf( display_out, "window 0x%lx, revert to ", focuswin);
    switch (focusrevert) {
    case RevertToParent:
      XmTextPrintf( display_out, "Parent\n");
      break;
    case RevertToNone:
      XmTextPrintf( display_out, "None\n");
      break;
    case RevertToPointerRoot:
      XmTextPrintf( display_out, "PointerRoot\n");
      break;
    default:			/* should not happen */
      XmTextPrintf( display_out, "%d\n", focusrevert);
      break;
    }
    break;
  }

  print_extension_info (dpy);

  XmTextPrintf( display_out,"default screen number:    %d\n", 
		DefaultScreen (dpy));
  XmTextPrintf( display_out, "number of screens:    %d\n", ScreenCount (dpy));

  return;
}

void print_extension_info (dpy)
    Display *dpy;
{
    int n = 0;
    char **extlist = XListExtensions (dpy, &n);

    XmTextPrintf( display_out, "number of extensions:    %d\n", n);

    if (extlist) {
	register int i;

	for (i = 0; i < n; i++) {
	    XmTextPrintf( display_out,"    %s\n", extlist[i]);
	}
	XFreeExtensionList (extlist);
    }
    return;
}

void
print_screen_info (dpy, scr)
    Display *dpy;
    int scr;
{
  Screen *s = ScreenOfDisplay (dpy, scr);  /* opaque structure */
  XVisualInfo viproto;		/* fill in for getting info */
  XVisualInfo *vip;			/* retured info */
  int nvi;				/* number of elements returned */
  int i;				/* temp variable: iterator */
  char eventbuf[80];			/* want 79 chars per line + nul */
  static char *yes = "YES", *no = "NO", *when = "WHEN MAPPED";
  double xres, yres;
  int ndepths = 0, *depths = NULL;


  /*
   * there are 2.54 centimeters to an inch; so there are 25.4 millimeters.
   *
   *     dpi = N pixels / (M millimeters / (25.4 millimeters / 1 inch))
   *         = N pixels / (M inch / 25.4)
   *         = N * 25.4 pixels / M inch
   */

  xres = ((((double) DisplayWidth(dpy,scr)) * 25.4) / 
	  ((double) DisplayWidthMM(dpy,scr)));
  yres = ((((double) DisplayHeight(dpy,scr)) * 25.4) / 
	  ((double) DisplayHeightMM(dpy,scr)));

  XmTextPrintf( display_out,"\n");
  XmTextPrintf( display_out,"screen #%d:\n", scr);
  XmTextPrintf( display_out,
		"  dimensions:    %dx%d pixels (%dx%d millimeters)\n",
		DisplayWidth (dpy, scr), DisplayHeight (dpy, scr),
		DisplayWidthMM(dpy, scr), DisplayHeightMM (dpy, scr));
  XmTextPrintf( display_out,"  resolution:    %dx%d dots per inch\n", 
		(int) (xres + 0.5), (int) (yres + 0.5));
  depths = XListDepths (dpy, scr, &ndepths);
  if (!depths) ndepths = 0;
  XmTextPrintf( display_out,"  depths (%d):    ", ndepths);
  for (i = 0; i < ndepths; i++) {
    XmTextPrintf( display_out,"%d", depths[i]);
    if (i < ndepths - 1) { 
      XmTextPrintf( display_out, ", " );
/*       putchar (','); */
/*       putchar (' '); */
    }
  }
/*   putchar ('\n'); */
  XmTextNewLine( display_out );
  if (depths) XFree ((char *) depths);
  XmTextPrintf( display_out,"  root window id:    0x%lx\n", 
		RootWindow (dpy, scr));
  XmTextPrintf( display_out,"  depth of root window:    %d plane%s\n",
		DisplayPlanes (dpy, scr),
		DisplayPlanes (dpy, scr) == 1 ? "" : "s");
  XmTextPrintf( display_out,
		"  number of colormaps:    minimum %d, maximum %d\n",
		MinCmapsOfScreen(s), MaxCmapsOfScreen(s));
  XmTextPrintf( display_out,"  default colormap:    0x%lx\n", 
		DefaultColormap (dpy, scr));
  XmTextPrintf( display_out,"  default number of colormap cells:    %d\n",
		DisplayCells (dpy, scr));
  XmTextPrintf( display_out,"  preallocated pixels:    black %d, white %d\n",
		BlackPixel (dpy, scr), WhitePixel (dpy, scr));
  XmTextPrintf( display_out,"  options:    backing-store %s, save-unders %s\n",
		(DoesBackingStore (s) == NotUseful) ? no :
		((DoesBackingStore (s) == Always) ? yes : when),
		DoesSaveUnders (s) ? yes : no);
  XmTextPrintf( display_out,"  current input event mask:    0x%lx\n", 
		EventMaskOfScreen (s));
  (void) print_event_mask (eventbuf, 79, 4, EventMaskOfScreen (s));
		      

  nvi = 0;
  viproto.screen = scr;
  vip = XGetVisualInfo (dpy, VisualScreenMask, &viproto, &nvi);
  XmTextPrintf( display_out,"  number of visuals:    %d\n", nvi);
  XmTextPrintf( display_out,"  default visual id:  0x%lx\n", 
		XVisualIDFromVisual (DefaultVisual (dpy, scr)));
  for (i = 0; i < nvi; i++) {
    print_visual_info (vip+i);
  }
  if (vip) XFree ((char *) vip);

  return;
}


void
print_multibuf_info(dpy, scr)
    Display *dpy;
    int scr;
{
    int j;			/* temp variable: iterator */
    int nmono, nstereo;		/* count */
    XmbufBufferInfo *mono_info = NULL, *stereo_info = NULL; /* arrays */
    static char *fmt = 
	"    visual id, max buffers, depth:    0x%lx, %d, %d\n";

    if (!XmbufGetScreenInfo (dpy, RootWindow(dpy, scr), &nmono, &mono_info,
			     &nstereo, &stereo_info)) {
/* 	fprintf (stderr, */
/* 		 "%s:  unable to get multibuffer info for screen %d\n", */
/* 		 ProgramName, scr); */
	fprintf (stderr,
		 "xqs:  unable to get multibuffer info for screen %d\n", scr);
    } else {
	XmTextPrintf( display_out,
		      "  number of mono multibuffer types:    %d\n", nmono);
	for (j = 0; j < nmono; j++) {
	    XmTextPrintf( display_out, fmt, mono_info[j].visualid, 
			  mono_info[j].max_buffers,
		    mono_info[j].depth);
	}
	XmTextPrintf( display_out,
		      "  number of stereo multibuffer types:    %d\n",nstereo);
	for (j = 0; j < nstereo; j++) {
	    XmTextPrintf( display_out,fmt, stereo_info[j].visualid,
			  stereo_info[j].max_buffers, stereo_info[j].depth);
	}
	if (mono_info) XFree ((char *) mono_info);
	if (stereo_info) XFree ((char *) stereo_info);
    }
}

void
print_visual_info (vip)
    XVisualInfo *vip;
{
    char errorbuf[40];			/* for sprintfing into */
    char *class = NULL;			/* for printing */

    switch (vip->class) {
      case StaticGray:    class = "StaticGray"; break;
      case GrayScale:    class = "GrayScale"; break;
      case StaticColor:    class = "StaticColor"; break;
      case PseudoColor:    class = "PseudoColor"; break;
      case TrueColor:    class = "TrueColor"; break;
      case DirectColor:    class = "DirectColor"; break;
      default:    
	sprintf( errorbuf, "unknown class %d", vip->class);
	class = errorbuf;
	break;
    }

    XmTextPrintf( display_out,"  visual:\n");
    XmTextPrintf( display_out,"    visual id:    0x%lx\n", vip->visualid);
    XmTextPrintf( display_out,"    class:    %s\n", class);
    XmTextPrintf( display_out,"    depth:    %d plane%s\n", vip->depth, 
		  vip->depth == 1 ? "" : "s");
    XmTextPrintf( display_out,"    size of colormap:    %d entries\n", 
		  vip->colormap_size);
    XmTextPrintf( display_out,
		  "    red, green, blue masks:    0x%lx, 0x%lx, 0x%lx\n",
		  vip->red_mask, vip->green_mask, vip->blue_mask);
    XmTextPrintf( display_out,
		  "    significant bits in color specification:    %d bits\n",
		  vip->bits_per_rgb);

    return;
}

/*
 * The following routine prints out an event mask, wrapping events at nice
 * boundaries.
 */

#define MASK_NAME_WIDTH 25

static struct _event_table {
    char *name;
    long value;
} event_table[] = {
    { "KeyPressMask             ", KeyPressMask },
    { "KeyReleaseMask           ", KeyReleaseMask },
    { "ButtonPressMask          ", ButtonPressMask },
    { "ButtonReleaseMask        ", ButtonReleaseMask },
    { "EnterWindowMask          ", EnterWindowMask },
    { "LeaveWindowMask          ", LeaveWindowMask },
    { "PointerMotionMask        ", PointerMotionMask },
    { "PointerMotionHintMask    ", PointerMotionHintMask },
    { "Button1MotionMask        ", Button1MotionMask },
    { "Button2MotionMask        ", Button2MotionMask },
    { "Button3MotionMask        ", Button3MotionMask },
    { "Button4MotionMask        ", Button4MotionMask },
    { "Button5MotionMask        ", Button5MotionMask },
    { "ButtonMotionMask         ", ButtonMotionMask },
    { "KeymapStateMask          ", KeymapStateMask },
    { "ExposureMask             ", ExposureMask },
    { "VisibilityChangeMask     ", VisibilityChangeMask },
    { "StructureNotifyMask      ", StructureNotifyMask },
    { "ResizeRedirectMask       ", ResizeRedirectMask },
    { "SubstructureNotifyMask   ", SubstructureNotifyMask },
    { "SubstructureRedirectMask ", SubstructureRedirectMask },
    { "FocusChangeMask          ", FocusChangeMask },
    { "PropertyChangeMask       ", PropertyChangeMask },
    { "ColormapChangeMask       ", ColormapChangeMask },
    { "OwnerGrabButtonMask      ", OwnerGrabButtonMask },
    { NULL, 0 }};

int print_event_mask (buf, lastcol, indent, mask)
    char *buf;				/* string to write into */
    int lastcol;			/* strlen(buf)+1 */
    int indent;				/* amount by which to indent */
    long mask;				/* event mask */
{
    struct _event_table *etp;
    int len;
    int bitsfound = 0;

    buf[0] = buf[lastcol] = '\0';	/* just in case */

#define INDENT() { register int i; len = indent; \
		   for (i = 0; i < indent; i++) buf[i] = ' '; }

    INDENT ();

    for (etp = event_table; etp->name; etp++) {
	if (mask & etp->value) {
	    if (len + MASK_NAME_WIDTH > lastcol) {
/* 		puts (buf); */
	      XmTextPrintf( display_out, buf );
	      XmTextNewLine( display_out );
		INDENT ();
	    }
	    strcpy (buf+len, etp->name);
	    len += MASK_NAME_WIDTH;
	    bitsfound++;
	}
    }

    if (bitsfound){ /* puts (buf); */ 
      XmTextPrintf( display_out, buf );
      XmTextNewLine( display_out );
    }      

#undef INDENT

    return (bitsfound);
}
