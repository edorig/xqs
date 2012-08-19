/****
 * Prefs.c
 ****
 * Written By: brett adams (bretta@jove.acs.unt.edu)
 * Description:  Prefs.c -- funtions and defintion to handle preference 
 * 	resources for the xqs program
 ****
 ****/

#include "Prefs.h"

extern WidgetList dpy_togg, cli_togg, win_togg, acc_togg, tree_togg,
  prop_togg; 

#ifdef KBD_TAB
extern WidgetList key_togg;
#endif

Bool StrToBool( String str );		/* dumb conversion routines */
String BoolToStr( Bool b );

/*
 * default values for preferences
 */

static Preference dpyPrefs[] = {
  { "dpyGeneral", "DpyGeneral", TRUE },
  { "dpyAllScreens", "DpyAllScreens", FALSE },
  { "dpyExtensions", "DpyExtensions", TRUE }
};

static Preference cliPrefs[] = {
  { "clientsAllScreens", "ClientsAllScreens", TRUE },
  { "clientsMore", "ClientsMore", TRUE }
};

static Preference winPrefs[] = {
  { "winChildren", "WinChildren", FALSE },
  { "winChildrensKids", "WinChildrensKids", FALSE },
  { "winGeometry", "WinGeometry", TRUE},
  { "winPixel", "WinPixel", TRUE},
  { "winEvents", "WinPixel", TRUE},
  { "winWMHints", "WinWMHints", TRUE},
  { "winSizeHints", "WinSizeHints", TRUE},
  { "winShapeHints", "WinShapeHints", TRUE}
};

static Preference propPrefs[] = {
  { "propType", "PropType", TRUE },
  { "propIgnoreFrames", "PropIgnoreFrames", TRUE },
};

static Preference treePrefs[] = {
  { "treeRetriveOnStart", "TreeRetrieveOnStart", TRUE },
  { "treeRetrieveWhenNewDisplay", "TreeRetrieveWhenNewDisplay", TRUE },
};

char *MiscRes[] = {
  "xqs*wintree.background: white",
  "xqs*geometry: 750x550",
  "xqs*display_out*translations:   <Btn3Down>:     pref_popup()",
  "xqs*clients_out*translations:   <Btn3Down>:     pref_popup()",
  "xqs*win_out*translations:       <Btn3Down>:     pref_popup()",
  "xqs*prop_out*translations:      <Btn3Down>:     pref_popup()",
  "xqs*res_out*translations:       <Btn3Down>:     pref_popup()",
  "xqs*access_out*translations:    <Btn3Down>:     pref_popup()",
};

void InitPreferenceDB()
{    
  int i;
  char *hdir;
  XrmDatabase db;

  pref_db.dpyPrefs = dpyPrefs;
  pref_db.winPrefs = winPrefs;
  pref_db.cliPrefs = cliPrefs;
  pref_db.propPrefs = propPrefs;
  pref_db.treePrefs = treePrefs;
  

  /* 
   * if an .xqsrc rcfile doesnt exist in the users home directory create one
   */
  hdir = getenv( "HOME" );
  rcfile = XtMalloc( strlen(hdir) + strlen(RCFILE) + 1 );
  sprintf( rcfile, "%s/%s", hdir, RCFILE );

  db = XtDatabase( XtDisplay(appshell) ); 
  for( i=0; i<XtNumber(MiscRes); i++ ) 
       XrmPutLineResource( &db, MiscRes[i] );

  pref_db.db = XrmGetFileDatabase( rcfile );
  GetPrefFromDB();  

}

void SavePrefDBtoFile() 
{  
  /* printf( "options saved\n" ); */
  SavePrefToDB();
  XrmPutFileDatabase( pref_db.db, rcfile );
}

void SavePrefToDB()
{
  int i; 

  for( i=0; i < N_DPY_PREFS; i++ )
       XrmPutStringResource( &pref_db.db, pref_db.dpyPrefs[i].name, 
 			     BoolToStr(pref_db.dpyPrefs[i].value ) );
  for( i=0; i < N_CLI_PREFS; i++ )
       XrmPutStringResource( &pref_db.db, pref_db.cliPrefs[i].name,
			     BoolToStr(pref_db.cliPrefs[i].value ) );
  for( i=0; i < N_WIN_PREFS; i++ )
       XrmPutStringResource( &pref_db.db, pref_db.winPrefs[i].name,
			     BoolToStr(pref_db.winPrefs[i].value ) );
  for( i=0; i < N_PROP_PREFS; i++ )
       XrmPutStringResource( &pref_db.db, pref_db.propPrefs[i].name,
			     BoolToStr(pref_db.propPrefs[i].value ) );
  for( i=0; i < N_TREE_PREFS; i++ )
       XrmPutStringResource( &pref_db.db, pref_db.treePrefs[i].name,
			     BoolToStr(pref_db.treePrefs[i].value ) );
}


