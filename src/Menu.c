/****
 * Menu.c
 *****
 * Written By: brett adams (bretta@jove.acs.unt.edu)
 * Description: Menu.c
 *****
 ****/

#include "xqs.h"
#include "Prefs.h"

void d_OpenCB();
void d_OpenOkCB();
void d_CloseCB();
void o_PrefCB();
void o_PrefApplyCB();
void o_PrefOkCB();
void o_PrefCancelCB();
void o_SaveCB();
void h_AboutCB();
void disarmFamilyToggCB();
extern void ActivateCurrentTab();

extern int UpdateWinTree(
#if NeedFunctionPrototypes
			 Display *d,
			 Window toplevel,
			 Widget listree
#endif			 
);

Widget o_pref_pop, o_dpy_form, o_cli_form, o_win_form,  o_prop_frame,
  o_prop_form, o_acc_frame, o_tree_frame, o_tree_form, o_other_form, combo;
WidgetList dpy_togg, cli_togg, win_togg, acc_togg, tree_togg, prop_togg,
  other_togg;

#ifdef KBD_TAB
#define N_KEY_PREFS	0 
Widget o_key_frame;
WidgetList key_togg;
#endif

/* 
 * this is very ugly, but effective....==== CHANGE=====
 */
void ChangeOptionForm( int pos )
{
  /* supposedly these just return if the widget is already unmanaged */
  XtUnmanageChild( o_dpy_form );
  XtUnmanageChild( o_cli_form );
  XtUnmanageChild( o_win_form );	
  XtUnmanageChild( o_other_form );
  
 switch( pos ){
  case 1:
    XtManageChild( o_dpy_form );
    break;
  case 2:
    XtManageChild( o_cli_form );
    break;
  case 3:
    XtManageChild( o_win_form );
    break;
  case 4:
    XtManageChild( o_other_form );
    break;
  }
 
}

void optionComboChangedCB( w, client, CBstruct )
  Widget w;
  XtPointer client;
  XgComboBoxCallbackStruct *CBstruct;
{  
  /* printf( "pos: %d, %s\n", CBstruct->list_pos, CBstruct->value ); */

  /* unmanage the old preferences form by the first letter of the value name */
  switch( CBstruct->old_value[0] ){
  case 'D':
    XtUnmanageChild( o_dpy_form );
    break;
  case 'C':
    XtUnmanageChild( o_cli_form );
    break;
  case 'W':
    XtUnmanageChild( o_win_form );	
    break;
  case 'O':
    XtUnmanageChild( o_other_form );
  }

  /* manage the new preferences */
  switch( CBstruct->list_pos ){
  case 1:
    XtManageChild( o_dpy_form );
    break;
  case 2:
    XtManageChild( o_cli_form );
    break;
  case 3:
    XtManageChild( o_win_form );
    break;
  case 4:
    XtManageChild( o_other_form );
    break;
  }
    
}

String combo_strings[] = {
  "Display",
  "Clients",
  "Windows",
  "Other"
};


/* 
 * translations and function to call when Btn3Down is pressed in any of
 * the tab output boxes
 */
void pref_popup();


static XtActionsRec tabActionsTable [] = {
  { "pref_popup", pref_popup },
};


/* static void pref_popup( w, event, params, num_params ) */
/*   Widget w; */
/*   XEvent *event; */
/*   String *params; */
/*   int num_params;  */
void pref_popup( w, str )
  Widget w;
  String str;
{  
  int tab_num = GetActiveTabNum();

  /* printf( "pref_popup called\n" ); */

  if( tab_num == DISPLAY_T || tab_num == CLIENTS_T || tab_num == WINDOWS_T ){
       XgComboBoxSelectPos( combo, tab_num+1, True );
       ChangeOptionForm( tab_num+1 );
  }
  else {
       XgComboBoxSelectPos( combo, XtNumber(combo_strings), True );
       ChangeOptionForm( XtNumber(combo_strings) );
  }

  /*** probabaly shouldn't do this ***/
  o_PrefCB( o_pref_pop, NULL, NULL );

}




