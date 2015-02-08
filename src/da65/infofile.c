/*****************************************************************************/
/*                                                                           */
/*                                infofile.h                                 */
/*                                                                           */
/*                      Disassembler info file handling                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2014, Ullrich von Bassewitz                                      */
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
#include <string.h>
#include <limits.h>
#if defined(_MSC_VER)
/* Microsoft compiler */
#  include <io.h>
#else
/* Anyone else */
#  include <unistd.h>
#endif

/* common */
#include "cpu.h"
#include "xmalloc.h"

/* da65 */
#include "asminc.h"
#include "attrtab.h"
#include "comments.h"
#include "error.h"
#include "global.h"
#include "infofile.h"
#include "labels.h"
#include "opctable.h"
#include "scanner.h"
#include "segment.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static void AddAttr (const char* Name, unsigned* Set, unsigned Attr)
/* Add an attribute to the set and check that it is not given twice */
{
    if (*Set & Attr) {
        /* Attribute is already in the set */
        InfoError ("%s given twice", Name);
    }
    *Set |= Attr;
}



static void AsmIncSection (void)
/* Parse a asminc section */
{
    static const IdentTok LabelDefs[] = {
        {   "COMMENTSTART",     INFOTOK_COMMENTSTART    },
        {   "FILE",             INFOTOK_FILE            },
        {   "IGNOREUNKNOWN",    INFOTOK_IGNOREUNKNOWN   },
    };

    /* Locals - initialize to avoid gcc warnings */
    char* Name = 0;
    int CommentStart = EOF;
    int IgnoreUnknown = -1;

    /* Skip the token */
    InfoNextTok ();

    /* Expect the opening curly brace */
    InfoConsumeLCurly ();

    /* Look for section tokens */
    while (InfoTok != INFOTOK_RCURLY) {

        /* Convert to special token */
        InfoSpecialToken (LabelDefs, ENTRY_COUNT (LabelDefs), "Asminc directive");

        /* Look at the token */
        switch (InfoTok) {

            case INFOTOK_COMMENTSTART:
                InfoNextTok ();
                if (CommentStart != EOF) {
                    InfoError ("Commentstart already given");
                }
                InfoAssureChar ();
                CommentStart = (char) InfoIVal;
                InfoNextTok ();
                break;

            case INFOTOK_FILE:
                InfoNextTok ();
                if (Name) {
                    InfoError ("File name already given");
                }
                InfoAssureStr ();
                if (InfoSVal[0] == '\0') {
                    InfoError ("File name may not be empty");
                }
                Name = xstrdup (InfoSVal);
                InfoNextTok ();
                break;

            case INFOTOK_IGNOREUNKNOWN:
                InfoNextTok ();
                if (IgnoreUnknown != -1) {
                    InfoError ("Ignoreunknown already specified");
                }
                InfoBoolToken ();
                IgnoreUnknown = (InfoTok != INFOTOK_FALSE);
                InfoNextTok ();
                break;

            default:
                Internal ("Unexpected token: %u", InfoTok);
        }

        /* Directive is followed by a semicolon */
        InfoConsumeSemi ();
    }

    /* Check for the necessary data and assume defaults */
    if (Name == 0) {
        InfoError ("File name is missing");
    }
    if (CommentStart == EOF) {
        CommentStart = ';';
    }
    if (IgnoreUnknown == -1) {
        IgnoreUnknown = 0;
    }

    /* Open the file and read the symbol definitions */
    AsmInc (Name, CommentStart, IgnoreUnknown);

    /* Delete the dynamically allocated memory for Name */
    xfree (Name);

    /* Consume the closing brace */
    InfoConsumeRCurly ();
}



