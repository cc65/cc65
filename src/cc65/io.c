
/* C I/O functions */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include "asmcode.h"
#include "global.h"
#include "error.h"
#include "mem.h"
#include "codegen.h"
#include "optimize.h"
#include "io.h"



/*****************************************************************************/
/*	  	    		     data				     */
/*****************************************************************************/



/* Input line stuff */
char linebuf [LINESIZE];
char* line = linebuf;
char* lptr = 0;

/* Input file table and number of open input files */
struct filent filetab[MAXFILES];
int ifile = 0;

/* Current input file stream data */
FILE* inp = 0;
char* fin = 0;
unsigned ln = 0;



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



static void CloseInclude (void)
/* Close an include file and switch to the higher level file. Set inp to NULL
 * if this was the main file.
 */
{
    struct filent* pftab;

    /* Close the file */
    fclose(inp);

    /* Leave the include file */
    if (ifile > 0) {
	xfree (fin);
	inp = (pftab = &filetab[--ifile])->f_iocb;
	ln = pftab->f_ln;
	fin = pftab->f_name;
    } else {
	inp = 0;
    }
}



int NextLine (void)
/* Get a line from the current input.  Returns -1 on end of file. */
{
    unsigned	Len;
    unsigned	Part;
    unsigned	Start;
    int		Done;

    /* Setup the line */
    kill ();

    /* If there is no file open, bail out */
    if (inp == 0) {
	return 0;
    }

    /* Read lines until we get one with real contents */
    Len = 0;
    Done = 0;
    while (!Done && Len < LINESIZE) {

       	while (fgets (line + Len, LINESIZE - Len, inp) == 0) {

      	    /* eof */
      	    kill ();

      	    /* Leave the current file */
      	    CloseInclude ();

      	    /* If this was the last file, bail out */
	    if (inp == 0) {
	     	return 0;
	    }
       	}

	/* We got a new line */
	++ln;

	/* Remove the trailing newline if we have one */
	Part = strlen (line + Len);
	Start = Len;
	Len += Part;
	while (Len > 0 && line [Len-1] == '\n') {
	    --Len;
	}
	line [Len] = '\0';

	/* Output the source line in the generated assembler file
	 * if requested.
	 */
	if (AddSource && line[Start] != '\0') {
	    AddCodeLine ("; %s", line+Start);
	}

	/* Check if we have a line continuation character at the end. If not,
	 * we're done.
	 */
	if (Len > 0 && line[Len-1] == '\\') {
	    line[Len-1] = '\n';		/* Replace by newline */
	} else {
	    Done = 1;
	}
    }

    /* Got a line */
    return 1;
}