void InitOptionsPopup()
{
  Widget pop_form, o_pref_apply, o_pref_ok, o_pref_cancel, o_prop_label,
    o_tree_label;
  XmString combo_labels[4], xmstr;
  int i;

  XtAppAddActions( app_context, tabActionsTable, XtNumber(tabActionsTable) ); 

  for( i=0; i<XtNumber(combo_strings); i++ )
       combo_labels[i] = XmStringCreateLocalized( combo_strings[i] );

  xmstr = XmStringCreateLocalized( "Preferences" );
  o_pref_pop = XmCreateTemplateDialog( main_window, "o_pref_pop", NULL, 0 );
  XtVaSetValues( o_pref_pop, 
		 XmNdialogTitle, xmstr, 
		 XmNautoUnmanage, FALSE, 
		 NULL );
  XmStringFree(xmstr );

  pop_form = XtVaCreateManagedWidget( "o_pref_pop", xmFormWidgetClass,
				      o_pref_pop, 
				      XmNtopAttachment, XmATTACH_FORM,
				      XmNbottomAttachment, XmATTACH_FORM,
				      XmNrightAttachment, XmATTACH_FORM,
				      XmNleftAttachment, XmATTACH_FORM,
				      NULL );

  combo = XtVaCreateManagedWidget( "combo", xgComboBoxWidgetClass,
				   pop_form,
				   XmNtopAttachment, XmATTACH_FORM,
				   XmNbottomAttachment, XmATTACH_NONE,
				   XmNrightAttachment, XmATTACH_FORM,
				   XmNleftAttachment, XmATTACH_FORM,
				   XmNitemCount, XtNumber(combo_labels),
				   XmNitems, combo_labels,
				   XgNautoFillIn, TRUE,
				   XmNvisibleItemCount, XtNumber(combo_labels),
				   XmNeditable, FALSE,
				   NULL );
  XtAddCallback( combo, XmNvalueChangedCallback, optionComboChangedCB, NULL );

  xmstr = XmStringCreateLocalized( "Apply" );
  o_pref_apply = XtVaCreateManagedWidget( "o_pref_apply", 
					  xmPushButtonGadgetClass, o_pref_pop,
					  XmNlabelString, xmstr,
					  NULL );
  XtAddCallback( o_pref_apply, XmNactivateCallback, o_PrefApplyCB, NULL );
  XmStringFree( xmstr );

  xmstr = XmStringCreateLocalized( "Ok" );
  o_pref_ok = XtVaCreateManagedWidget( "o_pref_ok", 
				       xmPushButtonGadgetClass, o_pref_pop,
				       XmNlabelString, xmstr,
				       NULL );
  XtAddCallback( o_pref_ok, XmNactivateCallback, o_PrefOkCB, NULL );
  XmStringFree( xmstr );

  xmstr = XmStringCreateLocalized( "Cancel" );
  o_pref_cancel = XtVaCreateManagedWidget( "o_pref_cancel", 
					   xmPushButtonGadgetClass,o_pref_pop,
					   XmNlabelString, xmstr,
					   NULL );
  XtAddCallback( o_pref_cancel, XmNactivateCallback, o_PrefCancelCB, NULL );
  XmStringFree( xmstr );

  /**********
   *
   * individual forms and toggles for preferences popup
   *
   **********/
  /* 
   * display
   */
  dpy_togg = (Widget *)XtCalloc( N_DPY_PREFS, sizeof(Widget) );

  o_dpy_form = XtVaCreateWidget( "o_dpy_form", xmFormWidgetClass, pop_form, 
				 XmNtopAttachment, XmATTACH_WIDGET,
				 XmNtopWidget, combo,
				 XmNbottomAttachment, XmATTACH_FORM,
				 XmNleftAttachment, XmATTACH_FORM,
				 XmNrightAttachment, XmATTACH_FORM, 
				 NULL );
  
  xmstr = XmStringCreateLocalized( "Show general information" );
  dpy_togg[0] = XtVaCreateWidget( "dpy_gen_togg", 
				  xmToggleButtonGadgetClass, o_dpy_form,
				  XmNlabelString, xmstr,
				  XmNtopAttachment, XmATTACH_FORM,
				  NULL );
  XmStringFree( xmstr );

  xmstr = XmStringCreateLocalized( "Show per screen information" );
  dpy_togg[1] = XtVaCreateWidget( "dpy_scr_togg", 
				  xmToggleButtonGadgetClass, o_dpy_form,
				  XmNlabelString, xmstr,
				  XmNtopAttachment, XmATTACH_WIDGET,
				  XmNtopWidget, dpy_togg[0],
				  NULL );
  XmStringFree( xmstr );
  
  xmstr = XmStringCreateLocalized( "Show extension information" );
  dpy_togg[2] = XtVaCreateWidget( "dpy_mbuff_togg", 
				  xmToggleButtonGadgetClass,  o_dpy_form,
				  XmNlabelString, xmstr,
				  XmNtopAttachment, XmATTACH_WIDGET,
				  XmNtopWidget, dpy_togg[1],
				  NULL );
  XmStringFree( xmstr );
    
  XtManageChildren( dpy_togg, N_DPY_PREFS );
  XtManageChild( o_dpy_form );

  /*
   * clients 
   */

  cli_togg = (Widget *)XtCalloc( N_CLI_PREFS, sizeof(Widget) );

  o_cli_form = XtVaCreateWidget( "o_cli_form", xmFormWidgetClass, pop_form, 
				 XmNtopAttachment, XmATTACH_WIDGET,
				 XmNtopWidget, combo,
				 XmNbottomAttachment, XmATTACH_FORM,
				 XmNleftAttachment, XmATTACH_FORM,
				 XmNrightAttachment, XmATTACH_FORM, 
				 NULL );

  xmstr = XmStringCreateLocalized( "Show clients on all screen\n(else just the default screen)" );
  cli_togg[0] = XtVaCreateWidget( "cli_scr_togg", 
				  xmToggleButtonGadgetClass, o_cli_form,
				  XmNlabelString, xmstr,
				  XmNtopAttachment, XmATTACH_FORM,
				  NULL );
  XmStringFree( xmstr );

  xmstr = XmStringCreateLocalized( "Show icon name and class hints" );
  cli_togg[1] = XtVaCreateWidget( "cli_more_togg", 
				   xmToggleButtonGadgetClass, o_cli_form,
				   XmNlabelString, xmstr,
				   XmNtopAttachment, XmATTACH_WIDGET,
				   XmNtopWidget, cli_togg[0],
				   NULL );
  XmStringFree( xmstr );


  XtManageChildren( cli_togg, N_CLI_PREFS );

  /*
  * windows 
  */
  win_togg = (Widget *)XtCalloc( N_WIN_PREFS, sizeof(Widget) );

  o_win_form = XtVaCreateWidget( "o_win_form", xmFormWidgetClass, pop_form, 
				 XmNtopAttachment, XmATTACH_WIDGET,
				 XmNtopWidget, combo,
				 XmNbottomAttachment, XmATTACH_FORM,
				 XmNleftAttachment, XmATTACH_FORM,
				 XmNrightAttachment, XmATTACH_FORM, 
				 NULL );


  xmstr = XmStringCreateLocalized( "Show parent and children" );
  win_togg[0] = XtVaCreateWidget( "win_family_togg", 
			       xmToggleButtonGadgetClass, o_win_form,
			       XmNlabelString, xmstr,
			       XmNtopAttachment, XmATTACH_FORM,
			       NULL );
  XmStringFree( xmstr );

  /*  win_gkids_togg can only be sensitive if win_family toggle is selected */
  XtAddCallback( win_togg[0], XmNdisarmCallback, disarmFamilyToggCB, NULL );


  xmstr = XmStringCreateLocalized( "Show children's children" );
  win_togg[1] = XtVaCreateWidget( "win_gkids_togg", 
				  xmToggleButtonGadgetClass, o_win_form,
				  XmNlabelString, xmstr,
				  XmNtopAttachment, XmATTACH_WIDGET,
				  XmNtopWidget, win_togg[0],
				  NULL );
  XmStringFree( xmstr );
  

  xmstr = XmStringCreateLocalized( "Show window geomoetry" );
  win_togg[2] = XtVaCreateWidget( "win_stat_togg", 
				  xmToggleButtonGadgetClass, o_win_form,
				  XmNlabelString, xmstr,
				  XmNtopAttachment, XmATTACH_WIDGET,
				  XmNtopWidget, win_togg[1],
				  NULL );
  XmStringFree( xmstr );

  xmstr = XmStringCreateLocalized( "Show window pixel information" );
  win_togg[3] = XtVaCreateWidget( "win_bit_togg", 
				  xmToggleButtonGadgetClass, o_win_form,
				  XmNlabelString, xmstr,
				  XmNtopAttachment, XmATTACH_WIDGET,
				  XmNtopWidget, win_togg[2],
				  NULL );
  XmStringFree( xmstr );

  xmstr = XmStringCreateLocalized( "Show events selected for window" );
  win_togg[4] = XtVaCreateWidget( "win_ev_togg", 
				  xmToggleButtonGadgetClass, o_win_form,
				  XmNlabelString, xmstr,
				  XmNtopAttachment, XmATTACH_WIDGET,
				  XmNtopWidget, win_togg[3],
				  NULL );
  XmStringFree( xmstr );

  xmstr = XmStringCreateLocalized( "Show window manager hints" );
  win_togg[5] = XtVaCreateWidget( "win_wm_togg", 
				  xmToggleButtonGadgetClass, o_win_form,
				  XmNlabelString, xmstr,
				  XmNtopAttachment, XmATTACH_WIDGET,
				  XmNtopWidget, win_togg[4],
				  NULL );
  XmStringFree( xmstr );

  xmstr = XmStringCreateLocalized( "Show size hints" );
  win_togg[6] = XtVaCreateWidget( "win_size_togg", 
				  xmToggleButtonGadgetClass, o_win_form,
				  XmNlabelString, xmstr,
				  XmNtopAttachment, XmATTACH_WIDGET,
				  XmNtopWidget, win_togg[5],
				  NULL );
  XmStringFree( xmstr );

  xmstr = XmStringCreateLocalized( "Show shape extents" );
  win_togg[7] = XtVaCreateWidget( "win_shp_togg", 
				  xmToggleButtonGadgetClass, o_win_form,
				  XmNlabelString, xmstr,
				  XmNtopAttachment, XmATTACH_WIDGET,
				  XmNtopWidget, win_togg[6],
				  NULL );
  XmStringFree( xmstr );

  XtManageChildren( win_togg, N_WIN_PREFS );


  /*
   * other
   */

  o_other_form = XtVaCreateWidget( "o_other_form", 
				   xmFormWidgetClass, pop_form, 
				   XmNtopAttachment, XmATTACH_WIDGET,
				   XmNtopWidget, combo,
				   XmNbottomAttachment, XmATTACH_FORM,
				   XmNleftAttachment, XmATTACH_FORM,
				   XmNrightAttachment, XmATTACH_FORM,
				   NULL );
  
  /*
   * properties
   */

  prop_togg = (Widget *)XtCalloc( N_PROP_PREFS, sizeof(Widget) );

  o_prop_frame = XtVaCreateWidget( "o_prop_frame", 
				   xmFrameWidgetClass, o_other_form, 
/* 				   XmNtopAttachment, XmATTACH_WIDGET, */
/* 				   XmNtopWidget, o_ev_frame, */
				   XmNtopAttachment, XmATTACH_NONE,
				   XmNbottomAttachment, XmATTACH_NONE,
				   XmNleftAttachment, XmATTACH_FORM,
				   XmNrightAttachment, XmATTACH_FORM, 
				   NULL );

  xmstr = XmStringCreateLocalized( "Properties" );
  o_prop_label = XtVaCreateManagedWidget( "o_prop_label",
					  xmLabelGadgetClass, o_prop_frame,
					  XmNlabelString, xmstr,
					  XmNchildType, XmFRAME_TITLE_CHILD,
					  NULL );
  XmStringFree( xmstr );
  
  o_prop_form = XtVaCreateManagedWidget( "o_prop_form", 
					 xmFormWidgetClass, o_prop_frame, 
					 XmNtopAttachment, XmATTACH_FORM,
					 XmNbottomAttachment, XmATTACH_FORM,
					 XmNleftAttachment, XmATTACH_FORM,
					 XmNrightAttachment, XmATTACH_FORM,
					 XmNchildType, XmFRAME_WORKAREA_CHILD,
					 NULL );


  xmstr = XmStringCreateLocalized( "Display the type field" );
  prop_togg[0] = XtVaCreateWidget( "prop_type_togg",
				   xmToggleButtonGadgetClass, o_prop_form,
				   XmNlabelString, xmstr,
				   XmNtopAttachment, XmATTACH_FORM,
				   NULL );
  XmStringFree( xmstr );

  xmstr = XmStringCreateLocalized( "Ignore window manager frames" );
  prop_togg[1] = XtVaCreateWidget( "prop_frame_togg",
				   xmToggleButtonGadgetClass, o_prop_form,
				   XmNlabelString, xmstr,
 				   XmNtopAttachment, XmATTACH_WIDGET,
 				   XmNtopWidget, prop_togg[0],
				   NULL );
  XmStringFree( xmstr );

  XtManageChildren( prop_togg, N_PROP_PREFS );
  XtManageChild( o_prop_frame );

  /*
   * wintree
   */

  tree_togg = (Widget *)XtCalloc( N_TREE_PREFS, sizeof(Widget) );

  o_tree_frame = XtVaCreateWidget( "o_tree_form",
				   xmFrameWidgetClass, o_other_form, 
				   XmNtopAttachment, XmATTACH_WIDGET,
				   XmNtopWidget, o_prop_frame,
				   XmNbottomAttachment, XmATTACH_NONE,
				   XmNleftAttachment, XmATTACH_FORM,
				   XmNrightAttachment, XmATTACH_FORM, 
				   NULL );

  xmstr = XmStringCreateLocalized( "WinTree" );
  o_tree_label = XtVaCreateManagedWidget( "o_tree_label",
					  xmLabelGadgetClass, o_tree_frame,
					  XmNlabelString, xmstr,
					  XmNchildType, XmFRAME_TITLE_CHILD,
					  NULL );
  XmStringFree( xmstr );
  
  o_tree_form = XtVaCreateManagedWidget( "o_tree_form",
					 xmFormWidgetClass, o_tree_frame, 
					 XmNtopAttachment, XmATTACH_FORM,
					 XmNbottomAttachment, XmATTACH_FORM,
					 XmNleftAttachment, XmATTACH_FORM,
					 XmNrightAttachment, XmATTACH_FORM, 
					 NULL );

  xmstr = XmStringCreateLocalized( "Retrieve on startup" );
  tree_togg[0] = XtVaCreateWidget( "tree_startup_togg", 
				   xmToggleButtonGadgetClass, o_tree_form,
				   XmNlabelString, xmstr,
				   XmNtopAttachment, XmATTACH_FORM,
				   NULL );
  XmStringFree( xmstr );

  xmstr = XmStringCreateLocalized( "Retrieve when a new display is opened ");
  tree_togg[1] = XtVaCreateWidget( "tree_startup_togg", 
				   xmToggleButtonGadgetClass, o_tree_form,
				   XmNlabelString, xmstr,
				   XmNtopAttachment, XmATTACH_WIDGET,
				   XmNtopWidget, tree_togg[0],
				   NULL );
  XmStringFree( xmstr );  

  XtManageChildren( tree_togg, N_TREE_PREFS );
  XtManageChild( o_tree_frame );

  
  /*
   * access 
   */

/*   o_acc_frame = XtVaCreateWidget( "o_acc_frame",  */
/* 				  xmFrameWidgetClass, o_other_form,  */
/* 				  XmNtopAttachment, XmATTACH_WIDGET, */
/* 				  XmNtopWidget, o_tree_frame, */
/* 				  XmNbottomAttachment, XmATTACH_NONE, */
/* 				  XmNleftAttachment, XmATTACH_FORM, */
/* 				  XmNrightAttachment, XmATTACH_FORM,  */
/* 				  NULL ); */
/*   XtManageChild( o_acc_frame ); */
  

  /*** misc. option popup widget resources ***/
  XgComboBoxSelectPos(combo, 1, TRUE);

}


