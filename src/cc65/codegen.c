/*****************************************************************************/
/*                                                                           */
/*				   codegen.c				     */
/*                                                                           */
/*			      6502 code generator			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2001 Ullrich von Bassewitz                                       */
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
#include <string.h>
#include <stdarg.h>

/* common */
#include "check.h"
#include "version.h"
#include "xmalloc.h"
#include "xsprintf.h"

/* b6502 */
#include "codeseg.h"
#include "dataseg.h"

/* cc65 */
#include "asmcode.h"
#include "asmlabel.h"
#include "cpu.h"
#include "error.h"
#include "global.h"
#include "segname.h"
#include "util.h"
#include "codegen.h"



/*****************************************************************************/
/*	  			     Data				     */
/*****************************************************************************/



/* Compiler relative stack pointer */
int oursp 	= 0;

/* Current segment */
segment_t CurSeg = SEG_INV;



/*****************************************************************************/
/*				    Helpers				     */
/*****************************************************************************/



static void typeerror (unsigned type)
/* Print an error message about an invalid operand type */
{
    Internal ("Invalid type in CF flags: %04X, type = %u", type, type & CF_TYPE);
}



static void CheckLocalOffs (unsigned Offs)
/* Check the offset into the stack for 8bit range */
{
    if (Offs >= 256) {
	/* Too many local vars */
	Error ("Too many local variables");
    }
}



static char* GetLabelName (unsigned flags, unsigned long label, unsigned offs)
{
    static char lbuf [128];		/* Label name */

    /* Create the correct label name */
    switch (flags & CF_ADDRMASK) {

	case CF_STATIC:
       	    /* Static memory cell */
	    sprintf (lbuf, "L%04X+%u", (unsigned)(label & 0xFFFF), offs);
	    break;

	case CF_EXTERNAL:
	    /* External label */
	    sprintf (lbuf, "_%s+%u", (char*) label, offs);
	    break;

	case CF_ABSOLUTE:
	    /* Absolute address */
	    sprintf (lbuf, "$%04X", (unsigned)((label+offs) & 0xFFFF));
	    break;

	case CF_REGVAR:
	    /* Variable in register bank */
	    sprintf (lbuf, "regbank+%u", (unsigned)((label+offs) & 0xFFFF));
	    break;

	default:
	    Internal ("Invalid address flags");
    }

    /* Return a pointer to the static buffer */
    return lbuf;
}



/*****************************************************************************/
/*		    	      Pre- and postamble			     */
/*****************************************************************************/



void g_preamble (void)
/* Generate the assembler code preamble */
{
    /* Generate the global segments and push them */
    PushCodeSeg (NewCodeSeg (""));
    PushDataSeg (NewDataSeg (""));
			       
    /* Identify the compiler version */
    AddDataSegLine (DS, "; File generated by cc65 v %u.%u.%u",
		    VER_MAJOR, VER_MINOR, VER_PATCH);

    /* Insert some object file options */
    AddDataSegLine (DS, ".fopt\t\tcompiler,\"cc65 v %u.%u.%u\"",
		    VER_MAJOR, VER_MINOR, VER_PATCH);

    /* If we're producing code for some other CPU, switch the command set */
    if (CPU == CPU_65C02) {
	AddDataSegLine (DS, ".pc02");
    }

    /* Allow auto import for runtime library routines */
    AddDataSegLine (DS, ".autoimport\ton");

    /* Switch the assembler into case sensitive mode */
    AddDataSegLine (DS, ".case\t\ton");

    /* Tell the assembler if we want to generate debug info */
    AddDataSegLine (DS, ".debuginfo\t%s", (DebugInfo != 0)? "on" : "off");

    /* Import the stack pointer for direct auto variable access */
    AddDataSegLine (DS, ".importzp\tsp, sreg, regsave, regbank, tmp1, ptr1");

    /* Define long branch macros */
    AddDataSegLine (DS, ".macpack\tlongbranch");
}



/*****************************************************************************/
/*	   			Segment support				     */
/*****************************************************************************/



static void UseSeg (int NewSeg)
/* Switch to a specific segment */
{
    if (CurSeg != NewSeg) {
  	CurSeg = (segment_t) NewSeg;
	if (CurSeg != SEG_CODE) {
	    AddDataSegLine (DS, ".segment\t\"%s\"", SegmentNames [CurSeg]);
	}
    }
}



void g_usecode (void)
/* Switch to the code segment */
{
    UseSeg (SEG_CODE);
}



void g_userodata (void)
/* Switch to the read only data segment */
{
    UseSeg (SEG_RODATA);
}



void g_usedata (void)
/* Switch to the data segment */
{
    UseSeg (SEG_DATA);
}



void g_usebss (void)
/* Switch to the bss segment */
{
    UseSeg (SEG_BSS);
}



static void SegName (segment_t Seg, const char* Name)
/* Set the name of a segment */
{
    /* Free the old name and set a new one */
    NewSegName (Seg, Name);

    /* If the new segment is the current segment, emit a segment directive
     * with the new name.
     */
    if (Seg == CurSeg) {
       	CurSeg = SEG_INV;	/* Invalidate */
	UseSeg (Seg);
    }
}



void g_codename (const char* Name)
/* Set the name of the CODE segment */
{
    SegName (SEG_CODE, Name);
}



void g_rodataname (const char* Name)
/* Set the name of the RODATA segment */
{
    SegName (SEG_RODATA, Name);
}



void g_dataname (const char* Name)
/* Set the name of the DATA segment */
{
    SegName (SEG_DATA, Name);
}



void g_bssname (const char* Name)
/* Set the name of the BSS segment */
{
    SegName (SEG_BSS, Name);
}



/*****************************************************************************/
/*  	       		 	     Code				     */
/*****************************************************************************/



unsigned sizeofarg (unsigned flags)
/* Return the size of a function argument type that is encoded in flags */
{
    switch (flags & CF_TYPE) {

	case CF_CHAR:
	    return (flags & CF_FORCECHAR)? 1 : 2;

	case CF_INT:
	    return 2;

	case CF_LONG:
	    return 4;

	default:
	    typeerror (flags);
	    /* NOTREACHED */
	    return 2;
    }
}



int pop (unsigned flags)
/* Pop an argument of the given size */
{
    return oursp += sizeofarg (flags);
}



int push (unsigned flags)
/* Push an argument of the given size */
{
    return oursp -= sizeofarg (flags);
}



static unsigned MakeByteOffs (unsigned Flags, unsigned Offs)
/* The value in Offs is an offset to an address in a/x. Make sure, an object
 * of the type given in Flags can be loaded or stored into this address by
 * adding part of the offset to the address in ax, so that the remaining
 * offset fits into an index register. Return the remaining offset.
 */
{
    /* If the offset is too large for a byte register, add the high byte
     * of the offset to the primary. Beware: We need a special correction
     * if the offset in the low byte will overflow in the operation.
     */
    unsigned O = Offs & ~0xFFU;
    if ((Offs & 0xFF) > 256 - sizeofarg (Flags)) {
	/* We need to add the low byte also */
	O += Offs & 0xFF;
    }

    /* Do the correction if we need one */
    if (O != 0) {
     	g_inc (CF_INT | CF_CONST, O);
     	Offs -= O;
    }

    /* Return the new offset */
    return Offs;
}



/*****************************************************************************/
/*		  	Functions handling local labels			     */
/*****************************************************************************/



void g_defloclabel (unsigned label)
/* Define a local label */
{
    if (CurSeg == SEG_CODE) {
     	AddLocCodeLabel (CS, LocalLabelName (label));
    } else {
     	AddDataSegLine (DS, "%s:", LocalLabelName (label));
    }
}



/*****************************************************************************/
/*   	     	       Functions handling global labels			     */
/*****************************************************************************/



void g_defgloblabel (const char* Name)
/* Define a global label with the given name */
{
    if (CurSeg == SEG_CODE) {
	/* ##### */
	char Buf[64];
	xsprintf (Buf, sizeof (Buf), "_%s", Name);
       	AddExtCodeLabel (CS, Buf);
    } else {
       	AddDataSegLine (DS, "_%s:", Name);
    }
}



void g_defexport (const char* Name, int ZP)
/* Export the given label */
{
    if (ZP) {
       	AddDataSegLine (DS, "\t.exportzp\t_%s", Name);
    } else {
     	AddDataSegLine (DS, "\t.export\t\t_%s", Name);
    }
}



void g_defimport (const char* Name, int ZP)
/* Import the given label */
{
    if (ZP) {
       	AddDataSegLine (DS, "\t.importzp\t_%s", Name);
    } else {
     	AddDataSegLine (DS, "\t.import\t\t_%s", Name);
    }
}



/*****************************************************************************/
/*     		     Load functions for various registers		     */
/*****************************************************************************/



static void ldaconst (unsigned val)
/* Load a with a constant */
{
    AddCodeSegLine (CS, "lda #$%02X", val & 0xFF);
}



static void ldxconst (unsigned val)
/* Load x with a constant */
{
    AddCodeSegLine (CS, "ldx #$%02X", val & 0xFF);
}



static void ldyconst (unsigned val)
/* Load y with a constant */
{
    AddCodeSegLine (CS, "ldy #$%02X", val & 0xFF);
}



/*****************************************************************************/
/*     			    Function entry and exit			     */
/*****************************************************************************/



/* Remember the argument size of a function. The variable is set by g_enter
 * and used by g_leave. If the functions gets its argument size by the caller
 * (variable param list or function without prototype), g_enter will set the
 * value to -1.
 */
static int funcargs;


void g_enter (unsigned flags, unsigned argsize)
/* Function prologue */
{
    if ((flags & CF_FIXARGC) != 0) {
	/* Just remember the argument size for the leave */
	funcargs = argsize;
    } else {
       	funcargs = -1;
       	AddCodeSegLine (CS, "jsr enter");
    }
}



void g_leave (int flags, int val)
/* Function epilogue */
{
    int k;
    char buf [40];

    /* CF_REG is set if we're returning a value from the function */
    if ((flags & CF_REG) == 0) {
	AddCodeHint ("x:-");
	AddCodeHint ("a:-");
    }

    /* How many bytes of locals do we have to drop? */
    k = -oursp;

    /* If we didn't have a variable argument list, don't call leave */
    if (funcargs >= 0) {

     	/* Load a function return code if needed */
     	if ((flags & CF_CONST) != 0) {
     	    g_getimmed (flags, val, 0);
     	}

     	/* Drop stackframe or leave with rts */
     	k += funcargs;
     	if (k == 0) {
	    AddCodeHint ("y:-");	/* Y register no longer used */
     	    AddCodeSegLine (CS, "rts");
     	} else if (k <= 8) {
	    AddCodeHint ("y:-");	/* Y register no longer used */
     	    AddCodeSegLine (CS, "jmp incsp%d", k);
     	} else {
     	    CheckLocalOffs (k);
     	    ldyconst (k);
     	    AddCodeSegLine (CS, "jmp addysp");
     	}

    } else {

     	strcpy (buf, "\tjmp\tleave");
     	if (k) {
     	    /* We've a stack frame to drop */
     	    ldyconst (k);
     	    strcat (buf, "y");
     	} else {
	    /* Y register no longer used */
	    AddCodeHint ("y:-");
	}
     	if (flags & CF_CONST) {
     	    if ((flags & CF_TYPE) != CF_LONG) {
     	   	/* Constant int sized value given for return code */
     	   	if (val == 0) {
     	   	    /* Special case: return 0 */
     	   	    strcat (buf, "00");
       	       	} else if (((val >> 8) & 0xFF) == 0) {
     	   	    /* Special case: constant with high byte zero */
     	   	    ldaconst (val);    		/* Load low byte */
     		    strcat (buf, "0");
     		} else {
     		    /* Others: arbitrary constant value */
     		    g_getimmed (flags, val, 0);	/* Load value */
     		}
     	    } else {
     		/* Constant long value: No shortcut possible */
     		g_getimmed (flags, val, 0);
     	    }
     	}

     	/* Output the jump */
     	AddCodeSegLine (CS, buf);
    }
}



/*****************************************************************************/
/*   		       	      Register variables			     */
/*****************************************************************************/



void g_save_regvars (int RegOffs, unsigned Bytes)
/* Save register variables */
{
    /* Don't loop for up to two bytes */
    if (Bytes == 1) {

     	AddCodeSegLine (CS, "lda regbank%+d", RegOffs);
       	AddCodeSegLine (CS, "jsr pusha");

    } else if (Bytes == 2) {

       	AddCodeSegLine (CS, "lda regbank%+d", RegOffs);
	AddCodeSegLine (CS, "ldx regbank%+d", RegOffs+1);
       	AddCodeSegLine (CS, "jsr pushax");

    } else {

     	/* More than two bytes - loop */
     	unsigned Label = GetLocalLabel ();
     	g_space (Bytes);
     	ldyconst (Bytes - 1);
     	ldxconst (Bytes);
     	g_defloclabel (Label);
	AddCodeSegLine (CS, "lda regbank%+d,x", RegOffs-1);
	AddCodeSegLine (CS, "sta (sp),y");
     	AddCodeSegLine (CS, "dey");
     	AddCodeSegLine (CS, "dex");
     	AddCodeSegLine (CS, "bne L%04X", Label);

    }

    /* We pushed stuff, correct the stack pointer */
    oursp -= Bytes;
}



void g_restore_regvars (int StackOffs, int RegOffs, unsigned Bytes)
/* Restore register variables */
{
    /* Calculate the actual stack offset and check it */
    StackOffs -= oursp;
    CheckLocalOffs (StackOffs);

    /* Don't loop for up to two bytes */
    if (Bytes == 1) {

     	ldyconst (StackOffs);
     	AddCodeSegLine (CS, "lda (sp),y");
	AddCodeSegLine (CS, "sta regbank%+d", RegOffs);

    } else if (Bytes == 2) {

     	ldyconst (StackOffs);
     	AddCodeSegLine (CS, "lda (sp),y");
	AddCodeSegLine (CS, "sta regbank%+d", RegOffs);
	AddCodeSegLine (CS, "iny");
	AddCodeSegLine (CS, "lda (sp),y");
	AddCodeSegLine (CS, "sta regbank%+d", RegOffs+1);

    } else {

     	/* More than two bytes - loop */
     	unsigned Label = GetLocalLabel ();
     	ldyconst (StackOffs+Bytes-1);
     	ldxconst (Bytes);
     	g_defloclabel (Label);
	AddCodeSegLine (CS, "lda (sp),y");
	AddCodeSegLine (CS, "sta regbank%+d,x", RegOffs-1);
	AddCodeSegLine (CS, "dey");
	AddCodeSegLine (CS, "dex");
	AddCodeSegLine (CS, "bne L%04X", Label);

    }
}



/*****************************************************************************/
/*			     Fetching memory cells	   		     */
/*****************************************************************************/



