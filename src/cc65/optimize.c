/*****************************************************************************/
/*                                                                           */
/*	     			  optimize.c				     */
/*                                                                           */
/*	   	     An optimizer for the cc65 C compiler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998     Ullrich von Bassewitz                                        */
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



#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* common */
#include "attrib.h"
#include "check.h"
#include "xmalloc.h"
#include "xsprintf.h"

/* cc65 */
#include "asmlabel.h"
#include "asmline.h"
#include "cpu.h"
#include "error.h"
#include "global.h"
#include "optimize.h"



/*****************************************************************************/
/*     	   	     		     Data				     */
/*****************************************************************************/



/* Bitset of flags that switch the different optimizer passes */
unsigned long OptDisable	= 0;



/* Bitmapped flags for the Flags field in the Line struct */
#define OF_CODE	 	0x0001		/* This line is in a code segment */

/* Pointer to first code line */
static Line*	    	FirstCode;

/* Label list */
static Line**		Labels = 0;	/* Pointers to label lines */
static unsigned		LabelCount = 0;	/* Count of local labels found */

/* A collection of lines */
typedef struct LineColl_ LineColl;
struct LineColl_ {
    unsigned	Count;			/* Count of lines in the collection */
    unsigned 	Max;			/* Maximum count of lines */
    Line*	Lines[1];		/* Lines, dynamically allocated */
};



/* Calculate the element count of a table */
#define COUNT(T) 	(sizeof (T) / sizeof (T [0]))

/* Macro to increment and decrement register contents if they're valid */
#define INC(reg,val)    if ((reg) >= 0) (reg) = ((reg) + val) & 0xFF
#define DEC(reg,val)   	if ((reg) >= 0) (reg) = ((reg) - val) & 0xFF

/* Defines for the conditions in a compare */
#define CMP_EQ		0
#define CMP_NE		1
#define CMP_GT		2
#define CMP_GE		3
#define CMP_LT		4
#define CMP_LE		5
#define CMP_UGT		6
#define CMP_UGE		7
#define CMP_ULT		8
#define CMP_ULE		9

/* Defines for registers */
#define REG_NONE	0x00
#define REG_A		0x01
#define REG_X		0x02
#define REG_Y		0x04
#define	REG_AX		(REG_A | REG_X)
#define REG_ALL		(REG_A | REG_X | REG_Y)

/* Description of the commands */
static const struct {
    const char* 	Insn; 		/* Instruction */
    unsigned char	FullMatch;	/* Match full instuction? */
    unsigned char  	Use;  		/* Registers used */
    unsigned char	Load; 		/* Registers loaded */
} CmdDesc [] = {
    { "\tadc\t",       	  0,   	REG_A, 	    REG_NONE	  },
    { "\tand\t",       	  0,   	REG_A, 	    REG_NONE	  },
    { "\tasl\ta",         1,   	REG_A, 	    REG_NONE	  },
    { "\tasl\t",       	  0,   	REG_NONE,   REG_NONE	  },
    { "\tclc", 	       	  1,   	REG_NONE,   REG_NONE	  },
    { "\tcld",		  1,    REG_NONE,   REG_NONE	  },
    { "\tcli", 	       	  1,   	REG_NONE,   REG_NONE	  },
    { "\tcmp\t",       	  0,   	REG_A, 	    REG_NONE	  },
    { "\tcpx\t",       	  0,   	REG_X, 	    REG_NONE	  },
    { "\tcpy\t",       	  0,   	REG_Y, 	    REG_NONE	  },
    { "\tdea",		  1,	REG_A,	    REG_NONE	  },
    { "\tdec\ta",  	  1,	REG_A,	    REG_NONE	  },
    { "\tdec\t",       	  0,   	REG_NONE,   REG_NONE	  },
    { "\tdex", 	       	  1,   	REG_X, 	    REG_NONE	  },
    { "\tdey", 	       	  1,   	REG_Y, 	    REG_NONE	  },
    { "\teor\t",       	  0,   	REG_A, 	    REG_NONE	  },
    { "\tina",		  1,	REG_A,	    REG_NONE	  },
    { "\tinc\ta",	  1,    REG_A,	    REG_NONE	  },
    { "\tinc\t",       	  0,   	REG_NONE,   REG_NONE	  },
    { "\tinx", 	       	  1,   	REG_X, 	    REG_NONE	  },
    { "\tiny", 	       	  1,   	REG_Y, 	    REG_NONE	  },
    { "\tjsr\tbool",   	  0,   	REG_NONE,   REG_AX     	  },
    { "\tjsr\tdecaxy",	  1,	REG_ALL,    REG_AX	  },
    { "\tjsr\tdecax",  	  0,   	REG_AX,	    REG_AX    	  },
    { "\tjsr\tldax0sp",   1,   	REG_Y, 	    REG_AX	  },
    { "\tjsr\tldaxysp",   1,   	REG_Y,      REG_AX 	  },
    { "\tjsr\tpusha",  	  1,   	REG_A, 	    REG_Y	  },
    { "\tjsr\tpusha0", 	  1,   	REG_A, 	    REG_X | REG_Y },
    { "\tjsr\tpushax", 	  1,   	REG_AX,	    REG_Y	  },
    { "\tjsr\tpushw0sp",  1,   	REG_NONE,   REG_ALL	  },
    { "\tjsr\tpushwysp",  1,   	REG_Y, 	    REG_ALL	  },
    { "\tjsr\ttosicmp",   1,   	REG_AX,	    REG_ALL    	  },
    { "\tlda\t",       	  0,   	REG_NONE,   REG_A  	  },
    { "\tldax\t",         0,   	REG_NONE,   REG_AX 	  },
    { "\tldx\t",       	  0,   	REG_NONE,   REG_X  	  },
    { "\tldy\t",       	  0,   	REG_NONE,   REG_Y  	  },
    { "\tlsr\ta",         1,   	REG_A, 	    REG_NONE   	  },
    { "\tlsr\t",       	  0,   	REG_NONE,   REG_NONE   	  },
    { "\tnop", 	       	  1,   	REG_NONE,   REG_NONE	  },
    { "\tora\t",       	  0,   	REG_A, 	    REG_NONE	  },
    { "\tpha", 	       	  1,   	REG_A, 	    REG_NONE	  },
    { "\tphp", 	       	  1,   	REG_NONE,   REG_NONE	  },
    { "\tpla", 	       	  1,   	REG_NONE,   REG_A	  },
    { "\tplp", 	       	  1,   	REG_NONE,   REG_NONE	  },
    { "\trol\ta",	  1,	REG_A,	    REG_A	  },
    { "\trol\t",       	  0,   	REG_NONE,   REG_NONE	  },
    { "\tror\ta",	  1,	REG_A,	    REG_A	  },
    { "\tror\t",       	  0,   	REG_NONE,   REG_NONE	  },
    { "\tsbc\t",       	  0,   	REG_A, 	    REG_NONE	  },
    { "\tsec",       	  1,   	REG_NONE,   REG_NONE	  },
    { "\tsed",       	  1,   	REG_NONE,   REG_NONE	  },
    { "\tsei",       	  1,   	REG_NONE,   REG_NONE	  },
    { "\tsta\t",       	  0,   	REG_A, 	    REG_NONE	  },
    { "\tstx\t",       	  0,   	REG_X, 	    REG_NONE	  },
    { "\tsty\t",       	  0,   	REG_Y, 	    REG_NONE	  },
    { "\tstz\t",	  0,	REG_NONE,   REG_NONE	  },
    { "\ttax",         	  1,   	REG_A, 	    REG_X  	  },
    { "\ttay",         	  1,   	REG_A, 	    REG_Y  	  },
    { "\ttrb\t",       	  0,   	REG_A,	    REG_NONE	  },
    { "\ttsb\t",       	  0,   	REG_A,	    REG_NONE	  },
    { "\ttsx",         	  1,   	REG_NONE,   REG_X  	  },
    { "\ttxa",         	  1,   	REG_X, 	    REG_A  	  },
    { "\ttya",         	  1,   	REG_Y, 	    REG_A  	  },
};



/* Table with the compare suffixes */
static const char CmpSuffixTab [][4] = {
    "eq", "ne", "gt", "ge", "lt", "le", "ugt", "uge", "ult", "ule"
};

/* Table used to invert a condition, indexed by condition */
static const unsigned char CmpInvertTab [] = {
    CMP_NE, CMP_EQ,
    CMP_LE, CMP_LT, CMP_GE, CMP_GT,
    CMP_ULE, CMP_ULT, CMP_UGE, CMP_UGT
};

/* Table to show which compares are signed (use the N flag) */
static const char CmpSignedTab [] = {
    0, 0, 1, 1, 1, 1, 0, 0, 0, 0
};



/* Lists of branches */
static const char* ShortBranches [] = {
    "\tbeq\t",
    "\tbne\t",
    "\tbpl\t",
    "\tbmi\t",
    "\tbcc\t",
    "\tbcs\t",
    "\tbvc\t",
    "\tbvs\t",
    0
};
static const char* LongBranches [] = {
    "\tjeq\t",
    "\tjne\t",
    "\tjpl\t",
    "\tjmi\t",
    "\tjcc\t",
    "\tjcs\t",
    "\tjvc\t",
    "\tjvs\t",
    0
};



/*****************************************************************************/
/*     		   		   Forwards				     */
/*****************************************************************************/



static unsigned EstimateSize (Line* L);
/* Estimate the size of an instruction */

static int IsLocalLabel (const Line* L);
/* Return true if the line is a local label line */

static unsigned GetLabelNum (const char* L);
/* Return the label number of a label line */

static unsigned RVUInt1 (Line* L, LineColl* LC, unsigned Used, unsigned Unused);
/* Subfunction for RegValUsed. Will be called recursively in case of branches. */

static Line* NewLineAfter (Line* LineBefore, const char* Format, ...) attribute ((format(printf,2,3)));
/* Create a new line, insert it after L and return it. The new line is marked
 * as code line.
 */

static Line* ReplaceLine (Line* L, const char* Format, ...)
	attribute ((format(printf,2,3)));
/* Replace one line by another */



/*****************************************************************************/
/*     		   	     	  List stuff				     */
/*****************************************************************************/



static Line* NewLineAfter (Line* LineBefore, const char* Format, ...)
/* Create a new line, insert it after L and return it. The new line is marked
 * as code line.
 */
{
    Line* L;

    /* Format the new line and add it */
    va_list ap;
    va_start (ap, Format);
    L = NewCodeLineAfter (LineBefore, Format, ap);
    va_end (ap);

    /* Make the line a code line */
    L->Flags = OF_CODE;

    /* Estimate the code size */
    L->Size = EstimateSize (L);

    /* Return the new line */
    return L;
}



static Line* NewLabelAfter (Line* L, unsigned Label)
/* Add a new line with a definition of a local label after the line L */
{
    char Buf [32];

    /* Create the label */
    sprintf (Buf, "L%04X:", Label);

    /* Create a new line */
    L = NewLineAfter (L, Buf);

    /* Insert this label into the label list */
    Labels [Label] = L;

    /* Return the new line */
    return L;
}



static void FreeLine (Line* L)
/* Remove a line from the list and free it */
{
    /* If this is a label line, remove it from the label list */
    if (IsLocalLabel (L)) {
       	Labels [GetLabelNum (L->Line)] = 0;
    }

    /* Unlink the line */
    FreeCodeLine (L);
}



static Line* ReplaceLine (Line* L, const char* Format, ...)
/* Replace one line by another */
{
    unsigned Len;
    char S [256];

    /* Format the new line */
    va_list ap;
    va_start (ap, Format);
    xvsprintf (S, sizeof (S), Format, ap);
    va_end (ap);

    /* Get the length of the new line */
    Len = strlen (S);

    /* We can copy the line if the old line has space enough */
    if (Len <= L->Len) {

       	/* Just copy the new line, but don't update the length */
       	memcpy (L->Line, S, Len);
       	L->Line [Len] = '\0';

    } else {

     	/* We must allocate new space */
     	Line* NewLine = xmalloc (sizeof (Line) + Len);

       	/* Set the values in the new struct */
     	NewLine->Flags	= L->Flags;
     	NewLine->Index	= L->Index;
	NewLine->Size	= L->Size;		/* Hmm ... */
     	NewLine->Len	= Len;
     	memcpy (NewLine->Line, S, Len + 1);

     	/* Replace the old struct in the list */
       	NewLine->Next = L->Next;
     	if (NewLine->Next) {
     	    NewLine->Next->Prev = NewLine;
     	} else {
     	    /* Last line */
     	    LastLine = NewLine;
       	}
       	NewLine->Prev = L->Prev;
     	if (NewLine->Prev) {
     	    NewLine->Prev->Next = NewLine;
     	} else {
     	    /* First line */
     	    FirstLine = NewLine;
     	}

     	/* Free the old struct */
     	xfree (L);
	L = NewLine;
    }

    /* Estimate the new size */
    if (L->Flags & OF_CODE) {
       	L->Size = EstimateSize (L);
    }

    /* Return the line */
    return L;
}



static Line* PrevCodeLine (Line* L)
/* Return the previous line containing code */
{
    L = L->Prev;
    while (L) {
	if (L->Flags & OF_CODE && L->Line [0] != '+') {
       	    break;
       	}
       	L = L->Prev;
    }
    return L;
}



static Line* NextCodeSegLine (Line* L)
/* Return the next line in the code segment */
{
    L = L->Next;
    while (L) {
       	if (L->Flags & OF_CODE) {
       	    break;
       	}
       	L = L->Next;
    }
    return L;
}



static Line* NextCodeLine (Line* L)
/* Return the next line containing code */
{
    L = L->Next;
    while (L) {
       	if ((L->Flags & OF_CODE) != 0 && L->Line [0] != '+') {
       	    break;
       	}
       	L = L->Next;
    }
    return L;
}



static Line* NextInstruction (Line* L)
/* Return the next line containing code, ignoring labels. */
{
    do {
	L = NextCodeLine (L);
    } while (L && (L->Line[0] == '+' || IsLocalLabel(L)));
    return L;
}



static void FreeLines (Line* Start, Line* End)
/* Delete all lines from Start to End, both inclusive */
{
    Line* L;
    do {
       	L = Start;
       	Start = NextCodeSegLine (Start);
       	FreeLine (L);
    } while (L != End);
}



/*****************************************************************************/
/*	    		     Line Collections				     */
/*****************************************************************************/



static LineColl* NewLineColl (unsigned Size)
/* Create a new line collection and return it */
{
    /* Allocate memory */
    LineColl* LC = xmalloc (sizeof (LineColl) + sizeof (Line) * (Size-1));

    /* Initialize members */
    LC->Count = 0;
    LC->Max   = Size;

    /* Return the new collection */
    return LC;
}



static void FreeLineColl (LineColl* LC)
/* Delete a line collection */
{
    xfree (LC);
}



static int LCAddLine (LineColl* LC, Line* L)
/* Add a line. Return 0 if no space available, return 1 otherwise */
{
    /* Check if there is enough space available */
    if (LC->Count >= LC->Max) {
    	/* No room available */
    	return 0;
    }

    /* Add the line */
    LC->Lines [LC->Count++] = L;

    /* Done */
    return 1;
}



static int LCHasLine (LineColl* LC, Line* L)
/* Check if the given line is in the collection */
{
    unsigned I;
    for (I = 0; I < LC->Count; ++I) {
	if (LC->Lines[I] == L) {
	    return 1;
	}
    }
    return 0;
}



/*****************************************************************************/
/*     	     	   	Test a line for several things			     */
/*****************************************************************************/



static int IsLocalLabel (const Line* L)
/* Return true if the line is a local label line */
{
    return (L->Line [0] == 'L' && isxdigit (L->Line [1]));
}



static int IsExtLabel (const Line* L)
/* Return true if the line is an external label line */
{
    return (L->Line [0] == '_');
}



static int IsLabel (const Line* L)
/* Return true if the line is a label line */
{
    return (L->Line [0] == 'L' && isxdigit (L->Line [1])) ||
	   (L->Line [0] == '_');;
}



static int IsHintLine (const Line* L)
/* Return true if the line contains an optimizer hint */
{
    return L->Line [0] == '+';
}



static int IsSegHint (const Line* L)
/* Return true if the given line contains a segment hint */
{
    return (L->Line [0] == '+' && strncmp (L->Line + 1, "seg:", 4) == 0);
}



static int IsHint (const Line* L, const char* Hint)
/* Check if the line contains a given hint */
{
    return (L->Line [0] == '+' && strcmp (L->Line + 1, Hint) == 0);
}



static int IsCondJump (const Line* L)
/* Return true if the line contains a conditional jump */
{
    return (L->Line [0] == '\t' &&
	    (strncmp (L->Line + 1, "beq\t", 4) == 0 ||
	     strncmp (L->Line + 1, "bne\t", 4) == 0 ||
	     strncmp (L->Line + 1, "jeq\t", 4) == 0 ||
	     strncmp (L->Line + 1, "jne\t", 4) == 0));
}



static int IsXAddrMode (const Line* L)
/* Return true if the given line does use the X register */
{
    unsigned Len = strlen (L->Line);
    return (strcmp (L->Line + Len - 3, ",x)") == 0 ||
  	    strcmp (L->Line + Len - 2, ",x")  == 0);
}



static int NoXAddrMode (const Line* L)
/* Return true if the given line does use the X register */
{
    return !IsXAddrMode (L);
}



static int IsYAddrMode (const Line* L)
/* Return true if the given line does use the Y register */
{
    unsigned Len = strlen (L->Line);
    return (strcmp (L->Line + Len - 2, ",y") == 0);
}



static int Is16BitStore (const Line* L1, const Line* L2)
/* Check if L1 and L2 are a store of ax into a 16 bit location */
{
    unsigned Len1 = strlen (L1->Line);
    return (strncmp (L1->Line, "\tsta\t", 5) == 0   		&&
	    strncmp (L2->Line, "\tstx\t", 5) == 0   		&&
	    strncmp (L1->Line+5, L2->Line+5, Len1-5) == 0	&&
	    strcmp (L2->Line+Len1, "+1") == 0);
}



static Line* FindHint (Line* L, const char* Hint)
/* Search for a line with the given hint */
{
    while (L) {
	if (IsHint (L, Hint)) {
     	    break;
       	}
     	L = L->Next;
    }
    return L;
}



static unsigned GetHexNum (const char* S)
/* Get a hex number from a string */
{
    unsigned I = 0;
    unsigned Val = 0;
    while (isxdigit (S [I])) {
     	int C = (unsigned char) (S [I++]);
     	if (C >= 'A') {
     	    C -= 'A' - 10;
     	} else {
     	    C -= '0';
     	}
     	Val = (Val << 4) + C;
    }
    return Val;
}



static unsigned GetLabelNum (const char* L)
/* Return the label number of a label line */
{
    CHECK (*L == 'L');
    return GetHexNum (L+1);
}



static Line* GetTargetLine (const char* L)
/* Get the line with the target label of a jump. L must be a pointer to the
 * string containing the label number.
 */
{
    Line* Target;

    /* Get the label number of the target */
    unsigned Label = GetLabelNum (L);
    CHECK (Label < LabelCount);

    /* Get the line with this label */
    Target = Labels [Label];
    CHECK (Target != 0 && (Target->Flags & OF_CODE) != 0);

    /* And return it */
    return Target;
}



