
/* C pre-processor functions */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

/* common */
#include "chartype.h"
#include "check.h"
#include "inline.h"
#include "print.h"
#include "xmalloc.h"

/* cc65 */
#include "codegen.h"
#include "error.h"
#include "expr.h"
#include "global.h"
#include "ident.h"
#include "incpath.h"
#include "input.h"
#include "lineinfo.h"
#include "macrotab.h"
#include "scanner.h"
#include "util.h"
#include "preproc.h"



/*****************************************************************************/
/*				   Forwards				     */
/*****************************************************************************/



static int Pass1 (const char* From, char* To);
/* Preprocessor pass 1. Remove whitespace and comments. */



/*****************************************************************************/
/*  		    		     Data				     */
/*****************************************************************************/



/* Set when the preprocessor calls expr() recursively */
unsigned char Preprocessing = 0;

/* Management data for #if */
#define MAX_IFS         64
#define IFCOND_NONE     0x00U
#define IFCOND_SKIP     0x01U
#define IFCOND_ELSE     0x02U
#define IFCOND_NEEDTERM 0x04U
static unsigned char IfStack[MAX_IFS];
static int           IfIndex = -1;

/* Buffer for macro expansion */
static char mlinebuf [LINESIZE];
static char* mline = mlinebuf;
static char* mptr;



/*****************************************************************************/
/*		     Low level preprocessor token handling                   */
/*****************************************************************************/



/* Types of preprocessor tokens */
typedef enum {
    PP_DEFINE,
    PP_ELIF,
    PP_ELSE,
    PP_ENDIF,
    PP_ERROR,
    PP_IF,
    PP_IFDEF,
    PP_IFNDEF,
    PP_INCLUDE,
    PP_LINE,
    PP_PRAGMA,
    PP_UNDEF,
    PP_ILLEGAL
} pptoken_t;



/* Preprocessor keyword to token mapping table */
static const struct PPToken {
    const char*	Key;	   	/* Keyword */
    pptoken_t  	Tok;	   	/* Token */
} PPTokens[] = {
    {  	"define",      	PP_DEFINE	},
    {   "elif",         PP_ELIF         },
    {  	"else",	       	PP_ELSE		},
    {  	"endif",       	PP_ENDIF	},
    {  	"error",       	PP_ERROR	},
    {  	"if",  	       	PP_IF		},
    {  	"ifdef",       	PP_IFDEF	},
    {  	"ifndef",      	PP_IFNDEF	},
    {  	"include",     	PP_INCLUDE	},
    {   "line",	       	PP_LINE		},
    {  	"pragma",      	PP_PRAGMA	},
    {  	"undef",       	PP_UNDEF	},
};

/* Number of preprocessor tokens */
#define PPTOKEN_COUNT  	(sizeof(PPTokens) / sizeof(PPTokens[0]))



static int CmpToken (const void* Key, const void* Elem)
/* Compare function for bsearch */
{
    return strcmp ((const char*) Key, ((const struct PPToken*) Elem)->Key);
}



static pptoken_t FindPPToken (const char* Ident)
/* Find a preprocessor token and return ut. Return PP_ILLEGAL if the identifier
 * is not a valid preprocessor token.
 */
{
    struct PPToken* P;
    P = bsearch (Ident, PPTokens, PPTOKEN_COUNT, sizeof (PPTokens[0]), CmpToken);
    return P? P->Tok : PP_ILLEGAL;
}



/*****************************************************************************/
/* 	      	    		     Code		    		     */
/*****************************************************************************/



#ifdef HAVE_INLINE
INLINE void KeepChar (char c)
/* Put character c into translation buffer. */
{
    *mptr++ = c;
}
#else
#define KeepChar(c)     *mptr++ = (c)
#endif



static void KeepStr (const char* S)
/* Put string str into translation buffer. */
{
    unsigned Len = strlen (S);
    memcpy (mptr, S, Len);
    mptr += Len;
}



