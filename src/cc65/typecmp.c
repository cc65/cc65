/*****************************************************************************/
/*                                                                           */
/*				   typecmp.c				     */
/*                                                                           */
/*		 Type compare function for the cc65 C compiler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2000 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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



#include <string.h>

/* cc65 */
#include "funcdesc.h"
#include "symtab.h"
#include "typecmp.h"



/*****************************************************************************/
/*				     Code				     */
/*****************************************************************************/



static void SetResult (typecmp_t* Result, typecmp_t Val)
/* Set a new result value if it is less than the existing one */
{
    if (Val < *Result) {
	*Result = Val;
    }
}



static int EqualFuncParams (SymTable* Tab1, SymTable* Tab2)
/* Compare two function symbol tables regarding function parameters. Return 1
 * if they are equal and 0 otherwise.
 */
{
    /* Compare the parameter lists */
    SymEntry* Sym1 = Tab1->SymHead;
    SymEntry* Sym2 = Tab2->SymHead;

    /* Compare the fields */
    while (Sym1 && (Sym1->Flags & SC_PARAM) && Sym2 && (Sym2->Flags & SC_PARAM)) {

	/* Compare this field */
       	if (TypeCmp (Sym1->Type, Sym2->Type) < TC_EQUAL) {
	    /* Field types not equal */
	    return 0;
	}

	/* Get the pointers to the next fields */
	Sym1 = Sym1->NextSym;
	Sym2 = Sym2->NextSym;
    }

    /* Check both pointers against NULL or a non parameter to compare the
     * field count
     */
    return (Sym1 == 0 || (Sym1->Flags & SC_PARAM) == 0) &&
	   (Sym2 == 0 || (Sym2->Flags & SC_PARAM) == 0);
}



static int EqualSymTables (SymTable* Tab1, SymTable* Tab2)
/* Compare two symbol tables. Return 1 if they are equal and 0 otherwise */
{
    /* Compare the parameter lists */
    SymEntry* Sym1 = Tab1->SymHead;
    SymEntry* Sym2 = Tab2->SymHead;

    /* Compare the fields */
    while (Sym1 && Sym2) {

	/* Compare the names of this field */
        if (!HasAnonName (Sym1) || !HasAnonName (Sym2)) {
            if (strcmp (Sym1->Name, Sym2->Name) != 0) {
                /* Names are not identical */
                return 0;
            }
        }
                                
        /* Compare the types of this field */
       	if (TypeCmp (Sym1->Type, Sym2->Type) < TC_EQUAL) {
	    /* Field types not equal */
	    return 0;
	}

	/* Get the pointers to the next fields */
	Sym1 = Sym1->NextSym;
	Sym2 = Sym2->NextSym;
    }

    /* Check both pointers against NULL to compare the field count */
    return (Sym1 == 0 && Sym2 == 0);
}



