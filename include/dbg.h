/*
 * dbg.h
 *
 * Ullrich von Bassewitz, 08.08.1998
 *
 *
 * This is the interface to the cc65 debugger. Since many of the functions
 * used for the debugger are quite usable even in another context, they
 * are declared here.
 *
 * To use the debugger, just call DbgStart in your application. This will
 * clear the screen and startup the debugger with the program counter
 * pointing to the next instruction after the call to DbgStart. Once DbgStart
 * has been executed, the debugger will also catch any BRK opcode. Use the
 * BREAK function declared below to insert additional breakpoints into your
 * code.
 *
 * There are currently a lot of things that cannot be debugged, graphical
 * applications are an example. The debugger does not save your screen
 * contents, so even your text screen gets destroyed. However, you can
 * debug the C and runtime library, even if the debugger is using this
 * stuff itself.
 *
 * Note: When using the debugger, there are some other identifiers with
 * external linkage, that start with Dbg. Avoid those names if you use the
 * module.
 */



#ifndef _DBG_H
#define _DBG_H



/*****************************************************************************/
/*			      Utuility functions			     */
/*****************************************************************************/



unsigned __fastcall__ DbgDisAsm (unsigned Addr, char* Buf, unsigned char Len);
/* Disassemble one instruction at address addr into the given buffer.
 * The resulting line has the format, "AAAA__BB_BB_BB___OPC_OPERAND",
 * where AAAA is the hexadecimal representation of addr, BB are the
 * bytes (in hex) that make the instruction, OPC is the mnemonic, and
 * OPERAND is an operand for the instruction.
 * The buffer is filled with spaces up to the given length and terminated as
 * a usual C string. NOTE: Buf must be able to hold Len+1 characters.
 * The function returns the length of the disassembled instruction, so,
 * to disassemble the next instruction, add the return value to addr
 * and call the function again.
 */

unsigned __fastcall__ DbgDisAsmLen (unsigned Addr);
/* Disassemble one instruction, but do only return the length, do not
 * create a visible representation. This function is useful when
 * disassembling backwards, it is much faster than DbgDisAsm.
 */

int __fastcall__ DbgIsRAM (unsigned Addr);
/* Return true if we can read and write the given address */

char* DbgMemDump (unsigned Addr, char* Buf, unsigned char Len);
/* Create a line of a memory dump in the given buffer. The buffer contains
 * the starting address (4 digits hex), then Len bytes in this format:
 * "AAAA__XX_YY_ZZ_...". The passed char buffer must hold Len*3+5 bytes
 * plus a terminator byte.
 * The function does not work correctly if the created string is longer
 * than 255 bytes.
 * The return value is Buf.
 */



/*****************************************************************************/
/*			   High level user interface   			     */
/*****************************************************************************/



void __fastcall__ DbgInit (unsigned unused);
/* Initialize the debugger. Use 0 as parameter. The debugger will popup on
 * next brk encountered.
 */

#define BREAK()	       	__asm__ ("\tbrk")
/* Use this to insert breakpoints into your code */



/* End of dbg.h */
#endif



