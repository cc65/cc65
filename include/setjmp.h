/*
 * setjmp.h
 *
 * Ullrich von Bassewitz, 06.06.1998
 *
 */



#ifndef _SETJMP_H
#define _SETJMP_H



typedef char jmp_buf [5];



int __fastcall__ _setjmp (jmp_buf buf);
#define setjmp	_setjmp		/* ISO insists on a macro */
void __fastcall__ longjmp (jmp_buf buf, int retval);



/* End of stddef.h */
#endif



