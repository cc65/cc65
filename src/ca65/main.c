/*****************************************************************************/
/*                                                                           */
/*                                  main.c                                   */
/*                                                                           */
/*                 Main program for the ca65 macroassembler                  */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2013, Ullrich von Bassewitz                                      */
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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* common */
#include "addrsize.h"
#include "chartype.h"
#include "cmdline.h"
#include "debugflag.h"
#include "mmodel.h"
#include "print.h"
#include "scopedefs.h"
#include "strbuf.h"
#include "target.h"
#include "tgttrans.h"
#include "version.h"

/* ca65 */
#include "abend.h"
#include "asserts.h"
#include "dbginfo.h"
#include "error.h"
#include "expr.h"
#include "feature.h"
#include "filetab.h"
#include "global.h"
#include "incpath.h"
#include "instr.h"
#include "istack.h"
#include "lineinfo.h"
#include "listing.h"
#include "macro.h"
#include "nexttok.h"
#include "objfile.h"
#include "options.h"
#include "pseudo.h"
#include "scanner.h"
#include "segment.h"
#include "sizeof.h"
#include "span.h"
#include "spool.h"
#include "symbol.h"
#include "symtab.h"
#include "ulabel.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static void Usage (void)
/* Print usage information and exit */
{
    printf ("Usage: %s [options] file\n"
            "Short options:\n"
            "  -D name[=value]\t\tDefine a symbol\n"
            "  -I dir\t\t\tSet an include directory search path\n"
            "  -U\t\t\t\tMark unresolved symbols as import\n"
            "  -V\t\t\t\tPrint the assembler version\n"
            "  -W n\t\t\t\tSet warning level n\n"
            "  -d\t\t\t\tDebug mode\n"
            "  -g\t\t\t\tAdd debug info to object file\n"
            "  -h\t\t\t\tHelp (this text)\n"
            "  -i\t\t\t\tIgnore case of symbols\n"
            "  -l name\t\t\tCreate a listing file if assembly was ok\n"
            "  -mm model\t\t\tSet the memory model\n"
            "  -o name\t\t\tName the output file\n"
            "  -s\t\t\t\tEnable smart mode\n"
            "  -t sys\t\t\tSet the target system\n"
            "  -v\t\t\t\tIncrease verbosity\n"
            "\n"
            "Long options:\n"
            "  --auto-import\t\t\tMark unresolved symbols as import\n"
            "  --bin-include-dir dir\t\tSet a search path for binary includes\n"
            "  --cpu type\t\t\tSet cpu type\n"
            "  --create-dep name\t\tCreate a make dependency file\n"
            "  --create-full-dep name\tCreate a full make dependency file\n"
            "  --debug\t\t\tDebug mode\n"
            "  --debug-info\t\t\tAdd debug info to object file\n"
            "  --feature name\t\tSet an emulation feature\n"
            "  --help\t\t\tHelp (this text)\n"
            "  --ignore-case\t\t\tIgnore case of symbols\n"
            "  --include-dir dir\t\tSet an include directory search path\n"
            "  --large-alignment\t\tDon't warn about large alignments\n"
            "  --listing name\t\tCreate a listing file if assembly was ok\n"
            "  --list-bytes n\t\tMaximum number of bytes per listing line\n"
            "  --memory-model model\t\tSet the memory model\n"
            "  --pagelength n\t\tSet the page length for the listing\n"
            "  --relax-checks\t\tRelax some checks (see docs)\n"
            "  --smart\t\t\tEnable smart mode\n"
            "  --target sys\t\t\tSet the target system\n"
            "  --verbose\t\t\tIncrease verbosity\n"
            "  --version\t\t\tPrint the assembler version\n",
            ProgName);
}



static void SetOptions (void)
/* Set the option for the translator */
{
    StrBuf Buf = STATIC_STRBUF_INITIALIZER;

    /* Set the translator */
    SB_Printf (&Buf, "ca65 V%s", GetVersionAsString ());
    OptTranslator (&Buf);

    /* Set date and time */
    OptDateTime ((unsigned long) time(0));

    /* Release memory for the string */
    SB_Done (&Buf);
}



