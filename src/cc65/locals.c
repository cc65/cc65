/*****************************************************************************/
/*                                                                           */
/*				   locals.c				     */
/*                                                                           */
/*		Local variable handling for the cc65 C compiler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2002 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@cc65.org                                                 */
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



/* common */
#include "xmalloc.h"
#include "xsprintf.h"

/* cc65 */
#include "anonname.h"
#include "asmlabel.h"
#include "codegen.h"
#include "declare.h"
#include "error.h"
#include "expr.h"
#include "function.h"
#include "global.h"
#include "symtab.h"
#include "locals.h"



/*****************************************************************************/
/*	     	    		     Data			 	     */
/*****************************************************************************/



/* Register variable management */
unsigned MaxRegSpace 	      	= 6; 	/* Maximum space available */
static unsigned RegOffs 	= 0;	/* Offset into register space */
static const SymEntry** RegSyms	= 0;    /* The register variables */
static unsigned RegSymCount 	= 0;    /* Number of register variables */



/*****************************************************************************/
/*		   		     Code		       		     */
/*****************************************************************************/



void InitRegVars (void)
/* Initialize register variable control data */
{
    /* If the register space is zero, bail out */
    if (MaxRegSpace == 0) {
       	return;
    }

    /* The maximum number of register variables is equal to the register
     * variable space available. So allocate one pointer per byte. This
     * will usually waste some space but we don't need to dynamically
     * grow the array.
     */
    RegSyms = (const SymEntry**) xmalloc (MaxRegSpace * sizeof (RegSyms[0]));
    RegOffs = MaxRegSpace;
}



void DoneRegVars (void)
/* Free the register variables */
{
    xfree (RegSyms);
    RegSyms = 0;
    RegOffs = MaxRegSpace;
    RegSymCount = 0;
}



static int AllocRegVar (const SymEntry* Sym, const type* tarray)
/* Allocate a register variable with the given amount of storage. If the
 * allocation was successful, return the offset of the register variable in
 * the register bank (zero page storage). If there is no register space left,
 * return -1.
 */
{
    /* Maybe register variables are disabled... */
    if (EnableRegVars) {

	/* Get the size of the variable */
	unsigned Size = CheckedSizeOf (tarray);

	/* Do we have space left? */
	if (RegOffs >= Size) {

	    /* Space left. We allocate the variables from high to low addresses,
	     * so the adressing is compatible with the saved values on stack.
	     * This allows shorter code when saving/restoring the variables.
	     */
	    RegOffs -= Size;
	    RegSyms [RegSymCount++] = Sym;
	    return RegOffs;
	}
    }

    /* No space left or no allocation */
    return -1;
}



static unsigned ParseAutoDecl (Declaration* Decl, unsigned Size, unsigned* SC)
/* Parse the declaration of an auto variable. The function returns the symbol
 * data, which is the offset for variables on the stack, and the label for
 * static variables.
 */
{
    unsigned Flags;
    unsigned SymData;

    /* Check if this is a variable on the stack or in static memory */
    if (StaticLocals == 0) {

        /* Change SC in case it was register */
        *SC = (*SC & ~SC_REGISTER) | SC_AUTO;
        if (CurTok.Tok == TOK_ASSIGN) {

            ExprDesc lval;

            /* Allocate previously reserved local space */
            F_AllocLocalSpace (CurrentFunc);

            /* Skip the '=' */
            NextToken ();

            /* Setup the type flags for the assignment */
            Flags = (Size == 1)? CF_FORCECHAR : CF_NONE;

            /* Get the expression into the primary */
            if (evalexpr (Flags, hie1, &lval) == 0) {
                /* Constant expression. Adjust the types */
                assignadjust (Decl->Type, &lval);
                Flags |= CF_CONST;
            } else {
                /* Expression is not constant and in the primary */
                assignadjust (Decl->Type, &lval);
            }

            /* Push the value */
            g_push (Flags | TypeOf (Decl->Type), lval.ConstVal);

            /* Mark the variable as referenced */
            *SC |= SC_REF;

            /* Variable is located at the current SP */
            SymData = oursp;

        } else {
            /* Non-initialized local variable. Just keep track of
             * the space needed.
             */
            SymData = F_ReserveLocalSpace (CurrentFunc, Size);
        }

    } else {

        /* Static local variables. */
        *SC = (*SC & ~(SC_REGISTER | SC_AUTO)) | SC_STATIC;

        /* Put them into the BSS */
        g_usebss ();

        /* Define the variable label */
        SymData = GetLocalLabel ();
        g_defdatalabel (SymData);

        /* Reserve space for the data */
        g_res (Size);

        /* Allow assignments */
        if (CurTok.Tok == TOK_ASSIGN) {

            ExprDesc lval;

            /* Skip the '=' */
            NextToken ();

            /* Setup the type flags for the assignment */
            Flags = (Size == 1)? CF_FORCECHAR : CF_NONE;

            /* Get the expression into the primary */
            if (evalexpr (Flags, hie1, &lval) == 0) {
                /* Constant expression. Adjust the types */
                assignadjust (Decl->Type, &lval);
                Flags |= CF_CONST;
                /* Load it into the primary */
                exprhs (Flags, 0, &lval);
            } else {
                /* Expression is not constant and in the primary */
                assignadjust (Decl->Type, &lval);
            }

            /* Store the value into the variable */
            g_putstatic (Flags | TypeOf (Decl->Type), SymData, 0);

            /* Mark the variable as referenced */
            *SC |= SC_REF;
        }
    }

    /* Return the symbol data */
    return SymData;
}



