/*
** _printf.h
**
** (C) Copyright 1998 Ullrich von Bassewitz (uz@cc65.org)
**
*/



#ifndef __PRINTF_H
#define __PRINTF_H



/* Forward */
struct outdesc;

/* Type of the function that is called to output data */
typedef void __cdecl__ (* outfunc) (struct outdesc* desc, const char* buf, unsigned count);



/* Control structure passed to the low level worker function.
** Beware: This function will access the structure on the assembly level,
** so check this when altering the structure.
*/
struct outdesc {
    int         ccount;         /* Character counter */
    outfunc     fout;           /* Routine used to output data */
    void*       ptr;            /* Data internal to print routine */
    unsigned    uns;            /* Data internal to print routine */
};



/* Internal formatting routine */
void __fastcall__ _printf (struct outdesc* d, const char* format, va_list ap);



/* End of _printf.h */
#endif