void 
disarmFamilyToggCB( w, client, call )
  Widget w;
  XtPointer client;
  XtPointer call;
{
  Bool is_set;
  XtVaGetValues( w, XmNset, &is_set, NULL );
  if( is_set )   XtSetSensitive( win_togg[1], TRUE );
  else { 
       XtSetSensitive( win_togg[1], FALSE );  
       XtVaSetValues( win_togg[1] , XmNset, FALSE, NULL );
  }

}

/***********
 *
 *  options menu functions
 * 
 ***********/

void
o_PrefCB( w, client, call )
  Widget w;
  XtPointer client;
  XtPointer call;
{ 
  SetPrefToggleStates(); 
  XtManageChild( o_pref_pop );
}


void
o_PrefApplyCB( w, client, call )
  Widget w;
  XtPointer client;
  XtPointer call;
{  
  GetPrefToggleStates();
  SavePrefToDB();
}

void
o_PrefOkCB( w, client, call )
  Widget w;
  XtPointer client;
  XtPointer call;
{
  GetPrefToggleStates();
  XtUnmanageChild( o_pref_pop );
}

void
o_PrefCancelCB( w, client, call )
  Widget w;
  XtPointer client;
  XtPointer call;
{
  XtUnmanageChild( o_pref_pop );
}


