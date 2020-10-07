/*****************************************************************************/
/*                                                                           */
/*                                 codeent.h                                 */
/*                                                                           */
/*                            Code segment entry                             */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2009, Ullrich von Bassewitz                                      */
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



#ifndef CODEENT_H
#define CODEENT_H



#include <string.h>

/* common */
#include "coll.h"
#include "inline.h"

/* cc65 */
#include "codelab.h"
#include "lineinfo.h"
#include "opcodes.h"
#include "reginfo.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Flags used */
#define CEF_USERMARK    0x0001U         /* Generic mark by user functions */
#define CEF_NUMARG      0x0002U         /* Insn has numerical argument */
#define CEF_DONT_REMOVE 0x0004U         /* Insn shouldn't be removed, marked by user functions */

/* Code entry structure */
typedef struct CodeEntry CodeEntry;
struct CodeEntry {
    unsigned char       OPC;            /* Opcode */
    unsigned char       AM;             /* Adressing mode */
    unsigned char       Size;           /* Estimated size */
    unsigned char       Flags;          /* Flags */
    char*               Arg;            /* Argument as string */
    unsigned long       Num;            /* Numeric argument */
    unsigned short      Info;           /* Additional code info */
    unsigned short      ArgInfo;        /* Additional argument info */
    unsigned int        Use;            /* Registers used */
    unsigned int        Chg;            /* Registers changed/destroyed */
    CodeLabel*          JumpTo;         /* Jump label */
    Collection          Labels;         /* Labels for this instruction */
    LineInfo*           LI;             /* Source line info for this insn */
    RegInfo*            RI;             /* Register info for this insn */
    char*               ArgBase;        /* Argument broken into a base and an offset, */
    long                ArgOff;         /* only done when requested. */
};

/* */
#define AIF_HAS_NAME        0x0001U     /* Argument has a name part */
#define AIF_HAS_OFFSET      0x0002U     /* Argument has a numeric part */
#define AIF_BUILTIN         0x0004U     /* The name is built-in */
#define AIF_EXTERNAL        0x0008U     /* The name is external */
#define AIF_LOCAL           0x0010U     /* The name is a local label */
#define AIF_ZP_NAME         0x0020U     /* The name is a zp location */
#define AIF_LOBYTE          0x0100U
#define AIF_HIBYTE          0x0200U
#define AIF_BANKBYTE        0x0400U
#define AIF_FAILURE         0x8000U     /* Argument was not parsed successfully */

#define AIF_WORD            (AIF_LOBYTE | AIF_HIBYTE)
#define AIF_FAR             (AIF_LOBYTE | AIF_HIBYTE | AIF_BANKBYTE)



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



int ParseOpcArgStr (const char* Arg, unsigned short* ArgInfo, struct StrBuf* Name, long* Offset);
/* Break the opcode argument string into a symbol name/label part plus an offset.
** Both parts are optional, but if there are any characters in the string that
** can't be parsed, it's an failure.
** The caller is responsible for managing the StrBuf.
** Return whether parsing succeeds or not.
*/

const char* MakeHexArg (unsigned Num);
/* Convert Num into a string in the form $XY, suitable for passing it as an
** argument to NewCodeEntry, and return a pointer to the string.
** BEWARE: The function returns a pointer to a static buffer, so the value is
** gone if you call it twice (and apart from that it's not thread and signal
** safe).
*/

void PreparseArg (CodeEntry* E);
/* Parse the argument string and memorize the result for the code entry */

CodeEntry* NewCodeEntry (opc_t OPC, am_t AM, const char* Arg,
                         CodeLabel* JumpTo, LineInfo* LI);
/* Create a new code entry, initialize and return it */

void FreeCodeEntry (CodeEntry* E);
/* Free the given code entry */

void CE_ReplaceOPC (CodeEntry* E, opc_t OPC);
/* Replace the opcode of the instruction. This will also replace related info,
** Size, Use and Chg, but it will NOT update any arguments or labels.
*/

int CodeEntriesAreEqual (const CodeEntry* E1, const CodeEntry* E2);
/* Check if both code entries are equal */

void CE_AttachLabel (CodeEntry* E, CodeLabel* L);
/* Attach the label to the entry */

void CE_ClearJumpTo (CodeEntry* E);
/* Clear the JumpTo entry and the argument (which contained the name of the
** label). Note: The function will not clear the backpointer from the label,
** so use it with care.
*/

#if defined(HAVE_INLINE)
INLINE int CE_HasLabel (const CodeEntry* E)
/* Check if the given code entry has labels attached */
{
    return (CollCount (&E->Labels) > 0);
}
#else
#  define CE_HasLabel(E)        (CollCount (&(E)->Labels) > 0)
#endif