void g_getimmed (unsigned flags, unsigned long val, unsigned offs)
/* Load a constant into the primary register */
{
    if ((flags & CF_CONST) != 0) {

     	/* Numeric constant */
     	switch (flags & CF_TYPE) {

     	    case CF_CHAR:
     		if ((flags & CF_FORCECHAR) != 0) {
     		    ldaconst (val);
     		    break;
     		}
     		/* FALL THROUGH */
     	    case CF_INT:
     		ldxconst ((val >> 8) & 0xFF);
     		ldaconst (val & 0xFF);
     		break;

     	    case CF_LONG:
     		if (val < 0x100) {
     		    AddCodeSegLine (CS, "ldx #$00");
     		    AddCodeSegLine (CS, "stx sreg+1");
     		    AddCodeSegLine (CS, "stx sreg");
     		    AddCodeSegLine (CS, "lda #$%02X", (unsigned char) val);
     		} else if ((val & 0xFFFF00FF) == 0) {
     		    AddCodeSegLine (CS, "lda #$00");
     		    AddCodeSegLine (CS, "sta sreg+1");
     		    AddCodeSegLine (CS, "sta sreg");
     		    AddCodeSegLine (CS, "ldx #$%02X", (unsigned char) (val >> 8));
     		} else if ((val & 0xFFFF0000) == 0 && CodeSizeFactor > 140) {
     		    AddCodeSegLine (CS, "lda #$00");
     		    AddCodeSegLine (CS, "sta sreg+1");
     		    AddCodeSegLine (CS, "sta sreg");
     		    AddCodeSegLine (CS, "lda #$%02X", (unsigned char) val);
     		    AddCodeSegLine (CS, "ldx #$%02X", (unsigned char) (val >> 8));
     		} else if ((val & 0xFFFFFF00) == 0xFFFFFF00) {
     		    AddCodeSegLine (CS, "ldx #$FF");
     		    AddCodeSegLine (CS, "stx sreg+1");
     		    AddCodeSegLine (CS, "stx sreg");
     		    if ((val & 0xFF) == 0xFF) {
     			AddCodeSegLine (CS, "txa");
     		    } else {
     			AddCodeSegLine (CS, "lda #$%02X", (unsigned char) val);
     		    }
     		} else if ((val & 0xFFFF00FF) == 0xFFFF00FF) {
     		    AddCodeSegLine (CS, "lda #$FF");
     		    AddCodeSegLine (CS, "sta sreg+1");
     		    AddCodeSegLine (CS, "sta sreg");
     		    AddCodeSegLine (CS, "ldx #$%02X", (unsigned char) (val >> 8));
     		} else {
     		    /* Call a subroutine that will load following value */
     		    AddCodeSegLine (CS, "jsr ldeax");
     		    AddCodeSegLine (CS, ".dword $%08lX", val & 0xFFFFFFFF);
     		}
     		break;

     	    default:
     		typeerror (flags);
     		break;

     	}

    } else {

	/* Some sort of label */
	const char* Label = GetLabelName (flags, val, offs);

	/* Load the address into the primary */
	AddCodeSegLine (CS, "lda #<(%s)", Label);
	AddCodeSegLine (CS, "ldx #>(%s)", Label);

    }
}



void g_getstatic (unsigned flags, unsigned long label, unsigned offs)
/* Fetch an static memory cell into the primary register */
{
    /* Create the correct label name */
    char* lbuf = GetLabelName (flags, label, offs);

    /* Check the size and generate the correct load operation */
    switch (flags & CF_TYPE) {

     	case CF_CHAR:
     	    if ((flags & CF_FORCECHAR) || (flags & CF_TEST)) {
     	        AddCodeSegLine (CS, "lda %s", lbuf);	/* load A from the label */
       	    } else {
     	     	ldxconst (0);
     	     	AddCodeSegLine (CS, "lda %s", lbuf);	/* load A from the label */
     	     	if (!(flags & CF_UNSIGNED)) {
     	     	    /* Must sign extend */
     	     	    AddCodeSegLine (CS, "bpl *+3");
     	     	    AddCodeSegLine (CS, "dex");
     		    AddCodeHint ("x:!");		/* X is invalid now */
     	     	}
     	    }
     	    break;

     	case CF_INT:
     	    AddCodeSegLine (CS, "lda %s", lbuf);
     	    if (flags & CF_TEST) {
     		AddCodeSegLine (CS, "ora %s+1", lbuf);
     	    } else {
     		AddCodeSegLine (CS, "ldx %s+1", lbuf);
     	    }
     	    break;

   	case CF_LONG:
     	    if (flags & CF_TEST) {
	     	AddCodeSegLine (CS, "lda %s+3", lbuf);
		AddCodeSegLine (CS, "ora %s+2", lbuf);
		AddCodeSegLine (CS, "ora %s+1", lbuf);
		AddCodeSegLine (CS, "ora %s+0", lbuf);
	    } else {
	     	AddCodeSegLine (CS, "lda %s+3", lbuf);
	     	AddCodeSegLine (CS, "sta sreg+1");
		AddCodeSegLine (CS, "lda %s+2", lbuf);
		AddCodeSegLine (CS, "sta sreg");
		AddCodeSegLine (CS, "ldx %s+1", lbuf);
		AddCodeSegLine (CS, "lda %s", lbuf);
	    }
	    break;

       	default:
       	    typeerror (flags);

    }
}



void g_getlocal (unsigned flags, int offs)
/* Fetch specified local object (local var). */
{
    offs -= oursp;
    CheckLocalOffs (offs);
    switch (flags & CF_TYPE) {

	case CF_CHAR:
	    if ((flags & CF_FORCECHAR) || (flags & CF_TEST)) {
		if (CPU == CPU_65C02 && offs == 0) {
		    AddCodeSegLine (CS, "lda (sp)");
		} else {
		    ldyconst (offs);
		    AddCodeSegLine (CS, "lda (sp),y");
		}
	    } else {
		if (offs == 0) {
		    AddCodeSegLine (CS, "ldx #$00");
		    AddCodeSegLine (CS, "lda (sp,x)");
		} else {
		    ldyconst (offs);
		    AddCodeSegLine (CS, "ldx #$00");
		    AddCodeSegLine (CS, "lda (sp),y");
		}
     	    	if ((flags & CF_UNSIGNED) == 0) {
     	    	    AddCodeSegLine (CS, "bpl *+3");
     	 	    AddCodeSegLine (CS, "dex");
     		    AddCodeHint ("x:!");	/* X is invalid now */
	 	}
	    }
	    break;

	case CF_INT:
	    CheckLocalOffs (offs + 1);
       	    if (flags & CF_TEST) {
	    	ldyconst (offs + 1);
	    	AddCodeSegLine (CS, "lda (sp),y");
		AddCodeSegLine (CS, "dey");
		AddCodeSegLine (CS, "ora (sp),y");
	    } else {
		if (CodeSizeFactor > 180) {
	    	    ldyconst (offs + 1);
		    AddCodeSegLine (CS, "lda (sp),y");
		    AddCodeSegLine (CS, "tax");
		    AddCodeSegLine (CS, "dey");
		    AddCodeSegLine (CS, "lda (sp),y");
		} else {
		    if (offs) {
			ldyconst (offs+1);
       			AddCodeSegLine (CS, "jsr ldaxysp");
		    } else {
			AddCodeSegLine (CS, "jsr ldax0sp");
		    }
		}
	    }
	    break;

	case CF_LONG:
    	    if (offs) {
    	 	ldyconst (offs+3);
    	 	AddCodeSegLine (CS, "jsr ldeaxysp");
    	    } else {
    	 	AddCodeSegLine (CS, "jsr ldeax0sp");
    	    }
       	    break;

       	default:
    	    typeerror (flags);
    }
}



void g_getind (unsigned flags, unsigned offs)
/* Fetch the specified object type indirect through the primary register
 * into the primary register
 */
{
    /* If the offset is greater than 255, add the part that is > 255 to
     * the primary. This way we get an easy addition and use the low byte
     * as the offset
     */
    offs = MakeByteOffs (flags, offs);

    /* Handle the indirect fetch */
    switch (flags & CF_TYPE) {

     	case CF_CHAR:
       	    /* Character sized */
     	    if (offs) {
     		ldyconst (offs);
     	        if (flags & CF_UNSIGNED) {
     	     	    AddCodeSegLine (CS, "jsr ldauidx");
       	     	} else {
     	     	    AddCodeSegLine (CS, "jsr ldaidx");
     	     	}
     	    } else {
     	        if (flags & CF_UNSIGNED) {
     		    if (CodeSizeFactor > 250) {
     			AddCodeSegLine (CS, "sta ptr1");
     			AddCodeSegLine (CS, "stx ptr1+1");
     		     	AddCodeSegLine (CS, "ldx #$00");
     			AddCodeSegLine (CS, "lda (ptr1,x)");
     		    } else {
     	     	        AddCodeSegLine (CS, "jsr ldaui");
     		    }
     	     	} else {
       	     	    AddCodeSegLine (CS, "jsr ldai");
     	     	}
       	    }
     	    break;

     	case CF_INT:
     	    if (flags & CF_TEST) {
     		ldyconst (offs);
     		AddCodeSegLine (CS, "sta ptr1");
     		AddCodeSegLine (CS, "stx ptr1+1");
     		AddCodeSegLine (CS, "lda (ptr1),y");
     		AddCodeSegLine (CS, "iny");
     		AddCodeSegLine (CS, "ora (ptr1),y");
     	    } else {
     		if (offs == 0) {
     		    AddCodeSegLine (CS, "jsr ldaxi");
     		} else {
     		    ldyconst (offs+1);
     		    AddCodeSegLine (CS, "jsr ldaxidx");
     		}
     	    }
     	    break;

       	case CF_LONG:
     	    if (offs == 0) {
     		AddCodeSegLine (CS, "jsr ldeaxi");
     	    } else {
     		ldyconst (offs+3);
     		AddCodeSegLine (CS, "jsr ldeaxidx");
     	    }
     	    if (flags & CF_TEST) {
       		AddCodeSegLine (CS, "jsr tsteax");
     	    }
     	    break;

     	default:
     	    typeerror (flags);

    }
}



void g_leasp (int offs)
/* Fetch the address of the specified symbol into the primary register */
{
    /* Calculate the offset relative to sp */
    offs -= oursp;

    /* For value 0 we do direct code */
    if (offs == 0) {
       	AddCodeSegLine (CS, "lda sp");
       	AddCodeSegLine (CS, "ldx sp+1");
    } else {
       	if (CodeSizeFactor < 300) {
       	    ldaconst (offs);         		/* Load A with offset value */
       	    AddCodeSegLine (CS, "jsr leaasp");	/* Load effective address */
       	} else {
       	    if (CPU == CPU_65C02 && offs == 1) {
       	     	AddCodeSegLine (CS, "lda sp");
       	     	AddCodeSegLine (CS, "ldx sp+1");
       	    	AddCodeSegLine (CS, "ina");
       	     	AddCodeSegLine (CS, "bne *+3");
       	     	AddCodeSegLine (CS, "inx");
       	     	AddCodeHint ("x:!");		/* Invalidate X */
       	    } else {
       	     	ldaconst (offs);
       	     	AddCodeSegLine (CS, "clc");
       	     	AddCodeSegLine (CS, "ldx sp+1");
       	     	AddCodeSegLine (CS, "adc sp");
       	     	AddCodeSegLine (CS, "bcc *+3");
       	     	AddCodeSegLine (CS, "inx");
       	     	AddCodeHint ("x:!");		/* Invalidate X */
       	    }
       	}
    }
}



void g_leavariadic (int Offs)
/* Fetch the address of a parameter in a variadic function into the primary
 * register
 */
{
    unsigned ArgSizeOffs;

    /* Calculate the offset relative to sp */
    Offs -= oursp;

    /* Get the offset of the parameter which is stored at sp+0 on function
     * entry and check if this offset is reachable with a byte offset.
     */
    CHECK (oursp <= 0);
    ArgSizeOffs = -oursp;
    CheckLocalOffs (ArgSizeOffs);

    /* Get the size of all parameters. */
    if (ArgSizeOffs == 0 && CPU == CPU_65C02) {
       	AddCodeSegLine (CS, "lda (sp)");
    } else {
       	ldyconst (ArgSizeOffs);
       	AddCodeSegLine (CS, "lda (sp),y");
    }

    /* Add the value of the stackpointer */
    if (CodeSizeFactor > 250) {
       	AddCodeSegLine (CS, "ldx sp+1");
       	AddCodeSegLine (CS, "clc");
       	AddCodeSegLine (CS, "adc sp");
       	AddCodeSegLine (CS, "bcc *+3");
       	AddCodeSegLine (CS, "inx");
       	AddCodeHint ("x:!");		/* Invalidate X */
    } else {
       	AddCodeSegLine (CS, "jsr leaasp");
    }

    /* Add the offset to the primary */
    if (Offs > 0) {
	g_inc (CF_INT | CF_CONST, Offs);
    } else if (Offs < 0) {
	g_dec (CF_INT | CF_CONST, -Offs);
    }
}



/*****************************************************************************/
/*     	    		       Store into memory			     */
/*****************************************************************************/



void g_putstatic (unsigned flags, unsigned long label, unsigned offs)
/* Store the primary register into the specified static memory cell */
{
    /* Create the correct label name */
    char* lbuf = GetLabelName (flags, label, offs);

    /* Check the size and generate the correct store operation */
    switch (flags & CF_TYPE) {

     	case CF_CHAR:
    	    AddCodeSegLine (CS, "sta %s", lbuf);
     	    break;

     	case CF_INT:
    	    AddCodeSegLine (CS, "sta %s", lbuf);
	    AddCodeSegLine (CS, "stx %s+1", lbuf);
     	    break;

     	case CF_LONG:
    	    AddCodeSegLine (CS, "sta %s", lbuf);
	    AddCodeSegLine (CS, "stx %s+1", lbuf);
	    AddCodeSegLine (CS, "ldy sreg");
	    AddCodeSegLine (CS, "sty %s+2", lbuf);
	    AddCodeSegLine (CS, "ldy sreg+1");
     	    AddCodeSegLine (CS, "sty %s+3", lbuf);
     	    break;

       	default:
       	    typeerror (flags);

    }
}



