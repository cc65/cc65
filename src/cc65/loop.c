/*
 * loop.c
 *
 * Ullrich von Bassewitz, 20.06.1998
 */



#include "error.h"
#include "mem.h"
#include "loop.h"



/*****************************************************************************/
/*	  	  		     data		     		     */
/*****************************************************************************/



/* The root */
static struct loopdesc* loopstack = 0;



/*****************************************************************************/
/*	   	  		     code		     		     */
/*****************************************************************************/



struct loopdesc* addloop (unsigned sp, unsigned loop, unsigned label,
	     	  	  unsigned linc, unsigned lstat)
/* Create and add a new loop descriptor */
{
    struct loopdesc* l;

    /* Allocate a new struct */
    l = xmalloc (sizeof (struct loopdesc));

    /* Fill in the data */
    l->sp      	= sp;
    l->loop    	= loop;
    l->label   	= label;
    l->linc    	= linc;
    l->lstat   	= lstat;

    /* Insert it into the list */
    l->next = loopstack;
    loopstack = l;

    /* Return a pointer to the struct */
    return l;
}



struct loopdesc* currentloop (void)
/* Return a pointer to the descriptor of the current loop */
{
    if (loopstack == 0) {
	/* Stack is empty */
	Error (ERR_NO_ACTIVE_LOOP);
    }
    return loopstack;
}



void delloop (void)
/* Remove the current loop */
{
    struct loopdesc* l;

    l = loopstack;
    loopstack = loopstack->next;
    xfree (l);
}