#if defined(HAVE_INLINE)
INLINE unsigned CE_GetLabelCount (const CodeEntry* E)
/* Get the number of labels attached to this entry */
{
    return CollCount (&E->Labels);
}
#else
#  define CE_GetLabelCount(E)   CollCount (&(E)->Labels)
#endif

#if defined(HAVE_INLINE)
INLINE CodeLabel* CE_GetLabel (CodeEntry* E, unsigned Index)
/* Get a label from this code entry */
{
    return CollAt (&E->Labels, Index);
}
#else
#  define CE_GetLabel(E, Index) CollAt (&(E)->Labels, (Index))
#endif

#if defined(HAVE_INLINE)
INLINE void CE_ReplaceLabel (CodeEntry* E, CodeLabel* L, unsigned Index)
/* Replace the code label at the specified index with L */
{
    return CollReplace (&E->Labels, L, Index);
}
#else
#  define CE_ReplaceLabel(E, L, Index) CollReplace (&(E)->Labels, (L), (Index))
#endif

void CE_MoveLabel (CodeLabel* L, CodeEntry* E);
/* Move the code label L from it's former owner to the code entry E. */

#if defined(HAVE_INLINE)
INLINE int CE_HasMark (const CodeEntry* E)
/* Return true if the given code entry has the CEF_USERMARK flag set */
{
    return (E->Flags & CEF_USERMARK) != 0;
}
#else
#  define CE_HasMark(E) (((E)->Flags & CEF_USERMARK) != 0)
#endif

#if defined(HAVE_INLINE)
INLINE void CE_SetMark (CodeEntry* E)
/* Set the CEF_USERMARK flag for the given entry */
{
    E->Flags |= CEF_USERMARK;
}
#else
#  define CE_SetMark(E) ((E)->Flags |= CEF_USERMARK)
#endif

#if defined(HAVE_INLINE)
INLINE void CE_ResetMark (CodeEntry* E)
/* Reset the CEF_USERMARK flag for the given entry */
{
    E->Flags &= ~CEF_USERMARK;
}
#else
#  define CE_ResetMark(E)       ((E)->Flags &= ~CEF_USERMARK)
#endif

#if defined(HAVE_INLINE)
INLINE int CE_HasNumArg (const CodeEntry* E)
/* Return true if the instruction has a numeric argument */
{
    return (E->Flags & CEF_NUMARG) != 0;
}
#else
#  define CE_HasNumArg(E)       (((E)->Flags & CEF_NUMARG) != 0)
#endif

void CE_SetArg (CodeEntry* E, const char* Arg);
/* Replace the argument by the new one. */

void CE_SetArgBaseAndOff (CodeEntry* E, const char* ArgBase, long ArgOff);
/* Replace the new argument base and offset. Argument base is always applied.
** Argument offset is applied if and only if E has the AIF_HAS_OFFSET flag set.
*/

void CE_SetArgBase (CodeEntry* E, const char* ArgBase);
/* Replace the argument base by the new one.
** The entry must have an existing base.
*/

void CE_SetArgOffset (CodeEntry* E, long ArgOff);
/* Replace the argument offset by the new one */

void CE_SetNumArg (CodeEntry* E, long Num);
/* Set a new numeric argument for the given code entry that must already
** have a numeric argument.
*/

int CE_IsArgStrParsed (const CodeEntry* E);
/* Return true if the argument of E was successfully parsed last time */

int CE_HasArgBase (const CodeEntry* E);
/* Return true if the argument of E has a non-blank base name */

int CE_HasArgOffset (const CodeEntry* E);
/* Return true if the argument of E has a non-zero offset */

int CE_IsConstImm (const CodeEntry* E);
/* Return true if the argument of E is a constant immediate value */

int CE_IsKnownImm (const CodeEntry* E, unsigned long Num);
/* Return true if the argument of E is a constant immediate value that is
** equal to Num.
*/

#if defined(HAVE_INLINE)
INLINE int CE_IsCallTo (const CodeEntry* E, const char* Name)
/* Check if this is a call to the given function */
{
    return (E->OPC == OP65_JSR && strcmp (E->Arg, Name) == 0);
}
#else
#  define CE_IsCallTo(E, Name) ((E)->OPC == OP65_JSR && strcmp ((E)->Arg, (Name)) == 0)
#endif

int CE_UseLoadFlags (CodeEntry* E);
/* Return true if the instruction uses any flags that are set by a load of
** a register (N and Z).
*/

void CE_FreeRegInfo (CodeEntry* E);
/* Free an existing register info struct */

void CE_GenRegInfo (CodeEntry* E, RegContents* InputRegs);
/* Generate register info for this instruction. If an old info exists, it is
** overwritten.
*/

void CE_Output (const CodeEntry* E);
/* Output the code entry to the output file */



/* End of codeent.h */

#endif
