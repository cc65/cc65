/*****************************************************************************/
/*                                                                           */
/*				    macro.c				     */
/*                                                                           */
/*		      Macros for the ca65 macroassembler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2004 Ullrich von Bassewitz                                       */
/*               Römerstraße 52                                              */
/*               D-70794 Filderstadt                                         */
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



#include <stdio.h>
#include <string.h>

/* common */
#include "check.h"
#include "hashstr.h"
#include "hashtab.h"
#include "xmalloc.h"

/* ca65 */
#include "condasm.h"
#include "error.h"
#include "global.h"
#include "instr.h"
#include "istack.h"
#include "nexttok.h"
#include "pseudo.h"
#include "toklist.h"
#include "macro.h"



/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



static unsigned HT_GenHash (const void* Key);
/* Generate the hash over a key. */

static const void* HT_GetKey (void* Entry);
/* Given a pointer to the user entry data, return a pointer to the key */

static HashNode* HT_GetHashNode (void* Entry);
/* Given a pointer to the user entry data, return a pointer to the hash node */

static int HT_Compare (const void* Key1, const void* Key2);
/* Compare two keys. The function must return a value less than zero if
 * Key1 is smaller than Key2, zero if both are equal, and a value greater
 * than zero if Key1 is greater then Key2.
 */



/*****************************************************************************/
/*     	       	    		     Data				     */
/*****************************************************************************/



/* Struct that describes an identifer (macro param, local list) */
typedef struct IdDesc IdDesc;
struct IdDesc {
    IdDesc*  	    Next;     	/* Linked list */
    char       	    Id [1];	/* Identifier, dynamically allocated */
};



/* Struct that describes a macro definition */
typedef struct Macro Macro;
struct Macro {
    HashNode        Node;       /* Hash list node */
    Macro*   	    List;	/* List of all macros */
    unsigned 	    LocalCount;	/* Count of local symbols */
    IdDesc*  	    Locals;	/* List of local symbols */
    unsigned 	    ParamCount;	/* Parameter count of macro */
    IdDesc*    	    Params;	/* Identifiers of macro parameters */
    unsigned 	    TokCount;	/* Number of tokens for this macro */
    TokNode* 	    TokRoot;	/* Root of token list */
    TokNode* 	    TokLast;	/* Pointer to last token in list */
    unsigned char   Style;	/* Macro style */
    char       	    Name [1];	/* Macro name, dynamically allocated */
};

/* Hash table functions */
static const HashFunctions HashFunc = {
    HT_GenHash,
    HT_GetKey,
    HT_GetHashNode,
    HT_Compare
};

/* Macro hash table */
static HashTable MacroTab = STATIC_HASHTABLE_INITIALIZER (117, &HashFunc);

/* Global macro data */
static Macro*  	MacroRoot = 0;	/* List of all macros */

/* Structs that holds data for a macro expansion */
typedef struct MacExp MacExp;
struct MacExp {
    MacExp*  	Next;		/* Pointer to next expansion */
    Macro*   	M;  	  	/* Which macro do we expand? */
    unsigned 	IfSP;		/* .IF stack pointer at start of expansion */
    TokNode*   	Exp;		/* Pointer to current token */
    TokNode* 	Final;		/* Pointer to final token */
    unsigned    LocalStart;	/* Start of counter for local symbol names */
    unsigned 	ParamCount;	/* Number of actual parameters */
    TokNode**	Params;	  	/* List of actual parameters */
    TokNode* 	ParamExp;	/* Node for expanding parameters */
};

/* Number of active macro expansions */
static unsigned MacExpansions = 0;

/* Flag if a macro expansion should get aborted */
static int DoMacAbort = 0;

/* Counter to create local names for symbols */
static unsigned LocalName = 0;



/*****************************************************************************/
/*                           Hash table functions                            */
/*****************************************************************************/



static unsigned HT_GenHash (const void* Key)
/* Generate the hash over a key. */
{
    return HashStr (Key);
}



static const void* HT_GetKey (void* Entry)
/* Given a pointer to the user entry data, return a pointer to the index */
{
    return ((Macro*) Entry)->Name;
}



static HashNode* HT_GetHashNode (void* Entry)
/* Given a pointer to the user entry data, return a pointer to the hash node */
{
    return &((Macro*) Entry)->Node;
}



