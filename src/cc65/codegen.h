/*****************************************************************************/
/*                                                                           */
/*                                 codegen.h                                 */
/*                                                                           */
/*                            6502 code generator                            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2013, Ullrich von Bassewitz                                      */
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



#ifndef CODEGEN_H
#define CODEGEN_H



/* common */
#include "coll.h"

/* cc65 */
#include "segments.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Code generator flags.
** Note: The type flags are designed so that a smaller type may override a
** larger one by or'ing it into the existing one.
** Note^2: The actual type including the sign flag is in the lower bits, so
** we can mask the information and use them as a table index.
*/
#define CF_NONE         0x0000  /* No special flags */

/* Values for the actual type */
#define CF_CHAR         0x0003  /* Operation on characters */
#define CF_INT          0x0001  /* Operation on ints */
#define CF_PTR          CF_INT  /* Alias for readability */
#define CF_LONG         0x0000  /* Operation on longs */
#define CF_FLOAT        0x0004  /* Operation on a float */

/* Signedness */
#define CF_UNSIGNED     0x0008  /* Value is unsigned */

/* Masks for retrieving type information */
#define CF_TYPEMASK     0x0007  /* Type information */
#define CF_STYPEMASK    0x000F  /* Includes signedness */

#define CF_NOKEEP       0x0010  /* Value may get destroyed when storing */
#define CF_CONST        0x0020  /* Constant value available */
#define CF_CONSTADDR    0x0040  /* Constant address value available */
#define CF_TEST         0x0080  /* Test value */
#define CF_FIXARGC      0x0100  /* Function has fixed arg count */
#define CF_FORCECHAR    0x0200  /* Handle chars as chars, not ints */
#define CF_REG          0x0800  /* Value is in primary register */

/* Type of static address */
#define CF_ADDRMASK     0xF000  /* Type of address */
#define CF_STATIC       0x0000  /* Static local */
#define CF_EXTERNAL     0x1000  /* Static external */
#define CF_ABSOLUTE     0x2000  /* Numeric absolute address */
#define CF_LOCAL        0x4000  /* Auto variable */
#define CF_REGVAR       0x8000  /* Register variable */



/* Forward */
struct StrBuf;



/*****************************************************************************/
/*                         Files, pre- and postamble                         */
/*****************************************************************************/



void g_preamble (void);
/* Generate the assembler code preamble */

void g_fileinfo (const char* Name, unsigned long Size, unsigned long MTime);
/* If debug info is enabled, place a file info into the source */



/*****************************************************************************/
/*                              Segment support                              */
/*****************************************************************************/



void g_userodata (void);
/* Switch to the read only data segment */

void g_usedata (void);
/* Switch to the data segment */

void g_usebss (void);
/* Switch to the bss segment */

void g_segname (segment_t Seg);
/* Emit the name of a segment if necessary */



/*****************************************************************************/
/*                      Functions handling local labels                      */
/*****************************************************************************/



void g_defcodelabel (unsigned label);
/* Define a local code label */

void g_defdatalabel (unsigned label);
/* Define a local data label */

void g_aliasdatalabel (unsigned label, unsigned baselabel, long offs);
/* Define label as a local alias for baselabel+offs */



/*****************************************************************************/
/*                     Functions handling global labels                      */
/*****************************************************************************/



void g_defgloblabel (const char* Name);
/* Define a global label with the given name */

void g_defexport (const char* Name, int ZP);
/* Export the given label */

void g_defimport (const char* Name, int ZP);
/* Import the given label */

void g_importstartup (void);
/* Forced import of the startup segment */

void g_importmainargs (void);
/* Forced import of a special symbol that handles arguments to main */



/*****************************************************************************/
/*                                   stack                                   */
/*****************************************************************************/



int pop (unsigned flags);
/* Pop an argument of the given size */

int push (unsigned flags);
/* Push an argument of the given size */

unsigned sizeofarg (unsigned flags);
/* Return the size of a function argument type that is encoded in flags */