static void NewSymbol (const char* SymName, long Val)
/* Define a symbol with a fixed numeric value in the current scope */
{
    ExprNode* Expr;
    SymEntry* Sym;

    /* Convert the name to a string buffer */
    StrBuf SymBuf = STATIC_STRBUF_INITIALIZER;
    SB_CopyStr (&SymBuf, SymName);

    /* Search for the symbol, allocate a new one if it doesn't exist */
    Sym = SymFind (CurrentScope, &SymBuf, SYM_ALLOC_NEW);

    /* Check if have already a symbol with this name */
    if (SymIsDef (Sym)) {
        AbEnd ("'%s' is already defined", SymName);
    }

    /* Generate an expression for the symbol */
    Expr = GenLiteralExpr (Val);

    /* Mark the symbol as defined */
    SymDef (Sym, Expr, ADDR_SIZE_DEFAULT, SF_NONE);

    /* Free string buffer memory */
    SB_Done (&SymBuf);
}



static void CBMSystem (const char* Sys)
/* Define a CBM system */
{
    NewSymbol ("__CBM__", 1);
    NewSymbol (Sys, 1);
}



static void SetSys (const char* Sys)
/* Define a target system */
{
    switch (Target = FindTarget (Sys)) {

        case TGT_NONE:
            break;

        case TGT_MODULE:
            AbEnd ("Cannot use 'module' as a target for the assembler");
            break;

        case TGT_ATARI2600:
            NewSymbol ("__ATARI2600__", 1);
            break;

        case TGT_ATARI5200:
            NewSymbol ("__ATARI5200__", 1);
            break;

        case TGT_ATARI7800:
            NewSymbol ("__ATARI7800__", 1);
            break;

        case TGT_ATARI:
            NewSymbol ("__ATARI__", 1);
            break;

        case TGT_ATARIXL:
            NewSymbol ("__ATARI__", 1);
            NewSymbol ("__ATARIXL__", 1);
            break;

        case TGT_C16:
            CBMSystem ("__C16__");
            break;

        case TGT_C64:
            CBMSystem ("__C64__");
            break;

        case TGT_C65:
            CBMSystem ("__C65__");
            break;

        case TGT_VIC20:
            CBMSystem ("__VIC20__");
            break;

        case TGT_C128:
            CBMSystem ("__C128__");
            break;

        case TGT_PLUS4:
            CBMSystem ("__C16__");
            NewSymbol ("__PLUS4__", 1);
            break;

        case TGT_CBM510:
            CBMSystem ("__CBM510__");
            break;

        case TGT_CBM610:
            CBMSystem ("__CBM610__");
            break;

        case TGT_PET:
            CBMSystem ("__PET__");
            break;

        case TGT_BBC:
            NewSymbol ("__BBC__", 1);
            break;

        case TGT_APPLE2:
            NewSymbol ("__APPLE2__", 1);
            break;

        case TGT_APPLE2ENH:
            NewSymbol ("__APPLE2__", 1);
            NewSymbol ("__APPLE2ENH__", 1);
            break;

        case TGT_GAMATE:
            NewSymbol ("__GAMATE__", 1);
            break;

        case TGT_GEOS_CBM:
            /* Do not handle as a CBM system */
            NewSymbol ("__GEOS__", 1);
            NewSymbol ("__GEOS_CBM__", 1);
            break;

        case TGT_CREATIVISION:
            NewSymbol ("__CREATIVISION__", 1);
            break;

        case TGT_GEOS_APPLE:
            NewSymbol ("__GEOS__", 1);
            NewSymbol ("__GEOS_APPLE__", 1);
            break;

        case TGT_LUNIX:
            NewSymbol ("__LUNIX__", 1);
            break;

        case TGT_ATMOS:
            NewSymbol ("__ATMOS__", 1);
            break;

        case TGT_TELESTRAT:
             NewSymbol ("__TELESTRAT__", 1);
             break;

        case TGT_NES:
            NewSymbol ("__NES__", 1);
            break;

        case TGT_SUPERVISION:
            NewSymbol ("__SUPERVISION__", 1);
            break;

        case TGT_LYNX:
            NewSymbol ("__LYNX__", 1);
            break;

        case TGT_SIM6502:
            NewSymbol ("__SIM6502__", 1);
            break;

        case TGT_SIM65C02:
            NewSymbol ("__SIM65C02__", 1);
            break;

        case TGT_OSIC1P:
            NewSymbol ("__OSIC1P__", 1);
            break;

        case TGT_PCENGINE:
            NewSymbol ("__PCE__", 1);
            break;

        case TGT_CX16:
            CBMSystem ("__CX16__");
            break;

        case TGT_SYM1:
            NewSymbol ("__SYM1__", 1);
            break;

        case TGT_KIM1:
            NewSymbol ("__KIM1__", 1);
            break;

        case TGT_RP6502:
            NewSymbol ("__RP6502__", 1);
            break;

        default:
            AbEnd ("Invalid target name: '%s'", Sys);

    }

    /* Initialize the translation tables for the target system */
    TgtTranslateInit ();
}