static void Stringize (const char* S)
/* Stringize the given string: Add double quotes at start and end and preceed
 * each occurance of " and \ by a backslash.
 */
{
    KeepChar ('\"');
    /* Replace any characters inside the string may not be part of a string
     * unescaped.
     */
    while (*S) {
        switch (*S) {
            case '\"':
            case '\\':
                KeepChar ('\\');
            /* FALLTHROUGH */
            default:
                KeepChar (*S);
                break;
        }
        ++S;
    }
    KeepChar ('\"');
}



static void SwapLineBuffers (void)
/* Swap both line buffers */
{
    /* Swap mline and line */
    char* p = line;
    line = mline;
    mline = p;
}



static void OldStyleComment (void)
/* Remove an old style C comment from line. */
{
    /* Remember the current line number, so we can output better error
     * messages if the comment is not terminated in the current file.
     */
    unsigned StartingLine = GetCurrentLine();

    /* Skip the start of comment chars */
    NextChar ();
    NextChar ();

    /* Skip the comment */
    while (CurC != '*' || NextC != '/') {
    	if (CurC == '\0') {
    	    if (NextLine () == 0) {
    	    	PPError ("End-of-file reached in comment starting at line %u",
	    		 StartingLine);
    	    	return;
    	    }
	} else {
	    if (CurC == '/' && NextC == '*') {
	    	PPWarning ("`/*' found inside a comment");
	    }
	    NextChar ();
	}
    }

    /* Skip the end of comment chars */
    NextChar ();
    NextChar ();
}



static void NewStyleComment (void)
/* Remove a new style C comment from line. */
{
    /* Beware: Because line continuation chars are handled when reading
     * lines, we may only skip til the end of the source line, which
     * may not be the same as the end of the input line. The end of the
     * source line is denoted by a lf (\n) character.
     */
    do {
    	NextChar ();
    } while (CurC != '\n' && CurC != '\0');
    if (CurC == '\n') {
    	NextChar ();
    }
}



static void SkipBlank (void)
/* Skip blanks and tabs in the input stream. */
{
    while (IsBlank (CurC)) {
	NextChar ();
    }
}



static char* CopyQuotedString (char* Target)
/* Copy a single or double quoted string from the input to Target. Return the
 * new target pointer. Target will not be terminated after the copy.
 */
{
    /* Remember the quote character, copy it to the target buffer and skip it */
    char Quote = CurC;
    *Target++  = CurC;
    NextChar ();

    /* Copy the characters inside the string */
    while (CurC != '\0' && CurC != Quote) {
       	/* Keep an escaped char */
      	if (CurC == '\\') {
 	    *Target++ = CurC;
	    NextChar ();
 	}
 	/* Copy the character */
 	*Target++ = CurC;
	NextChar ();
    }

    /* If we had a terminating quote, copy it */
    if (CurC != '\0') {
 	*Target++ = CurC;
	NextChar ();
    }

    /* Return the new target pointer */
    return Target;
}



/*****************************************************************************/
/*				  Macro stuff		    		     */
/*****************************************************************************/



static int MacName (char* Ident)
/* Get a macro symbol name into Ident.  If we have an error, print a
 * diagnostic message and clear the line.
 */
{
    if (IsSym (Ident) == 0) {
 	PPError ("Identifier expected");
     	ClearLine ();
 	return 0;
    } else {
    	return 1;
    }
}