#define STRING "String"
void GetPrefFromDB()
{
  int i;
  Bool stat;
  char *type;
  XrmValue val;

  for( i=0; i < N_DPY_PREFS; i++ ){
       stat = XrmGetResource( pref_db.db, pref_db.dpyPrefs[i].name, 
			      pref_db.dpyPrefs[i].class, &type, &val );  
       if( stat == True && strcmp(type,STRING) == 0 ) 
	    pref_db.dpyPrefs[i].value = StrToBool( val.addr );
  }

  for( i=0; i < N_CLI_PREFS; i++ ){
       stat = XrmGetResource( pref_db.db, pref_db.cliPrefs[i].name, 
			      pref_db.cliPrefs[i].class, &type, &val );
       if( stat == True && strcmp(type,STRING) == 0 )
	    pref_db.cliPrefs[i].value = StrToBool( val.addr );              
  }

  for( i=0; i < N_WIN_PREFS; i++ ){
       stat = XrmGetResource( pref_db.db, pref_db.winPrefs[i].name, 
			      pref_db.winPrefs[i].class, &type, &val );  
       if( stat == True && strcmp(type,STRING) == 0 )
 	    pref_db.winPrefs[i].value = StrToBool( val.addr );               
  }

  for( i=0; i < N_PROP_PREFS; i++ ){
       stat = XrmGetResource( pref_db.db, pref_db.propPrefs[i].name, 
			      pref_db.propPrefs[i].class, &type, &val );  
       if( stat == True && strcmp(type,STRING) == 0 )
 	    pref_db.propPrefs[i].value = StrToBool( val.addr );               
  }
  
  for( i=0; i < N_TREE_PREFS; i++ ){
       stat = XrmGetResource( pref_db.db, pref_db.treePrefs[i].name, 
			      pref_db.treePrefs[i].class, &type, &val );  
       if( stat == True && strcmp(type,STRING) == 0 )
 	    pref_db.treePrefs[i].value = StrToBool( val.addr );               
  }


}

void GetPrefToggleStates()
{
  int i;

  for( i=0; i < N_DPY_PREFS; i++ )
       XtVaGetValues( dpy_togg[i], XmNset, &pref_db.dpyPrefs[i].value, NULL );
  for( i=0; i < N_CLI_PREFS; i++ )
       XtVaGetValues( cli_togg[i], XmNset, &pref_db.cliPrefs[i].value, NULL );
  for( i=0; i < N_WIN_PREFS; i++ )
       XtVaGetValues( win_togg[i], XmNset, &pref_db.winPrefs[i].value, NULL );
  for( i=0; i < N_PROP_PREFS; i++ )
       XtVaGetValues( prop_togg[i], XmNset, &pref_db.propPrefs[i].value, NULL);
  for( i=0; i < N_TREE_PREFS; i++ )
       XtVaGetValues( tree_togg[i], XmNset, &pref_db.treePrefs[i].value, NULL);

}

void SetPrefToggleStates()
{
  int i;
  
  for( i=0; i < N_DPY_PREFS; i++ )
       XtVaSetValues( dpy_togg[i], XmNset, pref_db.dpyPrefs[i].value, NULL );
  for( i=0; i < N_CLI_PREFS; i++ )
       XtVaSetValues( cli_togg[i], XmNset, pref_db.cliPrefs[i].value, NULL );
  for( i=0; i < N_WIN_PREFS; i++ )
       XtVaSetValues( win_togg[i], XmNset, pref_db.winPrefs[i].value, NULL );
  for( i=0; i < N_PROP_PREFS; i++ )
       XtVaSetValues( prop_togg[i], XmNset, pref_db.propPrefs[i].value, NULL );
  for( i=0; i < N_TREE_PREFS; i++ )
       XtVaSetValues( tree_togg[i], XmNset, pref_db.treePrefs[i].value, NULL );

  /* winPref[1] can only be set if winPref[0] is set */
  if( pref_db.winPrefs[0].value == FALSE ){
       XtSetSensitive( win_togg[1], FALSE );  
       XtVaSetValues( win_togg[1] , XmNset, FALSE, NULL );
  }     
}


/* a dumb str to bool funtion */
Bool StrToBool(  char *str )
{
  if( str[0] == 't' || str[0] == 'T' )
       return TRUE;
  return FALSE;
}

/* a dumb bool to str funtion */
String BoolToStr(  Bool b )
{
  return b ? "True": "False";
}
