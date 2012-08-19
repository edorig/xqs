/****
 * WinTree.c
 ****
 * Written By: brett adams(bretta@jove.acs.unt.edu)
 * Description: WinTree.c -- functions and callbacks to control the window
 *	tree in the xqs prog, the wintree displays a list 
 ****
 ****/

#include "xqs.h"

void itemHighlightedCB();
int UpdateWinTree( Display *display, Window toplevel, Widget listtree );
extern void ListTreeGetHighlighted();
extern void PutWinInfo( Window window );
extern void PutPropInfo( Window window );


void
updateWinTreeCB( w, client, call )
Widget w;
XtPointer client;
XtPointer call;
{
  Window root;
  ListTreeItem *tree_item;

  if( info_dpy == NULL ) return;

  SetWidgetCursor( tree_form, XC_watch );
  SetStatus( "Retrieving window tree...." );

  while( (tree_item = ListTreeFirstItem(wintree)) != NULL )
       ListTreeDelete( wintree, tree_item );

  root = DefaultRootWindow( info_dpy );  
  UpdateWinTree( info_dpy, root, wintree );
  
  UnsetWidgetCursor( tree_form );   
  ClearStatus();

}


/* called when Mouse Button 2 is pressed on an item in the wintree
 * ListTree widget, popups a menu
 */
extern void ChangeOptionForm();
extern void o_PrefCB( Widget w, XtPointer, XtPointer );
extern Widget combo, o_pref_pop;


void winTreePopupCB( w, client, call )
  Widget w;
  XtPointer client;
  XtPointer call;
{
  /* fprintf( stderr, "winTreePopupCB called\n" ); */
  XgComboBoxSelectPos( combo, 4, True );
  ChangeOptionForm( 4 );
  o_PrefCB( o_pref_pop, NULL, NULL );  
  
}

int update_wintree( Display *, Window, Widget );
int UpdateWinTree( Display *d, Window toplevel, Widget wid )
{
  int n;

  ListTreeRefreshOff( wid );
  n = update_wintree( d, toplevel, wid );
  ListTreeRefreshOn( wid );
  ListTreeHighlightItem( wid, ListTreeFirstItem(wid) );
  return n;
}

int 
update_wintree( Display *display, Window toplevel, Widget listtree )
{
  int stat, i;
  static int position = 0;
  static int tree_level = 0;
  char buffer[256];
  static ListTreeItem *tree_item[50];

  unsigned int nkids;
  String win_name;
  Window root, parent, *kids;

  stat = XQueryTree( display, toplevel, &root, &parent, &kids, &nkids );
  if( stat == FALSE ){
       fprintf( stderr, 
		"Error -- UpdateWinTree(): Couldn't query window tree: 0x%x\n",
		(unsigned int)toplevel );
       return 0;
  }

  /* get the window name create buffer for wintree branches */
  stat = XFetchName( display, toplevel, &win_name );
  if( stat != 0 ){
       if( nkids > 0 )
	    sprintf( buffer, "0x%x,  \"%s\":  %u", (unsigned int)toplevel, 
		     win_name, nkids );
       else sprintf( buffer, "0x%x,  \"%s\"", (unsigned int)toplevel, 
		     win_name );
  }
  else{
       if( nkids > 0 )
	    sprintf( buffer, "0x%x:  %u", (unsigned int)toplevel, nkids );
       else sprintf( buffer, "0x%x", (unsigned int)toplevel );
  }
  
  if( tree_level == 0 )
       strcat( buffer, "(root)" );

  /* add the buffer to the wintree */
  if( tree_level == 0 )
       tree_item[tree_level] = ListTreeAdd( wintree, NULL, buffer );
  else tree_item[tree_level] = ListTreeAdd( wintree, tree_item[tree_level-1],
					    buffer );

  position++;
  
  tree_level++;
  for( i=0; i<nkids; i++ ){    
       UpdateWinTree( display, kids[i], listtree );    
  }
  tree_level--;  

  if( kids )  XFree( (char *)kids );
  if( win_name )  XFree( win_name );

  return position;

}/* end update_wintree() */


