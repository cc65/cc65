/*
 * include.c - Include file handling for cc65
 *
 * Ullrich von Bassewitz, 18.08.1998
 */



#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "mem.h"
#include "include.h"



/*****************************************************************************/
/*	      	     		     data		     		     */
/*****************************************************************************/



static char* SysIncludePath  = 0;
static char* UserIncludePath = 0;



/*****************************************************************************/
/*	      	     	      	     code		     		     */
/*****************************************************************************/



static char* Add (char* Orig, char* New)
/* Create a new path from Orig and New, delete Orig, return the result */
{
    unsigned Len, NewLen;
    char* NewPath;

    /* Check for a trailing path separator and remove it */
    NewLen = strlen (New);
    if (NewLen > 0 && (New [NewLen-1] == '\\' || New [NewLen-1] == '/')) {
    	New [--NewLen] = '\0';
    }

    /* Calculate the length of the combined paths */
    if (Orig) {
       	Len = strlen (Orig) + NewLen;
    } else {
	Len = NewLen;
    }

    /* Allocate memory for the new string */
    NewPath = xmalloc (Len + 2);

    /* Copy the strings */
    if (Orig) {
	strcpy (NewPath, Orig);
    } else {
	NewPath [0] = '\0';
    }
    strcat (NewPath, New);
    strcat (NewPath, ";");

    /* Delete the original path */
    xfree (Orig);

    /* Return the new path */
    return NewPath;
}



static char* Find (char* Path, char* File)
/* Search for a file in a list of directories. If found, return the complete
 * name including the path in a malloced data area, if not found, return 0.
 */
{
    char* P;
    unsigned Count;
    int Max;
    char PathName [FILENAME_MAX];

    /* Initialize variables */
    Max = sizeof (PathName) - strlen (File) - 2;
    if (Max < 0) {
	return 0;
    }
    P = Path;

    /* Handle a NULL pointer as replacement for an empty string */
    if (P == 0) {
	P = "";
    }

    /* Start the search */
    while (*P) {
        /* Copy the next path element into the buffer */
     	Count = 0;
     	while (*P != '\0' && *P != ';' && Count < Max) {
     	    PathName [Count++] = *P++;
     	}

	/* Add a path separator and the filename */
	if (Count) {
     	    PathName [Count++] = '/';
	}
	strcpy (PathName + Count, File);

	/* Check if this file exists */
	if (access (PathName, R_OK) == 0) {
	    /* The file exists */
	    return xstrdup (PathName);
	}

	/* Skip a list separator if we have one */
	if (*P == ';') {
	    ++P;
	}
    }

    /* Not found */
    return 0;
}



void AddIncludePath (char* NewPath, unsigned Where)
/* Add a new include path to the existing one */
{
    /* Allow a NULL path */
    if (NewPath) {
     	if (Where & INC_SYS) {
     	    SysIncludePath = Add (SysIncludePath, NewPath);
     	}
     	if (Where & INC_USER) {
     	    UserIncludePath = Add (UserIncludePath, NewPath);
     	}
    }
}



char* FindInclude (char* Name, unsigned Where)
/* Find an include file. Return a pointer to a malloced area that contains
 * the complete path, if found, return 0 otherwise.
 */
{
    if (Where & INC_SYS) {
	/* Search in the system include directories */
	return Find (SysIncludePath, Name);
    }
    if (Where & INC_USER) {
	/* Search in the user include directories */
	return Find (UserIncludePath, Name);
    }
    return 0;
}



