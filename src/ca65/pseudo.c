/*****************************************************************************/
/*                                                                           */
/*				   pseudo.c				     */
/*                                                                           */
/*		Pseudo instructions for the ca65 macroassembler		     */
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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "../common/bitops.h"

#include "condasm.h"
#include "error.h"
#include "expr.h"
#include "global.h"
#include "instr.h"
#include "listing.h"
#include "macpack.h"
#include "macro.h"
#include "nexttok.h"
#include "objcode.h"
#include "options.h"
#include "strexpr.h"
#include "symtab.h"
#include "pseudo.h"



/*****************************************************************************/
/*     	      	    	   	     Data				     */
/*****************************************************************************/



/* Keyword we're about to handle */
static char Keyword [sizeof (SVal)+1] = ".";



/*****************************************************************************/
/*				 Forwards				     */
/*****************************************************************************/



static void DoUnexpected (void);



/*****************************************************************************/
/*	      	    	        Helper functions			     */
/*****************************************************************************/



static void SetBoolOption (unsigned char* Flag)
/* Read a on/off/+/- option and set flag accordingly */
{
    static const char* Keys[] = {
       	"OFF",
    	"ON",
    };

    if (Tok == TOK_PLUS) {
       	*Flag = 1;
	NextTok ();
    } else if (Tok == TOK_MINUS) {
	*Flag = 0;
	NextTok ();
    } else if (Tok == TOK_IDENT) {
       	/* Map the keyword to a number */
       	switch (GetSubKey (Keys, sizeof (Keys) / sizeof (Keys [0]))) {
	    case 0:    	*Flag = 0; NextTok ();		break;
	    case 1:	*Flag = 1; NextTok ();		break;
	    default:	ErrorSkip (ERR_ONOFF_EXPECTED);	break;
	}
    } else if (Tok == TOK_SEP || Tok == TOK_EOF) {
	/* Without anything assume switch on */
	*Flag = 1;
    } else {
       	ErrorSkip (ERR_ONOFF_EXPECTED);
    }
}



static void ExportImport (void (*SymFunc) (const char*, int), int ZP)
/* Export or import symbols */
{
    while (1) {
     	if (Tok != TOK_IDENT) {
       	    ErrorSkip (ERR_IDENT_EXPECTED);
     	    break;
     	}
     	SymFunc (SVal, ZP);
     	NextTok ();
     	if (Tok == TOK_COMMA) {
     	    NextTok ();
     	} else {
     	    break;
     	}
    }
}



static long IntArg (long Min, long Max)
/* Read an integer argument and check a range. Accept the token "unlimited"
 * and return -1 in this case.
 */
{
    if (Tok == TOK_IDENT && strcmp (SVal, "unlimited") == 0) {
	NextTok ();
	return -1;
    } else {
	long Val = ConstExpression ();
	if (Val < Min || Val > Max) {
	    Error (ERR_RANGE);
	    Val = Min;
	}
	return Val;
    }
}



/*****************************************************************************/
/*	      	    	       Handler functions			     */
/*****************************************************************************/



static void DoA16 (void)
/* Switch the accu to 16 bit mode (assembler only) */
{
    if (GetCPU() != CPU_65816) {
	Error (ERR_816_MODE_ONLY);
    } else {
       	/* Immidiate mode has two extension bytes */
	ExtBytes [AMI_IMM_ACCU] = 2;
    }
}



static void DoA8 (void)
/* Switch the accu to 8 bit mode (assembler only) */
{
    if (GetCPU() != CPU_65816) {
	Error (ERR_816_MODE_ONLY);
    } else {
	/* Immidiate mode has one extension byte */
	ExtBytes [AMI_IMM_ACCU] = 1;
    }
}



static void DoAddr (void)
/* Define addresses */
{
    while (1) {
	if (GetCPU() == CPU_65816) {
       	    EmitWord (ForceWordExpr (Expression ()));
	} else {
	    /* Do a range check */
	    EmitWord (Expression ());
       	}
	if (Tok != TOK_COMMA) {
	    break;
	} else {
	    NextTok ();
	}
    }
}