static void GlobalSection (void)
/* Parse a global section */
{
    static const IdentTok GlobalDefs[] = {
        {   "ARGUMENTCOL",      INFOTOK_ARGUMENT_COLUMN },
        {   "ARGUMENTCOLUMN",   INFOTOK_ARGUMENT_COLUMN },
        {   "COMMENTCOL",       INFOTOK_COMMENT_COLUMN  },
        {   "COMMENTCOLUMN",    INFOTOK_COMMENT_COLUMN  },
        {   "COMMENTS",         INFOTOK_COMMENTS        },
        {   "CPU",              INFOTOK_CPU             },
        {   "HEXOFFS",          INFOTOK_HEXOFFS         },
        {   "INPUTNAME",        INFOTOK_INPUTNAME       },
        {   "INPUTOFFS",        INFOTOK_INPUTOFFS       },
        {   "INPUTSIZE",        INFOTOK_INPUTSIZE       },
        {   "LABELBREAK",       INFOTOK_LABELBREAK      },
        {   "MNEMONICCOL",      INFOTOK_MNEMONIC_COLUMN },
        {   "MNEMONICCOLUMN",   INFOTOK_MNEMONIC_COLUMN },
        {   "NEWLINEAFTERJMP",  INFOTOK_NL_AFTER_JMP    },
        {   "NEWLINEAFTERRTS",  INFOTOK_NL_AFTER_RTS    },
        {   "OUTPUTNAME",       INFOTOK_OUTPUTNAME      },
        {   "PAGELENGTH",       INFOTOK_PAGELENGTH      },
        {   "STARTADDR",        INFOTOK_STARTADDR       },
        {   "TEXTCOL",          INFOTOK_TEXT_COLUMN     },
        {   "TEXTCOLUMN",       INFOTOK_TEXT_COLUMN     },
    };

    /* Skip the token */
    InfoNextTok ();

    /* Expect the opening curly brace */
    InfoConsumeLCurly ();

    /* Look for section tokens */
    while (InfoTok != INFOTOK_RCURLY) {

        /* Convert to special token */
        InfoSpecialToken (GlobalDefs, ENTRY_COUNT (GlobalDefs), "Global directive");

        /* Look at the token */
        switch (InfoTok) {

            case INFOTOK_ARGUMENT_COLUMN:
                InfoNextTok ();
                InfoAssureInt ();
                InfoRangeCheck (MIN_ACOL, MAX_ACOL);
                ACol = InfoIVal;
                InfoNextTok ();
                break;

            case INFOTOK_COMMENT_COLUMN:
                InfoNextTok ();
                InfoAssureInt ();
                InfoRangeCheck (MIN_CCOL, MAX_CCOL);
                CCol = InfoIVal;
                InfoNextTok ();
                break;

            case INFOTOK_COMMENTS:
                InfoNextTok ();
                InfoAssureInt ();
                InfoRangeCheck (MIN_COMMENTS, MAX_COMMENTS);
                Comments = InfoIVal;
                InfoNextTok ();
                break;

            case INFOTOK_CPU:
                InfoNextTok ();
                InfoAssureStr ();
                if (CPU != CPU_UNKNOWN) {
                    InfoError ("CPU already specified");
                }
                CPU = FindCPU (InfoSVal);
                SetOpcTable (CPU);
                InfoNextTok ();
                break;

            case INFOTOK_HEXOFFS:
                InfoNextTok ();
                InfoBoolToken ();
                switch (InfoTok) {
                    case INFOTOK_FALSE: UseHexOffs = 0; break;
                    case INFOTOK_TRUE:  UseHexOffs = 1; break;
                }
                InfoNextTok ();
                break;

            case INFOTOK_INPUTNAME:
                InfoNextTok ();
                InfoAssureStr ();
                if (InFile) {
                    InfoError ("Input file name already given");
                }
                InFile = xstrdup (InfoSVal);
                InfoNextTok ();
                break;

            case INFOTOK_INPUTOFFS:
                InfoNextTok ();
                InfoAssureInt ();
                InputOffs = InfoIVal;
                InfoNextTok ();
                break;

            case INFOTOK_INPUTSIZE:
                InfoNextTok ();
                InfoAssureInt ();
                InfoRangeCheck (1, 0x10000);
                InputSize = InfoIVal;
                InfoNextTok ();
                break;

            case INFOTOK_LABELBREAK:
                InfoNextTok ();
                InfoAssureInt ();
                InfoRangeCheck (0, UCHAR_MAX);
                LBreak = (unsigned char) InfoIVal;
                InfoNextTok ();
                break;

            case INFOTOK_MNEMONIC_COLUMN:
                InfoNextTok ();
                InfoAssureInt ();
                InfoRangeCheck (MIN_MCOL, MAX_MCOL);
                MCol = InfoIVal;
                InfoNextTok ();
                break;

            case INFOTOK_NL_AFTER_JMP:
                InfoNextTok ();
                if (NewlineAfterJMP != -1) {
                    InfoError ("NLAfterJMP already specified");
                }
                InfoBoolToken ();
                NewlineAfterJMP = (InfoTok != INFOTOK_FALSE);
                InfoNextTok ();
                break;

            case INFOTOK_NL_AFTER_RTS:
                InfoNextTok ();
                InfoBoolToken ();
                if (NewlineAfterRTS != -1) {
                    InfoError ("NLAfterRTS already specified");
                }
                NewlineAfterRTS = (InfoTok != INFOTOK_FALSE);
                InfoNextTok ();
                break;

            case INFOTOK_OUTPUTNAME:
                InfoNextTok ();
                InfoAssureStr ();
                if (OutFile) {
                    InfoError ("Output file name already given");
                }
                OutFile = xstrdup (InfoSVal);
                InfoNextTok ();
                break;

            case INFOTOK_PAGELENGTH:
                InfoNextTok ();
                InfoAssureInt ();
                if (InfoIVal != 0) {
                    InfoRangeCheck (MIN_PAGE_LEN, MAX_PAGE_LEN);
                }
                PageLength = InfoIVal;
                InfoNextTok ();
                break;

            case INFOTOK_STARTADDR:
                InfoNextTok ();
                InfoAssureInt ();
                InfoRangeCheck (0x0000, 0xFFFF);
                StartAddr = InfoIVal;
                InfoNextTok ();
                break;

            case INFOTOK_TEXT_COLUMN:
                InfoNextTok ();
                InfoAssureInt ();
                InfoRangeCheck (MIN_TCOL, MAX_TCOL);
                TCol = InfoIVal;
                InfoNextTok ();
                break;

            default:
                Internal ("Unexpected token: %u", InfoTok);

        }

        /* Directive is followed by a semicolon */
        InfoConsumeSemi ();

    }

    /* Consume the closing brace */
    InfoConsumeRCurly ();
}