static unsigned ParseStaticDecl (Declaration* Decl, unsigned Size, unsigned* SC)
/* Parse the declaration of a static variable. The function returns the symbol
 * data, which is the asm label of the variable.
 */
{
    unsigned SymData;

    /* Static data */
    if (CurTok.Tok == TOK_ASSIGN) {

        /* Initialization ahead, switch to data segment */
        if (IsQualConst (Decl->Type)) {
            g_userodata ();
        } else {
            g_usedata ();
        }

        /* Define the variable label */
        SymData = GetLocalLabel ();
        g_defdatalabel (SymData);

        /* Skip the '=' */
        NextToken ();

        /* Allow initialization of static vars */
        ParseInit (Decl->Type);

        /* If the previous size has been unknown, it must be known now */
        if (Size == 0) {
            Size = SizeOf (Decl->Type);
        }

        /* Mark the variable as referenced */
        *SC |= SC_REF;

    } else {

        /* Uninitialized data, use BSS segment */
        g_usebss ();

        /* Define the variable label */
        SymData = GetLocalLabel ();
        g_defdatalabel (SymData);

        /* Reserve space for the data */
        g_res (Size);

    }

    /* Return the symbol data */
    return SymData;
}



static void ParseOneDecl (const DeclSpec* Spec)
/* Parse one variable declaration */
{
    unsigned    SC;      	/* Storage class for symbol */
    unsigned    Size;           /* Size of the data object */
    unsigned    SymData = 0;    /* Symbol data (offset, label name, ...) */
    Declaration Decl;		/* Declaration data structure */

    /* Remember the storage class for the new symbol */
    SC = Spec->StorageClass;

    /* Read the declaration */
    ParseDecl (Spec, &Decl, DM_NEED_IDENT);

    /* Set the correct storage class for functions */
    if (IsTypeFunc (Decl.Type)) {
	/* Function prototypes are always external */
	if ((SC & SC_EXTERN) == 0) {
       	    Warning ("Function must be extern");
	}
       	SC |= SC_FUNC | SC_EXTERN;

    }

    /* If we don't have a name, this was flagged as an error earlier.
     * To avoid problems later, use an anonymous name here.
     */
    if (Decl.Ident[0] == '\0') {
	AnonName (Decl.Ident, "param");
    }

    /* Handle anything that needs storage (no functions, no typdefs) */
    if ((SC & SC_FUNC) != SC_FUNC && (SC & SC_TYPEDEF) != SC_TYPEDEF) {

        /* Get the size of the variable */
        Size = SizeOf (Decl.Type);

        /* */
       	if (SC & (SC_AUTO | SC_REGISTER)) {

            /* Auto variable */
            SymData = ParseAutoDecl (&Decl, Size, &SC);

	} else if ((SC & SC_STATIC) == SC_STATIC) {

            /* Static variable */
            SymData = ParseStaticDecl (&Decl, Size, &SC);

       	}

	/* Cannot allocate a variable of zero size */
	if (Size == 0) {
	    Error ("Variable `%s' has unknown size", Decl.Ident);
	    return;
	}
    }

    /* If the symbol is not marked as external, it will be defined */
    if ((SC & SC_EXTERN) == 0) {
    	SC |= SC_DEF;
    }

    /* Add the symbol to the symbol table */
    AddLocalSym (Decl.Ident, Decl.Type, SC, SymData);
}



