
/* C pre-processor functions */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>

#include "codegen.h"
#include "error.h"
#include "expr.h"
#include "global.h"
#include "ident.h"
#include "include.h"
#include "io.h"
#include "macrotab.h"
#include "mem.h"
#include "scanner.h"
#include "util.h"
#include "preproc.h"



/*****************************************************************************/
/*				   Forwards				     */
/*****************************************************************************/



static int Pass1 (char* from, char* to);



/*****************************************************************************/
/*		    		     data				     */
/*****************************************************************************/



/* Set when the pp calls expr() recursively */
unsigned char Preprocessing = 0;

/* Management data for #if */
#define N_IFDEF		16
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



static void comment (void)
/* Remove comment from line. */
{
    unsigned StartingLine = ln;

    gch ();
    gch ();
    while (*lptr != '*' || nch () != '/') {
    	if (*lptr == '\0') {
    	    if (readline () == 0) {
    		PPError (ERR_EOF_IN_COMMENT, StartingLine);
    	    	return;
    	    }
	} else {
	    if (*lptr == '/' && nch() == '*') {
		PPWarning (WARN_NESTED_COMMENT);
	    }
	    ++lptr;
	}
    }
    gch ();
    gch ();
}



static void skipblank (void)
/* Skip blanks and tabs in the input stream. */
{
    while (IsBlank (*lptr)) {
	++lptr;
    }
}



static char* CopyQuotedString (int Quote, char* Target)
/* Copy a single or double quoted string from lptr to Target. Return the
 * new target pointer. Target will not be terminated after the copy.
 */
{
    /* Copy the starting quote */
    *Target++ = gch();

    /* Copy the characters inside the string */
    while (*lptr != '\0' && *lptr != Quote) {
       	/* Keep an escaped char */
 	if (*lptr == '\\') {
 	    *Target++ = gch();
 	}
 	/* Copy the character */
 	*Target++ = cgch();
    }

    /* If we had a terminating quote, copy it */
    if (*lptr) {
 	*Target++ = gch();
    }

    /* Return the new target pointer */
    return Target;
}



/*****************************************************************************/
/*				  Macro stuff				     */
/*****************************************************************************/



static int macname (char *sname)
/* Get macro symbol name.  If error, print message and kill line. */
{
    if (issym (sname) == 0) {
	PPError (ERR_IDENT_EXPECTED);
     	kill ();
	return 0;
    } else {
    	return 1;
    }
}



static void ExpandMacroArgs (Macro* M)
/* Preprocessor pass 2.  Perform macro substitution. */
{
    int		C;
    ident	Ident;
    const char* Replacement;
    char*	SavePtr;

    /* Save the current line pointer and setup the new ones */
    SavePtr = lptr;
    lptr    = M->Replacement;

    /* Copy the macro replacement checking for parameters to replace */
    while ((C = *lptr) != '\0') {
	/* If the next token is an identifier, check for a macro arg */
     	if (IsIdent (C)) {
     	    symname (Ident);
	    Replacement = FindMacroArg (M, Ident);
	    if (Replacement) {
		/* Macro arg, keep the replacement */
     	    	keepstr (Replacement);
     	    } else {
		/* No macro argument, keep the original identifier */
     	    	keepstr (Ident);
     	    }
     	} else if (C == '#' && IsIdent (nch ())) {
     	    ++lptr;
     	    symname (Ident);
	    Replacement = FindMacroArg (M, Ident);
       	    if (Replacement) {
     	    	keepch ('\"');
     	    	keepstr (Replacement);
     	    	keepch ('\"');
     	    } else {
     	    	keepch ('#');
     	    	keepstr (Ident);
     	    }
     	} else if (IsQuoteChar(C)) {
     	    mptr = CopyQuotedString (C, mptr);
     	} else {
     	    *mptr++ = *lptr++;
     	}
    }

    /* Reset the line pointer */
    lptr = SavePtr;
}



