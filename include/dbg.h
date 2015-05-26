/*****************************************************************************/
/*                                                                           */
/*                                   dbg.h                                   */
/*                                                                           */
/*                         Debugger module interface                         */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2000, Ullrich von Bassewitz                                      */
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



/*
** This is the interface to the cc65 debugger. Since many of the functions
** used for the debugger are quite usable even in another context, they
** are declared here.
**
** To use the debugger, just call DbgInit in your application. Once it has
** been called, the debugger will catch any BRK opcode. Use the BREAK macro 
** defined below to insert breakpoints into your code.
**
** There are currently a lot of things that cannot be debugged, graphical
** applications are an example. The debugger does not save your screen
** contents, so even your text screen gets destroyed. However, you can
** debug the C and runtime library, even if the debugger is using this
** stuff itself.
**
** Note: When using the debugger, there are some other identifiers with
** external linkage, that start with Dbg. Avoid those names if you use the
** module.
*/



#ifndef _DBG_H
#define _DBG_H



/*****************************************************************************/
/*                             Utility functions                             */
/*****************************************************************************/



unsigned __fastcall__ DbgDisAsm (unsigned Addr, char* Buf, unsigned char Len);
/* Disassemble one instruction at address addr into the given buffer.
** The resulting line has the format, "AAAA__BB_BB_BB___OPC_OPERAND",
** where AAAA is the hexadecimal representation of addr, BB are the
** bytes (in hex) that make the instruction, OPC is the mnemonic, and
** OPERAND is an operand for the instruction.
** The buffer is filled with spaces up to the given length and terminated as
** a usual C string. NOTE: Buf must be able to hold Len+1 characters.
** The function returns the length of the disassembled instruction, so,
** to disassemble the next instruction, add the return value to addr
** and call the function again.
*/

unsigned __fastcall__ DbgDisAsmLen (unsigned Addr);
/* Disassemble one instruction, but do only return the length, do not
** create a visible representation. This function is useful when
** disassembling backwards, it is much faster than DbgDisAsm.
*/

int __fastcall__ DbgIsRAM (unsigned Addr);
/* Return true if we can read and write the given address */

char* __cdecl__ DbgMemDump (unsigned Addr, char* Buf, unsigned char Len);
/* Create a line of a memory dump in the given buffer. The buffer contains
** the starting address (4 digits hex), then Len bytes in this format:
** "AAAA__XX_YY_ZZ_...". The passed char buffer must hold Len*3+5 bytes
** plus a terminator byte.
** The function does not work correctly if the created string is longer
** than 255 bytes.
** The return value is Buf.
*/



/*****************************************************************************/
/*                         High level user interface                         */
/*****************************************************************************/



void __fastcall__ DbgInit (unsigned unused);
/* Initialize the debugger. Use 0 as parameter. The debugger will popup on
** next brk encountered.
*/

#define BREAK()         __asm__ ("brk")
/* Use this to insert breakpoints into your code */



/* End of dbg.h */
#endif



                                  
