/****
 * Prefs.h
 ****
 * Written By: brett adams (bretta@jove.acs.unt.edu)
 * Description: Prefs.h -- declarations for the preferences part of the xqs
 *	program
 ****
 ****/

#include "xqs.h"

#define N_DPY_PREFS	3
#define N_CLI_PREFS	2
#define N_WIN_PREFS	8
#define N_ACC_PREFS	0
#define N_PROP_PREFS	2
#define N_TREE_PREFS	2
#define N_OTHER_PREFS	N_PROP_PREFS + N_TREE_PREFS
#ifdef KBD_TAB
#define N_KEY_PREFS	0 
#endif

#define RCFILE     	".xqsrc"

typedef struct {
  char *name;
  char *class;
  Bool value;
}Preference;

typedef struct{
  Preference *dpyPrefs;
  Preference *cliPrefs;
  Preference *winPrefs;
  Preference *accPrefs;
  Preference *propPrefs;
  Preference *treePrefs;  
  XrmDatabase db;
} PreferenceDB;

PreferenceDB pref_db;
char *rcfile;

void SetPrefToggleStates();
void GetPrefToggleStates();
void GetPrefFromDB();
void SavePrefToDB();
void SavePrefDBtoFile();