static void FileNameOption (const char* Opt, const char* Arg, StrBuf* Name)
/* Handle an option that remembers a file name for later */
{
    /* Cannot have the option twice */
    if (SB_NotEmpty (Name)) {
        AbEnd ("Cannot use option '%s' twice", Opt);
    }
    /* Remember the file name for later */
    SB_CopyStr (Name, Arg);
    SB_Terminate (Name);
}



static void DefineSymbol (const char* Def)
/* Define a symbol from the command line */
{
    const char* P;
    long Val;
    StrBuf SymName = AUTO_STRBUF_INITIALIZER;


    /* The symbol must start with a character or underline */
    if (!IsIdStart (Def [0])) {
        InvDef (Def);
    }
    P = Def;

    /* Copy the symbol, checking the rest */
    while (IsIdChar (*P)) {
        SB_AppendChar (&SymName, *P++);
    }
    SB_Terminate (&SymName);

    /* Do we have a value given? */
    if (*P != '=') {
        if (*P != '\0') {
            InvDef (Def);
        }
        Val = 0;
    } else {
        /* We have a value */
        ++P;
        if (*P == '$') {
            ++P;
            if (sscanf (P, "%lx", &Val) != 1) {
                InvDef (Def);
            }
        } else {
            if (sscanf (P, "%li", &Val) != 1) {
                InvDef (Def);
            }
        }
    }

    /* Define the new symbol */
    NewSymbol (SB_GetConstBuf (&SymName), Val);

    /* Release string memory */
    SB_Done (&SymName);
}



static void OptAutoImport (const char* Opt attribute ((unused)),
                           const char* Arg attribute ((unused)))
/* Mark unresolved symbols as imported */
{
    AutoImport = 1;
}



static void OptBinIncludeDir (const char* Opt attribute ((unused)), const char* Arg)
/* Add an include search path for binaries */
{
    AddSearchPath (BinSearchPath, Arg);
}