void
o_SaveCB( w, client, call )
  Widget w;
  XtPointer client;
  XtPointer call;
{  
  SavePrefDBtoFile();
}



/**********
 *
 * display menu functions
 *
 **********/

void
d_CloseCB( Widget w, XtPointer client, XtPointer call )
{
  STOP = TRUE;
  
  if( info_dpy != NULL ) {
       if( info_dpy != XtDisplay(appshell) )   XtCloseDisplay( info_dpy );
       info_dpy = NULL;
  }
  SetStatus( "No Display Open" );
}

void
d_OpenCB( Widget w, XtPointer client, XtPointer call )
{
  static Widget d_open_pop, pop_form, label, ok_butt, cancel_butt;
  XmString xmstr;


  if( ! d_open_pop ){
       
       d_open_pop = XmCreateTemplateDialog( main_window, "d_open_pop", 
					    NULL, 0 );
       
       xmstr = XmStringCreateLocalized( "Ok" );
       ok_butt = XtVaCreateManagedWidget( "d_open_ok",xmPushButtonGadgetClass,
					  d_open_pop, 
					  XmNlabelString, xmstr,
					  NULL );
       XtAddCallback( ok_butt, XtNactivateCallback, d_OpenOkCB, NULL );
       XmStringFree( xmstr );

       xmstr = XmStringCreateLocalized( "Cancel" );
       cancel_butt = XtVaCreateManagedWidget( "d_open_cancel",
					      xmPushButtonGadgetClass, 
					      d_open_pop,
					      XmNlabelString, xmstr,
					      NULL );
       XmStringFree( xmstr );
       
       pop_form = XtVaCreateManagedWidget( "pop_form", xmFormWidgetClass, 
					   d_open_pop, NULL );
       
       d_open_text = XtVaCreateManagedWidget( "d_open_text", 
					      xmTextFieldWidgetClass,pop_form,
					      XmNtopAttachment, XmATTACH_NONE,
					      XmNbottomAttachment,
					      XmATTACH_FORM,
					      XmNleftAttachment,XmATTACH_FORM,
					      XmNrightAttachment, 
					      XmATTACH_FORM,
					      NULL );

       xmstr = XmStringCreateLocalized( "Display Name:" );
       label = XtVaCreateManagedWidget( "d_open_label", xmLabelGadgetClass, 
					pop_form,
					XmNlabelString, xmstr,
					XmNtopAttachment, XmATTACH_FORM,
					XmNbottomAttachment, XmATTACH_WIDGET,
					XmNbottomWidget, d_open_text,
					XmNleftAttachment, XmATTACH_FORM,
					XmNrightAttachment, XmATTACH_FORM,
					XmNalignment, XmALIGNMENT_BEGINNING,
					XmNmarginHeight, 3,
					NULL );

       xmstr = XmStringCreateLocalized( "Connect to Display...  " );
       XtVaSetValues( d_open_pop,
		      XmNdialogTitle, xmstr,
		      XmNkeyboardFocusPolicy, XmEXPLICIT,
		      XmNinitialFocus, d_open_text,
		      NULL );
       XmStringFree( xmstr );
       
       XtRealizeWidget( d_open_pop );
    
       XmStringFree( xmstr );
  }

  XtManageChild( d_open_pop );
  
  XtCallAcceptFocus( d_open_text, CurrentTime );
  XtCallAcceptFocus( d_open_pop, CurrentTime );
  XtSetKeyboardFocus( pop_form, d_open_text );
  XtSetKeyboardFocus( d_open_text, d_open_text );
}