/*****************************************************************************/
/*                    type conversion and similiar stuff                     */
/*****************************************************************************/



void g_toslong (unsigned flags);
/* Make sure, the value on TOS is a long. Convert if necessary */

void g_tosint (unsigned flags);
/* Make sure, the value on TOS is an int. Convert if necessary */

void g_regint (unsigned Flags);
/* Make sure, the value in the primary register an int. Convert if necessary */

void g_reglong (unsigned Flags);
/* Make sure, the value in the primary register a long. Convert if necessary */

unsigned g_typeadjust (unsigned lhs, unsigned rhs);
/* Adjust the integer operands before doing a binary operation. lhs is a flags
** value, that corresponds to the value on TOS, rhs corresponds to the value
**  in (e)ax. The return value is the the flags value for the resulting type.
*/

unsigned g_typecast (unsigned lhs, unsigned rhs);
/* Cast the value in the primary register to the operand size that is flagged
** by the lhs value. Return the result value.
*/

void g_scale (unsigned flags, long val);
/* Scale the value in the primary register by the given value. If val is positive,
** scale up, is val is negative, scale down. This function is used to scale
** the operands or results of pointer arithmetic by the size of the type, the
** pointer points to.
*/



/*****************************************************************************/
/*                          Function entry and exit                          */
/*****************************************************************************/



void g_enter (unsigned flags, unsigned argsize);
/* Function prologue */

void g_leave (void);
/* Function epilogue */



/*****************************************************************************/
/*                            Register variables                             */
/*****************************************************************************/



void g_swap_regvars (int StackOffs, int RegOffs, unsigned Bytes);
/* Swap a register variable with a location on the stack */

void g_save_regvars (int RegOffs, unsigned Bytes);
/* Save register variables */

void g_restore_regvars (int StackOffs, int RegOffs, unsigned Bytes);
/* Restore register variables */



/*****************************************************************************/
/*                           Fetching memory cells                           */
/*****************************************************************************/



void g_getimmed (unsigned Flags, unsigned long Val, long Offs);
/* Load a constant into the primary register */

void g_getstatic (unsigned Flags, unsigned long Label, long Offs);
/* Fetch an static memory cell into the primary register */

void g_getlocal (unsigned Flags, int Offs);
/* Fetch specified local object (local var). */

void g_getind (unsigned Flags, unsigned Offs);
/* Fetch the specified object type indirect through the primary register
** into the primary register
*/

void g_leasp (int Offs);
/* Fetch the address of the specified symbol into the primary register */

void g_leavariadic (int Offs);
/* Fetch the address of a parameter in a variadic function into the primary
** register
*/



/*****************************************************************************/
/*                             Store into memory                             */
/*****************************************************************************/



void g_putstatic (unsigned flags, unsigned long label, long offs);
/* Store the primary register into the specified static memory cell */

void g_putlocal (unsigned Flags, int Offs, long Val);
/* Put data into local object. */

void g_putind (unsigned flags, unsigned offs);
/* Store the specified object type in the primary register at the address
** on the top of the stack
*/



/*****************************************************************************/
/*              Adds and subs of variables fix a fixed address               */
/*****************************************************************************/



void g_addlocal (unsigned flags, int offs);
/* Add a local variable to ax */

void g_addstatic (unsigned flags, unsigned long label, long offs);
/* Add a static variable to ax */



/*****************************************************************************/
/*                           Special op= functions                           */
/*****************************************************************************/



void g_addeqstatic (unsigned flags, unsigned long label, long offs,
                    unsigned long val);
/* Emit += for a static variable */

void g_addeqlocal (unsigned flags, int offs, unsigned long val);
/* Emit += for a local variable */

void g_addeqind (unsigned flags, unsigned offs, unsigned long val);
/* Emit += for the location with address in ax */

void g_subeqstatic (unsigned flags, unsigned long label, long offs,
                    unsigned long val);
/* Emit -= for a static variable */

void g_subeqlocal (unsigned flags, int offs, unsigned long val);
/* Emit -= for a local variable */