static void DoAlign (void)
/* Align the PC to some boundary */
{
    long Val;
    long Align;
    unsigned Bit;

    /* Read the alignment value */
    Align = ConstExpression ();
    if (Align <= 0 || Align > 0x10000) {
       	ErrorSkip (ERR_RANGE);
      	return;
    }

    /* Optional value follows */
    if (Tok == TOK_COMMA) {
	NextTok ();
	Val = ConstExpression ();
	/* We need a byte value here */
	if (!IsByteRange (Val)) {
       	    ErrorSkip (ERR_RANGE);
	    return;
	}
    } else {
	Val = -1;
    }

    /* Check if the alignment is a power of two */
    Bit = BitFind (Align);
    if (Align != (0x01L << Bit)) {
	Error (ERR_ALIGN);
    } else {
	SegAlign (Bit, (int) Val);
    }
}



static void DoASCIIZ (void)
/* Define text with a zero terminator */
{
    while (1) {
	if (StringExpression () == 0) {
	    ErrorSkip (ERR_STRCON_EXPECTED);
	    return;
	}
	EmitData (SVal, strlen (SVal));
	NextTok ();
	if (Tok == TOK_COMMA) {
	    NextTok ();
	} else {
	    break;
	}
    }
    Emit0 (0);
}



static void DoAutoImport (void)
/* Mark unresolved symbols as imported */
{
    SetBoolOption (&AutoImport);
}



static void DoBss (void)
/* Switch to the BSS segment */
{
    UseBssSeg ();
}



static void DoByte (void)
/* Define bytes */
{
    while (1) {
	if (StringExpression () != 0) {
	    /* A string */
	    EmitData (SVal, strlen (SVal));
	    NextTok ();
	} else {
	    EmitByte (Expression ());
	}
	if (Tok != TOK_COMMA) {
	    break;
	} else {
	    NextTok ();
	    /* Do smart handling of dangling comma */
	    if (Tok == TOK_SEP) {
		Error (ERR_UNEXPECTED_EOL);
	 	break;
	    }
	}
    }
}



static void DoCase (void)
/* Switch the IgnoreCase option */
{
    SetBoolOption (&IgnoreCase);
    IgnoreCase = !IgnoreCase;
}



static void DoCode (void)
/* Switch to the code segment */
{
    UseCodeSeg ();
}



static void DoData (void)
/* Switch to the data segment */
{
    UseDataSeg ();
}



static void DoDByt (void)
/* Output double bytes */
{
    while (1) {
	EmitWord (SwapExpr (Expression ()));
	if (Tok != TOK_COMMA) {
	    break;
	} else {
	    NextTok ();
	}
    }
}



static void DoDebugInfo (void)
/* Switch debug info on or off */
{
    SetBoolOption (&DbgSyms);
}



static void DoDefine (void)
/* Define a one line macro */
{
    MacDef (MAC_STYLE_DEFINE);
}



static void DoDWord (void)
/* Define dwords */
{
    while (1) {
       	EmitDWord (Expression ());
	if (Tok != TOK_COMMA) {
	    break;
	} else {
	    NextTok ();
	}
    }
}



static void DoEnd (void)
/* End of assembly */
{
    ForcedEnd = 1;
}



static void DoEndProc (void)
/* Leave a lexical level */
{
    SymLeaveLevel ();
}



static void DoError (void)
/* Use error */
{
    if (StringExpression () == 0) {
 	ErrorSkip (ERR_STRCON_EXPECTED);
    } else {
       	Error (ERR_USER, SVal);
	SkipUntilSep ();
    }
}



static void DoExitMacro (void)
/* Exit a macro expansion */
{
    if (!InMacExpansion ()) {
	/* We aren't expanding a macro currently */
       	DoUnexpected ();
    } else {
	MacAbort ();
    }
}