void InitWinTree()
{

  XmString xmstr;
  Widget tree_scroll, tree_label, update_butt;

  /*printf( "InitWinTree() called\n" );*/
  tree_form = XmCreateForm( work_area, "tree_form", NULL, 0 );
  XtManageChild( tree_form );
  
  wintree = XmCreateScrolledListTree( tree_form, "wintree", NULL, 0 );
  XtManageChild( wintree );
  XtAddCallback( wintree, XtNmenuCallback, winTreePopupCB, NULL );
  XtAddCallback( wintree, XtNhighlightCallback, itemHighlightedCB, NULL );

  tree_label = XmCreateLabelGadget( tree_form, "tree_label", NULL, 0 );
  XtManageChild( tree_label );

  update_butt = XmCreatePushButtonGadget( tree_form,"update_butt", NULL, 0 );
  XtManageChild( update_butt );
  XtAddCallback( update_butt, XmNactivateCallback, updateWinTreeCB, NULL );

  /* get the id of the scrolled window for wintree and win_out */
  tree_scroll = XtParent( wintree );
  
  /* resource for the form for the window tab */ 
  XtVaSetValues( tree_form,
		 XmNtopAttachment, XmATTACH_FORM,
		 XmNbottomAttachment, XmATTACH_FORM,
		 XmNleftAttachment, XmATTACH_FORM,	
		 XmNrightAttachment, XmATTACH_NONE,
		 XmNmarginWidth, 3,
		 XmNmarginHeight, 3,
		 NULL );

  /* create and set the resources for the label */
  xmstr = XmStringCreateSimple( "Id,   Name    : (# of children)" );
  XtVaSetValues( tree_label,
		 XmNlabelString, xmstr,
		 XmNtopAttachment, XmATTACH_FORM,
		 XmNbottomAttachment, XmATTACH_NONE,
		 XmNleftAttachment, XmATTACH_FORM,	
		 XmNrightAttachment, XmATTACH_NONE,
		 NULL );
  XmStringFree( xmstr );

  /* resource for the window tree */
  XtVaSetValues( wintree,
		 XtNmargin, 3,
		 XtNverticalSpacing, 3,
		 XtNclickPixmapToOpen, TRUE,
		 XtNlineWidth, 100,
		 NULL );  

  /* resources for the scrollbar */
  XtVaSetValues( tree_scroll,
		 XmNtopAttachment, XmATTACH_WIDGET,
		 XmNtopWidget, tree_label,
		 XmNbottomAttachment, XmATTACH_WIDGET,
		 XmNbottomWidget, update_butt,		 
		 XmNleftAttachment, XmATTACH_FORM,	
		 XmNrightAttachment, XmATTACH_NONE,
		 XmNscrollBarPlacement, XmBOTTOM_RIGHT,
		 NULL );

  xmstr = XmStringCreateSimple( "Update WinTree" );
  XtVaSetValues( update_butt,
		 XmNtopAttachment, XmATTACH_NONE,
		 XmNbottomAttachment, XmATTACH_FORM,
		 XmNleftAttachment, XmATTACH_FORM,
		 XmNrightAttachment, XmATTACH_FORM,
		 XmNlabelString, xmstr,
		 NULL );
  XmStringFree( xmstr);
}	       


Window GetHighlightedWindow( Widget w )
{
  int i;
  ListTreeMultiReturnStruct ret;
  char buffer[256];

  /* get the window id of the selected item */
  ListTreeGetHighlighted( wintree, &ret );
  if( ret.count < 1 ) return 0;
  
  for( i=0; ret.items[0][0].text[i] != ' ' &&
	    ret.items[0][0].text[i] != ',' && 
	    ret.items[0][0].text[i] != ':' &&
	    ret.items[0][0].text[i] != '\0'; 
       i++ )
  {
       buffer[i] = ret.items[0][0].text[i];

  }
  buffer[i] = '\0';

  return strtoul( buffer, (char **)NULL, 16 );

}


void
itemHighlightedCB( w, client, call )
  Widget w;
  XtPointer client;
  XtPointer call;
{
  int tab_num;
  Window window;

  if( info_dpy == NULL )   return;

  /* get the window id of the selected item */
  window = GetHighlightedWindow( w );

  tab_num = GetActiveTabNum();
  
  switch( tab_num ){
  case WINDOWS_T:
    PutWinInfo( window );
    break;
  case PROP_T:
    PutPropInfo( window );
    break;
  default:
    ;; /* nothing */
/*     fprintf( stderr, "Error: itemHighlightedCB()\n" */
/* 	     "-- no callback for selected tab --\n" ); */
    break;    
  }
    
}
