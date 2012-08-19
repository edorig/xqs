/****
 * XbEvent.c
 ****
 * Written By: brett adams( bretta@jove.acs.unt.edu )
 * Description: helper and utility funtions to manipulate or retrieve 
 * 	information from an XEvent structure
 ****
 ****/

#include "Xb.h"

/* return the string of the passed message type */
String EventTypeToString( int type )
{
  char *event_types[] = { "",
			  "",
			  "KeyPress",
			  "KeyRelease",
			  "ButtonPress", 
			  "ButtonRelease",
			  "MotionNotify",
			  "EnterNotify",
			  "LeaveNotify",
			  "FocusIn",
			  "FocusOut",
			  "KeymapNotify",
			  "Expose",
			  "GraphicsExpose",
			  "NoExpose",
			  "VisibilityNotify",
			  "CreateNotify",
			  "DestroyNotify",
			  "UnmapNotify",
			  "MapNotify",
			  "MapRequest",
			  "ReparentNotify",
			  "ConfigureNotify",
			  "ConfigureRequest",
			  "GravityNotify",
			  "ResizeRequest",
			  "CirculateNotify",
			  "CirculateRequest",
			  "PropertyNotify",
			  "SelectionClear",
			  "SelectionRequest",
			  "SelectionNotify",
			  "ColormapNotify",
			  "ClientMessage",
			  "MappingNotify",
			  "LASTEvent" };
  return event_types[ type ];
}

/* return the string name of a notify events detail member */
String NotifyDetailToString( int detail )
{
  char *notify_detail[] = { "NotifyAncestor",
			   "NotifyVirtual",
			   "NotifyInferior",
			   "NotifyNonlinear",
			   "NotfiyNonlinearVirtual",
			   "NotifyPointer",
			   "NotifyPointerRoot",
			   "NotifyDetailNone" };
  return notify_detail[ detail ];
}

/* return the string name of a notify events mode member */
String NotifyModeToString( int mode )
{
  char *notify_mode[] = { "NotifyNormal",
			 "NotifyGrab",
			 "NotifyUngrab",
			 "NotifyWhileGrabbed" };


  return notify_mode[ mode ];
}