static unsigned GetJumpDistance (Line* L, Line* Target)
/* Get the distance between both lines */
{
    unsigned Distance = 0;

    if (L != Target) {
	if (Target->Index > L->Index) {
	    /* This is a forward jump. */
	    do {
		L = NextCodeLine (L);
		Distance += L->Size;
	    } while (L != Target);
	} else {
	    /* This is a backward jump */
	    do {
		L = PrevCodeLine (L);
		Distance += L->Size;
	    } while (L != Target);
	}
    }

    /* Return the calculated distance */
    return Distance;
}



static int LineMatch (const Line* L, const char* Start)
/* Check if the start of the line matches Start */
{
    return strncmp (L->Line, Start, strlen (Start)) == 0;
}



static int LineFullMatch (const Line* L, const char* Start)
/* Check if the matches Start */
{
    return strcmp (L->Line, Start) == 0;
}



static int LineMatchX (const Line* L, const char** Start)
/* Check the start of the line against a list of patterns. Return the
 * number of the pattern that matched, or -1 in case of no match.
 */
{
    unsigned I = 0;
    while (*Start) {
    	if (LineMatch (L, *Start)) {
	    /* Found */
    	    return I;
    	}
    	++Start;
    	++I;
    }
    /* Not found */
    return -1;
}



static int LineFullMatchX (const Line* L, const char** Start)
/* Check the the line against a list of patterns. Return the
 * number of the pattern that matched, or -1 in case of no match.
 */
{
    unsigned I = 0;
    while (*Start) {
    	if (LineFullMatch (L, *Start)) {
	    /* Found */
    	    return I;
    	}
    	++Start;
    	++I;
    }
    /* Not found */
    return -1;
}



static int IsLoadAX (Line* L1, Line* L2)
/* Check if the both lines load a static variable into ax. That is, both lines
 * look like
 *	lda	x+0
 *	ldx	x+0+1
 */
{
    return LineMatch (L1, "\tlda\t") 					&&
	   LineMatch (L2, "\tldx\t")					&&
	   strncmp (L1->Line+5, L2->Line+5, strlen (L1->Line+5)) == 0	&&
	   strcmp (L2->Line+strlen(L1->Line), "+1") == 0;
}



/*****************************************************************************/
/*   		      	    Initial optimizer setup			     */
/*****************************************************************************/



static void FindCodeStart (void)
/* Find and remember the first line of actual code */
{
    Line* L = FindHint (FirstLine, "end_of_preamble");
    FirstCode = L? L->Next : 0;
}



static unsigned EstimateDataSize (Line* L, unsigned Chunk)
/* Estimate the size of a .byte, .word or .dword command */
{
    unsigned Size = Chunk;
    char* S = L->Line;
    while ((S = strchr (S, ',')) != 0) {
	Size += Chunk;
	++S;
    }
    return Size;
}



static unsigned EstimateSize (Line* L)
/* Estimate the size of an instruction */
{
    static const char* OneByteCmds [] = {
	"\tdea",
	"\tdex",
	"\tdey",
	"\tina",
	"\tinx",
	"\tiny"
	"\ttax",
	"\ttay",
	"\ttsx",
	"\ttxa",
	"\ttya",
	0
    };
    char OpStart;

    if (L->Line [0] != '\t') {
      	return 0;
    }
    if (LineMatch (L, "\tldax\t")) {
	/* Immidiate load of both, A and X */
	return 4;
    }
    if (LineMatch (L, "\tld")) {
      	OpStart = L->Line [5];
      	return (OpStart == '#' || OpStart == '(')? 2 : 3;
    }
    if (LineMatch (L, "\tst")) {
      	OpStart = L->Line [5];
      	return (OpStart == '(')? 2 : 3;
    }
    if (LineMatch (L, "\t.byte\t")) {
      	return EstimateDataSize (L, 1);
    }
    if (LineMatch (L, "\t.word\t")) {
      	return EstimateDataSize (L, 2);
    }
    if (LineMatch (L, "\t.dword\t")) {
      	return EstimateDataSize (L, 4);
    }
    if (LineMatchX (L, ShortBranches) >= 0) {
	return 2;
    }
    if (LineMatchX (L, LongBranches) >= 0) {
	return 5;
    }
    if (LineMatchX (L, OneByteCmds) >= 0) {
	return 1;
    }
    return 3;
}



static void MarkCodeLines (void)
/* Mark all lines that are inside a code segment */
{
    int InCode = 1;
    Line* L = FirstCode;
    while (L) {
       	if (IsSegHint (L)) {
	    InCode = IsHint (L, "seg:code");
       	} else if (InCode && L->Line[0] != '\0') {
	    L->Flags |= OF_CODE;
       	    L->Size = EstimateSize (L);
	}
	L = L->Next;
    }
}



static void CreateLabelList (void)
/* Create a list with pointers to local labels */
{
    unsigned I;
    Line* L;


    /* Get the next label number. This is also the current label count.
     * Make some room for more labels when optimizing code.
     */
    LabelCount = GetLabel () + 100;

    /* Allocate memory for the array and clear it */
    Labels = xmalloc (LabelCount * sizeof (Line*));
    for (I = 0; I < LabelCount; ++I) {
       	Labels [I] = 0;
    }

    /* Walk through the code and insert all label lines */
    L = FirstLine;
    while (L) {
       	if (IsLocalLabel (L)) {
       	    unsigned LabelNum = GetLabelNum (L->Line);
       	    CHECK (LabelNum < LabelCount);
       	    Labels [LabelNum] = L;
       	}
       	L = L->Next;
    }
}



static unsigned AllocLabel (void)
/* Get a new label. The current code does not realloc the label list, so there
 * must be room enough in the current list.
 */
{
    unsigned I;

    /* Search for a free slot, start at 1, since 0 is "no label" */
    for (I = 1; I < LabelCount; ++I) {
	if (Labels[I] == 0) {
	    /* Found a free slot */
	    return I;
	}
    }

    /* No label space available */
    Internal ("Out of label space in the optimizer");

    /* Not reached */
    return 0;
}



/*****************************************************************************/
/*     		 	       Helper functions				     */
/*****************************************************************************/



static int GetNextCodeLines (Line* L, Line** Lines, unsigned Count)
/* Get a number of code lines ignoring hints and other stuff. The function
 * returns 1 if we got the lines and 0 if we are at the end of the code
 * segment or if we hit a label.
 */
{
    while (Count--) {

	/* Get the next valid line */
     	do {
	    L = NextCodeLine (L);
	} while (L && IsHintLine (L));

	/* Did we get one? */
	if (L == 0 || IsLabel (L)) {
	    /* Error */
	    return 0;
	}

	/* Remember the line */
	*Lines++ = L;
    }

    /* Success */
    return 1;
}



static int FindCond (const char* Suffix)
/* Map a condition suffix to a code. Return the code or -1 on failure */
{
    int I;

    /* Linear search */
    for (I = 0; I < sizeof (CmpSuffixTab) / sizeof (CmpSuffixTab [0]); ++I) {
     	if (strncmp (Suffix, CmpSuffixTab [I], strlen (CmpSuffixTab[I])) == 0) {
	    /* Found */
     	    return I;
     	}
    }

    /* Not found */
    return -1;
}



static int CheckAndGetIntCmp (const Line* JSR, const Line* JMP)
/* Helper function to check for a compare subroutine call followed by a
 * conditional branch. Will return the condition found, or -1 if no
 * or invalid condition.
 */
{
    char Cond[5];
    const char* Tail;
    int C;

    /* Extract the condition from the function name. */
    if ((Cond [0] = JSR->Line [8]) == 'u') {
	Cond [1] = JSR->Line [9];
	Cond [2] = JSR->Line [10];
	Cond [3] = '\0';
	Tail = JSR->Line + 11;
    } else {
	Cond [1] = JSR->Line [9];
	Cond [2] = '\0';
	Tail = JSR->Line + 10;
    }

    /* Check if this is indeed an integer function */
    if (strcmp (Tail, "ax") != 0) {
	/* No! */
	return -1;
    }

    /* Get the condition code */
    C = FindCond (Cond);
    if (C < 0) {
	/* OOPS! */
	return -1;
    }

    /* Invert the code if we jump on condition not met. */
    if (JMP->Line [2] == 'e' && JMP->Line [3] == 'q') {
	/* Jumps if condition false, invert condition */
	C = CmpInvertTab [C];
    }

    /* Return the condition code */
    return C;
}



static int TosCmpFunc (Line* L)
/* Check if this is a call to one of the TOS compare functions (tosgtax).
 * Return the condition code or -1 if not found.
 */
{
    if (LineMatch (L, "\tjsr\ttos")				&&
    	strcmp (L->Line+strlen(L->Line)-2, "ax") == 0) {

	/* Ok, found. Get the condition. */
       	return FindCond (L->Line+8);

    } else {

	/* Not found */
	return -1;
    }
}



static int IsUnsignedCmp (int Code)
/* Check if this is an unsigned compare */
{
    CHECK (Code >= 0);
    return CmpSignedTab [Code] == 0;
}



static void InvertZJump (Line* L)
/* Invert a jeq/jne jump */
{
    if (L->Line [2] == 'n' && L->Line [3] == 'e') {
	/* This was a bne/jne */
	L->Line [2] = 'e';
     	L->Line [3] = 'q';
    } else {
	/* This was (hopefully) a beq/jeq */
	L->Line [2] = 'n';
	L->Line [3] = 'e';
    }
}



static int FindCmd (Line* L)
{
    int I;

    /* Search for the known patterns */
    for (I = 0; I < COUNT(CmdDesc); ++I) {
	if (CmdDesc[I].FullMatch) {
	    if (LineFullMatch (L, CmdDesc[I].Insn)) {
		/* found */
		return I;
	    }
	} else {
	    if (LineMatch (L, CmdDesc[I].Insn)) {
		/* found */
		return I;
	    }
	}
    }
    /* Not found */
    return -1;
}



static unsigned RVUInt2 (Line* L,
		       	 LineColl* LC, 	    /* To remember visited lines */
		   	 unsigned Used,     /* Definitely used registers */
		   	 unsigned Unused)   /* Definitely unused registers */
/* Subfunction for RegValUsed. Will be called recursively in case of branches. */
{
    int I;

    /* Check the following instructions. We classifiy them into primary
     * loads (register value not used), neutral (check next instruction),
     * and unknown (assume register was used).
     */
    while (1) {

    	unsigned R;

	/* Get the next line and follow jumps */
	do {

	    /* Handle jumps to local labels (continue there) */
       	    if (LineMatch (L, "\tjmp\tL") || LineMatch (L, "\tbra\tL")) {
	     	/* Get the target of the jump */
	     	L = GetTargetLine (L->Line+5);
	    }

	    /* Get the next line, skip local labels */
	    do {
       	    	L = NextCodeSegLine (L);
	    } while (L && (IsLocalLabel (L) || L->Line[0] == '\0'));

	    /* Bail out if we're done */
	    if (L == 0 || IsExtLabel (L)) {
	    	/* End of function reached */
	    	goto ExitPoint;
	    }

	    /* Check if we had this line already. If so, bail out, if not,
	     * add it to the list of known lines.
	     */
	    if (LCHasLine (LC, L) || !LCAddLine (LC, L)) {
	    	goto ExitPoint;
	    }

	} while (LineMatch (L, "\tjmp\tL") || LineMatch (L, "\tbra\tL"));

	/* Special handling of code hints */
       	if (IsHintLine (L)) {

	    if (IsHint (L, "a:-") && (Used & REG_A) == 0) {
		Unused |= REG_A;
	    } else if (IsHint (L, "x:-") && (Used & REG_X) == 0) {
		Unused |= REG_X;
	    } else if (IsHint (L, "y:-") && (Used & REG_Y) == 0) {
		Unused |= REG_Y;
	    }

	/* Special handling for branches */
	} else if (LineMatchX (L, ShortBranches) >= 0 ||
	    LineMatchX (L, LongBranches) >= 0) {
	    const char* Target = L->Line+5;
	    if (Target[0] == 'L') {
	       	/* Jump to local label. Check the register usage starting at
	       	 * the branch target and at the code following the branch.
	       	 * All registers that are unused in both execution flows are
	       	 * returned as unused.
	       	 */
	       	unsigned U1, U2;
       	       	U2 = RVUInt1 (GetTargetLine (Target), LC, Used, Unused);
	       	U1 = RVUInt1 (L, LC, Used, Unused);
	       	return U1 | U2;		/* Used in any of the branches */
	    }
	} else {

	    /* Search for the instruction in this line */
	    I = FindCmd (L);

	    /* If we don't find it, assume all other registers are used */
	    if (I < 0) {
		break;
	    }

	    /* Evaluate the use flags, check for addressing modes */
	    R = CmdDesc[I].Use;
	    if (IsXAddrMode (L)) {
		R |= REG_X;
	    } else if (IsYAddrMode (L)) {
		R |= REG_Y;
	    }
	    if (R) {
		/* Remove registers that were already new loaded */
		R &= ~Unused;

		/* Remember the remaining registers */
		Used |= R;
	    }

	    /* Evaluate the load flags */
	    R = CmdDesc[I].Load;
	    if (R) {
		/* Remove registers that were already used */
		R &= ~Used;

		/* Remember the remaining registers */
		Unused |= R;
	    }

	}

       	/* If we know about all registers, bail out */
       	if ((Used | Unused) == REG_ALL) {
	    break;
	}
    }

ExitPoint:
    /* Return to the caller the complement of all unused registers */
    return ~Unused & REG_ALL;
}



static unsigned RVUInt1 (Line* L,
		       	 LineColl* LC, 	    /* To remember visited lines */
		       	 unsigned Used,     /* Definitely used registers */
		       	 unsigned Unused)   /* Definitely unused registers */
/* Subfunction for RegValUsed. Will be called recursively in case of branches. */
{
    /* Remember the current count of the line collection */
    unsigned Count = LC->Count;

    /* Call the worker routine */
    unsigned R = RVUInt2 (L, LC, Used, Unused);

    /* Restore the old count */
    LC->Count = Count;

    /* Return the result */
    return R;
}



static unsigned RegValUsed (Line* Start)
/* Check the next instructions after the one in L for register usage. If
 * a register is used as an index, or in a store or other instruction, it
 * is assumed to be used. If a register is loaded with a value, before it
 * was used by one of the actions described above, it is assumed unused.
 * If the end of the lookahead is reached, all registers that are uncertain
 * are marked as used.
 * The result of the search is returned.
 */
{
    unsigned R;

    /* Create a new line collection and enter the start line */
    LineColl* LC = NewLineColl (256);
    LCAddLine (LC, Start);

    /* Call the recursive subfunction */
    R = RVUInt1 (Start, LC, REG_NONE, REG_NONE);

    /* Delete the line collection */
    FreeLineColl (LC);

    /* Return the registers used */
    return R;
}



static int RegAUsed (Line* Start)
/* Check if the value in A is used. */
{
    return (RegValUsed (Start) & REG_A) != 0;
}



static int RegXUsed (Line* Start)
/* Check if the value in X is used. */
{
    return (RegValUsed (Start) & REG_X) != 0;
}



static int RegYUsed (Line* Start)
/* Check if the value in Y is used. */
{
    return (RegValUsed (Start) & REG_Y) != 0;
}



/*****************************************************************************/
/*   	       	   	    Real optimizer routines			     */
/*****************************************************************************/



static void OptCompares1 (void)
/* Try to optimize the integer compare subroutines. */
{
    Line*    L2[10];	     	/* Line lookahead */
    int      Cond;	      	/* Condition to evaluate */
    unsigned Label;	      	/* Local label number */
    unsigned Offs;		/* Stack offset */
    Line*    DelStart;		/* First line to delete */

    Line* L = FirstCode;
    while (L) {

	/* Search for compares of local byte sized variables. This looks
	 * like:
	 *
         *  	ldy     #$xx
	 *  	ldx	#$00
         *  	lda     (sp),y
         *  	jsr     pushax
         *  	ldy	#$yy
	 *  	ldx	#$00
         *  	lda     (sp),y
         *  	jsr     tosugtax
	 *
	 * Replace it by a direct compare:
	 *
	 *  	ldy	#$xx
	 *  	lda	(sp),y
	 *  	ldy	#$yy
	 *  	cmp	(sp),y
	 *  	jsr	boolugt
	 */
	if (LineMatch (L, "\tldy\t#$")					&&
	    GetNextCodeLines (L, L2, 7)					&&
	    LineFullMatch (L2[0], "\tldx\t#$00")			&&
	    LineFullMatch (L2[1], "\tlda\t(sp),y")			&&
	    LineFullMatch (L2[2], "\tjsr\tpushax")			&&
	    LineMatch	  (L2[3], "\tldy\t#$")				&&
	    LineFullMatch (L2[4], "\tldx\t#$00")			&&
	    LineFullMatch (L2[5], "\tlda\t(sp),y")			&&
	    (Cond = TosCmpFunc (L2[6])) >= 0) {

	    /* Get the stack offset and correct it, since we will remove
	     * the pushax.
	     */
	    Offs = GetHexNum (L2[3]->Line+7) - 2;

	    /* Replace it */
	    L = NewLineAfter (L, "\tlda\t(sp),y");
	    L =	NewLineAfter (L, "\tldy\t#$%02X", Offs);
	    L = NewLineAfter (L, "\tcmp\t(sp),y");
 	    L = NewLineAfter (L, "\tjsr\tbool%s", CmpSuffixTab[Cond]);

	    /* Remove the old cruft */
	    FreeLines (L2[0], L2[6]);
	}

	/* Compares of byte sized global variables */
	else if (LineFullMatch (L, "\tldx\t#$00")	 		&&
	    	 GetNextCodeLines (L, L2, 5)		 		&&
	    	 LineMatch     (L2[0], "\tlda\t")	 		&&
	    	 LineFullMatch (L2[1], "\tjsr\tpushax")	 		&&
	    	 LineFullMatch (L2[2], "\tldx\t#$00")	 		&&
	    	 LineMatch     (L2[3], "\tlda\t")	 		&&
	    	 (Cond = TosCmpFunc (L2[4])) >= 0) {

	    /* Replace it */
	    if (IsXAddrMode (L2[0])) {
		/* The load is X indirect, so we may not remove the load
		 * of the X register.
		 */
	    	L = L2[0];
	    	DelStart = L2[1];
	    } else {
	    	L = ReplaceLine  (L, L2[0]->Line);
	    	DelStart = L2[0];
	    }
	    L = NewLineAfter (L, "\tcmp\t%s", L2[3]->Line+5);
 	    L = NewLineAfter (L, "\tjsr\tbool%s", CmpSuffixTab[Cond]);

	    /* Remove the old cruft */
	    FreeLines (DelStart, L2[4]);

	}

	/* Byte sized local to global */
       	else if (LineMatch (L, "\tldy\t#$")  		 		&&
	    	 GetNextCodeLines (L, L2, 6)   		 		&&
       	       	 LineFullMatch (L2[0], "\tldx\t#$00")	 		&&
	    	 LineFullMatch (L2[1], "\tlda\t(sp),y")	 		&&
	    	 LineFullMatch (L2[2], "\tjsr\tpushax")	 		&&
	    	 LineFullMatch (L2[3], "\tldx\t#$00")	 		&&
	    	 LineMatch     (L2[4], "\tlda\t")	 		&&
	    	 (Cond = TosCmpFunc (L2[5])) >= 0) {

	    /* Replace it */
       	    L = NewLineAfter (L, L2[1]->Line);
	    L = NewLineAfter (L, "\tcmp\t%s", L2[4]->Line+5);
 	    L = NewLineAfter (L, "\tjsr\tbool%s", CmpSuffixTab[Cond]);

	    /* Remove the old cruft */
	    FreeLines (L2[0], L2[5]);

	}

	/* Byte sized global to local */
	else if (LineFullMatch (L, "\tldx\t#$00")	 		&&
	    	 GetNextCodeLines (L, L2, 6)		 		&&
	    	 LineMatch     (L2[0], "\tlda\t")	 		&&
	    	 LineFullMatch (L2[1], "\tjsr\tpushax")	 		&&
       	       	 LineMatch     (L2[2], "\tldy\t#$")			&&
		 LineFullMatch (L2[3], "\tldx\t#$00")			&&
		 LineFullMatch (L2[4], "\tlda\t(sp),y")			&&
		 (Cond = TosCmpFunc (L2[5])) >= 0) {

	    /* Get the stack offset and correct it, since we will remove
	     * the pushax.
	     */
	    Offs = GetHexNum (L2[2]->Line+7) - 2;

	    /* Replace it */
	    if (IsXAddrMode (L2[0])) {
		/* The load is X indirect, so we may not remove the load
		 * of the X register.
		 */
		L = L2[0];
		DelStart = L2[1];
	    } else {
	    	L = ReplaceLine  (L, L2[0]->Line);
		DelStart = L2[0];
	    }
	    L =	NewLineAfter (L, "\tldy\t#$%02X", Offs);
	    L = NewLineAfter (L, "\tcmp\t(sp),y");
 	    L = NewLineAfter (L, "\tjsr\tbool%s", CmpSuffixTab[Cond]);

	    /* Remove the old cruft */
	    FreeLines (DelStart, L2[5]);

	}

	/* Search for unsigned compares against global variables. This looks
	 * like:
	 *
         *  	jsr     pushax
         *  	lda     _b+0
         *  	ldx     _b+0+1
         *  	jsr     tosugtax
 	 *
       	 * Replace that by a direct compare:
 	 *
 	 *  	cpx	_b+0+1
 	 *  	bne	L
 	 *  	cmp	_b+0
 	 * L:
 	 *     	jsr	boolugt
 	 */
       	else if (LineFullMatch (L, "\tjsr\tpushax")    			&&
       	    	 GetNextCodeLines (L, L2, 3)				&&
 	    	 IsLoadAX (L2[0], L2[1]) 				&&
 	    	 (Cond = TosCmpFunc (L2[2])) >= 0			&&
       	       	 IsUnsignedCmp (Cond)) {

 	    /* Get a free label number */
 	    Label = AllocLabel ();

 	    /* Replace the code */
 	    L = ReplaceLine  (L, "\tcpx\t%s", L2[1]->Line+5);
 	    L = NewLineAfter (L, "\tbne\tL%04X", Label);
 	    L = NewLineAfter (L, "\tcmp\t%s", L2[0]->Line+5);
 	    L = NewLabelAfter(L, Label);
 	    L = NewLineAfter (L, "\tjsr\tbool%s", CmpSuffixTab[Cond]);

 	    /* Remove the old code */
 	    FreeLines (L2[0], L2[2]);

	}

     	L = NextCodeLine (L);
    }
}