static void DoExport (void)
/* Export a symbol */
{
    ExportImport (SymExport, 0);
}



static void DoExportZP (void)
/* Export a zeropage symbol */
{
    ExportImport (SymExport, 1);
}



static void DoFarAddr (void)
/* Define far addresses (24 bit) */
{
    while (1) {
       	EmitFarAddr (Expression ());
	if (Tok != TOK_COMMA) {
	    break;
	} else {
	    NextTok ();
	}
    }
}



static void DoFeature (void)
/* Switch the Feature option */
{
    int Feature;

    static const char* Keys[] = {
       	"DOLLAR_IS_PC",
    	"LABELS_WITHOUT_COLONS",
    	"LOOSE_STRING_TERM",
    	"AT_IN_IDENTIFIERS",
    	"DOLLAR_IN_IDENTIFIERS",
    };

    /* Allow a list of comma separated keywords */
    while (1) {

     	/* We expect an identifier */
     	if (Tok != TOK_IDENT) {
     	    ErrorSkip (ERR_IDENT_EXPECTED);
     	    return;
     	}

     	/* Map the keyword to a number */
     	Feature = GetSubKey (Keys, sizeof (Keys) / sizeof (Keys [0]));
     	if (Feature < 0) {
     	    /* Not found */
     	    ErrorSkip (ERR_ILLEGAL_FEATURE);
     	    return;
     	}

     	/* Skip the keyword */
     	NextTok ();

     	/* Switch the feature on */
     	switch (Feature) {
     	    case 0:	DollarIsPC 	= 1;	break;
     	    case 1:	NoColonLabels 	= 1;	break;
     	    case 2:	LooseStringTerm = 1;	break;
	    case 3:	AtInIdents 	= 1;	break;
	    case 4:	DollarInIdents	= 1;	break;
     	    default:	Internal ("Invalid feature: %d", Feature);
     	}

     	/* Allow more than one keyword */
     	if (Tok == TOK_COMMA) {
     	    NextTok ();
     	} else {
     	    break;
     	}
    }
}



static void DoFileOpt (void)
/* Insert a file option */
{
    long OptNum;

    /* The option type may be given as a keyword or as a number. */
    if (Tok == TOK_IDENT) {

	/* Option given as keyword */
	static const char* Keys [] = {
	    "AUTHOR", "COMMENT", "COMPILER"
      	};

	/* Map the option to a number */
	OptNum = GetSubKey (Keys, sizeof (Keys) / sizeof (Keys [0]));
	if (OptNum < 0) {
	    /* Not found */
	    ErrorSkip (ERR_OPTION_KEY_EXPECTED);
	    return;
	}

	/* Skip the keyword */
	NextTok ();

	/* Must be followed by a comma */
	ConsumeComma ();

	/* We accept only string options for now */
	if (StringExpression () == 0) {
	    ErrorSkip (ERR_STRCON_EXPECTED);
	    return;
	}

       	/* Insert the option */
	switch (OptNum) {

	    case 0:
		/* Author */
		OptAuthor (SVal);
		break;

	    case 1:
		/* Comment */
		OptComment (SVal);
		break;

	    case 2:
		/* Compiler */
		OptCompiler (SVal);
		break;

	    default:
	        Internal ("Invalid OptNum: %l", OptNum);

	}

	/* Done */
	NextTok ();

    } else {

     	/* Option given as number */
       	OptNum = ConstExpression ();
     	if (!IsByteRange (OptNum)) {
     	    ErrorSkip (ERR_RANGE);
     	    return;
     	}

	/* Must be followed by a comma */
	ConsumeComma ();

	/* We accept only string options for now */
	if (StringExpression () == 0) {
	    ErrorSkip (ERR_STRCON_EXPECTED);
	    return;
	}

	/* Insert the option */
	OptStr ((unsigned char) OptNum, SVal);

	/* Done */
	NextTok ();
    }
}