static void ExpandMacroArgs (Macro* M)
/* Expand the arguments of a macro */
{
    ident	Ident;
    const char* Replacement;
    const char*	SavePtr;

    /* Save the current line pointer and setup the new ones */
    SavePtr = lptr;
    InitLine (M->Replacement);

    /* Copy the macro replacement checking for parameters to replace */
    while (CurC != '\0') {
 	/* If the next token is an identifier, check for a macro arg */
     	if (IsIdent (CurC)) {
     	    SymName (Ident);
 	    Replacement = FindMacroArg (M, Ident);
 	    if (Replacement) {
 		/* Macro arg, keep the replacement */
     	    	KeepStr (Replacement);
     	    } else {
 		/* No macro argument, keep the original identifier */
     	    	KeepStr (Ident);
     	    }
     	} else if (CurC == '#' && IsIdent (NextC)) {
       	    NextChar ();
     	    SymName (Ident);
 	    Replacement = FindMacroArg (M, Ident);
       	    if (Replacement) {
                /* Make a valid string from Replacement */
                Stringize (Replacement);
     	    } else {
                /* No replacement - keep the input */
     	    	KeepChar ('#');
     	    	KeepStr (Ident);
     	    }
     	} else if (IsQuote (CurC)) {
     	    mptr = CopyQuotedString (mptr);
     	} else {
     	    KeepChar (CurC);
 	    NextChar ();
     	}
    }

    /* Reset the line pointer */
    InitLine (SavePtr);
}



static int MacroCall (Macro* M)
/* Process a function like macro */
{
    int         ArgCount;      	/* Macro argument count */
    unsigned 	ParCount;       /* Number of open parenthesis */
    char     	Buf[LINESIZE];	/* Argument buffer */
    const char* ArgStart;
    char*    	B;

    /* Expect an argument list */
    SkipBlank ();
    if (CurC != '(') {
     	PPError ("Illegal macro call");
     	return 0;
    }

    /* Eat the left paren */
    NextChar ();

    /* Read the actual macro arguments and store pointers to these arguments
     * into the array of actual arguments in the macro definition.
     */
    ArgCount = 0;
    ParCount = 0;
    ArgStart = Buf;
    B 	     = Buf;
    while (1) {
       	if (CurC == '(') {
 	    /* Nested parenthesis */
     	    *B++ = CurC;
 	    NextChar ();
     	    ++ParCount;
     	} else if (IsQuote (CurC)) {
    	    B = CopyQuotedString (B);
     	} else if (CurC == ',' || CurC == ')') {
     	    if (ParCount == 0) {
 	    	/* End of actual argument */
     	       	*B++ = '\0';
 	    	while (IsBlank(*ArgStart)) {
 	    	    ++ArgStart;
 	    	}
    	      	if (ArgCount < M->ArgCount) {
    	      	    M->ActualArgs[ArgCount++] = ArgStart;
       	       	} else if (CurC != ')' || *ArgStart != '\0' || M->ArgCount > 0) {
 	       	    /* Be sure not to count the single empty argument for a
 	    	     * macro that does not have arguments.
 	    	     */
    	      	    ++ArgCount;
 	    	}

 	    	/* Check for end of macro param list */
    	    	if (CurC == ')') {
 	    	    NextChar ();
 	    	    break;
    	    	}

       	       	/* Start the next param */
     	       	ArgStart = B;
 	    	NextChar ();
     	    } else {
    	    	/* Comma or right paren inside nested parenthesis */
     	       	if (CurC == ')') {
     	       	    --ParCount;
     	       	}
     	       	*B++ = CurC;
      	    	NextChar ();
     	    }
     	} else if (IsBlank (CurC)) {
 	    /* Squeeze runs of blanks */
     	    *B++ = ' ';
     	    SkipBlank ();
	} else if (CurC == '/' && NextC == '*') {
	    *B++ = ' ';
     	    OldStyleComment ();
     	} else if (ANSI == 0 && CurC == '/' && NextC == '/') {
     	    *B++ = ' ';
	    NewStyleComment ();
     	} else if (CurC == '\0') {
    	    /* End of line inside macro argument list - read next line */
     	    if (NextLine () == 0) {
     	       	return 0;
     	    }
     	} else {
    	    /* Just copy the character */
     	    *B++ = CurC;
    	    NextChar ();
     	}
    }

    /* Compare formal argument count with actual */
    if (M->ArgCount != ArgCount) {
    	PPError ("Macro argument count mismatch");
    	/* Be sure to make enough empty arguments available */
    	while (ArgCount < M->ArgCount) {
    	    M->ActualArgs [ArgCount++] = "";
    	}
    }

    /* Preprocess the line, replacing macro parameters */
    ExpandMacroArgs (M);

    /* Done */
    return 1;
}