static void OptDeadJumps (void)
/* Remove jumps to the following instruction */
{
    static const char* Jumps [] = {
	"\tbeq\tL",
	"\tbne\tL",
	"\tjeq\tL",
	"\tjne\tL",
	"\tjmp\tL",
	0
    };

    Line* L = FirstCode;
    while (L) {

	/* Get a pointer to the next instruction line */
	Line* NextLine = NextInstruction (L);

       	/* Is this line a jump? */
	int I = LineMatchX (L, Jumps);
	if (I >= 0) {
	    /* Yes. Get the target label, skip labels */
       	    Line* Target = NextInstruction (GetTargetLine (L->Line+5));

	    /* If the target label is the next line, remove the jump */
	    if (Target == NextLine) {
	 	FreeLine (L);
	    }
	}

	/* Go to the next line */
	L = NextLine;
    }
}



static void OptLoads (void)
/* Remove unnecessary loads of values */
{
    Line* L2 [10];

    Line* L = FirstCode;
    while (L) {

	/* Check for
	 *
	 *  	ldy	#$..
     	 *  	lda	(sp),y
	 *  	tax
	 *  	dey
	 *  	lda	(sp),y
	 *  	jsr	pushax
      	 *
	 * and replace it by
	 *
	 *  	ldy	#$..
	 *  	jsr	pushwysp
	 *
	 * or even
	 *
	 *  	jsr	pushw0sp
	 *
	 * This change will cost 3 cycles (one additional jump inside the
	 * subroutine), but it saves a lot of code (6 bytes per occurrence),
	 * so we will accept the overhead. It may even be possible to rewrite
	 * the library routine to get rid of the additional overhead.
	 */
	if (LineMatch (L, "\tldy\t#$")			&&
	    GetNextCodeLines (L, L2, 5)			&&
	    LineFullMatch (L2 [0], "\tlda\t(sp),y")	&&
	    LineFullMatch (L2 [1], "\ttax")		&&
	    LineFullMatch (L2 [2], "\tdey")		&&
	    LineFullMatch (L2 [3], "\tlda\t(sp),y")	&&
	    LineFullMatch (L2 [4], "\tjsr\tpushax")) {

	    /* Found - replace it */
	    if (LineFullMatch (L, "\tldy\t#$01")) {
	    	/* Word at offset zero */
	    	FreeLine (L);
       	       	L = ReplaceLine (L2 [4], "\tjsr\tpushw0sp");
	    } else {
	       	ReplaceLine (L2 [4], "\tjsr\tpushwysp");
	    }

	    /* Delete the remaining lines */
	    FreeLines (L2 [0], L2 [3]);

	/* Check for
	 *
	 *  	ldy  	#$xx
	 *  	lda	(sp),y
     	 *  	tax
	 *  	dey
      	 *  	lda	(sp),y
      	 *  	ldy	#$yy
      	 *  	jsr	ldauidx
      	 *
      	 * and replace it by
      	 *
      	 *  	ldy	#$xx
      	 *  	ldx	#$yy
      	 *  	jsr	ldauiysp
      	 *
      	 * or even
      	 *
      	 *  	jsr	ldaui0sp
      	 *
      	 * This change will cost 2 cycles, but it saves a lot of code (6 bytes
      	 * per occurrence), so we will accept the overhead. It may even be
      	 * possible to rewrite the library routine to get rid of the additional
      	 * overhead.
      	 */
       	} else if (LineMatch (L, "\tldy\t#$")  		&&
      	    GetNextCodeLines (L, L2, 6)			&&
      	    LineFullMatch (L2 [0], "\tlda\t(sp),y")	&&
      	    LineFullMatch (L2 [1], "\ttax")		&&
      	    LineFullMatch (L2 [2], "\tdey")		&&
      	    LineFullMatch (L2 [3], "\tlda\t(sp),y")	&&
       	    LineMatch     (L2 [4], "\tldy\t#$")		&&
      	    LineFullMatch (L2 [5], "\tjsr\tldauidx")) {

      	    /* Found - replace it */
       	    L2 [4]->Line [3] = 'x';		/* Change to ldx */
      	    if (LineFullMatch (L, "\tldy\t#$01")) {
      	    	/* Word at offset zero */
      	       	FreeLine (L);
      	    	L = ReplaceLine (L2 [5], "\tjsr\tldaui0sp");
      	    } else {
       	       	ReplaceLine (L2 [5], "\tjsr\tldauiysp");
      	    }

	    /* Delete the remaining lines */
	    FreeLines (L2 [0], L2 [3]);

	/* Search for:
     	 *
       	 *     	lda    	(sp),y
	 *     	jsr	pusha
	 *
       	 * And replace by
	 *
       	 *     	jsr    	pushaysp
	 */
       	} else if (LineFullMatch (L, "\tlda\t(sp),y")	&&
      	    GetNextCodeLines (L, L2, 1)	       	       	&&
      	    LineFullMatch (L2 [0], "\tjsr\tpusha")) {

	    /* Found, replace it */
	    L = ReplaceLine (L, "\tjsr\tpushaysp");
	    FreeLine (L2 [0]);

	/* Search for:
	 *
	 *     	ldx    	xx
	 *     	lda	yy
	 *    	sta	zzz
	 *    	stx	zzz+1
	 *
	 * and replace it by:
	 *
	 *     	lda    	xx
	 *    	sta	zzz+1
       	 *     	lda	yy
	 *    	sta	zzz
	 *
	 * provided that that the X register is not used later. While this is
	 * no direct optimization, it helps with other optimizations.
     	 */
       	} else if (LineMatch (L, "\tldx\t")		&&
       	    GetNextCodeLines (L, L2, 3)	 		&&
      	    LineMatch (L2 [0], "\tlda\t")		&&
	    Is16BitStore (L2[1], L2[2])			&&
	    !RegXUsed (L2[2])) {

      	    /* Found - replace it */
	    L->Line[3] = 'a';
	    NewLineAfter (L, "\tsta\t%s", L2[2]->Line+5);
	    FreeLine (L2[2]);
	    L = L2[1];

	/* Search for:
	 *
	 *     	ldx    	xx
	 *     	lda	yy
       	 *     	ldy	#$zz
	 *    	jsr	staxysp
	 *
	 * and replace it by:
	 *
	 *     	lda    	xx
	 *	ldy	#$zz+1
	 *    	sta	(sp),y
       	 *     	dey
	 *	lda	yy
	 *    	sta	(sp),y
	 *
	 * provided that that the X register is not used later. This code
	 * sequence is two bytes longer, but a lot faster and it does not
	 * use the X register, so other loads may get removed later.
     	 */
       	} else if (LineMatch (L, "\tldx\t")	  	&&
       	    GetNextCodeLines (L, L2, 3)	 	  	&&
      	    LineMatch (L2 [0], "\tlda\t")	  	&&
	    LineMatch (L2 [1], "\tldy\t#$")	  	&&
	    LineFullMatch (L2 [2], "\tjsr\tstaxysp")	&&
	    !RegXUsed (L2[2])) {

      	    /* Found - replace it */
	    L->Line[3] = 'a';
	    L = NewLineAfter (L, "\tldy\t#$%02X", GetHexNum (L2[1]->Line+7)+1);
	    L = NewLineAfter (L, "\tsta\t(sp),y");
	    L = NewLineAfter (L, "\tdey");
	    L = NewLineAfter (L2[0], "\tsta\t(sp),y");

	    /* Remove the remaining lines */
	    FreeLines (L2[1], L2[2]);
	}


       	/* All other patterns start with this one: */
	if (!LineFullMatch (L, "\tldx\t#$00")) {
	    /* Next line */
	    goto NextLine;
	}

	/* Search for:
	 *
	 *  	ldx   	#$00
	 *  	jsr   	pushax
	 *
	 * and replace it by:
	 *
	 *  	jsr   	pusha0
	 *
	 */
       	if (GetNextCodeLines (L, L2, 1)			&&
       	    LineFullMatch (L2 [0], "\tjsr\tpushax")) {

	    /* Replace the subroutine call */
       	    L = ReplaceLine (L, "\tjsr\tpusha0");

	    /* Remove the unnecessary line */
	    FreeLine (L2[0]);
	}

	/* Search for:
	 *
	 *  	ldx   	#$00
	 *  	lda   	...
	 *  	jsr   	pushax
	 *
	 * and replace it by:
	 *
	 *  	lda   	...
	 *  	jsr   	pusha0
	 *
	 */
       	else if (GetNextCodeLines (L, L2, 2)			&&
	         LineMatch (L2 [0], "\tlda\t")      	  	&&
 	         LineFullMatch (L2 [1], "\tjsr\tpushax")) {

	    /* Be sure, X is not used in the load */
	    if (NoXAddrMode (L2 [0])) {

	    	/* Replace the subroutine call */
	    	L2 [1] = ReplaceLine (L2 [1], "\tjsr\tpusha0");

     	    	/* Remove the unnecessary load */
	       	FreeLine (L);

	       	/* L must be valid */
	       	L = L2 [0];
	    }

	}

	/* Search for:
	 *
	 *     	ldx   	#$00
	 *     	lda   	...
	 *     	cmp   	#$..
	 *
	 * and replace it by:
	 *
	 *     	lda   	...
	 *     	cmp   	#$..
	 */
       	else if (GetNextCodeLines (L, L2, 2)		&&
	    	 LineMatch (L2 [0], "\tlda\t")		&&
	    	 LineMatch (L2 [1], "\tcmp\t#$")) {

	    /* Be sure, X is not used in the load */
	    if (NoXAddrMode (L2 [0])) {

	       	/* Remove the unnecessary load */
	       	FreeLine (L);

	       	/* L must be valid */
	       	L = L2 [0];
	    }
	}

	/* Search for:
	 *
	 *     	ldx   	#$00
	 *     	lda   	...
	 *     	jsr 	bnega
	 *
	 * and replace it by:
	 *
	 *     	lda   	...
	 *     	jsr	bnega
     	 */
	else if (GetNextCodeLines (L, L2, 2)		&&
	    	 LineMatch (L2 [0], "\tlda\t")		&&
       	         LineFullMatch (L2 [1], "\tjsr\tbnega")) {

	    /* Be sure, X is not used in the load */
 	    if (NoXAddrMode (L2 [0])) {

	       	/* Remove the unnecessary load */
	       	FreeLine (L);

	       	/* L must be valid */
	       	L = L2 [0];
	    }
	}

NextLine:
	/* Go to the next line */
	L = NextCodeLine (L);
    }
}



static void OptRegLoads (void)
/* Remove unnecessary loads of registers */
{
    unsigned Deletions;
    Line* L;
    Line* Lx;

    /* Repeat this until there is nothing more to delete */
    do {
	Deletions = 0;
	L = FirstCode;
	while (L) {

	    int Delete = 0;

	    /* Search for a load of X and check if the value is used later */
	    if (LineMatch (L, "\tldx\t") 		&&
	    	!RegXUsed (L)		 		&&
	    	!IsCondJump (NextInstruction (L))) {

	    	/* Remember to delete this line */
	    	Delete = 1;
	    }

	    /* Search for a load of A and check if the value is used later */
	    else if (LineMatch (L, "\tlda\t") 	       	&&
	    	       !RegAUsed (L)	 	       	&&
	    	       !IsCondJump (NextInstruction (L))) {

	    	/* Remember to delete this line */
	    	Delete = 1;
	    }

	    /* Search for a load of Y and check if the value is used later */
	    else if (LineMatch (L, "\tldy\t") 	       	&&
       	       	       !RegYUsed (L) 	       	       	&&
	     	       !IsCondJump (NextInstruction (L))) {

	    	/* Remember to delete this line */
	    	Delete = 1;
	    }

	    /* Go to the next line, delete the current if requested */
       	    Lx = L;
	    L = NextCodeLine (L);
	    if (Delete) {
	    	FreeLine (Lx);
	    	++Deletions;
	    }
	}
    } while (Deletions > 0);
}