static void LabelSection (void)
/* Parse a label section */
{
    static const IdentTok LabelDefs[] = {
        {   "COMMENT",  INFOTOK_COMMENT },
        {   "ADDR",     INFOTOK_ADDR    },
        {   "NAME",     INFOTOK_NAME    },
        {   "SIZE",     INFOTOK_SIZE    },
    };

    /* Locals - initialize to avoid gcc warnings */
    char* Name    = 0;
    char* Comment = 0;
    long Value    = -1;
    long Size     = -1;

    /* Skip the token */
    InfoNextTok ();

    /* Expect the opening curly brace */
    InfoConsumeLCurly ();

    /* Look for section tokens */
    while (InfoTok != INFOTOK_RCURLY) {

        /* Convert to special token */
        InfoSpecialToken (LabelDefs, ENTRY_COUNT (LabelDefs), "Label attribute");

        /* Look at the token */
        switch (InfoTok) {

            case INFOTOK_ADDR:
                InfoNextTok ();
                if (Value >= 0) {
                    InfoError ("Value already given");
                }
                InfoAssureInt ();
                InfoRangeCheck (0, 0xFFFF);
                Value = InfoIVal;
                InfoNextTok ();
                break;

            case INFOTOK_COMMENT:
                InfoNextTok ();
                if (Comment) {
                    InfoError ("Comment already given");
                }
                InfoAssureStr ();
                if (InfoSVal[0] == '\0') {
                    InfoError ("Comment may not be empty");
                }
                Comment = xstrdup (InfoSVal);
                InfoNextTok ();
                break;

            case INFOTOK_NAME:
                InfoNextTok ();
                if (Name) {
                    InfoError ("Name already given");
                }
                InfoAssureStr ();
                Name = xstrdup (InfoSVal);
                InfoNextTok ();
                break;

            case INFOTOK_SIZE:
                InfoNextTok ();
                if (Size >= 0) {
                    InfoError ("Size already given");
                }
                InfoAssureInt ();
                InfoRangeCheck (1, 0x10000);
                Size = InfoIVal;
                InfoNextTok ();
                break;

            default:
                Internal ("Unexpected token: %u", InfoTok);
        }

        /* Directive is followed by a semicolon */
        InfoConsumeSemi ();
    }

    /* Did we get the necessary data */
    if (Name == 0) {
        InfoError ("Label name is missing");
    }
    if (Name[0] == '\0' && Size > 1) {
        InfoError ("Unnamed labels must not have a size > 1");
    }
    if (Value < 0) {
        InfoError ("Label value is missing");
    }
    if (Size < 0) {
        /* Use default */
        Size = 1;
    }
    if (Value + Size > 0x10000) {
        InfoError ("Invalid size (address out of range)");
    }
    if (HaveLabel ((unsigned) Value)) {
        InfoError ("Label for address $%04lX already defined", Value);
    }

    /* Define the label(s) */
    if (Name[0] == '\0') {
        /* Size has already beed checked */
        AddUnnamedLabel (Value);
    } else {
        AddExtLabelRange ((unsigned) Value, Name, Size);
    }

    /* Define the comment */
    if (Comment) {
        SetComment (Value, Comment);
    }

    /* Delete the dynamically allocated memory for Name and Comment */
    xfree (Name);
    xfree (Comment);

    /* Consume the closing brace */
    InfoConsumeRCurly ();
}