void g_putlocal (unsigned Flags, int Offs, long Val)
/* Put data into local object. */
{
    Offs -= oursp;
    CheckLocalOffs (Offs);
    switch (Flags & CF_TYPE) {

     	case CF_CHAR:
	    if (Flags & CF_CONST) {
	     	AddCodeSegLine (CS, "lda #$%02X", (unsigned char) Val);
	    }
     	    if (CPU == CPU_65C02 && Offs == 0) {
     	     	AddCodeSegLine (CS, "sta (sp)");
     	    } else {
     	     	ldyconst (Offs);
     	     	AddCodeSegLine (CS, "sta (sp),y");
     	    }
     	    break;

     	case CF_INT:
	    if (Flags & CF_CONST) {
		ldyconst (Offs+1);
		AddCodeSegLine (CS, "lda #$%02X", (unsigned char) (Val >> 8));
		AddCodeSegLine (CS, "sta (sp),y");
		if ((Flags & CF_NOKEEP) == 0) {
		    /* Place high byte into X */
		    AddCodeSegLine (CS, "tax");
		}
		if (CPU == CPU_65C02 && Offs == 0) {
		    AddCodeSegLine (CS, "lda #$%02X", (unsigned char) Val);
		    AddCodeSegLine (CS, "sta (sp)");
		} else {
		    if ((Val & 0xFF) == Offs+1) {
			/* The value we need is already in Y */
			AddCodeSegLine (CS, "tya");
			AddCodeSegLine (CS, "dey");
		    } else {
			AddCodeSegLine (CS, "dey");
			AddCodeSegLine (CS, "lda #$%02X", (unsigned char) Val);
		    }
		    AddCodeSegLine (CS, "sta (sp),y");
		}
	    } else {
		if ((Flags & CF_NOKEEP) == 0 || CodeSizeFactor < 160) {
		    if (Offs) {
			ldyconst (Offs);
			AddCodeSegLine (CS, "jsr staxysp");
		    } else {
			AddCodeSegLine (CS, "jsr stax0sp");
		    }
		} else {
		    if (CPU == CPU_65C02 && Offs == 0) {
			AddCodeSegLine (CS, "sta (sp)");
		     	ldyconst (1);
		     	AddCodeSegLine (CS, "txa");
		     	AddCodeSegLine (CS, "sta (sp),y");
		    } else {
			ldyconst (Offs);
			AddCodeSegLine (CS, "sta (sp),y");
			AddCodeSegLine (CS, "iny");
			AddCodeSegLine (CS, "txa");
			AddCodeSegLine (CS, "sta (sp),y");
		    }
		}
	    }
     	    break;

     	case CF_LONG:
	    if (Flags & CF_CONST) {
	     	g_getimmed (Flags, Val, 0);
	    }
     	    if (Offs) {
     	     	ldyconst (Offs);
     	     	AddCodeSegLine (CS, "jsr steaxysp");
     	    } else {
     	     	AddCodeSegLine (CS, "jsr steax0sp");
     	    }
     	    break;

       	default:
     	    typeerror (Flags);

    }
}



void g_putind (unsigned Flags, unsigned Offs)
/* Store the specified object type in the primary register at the address
 * on the top of the stack
 */
{
    /* We can handle offsets below $100 directly, larger offsets must be added
     * to the address. Since a/x is in use, best code is achieved by adding
     * just the high byte. Be sure to check if the low byte will overflow while
     * while storing.
     */
    if ((Offs & 0xFF) > 256 - sizeofarg (Flags | CF_FORCECHAR)) {

	/* Overflow - we need to add the low byte also */
	AddCodeSegLine (CS, "ldy #$00");
	AddCodeSegLine (CS, "clc");
	AddCodeSegLine (CS, "pha");
	AddCodeSegLine (CS, "lda #$%02X", Offs & 0xFF);
	AddCodeSegLine (CS, "adc (sp),y");
	AddCodeSegLine (CS, "sta (sp),y");
	AddCodeSegLine (CS, "iny");
       	AddCodeSegLine (CS, "lda #$%02X", (Offs >> 8) & 0xFF);
	AddCodeSegLine (CS, "adc (sp),y");
	AddCodeSegLine (CS, "sta (sp),y");
	AddCodeSegLine (CS, "pla");

	/* Complete address is on stack, new offset is zero */
	Offs = 0;

    } else if ((Offs & 0xFF00) != 0) {

	/* We can just add the high byte */
	AddCodeSegLine (CS, "ldy #$01");
	AddCodeSegLine (CS, "clc");
     	AddCodeSegLine (CS, "pha");
	AddCodeSegLine (CS, "lda #$%02X", (Offs >> 8) & 0xFF);
	AddCodeSegLine (CS, "adc (sp),y");
	AddCodeSegLine (CS, "sta (sp),y");
	AddCodeSegLine (CS, "pla");

	/* Offset is now just the low byte */
	Offs &= 0x00FF;
    }

    /* Check the size and determine operation */
    switch (Flags & CF_TYPE) {

     	case CF_CHAR:
     	    if (Offs) {
     	        ldyconst (Offs);
     	       	AddCodeSegLine (CS, "jsr staspidx");
     	    } else {
     	     	AddCodeSegLine (CS, "jsr staspp");
     	    }
     	    break;

     	case CF_INT:
     	    if (Offs) {
     	        ldyconst (Offs);
     	     	AddCodeSegLine (CS, "jsr staxspidx");
     	    } else {
     	     	AddCodeSegLine (CS, "jsr staxspp");
     	    }
     	    break;

     	case CF_LONG:
     	    if (Offs) {
     	        ldyconst (Offs);
     	     	AddCodeSegLine (CS, "jsr steaxspidx");
     	    } else {
     	     	AddCodeSegLine (CS, "jsr steaxspp");
     	    }
     	    break;

     	default:
     	    typeerror (Flags);

    }

    /* Pop the argument which is always a pointer */
    pop (CF_PTR);
}



/*****************************************************************************/
/*		      type conversion and similiar stuff		     */
/*****************************************************************************/



void g_toslong (unsigned flags)
/* Make sure, the value on TOS is a long. Convert if necessary */
{
    switch (flags & CF_TYPE) {

	case CF_CHAR:
	case CF_INT:
	    if (flags & CF_UNSIGNED) {
		AddCodeSegLine (CS, "jsr tosulong");
	    } else {
		AddCodeSegLine (CS, "jsr toslong");
	    }
	    push (CF_INT);
	    break;

	case CF_LONG:
	    break;

	default:
	    typeerror (flags);
    }
}



void g_tosint (unsigned flags)
/* Make sure, the value on TOS is an int. Convert if necessary */
{
    switch (flags & CF_TYPE) {

	case CF_CHAR:
	case CF_INT:
	    break;

	case CF_LONG:
	    AddCodeSegLine (CS, "jsr tosint");
	    pop (CF_INT);
	    break;

	default:
	    typeerror (flags);
    }
}



void g_reglong (unsigned flags)
/* Make sure, the value in the primary register a long. Convert if necessary */
{
    switch (flags & CF_TYPE) {

	case CF_CHAR:
	case CF_INT:
	    if (flags & CF_UNSIGNED) {
	    	if (CodeSizeFactor >= 200) {
	    	    ldyconst (0);
	    	    AddCodeSegLine (CS, "sty sreg");
		    AddCodeSegLine (CS, "sty sreg+1");
	    	} else {
	       	    AddCodeSegLine (CS, "jsr axulong");
	    	}
	    } else {
	    	AddCodeSegLine (CS, "jsr axlong");
	    }
	    break;

	case CF_LONG:
	    break;

	default:
	    typeerror (flags);
    }
}



unsigned g_typeadjust (unsigned lhs, unsigned rhs)
/* Adjust the integer operands before doing a binary operation. lhs is a flags
 * value, that corresponds to the value on TOS, rhs corresponds to the value
 * in (e)ax. The return value is the the flags value for the resulting type.
 */
{
    unsigned ltype, rtype;
    unsigned result;

    /* Get the type spec from the flags */
    ltype = lhs & CF_TYPE;
    rtype = rhs & CF_TYPE;

    /* Check if a conversion is needed */
    if (ltype == CF_LONG && rtype != CF_LONG && (rhs & CF_CONST) == 0) {
   	/* We must promote the primary register to long */
   	g_reglong (rhs);
   	/* Get the new rhs type */
   	rhs = (rhs & ~CF_TYPE) | CF_LONG;
   	rtype = CF_LONG;
    } else if (ltype != CF_LONG && (lhs & CF_CONST) == 0 && rtype == CF_LONG) {
   	/* We must promote the lhs to long */
	if (lhs & CF_REG) {
	    g_reglong (lhs);
	} else {
   	    g_toslong (lhs);
	}
   	/* Get the new rhs type */
   	lhs = (lhs & ~CF_TYPE) | CF_LONG;
   	ltype = CF_LONG;
    }

    /* Determine the result type for the operation:
     *	- The result is const if both operands are const.
     *	- The result is unsigned if one of the operands is unsigned.
     *	- The result is long if one of the operands is long.
     *	- Otherwise the result is int sized.
     */
    result = (lhs & CF_CONST) & (rhs & CF_CONST);
    result |= (lhs & CF_UNSIGNED) | (rhs & CF_UNSIGNED);
    if (rtype == CF_LONG || ltype == CF_LONG) {
	result |= CF_LONG;
    } else {
	result |= CF_INT;
    }
    return result;
}



unsigned g_typecast (unsigned lhs, unsigned rhs)
/* Cast the value in the primary register to the operand size that is flagged
 * by the lhs value. Return the result value.
 */
{
    unsigned ltype, rtype;

    /* Get the type spec from the flags */
    ltype = lhs & CF_TYPE;
    rtype = rhs & CF_TYPE;

    /* Check if a conversion is needed */
    if (ltype == CF_LONG && rtype != CF_LONG && (rhs & CF_CONST) == 0) {
	/* We must promote the primary register to long */
	g_reglong (rhs);
    }

    /* Do not need any other action. If the left type is int, and the primary
     * register is long, it will be automagically truncated. If the right hand
     * side is const, it is not located in the primary register and handled by
     * the expression parser code.
     */

    /* Result is const if the right hand side was const */
    lhs |= (rhs & CF_CONST);

    /* The resulting type is that of the left hand side (that's why you called
     * this function :-)
     */
    return lhs;
}



void g_scale (unsigned flags, long val)
/* Scale the value in the primary register by the given value. If val is positive,
 * scale up, is val is negative, scale down. This function is used to scale
 * the operands or results of pointer arithmetic by the size of the type, the
 * pointer points to.
 */
{
    int p2;

    /* Value may not be zero */
    if (val == 0) {
       	Internal ("Data type has no size");
    } else if (val > 0) {

     	/* Scale up */
     	if ((p2 = powerof2 (val)) > 0 && p2 <= 3) {

     	    /* Factor is 2, 4 or 8, use special function */
     	    switch (flags & CF_TYPE) {

     		case CF_CHAR:
     		    if (flags & CF_FORCECHAR) {
     		     	while (p2--) {
     		     	    AddCodeSegLine (CS, "asl a");
     	     	     	}
     	     	     	break;
     	     	    }
     	     	    /* FALLTHROUGH */

     	     	case CF_INT:
		    if (CodeSizeFactor >= (p2+1)*130U) {
     	     		AddCodeSegLine (CS, "stx tmp1");
     	     	  	while (p2--) {
     	     		    AddCodeSegLine (CS, "asl a");
	     		    AddCodeSegLine (CS, "rol tmp1");
     	     		}
     	     		AddCodeSegLine (CS, "ldx tmp1");
     	     	    } else {
     	     	       	if (flags & CF_UNSIGNED) {
     	     	     	    AddCodeSegLine (CS, "jsr shlax%d", p2);
     	     	     	} else {
     	     	     	    AddCodeSegLine (CS, "jsr aslax%d", p2);
     	     	     	}
     	     	    }
     	     	    break;

     	     	case CF_LONG:
     	     	    if (flags & CF_UNSIGNED) {
     	     	     	AddCodeSegLine (CS, "jsr shleax%d", p2);
     	     	    } else {
     	     		AddCodeSegLine (CS, "jsr asleax%d", p2);
     	     	    }
     	     	    break;

     		default:
     		    typeerror (flags);

     	    }

     	} else if (val != 1) {

       	    /* Use a multiplication instead */
     	    g_mul (flags | CF_CONST, val);

     	}

    } else {

     	/* Scale down */
     	val = -val;
     	if ((p2 = powerof2 (val)) > 0 && p2 <= 3) {

     	    /* Factor is 2, 4 or 8, use special function */
     	    switch (flags & CF_TYPE) {

     		case CF_CHAR:
     		    if (flags & CF_FORCECHAR) {
     			if (flags & CF_UNSIGNED) {
     			    while (p2--) {
     			      	AddCodeSegLine (CS, "lsr a");
     			    }
     			    break;
     			} else if (p2 <= 2) {
     		  	    AddCodeSegLine (CS, "cmp #$80");
     			    AddCodeSegLine (CS, "ror a");
     			    break;
     			}
     		    }
     		    /* FALLTHROUGH */

     		case CF_INT:
     		    if (flags & CF_UNSIGNED) {
			if (CodeSizeFactor >= (p2+1)*130U) {
			    AddCodeSegLine (CS, "stx tmp1");
			    while (p2--) {
	     		    	AddCodeSegLine (CS, "lsr tmp1");
				AddCodeSegLine (CS, "ror a");
			    }
			    AddCodeSegLine (CS, "ldx tmp1");
			} else {
     			    AddCodeSegLine (CS, "jsr lsrax%d", p2);
			}
     		    } else {
			if (CodeSizeFactor >= (p2+1)*150U) {
			    AddCodeSegLine (CS, "stx tmp1");
			    while (p2--) {
			    	AddCodeSegLine (CS, "cpx #$80");
    			    	AddCodeSegLine (CS, "ror tmp1");
			    	AddCodeSegLine (CS, "ror a");
			    }
			    AddCodeSegLine (CS, "ldx tmp1");
			} else {
     			    AddCodeSegLine (CS, "jsr asrax%d", p2);
	    	     	}
     		    }
     		    break;

     		case CF_LONG:
     		    if (flags & CF_UNSIGNED) {
     		     	AddCodeSegLine (CS, "jsr lsreax%d", p2);
     		    } else {
     		       	AddCodeSegLine (CS, "jsr asreax%d", p2);
     		    }
     		    break;

     		default:
     		    typeerror (flags);

     	    }

     	} else if (val != 1) {

       	    /* Use a division instead */
     	    g_div (flags | CF_CONST, val);

     	}
    }
}



/*****************************************************************************/
/*	     	Adds and subs of variables fix a fixed address		     */
/*****************************************************************************/