static int OptPtrOps1 (Line** Start)
/* Optimize several pointer and array constructs - subfunction 1 */
{
    Line* L2[15];
    Line** L3;
    unsigned NeedLoad;
    unsigned LinesToRemove;
    unsigned Inc;
    unsigned Done;
    unsigned Offs;

    /* Use a local variable for the working line */
    Line* L = *Start;

    /* Search for (23B/XXT)
     *
     *      	lda     _b+0
     *      	ldx     _b+0+1
     *      	sta     regsave
     *      	stx     regsave+1
     *       	jsr     incax1
     *       	sta     _b+0
     *       	stx     _b+0+1
     *       	lda     regsave
     *       	ldx	regsave+1
     *
     * and replace it by something like (24B/26T)
     *
     *       	lda	_b+0
     *       	sta	regsave
     *       	clc
     *       	adc	#$01
     *       	sta	_b+0
     *       	lda	_b+0+1
     *       	sta	regsave+1
     *       	adc	#$00
     *       	sta	_b+0+1
     *       	tax
     *       	lda	regsave
     */
    if (!LineMatch (L, "\tlda\t")  				||
       	!GetNextCodeLines (L, L2, 4)				||
    	!IsLoadAX (L, L2 [0])	  				||
    	!LineFullMatch (L2[1], "\tsta\tregsave")		||
    	!LineFullMatch (L2[2], "\tstx\tregsave+1")) {

	/* Not found */
	return 0;
    }

    /* */
    if (LineMatch (L2[3], "\tjsr\tincax")) {
       	/* Get next code lines */
       	if (GetNextCodeLines (L2[3], &L2[4], 4) == 0) {
       	    /* Cannot get lines */
       	    return 0;
       	}
       	Inc = GetHexNum (L2[3]->Line+10);
       	L3 = &L2[4];
	LinesToRemove = 8;
    } else {
	/* Get next code lines */
	if (GetNextCodeLines (L2[3], &L2[4], 7) == 0) {
	    /* Cannot get lines */
	    return 0;
	}
	if (LineFullMatch (L2[3], "\tclc") 			&&
	    LineMatch (L2[4], "\tadc\t#$") 			&&
	    LineFullMatch (L2[5], "\tbcc\t*+3")			&&
	    LineFullMatch (L2[6], "\tinx")) {
	    /* Inlined increment */
	    Inc = GetHexNum (L2[4]->Line+7);
	    L3 = &L2[7];
	    LinesToRemove = 11;
	} else {
	    /* Not found */
	    return 0;
	}
    }

    /* Check for the remainder */
    if (!LineMatch (L3[0], "\tsta\t") 	   			||
	strcmp (L3[0]->Line+5, L->Line+5) != 0			||
	!LineMatch (L3[1], "\tstx\t") 	   			||
	strcmp (L3[1]->Line+5, L2[0]->Line+5) != 0		||
	!LineFullMatch (L3[2], "\tlda\tregsave")		||
	!LineFullMatch (L3[3], "\tldx\tregsave+1")) {

	/* Not found */
	return 0;
    }

    /* Check if AX is actually used following the code above. If not,
     * we don't need to load A/X from regsave. Since X will never be
     * used without A, check just for A.
     */
    NeedLoad = RegAUsed (L3[3]);

    /* Special code for register variables */
    Done = 0;
    if (LineMatch (L, "\tlda\tregbank+")   	&&
       	GetNextCodeLines (L3[3], &L3[4], 1)	&&
	Inc == 1) {

	/* Remember the offset into the register bank */
	char Reg[20];
	strcpy (Reg, L->Line+5);

	/* Check for several special sequences */
	if (LineFullMatch (L3[4], "\tjsr\tldaui")) {
	    /* Load char indirect */
	    L = ReplaceLine  (L, "\tldx\t#$00");
	    L = NewLineAfter (L, "\tlda\t(%s,x)", Reg);
	    L = NewLineAfter (L, "\tinc\t%s", Reg);
	    L = NewLineAfter (L, "\tbne\t*+4");
	    L = NewLineAfter (L, "\tinc\t%s+1", Reg);
	    Done = 1;
	    ++LinesToRemove;
	} else if (LineFullMatch (L3[4], "\tsta\tptr1")		&&
	     	   GetNextCodeLines (L3[4], &L3[5], 3)		&&
	     	   LineFullMatch (L3[5], "\tstx\tptr1+1")	&&
	     	   LineFullMatch (L3[6], "\tldx\t#$00")		&&
	     	   LineFullMatch (L3[7], "\tlda\t(ptr1,x)")) {

	    /* Load char indirect, inlined */
	    L = ReplaceLine  (L, "\tldx\t#$00");
	    L = NewLineAfter (L, "\tlda\t(%s,x)", Reg);
	    L = NewLineAfter (L, "\tinc\t%s", Reg);
	    L = NewLineAfter (L, "\tbne\t*+4");
	    L = NewLineAfter (L, "\tinc\t%s+1", Reg);
	    Done = 1;
	    LinesToRemove += 4;

	} else if (LineFullMatch (L3[4], "\tjsr\tpushax")) {
	    if (GetNextCodeLines (L3[4], &L3[5], 2) 	       	&&
	    	LineMatch      	 (L3[5], "\tlda\t")		&&
	    	LineFullMatch	 (L3[6], "\tjsr\tstaspp")) {

	    	/* Store to pointer */
	    	L = ReplaceLine  (L, L3[5]->Line);
	    	L = NewLineAfter (L, "\tldy\t#$00");
	    	L = NewLineAfter (L, "\tsta\t(%s),y", Reg);
	    	L = NewLineAfter (L, "\tinc\t%s", Reg);
	    	L = NewLineAfter (L, "\tbne\t*+4");
	    	L = NewLineAfter (L, "\tinc\t%s+1", Reg);

	    	Done = 1;
	    	LinesToRemove += 3;

	    } else if (GetNextCodeLines (L3[4], &L3[5], 3) 	&&
	    	       LineMatch     (L3[5], "\tldy\t#$")      	&&
	    	       LineFullMatch (L3[6], "\tlda\t(sp),y")	&&
	    	       LineFullMatch (L3[7], "\tjsr\tstaspp")) {

	    	/* Beware: We have to correct the stack offset, since we will
	    	 * remove the pushax instruction!
	    	 */
	    	Offs = GetHexNum (L3[5]->Line+7) - 2;

	    	/* Store to pointer */
	    	L = ReplaceLine  (L, "\tldy\t#$%02X", Offs);
	    	L = NewLineAfter (L, "\tldx\t#$00");
	    	L = NewLineAfter (L, "\tlda\t(sp),y");
	    	L = NewLineAfter (L, "\tsta\t(%s,x)", Reg);
	    	L = NewLineAfter (L, "\tinc\t%s", Reg);
	    	L = NewLineAfter (L, "\tbne\t*+4");
	    	L = NewLineAfter (L, "\tinc\t%s+1", Reg);

	    	Done = 1;
	    	LinesToRemove += 4;
	    }
	}
    }

    if (Done == 0) {

	/* No register variable - insert the first part of the code */
    	if (NeedLoad) {
    	    L = NewLineAfter (L, "\tsta\tptr1");
    	}
    	L = NewLineAfter (L, "\tclc");
    	L = NewLineAfter (L, "\tadc\t#$%02X", Inc);
    	L = NewLineAfter (L, "\tsta\t%s", L3[0]->Line+5);
    	L = NewLineAfter (L, "\tlda\t%s", L3[1]->Line+5);
    	if (NeedLoad) {
    	    L = NewLineAfter (L, "\tsta\tptr1+1");
    	}
    	L = NewLineAfter (L, "\tadc\t#$00");
    	L = NewLineAfter (L, "\tsta\t%s", L3[1]->Line+5);

    	/* Check if we must really load the old value into a/x or if the
    	 * code may be replaced by something else.
    	 */
    	if (GetNextCodeLines (L3[3], &L3[4], 1)) {
    	    if (LineFullMatch (L3[4], "\tjsr\tldaui")) {
    	     	/* Load char indirect */
    	     	L = NewLineAfter (L, "\tldx\t#$00");
	     	L = NewLineAfter (L, "\tlda\t(ptr1,x)");
	     	NeedLoad = 0;
	    	++LinesToRemove;
	    } else if (LineFullMatch (L3[4], "\tsta\tptr1")		&&
	    	       GetNextCodeLines (L3[4], &L3[5], 3)		&&
	    	       LineFullMatch (L3[5], "\tstx\tptr1+1")		&&
	    	       LineFullMatch (L3[6], "\tldx\t#$00")		&&
	    	       LineFullMatch (L3[7], "\tlda\t(ptr1,x)")) {

	    	/* Load char indirect, inlined */
	    	L = NewLineAfter (L, "\tldx\t#$00");
	    	L = NewLineAfter (L, "\tlda\t(ptr1,x)");
	    	NeedLoad = 0;
	    	LinesToRemove += 4;

	    } else if (LineFullMatch (L3[4], "\tjsr\tldaxi")) {
	     	/* Load word indirect */
	     	L = NewLineAfter (L, "\tldy\t#$01");
	     	L = NewLineAfter (L, "\tlda\t(ptr1),y");
	     	L = NewLineAfter (L, "\ttax");
    	     	L = NewLineAfter (L, "\tdey");
	     	L = NewLineAfter (L, "\tlda\t(ptr1),y");
	     	NeedLoad = 0;
	     	++LinesToRemove;

	    } else if (LineFullMatch (L3[4], "\tjsr\tpushax")) {
	     	if (GetNextCodeLines (L3[4], &L3[5], 2) 	       	&&
	     	    LineMatch      	 (L3[5], "\tlda\t")		&&
	     	    LineFullMatch	 (L3[6], "\tjsr\tstaspp")) {

	     	    /* Store to pointer */
	     	    L = NewLineAfter (L, L3[5]->Line);
	     	    L = NewLineAfter (L, "\tldy\t#$00");
	     	    L = NewLineAfter (L, "\tsta\t(ptr1),y");

	     	    NeedLoad = 0;
	     	    LinesToRemove += 3;
	     	} else if (GetNextCodeLines (L3[4], &L3[5], 3) 		&&
	     		   LineMatch     (L3[5], "\tldy\t#$")      	&&
	     		   LineFullMatch (L3[6], "\tlda\t(sp),y")	&&
	     		   LineFullMatch (L3[7], "\tjsr\tstaspp")) {

	     	    /* Beware: We have to correct the stack offset, since we will
	     	     * remove the pushax instruction!
	     	     */
	     	    sprintf (L3[5]->Line+7, "%02X", GetHexNum (L3[5]->Line+7)-2);

	     	    /* Store to pointer */
	     	    L = NewLineAfter (L, L3[5]->Line);
	     	    L = NewLineAfter (L, L3[6]->Line);
	     	    L = NewLineAfter (L, "\tldy\t#$00");
	     	    L = NewLineAfter (L, "\tsta\t(ptr1),y");

	     	    NeedLoad = 0;
	     	    LinesToRemove += 4;
	     	}

	    }
	}

	/* If we need to load a/x, add the code */
	if (NeedLoad) {
	    L = NewLineAfter (L, "\tlda\tptr1");
	    L = NewLineAfter (L, "\tldx\tptr1+1");
	}
    }

    /* Remove the code that is no longer needed */
    FreeLines (L2[0], L2[LinesToRemove-1]);

    /* Return the new line and success */
    *Start = NextCodeLine (L);
    return 1;
}



static int OptPtrOps2 (Line** Start)
/* Optimize several pointer and array constructs - subfunction 2 */
{
    Line* L2[25];
    Line** L3;
    unsigned NeedLoad;
    unsigned LinesToRemove;
    unsigned Inc;
    unsigned Offs;


    /* Use a local variable for the working line */
    Line* L = *Start;

    /* Same as subfunction 1 but for local variables. */
    if (LineMatch (L, "\tldy\t#$") == 0) {
	return 0;
    }

    /* Get the stack offset. The offset points to the high byte, correct that. */
    Offs = GetHexNum (L->Line+7) - 1;

    /* Check for the actual sequences */
    if (GetNextCodeLines (L, L2, 7)				&&
	LineFullMatch (L2[0], "\tjsr\tldaxysp")			&&
	LineFullMatch (L2[1], "\tsta\tregsave")			&&
	LineFullMatch (L2[2], "\tstx\tregsave+1")		&&
	LineMatch     (L2[3], "\tjsr\tincax")) {

	/* Non inlined version */
	Inc = GetHexNum (L2[3]->Line+10);

	/* Check for stack offset zero */
       	if (LineFullMatch (L2[4], "\tjsr\tstax0sp")		&&
     	    LineFullMatch (L2[5], "\tlda\tregsave")		&&
     	    LineFullMatch (L2[6], "\tldx\tregsave+1")) {

	    LinesToRemove = 7;

	} else if (GetNextCodeLines (L2[6], &L2[7], 1)		&&
	    	   LineMatch     (L2[4], "\tldy\t#$")   	&&
	    	   GetHexNum     (L2[4]->Line+7) == Offs  	&&
     	    	   LineFullMatch (L2[5], "\tjsr\tstaxysp")	&&
     	    	   LineFullMatch (L2[6], "\tlda\tregsave")	&&
     	    	   LineFullMatch (L2[7], "\tldx\tregsave+1")) {

     	    LinesToRemove = 8;

	} else {
	    /* Not found */
	    return 0;
	}

    } else if (GetNextCodeLines (L, L2, 13)    	       	       	&&
	       LineFullMatch (L2[0], "\tlda\t(sp),y")		&&
	       LineFullMatch (L2[1], "\ttax")			&&
	       LineFullMatch (L2[2], "\tdey")			&&
	       LineFullMatch (L2[3], "\tlda\t(sp),y")		&&
	       LineFullMatch (L2[4], "\tsta\tregsave")		&&
	       LineFullMatch (L2[5], "\tstx\tregsave+1")	&&
	       LineFullMatch (L2[6], "\tclc")			&&
	       LineMatch     (L2[7], "\tadc\t#$") 		&&
	       LineFullMatch (L2[8], "\tbcc\t*+3")		&&
     	       LineFullMatch (L2[9], "\tinx")) {

     	/* Inlined version */
     	Inc = GetHexNum (L2[7]->Line+7);

	/* Check for stack offset zero */
       	if (LineFullMatch (L2[10], "\tjsr\tstax0sp")		&&
     	    LineFullMatch (L2[11], "\tlda\tregsave")		&&
     	    LineFullMatch (L2[12], "\tldx\tregsave+1")) {

	    LinesToRemove = 13;

	} else if (GetNextCodeLines (L2[12], &L2[13], 1)	&&
	   	   LineMatch     (L2[10], "\tldy\t#$")   	&&
	    	   GetHexNum     (L2[10]->Line+7) == Offs  	&&
     	    	   LineFullMatch (L2[11], "\tjsr\tstaxysp")	&&
     	     	   LineFullMatch (L2[12], "\tlda\tregsave")	&&
     	    	   LineFullMatch (L2[13], "\tldx\tregsave+1")) {

     	    LinesToRemove = 14;

	} else {
	    /* Not found */
	    return 0;
	}
    } else {
     	/* Not found */
     	return 0;
    }

    /* Get a pointer to the last line of the preceding sequence */
    L3 = &L2[LinesToRemove-1];

    /* Check if AX is actually used following the code above. If not,
     * we don't need to load A/X from regsave. Since X will never by
     * used without A, check just for A.
     */
    NeedLoad = RegAUsed (L3[0]);

    /* Replace the ldy instruction, offset must point to the low byte */
    sprintf (L->Line+7, "%02X", Offs);

    /* Insert the first part of the code */
    L = NewLineAfter (L, "\tlda\t(sp),y");
    if (NeedLoad) {
	L = NewLineAfter (L, "\tsta\tptr1");
    }
    L = NewLineAfter (L, "\tclc");
    L = NewLineAfter (L, "\tadc\t#$%02X", Inc);
    L = NewLineAfter (L, "\tsta\t(sp),y");
    L = NewLineAfter (L, "\tiny");
    L = NewLineAfter (L, "\tlda\t(sp),y");
    if (NeedLoad) {
	L = NewLineAfter (L, "\tsta\tptr1+1");
    }
    L = NewLineAfter (L, "\tadc\t#$00");
    L = NewLineAfter (L, "\tsta\t(sp),y");

    /* Check if we must really load the old value into a/x or if the
     * code may be replaced by something else.
     */
    if (GetNextCodeLines (L3[0], &L3[1], 1)) {
	if (LineFullMatch (L3[1], "\tjsr\tldaui")) {
	    /* Load char indirect */
	    L = NewLineAfter (L, "\tldx\t#$00");
	    L = NewLineAfter (L, "\tlda\t(ptr1,x)");
	    NeedLoad = 0;
	    ++LinesToRemove;
	} else if (LineFullMatch (L3[1], "\tsta\tptr1")	       	&&
	   	   GetNextCodeLines (L3[1], &L3[2], 3)		&&
	   	   LineFullMatch (L3[2], "\tstx\tptr1+1")	&&
	   	   LineFullMatch (L3[3], "\tldx\t#$00")		&&
	   	   LineFullMatch (L3[4], "\tlda\t(ptr1,x)")) {

	    /* Load char indirect, inlined */
	    L = NewLineAfter (L, "\tldx\t#$00");
	    L = NewLineAfter (L, "\tlda\t(ptr1,x)");
	    NeedLoad = 0;
	    LinesToRemove += 4;

	} else if (LineFullMatch (L3[1], "\tjsr\tldaxi")) {
	    /* Load word indirect */
	    L = NewLineAfter (L, "\tldy\t#$01");
	    L = NewLineAfter (L, "\tlda\t(ptr1),y");
	    L = NewLineAfter (L, "\ttax");
	    L = NewLineAfter (L, "\tdey");
	    L = NewLineAfter (L, "\tlda\t(ptr1),y");
	    NeedLoad = 0;
       	    ++LinesToRemove;

       	} else if (LineFullMatch (L3[1], "\tjsr\tpushax")) {
	    if (GetNextCodeLines (L3[1], &L3[2], 2) 	       	&&
	       	LineMatch      	 (L3[2], "\tlda\t")		&&
	     	LineFullMatch	 (L3[3], "\tjsr\tstaspp")) {

	     	/* Store to pointer */
	     	L = NewLineAfter (L, L3[2]->Line);
	     	L = NewLineAfter (L, "\tldy\t#$00");
	     	L = NewLineAfter (L, "\tsta\t(ptr1),y");

	     	NeedLoad = 0;
	     	LinesToRemove += 3;
	    } else if (GetNextCodeLines (L3[1], &L3[2], 3) 	&&
	     	       LineMatch     (L3[2], "\tldy\t#$")      	&&
	       	       LineFullMatch (L3[3], "\tlda\t(sp),y")	&&
	     	       LineFullMatch (L3[4], "\tjsr\tstaspp")) {

	     	/* Beware: We have to correct the stack offset, since we will
	     	 * remove the pushax instruction!
	     	 */
	       	sprintf (L3[2]->Line+7, "%02X", GetHexNum (L3[2]->Line+7)-2);

	     	/* Store to pointer */
	     	L = NewLineAfter (L, L3[2]->Line);
		L = NewLineAfter (L, L3[3]->Line);
	     	L = NewLineAfter (L, "\tldy\t#$00");
	     	L = NewLineAfter (L, "\tsta\t(ptr1),y");

	     	NeedLoad = 0;
	     	LinesToRemove += 4;
	    }
	}

    }

    /* If we need to load a/x, add the code */
    if (NeedLoad) {
	L = NewLineAfter (L, "\tlda\tptr1");
	L = NewLineAfter (L, "\tldx\tptr1+1");
    }

    /* Remove the code that is no longer needed */
    FreeLines (L2[0], L2[LinesToRemove-1]);

    /* Return the new line and success */
    *Start = NextCodeLine (L);
    return 1;
}