static void ExpandMacro (Macro* M)
/* Expand a macro */
{
    /* Check if this is a function like macro */
    if (M->ArgCount >= 0) {
 	/* Function like macro */
       	if (MacroCall (M) == 0) {
     	    ClearLine ();
     	}
    } else {
 	/* Just copy the replacement text */
     	KeepStr (M->Replacement);
    }
}



static void DefineMacro (void)
/* Handle a macro definition. */
{
    char*   	saveptr;
    ident   	Ident;
    char    	Buf[LINESIZE];
    Macro*  	M;
    Macro*	Existing;

    /* Read the macro name */
    SkipBlank ();
    if (!MacName (Ident)) {
    	return;
    }

    /* Get an existing macro definition with this name */
    Existing = FindMacro (Ident);

    /* Create a new macro definition */
    M = NewMacro (Ident);

    /* Check if this is a function like macro */
    if (CurC == '(') {

    	/* Skip the left paren */
    	NextChar ();

       	/* Set the marker that this is a function like macro */
 	M->ArgCount = 0;

 	/* Read the formal parameter list */
     	while (1) {
     	    SkipBlank ();
     	    if (CurC == ')')
     	    	break;
     	    if (MacName (Ident) == 0) {
     	    	return;
     	    }
 	    AddMacroArg (M, Ident);
     	    SkipBlank ();
     	    if (CurC != ',')
     	    	break;
     	    NextChar ();
     	}

 	/* Check for a right paren and eat it if we find one */
     	if (CurC != ')') {
       	    PPError ("`)' expected");
     	    ClearLine ();
     	    return;
     	}
     	NextChar ();
    }

    /* Insert the macro into the macro table and allocate the ActualArgs array */
    InsertMacro (M);

    /* Remove whitespace and comments from the line, store the preprocessed
     * line into Buf.
     */
    SkipBlank ();
    saveptr = mptr;
    Pass1 (lptr, Buf);
    mptr = saveptr;

    /* Create a copy of the replacement */
    M->Replacement = xstrdup (Buf);

    /* If we have an existing macro, check if the redefinition is identical.
     * Print a diagnostic if not.
     */
    if (Existing) {
 	if (MacroCmp (M, Existing) != 0) {
 	    PPError ("Macro redefinition is not identical");
 	}
    }
}



/*****************************************************************************/
/*				 Preprocessing                               */
/*****************************************************************************/



static int Pass1 (const char* From, char* To)
/* Preprocessor pass 1. Remove whitespace and comments. */
{
    int     	done;
    ident   	Ident;
    int     	HaveParen;

    /* Initialize reading from "From" */
    InitLine (From);

    /* Target is "To" */
    mptr = To;

    /* Loop removing ws and comments */
    done = 1;
    while (CurC != '\0') {
     	if (IsBlank (CurC)) {
     	    KeepChar (' ');
     	    SkipBlank ();
       	} else if (IsIdent (CurC)) {
     	    SymName (Ident);
     	    if (Preprocessing && strcmp(Ident, "defined") == 0) {
     	    	/* Handle the "defined" operator */
     	    	SkipBlank();
     	    	HaveParen = 0;
     	    	if (CurC == '(') {
     	    	    HaveParen = 1;
     	    	    NextChar ();
     	    	    SkipBlank();
     	    	}
     	    	if (!IsIdent (CurC)) {
     	    	    PPError ("Identifier expected");
     	    	    KeepChar ('0');
     	    	} else {
     	    	    SymName (Ident);
     	    	    KeepChar (IsMacro (Ident)? '1' : '0');
     	    	    if (HaveParen) {
     	    	       	SkipBlank();
     	    	       	if (CurC != ')') {
     	    	       	    PPError ("`)' expected");
    	    	       	} else {
	    	       	    NextChar ();
	    	       	}
	    	    }
    	    	}
	    } else {
	    	if (MaybeMacro (Ident[0])) {
	    	    done = 0;
	    	}
	    	KeepStr (Ident);
	    }
	} else if (IsQuote (CurC)) {
	    mptr = CopyQuotedString (mptr);
	} else if (CurC == '/' && NextC == '*') {
	    KeepChar (' ');
     	    OldStyleComment ();
     	} else if (ANSI == 0 && CurC == '/' && NextC == '/') {
     	    KeepChar (' ');
	    NewStyleComment ();
     	} else {
     	    KeepChar (CurC);
	    NextChar ();
     	}
    }
    KeepChar ('\0');
    return done;
}