void g_addlocal (unsigned flags, int offs)
/* Add a local variable to ax */
{
    /* Correct the offset and check it */
    offs -= oursp;
    CheckLocalOffs (offs);

    switch (flags & CF_TYPE) {

     	case CF_CHAR:
	    AddCodeSegLine (CS, "ldy #$%02X", offs & 0xFF);
	    AddCodeSegLine (CS, "clc");
	    AddCodeSegLine (CS, "adc (sp),y");
	    AddCodeSegLine (CS, "bcc *+3");
	    AddCodeSegLine (CS, "inx");
	    AddCodeHint ("x:!");
	    break;

     	case CF_INT:
     	    AddCodeSegLine (CS, "ldy #$%02X", offs & 0xFF);
     	    AddCodeSegLine (CS, "clc");
     	    AddCodeSegLine (CS, "adc (sp),y");
     	    AddCodeSegLine (CS, "pha");
     	    AddCodeSegLine (CS, "txa");
     	    AddCodeSegLine (CS, "iny");
     	    AddCodeSegLine (CS, "adc (sp),y");
     	    AddCodeSegLine (CS, "tax");
     	    AddCodeSegLine (CS, "pla");
     	    break;

     	case CF_LONG:
     	    /* Do it the old way */
       	    g_push (flags, 0);
     	    g_getlocal (flags, offs);
     	    g_add (flags, 0);
     	    break;

     	default:
     	    typeerror (flags);

    }
}



void g_addstatic (unsigned flags, unsigned long label, unsigned offs)
/* Add a static variable to ax */
{
    /* Create the correct label name */
    char* lbuf = GetLabelName (flags, label, offs);

    switch (flags & CF_TYPE) {

	case CF_CHAR:
	    AddCodeSegLine (CS, "clc");
	    AddCodeSegLine (CS, "adc %s", lbuf);
	    AddCodeSegLine (CS, "bcc *+3");
	    AddCodeSegLine (CS, "inx");
	    AddCodeHint ("x:!");
	    break;

	case CF_INT:
	    AddCodeSegLine (CS, "clc");
	    AddCodeSegLine (CS, "adc %s", lbuf);
	    AddCodeSegLine (CS, "tay");
	    AddCodeSegLine (CS, "txa");
     	    AddCodeSegLine (CS, "adc %s+1", lbuf);
	    AddCodeSegLine (CS, "tax");
	    AddCodeSegLine (CS, "tya");
	    break;

	case CF_LONG:
	    /* Do it the old way */
       	    g_push (flags, 0);
	    g_getstatic (flags, label, offs);
	    g_add (flags, 0);
	    break;

	default:
	    typeerror (flags);

    }
}



/*****************************************************************************/
/*	       Compares of ax with a variable with fixed address	     */
/*****************************************************************************/



void g_cmplocal (unsigned flags, int offs)
/* Compare a local variable to ax */
{
    Internal ("g_cmplocal not implemented");
}



void g_cmpstatic (unsigned flags, unsigned label, unsigned offs)
/* Compare a static variable to ax */
{
    Internal ("g_cmpstatic not implemented");
}



/*****************************************************************************/
/*   	    	       	     Special op= functions			     */
/*****************************************************************************/



void g_addeqstatic (unsigned flags, unsigned long label, unsigned offs,
       	    	    unsigned long val)
/* Emit += for a static variable */
{
    /* Create the correct label name */
    char* lbuf = GetLabelName (flags, label, offs);

    /* Check the size and determine operation */
    switch (flags & CF_TYPE) {

       	case CF_CHAR:
       	    if (flags & CF_FORCECHAR) {
     	       	AddCodeSegLine (CS, "ldx #$00");
       	    	if (flags & CF_CONST) {
     	    	    if (val == 1) {
     	    	   	AddCodeSegLine (CS, "inc %s", lbuf);
     	     	   	AddCodeSegLine (CS, "lda %s", lbuf);
     	     	    } else {
       	       	       	AddCodeSegLine (CS, "lda #$%02X", (int)(val & 0xFF));
     	     	   	AddCodeSegLine (CS, "clc");
     	     	   	AddCodeSegLine (CS, "adc %s", lbuf);
     	     		AddCodeSegLine (CS, "sta %s", lbuf);
     	     	    }
       	       	} else {
     	     	    AddCodeSegLine (CS, "clc");
       	     	    AddCodeSegLine (CS, "adc %s", lbuf);
     	     	    AddCodeSegLine (CS, "sta %s", lbuf);
       	     	}
     	    	if ((flags & CF_UNSIGNED) == 0) {
     	    	    AddCodeSegLine (CS, "bpl *+3");
     		    AddCodeSegLine (CS, "dex");
     		    AddCodeHint ("x:!");	       	/* Invalidate X */
     		}
       		break;
       	    }
       	    /* FALLTHROUGH */

       	case CF_INT:
       	    if (flags & CF_CONST) {
     		if (val == 1) {
     		    label = GetLocalLabel ();
     		    AddCodeSegLine (CS, "inc %s", lbuf);
     		    AddCodeSegLine (CS, "bne L%04X", (int)label);
     		    AddCodeSegLine (CS, "inc %s+1", lbuf);
     		    g_defloclabel (label);
     		    AddCodeSegLine (CS, "lda %s", lbuf);  		/* Hmmm... */
     		    AddCodeSegLine (CS, "ldx %s+1", lbuf);
     		} else {
       	       	    AddCodeSegLine (CS, "lda #$%02X", (int)(val & 0xFF));
     		    AddCodeSegLine (CS, "clc");
     		    AddCodeSegLine (CS, "adc %s", lbuf);
     		    AddCodeSegLine (CS, "sta %s", lbuf);
     		    if (val < 0x100) {
     		       	label = GetLocalLabel ();
     		       	AddCodeSegLine (CS, "bcc L%04X", (int)label);
     		       	AddCodeSegLine (CS, "inc %s+1", lbuf);
       		       	g_defloclabel (label);
     		       	AddCodeSegLine (CS, "ldx %s+1", lbuf);
     		    } else {
       	       	       	AddCodeSegLine (CS, "lda #$%02X", (unsigned char)(val >> 8));
     		       	AddCodeSegLine (CS, "adc %s+1", lbuf);
     		       	AddCodeSegLine (CS, "sta %s+1", lbuf);
     		       	AddCodeSegLine (CS, "tax");
     		       	AddCodeSegLine (CS, "lda %s", lbuf);
     		    }
     		}
       	    } else {
     		AddCodeSegLine (CS, "clc");
       		AddCodeSegLine (CS, "adc %s", lbuf);
       		AddCodeSegLine (CS, "sta %s", lbuf);
       		AddCodeSegLine (CS, "txa");
       		AddCodeSegLine (CS, "adc %s+1", lbuf);
       	     	AddCodeSegLine (CS, "sta %s+1", lbuf);
       	     	AddCodeSegLine (CS, "tax");
	     	AddCodeSegLine (CS, "lda %s", lbuf);
	    }
       	    break;

       	case CF_LONG:
	    if (flags & CF_CONST) {
		if (val < 0x100) {
     		    AddCodeSegLine (CS, "ldy #<(%s)", lbuf);
		    AddCodeSegLine (CS, "sty ptr1");
		    AddCodeSegLine (CS, "ldy #>(%s+1)", lbuf);
		    if (val == 1) {
			AddCodeSegLine (CS, "jsr laddeq1");
		    } else {
			AddCodeSegLine (CS, "lda #$%02X", (int)(val & 0xFF));
		     	AddCodeSegLine (CS, "jsr laddeqa");
		    }
		} else {
		    g_getstatic (flags, label, offs);
		    g_inc (flags, val);
		    g_putstatic (flags, label, offs);
		}
	    } else {
		AddCodeSegLine (CS, "ldy #<(%s)", lbuf);
		AddCodeSegLine (CS, "sty ptr1");
		AddCodeSegLine (CS, "ldy #>(%s+1)", lbuf);
		AddCodeSegLine (CS, "jsr laddeq");
	    }
       	    break;

       	default:
       	    typeerror (flags);
    }
}



void g_addeqlocal (unsigned flags, int offs, unsigned long val)
/* Emit += for a local variable */
{
    /* Calculate the true offset, check it, load it into Y */
    offs -= oursp;
    CheckLocalOffs (offs);

    /* Check the size and determine operation */
    switch (flags & CF_TYPE) {

       	case CF_CHAR:
       	    if (flags & CF_FORCECHAR) {
       	     	if (offs == 0) {
       	     	    AddCodeSegLine (CS, "ldx #$00");
       	     	    if (flags & CF_CONST) {
       	     	       	AddCodeSegLine (CS, "clc");
       	       	       	AddCodeSegLine (CS, "lda #$%02X", (int)(val & 0xFF));
       	     	       	AddCodeSegLine (CS, "adc (sp,x)");
       	     	       	AddCodeSegLine (CS, "sta (sp,x)");
       	     	    } else {
       	     	       	AddCodeSegLine (CS, "clc");
       	     	       	AddCodeSegLine (CS, "adc (sp,x)");
       	     	       	AddCodeSegLine (CS, "sta (sp,x)");
       	     	    }
       	     	} else {
       	     	    ldyconst (offs);
     	     	    AddCodeSegLine (CS, "ldx #$00");
     	     	    if (flags & CF_CONST) {
     	     	       	AddCodeSegLine (CS, "clc");
       	       	       	AddCodeSegLine (CS, "lda #$%02X", (int)(val & 0xFF));
     	     		AddCodeSegLine (CS, "adc (sp),y");
     	     		AddCodeSegLine (CS, "sta (sp),y");
     	     	    } else {
     	     	 	AddCodeSegLine (CS, "clc");
     	     		AddCodeSegLine (CS, "adc (sp),y");
     	     		AddCodeSegLine (CS, "sta (sp),y");
     	     	    }
     	     	}
     	     	if ((flags & CF_UNSIGNED) == 0) {
     	     	    AddCodeSegLine (CS, "bpl *+3");
     	     	    AddCodeSegLine (CS, "dex");
     	     	    AddCodeHint ("x:!");	/* Invalidate X */
     	     	}
       	     	break;
       	    }
       	    /* FALLTHROUGH */

       	case CF_INT:
     	    if (flags & CF_CONST) {
     	     	g_getimmed (flags, val, 0);
     	    }
     	    if (offs == 0) {
     	     	AddCodeSegLine (CS, "jsr addeq0sp");
     	    } else {
     	     	ldyconst (offs);
     	     	AddCodeSegLine (CS, "jsr addeqysp");
     	    }
       	    break;

       	case CF_LONG:
     	    if (flags & CF_CONST) {
	     	g_getimmed (flags, val, 0);
	    }
	    if (offs == 0) {
		AddCodeSegLine (CS, "jsr laddeq0sp");
	    } else {
		ldyconst (offs);
		AddCodeSegLine (CS, "jsr laddeqysp");
	    }
       	    break;

       	default:
       	    typeerror (flags);
    }
}



void g_addeqind (unsigned flags, unsigned offs, unsigned long val)
/* Emit += for the location with address in ax */
{
    /* If the offset is too large for a byte register, add the high byte
     * of the offset to the primary. Beware: We need a special correction
     * if the offset in the low byte will overflow in the operation.
     */
    offs = MakeByteOffs (flags, offs);

    /* Check the size and determine operation */
    switch (flags & CF_TYPE) {

       	case CF_CHAR:
	    AddCodeSegLine (CS, "sta ptr1");
	    AddCodeSegLine (CS, "stx ptr1+1");
	    if (offs == 0) {
		AddCodeSegLine (CS, "ldx #$00");
		AddCodeSegLine (CS, "lda #$%02X", (int)(val & 0xFF));
		AddCodeSegLine (CS, "clc");
		AddCodeSegLine (CS, "adc (ptr1,x)");
		AddCodeSegLine (CS, "sta (ptr1,x)");
	    } else {
     		AddCodeSegLine (CS, "ldy #$%02X", offs);
       	       	AddCodeSegLine (CS, "ldx #$00");
       	       	AddCodeSegLine (CS, "lda #$%02X", (int)(val & 0xFF));
       	       	AddCodeSegLine (CS, "clc");
       	       	AddCodeSegLine (CS, "adc (ptr1),y");
       	       	AddCodeSegLine (CS, "sta (ptr1),y");
	    }
     	    break;

       	case CF_INT:
	    if (CodeSizeFactor >= 200) {
		/* Lots of code, use only if size is not important */
       	       	AddCodeSegLine (CS, "sta ptr1");
		AddCodeSegLine (CS, "stx ptr1+1");
		AddCodeSegLine (CS, "ldy #$%02X", offs);
		AddCodeSegLine (CS, "lda #$%02X", (int)(val & 0xFF));
		AddCodeSegLine (CS, "clc");
		AddCodeSegLine (CS, "adc (ptr1),y");
		AddCodeSegLine (CS, "sta (ptr1),y");
		AddCodeSegLine (CS, "pha");
		AddCodeSegLine (CS, "iny");
		AddCodeSegLine (CS, "lda #$%02X", (unsigned char)(val >> 8));
		AddCodeSegLine (CS, "adc (ptr1),y");
		AddCodeSegLine (CS, "sta (ptr1),y");
		AddCodeSegLine (CS, "tax");
		AddCodeSegLine (CS, "pla");
		break;
	    }
	    /* FALL THROUGH */

       	case CF_LONG:
       	    AddCodeSegLine (CS, "jsr pushax");  	/* Push the address */
	    push (flags);		    	/* Correct the internal sp */
	    g_getind (flags, offs);		/* Fetch the value */
	    g_inc (flags, val);	   		/* Increment value in primary */
	    g_putind (flags, offs);		/* Store the value back */
       	    break;

       	default:
       	    typeerror (flags);
    }
}



void g_subeqstatic (unsigned flags, unsigned long label, unsigned offs,
       		    unsigned long val)