static void OptPtrOps (void)
/* Optimize several pointer and array constructs */
{
    Line* L2 [10];

    Line* L = FirstCode;
    while (L) {

       	if (OptPtrOps1 (&L)) {
	    continue;
	} else if (OptPtrOps2 (&L)) {
	    continue;
	}

       	/* Search for the following sequence:
	 *
	 *     	lda	regsave
	 *     	ldx	regsave+1
	 *     	jsr	pushax
	 *     	lda	#$..
	 *     	jsr	staspp
	 *
	 * and replace it by:
	 *
	 *     	lda	#$..
 	 *     	ldy	#$00
	 *     	sta	(regsave),y
	 *
	 */
       	else if (LineFullMatch (L, "\tlda\tregsave")		&& /* Match on start */
       	    	 GetNextCodeLines (L, L2, 4)        	       	&& /* Fetch next lines */
       	    	 LineFullMatch (L2 [0], "\tldx\tregsave+1")  	&& /* Match line 2 ... */
 	    	 LineFullMatch (L2 [1], "\tjsr\tpushax")	&&
       	    	 LineMatch (L2 [2], "\tlda\t#$")		&&
       	    	 LineFullMatch (L2 [3], "\tjsr\tstaspp")) {

	    /* Found the sequence, replace it */
	    L      = ReplaceLine (L, L2 [2]->Line);		/* lda #$.. */
	    L2 [0] = ReplaceLine (L2 [0], "\tldy\t#$00");
	    L2 [1] = ReplaceLine (L2 [1], "\tsta\t(regsave),y");

	    /* Free the remaining lines */
	    FreeLines (L2 [2], L2 [3]);
      	}

       	/* Search for the following sequence:
	 *
     	 *     	lda	regsave
	 *     	ldx	regsave+1
	 *     	jsr	ldaui
	 *
	 * and replace it by:
	 *
	 *     	ldx    	#$00
 	 *     	lda    	(regsave,x)
	 *
	 */
       	else if (LineFullMatch (L, "\tlda\tregsave")	    && /* Match on start */
       	    	 GetNextCodeLines (L, L2, 2)        	    && /* Fetch next lines */
       	    	 LineFullMatch (L2 [0], "\tldx\tregsave+1") && /* Match line 2 ... */
 	    	 LineFullMatch (L2 [1], "\tjsr\tldaui")) {

	    /* Found the sequence, replace it */
	    L      = ReplaceLine (L, "\tldx\t#$00");
	    L2 [0] = ReplaceLine (L2 [0], "\tlda\t(regsave,x)");

	    /* Free the remaining lines */
	    FreeLine (L2 [1]);
      	}

	/*
	 * Search for the following sequence:
	 *
	 *     	lda 	regsave
	 *     	ldx  	regsave+1
	 *     	jsr 	pushax
	 *     	ldx 	#$high
	 *     	lda 	#$low
	 *     	jsr 	staxspp
	 *
	 * and replace it by:
	 *
	 *     	ldy 	#$01
	 *     	lda 	#$high
	 *    	sta 	(regsave),y
	 *    	tax
	 *    	dey
	 *    	lda 	#$low
	 *    	sta 	(regsave),y
	 *
	 */
      	else if (LineFullMatch (L, "\tlda\tregsave")   	    &&
       	         GetNextCodeLines (L, L2, 5)           	    &&
       	    	 LineFullMatch (L2 [0], "\tldx\tregsave+1") &&
      	    	 LineFullMatch (L2 [1], "\tjsr\tpushax")    &&
       	    	 LineMatch (L2 [2], "\tldx\t#$")	    &&
       	    	 LineMatch (L2 [3], "\tlda\t#$")	    &&
       	    	 LineFullMatch (L2 [4], "\tjsr\tstaxspp")) {

	    /* Found the sequence, replace it */
	    L      = ReplaceLine (L, "\tldy\t#$01");
	    L2 [0] = ReplaceLine (L2 [0], L2 [2]->Line);
	    L2 [0]->Line [3] = 'a';
	    L2 [1] = ReplaceLine (L2 [1], "\tsta\t(regsave),y");
	    L2 [4] = ReplaceLine (L2 [4], L2 [3]->Line);
	    L2 [2] = ReplaceLine (L2 [2], "\ttax");
	    L2 [3] = ReplaceLine (L2 [3], "\tdey");
	    L      = NewLineAfter (L2 [4], "\tsta\t(regsave),y");
	}

	/*
	 * Search for the following sequence:
	 *
       	 *     	lda  	regsave
	 *    	ldx    	regsave+1
	 *    	sta  	ptr1
	 *    	stx  	ptr1+1
	 *    	ldx  	#$00
	 *    	lda  	(ptr1,x)
	 *
	 * and replace it by:
	 *
	 *   	ldx    	#$00
	 *     	lda  	(regsave,x)
	 *
	 */
       	else if (LineFullMatch (L, "\tlda\tregsave")   	    &&
       	         GetNextCodeLines (L, L2, 5)        	    &&
       	    	 LineFullMatch (L2 [0], "\tldx\tregsave+1") &&
       	    	 LineFullMatch (L2 [1], "\tsta\tptr1")	    &&
       	       	 LineFullMatch (L2 [2], "\tstx\tptr1+1")    &&
	    	 LineFullMatch (L2 [3], "\tldx\t#$00") 	    &&
       	    	 LineFullMatch (L2 [4], "\tlda\t(ptr1,x)")) {

	    /* Found the sequence, replace it */
	    L      = ReplaceLine (L, "\tldx\t#$00");
	    L2 [0] = ReplaceLine (L2 [0], "\tlda\t(regsave,x)");

	    /* Remove the remaining lines */
 	    FreeLines (L2 [1], L2 [4]);
	}

       	/* Search for the following sequence:
	 *
	 *     	jsr	pushax
	 *     	lda	...
	 *     	jsr	staspp
	 *
	 * and replace it by:
	 *
	 *  	sta	ptr1
	 *     	stx    	ptr1+1
	 *     	lda	...
	 *  	ldy	#$00
	 *  	sta	(ptr1),y
	 *
	 */
       	else if (LineFullMatch (L, "\tjsr\tpushax")	    &&
       	         GetNextCodeLines (L, L2, 2)        	    &&
       	         LineMatch (L2 [0], "\tlda\t")  	    &&
       	         LineFullMatch (L2 [1], "\tjsr\tstaspp")) {

	    /* Found the sequence, replace it */
	    L      = ReplaceLine (L, "\tsta\tptr1");
       	    L2 [1] = ReplaceLine (L2 [1], L2 [0]->Line);   /* lda ... */
	    L2 [0] = ReplaceLine (L2 [0], "\tstx\tptr1+1");
	    L2 [2] = NewLineAfter (L2 [1], "\tldy\t#$00");
       	    L      = NewLineAfter (L2 [2], "\tsta\t(ptr1),y");
      	}

       	/* Search for the following sequence:
	 *
	 *     	jsr	pushax
	 *     	lda	...
	 *	ldy	#$nn
	 *     	jsr	staspidx
	 *
	 * and replace it by:
	 *
	 *  	sta	ptr1
	 *  	stx	ptr1+1
	 *     	lda	...
     	 *  	ldy	#$nn
	 *  	sta	(ptr1),y
	 *
	 */
       	else if (LineFullMatch (L, "\tjsr\tpushax")	    &&
       	         GetNextCodeLines (L, L2, 3)        	    &&
       	         LineMatch (L2 [0], "\tlda\t")  	    &&
		 LineMatch (L2 [1], "\tldy\t#$")	    &&
       	         LineFullMatch (L2 [2], "\tjsr\tstaspidx")) {

	    /* Found the sequence, replace it */
	    L      = ReplaceLine (L, "\tsta\tptr1");
	    L      = NewLineAfter (L, "\tstx\tptr1+1");
       	    L2 [2] = ReplaceLine (L2 [2], "\tsta\t(ptr1),y");
      	}

       	/* Search for the following sequence:
	 *
	 *     	jsr	pushax
	 *  	ldy	#$..
	 *     	lda	(sp),y
	 *     	jsr	staspp
	 *
	 * and replace it by:
	 *
	 *  	sta	ptr1
	 *  	stx	ptr1+1
 	 *  	ldy  	#$..
	 *     	lda	(sp),y
	 *  	ldy	#$00
	 *  	sta	(ptr1),y
	 *
	 * Beware: Since we remove a call to a function that changes the stack
	 * pointer, we have to adjust the stack address for the lda.
	 *
	 */
       	else if (LineFullMatch (L, "\tjsr\tpushax")    	    &&
       	         GetNextCodeLines (L, L2, 3)        	    &&
	    	 LineMatch (L2 [0], "\tldy\t#$")	    &&
       	    	 LineFullMatch (L2 [1], "\tlda\t(sp),y")    &&
       	    	 LineFullMatch (L2 [2], "\tjsr\tstaspp")) {

	    /* Found the sequence, replace it. First create a new load
	     * instruction for the changed stack offset.
	     */
     	    char Buf [30];
	    sprintf (Buf, "\tldy\t#$%02X", GetHexNum (L2 [0]->Line+7) - 2);
	    L      = ReplaceLine (L, "\tsta\tptr1");
       	    L2 [1] = ReplaceLine (L2 [1], Buf);   /* ldy ... */
	    L2 [0] = ReplaceLine (L2 [0], "\tstx\tptr1+1");
	    L2 [2] = ReplaceLine (L2 [2], "\tlda\t(sp),y");
       	    L2 [3] = NewLineAfter (L2 [2], "\tldy\t#$00");
	    L      = NewLineAfter (L2 [3], "\tsta\t(ptr1),y");
      	}

       	/* Search for the following sequence:
	 *
	 *     	jsr	pushax
	 *  	ldy	#$nn
	 *     	lda	(sp),y
	 *	ldy	#$mm
	 *     	jsr	staspidx
	 *
	 * and replace it by:
	 *
	 *  	sta	ptr1
	 *  	stx	ptr1+1
 	 *  	ldy	#$nn
	 *     	lda	(sp),y
	 *  	ldy	#$mm
	 *  	sta	(ptr1),y
	 *
	 * Beware: Since we remove a call to a function that changes the stack
	 * pointer, we have to adjust the stack address for the lda.
	 *
	 */
       	else if (LineFullMatch (L, "\tjsr\tpushax")    	    &&
       	         GetNextCodeLines (L, L2, 4)        	    &&
	    	 LineMatch (L2 [0], "\tldy\t#$")	    &&
       	    	 LineFullMatch (L2 [1], "\tlda\t(sp),y")    &&
		 LineMatch (L2 [2], "\tldy\t#$")	    &&
       	    	 LineFullMatch (L2 [3], "\tjsr\tstaspidx")) {

	    /* Found the sequence, replace it. First create a new load
	     * instruction for the changed stack offset.
	     */
	    char Buf [30];
	    sprintf (Buf, "\tldy\t#$%02X", GetHexNum (L2 [0]->Line+7) - 2);
	    L      = ReplaceLine (L, "\tsta\tptr1");
	    L	   = NewLineAfter (L, "\tstx\tptr1+1");
       	    L2 [0] = ReplaceLine (L2 [0], Buf);   /* ldy ... */
	    L2 [3] = ReplaceLine (L2 [3], "\tsta\t(ptr1),y");
      	}

       	/* Search for the following sequence:
	 *
       	 *     	ldax  	_label+0
	 *  	ldy   	#$..
	 *  	clc
	 *     	adc   	(sp),y
	 *     	bcc   	*+3
	 *   	inx
	 *  	sta   	ptr1
	 *   	stx   	ptr1+1
	 *  	ldx   	#$00
	 *  	lda   	(ptr1,x)
	 *
	 * and replace it by:
	 *
	 *  	ldy   	#$..
	 *  	lda   	(sp),y
 	 *  	tay
	 *  	ldx   	#$00
	 *  	lda   	_label+0,y
	 *
	 * The load of X may be omitted if X is not used below.
	 */
       	else if (LineMatch (L, "\tldax\t_")	    	    &&
       	         GetNextCodeLines (L, L2, 9)        	    &&
	    	 LineMatch (L2 [0], "\tldy\t#$")	    &&
       	    	 LineFullMatch (L2 [1], "\tclc")    	    &&
       	    	 LineFullMatch (L2 [2], "\tadc\t(sp),y")    &&
	    	 LineFullMatch (L2 [3], "\tbcc\t*+3")	    &&
	    	 LineFullMatch (L2 [4], "\tinx")	    &&
	    	 LineFullMatch (L2 [5], "\tsta\tptr1")	    &&
	    	 LineFullMatch (L2 [6], "\tstx\tptr1+1")    &&
	    	 LineFullMatch (L2 [7], "\tldx\t#$00")	    &&
	    	 LineFullMatch (L2 [8], "\tlda\t(ptr1,x)")) {

	    /* Found the sequence, replace it */
	    char Label [256];
	    strcpy (Label, L->Line + 6);       		/* Remember the label */
	    L = ReplaceLine  (L, L2 [0]->Line);		/* ldy .. */
	    L = NewLineAfter (L, "\tlda\t(sp),y");
       	    L = NewLineAfter (L, "\ttay");
	    if (RegXUsed (L2[8])) {
	    	L = NewLineAfter (L, "\tldx\t#$00");
	    }
       	    L = NewLineAfter (L, "\tlda\t%s,y", Label);

	    /* Remove the remaining stuff. There may be hints between the
	     * instructions, remove them too
	     */
       	    FreeLines (L2[0], L2 [8]);

      	}

	/* Check for
	 *
	 *	ldy	#$xx
	 *	lda	(sp),y
	 *	tax
	 *	dey
      	 *	lda	(sp),y
      	 *	ldy	#$yy
      	 *	jsr	ldauidx
      	 *
      	 * and replace it by
      	 *
      	 *	ldy	#$xx
      	 *  	ldx	#$yy
      	 *	jsr	ldauiysp
      	 *
      	 * or even
      	 *
      	 *	jsr	ldaui0sp
      	 *
      	 * This change will cost 2 cycles, but it saves a lot of code (6 bytes
      	 * per occurrence), so we will accept the overhead. It may even be
      	 * possible to rewrite the library routine to get rid of the additional
      	 * overhead.
      	 */
       	else if (LineMatch (L, "\tldy\t#$")			&&
      	    	 GetNextCodeLines (L, L2, 6)			&&
      	    	 LineFullMatch (L2 [0], "\tlda\t(sp),y")	&&
      	    	 LineFullMatch (L2 [1], "\ttax")	       	&&
      	    	 LineFullMatch (L2 [2], "\tdey")		&&
      	    	 LineFullMatch (L2 [3], "\tlda\t(sp),y")	&&
       	    	 LineMatch     (L2 [4], "\tldy\t#$")		&&
      	    	 LineFullMatch (L2 [5], "\tjsr\tldauidx")) {

      	    /* Found - replace it */
       	    L2 [4]->Line [3] = 'x';		/* Change to ldx */
      	    if (LineFullMatch (L, "\tldy\t#$01")) {
      	    	/* Word at offset zero */
      	    	FreeLine (L);
      	    	L = ReplaceLine (L2 [5], "\tjsr\tldaui0sp");
      	    } else {
       	       	ReplaceLine (L2 [5], "\tjsr\tldauiysp");
      	    }

	    /* Delete the remaining lines */
	    FreeLines (L2 [0], L2 [3]);
	}

	/* Check for
	 *
       	 *     	ldy	#$xx
	 *  	lda	(sp),y
	 *  	tax
	 *  	dey
      	 *  	lda	(sp),y
      	 *  	sta	ptr1
	 *     	stx	ptr1+1
	 *  	ldx	#$00
	 *  	lda	(ptr1,x)
      	 *
      	 * and replace it by
      	 *
      	 *  	ldy	#$xx
      	 *  	jsr	ldau0ysp
      	 *
      	 * or even
      	 *
      	 *  	jsr	ldau00sp
      	 *
      	 * This change will has an overhead of 10 cycles, but it saves 11(!)
	 * bytes per invocation. Maybe we should apply only if FavourSize is
	 * true?
      	 */
       	else if (LineMatch (L, "\tldy\t#$")			&&
      	    	 GetNextCodeLines (L, L2, 8)			&&
      	    	 LineFullMatch (L2 [0], "\tlda\t(sp),y")	&&
      	    	 LineFullMatch (L2 [1], "\ttax")		&&
      	    	 LineFullMatch (L2 [2], "\tdey")		&&
      	    	 LineFullMatch (L2 [3], "\tlda\t(sp),y")	&&
	    	 LineFullMatch (L2 [4], "\tsta\tptr1")		&&
	    	 LineFullMatch (L2 [5], "\tstx\tptr1+1")	&&
	    	 LineFullMatch (L2 [6], "\tldx\t#$00") 		&&
	    	 LineFullMatch (L2 [7], "\tlda\t(ptr1,x)")) {

      	    /* Found - replace it */
      	    if (LineFullMatch (L, "\tldy\t#$01")) {
      	    	/* Word at offset zero */
      	    	FreeLine (L);
       	       	L = ReplaceLine (L2 [0], "\tjsr\tldau00sp");
      	    } else {
       	       	ReplaceLine (L2 [0], "\tjsr\tldau0ysp");
      	    }

	    /* Delete the remaining lines */
	    FreeLines (L2 [1], L2 [7]);
	}

	/* Check for
	 *
       	 *     	ldy	#$xx
	 *  	lda	(sp),y
	 *  	tax
	 *  	dey
      	 *  	lda	(sp),y
      	 *  	sta	yyy
	 *     	stx	yyy+1
      	 *
      	 * and replace it by
      	 *
       	 *     	ldy	#$xx
	 *  	lda	(sp),y
	 *  	sta	yyy+1
	 *  	dey
      	 *  	lda	(sp),y
      	 *  	sta	yyy
      	 *
      	 * Provided that X is not used later.
      	 */
       	else if (LineMatch (L, "\tldy\t#$")			&&
      	    	 GetNextCodeLines (L, L2, 6)   			&&
      	    	 LineFullMatch (L2 [0], "\tlda\t(sp),y")       	&&
      	    	 LineFullMatch (L2 [1], "\ttax")      		&&
      	    	 LineFullMatch (L2 [2], "\tdey")      		&&
      	    	 LineFullMatch (L2 [3], "\tlda\t(sp),y")	&&
	       	 Is16BitStore (L2[4], L2[5])			&&
		 !RegXUsed (L2[5])) {

      	    /* Found - replace it */
	    L2[1] = ReplaceLine (L2[1], L2[5]->Line);
	    L2[1]->Line[3] = 'a';

	    /* Delete the remaining lines */
	    FreeLine (L2[5]);

	    /* Start over at last line */
	    L = L2[4];
	}

	/* Next Line */
	L = NextCodeLine (L);
    }
}