static void RangeSection (void)
/* Parse a range section */
{
    static const IdentTok RangeDefs[] = {
        {   "COMMENT",          INFOTOK_COMMENT },
        {   "END",              INFOTOK_END     },
        {   "NAME",             INFOTOK_NAME    },
        {   "START",            INFOTOK_START   },
        {   "TYPE",             INFOTOK_TYPE    },
    };

    static const IdentTok TypeDefs[] = {
        {   "ADDRTABLE",        INFOTOK_ADDRTAB  },
        {   "BYTETABLE",        INFOTOK_BYTETAB  },
        {   "CODE",             INFOTOK_CODE     },
        {   "DBYTETABLE",       INFOTOK_DBYTETAB },
        {   "DWORDTABLE",       INFOTOK_DWORDTAB },
        {   "RTSTABLE",         INFOTOK_RTSTAB   },
        {   "SKIP",             INFOTOK_SKIP     },
        {   "TEXTTABLE",        INFOTOK_TEXTTAB  },
        {   "WORDTABLE",        INFOTOK_WORDTAB  },
    };


    /* Which values did we get? */
    enum {
        tNone   = 0x00,
        tStart  = 0x01,
        tEnd    = 0x02,
        tType   = 0x04,
        tName   = 0x08,
        tComment= 0x10,
        tNeeded = (tStart | tEnd | tType)
    };
    unsigned Attributes = tNone;

    /* Locals - initialize to avoid gcc warnings */
    unsigned Start      = 0;
    unsigned End        = 0;
    unsigned char Type  = 0;
    char* Name          = 0;
    char* Comment       = 0;
    unsigned MemberSize = 0;


    /* Skip the token */
    InfoNextTok ();

    /* Expect the opening curly brace */
    InfoConsumeLCurly ();

    /* Look for section tokens */
    while (InfoTok != INFOTOK_RCURLY) {

        /* Convert to special token */
        InfoSpecialToken (RangeDefs, ENTRY_COUNT (RangeDefs), "Range attribute");

        /* Look at the token */
        switch (InfoTok) {

            case INFOTOK_COMMENT:
                AddAttr ("COMMENT", &Attributes, tComment);
                InfoNextTok ();
                InfoAssureStr ();
                if (InfoSVal[0] == '\0') {
                    InfoError ("Comment may not be empty");
                }
                Comment = xstrdup (InfoSVal);
                Attributes |= tComment;
                InfoNextTok ();
                break;

            case INFOTOK_END:
                AddAttr ("END", &Attributes, tEnd);
                InfoNextTok ();
                InfoAssureInt ();
                InfoRangeCheck (0x0000, 0xFFFF);
                End = InfoIVal;
                InfoNextTok ();
                break;

            case INFOTOK_NAME:
                AddAttr ("NAME", &Attributes, tName);
                InfoNextTok ();
                InfoAssureStr ();
                if (InfoSVal[0] == '\0') {
                    InfoError ("Name may not be empty");
                }
                Name = xstrdup (InfoSVal);
                Attributes |= tName;
                InfoNextTok ();
                break;

            case INFOTOK_START:
                AddAttr ("START", &Attributes, tStart);
                InfoNextTok ();
                InfoAssureInt ();
                InfoRangeCheck (0x0000, 0xFFFF);
                Start = InfoIVal;
                InfoNextTok ();
                break;

            case INFOTOK_TYPE:
                AddAttr ("TYPE", &Attributes, tType);
                InfoNextTok ();
                InfoSpecialToken (TypeDefs, ENTRY_COUNT (TypeDefs), "TYPE");
                switch (InfoTok) {
                    case INFOTOK_ADDRTAB:  Type = atAddrTab;  MemberSize = 2; break;
                    case INFOTOK_BYTETAB:  Type = atByteTab;  MemberSize = 1; break;
                    case INFOTOK_CODE:     Type = atCode;     MemberSize = 1; break;
                    case INFOTOK_DBYTETAB: Type = atDByteTab; MemberSize = 2; break;
                    case INFOTOK_DWORDTAB: Type = atDWordTab; MemberSize = 4; break;
                    case INFOTOK_RTSTAB:   Type = atRtsTab;   MemberSize = 2; break;
                    case INFOTOK_SKIP:     Type = atSkip;     MemberSize = 1; break;
                    case INFOTOK_TEXTTAB:  Type = atTextTab;  MemberSize = 1; break;
                    case INFOTOK_WORDTAB:  Type = atWordTab;  MemberSize = 2; break;
                }
                InfoNextTok ();
                break;

            default:
                Internal ("Unexpected token: %u", InfoTok);
        }

        /* Directive is followed by a semicolon */
        InfoConsumeSemi ();

    }

    /* Did we get all required values? */
    if ((Attributes & tNeeded) != tNeeded) {
        InfoError ("Required values missing from this section");
    }

    /* Start must be less than end */
    if (Start > End) {
        InfoError ("Start value must not be greater than end value");
    }

    /* Check the granularity */
    if (((End - Start + 1) % MemberSize) != 0) {
        InfoError ("Type of range needs a granularity of %u", MemberSize);
    }

    /* Set the range */
    MarkRange (Start, End, Type);

    /* Do we have a label? */
    if (Attributes & tName) {

        /* Define a label for the table */
        AddExtLabel (Start, Name);

        /* Set the comment if we have one */
        if (Comment) {
            SetComment (Start, Comment);
        }

        /* Delete name and comment */
        xfree (Name);
        xfree (Comment);
    }

    /* Consume the closing brace */
    InfoConsumeRCurly ();
}