/* Emit -= for a static variable */
{
    /* Create the correct label name */
    char* lbuf = GetLabelName (flags, label, offs);

    /* Check the size and determine operation */
    switch (flags & CF_TYPE) {

       	case CF_CHAR:
       	    if (flags & CF_FORCECHAR) {
       		AddCodeSegLine (CS, "ldx #$00");
       	  	if (flags & CF_CONST) {
       		    if (val == 1) {
       			AddCodeSegLine (CS, "dec %s", lbuf);
       			AddCodeSegLine (CS, "lda %s", lbuf);
       		    } else {
       		       	AddCodeSegLine (CS, "sec");
       		     	AddCodeSegLine (CS, "lda %s", lbuf);
       		     	AddCodeSegLine (CS, "sbc #$%02X", (int)(val & 0xFF));
       		     	AddCodeSegLine (CS, "sta %s", lbuf);
       		    }
       	  	} else {
       		    AddCodeSegLine (CS, "sec");
       		    AddCodeSegLine (CS, "sta tmp1");
       	  	    AddCodeSegLine (CS, "lda %s", lbuf);
       	       	    AddCodeSegLine (CS, "sbc tmp1");
       		    AddCodeSegLine (CS, "sta %s", lbuf);
       	  	}
       		if ((flags & CF_UNSIGNED) == 0) {
       		    AddCodeSegLine (CS, "bpl *+3");
       		    AddCodeSegLine (CS, "dex");
       		    AddCodeHint ("x:!");	       	/* Invalidate X */
       	     	}
       	  	break;
       	    }
       	    /* FALLTHROUGH */

       	case CF_INT:
	    AddCodeSegLine (CS, "sec");
     	    if (flags & CF_CONST) {
	       	AddCodeSegLine (CS, "lda %s", lbuf);
	  	AddCodeSegLine (CS, "sbc #$%02X", (unsigned char)val);
	  	AddCodeSegLine (CS, "sta %s", lbuf);
	   	if (val < 0x100) {
	  	    label = GetLocalLabel ();
	  	    AddCodeSegLine (CS, "bcs L%04X", (unsigned)label);
		    AddCodeSegLine (CS, "dec %s+1", lbuf);
		    g_defloclabel (label);
		    AddCodeSegLine (CS, "ldx %s+1", lbuf);
		} else {
		    AddCodeSegLine (CS, "lda %s+1", lbuf);
		    AddCodeSegLine (CS, "sbc #$%02X", (unsigned char)(val >> 8));
		    AddCodeSegLine (CS, "sta %s+1", lbuf);
		    AddCodeSegLine (CS, "tax");
		    AddCodeSegLine (CS, "lda %s", lbuf);
		}
	    } else {
		AddCodeSegLine (CS, "sta tmp1");
		AddCodeSegLine (CS, "lda %s", lbuf);
	        AddCodeSegLine (CS, "sbc tmp1");
		AddCodeSegLine (CS, "sta %s", lbuf);
       	       	AddCodeSegLine (CS, "stx tmp1");
		AddCodeSegLine (CS, "lda %s+1", lbuf);
		AddCodeSegLine (CS, "sbc tmp1");
		AddCodeSegLine (CS, "sta %s+1", lbuf);
		AddCodeSegLine (CS, "tax");
		AddCodeSegLine (CS, "lda %s", lbuf);
	    }
       	    break;

       	case CF_LONG:
	    if (flags & CF_CONST) {
		if (val < 0x100) {
		    AddCodeSegLine (CS, "ldy #<(%s)", lbuf);
		    AddCodeSegLine (CS, "sty ptr1");
		    AddCodeSegLine (CS, "ldy #>(%s+1)", lbuf);
		    if (val == 1) {
	     		AddCodeSegLine (CS, "jsr lsubeq1");
		    } else {
			AddCodeSegLine (CS, "lda #$%02X", (unsigned char)val);
			AddCodeSegLine (CS, "jsr lsubeqa");
		    }
     		} else {
		    g_getstatic (flags, label, offs);
		    g_dec (flags, val);
		    g_putstatic (flags, label, offs);
		}
	    } else {
		AddCodeSegLine (CS, "ldy #<(%s)", lbuf);
		AddCodeSegLine (CS, "sty ptr1");
		AddCodeSegLine (CS, "ldy #>(%s+1)", lbuf);
		AddCodeSegLine (CS, "jsr lsubeq");
       	    }
       	    break;

       	default:
       	    typeerror (flags);
    }
}



void g_subeqlocal (unsigned flags, int offs, unsigned long val)
/* Emit -= for a local variable */
{
    /* Calculate the true offset, check it, load it into Y */
    offs -= oursp;
    CheckLocalOffs (offs);

    /* Check the size and determine operation */
    switch (flags & CF_TYPE) {

       	case CF_CHAR:
       	    if (flags & CF_FORCECHAR) {
    	 	ldyconst (offs);
		AddCodeSegLine (CS, "ldx #$00");
       	 	AddCodeSegLine (CS, "sec");
		if (flags & CF_CONST) {
		    AddCodeSegLine (CS, "lda (sp),y");
		    AddCodeSegLine (CS, "sbc #$%02X", (unsigned char)val);
		} else {
		    AddCodeSegLine (CS, "sta tmp1");
	     	    AddCodeSegLine (CS, "lda (sp),y");
		    AddCodeSegLine (CS, "sbc tmp1");
		}
       	 	AddCodeSegLine (CS, "sta (sp),y");
		if ((flags & CF_UNSIGNED) == 0) {
	       	    AddCodeSegLine (CS, "bpl *+3");
		    AddCodeSegLine (CS, "dex");
		    AddCodeHint ("x:!");		/* Invalidate X */
		}
       	 	break;
       	    }
       	    /* FALLTHROUGH */

       	case CF_INT:
	    if (flags & CF_CONST) {
	     	g_getimmed (flags, val, 0);
	    }
	    if (offs == 0) {
	 	AddCodeSegLine (CS, "jsr subeq0sp");
	    } else {
	 	ldyconst (offs);
	 	AddCodeSegLine (CS, "jsr subeqysp");
	    }
       	    break;

       	case CF_LONG:
	    if (flags & CF_CONST) {
	     	g_getimmed (flags, val, 0);
	    }
	    if (offs == 0) {
		AddCodeSegLine (CS, "jsr lsubeq0sp");
	    } else {
		ldyconst (offs);
		AddCodeSegLine (CS, "jsr lsubeqysp");
	    }
       	    break;

       	default:
       	    typeerror (flags);
    }
}



void g_subeqind (unsigned flags, unsigned offs, unsigned long val)
/* Emit -= for the location with address in ax */
{
    /* If the offset is too large for a byte register, add the high byte
     * of the offset to the primary. Beware: We need a special correction
     * if the offset in the low byte will overflow in the operation.
     */
    offs = MakeByteOffs (flags, offs);

    /* Check the size and determine operation */
    switch (flags & CF_TYPE) {

       	case CF_CHAR:
	    AddCodeSegLine (CS, "sta ptr1");
	    AddCodeSegLine (CS, "stx ptr1+1");
	    if (offs == 0) {
	 	AddCodeSegLine (CS, "ldx #$00");
	       	AddCodeSegLine (CS, "lda (ptr1,x)");
       	       	AddCodeSegLine (CS, "sec");
	 	AddCodeSegLine (CS, "sbc #$%02X", (unsigned char)val);
	 	AddCodeSegLine (CS, "sta (ptr1,x)");
	    } else {
       	       	AddCodeSegLine (CS, "ldy #$%02X", offs);
	 	AddCodeSegLine (CS, "ldx #$00");
	 	AddCodeSegLine (CS, "lda (ptr1),y");
	 	AddCodeSegLine (CS, "sec");
	 	AddCodeSegLine (CS, "sbc #$%02X", (unsigned char)val);
		AddCodeSegLine (CS, "sta (ptr1),y");
	    }
     	    break;

       	case CF_INT:
	    if (CodeSizeFactor >= 200) {
		/* Lots of code, use only if size is not important */
		AddCodeSegLine (CS, "sta ptr1");
       	       	AddCodeSegLine (CS, "stx ptr1+1");
		AddCodeSegLine (CS, "ldy #$%02X", offs);
		AddCodeSegLine (CS, "lda (ptr1),y");
		AddCodeSegLine (CS, "sec");
		AddCodeSegLine (CS, "sbc #$%02X", (unsigned char)val);
		AddCodeSegLine (CS, "sta (ptr1),y");
		AddCodeSegLine (CS, "pha");
		AddCodeSegLine (CS, "iny");
		AddCodeSegLine (CS, "lda (ptr1),y");
		AddCodeSegLine (CS, "sbc #$%02X", (unsigned char)(val >> 8));
     		AddCodeSegLine (CS, "sta (ptr1),y");
	     	AddCodeSegLine (CS, "tax");
		AddCodeSegLine (CS, "pla");
		break;
	    }
	    /* FALL THROUGH */

       	case CF_LONG:
       	    AddCodeSegLine (CS, "jsr pushax");     	/* Push the address */
	    push (flags);  			/* Correct the internal sp */
	    g_getind (flags, offs);		/* Fetch the value */
	    g_dec (flags, val);			/* Increment value in primary */
	    g_putind (flags, offs);		/* Store the value back */
       	    break;

       	default:
       	    typeerror (flags);
    }
}



/*****************************************************************************/
/*		   Add a variable address to the value in ax		     */
/*****************************************************************************/



void g_addaddr_local (unsigned flags, int offs)
/* Add the address of a local variable to ax */
{
    /* Add the offset */
    offs -= oursp;
    if (offs != 0) {
	/* We cannot address more then 256 bytes of locals anyway */
	CheckLocalOffs (offs);
	AddCodeSegLine (CS, "clc");
	AddCodeSegLine (CS, "adc #$%02X", offs & 0xFF);
       	AddCodeSegLine (CS, "bcc *+4");	/* Do also skip the CLC insn below */
	AddCodeSegLine (CS, "inx");
	AddCodeHint ("x:!");    	       	/* Invalidate X */
    }

    /* Add the current stackpointer value */
    AddCodeSegLine (CS, "clc");
    AddCodeSegLine (CS, "adc sp");
    AddCodeSegLine (CS, "tay");
    AddCodeSegLine (CS, "txa");
    AddCodeSegLine (CS, "adc sp+1");
    AddCodeSegLine (CS, "tax");
    AddCodeSegLine (CS, "tya");
}



void g_addaddr_static (unsigned flags, unsigned long label, unsigned offs)
/* Add the address of a static variable to ax */
{
    /* Create the correct label name */
    char* lbuf = GetLabelName (flags, label, offs);

    /* Add the address to the current ax value */
    AddCodeSegLine (CS, "clc");
    AddCodeSegLine (CS, "adc #<(%s)", lbuf);
    AddCodeSegLine (CS, "tay");
    AddCodeSegLine (CS, "txa");
    AddCodeSegLine (CS, "adc #>(%s)", lbuf);
    AddCodeSegLine (CS, "tax");
    AddCodeSegLine (CS, "tya");
}



/*****************************************************************************/
/*			  	     					     */
/*****************************************************************************/



void g_save (unsigned flags)
/* Copy primary register to hold register. */
{
    /* Check the size and determine operation */
    switch (flags & CF_TYPE) {

	case CF_CHAR:
     	    if (flags & CF_FORCECHAR) {
	     	AddCodeSegLine (CS, "pha");
		break;
	    }
	    /* FALLTHROUGH */

	case CF_INT:
	    AddCodeSegLine (CS, "sta regsave");
	    AddCodeSegLine (CS, "stx regsave+1");
	    break;

	case CF_LONG:
	    AddCodeSegLine (CS, "jsr saveeax");
	    break;

	default:
	    typeerror (flags);
    }
}



void g_restore (unsigned flags)
/* Copy hold register to P. */
{
    /* Check the size and determine operation */
    switch (flags & CF_TYPE) {

	case CF_CHAR:
	    if (flags & CF_FORCECHAR) {
	       	AddCodeSegLine (CS, "pla");
	    	break;
	    }
	    /* FALLTHROUGH */

	case CF_INT:
	    AddCodeSegLine (CS, "lda regsave");
	    AddCodeSegLine (CS, "ldx regsave+1");
	    break;

	case CF_LONG:
	    AddCodeSegLine (CS, "jsr resteax");
	    break;

	default:
	    typeerror (flags);
    }
}



void g_cmp (unsigned flags, unsigned long val)
/* Immidiate compare. The primary register will not be changed, Z flag
 * will be set.
 */
{
    /* Check the size and determine operation */
    switch (flags & CF_TYPE) {

      	case CF_CHAR:
     	    if (flags & CF_FORCECHAR) {
	       	AddCodeSegLine (CS, "cmp #$%02X", (unsigned char)val);
     	    	break;
     	    }
     	    /* FALLTHROUGH */

     	case CF_INT:
	    AddCodeSegLine (CS, "cmp #$%02X", (unsigned char)val);
       	    AddCodeSegLine (CS, "bne *+4");
	    AddCodeSegLine (CS, "cpx #$%02X", (unsigned char)(val >> 8));
     	    break;

        case CF_LONG:
	    Internal ("g_cmp: Long compares not implemented");
	    break;

	default:
	    typeerror (flags);
    }
}



static void oper (unsigned flags, unsigned long val, char** subs)
/* Encode a binary operation. subs is a pointer to four groups of three
 * strings:
 *   	0-2	--> Operate on ints
 *	3-5	--> Operate on unsigneds
 *	6-8	--> Operate on longs
 *	9-11	--> Operate on unsigned longs
 *
 * The first subroutine names in each string group is used to encode an
 * operation with a zero constant, the second to encode an operation with
 * a 8 bit constant, and the third is used in all other cases.
 */
{
    unsigned offs;

    /* Determine the offset into the array */
    offs = (flags & CF_UNSIGNED)? 3 : 0;
    switch (flags & CF_TYPE) {
 	case CF_CHAR:
 	case CF_INT:
 	    break;

 	case CF_LONG:
 	    offs += 6;
 	    break;

 	default:
 	    typeerror (flags);
    }

    /* Encode the operation */
    if (flags & CF_CONST) {
 	/* Constant value given */
 	if (val == 0 && subs [offs+0]) {
 	    /* Special case: constant with value zero */
 	    AddCodeSegLine (CS, "jsr %s", subs [offs+0]);
 	} else if (val < 0x100 && subs [offs+1]) {
 	    /* Special case: constant with high byte zero */
 	    ldaconst (val);		/* Load low byte */
 	    AddCodeSegLine (CS, "jsr %s", subs [offs+1]);
 	} else {
 	    /* Others: arbitrary constant value */
 	    g_getimmed (flags, val, 0);   	       	/* Load value */
 	    AddCodeSegLine (CS, "jsr %s", subs [offs+2]);
 	}
    } else {
     	/* Value not constant (is already in (e)ax) */
 	AddCodeSegLine (CS, "jsr %s", subs [offs+2]);
    }

    /* The operation will pop it's argument */
    pop (flags);
}



void g_test (unsigned flags)
/* Force a test to set cond codes right */
{
    switch (flags & CF_TYPE) {

     	case CF_CHAR:
 	    if (flags & CF_FORCECHAR) {
 		AddCodeSegLine (CS, "tax");
 		break;
 	    }
 	    /* FALLTHROUGH */

     	case CF_INT:
 	    AddCodeSegLine (CS, "stx tmp1");
 	    AddCodeSegLine (CS, "ora tmp1");
     	    break;

     	case CF_LONG:
     	    if (flags & CF_UNSIGNED) {
     	    	AddCodeSegLine (CS, "jsr utsteax");
     	    } else {
     	    	AddCodeSegLine (CS, "jsr tsteax");
     	    }
     	    break;

     	default:
     	    typeerror (flags);

    }
}