static int Pass2 (const char* From, char* To)
/* Preprocessor pass 2.  Perform macro substitution. */
{
    int     	no_chg;
    ident   	Ident;
    Macro*  	M;

    /* Initialize reading from "From" */
    InitLine (From);

    /* Target is "To" */
    mptr = To;

    /* Loop substituting macros */
    no_chg = 1;
    while (CurC != '\0') {
    	/* If we have an identifier, check if it's a macro */
     	if (IsIdent (CurC)) {
     	    SymName (Ident);
     	    M = FindMacro (Ident);
	    if (M) {
	    	ExpandMacro (M);
	    	no_chg = 0;
	    } else {
	    	KeepStr (Ident);
	    }
     	} else if (IsQuote (CurC)) {
     	    mptr = CopyQuotedString (mptr);
     	} else {
     	    KeepChar (CurC);
	    NextChar ();
     	}
    }
    return no_chg;
}



static void PreprocessLine (void)
/* Translate one line. */
{
    unsigned I;

    /* Trim whitespace and remove comments. The function returns false if no
     * identifiers were found that may be macros. If this is the case, no
     * macro substitution is performed.
     */
    int Done = Pass1 (line, mline);

    /* Repeatedly expand macros in the line */
    for (I = 0; I < 256; ++I) {
     	/* Swap mline and line */
        SwapLineBuffers ();
      	if (Done) {
      	    break;
        }
        /* Perform macro expansion */
      	Done = Pass2 (line, mline);
      	KeepChar ('\0');
    }

    /* Reinitialize line parsing */
    InitLine (line);
}



static void DoUndef (void)
/* Process the #undef directive */
{
    ident Ident;

    SkipBlank ();
    if (MacName (Ident)) {
	UndefineMacro (Ident);
    }
}



static int PushIf (int Skip, int Invert, int Cond)
/* Push a new if level onto the if stack */
{
    /* Check for an overflow of the if stack */
    if (IfIndex >= MAX_IFS-1) {
	PPError ("Too many nested #if clauses");
	return 1;
    }

    /* Push the #if condition */
    ++IfIndex;
    if (Skip) {
     	IfStack[IfIndex] = IFCOND_SKIP | IFCOND_NEEDTERM;
     	return 1;
    } else {
     	IfStack[IfIndex] = IFCOND_NONE | IFCOND_NEEDTERM;
     	return (Invert ^ Cond);
    }
}



static int DoIf (int Skip)
/* Process #if directive */
{
    ExprDesc lval;
    char* S;

    /* We're about to abuse the compiler expression parser to evaluate the
     * #if expression. Save the current tokens to come back here later.
     * NOTE: Yes, this is a hack, but it saves a complete separate expression
     * evaluation for the preprocessor.
     */
    Token sv1 = CurTok;
    Token sv2 = NextTok;

    /* Make sure the line infos for the tokens won't get removed */
    if (sv1.LI) {
    	UseLineInfo (sv1.LI);
    }
    if (sv2.LI) {
    	UseLineInfo (sv2.LI);
    }

    /* Remove the #if from the line */
    SkipBlank ();
    S = line;
    while (CurC != '\0') {
    	*S++ = CurC;
    	NextChar ();
    }
    *S = '\0';

    /* Start over parsing from line */
    InitLine (line);

    /* Switch into special preprocessing mode */
    Preprocessing = 1;

    /* Expand macros in this line */
    PreprocessLine ();

    /* Add two semicolons as sentinels to the line, so the following
     * expression evaluation will eat these two tokens but nothing from
     * the following line.
     */
    strcat (line, ";;");

    /* Prime the token pump (remove old tokens from the stream) */
    NextToken ();
    NextToken ();

    /* Call the expression parser */
    ConstExpr (&lval);

    /* End preprocessing mode */
    Preprocessing = 0;

    /* Reset the old tokens */
    CurTok  = sv1;
    NextTok = sv2;

    /* Set the #if condition according to the expression result */
    return PushIf (Skip, 1, lval.ConstVal != 0);
}