static void SegmentSection (void)
/* Parse a segment section */
{
    static const IdentTok LabelDefs[] = {
        {   "END",      INFOTOK_END     },
        {   "NAME",     INFOTOK_NAME    },
        {   "START",    INFOTOK_START   },
    };

    /* Locals - initialize to avoid gcc warnings */
    long End    = -1;
    long Start  = -1;
    char* Name  = 0;

    /* Skip the token */
    InfoNextTok ();

    /* Expect the opening curly brace */
    InfoConsumeLCurly ();

    /* Look for section tokens */
    while (InfoTok != INFOTOK_RCURLY) {

        /* Convert to special token */
        InfoSpecialToken (LabelDefs, ENTRY_COUNT (LabelDefs), "Segment attribute");

        /* Look at the token */
        switch (InfoTok) {

            case INFOTOK_END:
                InfoNextTok ();
                if (End >= 0) {
                    InfoError ("Value already given");
                }
                InfoAssureInt ();
                InfoRangeCheck (0, 0xFFFF);
                End = InfoIVal;
                InfoNextTok ();
                break;

            case INFOTOK_NAME:
                InfoNextTok ();
                if (Name) {
                    InfoError ("Name already given");
                }
                InfoAssureStr ();
                Name = xstrdup (InfoSVal);
                InfoNextTok ();
                break;

            case INFOTOK_START:
                InfoNextTok ();
                if (Start >= 0) {
                    InfoError ("Value already given");
                }
                InfoAssureInt ();
                InfoRangeCheck (0, 0xFFFF);
                Start = InfoIVal;
                InfoNextTok ();
                break;

            default:
                Internal ("Unexpected token: %u", InfoTok);
        }

        /* Directive is followed by a semicolon */
        InfoConsumeSemi ();
    }

    /* Did we get the necessary data, and is it correct? */
    if (Name == 0 || Name[0] == '\0') {
        InfoError ("Segment name is missing");
    }
    if (End < 0) {
        InfoError ("End address is missing");
    }
    if (Start < 0) {
        InfoError ("Start address is missing");
    }
    if (Start > End) {
        InfoError ("Start address of segment is greater than end address");
    }

    /* Check that segments do not overlap */
    if (SegmentDefined ((unsigned) Start, (unsigned) End)) {
        InfoError ("Segments must not overlap");
    }

    /* Remember the segment data */
    AddAbsSegment ((unsigned) Start, (unsigned) End, Name);

    /* Delete the dynamically allocated memory for Name */
    xfree (Name);

    /* Consume the closing brace */
    InfoConsumeRCurly ();
}



