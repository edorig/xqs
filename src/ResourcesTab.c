/***** 
 * ResourcesTab.c 
 *****
 * Written By: brett adams (bretta@jove.acs.unt.edu)
 * Description: ResourcesTab.c 
 *****
 ****/

#include "xqs.h"
#include "Prefs.h"

Widget res_out;

void ResourcesTabActive()
{
  XtManageChild( res_frame );
  if( info_dpy == NULL )  return;
  
  XmTextClear( res_out );

  XmTextSetString( res_out,XResourceManagerString(info_dpy) );
  
}

void InitResourcesTab()
{
  Widget res_scroll;

  res_frame = XmCreateFrame( tab_form, "res_frame", NULL, 0 );

  res_out = XmCreateScrolledText( res_frame, "res_out", NULL, 0 );
  XtManageChild( res_out );

  res_scroll = XtParent( res_out );

  XtVaSetValues( res_frame,
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

  XtVaSetValues( res_scroll,
		 XmNtopAttachment, XmATTACH_FORM,
		 XmNrightAttachment, XmATTACH_FORM,
		 XmNleftAttachment, XmATTACH_FORM,
		 XmNbottomAttachment, XmATTACH_FORM,
		 XmNscrollBarPlacement, XmBOTTOM_RIGHT,
		 XmNchildType, XmFRAME_WORKAREA_CHILD,
		 NULL );
  
  XtVaSetValues( res_out,
		 XmNshadowThickness, 0,		 
		 XmNeditMode, XmMULTI_LINE_EDIT,
		 XmNeditable, FALSE,  			/* can't edit */
		 XmNcursorPositionVisible, FALSE, 	/* no cursor */
		 XmNscrollingPolicy, XmAUTOMATIC,
		 XmNwordWrap, TRUE,
		 NULL );
  
}