void g_push (unsigned flags, unsigned long val)
/* Push the primary register or a constant value onto the stack */
{
    unsigned char hi;

    if (flags & CF_CONST && (flags & CF_TYPE) != CF_LONG) {

     	/* We have a constant 8 or 16 bit value */
     	if ((flags & CF_TYPE) == CF_CHAR && (flags & CF_FORCECHAR)) {

     	    /* Handle as 8 bit value */
	    if (CodeSizeFactor >= 165 || val > 2) {
     	    	ldaconst (val);
     	    	AddCodeSegLine (CS, "jsr pusha");
     	    } else {
     	    	AddCodeSegLine (CS, "jsr pushc%d", (int) val);
     	    }

     	} else {

     	    /* Handle as 16 bit value */
     	    hi = (unsigned char) (val >> 8);
     	    if (val <= 7) {
		AddCodeSegLine (CS, "jsr push%u", (unsigned) val);
     	    } else if (hi == 0 || hi == 0xFF) {
     	    	/* Use special function */
     	    	ldaconst (val);
       	       	AddCodeSegLine (CS, "jsr %s", (hi == 0)? "pusha0" : "pushaFF");
     	    } else {
     	    	/* Long way ... */
     	    	g_getimmed (flags, val, 0);
     	    	AddCodeSegLine (CS, "jsr pushax");
     	    }
     	}

    } else {

     	/* Value is not 16 bit or not constant */
     	if (flags & CF_CONST) {
     	    /* Constant 32 bit value, load into eax */
     	    g_getimmed (flags, val, 0);
     	}

     	/* Push the primary register */
     	switch (flags & CF_TYPE) {

     	    case CF_CHAR:
     		if (flags & CF_FORCECHAR) {
     		    /* Handle as char */
     		    AddCodeSegLine (CS, "jsr pusha");
     		    break;
     		}
     		/* FALL THROUGH */
     	    case CF_INT:
     		AddCodeSegLine (CS, "jsr pushax");
     		break;

     	    case CF_LONG:
     	     	AddCodeSegLine (CS, "jsr pusheax");
     		break;

     	    default:
     		typeerror (flags);

     	}

    }

    /* Adjust the stack offset */
    push (flags);
}



void g_swap (unsigned flags)
/* Swap the primary register and the top of the stack. flags give the type
 * of *both* values (must have same size).
 */
{
    switch (flags & CF_TYPE) {

	case CF_CHAR:
	case CF_INT:
	    AddCodeSegLine (CS, "jsr swapstk");
	    break;

	case CF_LONG:
	    AddCodeSegLine (CS, "jsr swapestk");
	    break;

	default:
	    typeerror (flags);

    }
}



void g_call (unsigned Flags, const char* Label, unsigned ArgSize)
/* Call the specified subroutine name */
{
    if ((Flags & CF_FIXARGC) == 0) {
	/* Pass the argument count */
	ldyconst (ArgSize);
    }
    AddCodeSegLine (CS, "jsr _%s", Label);
    oursp += ArgSize;		/* callee pops args */
}



void g_callind (unsigned Flags, unsigned ArgSize)
/* Call subroutine with address in AX */
{
    if ((Flags & CF_FIXARGC) == 0) {
	/* Pass arg count */
	ldyconst (ArgSize);
    }
    AddCodeSegLine (CS, "jsr callax");	/* do the call */
    oursp += ArgSize;			/* callee pops args */
}



void g_jump (unsigned Label)
/* Jump to specified internal label number */
{
    AddCodeSegLine (CS, "jmp L%04X", Label);
}



void g_switch (unsigned Flags)
/* Output switch statement preamble */
{
    switch (Flags & CF_TYPE) {

     	case CF_CHAR:
     	case CF_INT:
     	    AddCodeSegLine (CS, "jsr switch");
     	    break;

     	case CF_LONG:
     	    AddCodeSegLine (CS, "jsr lswitch");
     	    break;

     	default:
     	    typeerror (Flags);

    }
}



void g_case (unsigned flags, unsigned label, unsigned long val)
/* Create table code for one case selector */
{
    switch (flags & CF_TYPE) {

     	case CF_CHAR:
    	case CF_INT:
    	    AddCodeSegLine (CS, ".word $%04X, L%04X",
			 (unsigned)(val & 0xFFFF),
			 (unsigned)(label & 0xFFFF));
       	    break;

    	case CF_LONG:
	    AddCodeSegLine (CS, ".dword $%08lX", val);
	    AddCodeSegLine (CS, ".word L%04X", label & 0xFFFF);
    	    break;

    	default:
    	    typeerror (flags);

    }
}



void g_truejump (unsigned flags, unsigned label)
/* Jump to label if zero flag clear */
{
    if (flags & CF_SHORT) {
	AddCodeSegLine (CS, "bne L%04X", label);
    } else {
        AddCodeSegLine (CS, "jne L%04X", label);
    }
}



void g_falsejump (unsigned flags, unsigned label)
/* Jump to label if zero flag set */
{
    if (flags & CF_SHORT) {
    	AddCodeSegLine (CS, "beq L%04X", label);
    } else {
       	AddCodeSegLine (CS, "jeq L%04X", label);
    }
}



static void mod_internal (int k, char* verb1, char* verb2)
{
    if (k <= 8) {
	AddCodeSegLine (CS, "jsr %ssp%c", verb1, k + '0');
    } else {
	CheckLocalOffs (k);
	ldyconst (k);
	AddCodeSegLine (CS, "jsr %ssp", verb2);
    }
}



void g_space (int space)
/* Create or drop space on the stack */
{
    if (space < 0) {
     	mod_internal (-space, "inc", "addy");
    } else if (space > 0) {
     	mod_internal (space, "dec", "suby");
    }
}



void g_cstackcheck (void)
/* Check for a C stack overflow */
{
    AddCodeSegLine (CS, "jsr cstkchk");
}



void g_stackcheck (void)
/* Check for a stack overflow */
{
    AddCodeSegLine (CS, "jsr stkchk");
}



void g_add (unsigned flags, unsigned long val)
/* Primary = TOS + Primary */
{
    static char* ops [12] = {
     	0,		"tosadda0",	"tosaddax",
     	0,		"tosadda0",	"tosaddax",
     	0,		0,	 	"tosaddeax",
     	0,		0,	 	"tosaddeax",
    };

    if (flags & CF_CONST) {
    	flags &= ~CF_FORCECHAR;	/* Handle chars as ints */
     	g_push (flags & ~CF_CONST, 0);
    }
    oper (flags, val, ops);
}



void g_sub (unsigned flags, unsigned long val)
/* Primary = TOS - Primary */
{
    static char* ops [12] = {
     	0,		"tossuba0",	"tossubax",
     	0,		"tossuba0",	"tossubax",
     	0,		0,	 	"tossubeax",
     	0,		0,	 	"tossubeax",
    };

    if (flags & CF_CONST) {
    	flags &= ~CF_FORCECHAR;	/* Handle chars as ints */
     	g_push (flags & ~CF_CONST, 0);
    }
    oper (flags, val, ops);
}



void g_rsub (unsigned flags, unsigned long val)
/* Primary = Primary - TOS */
{
    static char* ops [12] = {
	0,		"tosrsuba0",	"tosrsubax",
	0,		"tosrsuba0",	"tosrsubax",
	0,		0,	 	"tosrsubeax",
	0,		0,	 	"tosrsubeax",
    };
    oper (flags, val, ops);
}



void g_mul (unsigned flags, unsigned long val)
/* Primary = TOS * Primary */
{
    static char* ops [12] = {
     	0,		"tosmula0",	"tosmulax",
     	0,   		"tosumula0",	"tosumulax",
     	0,		0,	 	"tosmuleax",
     	0,		0,	 	"tosumuleax",
    };

    int p2;

    /* Do strength reduction if the value is constant and a power of two */
    if (flags & CF_CONST && (p2 = powerof2 (val)) >= 0) {
     	/* Generate a shift instead */
     	g_asl (flags, p2);
	return;
    }

    /* If the right hand side is const, the lhs is not on stack but still
     * in the primary register.
     */
    if (flags & CF_CONST) {

      	switch (flags & CF_TYPE) {

      	    case CF_CHAR:
      		if (flags & CF_FORCECHAR) {
		    /* Handle some special cases */
		    switch (val) {

		     	case 3:
		     	    AddCodeSegLine (CS, "sta tmp1");
		     	    AddCodeSegLine (CS, "asl a");
		     	    AddCodeSegLine (CS, "clc");
		     	    AddCodeSegLine (CS, "adc tmp1");
		     	    return;

		     	case 5:
		     	    AddCodeSegLine (CS, "sta tmp1");
		     	    AddCodeSegLine (CS, "asl a");
		     	    AddCodeSegLine (CS, "asl a");
		     	    AddCodeSegLine (CS, "clc");
     		     	    AddCodeSegLine (CS, "adc tmp1");
		     	    return;

		     	case 10:
		     	    AddCodeSegLine (CS, "sta tmp1");
		     	    AddCodeSegLine (CS, "asl a");
		     	    AddCodeSegLine (CS, "asl a");
	     	     	    AddCodeSegLine (CS, "clc");
		     	    AddCodeSegLine (CS, "adc tmp1");
		     	    AddCodeSegLine (CS, "asl a");
		     	    return;
		    }
      		}
     		/* FALLTHROUGH */

	    case CF_INT:
		break;

	    case CF_LONG:
		break;

	    default:
		typeerror (flags);
	}

	/* If we go here, we didn't emit code. Push the lhs on stack and fall
	 * into the normal, non-optimized stuff.
	 */
    	flags &= ~CF_FORCECHAR;	/* Handle chars as ints */
     	g_push (flags & ~CF_CONST, 0);

    }

    /* Use long way over the stack */
    oper (flags, val, ops);
}



void g_div (unsigned flags, unsigned long val)
/* Primary = TOS / Primary */
{
    static char* ops [12] = {
     	0,		"tosdiva0",	"tosdivax",
     	0,		"tosudiva0",	"tosudivax",
	0,		0,  		"tosdiveax",
	0,		0,  		"tosudiveax",
    };

    /* Do strength reduction if the value is constant and a power of two */
    int p2;
    if ((flags & CF_CONST) && (p2 = powerof2 (val)) >= 0) {
	/* Generate a shift instead */
	g_asr (flags, p2);
    } else {
	/* Generate a division */
	if (flags & CF_CONST) {
	    /* lhs is not on stack */
    	    flags &= ~CF_FORCECHAR;	/* Handle chars as ints */
	    g_push (flags & ~CF_CONST, 0);
     	}
	oper (flags, val, ops);
    }
}



void g_mod (unsigned flags, unsigned long val)
/* Primary = TOS % Primary */
{
    static char* ops [12] = {
     	0,		"tosmoda0",	"tosmodax",
     	0,		"tosumoda0",	"tosumodax",
     	0,		0,  		"tosmodeax",
     	0,		0,  		"tosumodeax",
    };
    int p2;

    /* Check if we can do some cost reduction */
    if ((flags & CF_CONST) && (flags & CF_UNSIGNED) && val != 0xFFFFFFFF && (p2 = powerof2 (val)) >= 0) {
     	/* We can do that with an AND operation */
     	g_and (flags, val - 1);
    } else {
      	/* Do it the hard way... */
     	if (flags & CF_CONST) {
     	    /* lhs is not on stack */
    	    flags &= ~CF_FORCECHAR;	/* Handle chars as ints */
     	    g_push (flags & ~CF_CONST, 0);
     	}
      	oper (flags, val, ops);
    }
}



void g_or (unsigned flags, unsigned long val)
/* Primary = TOS | Primary */
{
    static char* ops [12] = {
      	0,  	     	"tosora0",	"tosorax",
      	0,  	     	"tosora0",	"tosorax",
      	0,  	     	0,  		"tosoreax",
      	0,  	     	0,     		"tosoreax",
    };

    /* If the right hand side is const, the lhs is not on stack but still
     * in the primary register.
     */
    if (flags & CF_CONST) {

      	switch (flags & CF_TYPE) {

      	    case CF_CHAR:
      		if (flags & CF_FORCECHAR) {
     		    if ((val & 0xFF) != 0xFF) {
       	       	        AddCodeSegLine (CS, "ora #$%02X", (unsigned char)val);
     		    }
      		    return;
      		}
     		/* FALLTHROUGH */

	    case CF_INT:
		if (val <= 0xFF) {
		    AddCodeSegLine (CS, "ora #$%02X", (unsigned char)val);
		    return;
     		}
		break;

	    case CF_LONG:
		if (val <= 0xFF) {
		    AddCodeSegLine (CS, "ora #$%02X", (unsigned char)val);
		    return;
		}
		break;

	    default:
		typeerror (flags);
	}

	/* If we go here, we didn't emit code. Push the lhs on stack and fall
	 * into the normal, non-optimized stuff.
	 */
	g_push (flags & ~CF_CONST, 0);

    }

    /* Use long way over the stack */
    oper (flags, val, ops);
}



void g_xor (unsigned flags, unsigned long val)
/* Primary = TOS ^ Primary */
{
    static char* ops [12] = {
	0,		"tosxora0",	"tosxorax",
	0,		"tosxora0",	"tosxorax",
	0,		0,	   	"tosxoreax",
	0,		0,	   	"tosxoreax",
    };


    /* If the right hand side is const, the lhs is not on stack but still
     * in the primary register.
     */
    if (flags & CF_CONST) {

      	switch (flags & CF_TYPE) {

      	    case CF_CHAR:
      		if (flags & CF_FORCECHAR) {
     		    if ((val & 0xFF) != 0) {
       	       	    	AddCodeSegLine (CS, "eor #$%02X", (unsigned char)val);
     		    }
      		    return;
      		}
     		/* FALLTHROUGH */

	    case CF_INT:
		if (val <= 0xFF) {
		    if (val != 0) {
		     	AddCodeSegLine (CS, "eor #$%02X", (unsigned char)val);
		    }
		    return;
		} else if ((val & 0xFF) == 0) {
		    AddCodeSegLine (CS, "pha");
	     	    AddCodeSegLine (CS, "txa");
		    AddCodeSegLine (CS, "eor #$%02X", (unsigned char)(val >> 8));
		    AddCodeSegLine (CS, "tax");
		    AddCodeSegLine (CS, "pla");
		    return;
		}
		break;

	    case CF_LONG:
		if (val <= 0xFF) {
		    if (val != 0) {
       	       	       	AddCodeSegLine (CS, "eor #$%02X", (unsigned char)val);
		    }
		    return;
		}
		break;

	    default:
		typeerror (flags);
	}

	/* If we go here, we didn't emit code. Push the lhs on stack and fall
	 * into the normal, non-optimized stuff.
	 */
	g_push (flags & ~CF_CONST, 0);

    }

    /* Use long way over the stack */
    oper (flags, val, ops);
}



