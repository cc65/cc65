/*****************************************************************************/
/*                                                                           */
/*                                function.h                                 */
/*                                                                           */
/*                            Function management                            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2012, Ullrich von Bassewitz                                      */
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



#ifndef FUNCTION_H
#define FUNCTION_H

#include "coll.h"

/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/


/* Enumeration for function flags */
typedef enum {
    FF_NONE             = 0x0000,
    FF_HAS_RETURN       = 0x0001,       /* Function has a return statement */
    FF_IS_MAIN          = 0x0002,       /* This is the main function */
    FF_VOID_RETURN      = 0x0004,       /* Function returning void */
} funcflags_t;

/* Structure that holds all data needed for function activation */
struct Function {
    struct SymEntry*    FuncEntry;        /* Symbol table entry */
    Type*               ReturnType;       /* Function return type */
    FuncDesc*           Desc;             /* Function descriptor */
    int                 Reserved;         /* Reserved local space */
    unsigned            RetLab;           /* Return code label */
    int                 TopLevelSP;       /* SP at function top level */
    unsigned            RegOffs;          /* Register variable space offset */
    funcflags_t         Flags;            /* Function flags */
    Collection          LocalsBlockStack; /* Stack of blocks with local vars */
};

/* Structure that holds all data needed for function activation */
typedef struct Function Function;

/* Forward declaration */
struct FuncDesc;

/* Function activation data for current function (or NULL) */
extern Function* CurrentFunc;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



int F_CheckParamList (FuncDesc* D, int RequireAll);
/* Check and set the parameter sizes.
** If RequireAll is true, emit errors on parameters of incomplete types.
** Return true if all parameters have complete types.
*/

const char* F_GetFuncName (const Function* F);
/* Return the name of the current function */

unsigned F_GetParamCount (const Function* F);
/* Return the parameter count for the current function */

unsigned F_GetParamSize (const Function* F);
/* Return the parameter size for the current function */

Type* F_GetReturnType (Function* F);
/* Get the return type for the function */

int F_HasVoidReturn (const Function* F);
/* Return true if the function does not have a return value */

void F_ReturnFound (Function* F);
/* Mark the function as having a return statement */

int F_HasReturn (const Function* F);
/* Return true if the function contains a return statement*/

int F_IsMainFunc (const Function* F);
/* Return true if this is the main function */

int F_IsVariadic (const Function* F);
/* Return true if this is a variadic function */

int F_IsOldStyle (const Function* F);
/* Return true if this is an old style (K&R) function */

int F_HasOldStyleIntRet (const Function* F);
/* Return true if this is an old style (K&R) function with an implicit int return */

void F_SetRetLab (Function* F, unsigned NewRetLab);
/* Change the return jump label */

unsigned F_GetRetLab (const Function* F);
/* Return the return jump label */

int F_GetTopLevelSP (const Function* F);
/* Get the value of the stack pointer on function top level */

int F_ReserveLocalSpace (Function* F, unsigned Size);
/* Reserve (but don't allocate) the given local space and return the stack
** offset.
*/

int F_GetStackPtr (const Function* F);
/* Return the current stack pointer including reserved (but not allocated)
** space on the stack.
*/

void F_AllocLocalSpace (Function* F);
/* Allocate any local space previously reserved. The function will do
** nothing if there is no reserved local space.
*/

int F_AllocRegVar (Function* F, const Type* Type);
/* Allocate a register variable for the given variable type. If the allocation
** was successful, return the offset of the register variable in the register
** bank (zero page storage). If there is no register space left, return -1.
*/

void NewFunc (struct SymEntry* Func, struct FuncDesc* D);
/* Parse argument declarations and function body. */



/* End of function.h */

#endif
