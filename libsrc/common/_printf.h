/*
 * _printf.h
 *
 * (C) Copyright 1998 Ullrich von Bassewitz (uz@ibb.schwaben.com)
 *
 */



#ifndef __PRINTF_H
#define __PRINTF_H



/* Forward */
struct outdesc;

/* Type of the function that is called to output data */
typedef void (*outfunc) (struct outdesc* desc, char* buf, unsigned count);



struct outdesc {
    outfunc    	fout;		/* Routine used to output data */
    int		ccount;		/* Character counter */
    void*	ptr;        	/* Data internal to print routine */
    unsigned	uns;		/* Data internal to print routine */
};



/* Internal formatting routine */
int _printf (struct outdesc* d, const char* format, va_list ap);



/* End of _printf.h */
#endif