void
d_OpenOkCB( Widget w, XtPointer client, XtPointer call )
{
  char *d_name = XmTextFieldGetString(d_open_text);
  char str[64], **argv = NULL;
  int argc =0;
  Display *tmp_dpy;
  ListTreeItem *tree_item;

  sprintf( str, "Connecting to display: %s ....", d_name );
  SetStatus( str );
  
  XtSetSensitive( work_area, FALSE );
  tmp_dpy = XtOpenDisplay( app_context, d_name, "info_dpy", NULL, NULL, 0, 
			   &argc, argv );
  if( tmp_dpy ){
       if( info_dpy != NULL && info_dpy != XtDisplay(appshell) ) {
	    XtCloseDisplay( info_dpy );
	    /* info_dpy = NULL; */
       }
       info_dpy = tmp_dpy;
       ClearStatus();
       XtSetSensitive( work_area, TRUE );
       
       /* delete all items from the wintree */         
       while( (tree_item = ListTreeFirstItem(wintree)) != NULL )
	    ListTreeDelete( wintree, tree_item );
       /* ActivateCurrentTab(); */
       
       /* if retrieve-tree-when-new-display-is-opened is TRUE */
       if( pref_db.treePrefs[1].value == TRUE )
	    UpdateWinTree( info_dpy, DefaultRootWindow(info_dpy), wintree );
  }  
  else{
       XtSetSensitive( work_area, TRUE );
       SetStatus( "Could not open display." );
  }

}