void g_subeqind (unsigned flags, unsigned offs, unsigned long val);
/* Emit -= for the location with address in ax */



/*****************************************************************************/
/*                 Add a variable address to the value in ax                 */
/*****************************************************************************/



void g_addaddr_local (unsigned flags, int offs);
/* Add the address of a local variable to ax */

void g_addaddr_static (unsigned flags, unsigned long label, long offs);
/* Add the address of a static variable to ax */



/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/



void g_save (unsigned flags);
/* Copy primary register to hold register. */

void g_restore (unsigned flags);
/* Copy hold register to primary. */

void g_cmp (unsigned flags, unsigned long val);
/* Immidiate compare. The primary register will not be changed, Z flag
** will be set.
*/

void g_test (unsigned flags);
/* Test the value in the primary and set the condition codes */

void g_push (unsigned flags, unsigned long val);
/* Push the primary register or a constant value onto the stack */

void g_swap (unsigned flags);
/* Swap the primary register and the top of the stack. flags give the type
** of *both* values (must have same size).
*/

void g_call (unsigned Flags, const char* Label, unsigned ArgSize);
/* Call the specified subroutine name */

void g_callind (unsigned Flags, unsigned ArgSize, int Offs);
/* Call subroutine indirect */

void g_jump (unsigned Label);
/* Jump to specified internal label number */

void g_truejump (unsigned flags, unsigned label);
/* Jump to label if zero flag clear */

void g_falsejump (unsigned flags, unsigned label);
/* Jump to label if zero flag set */

void g_drop (unsigned Space);
/* Drop space allocated on the stack */

void g_space (int space);
/* Create or drop space on the stack */

void g_cstackcheck (void);
/* Check for a C stack overflow */

void g_stackcheck (void);
/* Check for a stack overflow */

void g_add (unsigned flags, unsigned long val);
void g_sub (unsigned flags, unsigned long val);
void g_rsub (unsigned flags, unsigned long val);
void g_mul (unsigned flags, unsigned long val);
void g_div (unsigned flags, unsigned long val);
void g_mod (unsigned flags, unsigned long val);
void g_or (unsigned flags, unsigned long val);
void g_xor (unsigned flags, unsigned long val);
void g_and (unsigned flags, unsigned long val);
void g_asr (unsigned flags, unsigned long val);
void g_asl (unsigned flags, unsigned long val);
void g_neg (unsigned flags);
void g_bneg (unsigned flags);
void g_com (unsigned flags);
void g_inc (unsigned flags, unsigned long n);
void g_dec (unsigned flags, unsigned long n);
void g_eq (unsigned flags, unsigned long val);
void g_ne (unsigned flags, unsigned long val);
void g_lt (unsigned flags, unsigned long val);
void g_le (unsigned flags, unsigned long val);
void g_gt (unsigned flags, unsigned long val);
void g_ge (unsigned flags, unsigned long val);

void g_res (unsigned n);
/* Reserve static storage, n bytes */

void g_defdata (unsigned flags, unsigned long val, long offs);
/* Define data with the size given in flags */

void g_defbytes (const void* bytes, unsigned count);
/* Output a row of bytes as a constant */

void g_zerobytes (unsigned Count);
/* Output Count bytes of data initialized with zero */

void g_initregister (unsigned Label, unsigned Reg, unsigned Size);
/* Initialize a register variable from static initialization data */

void g_initauto (unsigned Label, unsigned Size);
/* Initialize a local variable at stack offset zero from static data */

void g_initstatic (unsigned InitLabel, unsigned VarLabel, unsigned Size);
/* Initialize a static local variable from static initialization data */



/*****************************************************************************/
/*                             Switch statement                              */
/*****************************************************************************/



void g_switch (Collection* Nodes, unsigned DefaultLabel, unsigned Depth);
/* Generate code for a switch statement */



/*****************************************************************************/
/*                       User supplied assembler code                        */
/*****************************************************************************/



void g_asmcode (struct StrBuf* B);
/* Output one line of assembler code. */



/* End of codegen.h */

#endif
