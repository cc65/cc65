/*
 * _scanf.h
 *
 * (C) Copyright 2001 Ullrich von Bassewitz (uz@cc65.org)
 *
 */



#ifndef __SCANF_H
#define __SCANF_H



/* Forward */
struct indesc;

/* Type of the function that is called to input data */
typedef void (*infunc) (struct indesc* desc, const char* buf, unsigned count);



/* Control structure passed to the low level worker function.
 * Beware: The low level functions will access the structure on the assembly
 * level, so check this when altering the structure.
 */
struct indesc {
    char*      	buf;		/* Pointer to input buffer */
    unsigned	size;		/* Size of input buffer */
    unsigned	fill;		/* Fill mark of input buffer */
    unsigned	ridx;		/* Read index of input buffer */
    infunc	fin;		/* Pointer to input routine */
};



/* Internal scanning routine */
int _scanf (struct indesc* d, const char* format, va_list ap);



/* End of _scanf.h */
#endif