void g_and (unsigned flags, unsigned long val)
/* Primary = TOS & Primary */
{
    static char* ops [12] = {
     	0,	     	"tosanda0",	"tosandax",
     	0,	     	"tosanda0",	"tosandax",
      	0,	     	0,		"tosandeax",
     	0,	     	0,		"tosandeax",
    };

    /* If the right hand side is const, the lhs is not on stack but still
     * in the primary register.
     */
    if (flags & CF_CONST) {

     	switch (flags & CF_TYPE) {

     	    case CF_CHAR:
     		if (flags & CF_FORCECHAR) {
     		    AddCodeSegLine (CS, "and #$%02X", (unsigned char)val);
     		    return;
     		}
     		/* FALLTHROUGH */
     	    case CF_INT:
		if ((val & 0xFFFF) != 0xFFFF) {
       	       	    if (val <= 0xFF) {
		    	ldxconst (0);
		    	if (val == 0) {
		    	    ldaconst (0);
		    	} else if (val != 0xFF) {
		       	    AddCodeSegLine (CS, "and #$%02X", (unsigned char)val);
		    	}
		    } else if ((val & 0xFF00) == 0xFF00) {
		    	AddCodeSegLine (CS, "and #$%02X", (unsigned char)val);
		    } else if ((val & 0x00FF) == 0x0000) {
			AddCodeSegLine (CS, "txa");
			AddCodeSegLine (CS, "and #$%02X", (unsigned char)(val >> 8));
			AddCodeSegLine (CS, "tax");
			ldaconst (0);
		    } else {
			AddCodeSegLine (CS, "tay");
			AddCodeSegLine (CS, "txa");
			AddCodeSegLine (CS, "and #$%02X", (unsigned char)(val >> 8));
			AddCodeSegLine (CS, "tax");
			AddCodeSegLine (CS, "tya");
			if ((val & 0x00FF) != 0x00FF) {
			    AddCodeSegLine (CS, "and #$%02X", (unsigned char)val);
			}
		    }
		}
		return;

	    case CF_LONG:
		if (val <= 0xFF) {
		    ldxconst (0);
		    AddCodeSegLine (CS, "stx sreg+1");
	     	    AddCodeSegLine (CS, "stx sreg");
		    if ((val & 0xFF) != 0xFF) {
		     	 AddCodeSegLine (CS, "and #$%02X", (unsigned char)val);
		    }
		    return;
		} else if (val == 0xFF00) {
		    ldaconst (0);
		    AddCodeSegLine (CS, "sta sreg+1");
		    AddCodeSegLine (CS, "sta sreg");
		    return;
		}
		break;

	    default:
		typeerror (flags);
	}

	/* If we go here, we didn't emit code. Push the lhs on stack and fall
	 * into the normal, non-optimized stuff.
	 */
	g_push (flags & ~CF_CONST, 0);

    }

    /* Use long way over the stack */
    oper (flags, val, ops);
}



void g_asr (unsigned flags, unsigned long val)
/* Primary = TOS >> Primary */
{
    static char* ops [12] = {
      	0,	     	"tosasra0",	"tosasrax",
      	0,	     	"tosshra0",	"tosshrax",
      	0,	     	0,		"tosasreax",
      	0,	     	0,		"tosshreax",
    };

    /* If the right hand side is const, the lhs is not on stack but still
     * in the primary register.
     */
    if (flags & CF_CONST) {

      	switch (flags & CF_TYPE) {

      	    case CF_CHAR:
      	    case CF_INT:
		if (val >= 1 && val <= 3) {
		    if (flags & CF_UNSIGNED) {
		       	AddCodeSegLine (CS, "jsr shrax%ld", val);
		    } else {
		       	AddCodeSegLine (CS, "jsr asrax%ld", val);
		    }
		    return;
		} else if (val == 8 && (flags & CF_UNSIGNED)) {
      		    AddCodeSegLine (CS, "txa");
      		    ldxconst (0);
		    return;
		}
		break;

	    case CF_LONG:
		if (val >= 1 && val <= 3) {
		    if (flags & CF_UNSIGNED) {
		       	AddCodeSegLine (CS, "jsr shreax%ld", val);
		    } else {
		       	AddCodeSegLine (CS, "jsr asreax%ld", val);
		    }
		    return;
		} else if (val == 8 && (flags & CF_UNSIGNED)) {
		    AddCodeSegLine (CS, "txa");
		    AddCodeSegLine (CS, "ldx sreg");
		    AddCodeSegLine (CS, "ldy sreg+1");
		    AddCodeSegLine (CS, "sty sreg");
		    AddCodeSegLine (CS, "ldy #$00");
     		    AddCodeSegLine (CS, "sty sreg+1");
		    return;
     		} else if (val == 16) {
		    AddCodeSegLine (CS, "ldy #$00");
		    AddCodeSegLine (CS, "ldx sreg+1");
		    if ((flags & CF_UNSIGNED) == 0) {
		        AddCodeSegLine (CS, "bpl *+3");
		        AddCodeSegLine (CS, "dey");
		        AddCodeHint ("y:!");
		    }
     		    AddCodeSegLine (CS, "lda sreg");
		    AddCodeSegLine (CS, "sty sreg+1");
		    AddCodeSegLine (CS, "sty sreg");
	     	    return;
		}
		break;

	    default:
		typeerror (flags);
	}

	/* If we go here, we didn't emit code. Push the lhs on stack and fall
      	 * into the normal, non-optimized stuff.
	 */
	g_push (flags & ~CF_CONST, 0);

    }

    /* Use long way over the stack */
    oper (flags, val, ops);
}



void g_asl (unsigned flags, unsigned long val)
/* Primary = TOS << Primary */
{
    static char* ops [12] = {
	0,	     	"tosasla0",    	"tosaslax",
	0,	     	"tosshla0",    	"tosshlax",
	0,	     	0,     	       	"tosasleax",
	0,	     	0,     	       	"tosshleax",
    };


    /* If the right hand side is const, the lhs is not on stack but still
     * in the primary register.
     */
    if (flags & CF_CONST) {

      	switch (flags & CF_TYPE) {

      	    case CF_CHAR:
      	    case CF_INT:
		if (val >= 1 && val <= 3) {
		    if (flags & CF_UNSIGNED) {
		       	AddCodeSegLine (CS, "jsr shlax%ld", val);
		    } else {
	     	    	AddCodeSegLine (CS, "jsr aslax%ld", val);
		    }
		    return;
      		} else if (val == 8) {
      		    AddCodeSegLine (CS, "tax");
      		    AddCodeSegLine (CS, "lda #$00");
     		    return;
     		}
     		break;

	    case CF_LONG:
		if (val >= 1 && val <= 3) {
		    if (flags & CF_UNSIGNED) {
		       	AddCodeSegLine (CS, "jsr shleax%ld", val);
		    } else {
		       	AddCodeSegLine (CS, "jsr asleax%ld", val);
		    }
		    return;
		} else if (val == 8) {
		    AddCodeSegLine (CS, "ldy sreg");
		    AddCodeSegLine (CS, "sty sreg+1");
		    AddCodeSegLine (CS, "stx sreg");
		    AddCodeSegLine (CS, "tax");
		    AddCodeSegLine (CS, "lda #$00");
		    return;
		} else if (val == 16) {
		    AddCodeSegLine (CS, "stx sreg+1");
		    AddCodeSegLine (CS, "sta sreg");
		    AddCodeSegLine (CS, "lda #$00");
		    AddCodeSegLine (CS, "tax");
		    return;
		}
		break;

	    default:
		typeerror (flags);
	}

	/* If we go here, we didn't emit code. Push the lhs on stack and fall
      	 * into the normal, non-optimized stuff.
	 */
	g_push (flags & ~CF_CONST, 0);

    }

    /* Use long way over the stack */
    oper (flags, val, ops);
}



void g_neg (unsigned flags)
/* Primary = -Primary */
{
    switch (flags & CF_TYPE) {

	case CF_CHAR:
     	case CF_INT:
	    AddCodeSegLine (CS, "jsr negax");
	    break;

	case CF_LONG:
	    AddCodeSegLine (CS, "jsr negeax");
	    break;

	default:
	    typeerror (flags);
    }
}



void g_bneg (unsigned flags)
/* Primary = !Primary */
{
    switch (flags & CF_TYPE) {

	case CF_CHAR:
	    AddCodeSegLine (CS, "jsr bnega");
	    break;

	case CF_INT:
	    AddCodeSegLine (CS, "jsr bnegax");
	    break;

	case CF_LONG:
     	    AddCodeSegLine (CS, "jsr bnegeax");
	    break;

	default:
	    typeerror (flags);
    }
}



void g_com (unsigned flags)
/* Primary = ~Primary */
{
    switch (flags & CF_TYPE) {

	case CF_CHAR:
	case CF_INT:
	    AddCodeSegLine (CS, "jsr complax");
	    break;

	case CF_LONG:
	    AddCodeSegLine (CS, "jsr compleax");
     	    break;

	default:
     	    typeerror (flags);
    }
}



void g_inc (unsigned flags, unsigned long val)
/* Increment the primary register by a given number */
{
    /* Don't inc by zero */
    if (val == 0) {
     	return;
    }

    /* Generate code for the supported types */
    flags &= ~CF_CONST;
    switch (flags & CF_TYPE) {

     	case CF_CHAR:
     	    if (flags & CF_FORCECHAR) {
		if (CPU == CPU_65C02 && val <= 2) {
		    while (val--) {
		 	AddCodeSegLine (CS, "ina");
		    }
	     	} else {
		    AddCodeSegLine (CS, "clc");
		    AddCodeSegLine (CS, "adc #$%02X", (unsigned char)val);
		}
     		break;
     	    }
     	    /* FALLTHROUGH */

     	case CF_INT:
	    if (CPU == CPU_65C02 && val == 1) {
		AddCodeSegLine (CS, "ina");
		AddCodeSegLine (CS, "bne *+3");
		AddCodeSegLine (CS, "inx");
		/* Tell the optimizer that the X register may be invalid */
		AddCodeHint ("x:!");
     	    } else if (CodeSizeFactor < 200) {
     		/* Use jsr calls */
     		if (val <= 8) {
     		    AddCodeSegLine (CS, "jsr incax%lu", val);
     		} else if (val <= 255) {
     		    ldyconst (val);
     		    AddCodeSegLine (CS, "jsr incaxy");
     		} else {
     		    g_add (flags | CF_CONST, val);
     		}
     	    } else {
     		/* Inline the code */
		if (val < 0x300) {
		    if ((val & 0xFF) != 0) {
		       	AddCodeSegLine (CS, "clc");
		       	AddCodeSegLine (CS, "adc #$%02X", (unsigned char) val);
		       	AddCodeSegLine (CS, "bcc *+3");
		       	AddCodeSegLine (CS, "inx");
		       	/* Tell the optimizer that the X register may be invalid */
       	       	       	AddCodeHint ("x:!");
		    }
     		    if (val >= 0x100) {
     		       	AddCodeSegLine (CS, "inx");
     		    }
     		    if (val >= 0x200) {
     		       	AddCodeSegLine (CS, "inx");
     		    }
     		} else {
		    AddCodeSegLine (CS, "clc");
		    if ((val & 0xFF) != 0) {
	     	       	AddCodeSegLine (CS, "adc #$%02X", (unsigned char) val);
		       	/* Tell the optimizer that the X register may be invalid */
		       	AddCodeHint ("x:!");
		    }
     		    AddCodeSegLine (CS, "pha");
     		    AddCodeSegLine (CS, "txa");
     		    AddCodeSegLine (CS, "adc #$%02X", (unsigned char) (val >> 8));
     		    AddCodeSegLine (CS, "tax");
     		    AddCodeSegLine (CS, "pla");
     		}
     	    }
     	    break;

       	case CF_LONG:
     	    if (val <= 255) {
     		ldyconst (val);
     		AddCodeSegLine (CS, "jsr inceaxy");
     	    } else {
     		g_add (flags | CF_CONST, val);
     	    }
     	    break;

     	default:
     	    typeerror (flags);

    }
}



void g_dec (unsigned flags, unsigned long val)
/* Decrement the primary register by a given number */
{
    /* Don't dec by zero */
    if (val == 0) {
     	return;
    }

    /* Generate code for the supported types */
    flags &= ~CF_CONST;
    switch (flags & CF_TYPE) {

     	case CF_CHAR:
	    if (flags & CF_FORCECHAR) {
		if (CPU == CPU_65C02 && val <= 2) {
		    while (val--) {
		 	AddCodeSegLine (CS, "dea");
		    }
		} else {
		    AddCodeSegLine (CS, "sec");
	     	    AddCodeSegLine (CS, "sbc #$%02X", (unsigned char)val);
		}
		break;
     	    }
	    /* FALLTHROUGH */

     	case CF_INT:
	    if (CodeSizeFactor < 200) {
		/* Use subroutines */
		if (val <= 8) {
		    AddCodeSegLine (CS, "jsr decax%d", (int) val);
		} else if (val <= 255) {
		    ldyconst (val);
		    AddCodeSegLine (CS, "jsr decaxy");
		} else {
		    g_sub (flags | CF_CONST, val);
		}
	    } else {
		/* Inline the code */
		if (val < 0x300) {
		    if ((val & 0xFF) != 0) {
		       	AddCodeSegLine (CS, "sec");
		       	AddCodeSegLine (CS, "sbc #$%02X", (unsigned char) val);
     		       	AddCodeSegLine (CS, "bcs *+3");
     		       	AddCodeSegLine (CS, "dex");
		       	/* Tell the optimizer that the X register may be invalid */
       	       	       	AddCodeHint ("x:!");
		    }
     		    if (val >= 0x100) {
     		       	AddCodeSegLine (CS, "dex");
     		    }
     		    if (val >= 0x200) {
     		       	AddCodeSegLine (CS, "dex");
     		    }
     		} else {
		    AddCodeSegLine (CS, "sec");
		    if ((val & 0xFF) != 0) {
	     	       	AddCodeSegLine (CS, "sbc #$%02X", (unsigned char) val);
		       	/* Tell the optimizer that the X register may be invalid */
		       	AddCodeHint ("x:!");
		    }
     		    AddCodeSegLine (CS, "pha");
     		    AddCodeSegLine (CS, "txa");
     		    AddCodeSegLine (CS, "sbc #$%02X", (unsigned char) (val >> 8));
     		    AddCodeSegLine (CS, "tax");
     		    AddCodeSegLine (CS, "pla");
     		}
	    }
     	    break;

     	case CF_LONG:
     	    if (val <= 255) {
     		ldyconst (val);
     		AddCodeSegLine (CS, "jsr deceaxy");
     	    } else {
     		g_sub (flags | CF_CONST, val);
	    }
	    break;

	default:
	    typeerror (flags);

    }
}



/*
 * Following are the conditional operators. They compare the TOS against
 * the primary and put a literal 1 in the primary if the condition is
 * true, otherwise they clear the primary register
 */



