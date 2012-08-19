/***** 
 * WindowTab.c 
 *****
 * Written By: brett adams (bretta@jove.acs.unt.edu)
 * Description: WindowTab.c -- functions to control the window tab,
 *	outputs the window name, id and number of children for each 
 *	window of a display's default root window tree into the ListTree 
 *	widget developed by Robert W. McMullen <rwmcm@orion.ae.utexas.edu>,
 *	majority of the information retrieval code derived from xwininfo
 *****
 ****/

/*=====================
 * xwininfo
 *======================
 */
/* Copyright 1987, Massachusetts Institute of Technology */ 
/*
 * xwininfo.c   - MIT Project Athena, X Window system window
 *                information utility.
 *
 * $XConsortium: xwininfo.c,v 1.52 91/05/11 22:32:49 gildea Exp $
 *
 *      This program will report all relevant information
 *      about a specific window.
 *
 *  Author:     Mark Lillibridge, MIT Project Athena
 *              16-Jun-87
 */



#include "xqs.h"
#include "Prefs.h"

Widget win_out;

extern int UpdateWinTree( Display *display, Window toplevel, Widget listree );
extern GetHighlightedWindow();

void PutWinInfo( Window win );
void Display_Window_Id( Window win, Bool newline_wanted );
void Display_Events_Info( Window win);
void Display_Event_Mask( long mask );
void Display_Tree_Info();
void Display_Stats_Info();
void Display_Bits_Info();
void Display_WM_Info();
void Display_Size_Hints();
void Display_Window_Shape();
void xwininfo( Window, Bool, Bool, Bool, Bool, Bool, Bool, Bool, Bool );


void WindowTabActive()
{
  Window win;

  XtManageChild( win_frame );
  if( info_dpy == NULL ) return;

  win = GetHighlightedWindow( wintree );
  if( win == 0 ){
       XmTextClear( win_out );
       SetStatus( "No window selected in WinTree" );
  }
  else PutWinInfo( win );

}/* end WindowTabActive()  */



/* print window information to win_out */
void PutWinInfo( Window win )
{
  char str[256];
  Bool children, tree, stats, bits, events, wm, size, shape;

  if( ! XtIsManaged(win_frame) )   return;

  XmTextClear( win_out );

  sprintf( str,"Retrieving window information( 0x%x )...", (unsigned int)win );
  SetStatus( str );
  SetWidgetCursor( tab_form, XC_watch );


  children = pref_db.winPrefs[0].value;
  tree = pref_db.winPrefs[1].value;
  stats = pref_db.winPrefs[2].value;
  bits = pref_db.winPrefs[3].value;
  events = pref_db.winPrefs[4].value;
  wm = pref_db.winPrefs[5].value;
  size = pref_db.winPrefs[6].value;
  shape = pref_db.winPrefs[7].value;

  xwininfo( win, children, tree, stats, bits, events, wm, size, shape );

  XtVaSetValues( win_out, XmNcursorPosition, 0, NULL );
  ClearStatus();
  UnsetWidgetCursor( tab_form );
}

