/*
 * preproc.h
 *
 * Ullrich von Bassewitz, 07.06.1998
 */



#ifndef PREPROC_H
#define PREPROC_H



/*****************************************************************************/
/*	       			     data				     */
/*****************************************************************************/



/* Token table entry */
struct tok_elt {
    char *toknam;
    int toknbr;
};

/* Set when the pp calls expr() recursively */
extern unsigned char Preprocessing;



/*****************************************************************************/
/*	       			     code				     */
/*****************************************************************************/



void Preprocess (void);
/* Preprocess a line */



/* End of preproc.h */
#endif





