/*
 * include.h - Include file handling for cc65
 *
 * Ullrich von Bassewitz, 18.08.1998
 */



#ifndef INCLUDE_H
#define INCLUDE_H



/*****************************************************************************/
/*	      	     		     data		     		     */
/*****************************************************************************/



#define INC_SYS		0x0001		/* Add to system include path */
#define INC_USER	0x0002		/* Add to user include path */



/*****************************************************************************/
/*	      	     	     	     code		     		     */
/*****************************************************************************/



void AddIncludePath (const char* NewPath, unsigned Where);
/* Add a new include path to the existing one */

char* FindInclude (const char* Name, unsigned Where);
/* Find an include file. Return a pointer to a malloced area that contains
 * the complete path, if found, return 0 otherwise.
 */



/* End of include.h */
#endif



