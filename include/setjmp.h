/*****************************************************************************/
/*                                                                           */
/*                                 setjmp.h                                  */
/*                                                                           */
/*                              Nonlocal jumps                               */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2009, Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
/*                D-70794 Filderstadt                                        */
/* EMail:         uz@cc65.org                                                */
/*                                                                           */
/*                                                                           */
/* This software is provided 'as-is', without any expressed or implied       */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.                                    */
/*                                                                           */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:                            */
/*                                                                           */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated but is not required.                                       */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.                      */
/* 3. This notice may not be removed or altered from any source              */
/*    distribution.                                                          */
/*                                                                           */
/*****************************************************************************/



#ifndef _SETJMP_H
#define _SETJMP_H



typedef char jmp_buf [5];

/* 	The cc65 assembly implementation of setjmp and longjmp expect that the jmp_buf is passed by reference.
	"setjmp" receives the pointer to the "jmp_buf" in __AX__ while longjmp receives it with popax from the
	parameter stack.
	This is quite unconventional as normally the jmp_buf is passed by value which conforms also with the cc65
	header "setjmp.h".

	see: http://www.cplusplus.com/reference/csetjmp/setjmp/
	and: http://www.cplusplus.com/reference/csetjmp/longjmp/

	Due to this discrepancy between the original cc65 header and cc65 assembly implementation problems arise
	using "setjmp.h" functions.
	For example, if you dereference a pointer to a jmp_buf and pass it to longjmp the code will not perform.

	Therefore following changes are suggested:

	- Rename the function "longjmp" into "_longjmp" like it was done for setjmp.
	- Change the prototypes of "_longjmp" and "_setjmp" to receive jmp_buf "by reference" instead of "by value".
	- Hide the functions prototypes with the macros "setjmp(jmp_buf)" and "longjmp(jmp_buf, retval)" taking a jmp_buf by value.
	- Use the macro to convert jmp_buf to address of jmp_buf.

	*/

int __fastcall__ _setjmp (jmp_buf *buf);	//	non standard "call by ref" version to workaround cc65's incomplete
											//	implementation for passing arrays/structs by value
void __fastcall__ _longjmp (jmp_buf *buf, int retval) __attribute__((noreturn));
											//	non standard "call by ref" version to workaround cc65's incomplete
											//	implementation for passing arrays/structs by value

#define setjmp(x)  _setjmp(&(x))        /*	ISO insists on a macro.
												provides call by value interface for functions expecting call by ref*/

#define longjmp(x,y)  _longjmp(&(x),(y))/*	non standard macro.
												provides call by value interface for functions expecting call by ref*/


/*	From old header

int __fastcall__ _setjmp (jmp_buf buf);
#define setjmp  _setjmp         // ISO insists on a macro
void __fastcall__ longjmp (jmp_buf buf, int retval) __attribute__((noreturn));

*/

/* End of setjmp.h */
#endif