void DeclareLocals (void)
/* Declare local variables and types. */
{
    /* Remember the current stack pointer */
    int InitialStack = oursp;

    /* Loop until we don't find any more variables */
    while (1) {

	/* Check variable declarations. We need to distinguish between a
    	 * default int type and the end of variable declarations. So we
	 * will do the following: If there is no explicit storage class
	 * specifier *and* no explicit type given, it is assume that we
       	 * have reached the end of declarations.
	 */
	DeclSpec Spec;
	ParseDeclSpec (&Spec, SC_AUTO, T_INT);
       	if ((Spec.Flags & DS_DEF_STORAGE) != 0 && (Spec.Flags & DS_DEF_TYPE) != 0) {
	    break;
	}

	/* Accept type only declarations */
	if (CurTok.Tok == TOK_SEMI) {
	    /* Type declaration only */
	    CheckEmptyDecl (&Spec);
	    NextToken ();
	    continue;
	}

      	/* Parse a comma separated variable list */
      	while (1) {

	    /* Parse one declaration */
	    ParseOneDecl (&Spec);

	    /* Check if there is more */
     	    if (CurTok.Tok == TOK_COMMA) {
		/* More to come */
		NextToken ();
	    } else {
		/* Done */
     	      	break;
	    }
       	}

	/* A semicolon must follow */
	ConsumeSemi ();
    }

    /* Be sure to allocate any reserved space for locals */
    F_AllocLocalSpace (CurrentFunc);

    /* In case we've allocated local variables in this block, emit a call to
     * the stack checking routine if stack checks are enabled.
     */
    if (CheckStack && InitialStack != oursp) {
       	g_cstackcheck ();
    }
}



void RestoreRegVars (int HaveResult)
/* Restore the register variables for the local function if there are any.
 * The parameter tells us if there is a return value in ax, in that case,
 * the accumulator must be saved across the restore.
 */
{
    unsigned I, J;
    int Bytes, Offs;

    /* If we don't have register variables in this function, bail out early */
    if (RegSymCount == 0) {
    	return;
    }

    /* Save the accumulator if needed */
    if (!F_HasVoidReturn (CurrentFunc) && HaveResult) {
     	g_save (CF_CHAR | CF_FORCECHAR);
    }

    /* Walk through all variables. If there are several variables in a row
     * (that is, with increasing stack offset), restore them in one chunk.
     */
    I = 0;
    while (I < RegSymCount) {

	/* Check for more than one variable */
       	const SymEntry* Sym = RegSyms[I];
	Offs  = Sym->V.Offs;
	Bytes = CheckedSizeOf (Sym->Type);
	J = I+1;

       	while (J < RegSymCount) {

	    /* Get the next symbol */
	    const SymEntry* NextSym = RegSyms [J];

	    /* Get the size */
	    int Size = CheckedSizeOf (NextSym->Type);

	    /* Adjacent variable? */
	    if (NextSym->V.Offs + Size != Offs) {
	      	/* No */
	      	break;
	    }

	    /* Adjacent variable */
	    Bytes += Size;
	    Offs  -= Size;
	    Sym   = NextSym;
	    ++J;
	}

	/* Restore the memory range */
       	g_restore_regvars (Offs, Sym->V.Offs, Bytes);

	/* Next round */
	I = J;
    }

    /* Restore the accumulator if needed */
    if (!F_HasVoidReturn (CurrentFunc) && HaveResult) {
     	g_restore (CF_CHAR | CF_FORCECHAR);
    }
}