static void DoGlobal (void)
/* Declare a global symbol */
{
    ExportImport (SymGlobal, 0);
}



static void DoGlobalZP (void)
/* Declare a global zeropage symbol */
{
    ExportImport (SymGlobal, 1);
}



static void DoI16 (void)
/* Switch the index registers to 16 bit mode (assembler only) */
{
    if (GetCPU() != CPU_65816) {
     	Error (ERR_816_MODE_ONLY);
    } else {
       	/* Immidiate mode has two extension bytes */
     	ExtBytes [AMI_IMM_INDEX] = 2;
    }
}



static void DoI8 (void)
/* Switch the index registers to 16 bit mode (assembler only) */
{
    if (GetCPU() != CPU_65816) {
	Error (ERR_816_MODE_ONLY);
    } else {
	/* Immidiate mode has one extension byte */
	ExtBytes [AMI_IMM_INDEX] = 1;
    }
}



static void DoImport (void)
/* Import a symbol */
{
    ExportImport (SymImport, 0);
}



static void DoImportZP (void)
/* Import a zero page symbol */
{
    ExportImport (SymImport, 1);
}



static void DoIncBin (void)
/* Include a binary file */
{
    /* Name must follow */
    if (StringExpression () == 0) {
	ErrorSkip (ERR_STRCON_EXPECTED);
    } else {
	/* Try to open the file */
	FILE* F = fopen (SVal, "rb");
	if (F == 0) {
	    Error (ERR_CANNOT_OPEN_INCLUDE, SVal, strerror (errno));
	} else {
 	    unsigned char Buf [1024];
	    size_t Count;
	    /* Read chunks and insert them into the output */
	    while ((Count = fread (Buf, 1, sizeof (Buf), F)) > 0) {
		EmitData (Buf, Count);
	    }
	    /* Close the file, ignore errors since it's r/o */
	    (void) fclose (F);
	}
	/* Skip the name */
	NextTok ();
    }
}



static void DoInclude (void)
/* Include another file */
{
    char Name [MAX_STR_LEN+1];

    /* Name must follow */
    if (StringExpression () == 0) {
	ErrorSkip (ERR_STRCON_EXPECTED);
    } else {
	strcpy (Name, SVal);
	NextTok ();
	NewInputFile (Name);
    }
}



static void DoLineCont (void)
/* Switch the use of line continuations */
{
    SetBoolOption (&LineCont);
}



static void DoList (void)
/* Enable/disable the listing */
{
    /* Get the setting */
    unsigned char List;
    SetBoolOption (&List);

    /* Manage the counter */
    if (List) {
	EnableListing ();
    } else {
	DisableListing ();
    }
}



static void DoListBytes (void)
/* Set maximum number of bytes to list for one line */
{
    SetListBytes (IntArg (MIN_LIST_BYTES, MAX_LIST_BYTES));
}



static void DoLocalChar (void)
/* Define the character that starts local labels */
{
    if (Tok != TOK_CHARCON) {
     	ErrorSkip (ERR_CHARCON_EXPECTED);
    } else {
	if (IVal != '@' && IVal != '?') {
	    Error (ERR_ILLEGAL_LOCALSTART);
	} else {
     	    LocalStart = (char) IVal;
       	}
     	NextTok ();
    }
}



static void DoMacPack (void)
/* Insert a macro package */
{
    /* Macro package names */
    static const char* Keys [] = {
	"GENERIC",
       	"LONGBRANCH",
    };

    int Package;

    /* We expect an identifier */
    if (Tok != TOK_IDENT) {
    	ErrorSkip (ERR_IDENT_EXPECTED);
    	return;
    }

    /* Map the keyword to a number */
    Package = GetSubKey (Keys, sizeof (Keys) / sizeof (Keys [0]));
    if (Package < 0) {
    	/* Not found */
    	ErrorSkip (ERR_ILLEGAL_MACPACK);
    	return;
    }

    /* Skip the package name */
    NextTok ();

    /* Insert the package */
    InsertMacPack (Package);
}



