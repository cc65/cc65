
/* C I/O functions */

#include "global.h"
#include "io.h"



/*****************************************************************************/
/*	  	    		     data				     */
/*****************************************************************************/



/* Input line stuff */
char linebuf [LINESIZE];
char* line = linebuf;
char* lptr = 0;



/*****************************************************************************/
/*  	       	   		     code				     */
/*****************************************************************************/



int nch (void)
/* Get the next char in input stream (the one behind the current one) */
{
    if (*lptr == '\0') {
    	return 0;
    } else {
    	return lptr[1] & 0xFF;
    }
}



int cgch (void)
/* Get the current character in the input stream and advance line
 * pointer (unless already at end of line).
 */
{
    if (*lptr == '\0') {
   	return (0);
    } else {
   	return (*lptr++ & 0xFF);
    }
}



int gch (void)
/* Get the current character in the input stream and advance line
 * pointer (no end of line check is performed).
 */
{
    return (*lptr++ & 0xFF);
}



void kill (void)
/* Reset input line pointer, clear input line */
{
    lptr = line;
    *lptr = '\0';
}