static int MacroCall (Macro* M)
/* Process a function like macro */
{
    unsigned 	ArgCount;      	/* Macro argument count */
    unsigned 	ParCount;       /* Number of open parenthesis */
    char     	Buf[LINESIZE];	/* Argument buffer */
    char	C;
    const char* ArgStart;
    char*    	B;

    /* Expect an argument list */
    skipblank ();
    if (*lptr != '(') {
     	PPError (ERR_ILLEGAL_MACRO_CALL);
     	return 0;
    }

    /* Eat the left paren */
    ++lptr;

    /* Read the actual macro arguments and store pointers to these arguments
     * into the array of actual arguments in the macro definition.
     */
    ArgCount = 0;
    ParCount = 0;
    ArgStart = Buf;
    B 	     = Buf;
    while (1) {
     	C = *lptr;
     	if (C == '(') {
     	    *B++ = gch ();
     	    ++ParCount;
     	} else if (IsQuoteChar(C)) {
    	    B = CopyQuotedString (C, B);
     	} else if (C == ',' || C == ')') {
     	    if (ParCount == 0) {
	 	/* End of actual argument */
     	       	gch ();
     	       	*B++ = '\0';
		while (IsBlank(*ArgStart)) {
		    ++ArgStart;
		}
    	      	if (ArgCount < M->ArgCount) {
    	      	    M->ActualArgs[ArgCount++] = ArgStart;
       	       	} else if (C != ')' || *ArgStart != '\0' || M->ArgCount > 0) {
		    /* Be sure not to count the single empty argument for a
		     * macro that does not have arguments.
		     */
    	      	    ++ArgCount;
		}

       	       	/* Start the next one */
     	       	ArgStart = B;
     	       	if (C == ')') {
     	       	    break;
     	       	}
     	    } else {
     	       	*B++ = gch ();
     	       	if (C == ')') {
     	       	    --ParCount;
     	       	}
     	    }
     	} else if (IsBlank (C)) {
	    /* Squeeze runs of blanks */
     	    *B++ = ' ';
     	    skipblank ();
     	} else if (C == '\0') {
	    /* End of line inside macro argument list - read next line */
     	    if (readline () == 0) {
     	       	return 0;
     	    }
     	} else {
	    /* Just copy the character */
     	    *B++ = *lptr++;
     	}
    }

    /* Compare formal argument count with actual */
    if (M->ArgCount != ArgCount) {
	PPError (ERR_MACRO_ARGCOUNT);
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
     	    kill ();
     	}
    } else {
	/* Just copy the replacement text */
     	keepstr (M->Replacement);
    }
}



static void addmac (void)
/* Add a macro to the macro table. */
{
    char*	saveptr;
    ident	Ident;
    char 	Buf[LINESIZE];
    Macro* 	M;

    /* Read the macro name */
    skipblank ();
    if (!macname (Ident)) {
    	return;
    }

    /* Create a new macro definition */
    M = NewMacro (Ident);

    /* Check if this is a function like macro */
    if (*lptr == '(') {

    	/* Skip the left paren */
    	gch ();

       	/* Set the marker that this is a function like macro */
	M->ArgCount = 0;

	/* Read the formal parameter list */
     	while (1) {
     	    skipblank ();
     	    if (*lptr == ')')
     	    	break;
     	    if (macname (Ident) == 0) {
     	    	return;
     	    }
	    AddMacroArg (M, Ident);
     	    skipblank ();
     	    if (*lptr != ',')
     	    	break;
     	    gch ();
     	}
     	if (*lptr != ')') {
       	    PPError (ERR_RPAREN_EXPECTED);
     	    kill ();
     	    return;
     	}
     	gch ();
    }

    /* Insert the macro into the macro table and allocate the ActualArgs array */
    InsertMacro (M);

    /* Remove whitespace and comments from the line, store the preprocessed
     * line into Buf.
     */
    skipblank ();
    saveptr = mptr;
    Pass1 (lptr, Buf);
    mptr = saveptr;

    /* Create a copy of the replacement */
    M->Replacement = xstrdup (Buf);
}



/*****************************************************************************/

/*****************************************************************************/



static int Pass1 (char* from, char* to)
/* Preprocessor pass 1.  Remove whitespace and comments. */
{
    int 	c;
    int 	done;
    ident	Ident;
    int 	HaveParen;

    lptr = from;
    mptr = to;
    done = 1;
    while ((c = *lptr) != 0) {
     	if (IsBlank (c)) {
     	    keepch (' ');
     	    skipblank ();
       	} else if (IsIdent (c)) {
     	    symname (Ident);
     	    if (Preprocessing && strcmp(Ident, "defined") == 0) {
     	   	/* Handle the "defined" operator */
     	   	skipblank();
     	   	HaveParen = 0;
     	   	if (*lptr == '(') {
     	   	    HaveParen = 1;
     	   	    ++lptr;
     	   	    skipblank();
     	   	}
     	   	if (!IsIdent(c)) {
     	   	    PPError (ERR_IDENT_EXPECTED);
     	   	    *mptr++ = '0';
     	   	} else {
     	   	    symname (Ident);
     	   	    *mptr++ = IsMacro(Ident)? '1' : '0';
     	   	    if (HaveParen) {
     	   	  	skipblank();
     	   	  	if (*lptr != ')') {
     	   	  	    PPError (ERR_RPAREN_EXPECTED);
	   	  	} else {
	   	  	    ++lptr;
	   	  	}
	   	    }
	   	}
	    } else {
	   	if (MaybeMacro(c)) {
	   	    done = 0;
	   	}
	   	keepstr (Ident);
	    }
	} else if (IsQuoteChar(c)) {
	    mptr = CopyQuotedString (c, mptr);
	} else if (c == '/' && nch () == '*') {
	    keepch (' ');
     	    comment ();
     	} else if (ANSI == 0 && c == '/' && nch () == '/') {
     	    keepch (' ');
	    /* Beware: Because line continuation chars are handled when reading
	     * lines, we may only skip til the end of the source line, which
	     * may not be the same as the end of the input line. The end of the
	     * source line is denoted by a lf (\n) character.
	     */
	    do {
		++lptr;
	    } while (*lptr != '\n' && *lptr != '\0');
	    if (*lptr == '\n') {
		++lptr;
	    }
     	} else {
     	    *mptr++ = *lptr++;
     	}
    }
    keepch ('\0');
    return done;
}