static void DoMacro (void)
/* Start a macro definition */
{
    MacDef (MAC_STYLE_CLASSIC);
}



static void DoMid (void)
/* Handle .MID - this should never happen, since the keyword is actually
 * handled on a much lower level of the expression hierarchy.
 */
{
    Internal ("Unexpected token: .MID");
}



static void DoNull (void)
/* Switch to the NULL segment */
{
    UseNullSeg ();
}



static void DoOrg (void)
/* Start absolute code */
{
    long PC = ConstExpression ();
    if (PC < 0 || PC > 0xFFFF) {
	Error (ERR_RANGE);
    	return;
    }
    SetAbsPC (PC);
}



static void DoOut (void)
/* Output a string */
{
    if (StringExpression () == 0) {
	ErrorSkip (ERR_STRCON_EXPECTED);
    } else {
	/* Output the string and be sure to flush the output to keep it in
	 * sync with any error messages if the output is redirected to a file.
	 */
	printf ("%s\n", SVal);
	fflush (stdout);
	NextTok ();
    }
}



static void DoP02 (void)
/* Switch to 6502 CPU */
{
    SetCPU (CPU_6502);
}



static void DoPC02 (void)
/* Switch to 65C02 CPU */
{
    SetCPU (CPU_65C02);
}



static void DoP816 (void)
/* Switch to 65816 CPU */
{
    SetCPU (CPU_65816);
}



static void DoPageLength (void)
/* Set the page length for the listing */
{
    PageLength = IntArg (MIN_PAGE_LEN, MAX_PAGE_LEN);
}



static void DoProc (void)
/* Start a new lexical scope */
{
    if (Tok == TOK_IDENT) {
	/* The new scope has a name */
	SymDef (SVal, CurrentPC (), IsZPSeg ());
	NextTok ();
    }
    SymEnterLevel ();
}



static void DoReloc (void)
/* Enter relocatable mode */
{
    RelocMode = 1;
}



static void DoRepeat (void)
/* Repeat some instruction block */
{
    ErrorSkip (ERR_NOT_IMPLEMENTED);
}



static void DoRes (void)
/* Reserve some number of storage bytes */
{
    long Count;
    long Val;

    Count = ConstExpression ();
    if (Count > 0xFFFF || Count < 0) {
	ErrorSkip (ERR_RANGE);
	return;
    }
    if (Tok == TOK_COMMA) {
	NextTok ();
	Val = ConstExpression ();
	/* We need a byte value here */
	if (!IsByteRange (Val)) {
       	    ErrorSkip (ERR_RANGE);
	    return;
	}

	/* Emit constant values */
	while (Count--) {
	    Emit0 ((unsigned char) Val);
	}

    } else {
	/* Emit fill fragments */
	EmitFill (Count);
    }
}



static void DoROData (void)
/* Switch to the r/o data segment */
{
    UseRODataSeg ();
}



static void DoSegment (void)
/* Switch to another segment */
{
    static const char* AttrTab [] = {
	"ZEROPAGE", "DIRECT",
	"ABSOLUTE",
	"FAR", "LONG"
    };
    char Name [sizeof (SVal)];
    int SegType;

    if (StringExpression () == 0) {
	ErrorSkip (ERR_STRCON_EXPECTED);
    } else {

	/* Save the name of the segment and skip it */
	strcpy (Name, SVal);
	NextTok ();

	/* Check for an optional segment attribute */
	SegType = SEGTYPE_DEFAULT;
	if (Tok == TOK_COMMA) {
	    NextTok ();
	    if (Tok != TOK_IDENT) {
	     	ErrorSkip (ERR_IDENT_EXPECTED);
	    } else {
		int Attr = GetSubKey (AttrTab, sizeof (AttrTab) / sizeof (AttrTab [0]));
		switch (Attr) {

		    case 0:
		    case 1:
			/* Zeropage */
		    	SegType = SEGTYPE_ZP;
			break;

		    case 2:
			/* Absolute */
		    	SegType = SEGTYPE_ABS;
			break;

    		    case 3:
		    case 4:
			/* Far */
    		    	SegType = SEGTYPE_FAR;
			break;

		    default:
	     	        Error (ERR_ILLEGAL_SEG_ATTR);
	     	}
		NextTok ();
	    }
	}

	/* Set the segment */
     	UseSeg (Name, SegType);
    }
}



