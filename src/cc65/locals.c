/*****************************************************************************/
/*                                                                           */
/*				   locals.c				     */
/*                                                                           */
/*		Local variable handling for the cc65 C compiler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000     Ullrich von Bassewitz                                        */
/*              Wacholderweg 14                                              */
/*              D-70597 Stuttgart                                            */
/* EMail:       uz@musoftware.de                                             */
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



#include "../common/xmalloc.h"

#include "anonname.h"
#include "asmlabel.h"
#include "codegen.h"
#include "declare.h"
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
    RegSyms = xmalloc (MaxRegSpace * sizeof (RegSyms[0]));
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
	unsigned Size = SizeOf (tarray);

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



void DeclareLocals (void)
/* Declare local variables and types. */
{
    int offs = oursp;  	      	/* Current stack offset for variable */
    int AutoSpace = 0; 		/* Unallocated space on the stack */
    int Size;	       	      	/* Size of an auto variable */
    int Reg; 	       		/* Register variable offset */
    unsigned flags = 0;		/* Code generator flags */
    int SymbolSC;      		/* Storage class for symbol */
    int ldata = 0;     		/* Local symbol data temp storage */

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
	if (curtok == TOK_SEMI) {
	    /* Type declaration only */
	    CheckEmptyDecl (&Spec);
	    NextToken ();
	    continue;
	}

      	/* Parse a comma separated variable list */
      	while (1) {

      	    Declaration Decl;

      	    /* Remember the storage class for the new symbol */
      	    SymbolSC = Spec.StorageClass;

      	    /* Read the declaration */
      	    ParseDecl (&Spec, &Decl, DM_NEED_IDENT);

      	    /* If we don't have a name, this was flagged as an error earlier.
      	     * To avoid problems later, use an anonymous name here.
      	     */
       	    if (Decl.Ident[0] == '\0') {
      	    	AnonName (Decl.Ident, "param");
      	    }

       	    if (!IsFunc (Decl.Type) && (SymbolSC & SC_TYPEDEF) != SC_TYPEDEF) {

	     	/* Get the size of the variable */
	     	Size = SizeOf (Decl.Type);

#if 0
    	      	/* Check the storage class */
    	      	if ((SymbolSC & SC_REGISTER) && (Reg = AllocRegVar (psym, tarray)) >= 0) {

    	      	    /* We will store the current value of the register onto the
    	      	     * stack, thus making functions with register variables
    	      	     * reentrant. If we have pending auto variables, emit them
    	     	     * now.
    	      	     */
    	      	    g_usecode ();
    	     	    g_space (AutoSpace);
    	     	    oursp -= AutoSpace;
    	     	    AutoSpace = 0;

    	     	    /* Remember the register bank offset */
    	     	    ldata = Reg;

    	     	    /* Save the current register value onto the stack */
    	     	    g_save_regvars (Reg, Size);

    	     	    /* Allow variable initialization */
    	      	    if (curtok == TOK_ASSIGN) {

    	      	       	struct expent lval;

    	      	       	/* Skip the '=' */
    	      	       	NextToken ();

    	      	       	/* Get the expression into the primary */
	    		expression1 (&lval);

    	     		/* Make type adjustments if needed */
    	     		assignadjust (tarray, &lval);

    	      	     	/* Setup the type flags for the assignment */
	    		flags = TypeOf (tarray) | CF_REGVAR;
	    		if (Size == 1) {
	    		    flags |= CF_FORCECHAR;
	    		}

       	       	       	/* Store the value into the register */
       	       	       	g_putstatic (flags, Reg, 0);

     	      	       	/* Mark the variable as referenced */
     	      	       	SymbolSC |= SC_REF;

     	     	    }

     	     	    /* Account for the stack space needed and remember the
     	     	     * stack offset of the save area.
     	     	     */
     	     	    offs -= Size;
     	     	    psym->h_lattr = offs;

     	      	} else if (SymbolSC & (SC_AUTO | SC_REGISTER)) {
#endif
      	      	if (SymbolSC & (SC_AUTO | SC_REGISTER)) {

    	     	    /* Auto variable */
	    	    if (LocalsAreStatic == 0) {

			/* Change SC in case it was register */
       	       	       	SymbolSC = (SymbolSC & ~SC_REGISTER) | SC_AUTO;
	    	    	if (curtok == TOK_ASSIGN) {

	    	    	    struct expent lval;

	    	    	    /* Switch to the code segment, allocate space for
	    	    	     * uninitialized variables.
	    	    	     */
	    	    	    g_usecode ();
	    	    	    g_space (AutoSpace);
	    	    	    oursp -= AutoSpace;
	    	    	    AutoSpace = 0;

	    	    	    /* Skip the '=' */
	    	    	    NextToken ();

	    	    	    /* Setup the type flags for the assignment */
	    	    	    flags = Size == 1? CF_FORCECHAR : CF_NONE;

	    	    	    /* Get the expression into the primary */
	    	    	    if (evalexpr (flags, hie1, &lval) == 0) {
	    	    		/* Constant expression. Adjust the types */
	    	    		assignadjust (Decl.Type, &lval);
	    	    		flags |= CF_CONST;
	    	    	    } else {
	    	    		/* Expression is not constant and in the primary */
	    	    		assignadjust (Decl.Type, &lval);
	    	    	    }

	    	    	    /* Push the value */
	    	    	    g_push (flags | TypeOf (Decl.Type), lval.e_const);

	    	    	    /* Mark the variable as referenced */
	    	    	    SymbolSC |= SC_REF;

	    	    	} else {
	    	    	    /* Non-initialized local variable. Just keep track of
	    	    	     * the space needed.
	    	    	     */
	    	    	    AutoSpace += Size;
	    	    	}

	    	    	/* Allocate space on the stack, assign the offset */
	    	    	offs -= Size;
	    	    	ldata = offs;

	    	    } else {

	    	    	/* Static local variables. */
       	       	       	SymbolSC = (SymbolSC & ~(SC_REGISTER | SC_AUTO)) | SC_STATIC;

	    	    	/* Put them into the BSS */
	      	    	g_usebss ();

	      	       	/* Define the variable label */
     	      	        g_defloclabel (ldata = GetLabel ());

	      	       	/* Reserve space for the data */
     	      	       	g_res (Size);

	    	    	/* Allow assignments */
	    	    	if (curtok == TOK_ASSIGN) {

	    	    	    struct expent lval;

	    	    	    /* Switch to the code segment. */
	    	    	    g_usecode ();

	    	    	    /* Skip the '=' */
	    	    	    NextToken ();

	    	    	    /* Get the expression into the primary */
	    	    	    expression1 (&lval);

	    	    	    /* Make type adjustments if needed */
	    	    	    assignadjust (Decl.Type, &lval);

	    	    	    /* Setup the type flags for the assignment */
	    	    	    flags = TypeOf (Decl.Type);
	    	    	    if (Size == 1) {
	    	    		flags |= CF_FORCECHAR;
	    	    	    }

	    	    	    /* Store the value into the variable */
	    	     	    g_putstatic (flags, ldata, 0);

	    	     	    /* Mark the variable as referenced */
	    	     	    SymbolSC |= SC_REF;
	    	     	}
	    	    }

     	       	} else if ((SymbolSC & SC_STATIC) == SC_STATIC) {

	      	    /* Static data */
     	      	    if (curtok == TOK_ASSIGN) {

    	      	    	/* Initialization ahead, switch to data segment */
      	      	    	g_usedata ();

	      	    	/* Define the variable label */
     	      	        g_defloclabel (ldata = GetLabel ());

	      	      	/* Skip the '=' */
     	      	       	NextToken ();

     	      	       	/* Allow initialization of static vars */
     	      	       	ParseInit (Decl.Type);

	      		/* Mark the variable as referenced */
    	      	       	SymbolSC |= SC_REF;

     	      	    } else {

	      	  	/* Uninitialized data, use BSS segment */
	      	  	g_usebss ();

	      	       	/* Define the variable label */
     	      	        g_defloclabel (ldata = GetLabel ());

	      	       	/* Reserve space for the data */
     	      	       	g_res (Size);

     	      	    }
     	      	}

	    }

	    /* If the symbol is not marked as external, it will be defined */
	    if ((SymbolSC & SC_EXTERN) == 0) {
		SymbolSC |= SC_DEF;
	    }

	    /* Add the symbol to the symbol table */
 	    AddLocalSym (Decl.Ident, Decl.Type, SymbolSC, ldata);

     	    if (curtok != TOK_COMMA) {
     	      	break;
	    }
     	    NextToken ();
       	}
     	if (curtok == TOK_SEMI) {
     	    NextToken ();
     	}
    }

    /* In case we switched away from code segment, switch back now */
    g_usecode ();

    /* Create space for locals */
    g_space (AutoSpace);
    oursp -= AutoSpace;
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
    if (!HasVoidReturn (CurrentFunc) && HaveResult) {
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
	Bytes = SizeOf (Sym->Type);
	J = I+1;

       	while (J < RegSymCount) {

	    /* Get the next symbol */
	    const SymEntry* NextSym = RegSyms [J];

	    /* Get the size */
	    int Size = SizeOf (NextSym->Type);

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
    if (!HasVoidReturn (CurrentFunc) && HaveResult) {
     	g_restore (CF_CHAR | CF_FORCECHAR);
    }
}