static int HT_Compare (const void* Key1, const void* Key2)
/* Compare two keys. The function must return a value less than zero if
 * Key1 is smaller than Key2, zero if both are equal, and a value greater
 * than zero if Key1 is greater then Key2.
 */
{
    return strcmp (Key1, Key2);
}



/*****************************************************************************/
/*     	       	    	    	     Code     				     */
/*****************************************************************************/



static IdDesc* NewIdDesc (const char* Id)
/* Create a new IdDesc, initialize and return it */
{
    /* Allocate memory */
    unsigned Len = strlen (Id);
    IdDesc* I = xmalloc (sizeof (IdDesc) + Len);

    /* Initialize the struct */
    I->Next = 0;
    memcpy (I->Id, Id, Len);
    I->Id [Len] = '\0';

    /* Return the new struct */
    return I;
}



static Macro* NewMacro (const char* Name, unsigned char Style)
/* Generate a new macro entry, initialize and return it */
{
    /* Allocate memory */
    unsigned Len = strlen (Name);
    Macro* M = xmalloc (sizeof (Macro) + Len);

    /* Initialize the macro struct */
    InitHashNode (&M->Node, M);
    M->LocalCount = 0;
    M->Locals     = 0;
    M->ParamCount = 0;
    M->Params     = 0;
    M->TokCount	  = 0;
    M->TokRoot    = 0;
    M->TokLast    = 0;
    M->Style	  = Style;
    memcpy (M->Name, Name, Len+1);

    /* Insert the macro into the global macro list */
    M->List = MacroRoot;
    MacroRoot = M;

    /* Insert the macro into the hash table */
    HT_Insert (&MacroTab, &M->Node);

    /* Return the new macro struct */
    return M;
}



static MacExp* NewMacExp (Macro* M)
/* Create a new expansion structure for the given macro */
{
    unsigned I;

    /* Allocate memory */
    MacExp* E = xmalloc (sizeof (MacExp));

    /* Initialize the data */
    E->M       	  = M;
    E->IfSP	  = GetIfStack ();
    E->Exp     	  = M->TokRoot;
    E->Final	  = 0;
    E->LocalStart = LocalName;
    LocalName    += M->LocalCount;
    E->ParamCount = 0;
    E->Params     = xmalloc (M->ParamCount * sizeof (TokNode*));
    E->ParamExp	  = 0;
    for (I = 0; I < M->ParamCount; ++I) {
	E->Params [I] = 0;
    }

    /* One macro expansion more */
    ++MacExpansions;

    /* Return the new macro expansion */
    return E;
}



static void FreeMacExp (MacExp* E)
/* Remove and free the current macro expansion */
{
    unsigned I;

    /* One macro expansion less */
    --MacExpansions;

    /* Free the parameter list */
    for (I = 0; I < E->ParamCount; ++I) {
      	xfree (E->Params [I]);
    }
    xfree (E->Params);

    /* Free the final token if we have one */
    if (E->Final) {
      	FreeTokNode (E->Final);
    }

    /* Free the structure itself */
    xfree (E);
}



static void MacSkipDef (unsigned Style)
/* Skip a macro definition */
{
    if (Style == MAC_STYLE_CLASSIC) {
	/* Skip tokens until we reach the final .endmacro */
	while (Tok != TOK_ENDMACRO && Tok != TOK_EOF) {
	    NextTok ();
	}
	if (Tok != TOK_EOF) {
	    SkipUntilSep ();
	} else {
	    Error ("`.ENDMACRO' expected");
	}
    } else {
	/* Skip until end of line */
     	SkipUntilSep ();
    }
}