static void DoCompare (const type* lhs, const type* rhs, typecmp_t* Result)
/* Recursively compare two types. */
{
    unsigned	Indirections;
    unsigned	ElementCount;
    SymEntry* 	Sym1;
    SymEntry* 	Sym2;
    SymTable* 	Tab1;
    SymTable* 	Tab2;
    FuncDesc* 	F1;
    FuncDesc* 	F2;
    int	      	Ok;


    /* Initialize stuff */
    Indirections = 0;
    ElementCount = 0;

    /* Compare two types. Determine, where they differ */
    while (*lhs != T_END) {

       	type LeftType, RightType;
	type LeftSign, RightSign;
	type LeftQual, RightQual;
	unsigned LeftCount, RightCount;

       	/* Check if the end of the type string is reached */
       	if (*rhs == T_END) {
       	    /* End of comparison reached */
       	    return;
       	}

	/* Get the raw left and right types, signs and qualifiers */
	LeftType  = GetType (lhs);
	RightType = GetType (rhs);
	LeftSign  = GetSignedness (lhs);
	RightSign = GetSignedness (rhs);
	LeftQual  = GetQualifier (lhs);
	RightQual = GetQualifier (rhs);

	/* If the left type is a pointer and the right is an array, both
	 * are compatible.
	 */
	if (LeftType == T_TYPE_PTR && RightType == T_TYPE_ARRAY) {
	    RightType = T_TYPE_PTR;
	    rhs += DECODE_SIZE;
	}

	/* If the raw types are not identical, the types are incompatible */
	if (LeftType != RightType) {
	    SetResult (Result, TC_INCOMPATIBLE);
	    return;
	}

	/* On indirection level zero, a qualifier or sign difference is
	 * accepted. The types are no longer equal, but compatible.
	 */
	if (LeftSign != RightSign) {
       	    if (ElementCount == 0) {
       		SetResult (Result, TC_SIGN_DIFF);
       	    } else {
       		SetResult (Result, TC_INCOMPATIBLE);
       		return;
       	    }
       	}
       	if (LeftQual != RightQual) {
	    /* On the first indirection level, different qualifiers mean
	     * that the types are still compatible. On the second level,
	     * this is a (maybe minor) error, so we create a special
	     * return code, since a qualifier is dropped from a pointer.
	     * Starting from the next level, the types are incompatible
	     * if the qualifiers differ.
	     */
	    switch (Indirections) {

		case 0:
		    SetResult (Result, TC_STRICT_COMPATIBLE);
		    break;

		case 1:
		    /* A non const value on the right is compatible to a
		     * const one to the left, same for volatile.
		     */
		    if ((LeftQual & T_QUAL_CONST) < (RightQual & T_QUAL_CONST) ||
		       	(LeftQual & T_QUAL_VOLATILE) < (RightQual & T_QUAL_VOLATILE)) {
		       	SetResult (Result, TC_QUAL_DIFF);
		    } else {
			SetResult (Result, TC_STRICT_COMPATIBLE);
		    }
		    break;

		default:
		    SetResult (Result, TC_INCOMPATIBLE);
		    return;
	    }
       	}

       	/* Check for special type elements */
       	switch (LeftType) {

	    case T_TYPE_PTR:
		++Indirections;
		break;

       	    case T_TYPE_FUNC:
       	       	/* Compare the function descriptors */
       		F1 = DecodePtr (lhs+1);
       		F2 = DecodePtr (rhs+1);

       		/* If one of the functions is implicitly declared, both
       		 * functions are considered equal. If one of the functions is
       		 * old style, and the other is empty, the functions are
       		 * considered equal.
       		 */
       	       	if ((F1->Flags & FD_IMPLICIT) != 0 || (F2->Flags & FD_IMPLICIT) != 0) {
       		    Ok = 1;
       		} else if ((F1->Flags & FD_OLDSTYLE) != 0 && (F2->Flags & FD_EMPTY) != 0) {
       		    Ok = 1;
       		} else if ((F1->Flags & FD_EMPTY) != 0 && (F2->Flags & FD_OLDSTYLE) != 0) {
       		    Ok = 1;
       		} else {
       		    Ok = 0;
       		}

       		if (!Ok) {

       		    /* Check the remaining flags */
       		    if ((F1->Flags & ~FD_IGNORE) != (F2->Flags & ~FD_IGNORE)) {
       		    	/* Flags differ */
			SetResult (Result, TC_INCOMPATIBLE);
       		    	return;
       		    }

       		    /* Compare the parameter lists */
       		    if (EqualFuncParams (F1->SymTab, F2->SymTab) == 0 ||
       		    	EqualSymTables (F1->TagTab, F2->TagTab) == 0) {
       		    	/* One of the tables is not identical */
			SetResult (Result, TC_INCOMPATIBLE);
       		    	return;
       		    }
		}

		/* Skip the FuncDesc pointers to compare the return type */
    	       	lhs += DECODE_SIZE;
    	       	rhs += DECODE_SIZE;
    	       	break;

    	    case T_TYPE_ARRAY:
    	       	/* Check member count */
    	       	LeftCount  = Decode (lhs+1);
    	       	RightCount = Decode (rhs+1);
    	       	if (LeftCount != 0 && RightCount != 0 && LeftCount != RightCount) {
    	       	    /* Member count given but different */
		    SetResult (Result, TC_INCOMPATIBLE);
    	       	    return;
    	       	}
    	       	lhs += DECODE_SIZE;
     	       	rhs += DECODE_SIZE;
    	       	break;

    	    case T_TYPE_STRUCT:
    	    case T_TYPE_UNION:
       	       	/* Compare the fields recursively. To do that, we fetch the
    	       	 * pointer to the struct definition from the type, and compare
    	       	 * the fields.
    	       	 */
    	       	Sym1 = DecodePtr (lhs+1);
    	       	Sym2 = DecodePtr (rhs+1);

                /* If one symbol has a name, the names must be identical */
                if (!HasAnonName (Sym1) || !HasAnonName (Sym2)) {
                    if (strcmp (Sym1->Name, Sym2->Name) != 0) {
                        /* Names are not identical */
                        SetResult (Result, TC_INCOMPATIBLE);
                        return;
                    }
                }

		/* Get the field tables from the struct entry */
		Tab1 = Sym1->V.S.SymTab;
		Tab2 = Sym2->V.S.SymTab;

		/* One or both structs may be forward definitions. In this case,
		 * the symbol tables are both non existant. Assume that the
		 * structs are equal in this case.
		 */
		if (Tab1 != 0 && Tab2 != 0) {

		    if (EqualSymTables (Tab1, Tab2) == 0) {
			/* Field lists are not equal */
			SetResult (Result, TC_INCOMPATIBLE);
			return;
		    }

		}

    		/* Structs are equal */
    		lhs += DECODE_SIZE;
    	       	rhs += DECODE_SIZE;
     	      	break;
    	}

	/* Next type string element */
    	++lhs;
       	++rhs;
	++ElementCount;
    }

    /* Check if end of rhs reached */
    if (*rhs == T_END) {
     	SetResult (Result, TC_EQUAL);
    } else {
	SetResult (Result, TC_INCOMPATIBLE);
    }
}



typecmp_t TypeCmp (const type* lhs, const type* rhs)
/* Compare two types and return the result */
{
    /* Assume the types are identical */
    typecmp_t 	Result = TC_IDENTICAL;

#if 0
    printf ("Left : "); PrintRawType (stdout, lhs);
    printf ("Right: "); PrintRawType (stdout, rhs);
#endif

    /* Recursively compare the types if they aren't identical */
    if (rhs != lhs) {
     	DoCompare (lhs, rhs, &Result);
    }

    /* Return the result */
    return Result;
}



