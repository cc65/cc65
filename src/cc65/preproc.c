
/* C pre-processor functions */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

/* common */
#include "chartype.h"
#include "xmalloc.h"

/* cc65 */
#include "codegen.h"
#include "error.h"
#include "expr.h"
#include "global.h"
#include "ident.h"
#include "incpath.h"
#include "input.h"
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
/*  		    		     data				     */
/*****************************************************************************/



/* Set when the pp calls expr() recursively */
unsigned char Preprocessing = 0;

/* Management data for #if */
#define N_IFDEF	    	16
static int i_ifdef = -1;
static char s_ifdef[N_IFDEF];

/* Buffer for macro expansion */
static char mlinebuf [LINESIZE];
static char* mline = mlinebuf;
static char* mptr;

/* Flag: Expand macros in this line */
static int ExpandMacros = 1;



/*****************************************************************************/
/* 		    		     code				     */
/*****************************************************************************/



static int keepch (char c)
/* Put character c into translation buffer. */
{
    return (*mptr++ = c);
}



static void keepstr (const char* S)
/* Put string str into translation buffer. */
{
    while (*S) {
	keepch (*S++);
    }
}



static void Comment (void)
/* Remove a C comment from line. */
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
/*				  Macro stuff				     */
/*****************************************************************************/



static int MacName (char* Ident)
/* Get macro symbol name.  If error, print message and clear line. */
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
/* Preprocessor pass 2.  Perform macro substitution. */
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
     	    	keepstr (Replacement);
     	    } else {
		/* No macro argument, keep the original identifier */
     	    	keepstr (Ident);
     	    }
     	} else if (CurC == '#' && IsIdent (NextC)) {
       	    NextChar ();
     	    SymName (Ident);
	    Replacement = FindMacroArg (M, Ident);
       	    if (Replacement) {
     	    	keepch ('\"');
     	    	keepstr (Replacement);
     	    	keepch ('\"');
     	    } else {
     	    	keepch ('#');
     	    	keepstr (Ident);
     	    }
     	} else if (IsQuote (CurC)) {
     	    mptr = CopyQuotedString (mptr);
     	} else {
     	    *mptr++ = CurC;
	    NextChar ();
     	}
    }

    /* Reset the line pointer */
    InitLine (SavePtr);
}



static int MacroCall (Macro* M)
/* Process a function like macro */
{
    unsigned  	ArgCount;      	/* Macro argument count */
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
     	keepstr (M->Replacement);
    }
}



static void addmac (void)
/* Add a macro to the macro table. */
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
     	    keepch (' ');
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
     	    	    *mptr++ = '0';
     	    	} else {
     	    	    SymName (Ident);
     	    	    *mptr++ = IsMacro (Ident)? '1' : '0';
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
	    	keepstr (Ident);
	    }
	} else if (IsQuote (CurC)) {
	    mptr = CopyQuotedString (mptr);
	} else if (CurC == '/' && NextC == '*') {
	    keepch (' ');
     	    Comment ();
     	} else if (ANSI == 0 && CurC == '/' && NextC == '/') {
     	    keepch (' ');
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
     	} else {
     	    *mptr++ = CurC;
	    NextChar ();
     	}
    }
    keepch ('\0');
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
	    	keepstr (Ident);
	    }
     	} else if (IsQuote (CurC)) {
     	    mptr = CopyQuotedString (mptr);
     	} else {
     	    *mptr++ = CurC;
	    NextChar ();
     	}
    }
    return no_chg;
}



static void xlateline (void)
/* Translate one line. */
{
    int cnt;
    int Done;

    Done = Pass1 (line, mline);
    if (ExpandMacros == 0) {
	Done = 1;
	ExpandMacros = 1;	/* Reset to default */
    }
    cnt = 5;
    do {
	/* Swap mline and line */
       	char* p = line;
	line = mline;
    	mline = p;
	if (Done)
	    break;
	Done = Pass2 (line, mline);
	keepch ('\0');
    } while (--cnt);

    /* Reinitialize line parsing */
    InitLine (line);
}



static void doundef (void)
/* Process #undef directive */
{
    ident Ident;

    SkipBlank ();
    if (MacName (Ident)) {
	UndefineMacro (Ident);
    }
}



static int setmflag (int skip, int flag, int cond)
/* setmflag( skip, flag, cond ) */
{
    if (skip) {
     	s_ifdef[++i_ifdef] = 3;
     	return (1);
    } else {
     	s_ifdef[++i_ifdef] = 6;
     	return (flag ^ cond);
    }
}