void MacDef (unsigned Style)
/* Parse a macro definition */
{
    Macro* M;
    TokNode* T;
    int HaveParams;

    /* We expect a macro name here */
    if (Tok != TOK_IDENT) {
	Error ("Identifier expected");
	MacSkipDef (Style);
    	return;
    } else if (!UbiquitousIdents && FindInstruction (SVal) >= 0) {
        /* The identifier is a name of a 6502 instruction, which is not
         * allowed if not explicitly enabled.
         */
        Error ("Cannot use an instruction as macro name");
        MacSkipDef (Style);
        return;
    }

    /* Did we already define that macro? */
    if (HT_Find (&MacroTab, SVal) != 0) {
       	/* Macro is already defined */
     	Error ("A macro named `%s' is already defined", SVal);
     	/* Skip tokens until we reach the final .endmacro */
     	MacSkipDef (Style);
       	return;
    }

    /* Define the macro */
    M = NewMacro (SVal, Style);

    /* Switch to raw token mode and skip the macro name */
    EnterRawTokenMode ();
    NextTok ();

    /* If we have a DEFINE style macro, we may have parameters in braces,
     * otherwise we may have parameters without braces.
     */
    if (Style == MAC_STYLE_CLASSIC) {
	HaveParams = 1;
    } else {
	if (Tok == TOK_LPAREN) {
	    HaveParams = 1;
	    NextTok ();
	} else {
	    HaveParams = 0;
	}
    }

    /* Parse the parameter list */
    if (HaveParams) {

    	while (Tok == TOK_IDENT) {

	    /* Create a struct holding the identifier */
	    IdDesc* I = NewIdDesc (SVal);

	    /* Insert the struct into the list, checking for duplicate idents */
	    if (M->ParamCount == 0) {
		M->Params = I;
	    } else {
		IdDesc* List = M->Params;
		while (1) {
		    if (strcmp (List->Id, SVal) == 0) {
			Error ("Duplicate symbol `%s'", SVal);
		    }
		    if (List->Next == 0) {
			break;
		    } else {
			List = List->Next;
		    }
		}
		List->Next = I;
	    }
	    ++M->ParamCount;

       	    /* Skip the name */
	    NextTok ();

	    /* Maybe there are more params... */
	    if (Tok == TOK_COMMA) {
		NextTok ();
	    } else {
		break;
	    }
	}
    }

    /* For class macros, we expect a separator token, for define style macros,
     * we expect the closing paren.
     */
    if (Style == MAC_STYLE_CLASSIC) {
	ConsumeSep ();
    } else if (HaveParams) {
	ConsumeRParen ();
    }

    /* Preparse the macro body. We will read the tokens until we reach end of
     * file, or a .endmacro (or end of line for DEFINE style macros) and store
     * them into an token list internal to the macro. For classic macros, there
     * the .LOCAL command is detected and removed at this time.
     */
    while (1) {

	/* Check for end of macro */
	if (Style == MAC_STYLE_CLASSIC) {
	    /* In classic macros, only .endmacro is allowed */
	    if (Tok == TOK_ENDMACRO) {
		/* Done */
		break;
	    }
	    /* May not have end of file in a macro definition */
	    if (Tok == TOK_EOF) {
		Error ("`.ENDMACRO' expected");
		goto Done;
	    }
	} else {
	    /* Accept a newline or end of file for new style macros */
	    if (TokIsSep (Tok)) {
		break;
	    }
	}

     	/* Check for a .LOCAL declaration */
     	if (Tok == TOK_LOCAL && Style == MAC_STYLE_CLASSIC) {

     	    while (1) {

     	    	IdDesc* I;

     	  	/* Skip .local or comma */
       		NextTok ();

     		/* Need an identifer */
     		if (Tok != TOK_IDENT) {
     	       	    Error ("Identifier expected");
     		    SkipUntilSep ();
     		    break;
     		}

     		/* Put the identifier into the locals list and skip it */
       	       	I = NewIdDesc (SVal);
     		I->Next = M->Locals;
     		M->Locals = I;
     		++M->LocalCount;
     		NextTok ();

     	      	/* Check for end of list */
     		if (Tok != TOK_COMMA) {
     		    break;
     		}

     	    }

     	    /* We need end of line after the locals */
     	    ConsumeSep ();
     	    continue;
     	}

     	/* Create a token node for the current token */
     	T = NewTokNode ();

     	/* If the token is an ident, check if it is a local parameter */
     	if (Tok == TOK_IDENT) {
     	    unsigned Count = 0;
     	    IdDesc* I = M->Params;
     	    while (I) {
     	       	if (strcmp (I->Id, SVal) == 0) {
     	       	    /* Local param name, replace it */
     	       	    T->Tok  = TOK_MACPARAM;
     	       	    T->IVal = Count;
     	       	    break;
     	       	}
     	       	++Count;
       	       	I = I->Next;
     	    }
     	}

     	/* Insert the new token in the list */
     	if (M->TokCount == 0) {
     	    /* First token */
     	    M->TokRoot = M->TokLast = T;
     	} else {
     	    /* We have already tokens */
     	    M->TokLast->Next = T;
     	    M->TokLast = T;
     	}
     	++M->TokCount;

     	/* Read the next token */
     	NextTok ();
    }

    /* Skip the .endmacro for a classic macro */
    if (Style == MAC_STYLE_CLASSIC) {
	NextTok ();
    }

Done:
    /* Switch out of raw token mode */
    LeaveRawTokenMode ();
}