void 
InitWinTab()
{
  Widget  win_form, win_scroll;
  
  /*printf( "InitWinTab() called\n" );*/

  win_frame = XmCreateFrame( tab_form, "win_frame", NULL, 0 );

  win_form = XmCreateForm( win_frame, "win_form", NULL, 0 );
  XtManageChild( win_form );

  win_out = XmCreateScrolledText( win_form, "win_out", NULL, 0 );
  XtManageChild( win_out );
  
  /* get the id of the scrolled window for wintree and win_out */
  win_scroll = XtParent( win_out );
  
  XtVaSetValues( win_frame,
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

  XtVaSetValues( win_form,
		 XmNtopAttachment, XmATTACH_FORM,
		 XmNbottomAttachment, XmATTACH_FORM,
		 XmNleftAttachment, XmATTACH_FORM,	
		 XmNrightAttachment, XmATTACH_FORM,
		 NULL );
  
  XtVaSetValues( win_scroll,
		 XmNtopAttachment, XmATTACH_FORM,
		 XmNbottomAttachment, XmATTACH_FORM,		 
		 XmNleftAttachment, XmATTACH_FORM,	
		 XmNrightAttachment, XmATTACH_FORM,
		 XmNscrollBarPlacement, XmBOTTOM_RIGHT,
		 NULL );

  XtVaSetValues( win_out,
		 XmNshadowThickness, 0,
		 XmNeditMode, XmMULTI_LINE_EDIT,
		 XmNeditable, FALSE,			/* can't edit */
		 XmNcursorPositionVisible, FALSE, 	/* no cursor */
		 XmNautoShowCursorPosition, FALSE,
		 XmNscrollingPolicy, XmAUTOMATIC,
		 XmNwordWrap, TRUE,
		 NULL );

}


/****************
 *
 * code modified from xwininfo
 *
 *************/

void display_tree_info_1();
int bad_window_handler( Display *, XErrorEvent *);
static char *window_id_format = "0x%lx";
void Fatal_Error();
Bool BAD_WINDOW;

void xwininfo( window, children, tree, stats, bits, events, wm, size, shape )
  Window window;
  Bool children;
  Bool tree;
  Bool stats;
  Bool bits;
  Bool events;
  Bool wm;
  Bool size;
  Bool shape;
{
  Window root;
  int x, y;
  unsigned width, height, bw, depth;
  int (*old_handler)();

  BAD_WINDOW = FALSE;

  /*
   * make sure that the window is valid
   */
  old_handler = XSetErrorHandler(bad_window_handler);
  XGetGeometry (info_dpy, window, &root, &x, &y, &width, &height, &bw, 
		&depth);
  XSync (info_dpy, False);
  (void) XSetErrorHandler(old_handler);

  if( BAD_WINDOW ) {
       XmTextPrintf( win_out, "Window 0x%x does no exists on server %s",
		     (unsigned int)window, DisplayString(info_dpy) );
       return;
  }

  XmTextPrintf( win_out, "Window id: ");
  Display_Window_Id(window, True);
  if (children || tree)
    Display_Tree_Info(window, tree);
  if (stats)
    Display_Stats_Info(window);
  if (bits)
    Display_Bits_Info(window);
  if (events)
    Display_Events_Info(window);
  if (wm)
    Display_WM_Info(window);
  if (size)
    Display_Size_Hints(window);
  if (shape)
    Display_Window_Shape(window);

}




/*
 * Lookup: lookup a code in a table.
 */
/* typedef struct { */
/* 	long code; */
/* 	char *name; */
/* } binding; */



static binding _window_classes[] = {
	{ InputOutput, "InputOutput" },
	{ InputOnly, "InputOnly" },
        { 0, 0 } };

static binding _map_states[] = {
	{ IsUnmapped, "IsUnMapped" },
	{ IsUnviewable, "IsUnviewable" },
	{ IsViewable, "IsViewable" },
	{ 0, 0 } };

static binding _backing_store_states[] = {
	{ NotUseful, "NotUseful" },
	{ WhenMapped, "WhenMapped" },
	{ Always, "Always" },
	{ 0, 0 } };

static binding _bit_gravity_states[] = {
	{ ForgetGravity, "ForgetGravity" },
	{ NorthWestGravity, "NorthWestGravity" },
	{ NorthGravity, "NorthGravity" },
	{ NorthEastGravity, "NorthEastGravity" },
	{ WestGravity, "WestGravity" },
	{ CenterGravity, "CenterGravity" },
	{ EastGravity, "EastGravity" },
	{ SouthWestGravity, "SouthWestGravity" },
	{ SouthGravity, "SouthGravity" },
	{ SouthEastGravity, "SouthEastGravity" },
	{ StaticGravity, "StaticGravity" },
	{ 0, 0 }};

static binding _window_gravity_states[] = {
	{ UnmapGravity, "UnmapGravity" },
	{ NorthWestGravity, "NorthWestGravity" },
	{ NorthGravity, "NorthGravity" },
	{ NorthEastGravity, "NorthEastGravity" },
	{ WestGravity, "WestGravity" },
	{ CenterGravity, "CenterGravity" },
	{ EastGravity, "EastGravity" },
	{ SouthWestGravity, "SouthWestGravity" },
	{ SouthGravity, "SouthGravity" },
	{ SouthEastGravity, "SouthEastGravity" },
	{ StaticGravity, "StaticGravity" },
	{ 0, 0 }};

static binding _visual_classes[] = {
	{ StaticGray, "StaticGray" },
	{ GrayScale, "GrayScale" },
	{ StaticColor, "StaticColor" },
	{ PseudoColor, "PseudoColor" },
	{ TrueColor, "TrueColor" },
	{ DirectColor, "DirectColor" },
	{ 0, 0 }};

static char _lookup_buffer[100];
char *Lookup(code, table)
    int code;
    binding *table;
{
	char *name;

	sprintf(_lookup_buffer, "unknown (code = %ld. = 0x%lx)", 
		(long)code, (long)code);
	name = _lookup_buffer;

	while (table->name) {
		if (table->code == code) {
			name = table->name;
			break;
		}
		table++;
	}

	return(name);
}

/*
 * pixel to inch, metric converter.
 * Hacked in by Mark W. Eichin <eichin@athena> [eichin:19880619.1509EST]
 *
 * Simply put: replace the old numbers with string print calls.
 * Returning a local string is ok, since we only ever get called to
 * print one x and one y, so as long as they don't collide, they're
 * fine. This is not meant to be a general purpose routine.
 *
 */
/*#define screen DefaultScreenOfDisplay(info_dpy)*/
/*#define getdsp(var,fn) var = fn(dpy, DefaultScreen(dpy))*/
#define getdsp(var,fn) var = fn(info_dpy, DefaultScreen(info_dpy))
int xp=0, xmm=0;
int yp=0, ymm=0;
int bp=0, bmm=0;
int english = 0, metric = 0;

void scale_init()
{
  getdsp(yp,  DisplayHeight);
  getdsp(ymm, DisplayHeightMM);
  getdsp(xp,  DisplayWidth);
  getdsp(xmm, DisplayWidthMM);
  bp  = xp  + yp;
  bmm = xmm + ymm;
}

double drem();

#define MILE (5280*12)
#define YARD (3*12)
#define FOOT (12)

char *nscale(n, np, nmm, nbuf)
     int n, np, nmm;
     char *nbuf;
{
  sprintf(nbuf, "%d", n);
  if(metric||english) {
    sprintf(nbuf+strlen(nbuf), " (");
  }
  if(metric) {
    sprintf(nbuf+strlen(nbuf),"%.2f mm%s", ((float) n)*nmm/np, english?"; ":"");
  }
  if(english) {
    float in;
    int mi, yar, ft, inr;
    in = ((float) n)*(nmm/25.4)/np;
    inr = (int)in;
    if(inr>=MILE) {
      mi = inr/MILE;
      inr %= MILE;
      sprintf(nbuf+strlen(nbuf), "%d %s(?!?), ",
	      mi, (mi==1)?"mile":"miles");
    }
    if(inr>=YARD) {
      yar = inr/YARD;
      inr %= YARD;
      sprintf(nbuf+strlen(nbuf), "%d %s, ",
	      yar, (yar==1)?"yard":"yards");
    }
    if(inr>=FOOT) {
      ft = inr/FOOT;
      inr  %= FOOT;
      sprintf(nbuf+strlen(nbuf), "%d %s, ",
	      ft, (ft==1)?"foot":"feet");
    }
    sprintf(nbuf+strlen(nbuf), "%.2f inches", in);
  }
  if (english || metric) strcat (nbuf, ")");
  return(nbuf);
}	  
  
char xbuf[BUFSIZ];
char *xscale(x)
     int x;
{
  if(!xp) {
    scale_init();
  }
  return(nscale(x, xp, xmm, xbuf));
}

char ybuf[BUFSIZ];
char *yscale(y)
     int y;
{
  if(!yp) {
    scale_init();
  }
  return(nscale(y, yp, ymm, ybuf));
}

char bbuf[BUFSIZ];
char *bscale(b)
     int b;
{
  if(!bp) {
    scale_init();
  }
  return(nscale(b, bp, bmm, bbuf));
}

/* end of pixel to inch, metric converter */


/* This handler is enabled when we are checking
   to see if the -id the user specified is valid. */

/* ARGSUSED */
/*void bad_window_handler(disp, err)*/
int bad_window_handler(disp, err)
    Display *disp;
    XErrorEvent *err;
{
    char badid[20];

    sprintf(badid, window_id_format, err->resourceid);
    Fatal_Error("No such window with id %s.", badid);
    /* exit (1); */
    BAD_WINDOW = TRUE;    
    return 0;
}


void Display_Window_Id( window, newline_wanted )
  Window window;
  Bool newline_wanted;
{
  Display *dpy = info_dpy;
  char *win_name;

  XmTextPrintf( win_out, window_id_format, window );

  if( !window ) {
    XmTextPrintf( win_out," (none)" );
  } else {
      if( window == RootWindow(dpy, DefaultScreen(dpy)) ){
	XmTextPrintf( win_out, " (the root window)" );
      }
      if( !XFetchName(dpy, window, &win_name) ) {
	XmTextPrintf( win_out, " (has no name)" );
      } else if( win_name ){
	XmTextPrintf( win_out, " \"%s\"", win_name);
	XFree(win_name);
      } else 
	  XmTextPrintf( win_out, " (has no name)" );
  }

  if( newline_wanted )
    XmTextNewLine( win_out );

  /*return;*/
}

void
Display_Tree_Info(window, recurse)
     Window window;
     int recurse;		/* true if should show children's children */
{
    display_tree_info_1(window, recurse, 0);
}

void
display_tree_info_1(window, recurse, level)
     Window window;
     int recurse;
     int level;			/* recursion level */
{
  Display *dpy = info_dpy;
  int i, j;
  int rel_x, rel_y, abs_x, abs_y;
  unsigned int width, height, border, depth;
  Window root_win, parent_win;
  unsigned int num_children;
  Window *child_list;
  XClassHint classhint;

  if (!XQueryTree(dpy, window, &root_win, &parent_win, &child_list,
		  &num_children)){
    Fatal_Error("display_tree_info_1() -- Can't query window tree.");
    return;
  }

  if (level == 0) {
    XmTextPrintf( win_out, "\n  Root window id: ");
    Display_Window_Id(root_win, True);
    XmTextPrintf( win_out, "  Parent window id: ");
    Display_Window_Id(parent_win, True);
  }

  if (level == 0  ||  num_children > 0) {
    XmTextPrintf( win_out, "     ");
    
    for (j=0; j<level; j++) XmTextAppend( win_out, "  ");
    XmTextPrintf( win_out, "%d child%s%s\n", num_children, 
		  num_children == 1 ? "" : "ren", num_children ? ":" : ".");
  }

  for (i = (int)num_children - 1; i >= 0; i--) {
    XmTextPrintf( win_out, "     ");
    for (j=0; j<level; j++) XmTextAppend( win_out, "   ");
    Display_Window_Id(child_list[i], False);
    XmTextPrintf(win_out, ": (");
    if(XGetClassHint(dpy, child_list[i], &classhint)) {
	if(classhint.res_name) {
	    XmTextPrintf( win_out,"\"%s\" ", classhint.res_name);
	    XFree(classhint.res_name);
	} else
	    XmTextPrintf(win_out,"(none) ");
	if(classhint.res_class) {
	    XmTextPrintf(win_out,"\"%s\") ", classhint.res_class);
	    XFree(classhint.res_class);
	} else
	    XmTextPrintf(win_out,"(none)) ");
    } else
	XmTextPrintf(win_out,") ");

    if (XGetGeometry(dpy, child_list[i], &root_win,
		     &rel_x, &rel_y, &width, &height, &border, &depth)) {
	Window child;

	XmTextPrintf (win_out," %ux%u+%d+%d", width, height, rel_x, rel_y);
	if (XTranslateCoordinates (dpy, child_list[i], root_win,
				   0 ,0, &abs_x, &abs_y, &child)) {
	    XmTextPrintf (win_out, "  +%d+%d", abs_x - border, abs_y - border);
	}
    }
/*     printf("\n"); */
    XmTextNewLine(win_out);
    
    if (recurse)
	display_tree_info_1(child_list[i], 1, level+1);
  }

  if (child_list) XFree((char *)child_list);
}

/*
 * Display bits info:
 */
static binding _gravities[] = {
	{ UnmapGravity, "UnMapGravity" },      /* WARNIN: both of these have*/
	{ ForgetGravity, "ForgetGravity" },    /* the same value - see code */
	{ NorthWestGravity, "NorthWestGravity" },
	{ NorthGravity, "NorthGravity" },
	{ NorthEastGravity, "NorthEastGravity" },
	{ WestGravity, "WestGravity" },
	{ CenterGravity, "CenterGravity" },
	{ EastGravity, "EastGravity" },
	{ SouthWestGravity, "SouthWestGravity" },
	{ SouthGravity, "SouthGravity" },
	{ SouthEastGravity, "SouthEastGravity" },
	{ StaticGravity, "StaticGravity" },
	{ 0, 0 } };

static binding _backing_store_hint[] = {
	{ NotUseful, "NotUseful" },
	{ WhenMapped, "WhenMapped" },
	{ Always, "Always" },
	{ 0, 0 } };

static binding _bool[] = {
	{ 0, "No" },
	{ 1, "Yes" },
	{ 0, 0 } };

void
Display_Bits_Info(window)
     Window window;
{
  Display *dpy = info_dpy;
  XWindowAttributes win_attributes;

  if (!XGetWindowAttributes(dpy, window, &win_attributes)){
    Fatal_Error("Display_Bits_Info() -- Can't get window attributes.");
    return;
  }

/*   printf("\n"); */
  XmTextNewLine( win_out );
  XmTextPrintf( win_out, "  Bit gravity: %s\n",
		Lookup(win_attributes.bit_gravity, _gravities+1));
  XmTextPrintf( win_out, "  Window gravity: %s\n",
		Lookup(win_attributes.win_gravity, _gravities));
  XmTextPrintf( win_out, "  Backing-store hint: %s\n",
		Lookup(win_attributes.backing_store, _backing_store_hint));
  XmTextPrintf( win_out, "  Backing-planes to be preserved: 0x%x\n",
		(unsigned int)win_attributes.backing_planes);
  XmTextPrintf( win_out, "  Backing pixel: %d\n", 
		(int)win_attributes.backing_pixel);
  XmTextPrintf( win_out, "  Save-unders: %s\n",
		Lookup(win_attributes.save_under, _bool));
}

/*
 * Display a set of size hints
 */
void
Display_Hints(hints)
     XSizeHints *hints;
{
  
	long flags;

	flags = hints->flags;
	
	if (flags & USPosition)
	  XmTextPrintf( win_out, "      User supplied location: %s, %s\n",
			xscale(hints->x), yscale(hints->y));
	
	if (flags & PPosition)
	  XmTextPrintf( win_out, "      Program supplied location: %s, %s\n",
		       xscale(hints->x), yscale(hints->y));

	if (flags & USSize) {
	  XmTextPrintf( win_out, "      User supplied size: %s by %s\n",
			xscale(hints->width), yscale(hints->height));
	}

	if (flags & PSize)
	  XmTextPrintf( win_out, "      Program supplied size: %s by %s\n",
			xscale(hints->width), yscale(hints->height));

	if (flags & PMinSize)
	  XmTextPrintf( win_out, 
			"      Program supplied minimum size: %s by %s\n",
			xscale(hints->min_width), yscale(hints->min_height));

	if (flags & PMaxSize)
	  XmTextPrintf( win_out, 
			"      Program supplied maximum size: %s by %s\n",
			xscale(hints->max_width), yscale(hints->max_height));

	if (flags & PBaseSize) {
	  XmTextPrintf( win_out,"      Program supplied base size: %s by %s\n",
			xscale(hints->base_width), yscale(hints->base_height));
	}

	if (flags & PResizeInc) {
	  XmTextPrintf( win_out, 
		       "      Program supplied x resize increment: %s\n",
		       xscale(hints->width_inc));
	  XmTextPrintf( win_out, 
		       "      Program supplied y resize increment: %s\n",
		       yscale(hints->height_inc));
	  if (hints->width_inc != 0 && hints->height_inc != 0) {
	      if (flags & USSize)
		XmTextPrintf( win_out, 
			      "      User supplied size in resize increments:  %s by %s\n",
			      (xscale(hints->width / hints->width_inc)), 
			      (yscale(hints->height / hints->height_inc)));
	      if (flags & PSize)
		  XmTextPrintf( win_out, 
			       "      Program supplied size in resize increments:  %s by %s\n",
			       (xscale(hints->width / hints->width_inc)), 
			       (yscale(hints->height / hints->height_inc)));
	      if (flags & PMinSize)
		  XmTextPrintf( win_out, 
			       "      Program supplied minimum size in resize increments: %s by %s\n",
			       xscale(hints->min_width / hints->width_inc), 
			       yscale(hints->min_height / hints->height_inc));
	      if (flags & PBaseSize)
		  XmTextPrintf( win_out, 
			       "      Program supplied base size in resize increments:  %s by %s\n",
			       (xscale(hints->base_width / hints->width_inc)), 
			       (yscale(hints->base_height/hints->height_inc)));
	  }
        }

	if (flags & PAspect) {
	  XmTextPrintf( win_out, 
		       "      Program supplied min aspect ratio: %s/%s\n",
		       xscale(hints->min_aspect.x), 
		       yscale(hints->min_aspect.y));
	  XmTextPrintf( win_out, 
		       "      Program supplied max aspect ratio: %s/%s\n",
		       xscale(hints->max_aspect.x), 
		       yscale(hints->max_aspect.y));
        }

	if (flags & PWinGravity) {
	  XmTextPrintf( win_out, "      Program supplied window gravity: %s\n",
		       Lookup(hints->win_gravity, _gravities));
	}
}


/*
 * Display Size Hints info
 */
void
Display_Size_Hints(window)
     Window window;
{
  	Display *dpy = info_dpy;
	XSizeHints *hints = XAllocSizeHints();
	long supplied;

/* 	printf("\n"); */
	XmTextNewLine( win_out );
	if (!XGetWMNormalHints(dpy, window, hints, &supplied))
	    XmTextPrintf( win_out, "  No normal window size hints defined\n");
	else {
	    XmTextPrintf( win_out, "  Normal window size hints:\n");
	    hints->flags &= supplied;
	    Display_Hints(hints);
	}

	if (!XGetWMSizeHints(dpy, window, hints, &supplied, XA_WM_ZOOM_HINTS))
	    XmTextPrintf( win_out, "  No zoom window size hints defined\n");
	else {
	    XmTextPrintf( win_out, "  Zoom window size hints:\n");
	    hints->flags &= supplied;
	    Display_Hints(hints);
	}
	XFree((char *)hints);
}

void
Display_Window_Shape (window)
    Window  window;
{
    Display *dpy = info_dpy;
    Bool    ws, bs;
    int	    xws, yws, xbs, ybs;
    unsigned int wws, hws, wbs, hbs;

    if (!XShapeQueryExtension (dpy, &bs, &ws))
	return;

/*     printf("\n"); */
    XmTextNewLine( win_out );
    XShapeQueryExtents (dpy, window, &ws, &xws, &yws, &wws, &hws,
				     &bs, &xbs, &ybs, &wbs, &hbs);
    if (!ws)
	  XmTextPrintf( win_out, "  No window shape defined\n");
    else {
	  XmTextPrintf( win_out, "  Window shape extents:  %sx%s",
			xscale(wws), yscale(hws));
	  XmTextPrintf( win_out, "+%s+%s\n", xscale(xws), yscale(yws));
    }
    if (!bs)
	  XmTextPrintf( win_out, "  No border shape defined\n");
    else {
	  XmTextPrintf( win_out, "  Border shape extents:  %sx%s",
			xscale(wbs), yscale(hbs));
	  XmTextPrintf( win_out, "+%s+%s\n", xscale(xbs), yscale(ybs));
    }
}


void
Display_Stats_Info(window)
     Window window;
{
  Display *dpy = info_dpy;
  int screen = DefaultScreen(dpy);
  XWindowAttributes win_attributes;
  XVisualInfo vistemplate, *vinfo;
  XSizeHints hints;
  int dw = DisplayWidth (dpy, screen), dh = DisplayHeight (dpy, screen);
  int rx, ry, xright, ybelow;
  int showright = 0, showbelow = 0;
  Status status;
  Window wmframe;
  int junk;
  long longjunk;
  Window junkwin;

  if (!XGetWindowAttributes(dpy, window, &win_attributes)){
    Fatal_Error("Display_Stats_Info() -- Can't get window attributes.");
    return;
  }
  vistemplate.visualid = XVisualIDFromVisual(win_attributes.visual);
  vinfo = XGetVisualInfo(dpy, VisualIDMask, &vistemplate, &junk);

  (void) XTranslateCoordinates (dpy, window, win_attributes.root, 
				-win_attributes.border_width,
				-win_attributes.border_width,
				&rx, &ry, &junkwin);
				
  xright = (dw - rx - win_attributes.border_width * 2 -
	    win_attributes.width);
  ybelow = (dh - ry - win_attributes.border_width * 2 -
	    win_attributes.height);


  XmTextPrintf( win_out,"  Absolute upper-left X:  %s\n", xscale(rx) );
  XmTextPrintf( win_out, "  Absolute upper-left Y:  %s\n", yscale(ry));
  XmTextPrintf(win_out,"  Relative upper-left X:  %s\n",xscale(win_attributes.x));
  XmTextPrintf(win_out,"  Relative upper-left Y:  %s\n",yscale(win_attributes.y));
  XmTextPrintf( win_out,"  Width: %s\n", xscale(win_attributes.width));
  XmTextPrintf( win_out,"  Height: %s\n", yscale(win_attributes.height));
  XmTextPrintf( win_out,"  Depth: %d\n", win_attributes.depth);
  XmTextPrintf( win_out,
	       "  Visual Class: %s\n",Lookup(vinfo->class,_visual_classes));
  XmTextPrintf( win_out,
		"  Border width: %s\n", bscale(win_attributes.border_width));
  XmTextPrintf( win_out,"  Class: %s\n",
		Lookup(win_attributes.class, _window_classes));
  XmTextPrintf( win_out,"  Colormap: 0x%lx (%sinstalled)\n", 
		win_attributes.colormap,
		win_attributes.map_installed ? "" : "not ");
  XmTextPrintf( win_out, "  Bit Gravity State: %s\n",
		Lookup(win_attributes.bit_gravity, _bit_gravity_states));
  XmTextPrintf( win_out, "  Window Gravity State: %s\n",
		Lookup(win_attributes.win_gravity, _window_gravity_states));
  XmTextPrintf( win_out, "  Backing Store State: %s\n",
		Lookup(win_attributes.backing_store, _backing_store_states));
  XmTextPrintf( win_out, "  Save Under State: %s\n",
		win_attributes.save_under ? "yes" : "no");
  XmTextPrintf( win_out, "  Map State: %s\n",
		Lookup(win_attributes.map_state, _map_states));
  XmTextPrintf( win_out, "  Override Redirect State: %s\n",
		win_attributes.override_redirect ? "yes" : "no");
  XmTextPrintf( win_out, "  Corners:  +%d+%d  -%d+%d  -%d-%d  +%d-%d\n",
		rx, ry, xright, ry, xright, ybelow, rx, ybelow);

  /*
   * compute geometry string that would recreate window
   */
  XmTextPrintf( win_out,"  -geometry ");
  /* compute size in appropriate units */
  status = XGetWMNormalHints(dpy, window, &hints, &longjunk);
  if (status  &&  hints.flags & PResizeInc  &&
      hints.width_inc != 0  &&  hints.height_inc != 0) {
    if (hints.flags & (PBaseSize|PMinSize)) {
      if( hints.flags & PBaseSize) {
	win_attributes.width -= hints.base_width;
	win_attributes.height -= hints.base_height;
      } else {
	/* ICCCM says MinSize is default for BaseSize */
	win_attributes.width -= hints.min_width;
	win_attributes.height -= hints.min_height;
      }
    }
    XmTextPrintf( win_out, "%dx%d", win_attributes.width/hints.width_inc, 
 		  win_attributes.height/hints.height_inc); 
  } else
    XmTextPrintf( win_out,"%dx%d", win_attributes.width, win_attributes.height);

  if (!(hints.flags&PWinGravity))
    hints.win_gravity = NorthWestGravity; /* per ICCCM */

  /* find our window manager frame, if any */
  wmframe = window;
  while (True) {
    Window root, parent;
    Window *childlist;
    unsigned int ujunk;

    status = XQueryTree(dpy, wmframe, &root, &parent, &childlist, &ujunk);
    if (parent == root || !parent || !status)
      break;
    wmframe = parent;
    if (status && childlist)
      XFree((char *)childlist);
  }
  if (wmframe != window) {
    /* WM reparented, so find edges of the frame */
    /* Only works for ICCCM-compliant WMs, and then only if the
       window has corner gravity.  We would need to know the original width
       of the window to correctly handle the other gravities. */

    XWindowAttributes frame_attr;

    if (!XGetWindowAttributes(dpy, wmframe, &frame_attr)){
      Fatal_Error("Display_Stats_Info() -- Can't get frame attributes.");
      return;
    }
    switch (hints.win_gravity) {
    case NorthWestGravity: case SouthWestGravity:
    case NorthEastGravity: case SouthEastGravity:
    case WestGravity:
      rx = frame_attr.x;
    }
    switch (hints.win_gravity) {
    case NorthWestGravity: case SouthWestGravity:
    case NorthEastGravity: case SouthEastGravity:
    case EastGravity:
      xright = dw - frame_attr.x - frame_attr.width -
	2*frame_attr.border_width;
    }
    switch (hints.win_gravity) {
    case NorthWestGravity: case SouthWestGravity:
    case NorthEastGravity: case SouthEastGravity:
    case NorthGravity:
      ry = frame_attr.y;
    }
    switch (hints.win_gravity) {
    case NorthWestGravity: case SouthWestGravity:
    case NorthEastGravity: case SouthEastGravity:
    case SouthGravity:
      ybelow = dh - frame_attr.y - frame_attr.height -
	2*frame_attr.border_width;
    }
  }
  /* If edge gravity, offer a corner on that edge (because the application
     programmer cares about that edge), otherwise offer upper left unless
     some other corner is close to an edge of the screen.
     (For corner gravity, assume gravity was set by XWMGeometry.
     For CenterGravity, it doesn't matter.) */
  if (hints.win_gravity == EastGravity  ||
      (abs(xright) <= 100  &&  abs(xright) < abs(rx)
       &&  hints.win_gravity != WestGravity))
    showright = 1;
  if (hints.win_gravity == SouthGravity  ||
      (abs(ybelow) <= 100  &&  abs(ybelow) < abs(ry)
       &&  hints.win_gravity != NorthGravity))
    showbelow = 1;
  
  if (showright)
    XmTextPrintf( win_out,"-%d", xright);
  else
    XmTextPrintf( win_out,"+%d", rx);

  if (showbelow)
    XmTextPrintf( win_out,"-%d", ybelow);
  else
    XmTextPrintf( win_out,"+%d", ry);
  

  XmTextAppend( win_out, "\n" );

  return;
}





/*
 * Routine to display all events in an event mask
 */
static binding _event_mask_names[] = {
	{ KeyPressMask, "KeyPress" },
	{ KeyReleaseMask, "KeyRelease" },
	{ ButtonPressMask, "ButtonPress" },
	{ ButtonReleaseMask, "ButtonRelease" },
	{ EnterWindowMask, "EnterWindow" },
	{ LeaveWindowMask, "LeaveWindow" },
	{ PointerMotionMask, "PointerMotion" },
	{ PointerMotionHintMask, "PointerMotionHint" },
	{ Button1MotionMask, "Button1Motion" },
	{ Button2MotionMask, "Button2Motion" },
	{ Button3MotionMask, "Button3Motion" },
	{ Button4MotionMask, "Button4Motion" },
	{ Button5MotionMask, "Button5Motion" },
	{ ButtonMotionMask, "ButtonMotion" },
	{ KeymapStateMask, "KeymapState" },
	{ ExposureMask, "Exposure" },
	{ VisibilityChangeMask, "VisibilityChange" },
	{ StructureNotifyMask, "StructureNotify" },
	{ ResizeRedirectMask, "ResizeRedirect" },
	{ SubstructureNotifyMask, "SubstructureNotify" },
	{ SubstructureRedirectMask, "SubstructureRedirect" },
	{ FocusChangeMask, "FocusChange" },
	{ PropertyChangeMask, "PropertyChange" },
	{ ColormapChangeMask, "ColormapChange" },
	{ OwnerGrabButtonMask, "OwnerGrabButton" },
	{ 0, 0 } };

void
Display_Events_Info(window)
     Window window;
{
  Display *dpy = info_dpy;
  XWindowAttributes win_attributes;

  if (!XGetWindowAttributes(dpy, window, &win_attributes)) {
    Fatal_Error("Display_Events_Info() -- Can't get window attributes.");
    return;
  }

  XmTextAppend( win_out, "  Someone wants these events:\n" );
  Display_Event_Mask(win_attributes.all_event_masks);

  XmTextAppend( win_out, "  Do not propagate these events:\n" ); 
  Display_Event_Mask(win_attributes.do_not_propagate_mask);

  XmTextPrintf( win_out, "  Override redirection?: %s\n",
		Lookup(win_attributes.override_redirect, _bool));

}

void 
Display_Event_Mask(mask)
     long mask;
{
  long bit, bit_mask;

  for (bit=0, bit_mask=1; bit<sizeof(long)*8; bit++, bit_mask <<= 1)
    if (mask & bit_mask){
      XmTextPrintf( win_out, "      %s\n", 
		    Lookup(bit_mask, _event_mask_names));
    }
}


/*
 * Display Window Manager Info
 */
static binding _state_hints[] = {
	{ DontCareState, "Don't Care State" },
	{ NormalState, "Normal State" },
	{ ZoomState, "Zoomed State" },
	{ IconicState, "Iconic State" },
	{ InactiveState, "Inactive State" },
	{ 0, 0 } };

void
Display_WM_Info(window)  
     Window window;
{
  	Display *dpy = info_dpy;
        XWMHints *wmhints;
	long flags;

	wmhints = XGetWMHints(dpy, window);
/* 	printf("\n"); */
	XmTextNewLine( win_out );
	if (!wmhints) {
		XmTextPrintf( win_out, "  No window manager hints defined\n");
		return;
	}
	flags = wmhints->flags;
	
	XmTextPrintf( win_out,"  Window manager hints:\n");

	if (flags & InputHint)
	  XmTextPrintf( win_out,
			"      Client accepts input or input focus: %s\n",
			Lookup(wmhints->input, _bool));
	
	if (flags & IconWindowHint) {
		XmTextPrintf( win_out,"      Icon window id: ");
		Display_Window_Id(wmhints->icon_window, True);
	}

	if (flags & IconPositionHint)
	  XmTextPrintf( win_out,"      Initial icon position: %s, %s\n",
			xscale(wmhints->icon_x), yscale(wmhints->icon_y));

	if (flags & StateHint)
	  XmTextPrintf( win_out,"      Initial state is %s\n",
			Lookup(wmhints->initial_state, _state_hints));
}