static int DoIfDef (int skip, int flag)
/* Process #ifdef if flag == 1, or #ifndef if flag == 0. */
{
    ident Ident;

    SkipBlank ();
    if (MacName (Ident) == 0) {
       	return 0;
    } else {
	return PushIf (skip, flag, IsMacro(Ident));
    }
}



static void DoInclude (void)
/* Open an include file. */
{
    char    	RTerm;
    unsigned	DirSpec;


    /* Skip blanks */
    SkipBlank ();

    /* Get the next char and check for a valid file name terminator. Setup
     * the include directory spec (SYS/USR) by looking at the terminator.
     */
    switch (CurC) {

       	case '\"':
       	    RTerm   = '\"';
       	    DirSpec = INC_USER;
       	    break;

       	case '<':
       	    RTerm   = '>';
       	    DirSpec = INC_SYS;
       	    break;

       	default:
       	    PPError ("`\"' or `<' expected");
       	    goto Done;
    }
    NextChar ();

    /* Copy the filename into mline. Since mline has the same size as the
     * input line, we don't need to check for an overflow here.
     */
    mptr = mline;
    while (CurC != '\0' && CurC != RTerm) {
	KeepChar (CurC);
	NextChar ();
    }
    *mptr = '\0';

    /* Check if we got a terminator */
    if (CurC != RTerm) {
       	/* No terminator found */
       	PPError ("Missing terminator or file name too long");
       	goto Done;
    }

    /* Open the include file */
    OpenIncludeFile (mline, DirSpec);

Done:
    /* Clear the remaining line so the next input will come from the new
     * file (if open)
     */
    ClearLine ();
}



static void DoError (void)
/* Print an error */
{
    SkipBlank ();
    if (CurC == '\0') {
 	PPError ("Invalid #error directive");
    } else {
        PPError ("#error: %s", lptr);
    }

    /* Clear the rest of line */
    ClearLine ();
}



static void DoPragma (void)
/* Handle a #pragma line by converting the #pragma preprocessor directive into
 * the _Pragma() compiler operator.
 */
{
    /* Skip blanks following the #pragma directive */
    SkipBlank ();

    /* Copy the remainder of the line into mline removing comments and ws */
    Pass1 (lptr, mline);

    /* Convert the directive into the operator */
    mptr = line;
    KeepStr ("_Pragma (");
    Stringize (mline);
    KeepChar (')');
    *mptr = '\0';

    /* Initialize reading from line */
    InitLine (line);
}