static int Pass2 (char *from, char *to)
/* Preprocessor pass 2.  Perform macro substitution. */
{
    int    	C;
    int    	no_chg;
    ident	Ident;
    Macro* 	M;

    lptr = from;
    mptr = to;
    no_chg = 1;
    while ((C = *lptr) != '\0') {
	/* If we have an identifier, check if it's a macro */
     	if (IsIdent (C)) {
     	    symname (Ident);
     	    M = FindMacro (Ident);
	    if (M) {
		ExpandMacro (M);
		no_chg = 0;
	    } else {
		keepstr (Ident);
	    }
     	} else if (IsQuoteChar(C)) {
     	    mptr = CopyQuotedString (C, mptr);
     	} else {
     	    *mptr++ = *lptr++;
     	}
    }
    return no_chg;
}



static void xlateline (void)
/* Translate one line. */
{
    int cnt;
    int Done;
    char *p;

    Done = Pass1 (line, mline);
    if (ExpandMacros == 0) {
	Done = 1;
	ExpandMacros = 1;	/* Reset to default */
    }
    cnt = 5;
    do {
	p = line;
	line = mline;
    	mline = p;
	if (Done)
	    break;
	Done = Pass2 (line, mline);
	keepch ('\0');
    } while (--cnt);
    lptr = line;
}