static int doiff (int skip)
/* Process #if directive */
{
    struct expent lval;
    char* S;

    /* We're about to abuse the compiler expression parser to evaluate the
     * #if expression. Save the current tokens to come back here later.
     */
    Token sv1 = CurTok;
    Token sv2 = NextTok;

    /* Remove the #if from the line and add two semicolons as sentinels */
    SkipBlank ();
    S = line;
    while (CurC != '\0') {
	*S++ = CurC;
	NextChar ();
    }
    *S++ = ';';
    *S++ = ';';
    *S   = '\0';

    /* Start over parsing from line */
    InitLine (line);

    /* Switch into special preprocessing mode */
    Preprocessing = 1;

    /* Expand macros in this line */
    xlateline ();

    /* Prime the token pump (remove old tokens from the stream) */
    NextToken ();
    NextToken ();

    /* Call the expression parser */
    constexpr (&lval);

    /* End preprocessing mode */
    Preprocessing = 0;

    /* Reset the old tokens */
    CurTok  = sv1;
    NextTok = sv2;

    /* Set the #if condition according to the expression result */
    return (setmflag (skip, 1, lval.e_const != 0));
}



static int doifdef (int skip, int flag)
/* Process #ifdef if flag == 1, or #ifndef if flag == 0. */
{
    ident Ident;

    SkipBlank ();
    if (MacName (Ident) == 0) {
       	return 0;
    } else {
	return setmflag (skip, flag, IsMacro(Ident));
    }
}



static void doinclude (void)
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
	*mptr++ = CurC;
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



static void doerror (void)
/* Print an error */
{
    SkipBlank ();
    if (CurC == '\0') {
 	PPError ("Invalid #error directive");
    } else {
        PPError ("#error: %s", lptr);
    }

    /* clear rest of line */
    ClearLine ();
}



/* C preprocessor. */

/* stuff used to bum the keyword dispatching stuff */
enum {
    PP_DEFINE,
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
};

static const struct tok_elt pre_toks[] = {
    {  	"define",      	PP_DEFINE	},
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
    {  	0,     	       	PP_ILLEGAL	}
};



static int searchtok (const char *sym, const struct tok_elt *toks)
/* Search a token in a table */
{
    while (toks->toknam && strcmp (toks->toknam, sym))
	++toks;
    return (toks->toknbr);
}



void Preprocess (void)
/* Preprocess a line */
{
    int 	Skip;
    ident	Directive;

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
       	       	switch (searchtok (Directive, pre_toks)) {

       	       	    case PP_DEFINE:
       	    	    	if (!Skip) {
       	    	    	    addmac ();
       	    	    	}
       	    	    	break;

       	       	    case PP_ELSE:
       	    	    	if (s_ifdef[i_ifdef] & 2) {
       	    	    	    if (s_ifdef[i_ifdef] & 4) {
       	    	    	     	Skip = !Skip;
       	    	    	    }
       	    	    	    s_ifdef[i_ifdef] ^= 2;
       	    	    	} else {
       	    	    	    PPError ("Unexpected `#else'");
       	    	    	}
       	    	    	break;

       	       	    case PP_ENDIF:
       	    	    	if (i_ifdef >= 0) {
       	    	     	    Skip = s_ifdef[i_ifdef--] & 1;
       	    	    	} else {
       	    	    	    PPError ("Unexpected `#endif'");
       	    	    	}
       	    	    	break;

       	       	    case PP_ERROR:
       	    	    	if (!Skip) {
       	    	    	    doerror ();
	    	    	}
    	    	    	break;

       	       	    case PP_IF:
    	    	    	Skip = doiff (Skip);
    	    	    	break;

       	       	    case PP_IFDEF:
    	    	    	Skip = doifdef (Skip, 1);
    	    	    	break;

       	       	    case PP_IFNDEF:
    	    	    	Skip = doifdef (Skip, 0);
    	    	    	break;

       	       	    case PP_INCLUDE:
    	    	    	if (!Skip) {
    	    	    	    doinclude ();
    	    	    	}
    	    	    	break;

       	       	    case PP_LINE:
	    	    	/* Not allowed in strict ANSI mode */
	    	    	if (ANSI) {
	    	    	    PPError ("Preprocessor directive expected");
	    	    	    ClearLine ();
	    	    	}
	    	    	break;

       	       	    case PP_PRAGMA:
	    	    	if (!Skip) {
	    	    	    /* Don't expand macros in this line */
	    	    	    ExpandMacros = 0;
    	    	    	    /* #pragma is handled on the scanner level */
	    	    	    goto Done;
	    	    	}
    	    	    	break;

       	       	    case PP_UNDEF:
    	    	    	if (!Skip) {
    	    	    	    doundef ();
    	    	    	}
    	    	    	break;

    	    	    default:
    	    	    	PPError ("Preprocessor directive expected");
    	    	    	ClearLine ();
    	    	}
	    }

    	}
    	if (NextLine () == 0) {
    	    if (i_ifdef >= 0) {
    	    	PPError ("`#endif' expected");
    	    }
    	    return;
    	}
  	SkipBlank ();
    }

Done:
    xlateline ();
    if (Verbose > 1) {
    	printf ("line: %s\n", line);
    }
}