void Preprocess (void)
/* Preprocess a line */
{
    int    	Skip;
    ident  	Directive;

    /* Skip white space at the beginning of the line */
    SkipBlank ();

    /* Check for stuff to skip */
    Skip = 0;
    while (CurC == '\0' || CurC == '#' || Skip) {

       	/* Check for preprocessor lines lines */
       	if (CurC == '#') {
       	    NextChar ();
       	    SkipBlank ();
       	    if (CurC == '\0') {
       	    	/* Ignore the empty preprocessor directive */
       	    	continue;
       	    }
       	    if (!IsSym (Directive)) {
       	    	PPError ("Preprocessor directive expected");
       	    	ClearLine ();
       	    } else {
       	       	switch (FindPPToken (Directive)) {

       	       	    case PP_DEFINE:
       	    	    	if (!Skip) {
       	    	    	    DefineMacro ();
       	    	    	}
       	    	    	break;

	   	    case PP_ELIF:
	   	        if (IfIndex >= 0) {
	   	    	    if ((IfStack[IfIndex] & IFCOND_ELSE) == 0) {

	   			/* Handle as #else/#if combination */
	   	    	     	if ((IfStack[IfIndex] & IFCOND_SKIP) == 0) {
	   	    	     	    Skip = !Skip;
	   	    	     	}
	   	    	     	IfStack[IfIndex] |= IFCOND_ELSE;
	   			Skip = DoIf (Skip);

	   			/* #elif doesn't need a terminator */
	   			IfStack[IfIndex] &= ~IFCOND_NEEDTERM;
	   		    } else {
	   	    	  	PPError ("Duplicate #else/#elif");
	   	    	    }
		    	} else {
		    	    PPError ("Unexpected #elif");
		    	}
		        break;

       	       	    case PP_ELSE:
       	    	    	if (IfIndex >= 0) {
	    	    	    if ((IfStack[IfIndex] & IFCOND_ELSE) == 0) {
		    	     	if ((IfStack[IfIndex] & IFCOND_SKIP) == 0) {
		    	     	    Skip = !Skip;
		    	     	}
		    	     	IfStack[IfIndex] |= IFCOND_ELSE;
		    	    } else {
		    	     	PPError ("Duplicate #else");
		    	    }
       	    	    	} else {
       	    	    	    PPError ("Unexpected `#else'");
       	    	    	}
       	    	    	break;

       	       	    case PP_ENDIF:
       	    	    	if (IfIndex >= 0) {
			    /* Remove any clauses on top of stack that do not
			     * need a terminating #endif.
			     */
    			    while (IfIndex >= 0 && (IfStack[IfIndex] & IFCOND_NEEDTERM) == 0) {
			       	--IfIndex;
			    }

			    /* Stack may not be empty here or something is wrong */
			    CHECK (IfIndex >= 0);

			    /* Remove the clause that needs a terminator */
			    Skip = (IfStack[IfIndex--] & IFCOND_SKIP) != 0;
       	    	    	} else {
       	    	    	    PPError ("Unexpected `#endif'");
       	    	    	}
       	    	    	break;

       	       	    case PP_ERROR:
       	    	    	if (!Skip) {
       	    	    	    DoError ();
	    	    	}
    	    	    	break;

       	       	    case PP_IF:
    	    	    	Skip = DoIf (Skip);
    	    	    	break;

       	       	    case PP_IFDEF:
    	    	    	Skip = DoIfDef (Skip, 1);
    	    	    	break;

       	       	    case PP_IFNDEF:
    	    	    	Skip = DoIfDef (Skip, 0);
    	    	    	break;

       	       	    case PP_INCLUDE:
    	    	    	if (!Skip) {
    	    	    	    DoInclude ();
    	    	    	}
    	    	    	break;

       	       	    case PP_LINE:
	   		/* Not allowed in strict ANSI mode */
			if (!Skip && ANSI) {
			    PPError ("Preprocessor directive expected");
			    ClearLine ();
    			}
    	    	    	break;

       	       	    case PP_PRAGMA:
    	    	    	if (!Skip) {
                            DoPragma ();
                            goto Done;
    	    	    	}
    	    	    	break;

       	       	    case PP_UNDEF:
    	    	    	if (!Skip) {
    	    	    	    DoUndef ();
    	    	    	}
    	    	    	break;

    	    	    default:
    	    	    	PPError ("Preprocessor directive expected");
    	    	    	ClearLine ();
    	    	}
    	    }

    	}
    	if (NextLine () == 0) {
    	    if (IfIndex >= 0) {
    	    	PPError ("`#endif' expected");
    	    }
    	    return;
    	}
    	SkipBlank ();
    }

    PreprocessLine ();

Done:
    Print (stdout, 2, "line: %s\n", line);
}