static void DoSmart (void)
/* Smart mode on/off */
{
    SetBoolOption (&SmartMode);
}



static void DoSunPlus (void)
/* Switch to the SUNPLUS CPU */
{
    SetCPU (CPU_SUNPLUS);
}



static void DoUnexpected (void)
/* Got an unexpected keyword */
{
    Error (ERR_UNEXPECTED, Keyword);
    SkipUntilSep ();
}



static void DoWord (void)
/* Define words */
{
    while (1) {
       	EmitWord (Expression ());
	if (Tok != TOK_COMMA) {
	    break;
	} else {
	    NextTok ();
	}
    }
}



static void DoZeropage (void)
/* Switch to the zeropage segment */
{
    UseZeropageSeg ();
}



/*****************************************************************************/
/*	      		   	  Table data				     */
/*****************************************************************************/



/* Control commands flags */
enum {
    ccNone	= 0x0000,		/* No special flags */
    ccKeepToken	= 0x0001		/* Do not skip the current token */
};

/* Control command table */
struct CtrlDesc_ {
    unsigned   	Flags; 			/* Flags for this directive */
    void       	(*Handler) (void);	/* Command handler */
};
typedef struct CtrlDesc_ CtrlDesc;

#define PSEUDO_COUNT 	(sizeof (CtrlCmdTab) / sizeof (CtrlCmdTab [0]))
static CtrlDesc CtrlCmdTab [] = {
    { ccNone,		DoA16		},
    { ccNone,       	DoA8		},
    { ccNone,        	DoAddr 	       	},     	/* .ADDR */
    { ccNone,       	DoAlign		},
    { ccNone,       	DoASCIIZ	},
    { ccNone,       	DoAutoImport	},
    { ccNone,        	DoUnexpected	},	/* .BLANK */
    { ccNone,       	DoBss		},
    { ccNone,       	DoByte		},
    { ccNone,       	DoCase		},
    { ccNone,		DoCode		},
    { ccNone,		DoUnexpected	},	/* .CONST */
    { ccNone,		DoUnexpected	},	/* .CPU */
    { ccNone,		DoData		},
    { ccNone,		DoDByt		},
    { ccNone,        	DoDebugInfo	},
    { ccNone,		DoDefine	},
    { ccNone,		DoUnexpected	},	/* .DEFINED */
    { ccNone,		DoDWord		},
    { ccKeepToken,	DoConditionals	},	/* .ELSE */
    { ccKeepToken,	DoConditionals	},	/* .ELSEIF */
    { ccNone,		DoEnd	  	},
    { ccKeepToken,    	DoConditionals	},	/* .ENDIF */
    { ccNone,     	DoUnexpected	},	/* .ENDMACRO */
    { ccNone,		DoEndProc	},
    { ccNone,		DoUnexpected	},	/* .ENDREPEAT */
    { ccNone,		DoError	  	},
    { ccNone,		DoExitMacro	},
    { ccNone,		DoExport  	},
    { ccNone,       	DoExportZP	},
    { ccNone,        	DoFarAddr	},
    { ccNone,		DoFeature	},
    { ccNone,		DoFileOpt	},
    { ccNone,		DoGlobal	},
    { ccNone,		DoGlobalZP	},
    { ccNone,		DoI16	  	},
    { ccNone,		DoI8	  	},
    { ccKeepToken,	DoConditionals	},	/* .IF */
    { ccKeepToken,	DoConditionals	},	/* .IFBLANK */
    { ccKeepToken,	DoConditionals	},	/* .IFCONST */
    { ccKeepToken,	DoConditionals	},	/* .IFDEF */
    { ccKeepToken,     	DoConditionals	},	/* .IFNBLANK */
    { ccKeepToken,	DoConditionals	},	/* .IFNCONST */
    { ccKeepToken,	DoConditionals	},	/* .IFNDEF */
    { ccKeepToken,	DoConditionals	},	/* .IFNREF */
    { ccKeepToken,	DoConditionals	},	/* .IFP02 */
    { ccKeepToken,	DoConditionals	},	/* .IFP816 */
    { ccKeepToken,	DoConditionals	},	/* .IFPC02 */
    { ccKeepToken,	DoConditionals	},	/* .IFREF */
    { ccNone,		DoImport  	},
    { ccNone,		DoImportZP	},
    { ccNone,		DoIncBin	},
    { ccNone,      	DoInclude	},
    { ccNone,		DoLineCont	},
    { ccNone,		DoList		},
    { ccNone,  	       	DoListBytes	},
    { ccNone,		DoUnexpected	},	/* .LOCAL */
    { ccNone,		DoLocalChar	},
    { ccNone,		DoMacPack	},
    { ccNone,		DoMacro		},
    { ccNone,  	       	DoUnexpected	},	/* .MATCH */
    { ccNone,  	       	DoMid		},
    { ccNone,		DoNull		},
    { ccNone,		DoOrg		},
    { ccNone,		DoOut		},
    { ccNone,		DoP02		},
    { ccNone,		DoP816		},
    { ccNone,  	       	DoPageLength	},
    { ccNone,       	DoUnexpected   	},	/* .PARAMCOUNT */
    { ccNone,		DoPC02		},
    { ccNone,		DoProc		},
    { ccNone,    	DoUnexpected   	},	/* .REFERENCED */
    { ccNone,		DoReloc		},
    { ccNone,		DoRepeat	},
    { ccNone,		DoRes		},
    { ccNone,		DoROData	},
    { ccNone,       	DoSegment	},
    { ccNone,        	DoSmart		},
    { ccNone,          	DoUnexpected	},	/* .STRING */
    { ccNone,		DoSunPlus	},
    { ccNone,       	DoWord		},
    { ccNone,  	       	DoUnexpected	},	/* .XMATCH */
    { ccNone,       	DoZeropage	},
};



