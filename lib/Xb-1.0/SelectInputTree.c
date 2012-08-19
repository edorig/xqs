/* SelectInputTree.c
 *****
 * Written By: brett adams( bretta@jove.acs.unt.edu )
 * Description: select the event mask for and entire window tree, same as 
 *	XSelectInput but select the entire tree
 *****
 */


#include "Xb.h"

/* select the event mask for an entire window tree(toplevel) */
void 
SelectInputTree( Display *disp, 
		 Window  toplevel, 
		 EventMask mask )
{
  
  Window *kids;
  Window root, parent;
  unsigned int nkids;  
  int i, stat;

  /* return the window tree for toplevel */
  stat = XQueryTree( disp, toplevel, &root, &parent, &kids, &nkids);
  if( stat == FALSE ){
    fprintf(  stderr, "Couldn't query window tree: 0x%x\n",
	      (unsigned int) toplevel );
    return;
  }

  /* return if toplevel has no children */
  if( nkids == 0 )
    return; 

  /* select the input mask for toplevel and its entire window tree */
  XSelectInput( disp, toplevel, mask );
  for( i=0; i<nkids; i++ ) {
    XSelectInput( disp, kids[i], mask );   
    SelectInputTree( disp, kids[i], mask );
  }

  /* free the list of children */
  XFree( (char *)kids );

  return;
    
}