static void InfoParse (void)
/* Parse the config file */
{
    static const IdentTok Globals[] = {
        {   "ASMINC",   INFOTOK_ASMINC  },
        {   "GLOBAL",   INFOTOK_GLOBAL  },
        {   "LABEL",    INFOTOK_LABEL   },
        {   "RANGE",    INFOTOK_RANGE   },
        {   "SEGMENT",  INFOTOK_SEGMENT },
    };

    while (InfoTok != INFOTOK_EOF) {

        /* Convert an identifier into a token */
        InfoSpecialToken (Globals, ENTRY_COUNT (Globals), "Config directive");

        /* Check the token */
        switch (InfoTok) {

            case INFOTOK_ASMINC:
                AsmIncSection ();
                break;

            case INFOTOK_GLOBAL:
                GlobalSection ();
                break;

            case INFOTOK_LABEL:
                LabelSection ();
                break;

            case INFOTOK_RANGE:
                RangeSection ();
                break;

            case INFOTOK_SEGMENT:
                SegmentSection ();
                break;

            default:
                Internal ("Unexpected token: %u", InfoTok);
        }

        /* Semicolon expected */
        InfoConsumeSemi ();
    }
}



void ReadInfoFile (void)
/* Read the info file */
{
    /* Check if we have a info file given */
    if (InfoAvail()) {
        /* Open the config file */
        InfoOpenInput ();

        /* Parse the config file */
        InfoParse ();

        /* Close the file */
        InfoCloseInput ();
    }
}