static void OptRegVars (void)
/* Optimize register variable uses */
{
    Line* L2 [10];

    Line* L = FirstCode;
    while (L) {

       	/* Search for the following sequence:
	 *
       	 *     	lda    	regbank+n
	 *     	ldx	regbank+n+1
	 *     	jsr	ldaui
	 *
	 * and replace it by:
	 *
	 *     	ldx    	#$00
       	 *     	lda    	(regbank+n,x)
	 *
     	 */
       	if (LineMatch (L, "\tlda\tregbank+")		&& /* Match on start */
       	    GetNextCodeLines (L, L2, 2)        	       	&& /* Fetch next lines */
       	    LineMatch (L2 [0], "\tldx\tregbank+")       && /* Match line 2 ... */
 	    LineFullMatch (L2 [1], "\tjsr\tldaui")      &&
       	    L->Line [13] == L2 [0]->Line [13]	       	&& /* Offset equal */
	    strcmp (L2 [0]->Line + 14, "+1") == 0) {

	    char Buf [100];
	    sprintf (Buf, "\tlda\t(%s,x)", L->Line + 5);

	    /* Found the sequence, replace it */
       	    L      = ReplaceLine (L, "\tldx\t#$00");
	    L2 [0] = ReplaceLine (L2 [0], Buf);

	    /* Free the remaining lines */
	    FreeLine (L2 [1]);
      	}

       	/* Search for the following sequence:
	 *
       	 *     	lda    	regbank+n
	 *     	ldx	regbank+n+1
	 *	sta	ptr1
	 *	stx	ptr1+1
	 *	ldx 	#$00
	 *     	lda	(ptr1,x)
	 *
	 * and replace it by:
	 *
	 *     	ldx    	#$00
       	 *     	lda    	(regbank+n,x)
	 *
	 */
       	else if (LineMatch (L, "\tlda\tregbank+")      	 && /* Match on start */
       	    	 GetNextCodeLines (L, L2, 5)           	 && /* Fetch next lines */
       	    	 LineMatch (L2 [0], "\tldx\tregbank+")   && /* Match line 2 ... */
       	    	 L->Line [13] == L2 [0]->Line [13]     	 && /* Offset equal */
	    	 strcmp (L2 [0]->Line + 14, "+1") == 0 	 &&
	    	 LineFullMatch (L2 [1], "\tsta\tptr1") 	 &&
	  	 LineFullMatch (L2 [2], "\tstx\tptr1+1") &&
	  	 LineFullMatch (L2 [3], "\tldx\t#$00") 	 &&
	  	 LineFullMatch (L2 [4], "\tlda\t(ptr1,x)")) {

	    char Buf [100];
     	    sprintf (Buf, "\tlda\t(%s,x)", L->Line + 5);

	    /* Found the sequence, replace it */
       	    L      = ReplaceLine (L, "\tldx\t#$00");
	    L2 [0] = ReplaceLine (L2 [0], Buf);

	    /* Free the remaining lines */
	    FreeLines (L2 [1], L2 [4]);
      	}

       	/* Search for the following sequence:
	 *
       	 *     	lda    	regbank+n
	 *     	ldx	regbank+n+1
	 *	ldy	#$..
	 *  	jsr	ldauidx
	 *
	 * and replace it by:
	 *
	 *	ldy	#$..
	 *     	ldx    	#$00
       	 *     	lda    	(regbank+n),y
	 *
	 */
       	else if (LineMatch (L, "\tlda\tregbank+")      	 && /* Match on start */
       	    	 GetNextCodeLines (L, L2, 3)             && /* Fetch next lines */
       	    	 LineMatch (L2 [0], "\tldx\tregbank+")   && /* Match line 2 ... */
       	    	 L->Line [13] == L2 [0]->Line [13]     	 && /* Offset equal */
	    	 strcmp (L2 [0]->Line + 14, "+1") == 0 	 &&
	    	 LineMatch (L2 [1], "\tldy\t#$")       	 &&
	       	 LineFullMatch (L2 [2], "\tjsr\tldauidx")) {

	    char Buf [100];
	    sprintf (Buf, "\tlda\t(%s),y", L->Line + 5);

	    /* Found the sequence, replace it */
       	    L      = ReplaceLine (L, L2 [1]->Line);
	    L2 [0] = ReplaceLine (L2 [0], "\tldx\t#$00");
	    L2 [1] = ReplaceLine (L2 [1], Buf);

	    /* Free the remaining lines */
	    FreeLine (L2 [2]);
      	}

       	/* Search for the following sequence:
     	 *
       	 *     	lda    	regbank+n
	 *     	ldx	regbank+n+1
	 *  	sta	ptr1
	 *  	stx	ptr1+1
	 *     	lda	...
	 *  	ldy	#$mm
	 *  	sta	(ptr1),y
	 *
	 * and replace it by:
	 *
	 *     	lda    	...
	 *	ldy	#$mm
       	 *     	sta	(regbank+n),y
	 *
	 * The source form is not generated by the parser but by the optimizer.
	 */
       	else if (LineMatch (L, "\tlda\tregbank+")      	 && /* Match on start */
       	       	 GetNextCodeLines (L, L2, 6)             && /* Fetch next lines */
       	    	 LineMatch (L2 [0], "\tldx\tregbank+")   && /* Match line 2 ... */
       	    	 L->Line [13] == L2 [0]->Line [13]     	 && /* Offset equal */
	    	 strcmp (L2 [0]->Line + 14, "+1") == 0 	 &&
		 LineFullMatch (L2 [1], "\tsta\tptr1") 	 &&
		 LineFullMatch (L2 [2], "\tstx\tptr1+1") &&
		 LineMatch (L2 [3], "\tlda\t")	       	 &&
		 LineMatch (L2 [4], "\tldy\t#$")       	 &&
		 LineMatch (L2 [5], "\tsta\t(ptr1),y")) {

	    char Buf [100];
	    sprintf (Buf, "\tsta\t(%s),y", L->Line + 5);

	    /* Found the sequence, replace it */
       	    L2 [5] = ReplaceLine (L2 [5], Buf);

     	    /* Free the remaining lines */
     	    FreeLines (L, L2 [2]);

     	    /* Make the line pointer valid again */
     	    L = L2 [5];
      	}

       	/* Search for the following sequence:
     	 *
       	 *     	lda    	regbank+n
     	 *     	ldx	regbank+n+1
     	 *  	sta	ptr1
     	 *  	stx	ptr1+1
     	 *  	ldy	#$mm
     	 *     	lda	(sp),y
     	 *  	ldy	#$ll
     	 *  	sta	(ptr1),y
     	 *
     	 * and replace it by:
     	 *
     	 *	ldy	#$mm
     	 *     	lda    	(sp),y
     	 *  	ldy	#$ll
       	 *     	sta	(regbank+n),y
     	 *
     	 * The source form is not generated by the parser but by the optimizer.
     	 */
       	else if (LineMatch (L, "\tlda\tregbank+")      	 && /* Match on start */
       	       	 GetNextCodeLines (L, L2, 7)             && /* Fetch next lines */
       	    	 LineMatch (L2 [0], "\tldx\tregbank+")   && /* Match line 2 ... */
       	    	 L->Line [13] == L2 [0]->Line [13]     	 && /* Offset equal */
     	    	 strcmp (L2 [0]->Line + 14, "+1") == 0 	 &&
     		 LineFullMatch (L2 [1], "\tsta\tptr1") 	 &&
     		 LineFullMatch (L2 [2], "\tstx\tptr1+1") &&
     		 LineMatch (L2 [3], "\tldy\t#$")       	 &&
     		 LineFullMatch (L2 [4], "\tlda\t(sp),y") &&
     		 LineMatch (L2 [5], "\tldy\t#$")       	 &&
     		 LineMatch (L2 [6], "\tsta\t(ptr1),y")) {

     	    char Buf [100];
	    sprintf (Buf, "\tsta\t(%s),y", L->Line + 5);

	    /* Found the sequence, replace it */
       	    L2 [6] = ReplaceLine (L2 [6], Buf);

     	    /* Free the remaining lines */
	    FreeLines (L, L2 [2]);

	    /* Make the line pointer valid again */
  	    L = L2 [6];
      	}

	/* Next Line */
	L = NextCodeLine (L);
    }
}



static void OptDoubleJumps (void)
/* Remove/rearrange jumps that jump to other jumps */
{
    static const char* Jumps [] = {
    	"\tjeq\tL",
    	"\tjne\tL",
	"\tbeq\tL",
	"\tbne\tL",
    	"\tjmp\tL",
    	0
    };

    unsigned D;

    Line* L = FirstCode;
    while (L) {

	int I;

	/* Is this a jump? */
       	while ((I = LineMatchX (L, Jumps)) >= 0) {

	    /* Yes. Get the target label */
	    Line* Target = GetTargetLine (L->Line + 5);

	    /* Target points to the label itself. Skip lines until we reach
	     * one that is not a label.
	     */
       	    Target = NextInstruction (Target);

	    /* Be sure, this line is not the same as the one the jump is
     	     * in (this happens if there is an empty loop).
	     */
	    if (Target == L) {
	    	break;
	    }
	    D = 0;
	    if (LineMatch (Target, "\tjmp\t")) {

	    	/* The target is itself a jump. If this is a short branch, get
		 * the final target and check if it is in reach. Bail out if
		 * not.
		 */
	 	if (L->Line[1] == 'b') {
		    Line* FinalTarget = GetTargetLine (Target->Line+5);
		    FinalTarget = NextInstruction (FinalTarget);
		    if ((D = GetJumpDistance (L, FinalTarget)) >= 123) {
			break;
		    }
	    	}

	       	/* Make sure the jump does indeed point to another label.
  	       	 * It may happen that this is not the case for some endless
	       	 * loop (while(1) and similar).
	       	 */
	       	if (strcmp (L->Line+5, Target->Line+5) == 0) {
	       	    /* Same label, bail out */
	       	    break;
	       	}

	       	/* Use the label in the original jump instead */
	       	L = ReplaceLine (L, "%.5s%s", L->Line, Target->Line+5);

       	    } else if (I < 2 && LineMatch (Target, Jumps [I])) {

	       	/* Conditional jump. Use final label */
	     	strcpy (L->Line+5, Target->Line+5);

	    } else {
	     	break;
	    }
	}

	/* Next line */
       	L = NextCodeLine (L);
    }
}



static void OptJumpRTS (void)
/* Replace jumps to an RTS by an RTS */
{
    Line* L = FirstCode;
    while (L) {
     	/* Is this a jump to a numbered label? */
	if (LineMatch (L, "\tjmp\t") && L->Line [5] == 'L' && isdigit (L->Line [6])) {

	    /* Yes. Get the target label */
	    Line* Target = GetTargetLine (L->Line+5);

     	    /* Target points to the label itself. Get the next line */
	    Target = NextCodeLine (Target);
       	    if (LineFullMatch (Target, "\trts")) {
		/* Replace the jump by an RTS */
  		L = ReplaceLine (L, "\trts");
	    }
	}
	L = NextCodeLine (L);
    }
}



static void OptBoolTransforms (void)
/* Try to remove the boolean transformation subroutines where they aren't
 * necessary.
 */
{
    Line* L2 [2];
    unsigned Label;
    const char* BranchTarget;

    Line* L = FirstCode;
    while (L) {

	/* Search for a boolean transformer followed by a conditional jump. */
       	if (LineMatch (L, "\tjsr\tbool") &&
	    GetNextCodeLines (L, L2, 1) &&
	    IsCondJump (L2 [0])) {

	    /* Make the boolean transformer unnecessary by changing the
	     * the conditional jump to evaluate the condition flags that
	     * are set after the compare directly. Note: jeq jumps if
	     * the condition is not met, jne jumps if the condition is met.
	     */

	    /* Get the condition code */
     	    int Cond = FindCond (L->Line + 9);
	    if (Cond < 0) {
	    	/* OOPS! */
	    	goto NextLine;
	    }

     	    /* Invert the code if we jump on condition not met. */
       	    if (L2[0]->Line [2] == 'e' && L2[0]->Line [3] == 'q') {
	     	/* Jumps if condition false, invert condition */
	     	Cond = CmpInvertTab [Cond];
  	    }

	    /* For easier reading, get a pointer to the jump target */
       	    BranchTarget = L2[0]->Line+5;

	    /* Check if we can replace the jump (sometimes we would need two
	     * conditional jumps, we will not handle that for now since it
	     * has some complications - both jumps may be far jumps for
	     * example making the jumps more costly than the bool transformer
	     * subroutine). If we cannot replace the jump, bail out.
     	     */
 	    switch (Cond) {

	    	case CMP_EQ:
		    L = ReplaceLine (L, "\tjeq\t%s", BranchTarget);
	    	    break;

	    	case CMP_NE:
		    L = ReplaceLine (L, "\tjne\t%s", BranchTarget);
	    	    break;

	       	case CMP_GT:
		    Label = AllocLabel ();
		    L = ReplaceLine  (L, "\tbeq\tL%04X", Label);
		    L = NewLineAfter (L, "\tjpl\t%s", BranchTarget);
		    L = NewLabelAfter(L, Label);
		    break;

	    	case CMP_GE:
		    L = ReplaceLine (L, "\tjpl\t%s", BranchTarget);
	    	    break;

	    	case CMP_LT:
		    L = ReplaceLine (L, "\tjmi\t%s", BranchTarget);
	    	    break;

	    	case CMP_LE:
       	       	    L = ReplaceLine  (L, "\tjeq\t%s", BranchTarget);
		    L = NewLineAfter (L, "\tjmi\t%s", BranchTarget);
	    	    break;

     	    	case CMP_UGT:
		    Label = AllocLabel ();
		    L = ReplaceLine  (L, "\tbeq\tL%04X", Label);
		    L = NewLineAfter (L, "\tjcs\t%s", BranchTarget);
		    L = NewLabelAfter(L, Label);
		    break;

	    	case CMP_UGE:
       	       	    L = ReplaceLine (L, "\tjcs\t%s", BranchTarget);
	    	    break;

	    	case CMP_ULT:
       	       	    L = ReplaceLine (L, "\tjcc\t%s", BranchTarget);
	    	    break;

	    	case CMP_ULE:
       	       	    L = ReplaceLine (L, "\tjeq\t%s", BranchTarget);
       	       	    L = NewLineAfter (L, "\tjcc\t%s", BranchTarget);
	    	    break;

     	    	default:
	    	    Internal ("Unknown jump condition: %u", Cond);

	    }

	    /* Remove the old stuff */
	    FreeLine (L2[0]);

	}

NextLine:
	L = NextCodeLine (L);
    }
}



static void OptCompares2 (void)
/* Try to optimize the integer compare subroutines. */
{
    Line* L2[10];
    unsigned Label;
    const char* BranchTarget;
    int C;

    Line* L = FirstCode;
    while (L) {

	/* Search for
	 *
	 *  	lda	x
	 *  	ldx	x+1
	 *  	cpx	#$00
	 *  	bne	*+4
	 *  	cmp	#$00
	 *  	jne/jeq	...
	 *
	 * and replace it by
	 *
	 *  	lda	x
	 *  	ora	x+1
	 *  	jne/jeq ...
	 */
	if (LineMatch (L, "\tlda\t")					&&
	    GetNextCodeLines (L, L2, 5)					&&
	    IsLoadAX (L, L2[0])						&&
	    LineFullMatch (L2[1], "\tcpx\t#$00")			&&
	    LineFullMatch (L2[2], "\tbne\t*+4")				&&
	    LineFullMatch (L2[3], "\tcmp\t#$00")			&&
	    IsCondJump (L2[4])) {

	    /* Replace the load of X by an ora */
	    L2[0]->Line[1] = 'o';
	    L2[0]->Line[2] = 'r';
	    L2[0]->Line[3] = 'a';

	    /* Remove unneeded stuff */
	    FreeLines (L2[1], L2[3]);

	}

	/* Same for local variables: Replace
	 *
	 *      ldy     #$xx
	 *      lda     (sp),y
	 *      tax
	 *      dey
	 *      lda     (sp),y
	 *      cpx     #$00
	 *      bne     *+4                                                                                  cmp     #$00
	 *   	cmp	#$00
	 *      jne/jeq ...
	 *
	 * by
	 *
	 *      ldy     #$xx
	 *      lda     (sp),y
       	 *      dey
	 *      ora	(sp),y
	 *      jne/jeq ...
	 */
       	else if (LineMatch (L, "\tldy\t#$")				&&
	    	 GetNextCodeLines (L, L2, 8)		       		&&
		 LineFullMatch (L2[0], "\tlda\t(sp),y")			&&
	    	 LineFullMatch (L2[1], "\ttax")				&&
		 LineFullMatch (L2[2], "\tdey")				&&
	    	 LineFullMatch (L2[3], "\tlda\t(sp),y")			&&
		 LineFullMatch (L2[4], "\tcpx\t#$00")			&&
		 LineFullMatch (L2[5], "\tbne\t*+4")			&&
		 LineFullMatch (L2[6], "\tcmp\t#$00")			&&
		 IsCondJump (L2[7])) {

	    /* Replace the second load by an ora */
	    L2[3]->Line[1] = 'o';
	    L2[3]->Line[2] = 'r';
	    L2[3]->Line[3] = 'a';

	    /* Remove unneeded stuff */
	    FreeLine (L2[1]);
	    FreeLines (L2[4], L2[6]);

	}

       	/* Search for the call to a compare subroutine followed by a
       	 * conditional jump.
     	 */
       	else if (LineMatch (L, "\tjsr\ttos")   	       			&&
       	       	(L2[0] = NextCodeLine (L)) != 0  			&&
     	    	IsCondJump (L2[0])) {

       	    /* Extract the condition from the function name and branch */
	    C = CheckAndGetIntCmp (L, L2[0]);
	    if (C < 0) {
	       	/* Something is wrong */
	       	goto NextLine;
	    }

     	    /* Replace the subroutine call by a cheaper one */
     	    L = ReplaceLine (L, "\tjsr\ttosicmp");

	    /* For easier reading, get a pointer to the jump target */
	    BranchTarget = L2[0]->Line+5;

     	    /* Check if we can replace the jump (sometimes we would need two
     	     * conditional jumps, we will not handle that for now since it
     	     * has some complications - both jumps may be far jumps for
     	     * example making the jumps more costly than the bool transformer
     	     * subroutine). If we cannot replace the jump, bail out.
     	     */
     	    switch (C) {

     	       	case CMP_EQ:
	    	    L = NewLineAfter (L, "\tjeq\t%s", BranchTarget);
     	       	    break;

     	       	case CMP_NE:
	    	    L = NewLineAfter (L, "\tjne\t%s", BranchTarget);
     	       	    break;

     	       	case CMP_GT:
	    	    Label = AllocLabel ();
	    	    L = NewLineAfter (L, "\tbeq\tL%04X", Label);
	    	    L = NewLineAfter (L, "\tjpl\t%s", BranchTarget);
	    	    L = NewLabelAfter(L, Label);
     	    	    break;

     	    	case CMP_GE:
	    	    L = NewLineAfter (L, "\tjpl\t%s", BranchTarget);
     	    	    break;

     	    	case CMP_LT:
	    	    L = NewLineAfter (L, "\tjmi\t%s", BranchTarget);
     	    	    break;

     	    	case CMP_LE:
       	       	    L = NewLineAfter (L, "\tjeq\t%s", BranchTarget);
	    	    L = NewLineAfter (L, "\tjmi\t%s", BranchTarget);
     	    	    break;

     	    	case CMP_UGT:
	    	    Label = AllocLabel ();
	    	    L = NewLineAfter (L, "\tbeq\tL%04X", Label);
       	       	    L = NewLineAfter (L, "\tjcs\t%s", BranchTarget);
	    	    L = NewLabelAfter(L, Label);
     	    	    break;

     	    	case CMP_UGE:
		    L = NewLineAfter (L, "\tjcs\t%s", BranchTarget);
     	    	    break;

     	    	case CMP_ULT:
		    L = NewLineAfter (L, "\tjcc\t%s", BranchTarget);
     	    	    break;

     	    	case CMP_ULE:
       	       	    L = NewLineAfter (L, "\tjeq\t%s", BranchTarget);
		    L = NewLineAfter (L, "\tjcc\t%s", BranchTarget);
     	    	    break;

     	    	default:
     	    	    Internal ("Unknown jump condition: %u", C);

     	    }

     	    /* Remove the old stuff */
	    FreeLine (L2[0]);
     	}

NextLine:
     	L = NextCodeLine (L);
    }
}



static void OptTests (void)
/* Remove unnecessary tests */
{
    Line* L2 [2];

    static const char* BitOps [] = {
	"\tand\t",
	"\tora\t",
	"\teor\t",
	0
    };

    /* Search for lda/tay/jne or lda/tay/jeq, remove the tay.
     * Search for
     * 	lda ...
     *  cmp #$00
     *  jne/jeq
     * Remove the cmp.
     */
    Line* L = FirstCode;
    while (L) {

	/* Search for lda/tay/jne or lda/tay/jeq, remove the tay.
	 * Search for
	 *  	lda/and/ora/eor
       	 *  	cmp 	#$00
	 *  	jne/jeq	...
	 * Remove the cmp.
	 */
       	if ((LineMatch (L, "\tlda\t")  		    ||
	     LineMatch (L, "\tand\t") 		    ||
	     LineMatch (L, "\tora\t") 		    ||
	     LineMatch (L, "\teor\t")) 			&&
	    GetNextCodeLines (L, L2, 2)			&&
	    (LineFullMatch (L2 [0], "\ttay") 	    ||
	     LineFullMatch (L2 [0], "\tcmp\t#$00")) 	&&
	    IsCondJump (L2 [1])) {

	    /* We can remove the tay */
	    FreeLine (L2 [0]);

	}

	/* Search for
	 *
	 *  	and	...
	 *  	tax
	 *  	jeq/jne
	 *
	 * and remove the tax.
	 */
       	else if (LineMatchX (L, BitOps)	>= 0		&&
	    	 GetNextCodeLines (L, L2, 2)		&&
	    	 LineFullMatch (L2[0], "\ttax")		&&
	    	 IsCondJump (L2[1])) {

	    /* Remove the tax including a hint line of there is one */
	    if (LineFullMatch (L2[0]->Prev, "+forcetest")) {
	    	FreeLine (L2[0]->Prev);
	    }
	    FreeLine (L2[0]);

	    /* If the line before L loads X, this is useless and may be removed */
	    L2[0] = PrevCodeLine (L);
	    if (LineFullMatch (L2[0], "\tldx\t#$00")) {
	    	FreeLine (L2[0]);
	    }

	}

	/* Search for the sequence
	 *
	 *  	stx	xx
	 *  	stx	tmp1
	 *  	ora	tmp1
	 *
	 * and replace it by
	 *
	 *  	stx	xx
	 *  	ora	xx
	 */
	else if (LineMatch (L, "\tstx\t")		&&
	    	 GetNextCodeLines (L, L2, 2)		&&
       	       	 LineFullMatch (L2[0], "\tstx\ttmp1")	&&
	    	 LineFullMatch (L2[1], "\tora\ttmp1")) {

	    /* Found, replace it */
	    L = NewLineAfter (L, "\tora\t%s", L->Line+5);

	    /* Remove remaining stuff */
	    FreeLines (L2[0], L2[1]);

	}


	/* Next line */
	L = NextCodeLine (L);
    }
}



