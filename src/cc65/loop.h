/*
 * loop.h
 *
 * Ullrich von Bassewitz, 20.06.1998
 */



#ifndef LOOP_H
#define LOOP_H



/*****************************************************************************/
/*	  	  		     data		     		     */
/*****************************************************************************/



struct loopdesc {
    struct loopdesc* 	next;
    unsigned 		sp;
    unsigned		loop;
    unsigned		label;
    unsigned		linc;
    unsigned		lstat;
};



/*****************************************************************************/
/*	   	  		     code		     		     */
/*****************************************************************************/



struct loopdesc* addloop (unsigned sp, unsigned loop, unsigned label,
	     	  	  unsigned linc, unsigned lstat);
/* Create and add a new loop descriptor */

struct loopdesc* currentloop (void);
/* Return a pointer to the descriptor of the current loop */

void delloop (void);
/* Remove the current loop */



/* End of loop.h */

#endif