static int MacExpand (void* Data)
/* If we're currently expanding a macro, set the the scanner token and
 * attribute to the next value and return true. If we are not expanding
 * a macro, return false.
 */
{
    /* Cast the Data pointer to the actual data structure */
    MacExp* Mac = (MacExp*) Data;

    /* Check if we should abort this macro */
    if (DoMacAbort) {

	/* Reset the flag */
	DoMacAbort = 0;

	/* Abort any open .IF statements in this macro expansion */
	CleanupIfStack (Mac->IfSP);

	/* Terminate macro expansion */
	goto MacEnd;
    }

    /* We're expanding a macro. Check if we are expanding one of the
     * macro parameters.
     */
    if (Mac->ParamExp) {

       	/* Ok, use token from parameter list */
       	TokSet (Mac->ParamExp);

       	/* Set pointer to next token */
       	Mac->ParamExp = Mac->ParamExp->Next;

       	/* Done */
       	return 1;

    }

    /* We're not expanding macro parameters. Check if we have tokens left from
     * the macro itself.
     */
    if (Mac->Exp) {

       	/* Use next macro token */
       	TokSet (Mac->Exp);

       	/* Set pointer to next token */
       	Mac->Exp = Mac->Exp->Next;

       	/* Is it a request for actual parameter count? */
       	if (Tok == TOK_PARAMCOUNT) {
       	    Tok  = TOK_INTCON;
       	    IVal = Mac->ParamCount;
       	    return 1;
       	}

       	/* Is it the name of a macro parameter? */
       	if (Tok == TOK_MACPARAM) {

       	    /* Start to expand the parameter token list */
       	    Mac->ParamExp = Mac->Params [IVal];

       	    /* Recursive call to expand the parameter */
       	    return MacExpand (Mac);
       	}

       	/* If it's an identifier, it may in fact be a local symbol */
       	if (Tok == TOK_IDENT && Mac->M->LocalCount) {
       	    /* Search for the local symbol in the list */
       	    unsigned Index = 0;
       	    IdDesc* I = Mac->M->Locals;
       	    while (I) {
       	       	if (strcmp (SVal, I->Id) == 0) {
       	       	    /* This is in fact a local symbol, change the name. Be sure
                     * to generate a local label name if the original name was
                     * a local label, and also generate a name that cannot be
                     * generated by a user.
                     */
                    unsigned PrefixLen = (I->Id[0] == LocalStart);
       	       	    sprintf (SVal, "%.*sLOCAL-MACRO-SYMBOL-%04X", PrefixLen,
                             I->Id, Mac->LocalStart + Index);
       	       	    break;
       	       	}
       	       	/* Next symbol */
       	       	++Index;
       	       	I = I->Next;
       	    }

       	    /* Done */
       	    return 1;
       	}

       	/* The token was successfully set */
       	return 1;

    }

    /* No more macro tokens. Do we have a final token? */
    if (Mac->Final) {

      	/* Set the final token and remove it */
      	TokSet (Mac->Final);
      	FreeTokNode (Mac->Final);
      	Mac->Final = 0;

       	/* The token was successfully set */
       	return 1;

    }

MacEnd:
    /* End of macro expansion */
    FreeMacExp (Mac);

    /* Pop the input function */
    PopInput ();

    /* No token available */
    return 0;
}