void g_eq (unsigned flags, unsigned long val)
/* Test for equal */
{
    static char* ops [12] = {
     	"toseq00",	"toseqa0",	"toseqax",
     	"toseq00",	"toseqa0",	"toseqax",
     	0,		0,		"toseqeax",
     	0,		0,		"toseqeax",
    };

    /* If the right hand side is const, the lhs is not on stack but still
     * in the primary register.
     */
    if (flags & CF_CONST) {

      	switch (flags & CF_TYPE) {

      	    case CF_CHAR:
		if (flags & CF_FORCECHAR) {
		    AddCodeSegLine (CS, "cmp #$%02X", (unsigned char)val);
		    AddCodeSegLine (CS, "jsr booleq");
		    return;
		}
     		/* FALLTHROUGH */

      	    case CF_INT:
     		AddCodeSegLine (CS, "cpx #$%02X", (unsigned char)(val >> 8));
       	       	AddCodeSegLine (CS, "bne *+4");
     		AddCodeSegLine (CS, "cmp #$%02X", (unsigned char)val);
     		AddCodeSegLine (CS, "jsr booleq");
     		return;

     	    case CF_LONG:
     		break;

     	    default:
     		typeerror (flags);
     	}

     	/* If we go here, we didn't emit code. Push the lhs on stack and fall
      	 * into the normal, non-optimized stuff.
     	 */
     	g_push (flags & ~CF_CONST, 0);

    }

    /* Use long way over the stack */
    oper (flags, val, ops);
}



void g_ne (unsigned flags, unsigned long val)
/* Test for not equal */
{
    static char* ops [12] = {
     	"tosne00",	"tosnea0",	"tosneax",
     	"tosne00",	"tosnea0",	"tosneax",
     	0,		0,		"tosneeax",
     	0,		0,		"tosneeax",
    };


    /* If the right hand side is const, the lhs is not on stack but still
     * in the primary register.
     */
    if (flags & CF_CONST) {

      	switch (flags & CF_TYPE) {

      	    case CF_CHAR:
     		if (flags & CF_FORCECHAR) {
     		    AddCodeSegLine (CS, "cmp #$%02X", (unsigned char)val);
     		    AddCodeSegLine (CS, "jsr boolne");
     		    return;
     		}
     		/* FALLTHROUGH */

      	    case CF_INT:
     		AddCodeSegLine (CS, "cpx #$%02X", (unsigned char)(val >> 8));
     		AddCodeSegLine (CS, "bne *+4");
     		AddCodeSegLine (CS, "cmp #$%02X", (unsigned char)val);
     		AddCodeSegLine (CS, "jsr boolne");
     		return;

     	    case CF_LONG:
     		break;

     	    default:
     		typeerror (flags);
     	}

     	/* If we go here, we didn't emit code. Push the lhs on stack and fall
      	 * into the normal, non-optimized stuff.
     	 */
     	g_push (flags & ~CF_CONST, 0);

    }

    /* Use long way over the stack */
    oper (flags, val, ops);
}



void g_lt (unsigned flags, unsigned long val)
/* Test for less than */
{
    static char* ops [12] = {
     	"toslt00",	"toslta0", 	"tosltax",
     	"tosult00",	"tosulta0",	"tosultax",
     	0,		0,    	   	"toslteax",
     	0,		0,    	   	"tosulteax",
    };

    /* If the right hand side is const, the lhs is not on stack but still
     * in the primary register.
     */
    if (flags & CF_CONST) {

     	/* Give a warning in some special cases */
     	if ((flags & CF_UNSIGNED) && val == 0) {
     	    Warning ("Condition is never true");
     	}

     	/* Look at the type */
     	switch (flags & CF_TYPE) {

     	    case CF_CHAR:
     		if (flags & CF_FORCECHAR) {
     		    AddCodeSegLine (CS, "cmp #$%02X", (unsigned char)val);
     		    if (flags & CF_UNSIGNED) {
     			AddCodeSegLine (CS, "jsr boolult");
     		    } else {
     		        AddCodeSegLine (CS, "jsr boollt");
     		    }
     		    return;
     		}
     	     	/* FALLTHROUGH */

     	    case CF_INT:
		if ((flags & CF_UNSIGNED) == 0 && val == 0) {
		    /* If we have a signed compare against zero, we only need to
		     * test the high byte.
		     */
		    AddCodeSegLine (CS, "txa");
		    AddCodeSegLine (CS, "jsr boollt");
		    return;
		}
		/* Direct code only for unsigned data types */
		if (flags & CF_UNSIGNED) {
		    AddCodeSegLine (CS, "cpx #$%02X", (unsigned char)(val >> 8));
       	       	    AddCodeSegLine (CS, "bne *+4");
     	 	    AddCodeSegLine (CS, "cmp #$%02X", (unsigned char)val);
	 	    AddCodeSegLine (CS, "jsr boolult");
	 	    return;
     	 	}
     	 	break;

     	    case CF_LONG:
     	 	break;

     	    default:
	 	typeerror (flags);
	}

	/* If we go here, we didn't emit code. Push the lhs on stack and fall
	 * into the normal, non-optimized stuff.
	 */
	g_push (flags & ~CF_CONST, 0);

    }

    /* Use long way over the stack */
    oper (flags, val, ops);
}



void g_le (unsigned flags, unsigned long val)
/* Test for less than or equal to */
{
    static char* ops [12] = {
	"tosle00",   	"toslea0",	"tosleax",
	"tosule00",  	"tosulea0",	"tosuleax",
	0,	     	0,    		"tosleeax",
	0,	     	0,    		"tosuleeax",
    };


    /* If the right hand side is const, the lhs is not on stack but still
     * in the primary register.
     */
    if (flags & CF_CONST) {

     	/* Look at the type */
     	switch (flags & CF_TYPE) {

	    case CF_CHAR:
		if (flags & CF_FORCECHAR) {
		    AddCodeSegLine (CS, "cmp #$%02X", (unsigned char)val);
		    if (flags & CF_UNSIGNED) {
		     	AddCodeSegLine (CS, "jsr boolule");
		    } else {
		        AddCodeSegLine (CS, "jsr boolle");
		    }
		    return;
		}
		/* FALLTHROUGH */

	    case CF_INT:
		if (flags & CF_UNSIGNED) {
		    AddCodeSegLine (CS, "cpx #$%02X", (unsigned char)(val >> 8));
       	       	    AddCodeSegLine (CS, "bne *+4");
     		    AddCodeSegLine (CS, "cmp #$%02X", (unsigned char)val);
		    AddCodeSegLine (CS, "jsr boolule");
		    return;
		}
	  	break;

	    case CF_LONG:
		break;

	    default:
		typeerror (flags);
	}

	/* If we go here, we didn't emit code. Push the lhs on stack and fall
	 * into the normal, non-optimized stuff.
	 */
	g_push (flags & ~CF_CONST, 0);

    }

    /* Use long way over the stack */
    oper (flags, val, ops);
}



void g_gt (unsigned flags, unsigned long val)
/* Test for greater than */
{
    static char* ops [12] = {
	"tosgt00",    	"tosgta0",	"tosgtax",
	"tosugt00",   	"tosugta0",	"tosugtax",
	0,	      	0,	    	"tosgteax",
	0,	      	0, 	    	"tosugteax",
    };


    /* If the right hand side is const, the lhs is not on stack but still
     * in the primary register.
     */
    if (flags & CF_CONST) {

     	/* Look at the type */
     	switch (flags & CF_TYPE) {

	    case CF_CHAR:
		if (flags & CF_FORCECHAR) {
		    AddCodeSegLine (CS, "cmp #$%02X", (unsigned char)val);
		    if (flags & CF_UNSIGNED) {
		      	/* If we have a compare > 0, we will replace it by
		      	 * != 0 here, since both are identical but the latter
		      	 * is easier to optimize.
		      	 */
		      	if (val & 0xFF) {
		       	    AddCodeSegLine (CS, "jsr boolugt");
		      	} else {
		      	    AddCodeSegLine (CS, "jsr boolne");
		      	}
		    } else {
	     	        AddCodeSegLine (CS, "jsr boolgt");
		    }
		    return;
		}
		/* FALLTHROUGH */

	    case CF_INT:
		if (flags & CF_UNSIGNED) {
		    /* If we have a compare > 0, we will replace it by
		     * != 0 here, since both are identical but the latter
		     * is easier to optimize.
		     */
		    if ((val & 0xFFFF) == 0) {
			AddCodeSegLine (CS, "stx tmp1");
			AddCodeSegLine (CS, "ora tmp1");
			AddCodeSegLine (CS, "jsr boolne");
		    } else {
       	       	       	AddCodeSegLine (CS, "cpx #$%02X", (unsigned char)(val >> 8));
			AddCodeSegLine (CS, "bne *+4");
			AddCodeSegLine (CS, "cmp #$%02X", (unsigned char)val);
       	       	       	AddCodeSegLine (CS, "jsr boolugt");
		    }
		    return;
       	       	}
		break;

	    case CF_LONG:
		break;

	    default:
		typeerror (flags);
	}

	/* If we go here, we didn't emit code. Push the lhs on stack and fall
	 * into the normal, non-optimized stuff.
	 */
	g_push (flags & ~CF_CONST, 0);

    }

    /* Use long way over the stack */
    oper (flags, val, ops);
}



void g_ge (unsigned flags, unsigned long val)
/* Test for greater than or equal to */
{
    static char* ops [12] = {
     	"tosge00",	"tosgea0",  	"tosgeax",
     	"tosuge00",	"tosugea0",	"tosugeax",
     	0,		0,		"tosgeeax",
     	0,		0,		"tosugeeax",
    };


    /* If the right hand side is const, the lhs is not on stack but still
     * in the primary register.
     */
    if (flags & CF_CONST) {

	/* Give a warning in some special cases */
	if ((flags & CF_UNSIGNED) && val == 0) {
     	    Warning ("Condition is always true");
	}

	/* Look at the type */
	switch (flags & CF_TYPE) {

	    case CF_CHAR:
		if (flags & CF_FORCECHAR) {
		    AddCodeSegLine (CS, "cmp #$%02X", (unsigned char)val);
		    if (flags & CF_UNSIGNED) {
			AddCodeSegLine (CS, "jsr booluge");
		    } else {
		        AddCodeSegLine (CS, "jsr boolge");
		    }
		    return;
		}
		/* FALLTHROUGH */

	    case CF_INT:
		if (flags & CF_UNSIGNED) {
       	       	    AddCodeSegLine (CS, "cpx #$%02X", (unsigned char)(val >> 8));
       	       	    AddCodeSegLine (CS, "bne *+4");
     		    AddCodeSegLine (CS, "cmp #$%02X", (unsigned char)val);
		    AddCodeSegLine (CS, "jsr booluge");
		    return;
		}
	     	break;

	    case CF_LONG:
		break;

	    default:
		typeerror (flags);
	}

	/* If we go here, we didn't emit code. Push the lhs on stack and fall
	 * into the normal, non-optimized stuff.
	 */
	g_push (flags & ~CF_CONST, 0);

    }

    /* Use long way over the stack */
    oper (flags, val, ops);
}



/*****************************************************************************/
/*   			   Allocating static storage	     	 	     */
/*****************************************************************************/



void g_res (unsigned n)
/* Reserve static storage, n bytes */
{
    AddDataSegLine (DS, "\t.res\t%u,$00", n);
}



void g_defdata (unsigned flags, unsigned long val, unsigned offs)
/* Define data with the size given in flags */
{
    if (flags & CF_CONST) {

	/* Numeric constant */
	switch (flags & CF_TYPE) {

	    case CF_CHAR:
	     	AddDataSegLine (DS, "\t.byte\t$%02lX", val & 0xFF);
		break;

	    case CF_INT:
		AddDataSegLine (DS, "\t.word\t$%04lX", val & 0xFFFF);
		break;

	    case CF_LONG:
		AddDataSegLine (DS, "\t.dword\t$%08lX", val & 0xFFFFFFFF);
		break;

	    default:
		typeerror (flags);
		break;

	}

    } else {

	/* Create the correct label name */
	const char* Label = GetLabelName (flags, val, offs);

	/* Labels are always 16 bit */
	AddDataSegLine (DS, "\t.word\t%s", Label);

    }
}



void g_defbytes (const void* Bytes, unsigned Count)
/* Output a row of bytes as a constant */
{
    unsigned Chunk;
    char Buf [128];
    char* B;

    /* Cast the buffer pointer */
    const unsigned char* Data = (const unsigned char*) Bytes;

    /* Output the stuff */
    while (Count) {

     	/* How many go into this line? */
     	if ((Chunk = Count) > 16) {
     	    Chunk = 16;
     	}
     	Count -= Chunk;

     	/* Output one line */
	strcpy (Buf, "\t.byte\t");
       	B = Buf + 7;
     	do {
	    B += sprintf (B, "$%02X", *Data++);
     	    if (--Chunk) {
		*B++ = ',';
     	    }
     	} while (Chunk);

	/* Output the line */
     	AddCodeSegLine (CS, Buf);
    }
}



void g_zerobytes (unsigned n)
/* Output n bytes of data initialized with zero */
{
    AddDataSegLine (DS, "\t.res\t%u,$00", n);
}



/*****************************************************************************/
/*			 User supplied assembler code			     */
/*****************************************************************************/



void g_asmcode (const char* Line, int Len)
/* Output one line of assembler code. If Len is greater than zero, it is used
 * as the maximum number of characters to use from Line.
 */
{
    if (Len >= 0) {
	AddCodeSegLine (CS, "%.*s", Len, Line);
    } else {
	AddCodeSegLine (CS, "%s", Line);
    }
}



/*****************************************************************************/
/*	     		    Inlined known functions			     */
/*****************************************************************************/



void g_strlen (unsigned flags, unsigned long val, unsigned offs)
/* Inline the strlen() function */
{
    /* We need a label in both cases */
    unsigned label = GetLocalLabel ();

    /* Two different encodings */
    if (flags & CF_CONST) {

	/* The address of the string is constant. Create the correct label name */
    	char* lbuf = GetLabelName (flags, val, offs);

	/* Generate the strlen code */
	AddCodeSegLine (CS, "ldy #$FF");
	g_defloclabel (label);
	AddCodeSegLine (CS, "iny");
	AddCodeSegLine (CS, "lda %s,y", lbuf);
	AddCodeSegLine (CS, "bne L%04X", label);
       	AddCodeSegLine (CS, "tax");
	AddCodeSegLine (CS, "tya");

    } else {

       	/* Address not constant but in primary */
	if (CodeSizeFactor < 400) {
	    /* This is too much code, so call strlen instead of inlining */
    	    AddCodeSegLine (CS, "jsr _strlen");
	} else {
	    /* Inline the function */
	    AddCodeSegLine (CS, "sta ptr1");
	    AddCodeSegLine (CS, "stx ptr1+1");
	    AddCodeSegLine (CS, "ldy #$FF");
	    g_defloclabel (label);
	    AddCodeSegLine (CS, "iny");
	    AddCodeSegLine (CS, "lda (ptr1),y");
	    AddCodeSegLine (CS, "bne L%04X", label);
       	    AddCodeSegLine (CS, "tax");
	    AddCodeSegLine (CS, "tya");
     	}
    }
}