static void doundef (void)
/* Process #undef directive */
{
    ident Ident;

    skipblank ();
    if (macname (Ident)) {
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
    skipblank ();
    S = line;
    while ((*S++ = *lptr++) != '\0') ;
    strcat (line, ";;");
    lptr = line;

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

    skipblank ();
    if (macname (Ident) == 0) {
       	return 0;
    } else {
	return setmflag (skip, flag, IsMacro(Ident));
    }
}



static void doinclude (void)
/* Open an include file. */
{
    char name [80];
    unsigned count;
    char term;
    char c;
    char *p;

    if (ifile >= MAXFILES) {
     	PPError (ERR_INCLUDE_NESTING);
     	goto done;
    }
    mptr = mline;
    skipblank ();
    if (!strchr ("\"<", (term = cgch ()))) {
       	PPError (ERR_INCLUDE_LTERM_EXPECTED);
     	goto done;
    }
    if (term == '<') {
     	term = '>';	  	/* get right terminator */
    }

    /* Get the name of the include file */
    count = 0;
    while ((c = *lptr) && (c != term) && count < sizeof (name)-1) {
	name [count++] = c;
	++lptr;
    }
    if (c != term) {
     	PPError (ERR_INCLUDE_RTERM_EXPECTED);
	goto done;
    }
    name [count] = '\0';

    /* Now search for the name */
    p = FindInclude (name, (term == '\"')? INC_USER : INC_SYS);
    if (p == 0) {
	PPError (ERR_INCLUDE_NOT_FOUND, name);
     	goto done;
    }

    /* Save the existing file info */
    filetab[ifile].f_ln = ln;
    filetab[ifile].f_name = fin;
    filetab[ifile].f_iocb = inp;
    ++ifile;

    /* Assign the name and output it */
    fin = p;
    if (Verbose) {
     	printf ("including '%s'\n", fin);
    }

    /* Try to open the include file */
    if ((inp = fopen (fin, "r")) == 0) {
    	/* oops! restore old file */
	PPError (ERR_INCLUDE_OPEN_FAILURE, fin);
	xfree (fin);
	--ifile;
     	inp = filetab[ifile].f_iocb;
     	fin = filetab[ifile].f_name;
    } else {
     	ln = 0;
    }

done:
    /* clear rest of line so next read will come from new file (if open) */
    kill ();
}



static void doerror (void)
/* Print an error */
{
    skipblank ();
    if (*lptr == '\0') {
 	PPError (ERR_INVALID_USER_ERROR);
    } else {
        PPError (ERR_USER_ERROR, lptr);
    }

    /* clear rest of line */
    kill ();
}



/* C preprocessor. */

/* stuff used to bum the keyword dispatching stuff */
enum {
    D_DEFINE,
    D_ELSE,
    D_ENDIF,
    D_ERROR,
    D_IF,
    D_IFDEF,
    D_IFNDEF,
    D_INCLUDE,
    D_LINE,
    D_PRAGMA,
    D_UNDEF,
    D_ILLEGAL,
};

static const struct tok_elt pre_toks[] = {
    {	"define",	D_DEFINE	},
    {	"else",		D_ELSE		},
    {	"endif",	D_ENDIF		},
    {	"error",	D_ERROR		},
    {	"if",		D_IF		},
    {	"ifdef",	D_IFDEF		},
    {	"ifndef",	D_IFNDEF	},
    {	"include", 	D_INCLUDE	},
    {   "line",		D_LINE		},
    {	"pragma",	D_PRAGMA	},
    {	"undef",	D_UNDEF		},
    {	0,		D_ILLEGAL	}
};



int searchtok (const char *sym, const struct tok_elt *toks)
/* Search a token in a table */
{
    while (toks->toknam && strcmp (toks->toknam, sym))
	++toks;
    return (toks->toknbr);
}



void preprocess (void)
/* Preprocess a line */
{
    int 	c;
    int 	Skip;
    ident	sname;

    /* Process compiler directives, skip empty lines */
    lptr = line;

    /* Skip white space at the beginning of the line */
    skipblank ();

    /* Check for stuff to skip */
    Skip = 0;
    while ((c = *lptr) == '\0' || c == '#' || Skip) {

  	/* Check for preprocessor lines lines */
       	if (c == '#') {
    	    ++lptr;
    	    skipblank ();
    	    if (*lptr == '\0') {
    	   	/* ignore the empty preprocessor directive */
    	   	continue;
    	    }
    	    if (!issym (sname)) {
    	    	PPError (ERR_CPP_DIRECTIVE_EXPECTED);
    	    	kill ();
    	    } else {
       	       	switch (searchtok (sname, pre_toks)) {

    	    	    case D_DEFINE:
    	    		if (!Skip) {
    	    		    addmac ();
    	    		}
    	    		break;

    		    case D_ELSE:
    			if (s_ifdef[i_ifdef] & 2) {
    			    if (s_ifdef[i_ifdef] & 4) {
    			     	Skip = !Skip;
    			    }
    		    	    s_ifdef[i_ifdef] ^= 2;
    			} else {
    			    PPError (ERR_UNEXPECTED_CPP_ELSE);
    			}
    			break;

    		    case D_ENDIF:
    			if (i_ifdef >= 0) {
    		     	    Skip = s_ifdef[i_ifdef--] & 1;
    			} else {
    			    PPError (ERR_UNEXPECTED_CPP_ENDIF);
    			}
    			break;

       	       	    case D_ERROR:
			if (!Skip) {
	    		    doerror ();
			}
    	    		break;

    		    case D_IF:
    	   		Skip = doiff (Skip);
    	   		break;

    	   	    case D_IFDEF:
    	   		Skip = doifdef (Skip, 1);
    	   		break;

    	   	    case D_IFNDEF:
    	   		Skip = doifdef (Skip, 0);
    	   		break;

    	    	    case D_INCLUDE:
    	    		if (!Skip) {
    	    		    doinclude ();
    	    		}
    	    		break;

		    case D_LINE:
			/* Not allowed in strict ANSI mode */
			if (ANSI) {
			    PPError (ERR_CPP_DIRECTIVE_EXPECTED);
			    kill ();
			}
			break;

    		    case D_PRAGMA:
			if (!Skip) {
			    /* Don't expand macros in this line */
			    ExpandMacros = 0;
    			    /* #pragma is handled on the scanner level */
			    goto Done;
			}
    			break;

    	    	    case D_UNDEF:
    	    		if (!Skip) {
    	    		    doundef ();
    	    		}
    	    		break;

    		    default:
    			PPError (ERR_CPP_DIRECTIVE_EXPECTED);
    	    		kill ();
    		}
	    }

    	}
    	if (readline () == 0) {
    	    if (i_ifdef >= 0) {
    		PPError (ERR_CPP_ENDIF_EXPECTED);
    	    }
    	    return;
    	}
  	skipblank ();
    }

Done:
    xlateline ();
    if (Verbose > 1) {
    	printf ("line: %s\n", line);
    }
}