/*****************************************************************************/
/*     	       	    		     Code				     */
/*****************************************************************************/



int TokIsPseudo (unsigned Tok)
/* Return true if the given token is a pseudo instruction token */
{
    return (Tok >= TOK_FIRSTPSEUDO && Tok <= TOK_LASTPSEUDO);
}



void HandlePseudo (void)
/* Handle a pseudo instruction */
{
    CtrlDesc* D;

    /* Calculate the index into the table */
    unsigned Index = Tok - TOK_FIRSTPSEUDO;

    /* Safety check */
    if (PSEUDO_COUNT != (TOK_LASTPSEUDO - TOK_FIRSTPSEUDO + 1)) {
	Internal ("Pseudo mismatch: PSEUDO_COUNT = %u, actual count = %u\n",
		  PSEUDO_COUNT, TOK_LASTPSEUDO - TOK_FIRSTPSEUDO + 1);
    }
    CHECK (Index < PSEUDO_COUNT);

    /* Get the pseudo intruction descriptor */
    D = &CtrlCmdTab [Index];

    /* Remember the instruction, then skip it if needed */
    if ((D->Flags & ccKeepToken) == 0) {
    	strcpy (Keyword+1, SVal);
     	NextTok ();
    }

    /* Call the handler */
    D->Handler ();
}