static void OptBitOps (void)
/* Optimize bit oeprations */
{
    Line* L2 [2];

    /* Walk over the code */
    Line* L = FirstCode;
    while (L) {

	/* Search for
	 *
	 *  	lda	xxx
	 *  	and	#$yy	; adc/eor/ora
	 *  	sta	xxx
	 *
	 * and replace it by
	 *
	 *  	lda	#$yy
	 *  	and	xxx
	 *  	sta	xxx
	 *
	 * While this saves nothing here, it transforms the code to contain an
	 * explicit register load that may be removed by the basic block
	 * optimization later. As a special optimization for the 65C02, the
	 * "ora" and "and" ops may be replaced by "trb" and "tsb" resp. if the
	 * value in A is not used later.
	 */
	if (LineMatch (L, "\tlda\t") 	       	      	&&
	    L->Line[5] != '#'				&&
	    GetNextCodeLines (L, L2, 2)	      	      	&&
	    LineMatch (L2[1], "\tsta\t")      	      	&&
	    strcmp (L->Line+5, L2[1]->Line+5) == 0) {

	    if (LineMatch (L2[0], "\tand\t#$")) {

	    	unsigned Val = GetHexNum (L2[0]->Line+7);
		if (Val == 0x00) {

       	       	    /* AND with 0x00, remove the mem access */
		    FreeLine (L);
		    FreeLine (L2[1]);

		    /* Replace the AND by a load */
	    	    L = ReplaceLine (L2[0], "\tlda\t#$%02X", Val);

	    	} else if (Val == 0xFF) {

	    	    /* AND with 0xFF, just load the value from memory */
	    	    FreeLines (L2[0], L2[1]);

	    	} else if (CPU == CPU_65C02 	&&
	    		   !IsXAddrMode (L)	&&
	    		   !IsYAddrMode (L)	&&
	    		   !RegAUsed (L2[1])) {

	    	    /* Replace by trb */
	    	    ReplaceLine (L, 	"\tlda\t#$%02X", (~Val) & 0xFF);
	    	    ReplaceLine (L2[0], "\ttrb\t%s", L2[1]->Line+5);
	    	    FreeLine (L2[1]);
	    	    L = L2[0];

	    	} else {

	    	    /* Just reorder */
	    	    L = ReplaceLine (L, "\tlda\t#$%02X", Val);
	    	    ReplaceLine (L2[0], "\tand\t%s", L2[1]->Line+5);
	    	    L = L2[1];

	    	}

	    } else if (LineMatch (L2[0], "\tora\t#$")) {

	    	unsigned Val = GetHexNum (L2[0]->Line+7);
		if (Val == 0x00) {

		    /* ORA with 0x00, just load the value from memory */
		    FreeLines (L2[0], L2[1]);

		} else if (Val == 0xFF) {

       	       	    /* ORA with 0xFF, replace by a store of $FF */
		    FreeLine (L);
		    ReplaceLine (L2[0], "\tlda\t#$FF");

		} else if (CPU == CPU_65C02 	&&
			   !IsXAddrMode (L)	&&
			   !IsYAddrMode (L)	&&
			   !RegAUsed (L2[1])) {

		    /* Replace by trb */
		    ReplaceLine (L, 	"\tlda\t#$%02X", Val);
		    ReplaceLine (L2[0], "\ttsb\t%s", L2[1]->Line+5);
		    FreeLine (L2[1]);
		    L = L2[0];

		} else {

		    /* Just reorder */
		    L = ReplaceLine (L, "\tlda\t#$%02X", Val);
		    ReplaceLine (L2[0], "\tora\t%s", L2[1]->Line+5);
		    L = L2[1];

		}

	    } else if (LineMatch (L2[0], "\teor\t#$") ||
		       LineMatch (L2[0], "\tadc\t#$")) {

		/* Just reorder */
		L = ReplaceLine (L, "\tlda\t%s", L2[0]->Line+5);
		ReplaceLine (L2[0], "\t%.3s\t%s", L2[0]->Line+1, L2[1]->Line+5);

	    }
	}

	/* Next line */
	L = NextCodeLine (L);
    }
}



static void OptNeg (void)
/* Optimize the "bnegax/jeq" and "bnegax/jne" sequences */
{
    Line* L2 [10];

    Line* L = FirstCode;
    while (L) {

	/* Search for the sequence:
	 *
	 *  	lda	...
	 *  	jsr	bnega
	 *  	jeq/jne	...
	 *
	 * and replace it by:
	 *
	 *  	lda	...
	 *  	jne/jeq	...
	 */
       	if (LineMatch (L, "\tlda\t")  			&& /* Match on start */
       	    GetNextCodeLines (L, L2, 2)        	       	&& /* Fetch next lines */
       	    LineFullMatch (L2 [0], "\tjsr\tbnega")	&&
       	    IsCondJump (L2 [1])) {

	    /* Found the sequence, replace it */
	    FreeLine (L2 [0]);
	    InvertZJump (L2 [1]);

      	}

	/* Search for the sequence:
	 *
	 *  	ldy	#$xx
	 *  	lda	(sp),y
	 *  	tax
	 *  	dey
	 *  	lda	(sp),y
	 *  	jsr	bnegax
	 *  	jne/jeq	...
	 *
	 * and replace it by
	 *
	 *  	ldy	#$xx
	 *  	lda	(sp),y
	 *  	dey
	 *  	ora	(sp),y
	 *	jeq/jne	...
	 */
	else if (LineMatch (L, "\tldy\t#$")	       		&&
		 GetNextCodeLines (L, L2, 6)	      		&&
		 LineFullMatch (L2[0], "\tlda\t(sp),y")		&&
		 LineFullMatch (L2[1], "\ttax")			&&
		 LineFullMatch (L2[2], "\tdey")			&&
		 LineFullMatch (L2[3], "\tlda\t(sp),y")		&&
		 LineFullMatch (L2[4], "\tjsr\tbnegax")		&&
	    	 IsCondJump    (L2[5])) {

	    L2[1] = ReplaceLine (L2[1], "\tdey");
	    L2[2] = ReplaceLine (L2[2], "\tora\t(sp),y");
	    FreeLines (L2[3], L2[4]);
	    InvertZJump (L2[5]);

	}

	/* Search for the sequence:
	 *
	 *  	lda	xx
	 *  	ldx	xx+1
	 *  	jsr	bnegax
	 *  	jne/jeq	...
	 *
	 * and replace it by
	 *
	 *  	lda    	xx
	 *	ora	xx+1
	 *	jeq/jne	...
	 */
       	else if (LineMatch (L, "\tlda\t")	       		&&
		 GetNextCodeLines (L, L2, 3)	      		&&
		 IsLoadAX (L, L2[0])   				&&
       	       	 LineFullMatch (L2[1], "\tjsr\tbnegax")		&&
		 IsCondJump    (L2[2])) {

	    /* Replace the load of X by ora */
	    L2[0]->Line[1] = 'o';
	    L2[0]->Line[2] = 'r';
	    L2[0]->Line[3] = 'a';
       	    FreeLine (L2[1]);
	    InvertZJump (L2[2]);

	}

	/* Search for the sequence:
	 *
	 *  	jsr   	_xxx
	 *  	jsr   	bnega(x)
	 *  	jeq/jne	...
	 *
	 * and replace it by:
	 *
	 *      jsr	_xxx
	 *  	<boolean test>
	 *  	jne/jeq	...
	 */
       	else if (LineMatch (L, "\tjsr\t_")		&& /* Match on start */
       	         GetNextCodeLines (L, L2, 2)  		&&
       	         LineMatch (L2 [0], "\tjsr\tbnega")	&&
       	         IsCondJump (L2 [1])) {

	    if (LineFullMatch (L2 [0], "\tjsr\tbnega")) {
	    	/* Byte sized */
	    	L2 [0] = ReplaceLine (L2 [0], "\ttax");	/* Test a */
	    } else {
	    	/* Word sized */
	    	L2 [0] = ReplaceLine (L2 [0], "\tstx\ttmp1");
	    	NewLineAfter (L2 [0], "\tora\ttmp1");
	    }

	    /* Invert the jump */
	    InvertZJump (L2 [1]);

      	}

	/* Next line */
	L = NextCodeLine (L);
    }
}



static void OptTriples (void)
/* Replace code triples */
{
    static const char* Pat1 [] = {
	"\tjsr\tldaxysp",
	"\tjsr\tldax0sp",
	"\tjsr\tldaysp",
	"\tjsr\tleaasp",
	"\tjsr\tldaxi",
	0
    };
    static const char* Pat2 [] = {
	"\tjsr\tpushax",
	"\tjsr\tpushax",
	"\tjsr\tpushax",
	"\tjsr\tpushax",
	"\tjsr\tpushax",
	0
    };
    static const char* Replace [] = {
	"\tjsr\tpushwysp",
       	"\tjsr\tpushw0sp",
	"\tjsr\tpushbysp",
	"\tjsr\tpleaasp",
	"\tjsr\tpushw",
    };

    Line* L = FirstCode;
    while (L) {
	int I =	LineFullMatchX (L, Pat1);
	if (I >= 0) {
	    /* We found the first match, get the next line */
	    Line* L2 = NextCodeLine (L);
	    if (L2 && LineFullMatch (L2, Pat2 [I])) {
		/* Found. Replace by the short call */
		FreeLine (L2);
		L = ReplaceLine (L, Replace [I]);
	    }
	}
	/* Next line */
	L = NextCodeLine (L);
    }
}