static void StartExpClassic (Macro* M)
/* Start expanding the classic macro M */
{
    MacExp* E;

    /* Skip the macro name */
    NextTok ();

    /* Create a structure holding expansion data */
    E = NewMacExp (M);

    /* Read the actual parameters */
    while (!TokIsSep (Tok)) {

	TokNode* Last;

       	/* Check for maximum parameter count */
	if (E->ParamCount >= M->ParamCount) {
	    Error ("Too many macro parameters");
	    SkipUntilSep ();
	    break;
	}

	/* Read tokens for one parameter, accept empty params */
	Last = 0;
	while (Tok != TOK_COMMA && Tok != TOK_SEP) {

	    TokNode* T;

	    /* Check for end of file */
	    if (Tok == TOK_EOF) {
	    	Error ("Unexpected end of file");
	    	return;
	    }

	    /* Get the next token in a node */
	    T = NewTokNode ();

	    /* Insert it into the list */
	    if (Last == 0) {
	      	E->Params [E->ParamCount] = T;
	    } else {
	    	Last->Next = T;
	    }
	    Last = T;

	    /* And skip it... */
	    NextTok ();
	}

	/* One parameter more */
	++E->ParamCount;

	/* Check for a comma */
	if (Tok == TOK_COMMA) {
	    NextTok ();
	} else {
	    break;
	}
    }

    /* Insert a new token input function */
    PushInput (MacExpand, E, ".MACRO");
}



static void StartExpDefine (Macro* M)
/* Start expanding a DEFINE style macro */
{
    /* Create a structure holding expansion data */
    MacExp* E = NewMacExp (M);

    /* A define style macro must be called with as many actual parameters
     * as there are formal ones. Get the parameter count.
     */
    unsigned Count = M->ParamCount;

    /* Skip the current token */
    NextTok ();

    /* Read the actual parameters */
    while (Count--) {

       	TokNode* Last;

       	/* Check if there is really a parameter */
       	if (TokIsSep (Tok) || Tok == TOK_COMMA) {
       	    Error ("Macro parameter expected");
       	    SkipUntilSep ();
       	    return;
       	}

       	/* Read tokens for one parameter */
       	Last = 0;
       	do {

       	    TokNode* T;

       	    /* Get the next token in a node */
       	    T = NewTokNode ();

       	    /* Insert it into the list */
       	    if (Last == 0) {
       	    	E->Params [E->ParamCount] = T;
       	    } else {
       	    	Last->Next = T;
       	    }
       	    Last = T;

	    /* And skip it... */
	    NextTok ();

	} while (Tok != TOK_COMMA && !TokIsSep (Tok));

	/* One parameter more */
	++E->ParamCount;

       	/* Check for a comma */
       	if (Count > 0) {
       	    if (Tok == TOK_COMMA) {
       	        NextTok ();
       	    } else {
       		Error ("`,' expected");
       	    }
       	}
    }

    /* Macro expansion will overwrite the current token. This is a problem
     * for define style macros since these are called from the scanner level.
     * To avoid it, remember the current token and re-insert it if macro
     * expansion is done.
     */
    E->Final = NewTokNode ();

    /* Insert a new token input function */
    PushInput (MacExpand, E, ".DEFINE");
}



void MacExpandStart (void)
/* Start expanding the macro in SVal */
{
    /* Search for the macro */
    Macro* M = HT_FindEntry (&MacroTab, SVal);
    CHECK (M != 0);

    /* Call the apropriate subroutine */
    switch (M->Style) {
    	case MAC_STYLE_CLASSIC:	StartExpClassic (M);	break;
	case MAC_STYLE_DEFINE:	StartExpDefine (M);	break;
	default:   	      	Internal ("Invalid macro style: %d", M->Style);
    }
}



void MacAbort (void)
/* Abort the current macro expansion */
{
    /* Must have an expansion */
    CHECK (MacExpansions > 0);

    /* Set a flag so macro expansion will terminate on the next call */
    DoMacAbort = 1;
}



int IsMacro (const char* Name)
/* Return true if the given name is the name of a macro */
{
    return (HT_Find (&MacroTab, Name) != 0);
}



int IsDefine (const char* Name)
/* Return true if the given name is the name of a define style macro */
{
    Macro* M = HT_FindEntry (&MacroTab, Name);
    return (M != 0 && M->Style == MAC_STYLE_DEFINE);
}



int InMacExpansion (void)
/* Return true if we're currently expanding a macro */
{
    return (MacExpansions > 0);
}