/**********
 * 
 * help menu
 *
 **********/
void
h_AboutCB( w, client, call )
  Widget w;
  XtPointer client;
  XtPointer call;
{


}

void InitMenuBar()
{
  Widget dpy_butt, dpy_menu, d_open, d_close,
    opt_butt, opt_menu, o_pref, o_save;
    /* help_butt, help_menu, h_about; */
  XmString xmstr, accel_text;

  /*** display menu ***/

  dpy_butt = XmCreateCascadeButtonGadget( menu_bar, "dpy_butt", NULL, 0 );
  XtManageChild( dpy_butt );

  dpy_menu = XmCreatePulldownMenu( menu_bar, "dpy_menu", NULL, 0 );

  d_open = XmCreatePushButtonGadget( dpy_menu, "d_open", NULL, 0 );
  XtManageChild( d_open  );
  XtAddCallback( d_open, XmNactivateCallback, d_OpenCB, NULL ); 

  d_close = XmCreatePushButtonGadget( dpy_menu, "d_close", NULL, 0 );
  XtManageChild( d_close  );
  XtAddCallback( d_close, XmNactivateCallback, d_CloseCB, NULL ); 


  /*** option menu ***/

  opt_butt = XmCreateCascadeButtonGadget( menu_bar, "opt_butt", NULL, 0);
  XtManageChild( opt_butt );

  opt_menu = XmCreatePulldownMenu( menu_bar, "opt_menu", NULL, 0 );

  o_pref = XmCreatePushButtonGadget( opt_menu, "o_pref", NULL, 0);  
  XtManageChild( o_pref );
  XtAddCallback( o_pref, XmNactivateCallback, o_PrefCB, NULL );

  o_save = XmCreatePushButtonGadget( opt_menu, "opt_save", NULL, 0);
  XtManageChild( o_save );
  XtAddCallback( o_save, XmNactivateCallback, o_SaveCB, NULL );

  /*** help menu ***/

/*   help_butt = XmCreateCascadeButtonGadget( menu_bar, "help_butt", NULL, 0 ); */
/*   XtManageChild( help_butt ); */

/*   help_menu = XmCreatePulldownMenu( menu_bar, "help_menu", NULL, 0 ); */

/*   h_about = XmCreatePushButtonGadget( help_menu, "h_about", NULL, 0 ); */
/*   XtManageChild( h_about ); */
/*   XtAddCallback( h_about, XmNactivateCallback, h_AboutCB, NULL ); */

  /*** resources ***/

  XtVaSetValues( menu_bar,
		 XmNshadowThickness, 0,
		 NULL );

  /*****/

  xmstr = XmStringCreateLocalized( "Display" );
  XtVaSetValues( dpy_butt,
		 XmNlabelString, xmstr,
		 XmNsubMenuId, dpy_menu,
		 XmNshadowThickness, 1,
		 NULL );
  XmStringFree(xmstr); 

  XtVaSetValues( dpy_menu,
		 XmNshadowThickness, 1,
		 NULL );

  
  xmstr = XmStringCreateLocalized( "Open" );
  accel_text = XmStringCreateLocalized( "Ctrl-D" );
  XtVaSetValues( d_open,
		 XmNlabelString, xmstr,
		 XmNshadowThickness, 1,
		 XmNaccelerator, "Ctrl<Key>d",
		 XmNacceleratorText, accel_text,
		 NULL );
  XmStringFree(xmstr);
  XmStringFree(accel_text);

  xmstr = XmStringCreateLocalized( "Close" );
  XtVaSetValues( d_close,
		 XmNlabelString, xmstr,
		 XmNshadowThickness, 1,
		 NULL );
  XmStringFree(xmstr);

  /*****/

  xmstr = XmStringCreateLocalized( "Options" );
  XtVaSetValues( opt_butt,
		 XmNlabelString, xmstr,
		 XmNsubMenuId, opt_menu,
		 XmNshadowThickness, 1,
		 NULL );
  XmStringFree(xmstr); 

  XtVaSetValues( opt_menu,
		 XmNshadowThickness, 1,
		 NULL );

  xmstr = XmStringCreateLocalized( "Preferences" );
  accel_text = XmStringCreateLocalized( "Ctrl-P" );
  XtVaSetValues( o_pref,
		 XmNlabelString, xmstr,
		 XmNshadowThickness, 1,
		 XmNaccelerator, "Ctrl<Key>p",
		 XmNacceleratorText, accel_text,
		 NULL );
  XmStringFree(xmstr);
  XmStringFree(accel_text);

  xmstr = XmStringCreateLocalized( "Save Options" );
  accel_text = XmStringCreateLocalized( "Ctrl-O" );
  XtVaSetValues( o_save,
		 XmNlabelString, xmstr,
		 XmNshadowThickness, 1,
		 XmNaccelerator, "Ctrl<Key>o",
		 XmNacceleratorText, accel_text,
		 NULL );
  XmStringFree(xmstr);
  XmStringFree(accel_text);

  /*****/

/*   xmstr = XmStringCreateLocalized( "Help" ); */
/*   XtVaSetValues( help_butt, */
/* 		 XmNlabelString, xmstr, */
/* 		 XmNsubMenuId, help_menu, */
/* 		 XmNshadowThickness, 1, */
/* 		 NULL ); */
/*   XmStringFree(xmstr); */

/*   XtVaSetValues( help_menu, */
/* 		 XmNshadowThickness, 1, */
/* 		 NULL ); */

/*   xmstr = XmStringCreateLocalized( "About" ); */
/*   XtVaSetValues( h_about, */
/* 		 XmNlabelString, xmstr, */
/* 		 XmNshadowThickness, 1, */
/* 		 NULL ); */
/*   XmStringFree(xmstr);  */

}