static Line* OptOneBlock (Line* L)
/* Optimize the register contents inside one basic block */
{
    static const char* Compares [] = {
	"\tjsr\ttoseq00",   "\tjsr\ttoseqa0",   "\tjsr\ttoseqax",
	"\tjsr\ttoseqeax",  "\tjsr\ttosne00",   "\tjsr\ttosnea0",
	"\tjsr\ttosneax",   "\tjsr\ttosneeax", 	"\tjsr\ttoslt00",
	"\tjsr\ttoslta0",   "\tjsr\ttosltax",   "\tjsr\ttosult00",
       	"\tjsr\ttosulta0",  "\tjsr\ttosultax",	"\tjsr\ttoslteax",
	"\tjsr\ttosulteax", "\tjsr\ttosle00",   "\tjsr\ttoslea0",
	"\tjsr\ttosleax",   "\tjsr\ttosule00",  "\tjsr\ttosulea0",
  	"\tjsr\ttosuleax",  "\tjsr\ttosleeax",  "\tjsr\ttosuleeax",
	"\tjsr\ttosgt00",   "\tjsr\ttosgta0",   "\tjsr\ttosgtax",
	"\tjsr\ttosugt00",  "\tjsr\ttosugta0",  "\tjsr\ttosugtax",
	"\tjsr\ttosgteax",  "\tjsr\ttosugteax", "\tjsr\ttosge00",
	"\tjsr\ttosgea0",   "\tjsr\ttosgeax",   "\tjsr\ttosuge00",
       	"\tjsr\ttosugea0",  "\tjsr\ttosugeax",	"\tjsr\ttosgeeax",
	"\tjsr\ttosugeeax",
	0
    };

    static const char* MakeBool [] = {
	"\tjsr\tbooleq",    "\tjsr\tboolne",    "\tjsr\tboollt",
       	"\tjsr\tboolle",    "\tjsr\tboolgt",    "\tjsr\tboolge",
       	"\tjsr\tboolult",   "\tjsr\tboolule",   "\tjsr\tboolugt",
	"\tjsr\tbooluge",
	0
    };

    int A = -1;	      	   	/* Contents of A register */
    int X = -1;	      	   	/* Contents of X register */
    int Y = -1;	      	    	/* Contents of Y register */
    Line* L2;
    unsigned NewVal;
    int Delete;

    while (L && !IsLabel (L)) {

	/* Handle all instructions. All instructions not tested here have
	 * no effects on the register contents.
	 */
	Delete = 0;
	if (L->Line [0] == '+') {
	    /* This is a hint */
	    if (LineMatch (L, "+a:")) {
	     	/* Information about a */
	     	switch (L->Line [3]) {
       	       	    case '!':  	A = -1;	       	       	       	break;
	     	    case '=':	A = GetHexNum (L->Line + 4);	break;
	     	}
	    } else if (LineMatch (L, "+x:")) {
	     	/* The code generator tells something about the x register */
	     	switch (L->Line [3]) {
	     	    case '!':	X = -1;				break;
	     	    case '=':	X = GetHexNum (L->Line + 4);	break;
	     	}
	    } else if (LineMatch (L, "+y:")) {
	     	/* Information about the y register */
	     	switch (L->Line [3]) {
       	       	    case '!':  	Y = -1;				break;
	     	    case '=':	Y = GetHexNum (L->Line + 4);	break;
	     	}
	    }
       	} else if (LineMatch (L, "\tadc\t")) {
	    if (CPU == CPU_65C02 && Y == 0 && L->Line[5] == '(' && IsYAddrMode(L)) {
	    	L->Line[strlen(L->Line)-2] = '\0';
	    }
	    A = -1;
	} else if (LineMatch (L, "\tand\t")) {
	    A = -1;
	} else if (LineFullMatch (L, "\tasl\ta")) {
	    if (A != -1) {
	     	A = (A << 1) & 0xFF;
	    }
       	} else if (CPU == CPU_65C02 && Y == 0 && L->Line[5] == '(' && IsYAddrMode(L)) {
	    L->Line[strlen(L->Line)-2] = '\0';
	} else if (CPU == CPU_65C02 && (LineFullMatch (L, "\tdea") ||
					LineFullMatch (L, "\tdec\ta"))) {
	    DEC (A, 1);
	} else if (LineFullMatch (L, "\tdex")) {
	    DEC (X, 1);
	} else if (LineFullMatch (L, "\tdey")) {
	    DEC (Y, 1);
	} else if (LineMatch (L, "\teor")) {
	    A = -1;
	} else if (CPU == CPU_65C02 && (LineFullMatch (L, "\tina") ||
					LineFullMatch (L, "\tinc\ta"))) {
	    INC (A, 1);
	} else if (LineFullMatch (L, "\tinx")) {
	    INC (X, 1);
	} else if (LineFullMatch (L, "\tiny")) {
	    INC (Y, 1);
      	} else if (LineFullMatch (L, "\tjsr\taddeq0sp")) {
	    /* We know about this function */
	    A = X = -1;
	    Y = 1;
	} else if (LineFullMatch (L, "\tjsr\taddeqysp")) {
	    /* We know about this function */
	    A = X = -1;
	    INC (Y, 1);
	} else if (LineFullMatch (L, "\tjsr\taxulong")) {
	    /* We know about this function and we're trying to replace it by
	     * inline code if we have already a register that contains zero.
	     */
	    char C;
	    if (A == 0) {
	     	C = 'a';
	    } else if (X == 0) {
	     	C = 'x';
	    } else if (Y == 0) {
	     	C = 'y';
	    } else {
	     	C = '\0';
	    }
	    if (C == '\0') {
		/* We cannot replace the code, but we know about the results */
		Y = 0;
	    } else {
		L = ReplaceLine (L, "\tst%c\tsreg", C);
		NewLineAfter (L, "\tst%c\tsreg+1", C);
	    }
	} else if (LineFullMatch (L, "\tjsr\tbnega")) {
	    /* We know about this function */
	    A = -1;
	    X = 0;
	} else if (LineFullMatch (L, "\tjsr\tbnegax")) {
	    /* We know about this function */
	    A = -1;
	    X = 0;
	} else if (LineFullMatch (L, "\tjsr\tbnegeax")) {
	    /* We know about this function */
	    A = -1;
	    X = 0;
	} else if (LineFullMatch (L, "\tjsr\tcomplax")) {
	    /* We know about this function */
	    if (A != -1) {
		A ^= 0xFF;
	    }
	    if (X != -1) {
		X ^= 0xFF;
	    }
	} else if (LineFullMatch (L, "\tjsr\tdecax1")) {
	    /* We know about this function */
	    A = X = -1;
	} else if (LineFullMatch (L, "\tjsr\tdecax2")) {
	    /* We know about this function */
	    A = X = -1;
	} else if (LineFullMatch (L, "\tjsr\tdecaxy")) {
	    /* We know about this function */
	    A = X = -1;
	} else if (LineFullMatch (L, "\tjsr\tdeceaxy")) {
	    /* We know about this function */
	    A = X = -1;
      	} else if (LineFullMatch (L, "\tjsr\tincax1")) {
	    /* We know about this function */
	    A = X = -1;
	} else if (LineFullMatch (L, "\tjsr\tincax2")) {
	    /* We know about this function */
	    A = X = -1;
	} else if (LineFullMatch (L, "\tjsr\tinceaxy")) {
	    /* We know about this function */
	    A = X = -1;
	} else if (LineFullMatch (L, "\tjsr\tladdeq")) {
	    /* We know about this function */
	    A = X = -1;
	    Y = 3;
	} else if (LineFullMatch (L, "\tjsr\tladdeqb")) {
	    /* We know about this function */
	    A = X = -1;
	    Y = 3;
	} else if (LineFullMatch (L, "\tjsr\tlbneg")) {
	    /* We know about this function */
       	    A = -1;
	    X = 0;
	} else if (LineFullMatch (L, "\tjsr\tldai")) {
	    /* We know about this function */
	    A = X = -1;
	    Y = 0;
	} else if (LineFullMatch (L, "\tjsr\tldaidx")) {
	    /* We know about this function */
	    A = X = -1;
	} else if (LineFullMatch (L, "\tjsr\tldau00sp")) {
	    /* We know about this function */
	    A = -1;
	    X = 0;
	    Y = 0;
	} else if (LineFullMatch (L, "\tjsr\tldau0ysp")) {
	    /* We know about this function */
	    A = -1;
	    X = 0;
	    DEC (Y, 1);
	} else if (LineFullMatch (L, "\tjsr\tldaui")) {
	    /* We know about this function */
	    A = -1;
	    X = 0;
	    Y = 0;
	} else if (LineFullMatch (L, "\tjsr\tldaui0sp")) {
	    A = -1;
       	    Y = X;
	    X = 0;
      	} else if (LineFullMatch (L, "\tjsr\tldauidx")) {
      	    /* We know about this function */
      	    A = -1;
      	    X = 0;
      	} else if (LineFullMatch (L, "\tjsr\tldauiysp")) {
      	    /* We know about this function */
      	    A = -1;
	    Y = X;
      	    X = 0;
      	} else if (LineFullMatch (L, "\tjsr\tldax0sp")) {
      	    /* We know about this function */
      	    A = X = -1;
      	    Y = 0;
      	} else if (LineFullMatch (L, "\tjsr\tldaxi")) {
      	    /* We know about this function */
      	    A = X = -1;
      	    Y = 0;
      	} else if (LineFullMatch (L, "\tjsr\tldaxidx")) {
      	    /* We know about this function */
      	    A = X = -1;
	    DEC (Y, 1);
      	} else if (LineFullMatch (L, "\tjsr\tldaxysp")) {
      	    /* We know about this function */
      	    A = X = -1;
	    DEC (Y, 1);
      	} else if (LineFullMatch (L, "\tjsr\tldeaxi")) {
      	    /* We know about this function */
      	    A = X = -1;
      	    Y = 0;
      	} else if (LineFullMatch (L, "\tjsr\tldeaxidx")) {
      	    /* We know about this function */
      	    A = X = -1;
	    DEC (Y, 3);
       	} else if (LineFullMatch (L, "\tjsr\tlsubeq")) {
	    /* We know about this function */
	    A = X = -1;
	    Y = 3;
       	} else if (LineFullMatch (L, "\tjsr\tlsubeqb")) {
	    /* We know about this function */
	    A = X = -1;
	    Y = 3;
       	} else if (LineFullMatch (L, "\tjsr\tnegax")) {
	    /* We know about this function */
	    A = X = -1;
       	} else if (LineFullMatch (L, "\tjsr\tnegeax")) {
	    /* We know about this function */
	    A = X = -1;
      	} else if (LineFullMatch (L, "\tjsr\tpush0")) {
	    /* We know about this function */
	    A = 0;
	    X = 0;
	    Y = 1;
	} else if (LineFullMatch (L, "\tjsr\tpush1")) {
	    /* We know about this function */
	    A = 1;
	    X = 0;
	    Y = 1;
	} else if (LineFullMatch (L, "\tjsr\tpush2")) {
	    /* We know about this function */
	    A = 2;
	    X = 0;
	    Y = 1;
	} else if (LineFullMatch (L, "\tjsr\tpush3")) {
	    /* We know about this function */
	    A = 3;
	    X = 0;
	    Y = 1;
	} else if (LineFullMatch (L, "\tjsr\tpush4")) {
	    /* We know about this function */
	    A = 4;
	    X = 0;
	    Y = 1;
	} else if (LineFullMatch (L, "\tjsr\tpush5")) {
	    /* We know about this function */
	    A = 5;
	    X = 0;
	    Y = 1;
	} else if (LineFullMatch (L, "\tjsr\tpush6")) {
	    /* We know about this function */
	    A = 6;
	    X = 0;
      	    Y = 1;
	} else if (LineFullMatch (L, "\tjsr\tpush7")) {
	    /* We know about this function */
	    A = 7;
	    X = 0;
	    Y = 1;
	} else if (CPU == CPU_65C02 && LineFullMatch (L, "\tjsr\tpusha")) {
	    /* We know about this function */
	    Y = 0;
	} else if (LineFullMatch (L, "\tjsr\tpusha0")) {
	    /* We know about this function
	     * If X is already zero, we may call pushax instead and save two
	     * cycles.
	     */
	    if (X == 0) {
	    	L = ReplaceLine (L, "\tjsr\tpushax");
	    }
	    X = 0;
	    Y = 1;
	} else if (LineFullMatch (L, "\tjsr\tpushax")) {
	    /* We know about this function */
	    Y = 1;
	} else if (LineFullMatch (L, "\tjsr\tpushaysp")) {
	    /* We know about this function */
	    A = -1;
	    Y = 0;
	} else if (LineFullMatch (L, "\tjsr\tpushc0")) {
	    /* We know about this function */
	    A = 0;
	    Y = 0;
	} else if (LineFullMatch (L, "\tjsr\tpushc1")) {
	    /* We know about this function */
	    A = 1;
	    Y = 0;
	} else if (LineFullMatch (L, "\tjsr\tpushc2")) {
	    /* We know about this function */
	    A = 2;
	    Y = 0;
	} else if (LineFullMatch (L, "\tjsr\tpushw")) {
	    /* We know about this function (calls pushax) */
	    A = X = -1;
	    Y = 1;
	} else if (LineFullMatch (L, "\tjsr\tpushw0sp")) {
	    /* We know about this function(calls pushax)  */
	    A = X = -1;
	    Y = 1;
	} else if (LineFullMatch (L, "\tjsr\tpushwidx")) {
	    /* We know about this function (calls pushax) */
	    A = X = -1;
	    Y = 1;
	} else if (LineFullMatch (L, "\tjsr\tpushwysp")) {
	    /* We know about this function (calls pushax) */
	    A = X = -1;
	    Y = 1;
	} else if (LineFullMatch (L, "\tjsr\tresteax")) {
	    /* We know about this function */
	    A = X = -1;
	} else if (LineFullMatch (L, "\tjsr\tsaveeax")) {
	    /* We know about this function */
	    /* Changes nothing */
	} else if (LineFullMatch (L, "\tjsr\tshrax1")) {
	    /* We know about this function */
	    A = X = -1;
	} else if (LineFullMatch (L, "\tjsr\tshrax2")) {
	    /* We know about this function */
	    A = X = -1;
	} else if (LineFullMatch (L, "\tjsr\tshrax3")) {
	    /* We know about this function */
	    A = X = -1;
	} else if (LineFullMatch (L, "\tjsr\tshreax1")) {
	    /* We know about this function */
	    A = X = -1;
	} else if (LineFullMatch (L, "\tjsr\tshreax2")) {
	    /* We know about this function */
	    A = X = -1;
	} else if (LineFullMatch (L, "\tjsr\tshreax3")) {
	    /* We know about this function */
	    A = X = -1;
	} else if (LineFullMatch (L, "\tjsr\tstaspp")) {
	    /* We know about this function */
	    Y = -1;
	} else if (LineFullMatch (L, "\tjsr\tstaxspp")) {
	    /* We know about this function */
	    Y = -1;
	} else if (LineFullMatch (L, "\tjsr\tstax0sp")) {
	    /* We know about this function */
	    Y = 1;
	} else if (LineFullMatch (L, "\tjsr\tstaxysp")) {
	    /* We know about this function */
	    INC (Y, 1);
	} else if (LineFullMatch (L, "\tjsr\tsubeq0sp")) {
	    /* We know about this function */
	    A = X = -1;
	    Y = 1;
	} else if (LineFullMatch (L, "\tjsr\tsubeqysp")) {
	    /* We know about this function */
	    A = X = -1;
	    INC (Y, 1);
	} else if (LineFullMatch (L, "\tjsr\ttosadda0")) {
	    /* We know about this function */
	    A = X = -1;
       	    Y = 1;
	} else if (LineFullMatch (L, "\tjsr\ttosaddax")) {
	    /* We know about this function */
	    A = X = -1;
       	    Y = 1;
	} else if (LineFullMatch (L, "\tjsr\ttosicmp")) {
	    /* We know about this function */
	    A = X = -1;
       	    Y = 0;
	} else if (LineFullMatchX (L, Compares) >= 0) {
	    A = Y = -1;
	    X = 0;
	} else if (LineFullMatchX (L, MakeBool) >= 0) {
	    A = -1;
	    X = 0;
	} else if (LineMatch (L, "\tjsr\t")) {
	    /* Subroutine call, forget all register information */
	    A = X = Y = -1;
	} else if (LineMatch (L, "\tlda\t")) {
       	    if (!RegAUsed (L) && !IsCondJump (NextInstruction (L))) {
	     	/* The value loaded is not used later, remove it */
	     	Delete = 1;
	    } else if (LineMatch (L, "\tlda\t(")) {
	     	if (IsXAddrMode (L)) {
	     	    /* lda (zp,x). */
		    /* If X is zero and we have a 65C02 cpu, replace it by
		     * an indirect load.
		     */
		    if (X == 0 && CPU == CPU_65C02) {
			unsigned Len = strlen (L->Line);
			L->Line [Len-3] = ')';
			L->Line [Len-2] = '\0';
       	       	    /* If Y and X are both zero, replace by load indirect
		     * y and save one cycle in some cases.
		     */
	    	    } else if (X == 0 && Y == 0) {
		    	char Buf [256];
		    	const char* S = L->Line + 6;
		    	char* T = Buf + 6;
		    	strcpy (Buf, "\tlda\t(");
		    	while (*S != ',') {
		    	    *T++ = *S++;
		    	}
		    	*T++ = ')';
		    	*T++ = ',';
		    	*T++ = 'y';
	    	    	*T   = '\0';
		    	L = ReplaceLine (L, Buf);
		    }
		} else if (IsYAddrMode (L)) {
		    /* lda (zp),y. If Y is zero and we have a 65C02 CPU,
		     * replace it by an indirect load.
		     */
		    if (Y == 0 && CPU == CPU_65C02) {
			unsigned Len = strlen (L->Line);
			L->Line [Len-3] = ')';
			L->Line [Len-2] = '\0';
		    }
		}
		/* In any case invalidate A */
	    	A = -1;
	    } else if (LineMatch (L, "\tlda\t#$")) {
		/* Immidiate load into A */
	    	NewVal = GetHexNum (L->Line + 7);
		if (NewVal == A) {
		    /* Load has no effect */
		    Delete = 1;
		} else if (NewVal == X) {
	     	    /* Requested value is already in X */
		    L = ReplaceLine (L, "\ttxa");
		} else if (NewVal == Y) {
		    /* Requested value is already in Y */
		    L = ReplaceLine (L, "\ttya");
		} else if (CPU == CPU_65C02 && A != -1) {
		    /* Try ina/dea operators of 65C02 */
		    if (NewVal == ((A - 1) & 0xFF)) {
			L = ReplaceLine (L, "\tdea");
		    } else if (NewVal == ((A + 1) & 0xFF)) {
			L = ReplaceLine (L, "\tina");
		    }
		}
		/* Anyway, the new value is now in A */
		A = NewVal;
	    } else {
		/* Memory load into A */
		A = -1;
	    }
	} else if (LineMatch (L, "\tldax\t")) {
	    /* Memory load into A and X */
	    A = X = -1;
	} else if (LineMatch (L, "\tldx\t")) {
       	    if (!RegXUsed (L) && !IsCondJump (NextInstruction (L))) {
		/* The value loaded is not used later, remove it */
		Delete = 1;
	    } else if (LineMatch (L, "\tldx\t#$")) {
		/* Immidiate load into X */
		NewVal = GetHexNum (L->Line + 7);
		if (NewVal == X) {
		    /* Load has no effect */
		    Delete = 1;
		} else if (NewVal == A) {
		    /* Requested value is already in A */
		    L = ReplaceLine (L, "\ttax");
	    	} else if (X != -1 && NewVal == ((X + 1) & 0xFF)) {
		    /* Requested value is one more than current contents */
		    L = ReplaceLine (L, "\tinx");
	    	} else if (X != -1 && NewVal == ((X - 1) & 0xFF)) {
	    	    /* Requested value is one less than current contents */
	     	    L = ReplaceLine (L, "\tdex");
	     	}
	     	/* Anyway, the new value is now in X */
	     	X = NewVal;
	    } else {
	     	/* Memory load into X */
	     	X = -1;
	    }
	} else if (LineMatch (L, "\tldy\t")) {
       	    if (!RegYUsed (L) && !IsCondJump (NextInstruction (L))) {
		/* The value loaded is not used later, remove it */
		Delete = 1;
	    } else if (LineMatch (L, "\tldy\t#$")) {
		/* Immidiate load into Y */
		NewVal = GetHexNum (L->Line + 7);
		if (NewVal == Y) {
		    /* Load has no effect */
		    Delete = 1;
		} else if (NewVal == A) {
		    /* Requested value is already in A */
		    L = ReplaceLine (L, "\ttay");
		} else if (Y != -1 && NewVal == ((Y + 1) & 0xFF)) {
	    	    /* Requested value is one more than current contents */
		    L = ReplaceLine (L, "\tiny");
		} else if (Y != -1 && NewVal == ((Y - 1) & 0xFF)) {
		    /* Requested value is one less than current contents */
		    L = ReplaceLine (L, "\tdey");
		}
		/* Anyway, the new value is now in Y */
		Y = NewVal;
	    } else {
		/* Memory load into Y */
		Y = -1;
	    }
	} else if (LineFullMatch (L, "\tlsr\ta")) {
	    if (A != -1) {
		A >>= 1;
	    }
	} else if (LineMatch (L, "\tora\t#$")) {
	    if (A != -1) {
		A |= GetHexNum (L->Line + 7);
	    }
	} else if (LineMatch (L, "\tora\t")) {
	    A = -1;
	} else if (LineFullMatch (L, "\tpla")) {
	    A = -1;
	} else if (LineFullMatch (L, "\trol\ta")) {
	    A = -1;
	} else if (LineFullMatch (L, "\tror\ta")) {
	    A = -1;
	} else if (LineFullMatch (L, "\trts")) {
	    A = X = Y = -1;
	} else if (LineFullMatch (L, "\trti")) {
	    A = X = Y = -1;
	} else if (LineMatch (L, "\tsbc\t")) {
	    if (CPU == CPU_65C02 && Y == 0 && L->Line[5] == '(' && IsYAddrMode(L)) {
	    	L->Line[strlen(L->Line)-2] = '\0';
	    }
	    A = -1;
	} else if (CPU == CPU_65C02 && LineMatch (L, "\tst")) {
	    /* Try to replace by stz if possible */
	    if (A == 0 && LineMatch (L, "\tsta\t")) {
	    	/* Not indirect and not Y allowed */
	    	if (L->Line[5] != '(' && !IsYAddrMode (L)) {
	    	    L->Line[3] = 'z';
  	    	}
	    } else if (X == 0 && LineMatch (L, "\tstx\t")) {
	    	/* absolute,y not allowed */
	    	if (!IsYAddrMode (L)) {
	    	    L->Line[3] = 'z';
	    	}
	    } else if (Y == 0 && LineMatch (L, "\tsty\t")) {
	    	/* sty and stz share all addressing modes */
 	    	L->Line[3] = 'z';
	    }
	} else if (LineFullMatch (L, "\ttax")) {
	    if (A != -1 && X == A) {
	    	/* Load has no effect */
	    	Delete = 1;
	    } else {
       	    	X = A;
	    }
	} else if (LineFullMatch (L, "\ttay")) {
	    if (A != -1 && Y == A) {
	    	/* Load has no effect */
	    	Delete = 1;
	    } else {
	    	Y = A;
	    }
	} else if (LineFullMatch (L, "\ttsx")) {
	    X = -1;
	} else if (LineFullMatch (L, "\ttxa")) {
	    if (X != -1 && A == X) {
	    	/* Load has no effect */
	    	Delete = 1;
	    } else {
	    	A = X;
	    }
	} else if (LineFullMatch (L, "\ttya")) {
	    if (Y != -1 && A == Y) {
	    	/* Load has no effect */
	    	Delete = 1;
	    } else {
	        A = Y;
	    }
	}

	/* Set to next line, handle deletions */
	L2 = NextCodeSegLine (L);
	if (Delete) {
	    FreeLine (L);
	}
	L = L2;

    }
    if (L) {
	/* Skip the label */
	L = NextCodeSegLine (L);
    }
    return L;
}



static void OptBlocks (void)
/* Optimize the register contents inside basic blocks */
{
    Line* L = FirstCode;
    while (L) {
	L = OptOneBlock (L);
    }
}



static void OptJumps (void)
/* Optimize jumps */
{
    static const char* Jumps [] = {
	"\tjeq\tL",
	"\tjne\tL",
	"\tjmi\tL",
	"\tjpl\tL",
	"\tjcs\tL",
	"\tjcc\tL",
 	0
    };

    Line* L = FirstCode;
    while (L) {
 	int I = LineMatchX (L, Jumps);
 	if (I >= 0) {
 	    Line* Target = GetTargetLine (L->Line+5);
       	    if (Target->Index > L->Index) {
 	       	/* This is a forward jump. Backward jumps are handled
 	       	 * automagically by the assembler.
 	       	 */
 	       	unsigned Distance = GetJumpDistance (L, Target);
       	       	if (Distance < 123) {		/* Safety */
 	       	    L->Line [1] = 'b';		/* Make a short branch */
 	    	    L->Size = 2;		/* Set new size */
 	       	}
 	    }
 	}
 	L = NextCodeLine (L);
    }

    /* Special treatment for jumps on the 65C02 */
    if (CPU == CPU_65C02) {

	Line* L = FirstCode;
	while (L) {
	    if (LineMatch (L, "\tjmp\tL")) {
		Line* Target = GetTargetLine (L->Line+5);
		unsigned Distance = GetJumpDistance (L, Target);
		if (Distance < 123) {		/* Safety */
       	       	    L->Line [1] = 'b';		/* Make a short branch */
		    L->Line [2] = 'r';
		    L->Line [3] = 'a';
		    L->Size = 2;      		/* Set new size */
		}
	    }
	    L = NextCodeLine (L);
	}

    }
}



static void OptRTS (void)
/* Change sequences of jsr XXX/rts to jmp XXX */
{
    Line* L = FirstCode;
    while (L) {
       	if (LineMatch (L, "\tjsr\t")) {
	    /* This is a jsr, get the next instruction */
	    Line* L2 = NextCodeLine (L);
       	    if (L2 && LineFullMatch (L2, "\trts")) {
	     	/* We found a sequence */
	     	FreeLine (L2);
       	       	L->Line [2] = 'm';
	     	L->Line [3] = 'p';
	    }
	}
	/* Try the next line */
	L = NextCodeLine (L);
    }
}



/*****************************************************************************/
/*	      	     		     Code			 	     */
/*****************************************************************************/



void OptDoOpt (void)
/* Run the optimizer over the collected stuff */
{
    typedef void (*OptFunc)(void);

    /* Table with optimizer steps -  are called in this order */
    static const OptFunc OptFuncs [] = {
       	OptCompares1,  		/* Optimize compares - first step */
       	OptDeadJumps,  		/* Remove dead jumps */
       	OptLoads,      		/* Remove unnecessary loads */
       	OptRegLoads,   		/* Remove unnecessary register loads */
       	OptPtrOps,     		/* Optimize stores through pointers */
       	OptRegVars,    		/* Optimize use of register variables */
       	OptDoubleJumps,		/* Remove jump cascades - must be used before OptNeg */
       	OptNeg,	       		/* Remove unnecessary boolean negates */
       	OptJumpRTS,    		/* Replace jumps to an RTS by an RTS */
	OptBoolTransforms,	/* Optimize boolean transforms */
       	OptCompares2,  		/* Optimize compares */
       	OptTests,      		/* Remove unnecessary tests */
	OptBitOps,		/* Optimize bit operations */
       	OptTriples,    		/* Optimize several triples */
       	OptBlocks,     		/* Optimize basic blocks */
       	OptRegLoads,   		/* Remove unnecessary register loads (another pass) */
       	OptBlocks,     		/* Optimize basic blocks */
       	OptJumps,      		/* Optimize jumps */
       	OptRTS,			/* Optimize jsr/rts sequences */
    };

    unsigned long Flags;
    unsigned 	  I;

    /* Find and remember the first line of code */
    FindCodeStart ();

    /* Mark all lines inside the code segment */
    MarkCodeLines ();

    /* Create a list of all local labels for fast access */
    CreateLabelList ();

    /* Ok, now start the real optimizations */
    Flags = 1UL;
    for (I = 0; I < sizeof(OptFuncs)/sizeof(OptFuncs[0]); ++I, Flags <<= 1) {
       	if ((OptDisable & Flags) == 0) {
	    OptFuncs[I] ();
	} else if (Verbose || Debug) {
	    printf ("Optimizer pass %u skipped\n", I);
	}
    }
}



