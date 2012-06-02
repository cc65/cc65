/*
 * Internal include file, do not use directly.
 * Written by Ullrich von Bassewitz. Based on code by Groepaz.
 */



#ifndef _DIR_H
#define _DIR_H



/*****************************************************************************/
/*	       	   	  	     Data	   			     */
/*****************************************************************************/



struct DIR {
    int	        fd;             /* File descriptor for directory */
    unsigned    off;            /* Current byte offset in directory */
    char        name[16+1];     /* Name passed to opendir */
};



/*****************************************************************************/
/*	       	   	  	     Code	   			     */
/*****************************************************************************/



unsigned char __fastcall__ _dirskip (unsigned char count, struct DIR* dir);
/* Skip bytes from the directory and make sure, errno is set if this isn't
 * possible. Return true if anything is ok and false otherwise. For
 * simplicity we assume that read will never return less than count if there
 * is no error and end-of-file is not reached.
 * Note: count must not be more than 254.
 */



/* End of dir.h */
#endif



