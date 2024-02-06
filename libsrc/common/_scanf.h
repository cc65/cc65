/*
** _scanf.h
**
** (c) Copyright 2004, Ullrich von Bassewitz <uz@cc65.org>
**
*/



#ifndef __SCANF_H
#define __SCANF_H



/* Type of the function that is called to input data. The function will
** return EOF if no more data is available.
*/
typedef int __fastcall__ (*getfunc) (void* data);

/* Type of the function that is called to put back unused data */
typedef int __fastcall__ (*ungetfunc) (int c, void* data);



/* Control structure passed to the low level worker function.
** Beware: This structure is mirrored in the _scanf.inc assembler include
** file, so check this when altering the structure.
*/
struct scanfdata {
    getfunc     get;    /* Pointer to input routine */
    ungetfunc   unget;  /* Pointer to pushback routine */
    void*       data;   /* Pointer to struct. used outside of _scanf() */
};



/* Internal scanning routine */
int __fastcall__ _scanf (const struct scanfdata* d, const char* format, va_list ap);



/* End of _scanf.h */
#endif