static void OptCPU (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --cpu option */
{
    cpu_t CPU = FindCPU (Arg);
    if (CPU == CPU_UNKNOWN) {
        AbEnd ("Invalid CPU: '%s'", Arg);
    } else {
        SetCPU (CPU);
    }
}



static void OptCreateDep (const char* Opt, const char* Arg)
/* Handle the --create-dep option */
{
    FileNameOption (Opt, Arg, &DepName);
}



static void OptCreateFullDep (const char* Opt attribute ((unused)),
                              const char* Arg)
/* Handle the --create-full-dep option */
{
    FileNameOption (Opt, Arg, &FullDepName);
}



static void OptDebug (const char* Opt attribute ((unused)),
                      const char* Arg attribute ((unused)))
/* Compiler debug mode */
{
    ++Debug;
}



static void OptDebugInfo (const char* Opt attribute ((unused)),
                          const char* Arg attribute ((unused)))
/* Add debug info to the object file */
{
    DbgSyms = 1;
}



static void OptFeature (const char* Opt attribute ((unused)), const char* Arg)
/* Set an emulation feature */
{
    /* Make a string buffer from Arg and use it to find the feature. */
    StrBuf StrFeature;
    feature_t Feature = FindFeature (SB_InitFromString (&StrFeature, Arg));

    /* Enable the feature, check for errors */
    if (Feature == FEAT_UNKNOWN) {
        AbEnd ("Illegal emulation feature: '%s'", Arg);
    } else {
        SetFeature (Feature, 1);
    }
}



static void OptHelp (const char* Opt attribute ((unused)),
                     const char* Arg attribute ((unused)))
/* Print usage information and exit */
{
    Usage ();
    exit (EXIT_SUCCESS);
}



static void OptIgnoreCase (const char* Opt attribute ((unused)),
                           const char* Arg attribute ((unused)))
/* Ignore case on symbols */
{
    IgnoreCase = 1;
}



static void OptIncludeDir (const char* Opt attribute ((unused)), const char* Arg)
/* Add an include search path */
{
    AddSearchPath (IncSearchPath, Arg);
}



static void OptLargeAlignment (const char* Opt attribute ((unused)),
                               const char* Arg attribute ((unused)))
/* Don't warn about large alignments */
{
    LargeAlignment = 1;
}



static void OptListBytes (const char* Opt, const char* Arg)
/* Set the maximum number of bytes per listing line */
{
    unsigned Num;
    char     Check;

    /* Convert the argument to a number */
    if (sscanf (Arg, "%u%c", &Num, &Check) != 1) {
        InvArg (Opt, Arg);
    }

    /* Check the bounds */
    if (Num != 0 && (Num < MIN_LIST_BYTES || Num > MAX_LIST_BYTES)) {
        AbEnd ("Argument for option '%s' is out of range", Opt);
    }

    /* Use the value */
    SetListBytes (Num);
}



static void OptListing (const char* Opt, const char* Arg)
/* Create a listing file */
{
    /* Since the meaning of -l and --listing has changed, print an error if
    ** the filename is empty or begins with the option char.
    */
    if (Arg == 0 || *Arg == '\0' || *Arg == '-') {
        Fatal ("The meaning of '%s' has changed. It does now "
               "expect a file name as argument.", Opt);
    }

    /* Get the file name */
    FileNameOption (Opt, Arg, &ListingName);
}



static void OptMemoryModel (const char* Opt, const char* Arg)
/* Set the memory model */
{
    mmodel_t M;

    /* Check the current memory model */
    if (MemoryModel != MMODEL_UNKNOWN) {
        AbEnd ("Cannot use option '%s' twice", Opt);
    }

    /* Translate the memory model name and check it */
    M = FindMemoryModel (Arg);
    if (M == MMODEL_UNKNOWN) {
        AbEnd ("Unknown memory model: %s", Arg);
    } else if (M == MMODEL_HUGE) {
        AbEnd ("Unsupported memory model: %s", Arg);
    }

    /* Set the memory model */
    SetMemoryModel (M);
}



static void OptPageLength (const char* Opt attribute ((unused)), const char* Arg)
/* Handle the --pagelength option */
{
    int Len = atoi (Arg);
    if (Len != -1 && (Len < MIN_PAGE_LEN || Len > MAX_PAGE_LEN)) {
        AbEnd ("Invalid page length: %d", Len);
    }
    PageLength = Len;
}



static void OptRelaxChecks (const char* Opt attribute ((unused)),
                            const char* Arg attribute ((unused)))
/* Handle the --relax-checks options */
{
    RelaxChecks = 1;
}



static void OptSmart (const char* Opt attribute ((unused)),
                      const char* Arg attribute ((unused)))
/* Handle the -s/--smart options */
{
    SmartMode = 1;
}



static void OptTarget (const char* Opt attribute ((unused)), const char* Arg)
/* Set the target system */
{
    SetSys (Arg);
}



static void OptVerbose (const char* Opt attribute ((unused)),
                        const char* Arg attribute ((unused)))
/* Increase verbosity */
{
    ++Verbosity;
}



static void OptVersion (const char* Opt attribute ((unused)),
                        const char* Arg attribute ((unused)))
/* Print the assembler version */
{
    fprintf (stderr, "%s V%s\n", ProgName, GetVersionAsString ());
    exit(EXIT_SUCCESS);
}



static void OptWarningsAsErrors (const char* Opt attribute ((unused)),
                                 const char* Arg attribute ((unused)))
/* Generate an error if any warnings occur */
{
    WarningsAsErrors = 1;
}



static void DoPCAssign (void)
/* Start absolute code */
{
    long PC = ConstExpression ();
    if (PC < 0 || PC > 0xFFFFFF) {
        Error ("Range error");
    } else {
        EnterAbsoluteMode (PC);
    }
}



static void OneLine (void)
/* Assemble one line */
{
    Segment*      Seg   = 0;
    unsigned long PC    = 0;
    SymEntry*     Sym   = 0;
    Macro*        Mac   = 0;
    int           Instr = -1;

    /* Initialize the new listing line if we are actually reading from file
    ** and not from internally pushed input.
    */
    if (!HavePushedInput ()) {
        InitListingLine ();
    }

    /* Single colon means unnamed label */
    if (CurTok.Tok == TOK_COLON) {
        ULabDef ();
        NextTok ();
    }

    /* Handle @-style unnamed labels */
    if (CurTok.Tok == TOK_ULABEL) {
        if (CurTok.IVal != 0) {
            Error ("Invalid unnamed label definition");
        }
        ULabDef ();
        NextTok ();

        /* Skip the colon. If NoColonLabels is enabled, allow labels without
        ** a colon if there is no whitespace before the identifier.
        */
        if (CurTok.Tok == TOK_COLON) {
            NextTok ();
        } else if (CurTok.WS || !NoColonLabels) {
            Error ("':' expected");
        }
    }

    /* If the first token on the line is an identifier, check for a macro or
    ** an instruction.
    */
    if (CurTok.Tok == TOK_IDENT) {
        if (UbiquitousIdents) {
            /* Macros CAN be instructions, so check for them first */
            Mac = FindMacro (&CurTok.SVal);
            if (Mac == 0) {
                Instr = FindInstruction (&CurTok.SVal);
            }
        } else {
            /* Macros and symbols may NOT use the names of instructions */
            Instr = FindInstruction (&CurTok.SVal);
            if (Instr < 0) {
                Mac = FindMacro (&CurTok.SVal);
            }
        }
    }

    /* Handle an identifier. This may be a cheap local symbol, or a fully
    ** scoped identifier which may start with a namespace token (for global
    ** namespace)
    */
    if (CurTok.Tok == TOK_LOCAL_IDENT ||
        CurTok.Tok == TOK_NAMESPACE   ||
        (CurTok.Tok == TOK_IDENT && Instr < 0 && Mac == 0)) {

        /* Did we have whitespace before the ident? */
        int HadWS = CurTok.WS;

        /* Generate the symbol table entry, then skip the name */
        Sym = ParseAnySymName (SYM_ALLOC_NEW);

        /* If a colon follows, this is a label definition. If there
        ** is no colon, it's an assignment.
        */
        if (CurTok.Tok == TOK_EQ || CurTok.Tok == TOK_ASSIGN) {

            /* Determine the symbol flags from the assignment token */
            unsigned Flags = (CurTok.Tok == TOK_ASSIGN)? SF_LABEL : SF_NONE;

            /* Skip the '=' */
            NextTok ();

            /* Define the symbol with the expression following the '=' */
            SymDef (Sym, Expression(), ADDR_SIZE_DEFAULT, Flags);

            /* Don't allow anything after a symbol definition */
            ConsumeSep ();
            return;

        } else if (CurTok.Tok == TOK_SET) {

            ExprNode* Expr;

            /* .SET defines variables (= redefinable symbols) */
            NextTok ();

            /* Read the assignment expression, which must be constant */
            Expr = GenLiteralExpr (ConstExpression ());

            /* Define the symbol with the constant expression following
            ** the '='
            */
            SymDef (Sym, Expr, ADDR_SIZE_DEFAULT, SF_VAR);

            /* Don't allow anything after a symbol definition */
            ConsumeSep ();
            return;

        } else {

            /* A label. Remember the current segment, so we can later
            ** determine the size of the data stored under the label.
            */
            Seg = ActiveSeg;
            PC  = GetPC ();

            /* Define the label */
            SymDef (Sym, GenCurrentPC (), ADDR_SIZE_DEFAULT, SF_LABEL);

            /* Skip the colon. If NoColonLabels is enabled, allow labels
            ** without a colon if there is no whitespace before the
            ** identifier.
            */
            if (CurTok.Tok != TOK_COLON) {
                if (HadWS || !NoColonLabels) {
                    Error ("':' expected");
                    /* Try some smart error recovery */
                    if (CurTok.Tok == TOK_NAMESPACE) {
                        NextTok ();
                    }
                }
            } else {
                /* Skip the colon */
                NextTok ();
            }

            /* If we come here, a new identifier may be waiting, which may
            ** be a macro or instruction.
            */
            if (CurTok.Tok == TOK_IDENT) {
                if (UbiquitousIdents) {
                    /* Macros CAN be instructions, so check for them first */
                    Mac = FindMacro (&CurTok.SVal);
                    if (Mac == 0) {
                        Instr = FindInstruction (&CurTok.SVal);
                    }
                } else {
                    /* Macros and symbols may NOT use the names of instructions */
                    Instr = FindInstruction (&CurTok.SVal);
                    if (Instr < 0) {
                        Mac = FindMacro (&CurTok.SVal);
                    }
                }
            }
        }
    }

    /* We've handled a possible label, now handle the remainder of the line */
    if (CurTok.Tok >= TOK_FIRSTPSEUDO && CurTok.Tok <= TOK_LASTPSEUDO) {
        /* A control command */
        HandlePseudo ();
    } else if (Mac != 0) {
        /* A macro expansion */
        MacExpandStart (Mac);
    } else if (Instr >= 0) {
        /* A mnemonic - assemble one instruction */
        HandleInstruction (Instr);
    } else if (PCAssignment && (CurTok.Tok == TOK_STAR || CurTok.Tok == TOK_PC)) {
        NextTok ();
        if (CurTok.Tok != TOK_EQ) {
            Error ("'=' expected");
            SkipUntilSep ();
        } else {
            /* Skip the equal sign */
            NextTok ();
            /* Enter absolute mode */
            DoPCAssign ();
        }
    }

    /* If we have defined a label, remember its size. Sym is also set by
    ** a symbol assignment, but in this case Done is false, so we don't
    ** come here.
    */
    if (Sym) {
        unsigned long Size;
        if (Seg == ActiveSeg) {
            /* Same segment */
            Size = GetPC () - PC;
        } else {
            /* The line has switched the segment */
            Size = 0;
        }
        /* Suppress .size Symbol if this Symbol already has a multiply-defined error,
        ** as it will only create its own additional unnecessary error.
        */
        if ((Sym->Flags & SF_MULTDEF) == 0) {
            DefSizeOfSymbol (Sym, Size);
        }
    }

    /* Line separator must come here */
    ConsumeSep ();
}



static void Assemble (void)
/* Start the ball rolling ... */
{
    /* Prime the pump */
    NextTok ();

    /* Assemble lines until end of file */
    while (CurTok.Tok != TOK_EOF) {
        OneLine ();
    }
}



static void CreateObjFile (void)
/* Create the object file */
{
    /* Open the object, write the header */
    ObjOpen ();

    /* Write the object file options */
    WriteOptions ();

    /* Write the list of input files */
    WriteFiles ();

    /* Write the segment data to the file */
    WriteSegments ();

    /* Write the import list */
    WriteImports ();

    /* Write the export list */
    WriteExports ();

    /* Write debug symbols if requested */
    WriteDbgSyms ();

    /* Write the scopes if requested */
    WriteScopes ();

    /* Write line infos if requested */
    WriteLineInfos ();

    /* Write the string pool */
    WriteStrPool ();

    /* Write the assertions */
    WriteAssertions ();

    /* Write the spans */
    WriteSpans ();

    /* Write an updated header and close the file */
    ObjClose ();
}



int main (int argc, char* argv [])
/* Assembler main program */
{
    /* Program long options */
    static const LongOpt OptTab[] = {
        { "--auto-import",         0,      OptAutoImport           },
        { "--bin-include-dir",     1,      OptBinIncludeDir        },
        { "--cpu",                 1,      OptCPU                  },
        { "--create-dep",          1,      OptCreateDep            },
        { "--create-full-dep",     1,      OptCreateFullDep        },
        { "--debug",               0,      OptDebug                },
        { "--debug-info",          0,      OptDebugInfo            },
        { "--feature",             1,      OptFeature              },
        { "--help",                0,      OptHelp                 },
        { "--ignore-case",         0,      OptIgnoreCase           },
        { "--include-dir",         1,      OptIncludeDir           },
        { "--large-alignment",     0,      OptLargeAlignment       },
        { "--list-bytes",          1,      OptListBytes            },
        { "--listing",             1,      OptListing              },
        { "--memory-model",        1,      OptMemoryModel          },
        { "--pagelength",          1,      OptPageLength           },
        { "--relax-checks",        0,      OptRelaxChecks          },
        { "--smart",               0,      OptSmart                },
        { "--target",              1,      OptTarget               },
        { "--verbose",             0,      OptVerbose              },
        { "--version",             0,      OptVersion              },
        { "--warnings-as-errors",  0,      OptWarningsAsErrors     },
    };

    /* Name of the global name space */
    static const StrBuf GlobalNameSpace = STATIC_STRBUF_INITIALIZER;

    unsigned I;

    /* Initialize the cmdline module */
    InitCmdLine (&argc, &argv, "ca65");

    /* Initialize the string pool */
    InitStrPool ();

    /* Initialize the include search paths */
    InitIncludePaths ();

    /* Create the predefined segments */
    SegInit ();

    /* Enter the base lexical level. We must do that here, since we may
    ** define symbols using -D.
    */
    SymEnterLevel (&GlobalNameSpace, SCOPE_FILE, ADDR_SIZE_DEFAULT, 0);

    /* Initialize the line infos. Must be done here, since we need line infos
    ** for symbol definitions.
    */
    InitLineInfo ();

    /* Check the parameters */
    I = 1;
    while (I < ArgCount) {

        /* Get the argument */
        const char* Arg = ArgVec [I];

        /* Check for an option */
        if (Arg[0] == '-') {
            switch (Arg[1]) {

                case '-':
                    LongOption (&I, OptTab, sizeof(OptTab)/sizeof(OptTab[0]));
                    break;

                case 'd':
                    OptDebug (Arg, 0);
                    break;

                case 'g':
                    OptDebugInfo (Arg, 0);
                    break;

                case 'h':
                    OptHelp (Arg, 0);
                    break;

                case 'i':
                    OptIgnoreCase (Arg, 0);
                    break;

                case 'l':
                    OptListing (Arg, GetArg (&I, 2));
                    break;

                case 'm':
                    if (Arg[2] == 'm') {
                        OptMemoryModel (Arg, GetArg (&I, 3));
                    } else {
                        UnknownOption (Arg);
                    }
                    break;

                case 'o':
                    OutFile = GetArg (&I, 2);
                    break;

                case 's':
                    OptSmart (Arg, 0);
                    break;

                case 't':
                    OptTarget (Arg, GetArg (&I, 2));
                    break;

                case 'v':
                    OptVerbose (Arg, 0);
                    break;

                case 'D':
                    DefineSymbol (GetArg (&I, 2));
                    break;

                case 'I':
                    OptIncludeDir (Arg, GetArg (&I, 2));
                    break;

                case 'U':
                    OptAutoImport (Arg, 0);
                    break;

                case 'V':
                    OptVersion (Arg, 0);
                    break;

                case 'W':
                    WarnLevel = atoi (GetArg (&I, 2));
                    break;

                default:
                    UnknownOption (Arg);
                    break;

            }
        } else {
            /* Filename. Check if we already had one */
            if (InFile) {
                fprintf (stderr, "%s: Don't know what to do with '%s'\n",
                         ProgName, Arg);
                exit (EXIT_FAILURE);
            } else {
                InFile = Arg;
            }
        }

        /* Next argument */
        ++I;
    }

    /* Do we have an input file? */
    if (InFile == 0) {
        fprintf (stderr, "%s: No input files\n", ProgName);
        exit (EXIT_FAILURE);
    }

    /* Add the default include search paths. */
    FinishIncludePaths ();

    /* If no CPU given, use the default CPU for the target */
    if (GetCPU () == CPU_UNKNOWN) {
        if (Target != TGT_UNKNOWN) {
            SetCPU (GetTargetProperties (Target)->DefaultCPU);
        } else {
            SetCPU (CPU_6502);
        }
    }

    /* If no memory model was given, use the default */
    if (MemoryModel == MMODEL_UNKNOWN) {
        SetMemoryModel (MMODEL_NEAR);
    }

    /* Set the default segment sizes according to the memory model */
    SetSegmentSizes ();

    /* Initialize the scanner, open the input file */
    InitScanner (InFile);

    /* Define the default options */
    SetOptions ();

    /* Assemble the input */
    Assemble ();

    /* If we didn't have any errors, check the pseudo insn stacks */
    if (ErrorCount == 0) {
        CheckPseudo ();
    }

    /* If we didn't have any errors, check and cleanup the unnamed labels */
    if (ErrorCount == 0) {
        ULabDone ();
    }

    /* If we didn't have any errors, check the symbol table */
    if (ErrorCount == 0) {
        SymCheck ();
    }

    /* If we didn't have any errors, check the hll debug symbols */
    if (ErrorCount == 0) {
        DbgInfoCheck ();
    }

    /* If we didn't have any errors, close the file scope lexical level */
    if (ErrorCount == 0) {
        SymLeaveLevel ();
    }

    /* If we didn't have any errors, check and resolve the segment data */
    if (ErrorCount == 0) {
        SegDone ();
    }

    /* If we didn't have any errors, check the assertions */
    if (ErrorCount == 0) {
        CheckAssertions ();
    }

    /* Dump the data */
    if (Verbosity >= 2) {
        SymDump (stdout);
        SegDump ();
    }

    if (WarningCount > 0 && WarningsAsErrors) {
        Error("Warnings as errors");
    }

    /* If we didn't have an errors, finish off the line infos */
    DoneLineInfo ();

    /* If we didn't have any errors, create the object, listing and
    ** dependency files
    */
    if (ErrorCount == 0) {
        CreateObjFile ();
        if (SB_GetLen (&ListingName) > 0) {
            CreateListing ();
        }
       CreateDependencies ();
    }

    /* Close the input file */
    DoneScanner ();

    /* Return an apropriate exit code */
    return (ErrorCount == 0)? EXIT_SUCCESS : EXIT_FAILURE;
}
