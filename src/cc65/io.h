/*
 * io.h
 *
 * Ullrich von Bassewitz, 19.06.1998
 */



#ifndef IO_H
#define IO_H



#include <stdio.h>



/*****************************************************************************/
/*	 			     data				     */
/*****************************************************************************/



/* Maximum length of an input line and the corresponding char array */
#define LINEMAX		4095
#define LINESIZE 	LINEMAX+1

/* Maximum number of nested input files */
#define MAXFILES	16

/* Input line stuff */
extern char linebuf [LINESIZE];
extern char* line;
extern char* lptr;

/* File table entry */
struct filent {
    FILE* f_iocb;
    char* f_name;
    int   f_ln;
};



/*****************************************************************************/
/*	  	   		     code		     		     */
/*****************************************************************************/



void kill (void);
/* Reset input line pointer, clear input line */

int nch (void);
/* Get the next char in input stream (the one behind the current one) */

int cgch (void);
/* Get the current character in the input stream and advance line
 * pointer (unless already at end of line).
 */

int gch (void);
/* Get the current character in the input stream and advance line
 * pointer (no end of line check is performed).
 */



/* End of io.h */

#endif



