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

/* Input file table and number of open input files */
extern struct filent filetab[MAXFILES];
extern int ifile;

/* Current input file stream data */
extern FILE* inp;		/* Input file stream */
extern char* fin;		/* Input file name */
extern unsigned ln;		/* Line number */



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

int NextLine (void);
/* Get a line from the current input.  Returns -1 on end of file. */



/* End of io.h */

#endif



