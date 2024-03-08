/*****************************************************************************/
/*                                                                           */
/*                                 codegen.c                                 */
/*                                                                           */
/*                            6502 code generator                            */
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



#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

/* common */
#include "addrsize.h"
#include "attrib.h"
#include "check.h"
#include "cpu.h"
#include "shift.h"
#include "strbuf.h"
#include "xmalloc.h"
#include "xsprintf.h"
#include "version.h"

/* cc65 */
#include "asmcode.h"
#include "asmlabel.h"
#include "casenode.h"
#include "codeseg.h"
#include "dataseg.h"
#include "error.h"
#include "global.h"
#include "segments.h"
#include "stackptr.h"
#include "stdfunc.h"
#include "textseg.h"
#include "util.h"
#include "codegen.h"

/* This is a terrible hack that tries to combat the ever reoccuring issue with
   Mingw and PRIXPTR - the macro should have been defined like this for us in
   the first place.
   NOTE: "I64x" works in the github actions now, so if your local mingw64 fails,
         you probably have to update.
*/
#if defined(__MINGW64__)
#undef PRIXPTR
#define PRIXPTR "I64x"
#endif

/*****************************************************************************/
/*                                  Helpers                                  */
/*****************************************************************************/



static void typeerror (unsigned type)
/* Print an error message about an invalid operand type */
{
    /* Special handling for floats here: */
    if ((type & CF_TYPEMASK) == CF_FLOAT) {
        Fatal ("Floating point type is currently unsupported");
    } else {
        Internal ("Invalid type in CF flags: %04X, type = %u", type, type & CF_TYPEMASK);
    }
}



static void CheckLocalOffs (unsigned Offs)
/* Check the offset into the stack for 8bit range */
{
    if (Offs >= 256) {
        /* Too many local vars */
        Error ("Too many local variables");
    }
}



static const char* GetLabelName (unsigned Flags, uintptr_t Label, long Offs)
{
    static char Buf [256];              /* Label name */

    /* Create the correct label name */
    switch (Flags & CF_ADDRMASK) {

        case CF_IMM:
            /* Immediate constant values */
            xsprintf (Buf, sizeof (Buf), "$%04X", (unsigned)((Offs) & 0xFFFF));
            break;

        case CF_STATIC:
            /* Local static memory cell */
            if (Offs) {
                xsprintf (Buf, sizeof (Buf), "%s%+ld", LocalDataLabelName (Label), Offs);
            } else {
                xsprintf (Buf, sizeof (Buf), "%s", LocalDataLabelName (Label));
            }
            break;

        case CF_EXTERNAL:
            /* External label */
            if (Offs) {
                xsprintf (Buf, sizeof (Buf), "_%s%+ld", (char*) Label, Offs);
            } else {
                xsprintf (Buf, sizeof (Buf), "_%s", (char*) Label);
            }
            break;

        case CF_LITERAL:
            /* Literal */
            /* Static memory cell */
            if (Offs) {
                xsprintf (Buf, sizeof (Buf), "%s%+ld", PooledLiteralLabelName (Label), Offs);
            } else {
                xsprintf (Buf, sizeof (Buf), "%s", PooledLiteralLabelName (Label));
            }
            break;

        case CF_ABSOLUTE:
            /* Absolute address */
            xsprintf (Buf, sizeof (Buf), "$%04X", (unsigned)((Label+Offs) & 0xFFFF));
            break;

        case CF_REGVAR:
            /* Variable in register bank */
            xsprintf (Buf, sizeof (Buf), "regbank+%u", (unsigned)((Label+Offs) & 0xFFFF));
            break;

        case CF_CODE:
            /* Code label location */
            if (Offs) {
                xsprintf (Buf, sizeof (Buf), "%s%+ld", LocalLabelName (Label), Offs);
            } else {
                xsprintf (Buf, sizeof (Buf), "%s", LocalLabelName (Label));
            }
            break;

        default:
            Internal ("Invalid address flags: %04X", Flags);
    }

    /* Return a pointer to the static buffer */
    return Buf;
}



/*****************************************************************************/
/*                            Pre- and postamble                             */
/*****************************************************************************/



void g_preamble (void)
/* Generate the assembler code preamble */
{
    /* Identify the compiler version */
    AddTextLine (";");
    AddTextLine ("; File generated by cc65 v %s", GetVersionAsString ());
    AddTextLine (";");

    /* Insert some object file options */
    AddTextLine ("\t.fopt\t\tcompiler,\"cc65 v %s\"",
                 GetVersionAsString ());

    /* If we're producing code for some other CPU, switch the command set */
    switch (CPU) {
        case CPU_6502:      AddTextLine ("\t.setcpu\t\t\"6502\"");      break;
        case CPU_6502X:     AddTextLine ("\t.setcpu\t\t\"6502X\"");     break;
        case CPU_6502DTV:   AddTextLine ("\t.setcpu\t\t\"6502DTV\"");   break;
        case CPU_65SC02:    AddTextLine ("\t.setcpu\t\t\"65SC02\"");    break;
        case CPU_65C02:     AddTextLine ("\t.setcpu\t\t\"65C02\"");     break;
        case CPU_65816:     AddTextLine ("\t.setcpu\t\t\"65816\"");     break;
        case CPU_HUC6280:   AddTextLine ("\t.setcpu\t\t\"HUC6280\"");   break;
        default:            Internal ("Unknown CPU: %d", CPU);
    }

    /* Use smart mode */
    AddTextLine ("\t.smart\t\ton");

    /* Allow auto import for runtime library routines */
    AddTextLine ("\t.autoimport\ton");

    /* Switch the assembler into case sensitive mode */
    AddTextLine ("\t.case\t\ton");

    /* Tell the assembler if we want to generate debug info */
    AddTextLine ("\t.debuginfo\t%s", (DebugInfo != 0)? "on" : "off");

    /* Import zero page variables */
    AddTextLine ("\t.importzp\tsp, sreg, regsave, regbank");
    AddTextLine ("\t.importzp\ttmp1, tmp2, tmp3, tmp4, ptr1, ptr2, ptr3, ptr4");

    /* Define long branch macros */
    AddTextLine ("\t.macpack\tlongbranch");
}



void g_fileinfo (const char* Name, unsigned long Size, unsigned long MTime)
/* If debug info is enabled, place a file info into the source */
{
    if (DebugInfo) {
        /* We have to place this into the global text segment, so it will
        ** appear before all .dbg line statements.
        */
        TS_AddLine (GS->Text, "\t.dbg\t\tfile, \"%s\", %lu, %lu", Name, Size, MTime);
    }
}



/*****************************************************************************/
/*                              Segment support                              */
/*****************************************************************************/



void g_userodata (void)
/* Switch to the read only data segment */
{
    UseDataSeg (SEG_RODATA);
}



void g_usedata (void)
/* Switch to the data segment */
{
    UseDataSeg (SEG_DATA);
}



void g_usebss (void)
/* Switch to the bss segment */
{
    UseDataSeg (SEG_BSS);
}



void g_segname (segment_t Seg)
/* Emit the name of a segment if necessary */
{
    unsigned char AddrSize;
    const char* Name;

    /* Emit a segment directive for the data style segments */
    DataSeg* S;
    switch (Seg) {
        case SEG_RODATA: S = CS->ROData; break;
        case SEG_DATA:   S = CS->Data;   break;
        case SEG_BSS:    S = CS->BSS;    break;
        default:         S = 0;          break;
    }
    if (S) {
        Name = GetSegName (Seg);
        AddrSize = GetSegAddrSize (Name);
        if (AddrSize != ADDR_SIZE_INVALID) {
            DS_AddLine (S, ".segment\t\"%s\": %s", Name, AddrSizeToStr (AddrSize));
        } else {
            DS_AddLine (S, ".segment\t\"%s\"", Name);
        }
    }
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



unsigned sizeofarg (unsigned flags)
/* Return the size of a function argument type that is encoded in flags */
{
    switch (flags & CF_TYPEMASK) {

        case CF_CHAR:
            return (flags & CF_FORCECHAR)? 1 : 2;

        case CF_INT:
            return 2;

        case CF_LONG:
            return 4;

        case CF_FLOAT:
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
    return StackPtr += sizeofarg (flags);
}



int push (unsigned flags)
/* Push an argument of the given size */
{
    return StackPtr -= sizeofarg (flags);
}



static unsigned MakeByteOffs (unsigned Flags, unsigned Offs)
/* The value in Offs is an offset to an address in a/x. Make sure, an object
** of the type given in Flags can be loaded or stored into this address by
** adding part of the offset to the address in ax, so that the remaining
** offset fits into an index register. Return the remaining offset.
*/
{
    /* If the offset is too large for a byte register, add the high byte
    ** of the offset to the primary. Beware: We need a special correction
    ** if the offset in the low byte will overflow in the operation.
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
/*                      Functions handling local labels                      */
/*****************************************************************************/



void g_defcodelabel (unsigned label)
/* Define a local code label */
{
    CS_AddLabel (CS->Code, LocalLabelName (label));
}



void g_defdatalabel (unsigned label)
/* Define a local data label */
{
    AddDataLine ("%s:", LocalDataLabelName (label));
}



/*****************************************************************************/
/*                     Functions handling global labels                      */
/*****************************************************************************/



void g_defgloblabel (const char* Name)
/* Define a global label with the given name */
{
    /* Global labels are always data labels */
    AddDataLine ("_%s:", Name);
}



void g_defliterallabel (unsigned label)
/* Define a literal data label */
{
    /* Literal labels are always data labels */
    AddDataLine ("%s:", PooledLiteralLabelName (label));
}



void g_aliasliterallabel (unsigned label, unsigned baselabel, long offs)
/* Define label as an alias for baselabel+offs */
{
    /* We need an intermediate buffer here since LocalLabelName uses a
    ** static buffer which changes with each call.
    */
    StrBuf L = AUTO_STRBUF_INITIALIZER;
    SB_AppendStr (&L, PooledLiteralLabelName (label));
    SB_Terminate (&L);
    AddDataLine ("%s\t:=\t%s+%ld",
                 SB_GetConstBuf (&L),
                 PooledLiteralLabelName (baselabel),
                 offs);
    SB_Done (&L);
}



void g_defexport (const char* Name, int ZP)
/* Export the given label */
{
    if (ZP) {
        AddTextLine ("\t.exportzp\t_%s", Name);
    } else {
        AddTextLine ("\t.export\t\t_%s", Name);
    }
}



void g_defimport (const char* Name, int ZP)
/* Import the given label */
{
    if (ZP) {
        AddTextLine ("\t.importzp\t_%s", Name);
    } else {
        AddTextLine ("\t.import\t\t_%s", Name);
    }
}



void g_importstartup (void)
/* Forced import of the startup module */
{
    AddTextLine ("\t.forceimport\t__STARTUP__");
}



void g_importmainargs (void)
/* Forced import of a special symbol that handles arguments to main. This will
   happen only when the compiler sees a main function that takes arguments. */
{
    AddTextLine ("\t.forceimport\tinitmainargs");
}



/*****************************************************************************/
/*                          Function entry and exit                          */
/*****************************************************************************/



/* Remember the argument size of a function. The variable is set by g_enter
** and used by g_leave. If the function gets its argument size by the caller
** (variable param list or function without prototype), g_enter will set the
** value to -1.
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
        AddCodeLine ("jsr enter");
    }
}



void g_leave (void)
/* Function epilogue */
{
    /* How many bytes of locals do we have to drop? */
    unsigned ToDrop = (unsigned) -StackPtr;

    /* If we didn't have a variable argument list, don't call leave */
    if (funcargs >= 0) {

        /* Drop stackframe if needed */
        g_drop (ToDrop + funcargs);

    } else if (StackPtr != 0) {

        /* We've a stack frame to drop */
        if (ToDrop > 255) {
            g_drop (ToDrop);            /* Inlines the code */
            AddCodeLine ("jsr leave");
        } else {
            AddCodeLine ("ldy #$%02X", ToDrop);
            AddCodeLine ("jsr leavey");
        }

    } else {

        /* Nothing to drop */
        AddCodeLine ("jsr leave");

    }

    /* Add the final rts */
    AddCodeLine ("rts");
}



/*****************************************************************************/
/*                            Register variables                             */
/*****************************************************************************/



void g_swap_regvars (int StackOffs, int RegOffs, unsigned Bytes)
/* Swap a register variable with a location on the stack */
{
    /* Calculate the actual stack offset and check it */
    StackOffs -= StackPtr;
    CheckLocalOffs (StackOffs);

    /* Generate code */
    AddCodeLine ("ldy #$%02X", StackOffs & 0xFF);
    if (Bytes == 1) {

        if (IS_Get (&CodeSizeFactor) < 165) {
            AddCodeLine ("ldx #$%02X", RegOffs & 0xFF);
            AddCodeLine ("jsr regswap1");
        } else {
            AddCodeLine ("lda (sp),y");
            AddCodeLine ("ldx regbank%+d", RegOffs);
            AddCodeLine ("sta regbank%+d", RegOffs);
            AddCodeLine ("txa");
            AddCodeLine ("sta (sp),y");
        }

    } else if (Bytes == 2) {

        AddCodeLine ("ldx #$%02X", RegOffs & 0xFF);
        AddCodeLine ("jsr regswap2");

    } else {

        AddCodeLine ("ldx #$%02X", RegOffs & 0xFF);
        AddCodeLine ("lda #$%02X", Bytes & 0xFF);
        AddCodeLine ("jsr regswap");
    }
}



void g_save_regvars (int RegOffs, unsigned Bytes)
/* Save register variables */
{
    /* Don't loop for up to two bytes */
    if (Bytes == 1) {

        AddCodeLine ("lda regbank%+d", RegOffs);
        AddCodeLine ("jsr pusha");

    } else if (Bytes == 2) {

        AddCodeLine ("lda regbank%+d", RegOffs);
        AddCodeLine ("ldx regbank%+d", RegOffs+1);
        AddCodeLine ("jsr pushax");

    } else {

        /* More than two bytes - loop */
        unsigned Label = GetLocalLabel ();
        g_space (Bytes);
        AddCodeLine ("ldy #$%02X", (unsigned char) (Bytes - 1));
        AddCodeLine ("ldx #$%02X", (unsigned char) Bytes);
        g_defcodelabel (Label);
        AddCodeLine ("lda regbank%+d,x", RegOffs-1);
        AddCodeLine ("sta (sp),y");
        AddCodeLine ("dey");
        AddCodeLine ("dex");
        AddCodeLine ("bne %s", LocalLabelName (Label));

    }

    /* We pushed stuff, correct the stack pointer */
    StackPtr -= Bytes;
}



void g_restore_regvars (int StackOffs, int RegOffs, unsigned Bytes)
/* Restore register variables */
{
    /* Calculate the actual stack offset and check it */
    StackOffs -= StackPtr;
    CheckLocalOffs (StackOffs);

    /* Don't loop for up to two bytes */
    if (Bytes == 1) {

        AddCodeLine ("ldy #$%02X", StackOffs);
        AddCodeLine ("lda (sp),y");
        AddCodeLine ("sta regbank%+d", RegOffs);

    } else if (Bytes == 2) {

        AddCodeLine ("ldy #$%02X", StackOffs);
        AddCodeLine ("lda (sp),y");
        AddCodeLine ("sta regbank%+d", RegOffs);
        AddCodeLine ("iny");
        AddCodeLine ("lda (sp),y");
        AddCodeLine ("sta regbank%+d", RegOffs+1);

    } else if (Bytes == 3 && IS_Get (&CodeSizeFactor) >= 133) {

        AddCodeLine ("ldy #$%02X", StackOffs);
        AddCodeLine ("lda (sp),y");
        AddCodeLine ("sta regbank%+d", RegOffs);
        AddCodeLine ("iny");
        AddCodeLine ("lda (sp),y");
        AddCodeLine ("sta regbank%+d", RegOffs+1);
        AddCodeLine ("iny");
        AddCodeLine ("lda (sp),y");
        AddCodeLine ("sta regbank%+d", RegOffs+2);

    } else if (StackOffs <= RegOffs) {

        /* More bytes, but the relation between the register offset in the
        ** register bank and the stack offset allows us to generate short
        ** code that uses just one index register.
        */
        unsigned Label = GetLocalLabel ();
        AddCodeLine ("ldy #$%02X", StackOffs);
        g_defcodelabel (Label);
        AddCodeLine ("lda (sp),y");
        AddCodeLine ("sta regbank%+d,y", RegOffs - StackOffs);
        AddCodeLine ("iny");
        AddCodeLine ("cpy #$%02X", StackOffs + Bytes);
        AddCodeLine ("bne %s", LocalLabelName (Label));

    } else {

        /* OK, this is the generic code. We need to save X because the
        ** caller will only save A.
        */
        unsigned Label = GetLocalLabel ();
        AddCodeLine ("stx tmp1");
        AddCodeLine ("ldy #$%02X", (unsigned char) (StackOffs + Bytes - 1));
        AddCodeLine ("ldx #$%02X", (unsigned char) (Bytes - 1));
        g_defcodelabel (Label);
        AddCodeLine ("lda (sp),y");
        AddCodeLine ("sta regbank%+d,x", RegOffs);
        AddCodeLine ("dey");
        AddCodeLine ("dex");
        AddCodeLine ("bpl %s", LocalLabelName (Label));
        AddCodeLine ("ldx tmp1");

    }
}



/*****************************************************************************/
/*                           Fetching memory cells                           */
/*****************************************************************************/



void g_getimmed (unsigned Flags, uintptr_t Val, long Offs)
/* Load a constant into the primary register */
{
    unsigned char B1, B2, B3, B4;


    if ((Flags & CF_CONST) != 0) {

        /* Numeric constant */
        switch (Flags & CF_TYPEMASK) {

            case CF_CHAR:
                if ((Flags & CF_FORCECHAR) != 0) {
                    AddCodeLine ("lda #$%02X", (unsigned char) Val);
                    break;
                }
                /* FALL THROUGH */
            case CF_INT:
                AddCodeLine ("ldx #$%02X", (unsigned char) (Val >> 8));
                AddCodeLine ("lda #$%02X", (unsigned char) Val);
                break;

            case CF_LONG:
                /* Split the value into 4 bytes */
                B1 = (unsigned char) (Val >>  0);
                B2 = (unsigned char) (Val >>  8);
                B3 = (unsigned char) (Val >> 16);
                B4 = (unsigned char) (Val >> 24);

                /* Load the value. Don't be too smart here and let
                 * the optimizer do its job.
                 */
                AddCodeLine ("lda #$%02X", B4);
                AddCodeLine ("sta sreg+1");
                AddCodeLine ("lda #$%02X", B3);
                AddCodeLine ("sta sreg");
                AddCodeLine ("lda #$%02X", B1);
                AddCodeLine ("ldx #$%02X", B2);
                break;

            default:
                typeerror (Flags);
                break;

        }

    } else {

        /* Some sort of label */
        const char* Label = GetLabelName (Flags, Val, Offs);

        /* Load the address into the primary */
        AddCodeLine ("lda #<(%s)", Label);
        AddCodeLine ("ldx #>(%s)", Label);

    }
}



void g_getstatic (unsigned flags, uintptr_t label, long offs)
/* Fetch an static memory cell into the primary register */
{
    /* Create the correct label name */
    const char* lbuf = GetLabelName (flags, label, offs);

    /* Check the size and generate the correct load operation */
    switch (flags & CF_TYPEMASK) {

        case CF_CHAR:
            if ((flags & CF_FORCECHAR) || (flags & CF_TEST)) {
                AddCodeLine ("lda %s", lbuf);   /* load A from the label */
            } else {
                AddCodeLine ("ldx #$00");
                AddCodeLine ("lda %s", lbuf);   /* load A from the label */
                if (!(flags & CF_UNSIGNED)) {
                    /* Must sign extend */
                    unsigned L = GetLocalLabel ();
                    AddCodeLine ("bpl %s", LocalLabelName (L));
                    AddCodeLine ("dex");
                    g_defcodelabel (L);
                }
            }
            break;

        case CF_INT:
            AddCodeLine ("lda %s", lbuf);
            if (flags & CF_TEST) {
                AddCodeLine ("ora %s+1", lbuf);
            } else {
                AddCodeLine ("ldx %s+1", lbuf);
            }
            break;

        case CF_LONG:
            if (flags & CF_TEST) {
                AddCodeLine ("lda %s+3", lbuf);
                AddCodeLine ("ora %s+2", lbuf);
                AddCodeLine ("ora %s+1", lbuf);
                AddCodeLine ("ora %s+0", lbuf);
            } else {
                AddCodeLine ("lda %s+3", lbuf);
                AddCodeLine ("sta sreg+1");
                AddCodeLine ("lda %s+2", lbuf);
                AddCodeLine ("sta sreg");
                AddCodeLine ("ldx %s+1", lbuf);
                AddCodeLine ("lda %s", lbuf);
            }
            break;

        default:
            typeerror (flags);

    }
}



void g_getlocal (unsigned Flags, int Offs)
/* Fetch specified local object (local var). */
{
    Offs -= StackPtr;
    switch (Flags & CF_TYPEMASK) {

        case CF_CHAR:
            CheckLocalOffs (Offs);
            if ((Flags & CF_FORCECHAR) || (Flags & CF_TEST)) {
                AddCodeLine ("ldy #$%02X", Offs);
                AddCodeLine ("lda (sp),y");
            } else {
                AddCodeLine ("ldy #$%02X", Offs);
                AddCodeLine ("ldx #$00");
                AddCodeLine ("lda (sp),y");
                if ((Flags & CF_UNSIGNED) == 0) {
                    unsigned L = GetLocalLabel();
                    AddCodeLine ("bpl %s", LocalLabelName (L));
                    AddCodeLine ("dex");
                    g_defcodelabel (L);
                }
            }
            break;

        case CF_INT:
            CheckLocalOffs (Offs + 1);
            AddCodeLine ("ldy #$%02X", (unsigned char) (Offs+1));
            if (Flags & CF_TEST) {
                AddCodeLine ("lda (sp),y");
                AddCodeLine ("dey");
                AddCodeLine ("ora (sp),y");
            } else {
                AddCodeLine ("jsr ldaxysp");
            }
            break;

        case CF_LONG:
            CheckLocalOffs (Offs + 3);
            AddCodeLine ("ldy #$%02X", (unsigned char) (Offs+3));
            AddCodeLine ("jsr ldeaxysp");
            if (Flags & CF_TEST) {
                g_test (Flags);
            }
            break;

        default:
            typeerror (Flags);
    }
}



void g_getind (unsigned Flags, unsigned Offs)
/* Fetch the specified object type indirect through the primary register
** into the primary register
*/
{
    /* If the offset is greater than 255, add the part that is > 255 to
    ** the primary. This way we get an easy addition and use the low byte
    ** as the offset
    */
    Offs = MakeByteOffs (Flags, Offs);

    /* Handle the indirect fetch */
    switch (Flags & CF_TYPEMASK) {

        case CF_CHAR:
            /* Character sized */
            AddCodeLine ("ldy #$%02X", Offs);
            if (Flags & CF_UNSIGNED) {
                AddCodeLine ("jsr ldauidx");
            } else {
                AddCodeLine ("jsr ldaidx");
            }
            break;

        case CF_INT:
            if (Flags & CF_TEST) {
                AddCodeLine ("ldy #$%02X", Offs);
                AddCodeLine ("sta ptr1");
                AddCodeLine ("stx ptr1+1");
                AddCodeLine ("lda (ptr1),y");
                AddCodeLine ("iny");
                AddCodeLine ("ora (ptr1),y");
            } else {
                AddCodeLine ("ldy #$%02X", Offs+1);
                AddCodeLine ("jsr ldaxidx");
            }
            break;

        case CF_LONG:
            AddCodeLine ("ldy #$%02X", Offs+3);
            AddCodeLine ("jsr ldeaxidx");
            if (Flags & CF_TEST) {
                g_test (Flags);
            }
            break;

        default:
            typeerror (Flags);

    }
}



void g_leasp (int Offs)
/* Fetch the address of the specified symbol into the primary register */
{
    unsigned char Lo, Hi;

    /* Calculate the offset relative to sp */
    Offs -= StackPtr;

    /* Get low and high byte */
    Lo = (unsigned char) Offs;
    Hi = (unsigned char) (Offs >> 8);

    /* Generate code */
    if (Lo == 0) {
        if (Hi <= 3) {
            AddCodeLine ("lda sp");
            AddCodeLine ("ldx sp+1");
            while (Hi--) {
                AddCodeLine ("inx");
            }
        } else {
            AddCodeLine ("lda sp+1");
            AddCodeLine ("clc");
            AddCodeLine ("adc #$%02X", Hi);
            AddCodeLine ("tax");
            AddCodeLine ("lda sp");
        }
    } else if (Hi == 0) {
        /* 8 bit offset */
        if (IS_Get (&CodeSizeFactor) < 200) {
            /* 8 bit offset with subroutine call */
            AddCodeLine ("lda #$%02X", Lo);
            AddCodeLine ("jsr leaa0sp");
        } else {
            /* 8 bit offset inlined */
            unsigned L = GetLocalLabel ();
            AddCodeLine ("lda sp");
            AddCodeLine ("ldx sp+1");
            AddCodeLine ("clc");
            AddCodeLine ("adc #$%02X", Lo);
            AddCodeLine ("bcc %s", LocalLabelName (L));
            AddCodeLine ("inx");
            g_defcodelabel (L);
        }
    } else if (IS_Get (&CodeSizeFactor) < 170) {
        /* Full 16 bit offset with subroutine call */
        AddCodeLine ("lda #$%02X", Lo);
        AddCodeLine ("ldx #$%02X", Hi);
        AddCodeLine ("jsr leaaxsp");
    } else {
        /* Full 16 bit offset inlined */
        AddCodeLine ("lda sp");
        AddCodeLine ("clc");
        AddCodeLine ("adc #$%02X", Lo);
        AddCodeLine ("pha");
        AddCodeLine ("lda sp+1");
        AddCodeLine ("adc #$%02X", Hi);
        AddCodeLine ("tax");
        AddCodeLine ("pla");
    }
}



void g_leavariadic (int Offs)
/* Fetch the address of a parameter in a variadic function into the primary
** register
*/
{
    unsigned ArgSizeOffs;

    /* Calculate the offset relative to sp */
    Offs -= StackPtr;

    /* Get the offset of the parameter which is stored at sp+0 on function
    ** entry and check if this offset is reachable with a byte offset.
    */
    CHECK (StackPtr <= 0);
    ArgSizeOffs = -StackPtr;
    CheckLocalOffs (ArgSizeOffs);

    /* Get the size of all parameters. */
    AddCodeLine ("ldy #$%02X", ArgSizeOffs);
    AddCodeLine ("lda (sp),y");

    /* Add the value of the stackpointer */
    if (IS_Get (&CodeSizeFactor) > 250) {
        unsigned L = GetLocalLabel();
        AddCodeLine ("ldx sp+1");
        AddCodeLine ("clc");
        AddCodeLine ("adc sp");
        AddCodeLine ("bcc %s", LocalLabelName (L));
        AddCodeLine ("inx");
        g_defcodelabel (L);
    } else {
        AddCodeLine ("ldx #$00");
        AddCodeLine ("jsr leaaxsp");
    }

    /* Add the offset to the primary */
    if (Offs > 0) {
        g_inc (CF_INT | CF_CONST, Offs);
    } else if (Offs < 0) {
        g_dec (CF_INT | CF_CONST, -Offs);
    }
}



/*****************************************************************************/
/*                             Store into memory                             */
/*****************************************************************************/



void g_putstatic (unsigned flags, uintptr_t label, long offs)
/* Store the primary register into the specified static memory cell */
{
    /* Create the correct label name */
    const char* lbuf = GetLabelName (flags, label, offs);

    /* Check the size and generate the correct store operation */
    switch (flags & CF_TYPEMASK) {

        case CF_CHAR:
            AddCodeLine ("sta %s", lbuf);
            break;

        case CF_INT:
            AddCodeLine ("sta %s", lbuf);
            AddCodeLine ("stx %s+1", lbuf);
            break;

        case CF_LONG:
            AddCodeLine ("sta %s", lbuf);
            AddCodeLine ("stx %s+1", lbuf);
            AddCodeLine ("ldy sreg");
            AddCodeLine ("sty %s+2", lbuf);
            AddCodeLine ("ldy sreg+1");
            AddCodeLine ("sty %s+3", lbuf);
            break;

        default:
            typeerror (flags);

    }
}



void g_putlocal (unsigned Flags, int Offs, long Val)
/* Put data into local object. */
{
    Offs -= StackPtr;
    CheckLocalOffs (Offs);
    switch (Flags & CF_TYPEMASK) {

        case CF_CHAR:
            if (Flags & CF_CONST) {
                AddCodeLine ("lda #$%02X", (unsigned char) Val);
            }
            AddCodeLine ("ldy #$%02X", Offs);
            AddCodeLine ("sta (sp),y");
            break;

        case CF_INT:
            if (Flags & CF_CONST) {
                AddCodeLine ("ldy #$%02X", Offs+1);
                AddCodeLine ("lda #$%02X", (unsigned char) (Val >> 8));
                AddCodeLine ("sta (sp),y");
                if ((Flags & CF_NOKEEP) == 0) {
                    /* Place high byte into X */
                    AddCodeLine ("tax");
                }
                if ((Val & 0xFF) == Offs+1) {
                    /* The value we need is already in Y */
                    AddCodeLine ("tya");
                    AddCodeLine ("dey");
                } else {
                    AddCodeLine ("dey");
                    AddCodeLine ("lda #$%02X", (unsigned char) Val);
                }
                AddCodeLine ("sta (sp),y");
            } else {
                AddCodeLine ("ldy #$%02X", Offs);
                if ((Flags & CF_NOKEEP) == 0 || IS_Get (&CodeSizeFactor) < 160) {
                    AddCodeLine ("jsr staxysp");
                } else {
                    AddCodeLine ("sta (sp),y");
                    AddCodeLine ("iny");
                    AddCodeLine ("txa");
                    AddCodeLine ("sta (sp),y");
                }
            }
            break;

        case CF_LONG:
            if (Flags & CF_CONST) {
                g_getimmed (Flags, Val, 0);
            }
            AddCodeLine ("ldy #$%02X", Offs);
            AddCodeLine ("jsr steaxysp");
            break;

        default:
            typeerror (Flags);

    }
}



void g_putind (unsigned Flags, unsigned Offs)
/* Store the specified object type in the primary register at the address
** on the top of the stack
*/
{
    /* We can handle offsets below $100 directly, larger offsets must be added
    ** to the address. Since a/x is in use, best code is achieved by adding
    ** just the high byte. Be sure to check if the low byte will overflow while
    ** while storing.
    */
    if ((Offs & 0xFF) > 256 - sizeofarg (Flags | CF_FORCECHAR)) {

        /* Overflow - we need to add the low byte also */
        AddCodeLine ("ldy #$00");
        AddCodeLine ("clc");
        if ((Flags & CF_NOKEEP) == 0) {
            AddCodeLine ("pha");
        }
        AddCodeLine ("lda #$%02X", Offs & 0xFF);
        AddCodeLine ("adc (sp),y");
        AddCodeLine ("sta (sp),y");
        AddCodeLine ("iny");
        AddCodeLine ("lda #$%02X", (Offs >> 8) & 0xFF);
        AddCodeLine ("adc (sp),y");
        AddCodeLine ("sta (sp),y");
        if ((Flags & CF_NOKEEP) == 0) {
            AddCodeLine ("pla");
        }

        /* Complete address is on stack, new offset is zero */
        Offs = 0;

    } else if ((Offs & 0xFF00) != 0) {

        /* We can just add the high byte */
        AddCodeLine ("ldy #$01");
        AddCodeLine ("clc");
        if ((Flags & CF_NOKEEP) == 0) {
            AddCodeLine ("pha");
        }
        AddCodeLine ("lda #$%02X", (Offs >> 8) & 0xFF);
        AddCodeLine ("adc (sp),y");
        AddCodeLine ("sta (sp),y");
        if ((Flags & CF_NOKEEP) == 0) {
            AddCodeLine ("pla");
        }
        /* Offset is now just the low byte */
        Offs &= 0x00FF;
    }

    /* Check the size and determine operation */
    AddCodeLine ("ldy #$%02X", Offs);
    switch (Flags & CF_TYPEMASK) {

        case CF_CHAR:
            AddCodeLine ("jsr staspidx");
            break;

        case CF_INT:
            AddCodeLine ("jsr staxspidx");
            break;

        case CF_LONG:
            AddCodeLine ("jsr steaxspidx");
            break;

        default:
            typeerror (Flags);

    }

    /* Pop the argument which is always a pointer */
    pop (CF_PTR);
}



/*****************************************************************************/
/*                    type conversion and similiar stuff                     */
/*****************************************************************************/



void g_toslong (unsigned flags)
/* Make sure, the value on TOS is a long. Convert if necessary */
{
    switch (flags & CF_TYPEMASK) {

        case CF_CHAR:
        case CF_INT:
            if (flags & CF_UNSIGNED) {
                AddCodeLine ("jsr tosulong");
            } else {
                AddCodeLine ("jsr toslong");
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
    switch (flags & CF_TYPEMASK) {

        case CF_CHAR:
        case CF_INT:
            break;

        case CF_LONG:
            AddCodeLine ("jsr tosint");
            pop (CF_INT);
            break;

        default:
            typeerror (flags);
    }
}



static void g_regchar (unsigned to)
/* Treat the value in the primary register as a char with specified signedness
** and convert it to an int (whose representation is irrelevent of signedness).
*/
{
    /* Since char is the smallest type supported here, we never need any info
    ** about the original type to "promote from it". However, we have to make
    ** sure the entire AX contains the correct char value as an int, since we
    ** will almost always use the char value as an int in AX directly in code
    ** generation (unless CF_FORCECHAR is specified). That is to say, we don't
    ** need the original "from" flags for the first conversion to char, but do
    ** need the original "to" flags as the new "from" flags for the conversion
    ** to int.
    */
    g_regint (to | CF_FORCECHAR);
}



void g_regint (unsigned from)
/* Convert the value in the primary register to an int (whose representation
** is irrelevent of signedness).
*/
{
    switch (from & CF_TYPEMASK) {

        case CF_CHAR:
            /* If the original value was forced to use only A, it must be
            ** extended from char to fill AX. Otherwise nothing to do here
            ** since AX would already have the correct int value.
            */
            if (from & CF_FORCECHAR) {
                AddCodeLine ("ldx #$00");

                if ((from & CF_UNSIGNED) == 0) {
                    /* Sign extend */
                    unsigned L = GetLocalLabel();
                    AddCodeLine ("cmp #$80");
                    AddCodeLine ("bcc %s", LocalLabelName (L));
                    AddCodeLine ("dex");
                    g_defcodelabel (L);
                }
                break;
            }
            /* FALLTHROUGH */

        case CF_INT:
        case CF_LONG:
            break;

        default:
            typeerror (from);
    }
}



void g_reglong (unsigned from)
/* Convert the value in the primary register to a long (whose representation
** is irrelevent of signedness).
*/
{
    switch (from & CF_TYPEMASK) {

        case CF_CHAR:
            /* If the original value was forced to use only A, it must be
            ** extended from char to long. Otherwise AX would already have
            ** the correct int value to be extened to long.
            */
            if (from & CF_FORCECHAR) {
                /* Conversion is from char */
                if (from & CF_UNSIGNED) {
                    if (IS_Get (&CodeSizeFactor) >= 200) {
                        AddCodeLine ("ldx #$00");
                        AddCodeLine ("stx sreg");
                        AddCodeLine ("stx sreg+1");
                    } else {
                        AddCodeLine ("jsr aulong");
                    }
                } else {
                    if (IS_Get (&CodeSizeFactor) >= 366) {
                        g_regint (from);
                        AddCodeLine ("stx sreg");
                        AddCodeLine ("stx sreg+1");
                    } else {
                        AddCodeLine ("jsr along");
                    }
                }
                break;
            }
            /* FALLTHROUGH */

        case CF_INT:
            if (from & CF_UNSIGNED) {
                if (IS_Get (&CodeSizeFactor) >= 200) {
                    AddCodeLine ("ldy #$00");
                    AddCodeLine ("sty sreg");
                    AddCodeLine ("sty sreg+1");
                } else {
                    AddCodeLine ("jsr axulong");
                }
            } else {
                AddCodeLine ("jsr axlong");
            }
            break;

        case CF_LONG:
            break;

        default:
            typeerror (from);
    }
}



static unsigned g_intpromotion (unsigned flags)
/* Return new flags for integral promotions for types smaller than int. */
{
    /* https://port70.net/~nsz/c/c89/c89-draft.html#3.2.1.1
    ** A char, a short int, or an int bit-field, or their signed or unsigned varieties, or an
    ** object that has enumeration type, may be used in an expression wherever an int or
    ** unsigned int may be used. If an int can represent all values of the original type, the value
    ** is converted to an int; otherwise it is converted to an unsigned int.
    ** These are called the integral promotions.
    */

    if ((flags & CF_TYPEMASK) == CF_CHAR) {
        /* int can represent all unsigned chars, so unsigned char is promoted to int. */
        flags &= ~CF_TYPEMASK;
        flags &= ~CF_UNSIGNED;
        flags |= CF_INT;
        return flags;
    } else if ((flags & CF_TYPEMASK) == CF_SHORT) {
        /* int cannot represent all unsigned shorts, so unsigned short is promoted to
        ** unsigned int.
        */
        flags &= ~CF_TYPEMASK;
        flags |= CF_INT;
        return flags;
    } else {
        /* Otherwise, the type is not smaller than int, so leave it alone. */
        return flags;
    }
}



unsigned g_typeadjust (unsigned lhs, unsigned rhs)
/* Adjust the integer operands before doing a binary operation. lhs is a flags
** value, that corresponds to the value on TOS, rhs corresponds to the value
** in (e)ax. The return value is the flags value for the resulting type.
*/
{
    /* Get the type spec from the flags */
    unsigned ltype = lhs & CF_TYPEMASK;
    unsigned rtype = rhs & CF_TYPEMASK;

    /* Check if a conversion is needed */
    if (ltype == CF_LONG && rtype != CF_LONG && (rhs & CF_CONST) == 0) {
        /* We must promote the primary register to long */
        g_reglong (rhs);
    } else if (ltype != CF_LONG && (lhs & CF_CONST) == 0 && rtype == CF_LONG) {
        /* We must promote the lhs to long */
        if (lhs & CF_PRIMARY) {
            g_reglong (lhs);
        } else {
            g_toslong (lhs);
        }
    }

    /* Result is const if both operands are const. */
    unsigned const_flag = (lhs & CF_CONST) & (rhs & CF_CONST);

    /* https://port70.net/~nsz/c/c89/c89-draft.html#3.2.1.5
    ** Many binary operators that expect operands of arithmetic type cause conversions and yield
    ** result types in a similar way. The purpose is to yield a common type, which is also the type
    ** of the result. This pattern is called the usual arithmetic conversions.
    */

    /* Note that this logic is largely duplicated by ArithmeticConvert. */

    /* Before we apply the integral promotions, we check if both types are the same character type.
    ** If so, we return that type, rather than int, which would be returned by the standard
    ** rules.  This is only a performance optimization allowing the use of unsigned and/or char
    ** operations; it does not affect correctness, as the flags are only used for code generation,
    ** and not to determine types of other expressions containing this one.  For codgen, CF_CHAR
    ** means the operands are char and the result is int (unless CF_FORCECHAR is also set, in
    ** which case the result is char).  This special case part is not duplicated by
    ** ArithmeticConvert.
    */
    if ((lhs & CF_TYPEMASK) == CF_CHAR && (rhs & CF_TYPEMASK) == CF_CHAR &&
        (lhs & CF_UNSIGNED) == (rhs & CF_UNSIGNED)) {
        /* Signedness flags are the same, so just use one of them. */
        const unsigned unsigned_flag = lhs & CF_UNSIGNED;
        return const_flag | unsigned_flag | CF_CHAR;
    }

    /* Apply integral promotions for types char/short. */
    lhs = g_intpromotion (lhs);
    rhs = g_intpromotion (rhs);
    ltype = lhs & CF_TYPEMASK;
    rtype = rhs & CF_TYPEMASK;

    /* If either operand has type unsigned long int, the other operand is converted to
    ** unsigned long int.
    */
    if ((ltype == CF_LONG && (lhs & CF_UNSIGNED)) ||
        (rtype == CF_LONG && (rhs & CF_UNSIGNED))) {
        return const_flag | CF_UNSIGNED | CF_LONG;
    }

    /* Otherwise, if one operand has type long int and the other has type unsigned int,
    ** if a long int can represent all values of an unsigned int, the operand of type unsigned int
    ** is converted to long int ; if a long int cannot represent all the values of an unsigned int,
    ** both operands are converted to unsigned long int.
    */
    if ((ltype == CF_LONG && rtype == CF_INT && (rhs & CF_UNSIGNED)) ||
        (rtype == CF_LONG && ltype == CF_INT && (rhs & CF_UNSIGNED))) {
        /* long can represent all unsigneds, so we are in the first sub-case. */
        return const_flag | CF_LONG;
    }

    /* Otherwise, if either operand has type long int, the other operand is converted to long int.
    */
    if (ltype == CF_LONG || rtype == CF_LONG) {
        return const_flag | CF_LONG;
    }

    /* Otherwise, if either operand has type unsigned int, the other operand is converted to
    ** unsigned int.
    */
    if ((ltype == CF_INT && (lhs & CF_UNSIGNED)) ||
        (rtype == CF_INT && (rhs & CF_UNSIGNED))) {
        return const_flag | CF_UNSIGNED | CF_INT;
    }

    /* Otherwise, both operands have type int. */
    CHECK (ltype == CF_INT);
    CHECK (!(lhs & CF_UNSIGNED));
    CHECK (rtype == CF_INT);
    CHECK (!(rhs & CF_UNSIGNED));
    return const_flag | CF_INT;
}



unsigned g_typecast (unsigned to, unsigned from)
/* Cast the value in the primary register to the specified operand size and
** signedness. Return the result flags.
*/
{
    /* Check if a conversion is needed */
    if ((from & CF_CONST) == 0) {
        switch (to & CF_TYPEMASK) {

            case CF_LONG:
                /* We must promote the primary register to long in EAX */
                g_reglong (from);
                break;

            case CF_INT:
                /* We must promote the primary register to int in AX */
                g_regint (from);
                break;

            case CF_CHAR:
                /* We must truncate the primary register to char and then
                ** sign-extend it to signed int in AX.
                */
                g_regchar (to);
                break;

            default:
                /* Since we are switching on "to", report an error on it */
                typeerror (to);
        }
    }

    /* Do not need any other action. If the "to" type is int, and the primary
    ** register is long, it will be automagically truncated. If the right hand
    ** side is const, it is not located in the primary register and handled by
    ** the expression parser code.
    */

    /* Result is const if the right hand side was const */
    to |= (from & CF_CONST);

    /* The resulting type is "to" (that's why you called this function :-) */
    return to;
}



void g_scale (unsigned flags, long val)
/* Scale the value in the primary register by the given value. If val is positive,
** scale up, is val is negative, scale down. This function is used to scale
** the operands or results of pointer arithmetic by the size of the type, the
** pointer points to.
*/
{
    /* Value may not be zero */
    if (val == 0) {
        Internal ("Data type has no size");
    } else if (val > 0) {

        /* Use a multiplication instead */
        if (val != 1) {
            g_mul (flags | CF_CONST, val);
        }

    } else {

        /* Scale down */
        val = -val;

        /* Use a division instead */
        if (val != 1) {
            g_div (flags | CF_CONST, val);
        }
    }
}



/*****************************************************************************/
/*              Adds and subs of variables fix a fixed address               */
/*****************************************************************************/



void g_addlocal (unsigned flags, int offs)
/* Add a local variable to ax */
{
    unsigned L;
    int NewOff;

    /* Correct the offset and check it */
    NewOff = offs - StackPtr;
    CheckLocalOffs (NewOff);

    switch (flags & CF_TYPEMASK) {

        case CF_CHAR:
            L = GetLocalLabel();
            AddCodeLine ("ldy #$%02X", NewOff & 0xFF);
            AddCodeLine ("clc");
            AddCodeLine ("adc (sp),y");
            AddCodeLine ("bcc %s", LocalLabelName (L));
            AddCodeLine ("inx");
            g_defcodelabel (L);
            break;

        case CF_INT:
            AddCodeLine ("ldy #$%02X", NewOff & 0xFF);
            AddCodeLine ("clc");
            AddCodeLine ("adc (sp),y");
            AddCodeLine ("pha");
            AddCodeLine ("txa");
            AddCodeLine ("iny");
            AddCodeLine ("adc (sp),y");
            AddCodeLine ("tax");
            AddCodeLine ("pla");
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



void g_addstatic (unsigned flags, uintptr_t label, long offs)
/* Add a static variable to ax */
{
    unsigned L;

    /* Create the correct label name */
    const char* lbuf = GetLabelName (flags, label, offs);

    switch (flags & CF_TYPEMASK) {

        case CF_CHAR:
            L = GetLocalLabel();
            AddCodeLine ("clc");
            AddCodeLine ("adc %s", lbuf);
            AddCodeLine ("bcc %s", LocalLabelName (L));
            AddCodeLine ("inx");
            g_defcodelabel (L);
            break;

        case CF_INT:
            AddCodeLine ("clc");
            AddCodeLine ("adc %s", lbuf);
            AddCodeLine ("tay");
            AddCodeLine ("txa");
            AddCodeLine ("adc %s+1", lbuf);
            AddCodeLine ("tax");
            AddCodeLine ("tya");
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
/*                           Special op= functions                           */
/*****************************************************************************/



void g_addeqstatic (unsigned flags, uintptr_t label, long offs,
                    unsigned long val)
/* Emit += for a static variable */
{
    /* Create the correct label name */
    const char* lbuf = GetLabelName (flags, label, offs);

    /* Check the size and determine operation */
    switch (flags & CF_TYPEMASK) {

        case CF_CHAR:
            if (flags & CF_FORCECHAR) {
                AddCodeLine ("ldx #$00");
                if (flags & CF_CONST) {
                    if (val == 1) {
                        AddCodeLine ("inc %s", lbuf);
                        if ((flags & CF_NOKEEP) == 0) {
                            AddCodeLine ("lda %s", lbuf);
                        }
                    } else {
                        AddCodeLine ("lda #$%02X", (int)(val & 0xFF));
                        AddCodeLine ("clc");
                        AddCodeLine ("adc %s", lbuf);
                        AddCodeLine ("sta %s", lbuf);
                    }
                } else {
                    AddCodeLine ("clc");
                    AddCodeLine ("adc %s", lbuf);
                    AddCodeLine ("sta %s", lbuf);
                }
                if ((flags & CF_UNSIGNED) == 0) {
                    unsigned L = GetLocalLabel();
                    AddCodeLine ("bpl %s", LocalLabelName (L));
                    AddCodeLine ("dex");
                    g_defcodelabel (L);
                }
                break;
            }
            /* FALLTHROUGH */

        case CF_INT:
            if (flags & CF_CONST) {
                if (val == 1) {
                    unsigned L = GetLocalLabel ();
                    AddCodeLine ("inc %s", lbuf);
                    AddCodeLine ("bne %s", LocalLabelName (L));
                    AddCodeLine ("inc %s+1", lbuf);
                    g_defcodelabel (L);
                    if ((flags & CF_NOKEEP) == 0) {
                        AddCodeLine ("lda %s", lbuf);               /* Hmmm... */
                        AddCodeLine ("ldx %s+1", lbuf);
                    }
                } else {
                    AddCodeLine ("lda #$%02X", (int)(val & 0xFF));
                    AddCodeLine ("clc");
                    AddCodeLine ("adc %s", lbuf);
                    AddCodeLine ("sta %s", lbuf);
                    if (val < 0x100) {
                        unsigned L = GetLocalLabel ();
                        AddCodeLine ("bcc %s", LocalLabelName (L));
                        AddCodeLine ("inc %s+1", lbuf);
                        g_defcodelabel (L);
                        if ((flags & CF_NOKEEP) == 0) {
                            AddCodeLine ("ldx %s+1", lbuf);
                        }
                    } else {
                        AddCodeLine ("lda #$%02X", (unsigned char)(val >> 8));
                        AddCodeLine ("adc %s+1", lbuf);
                        AddCodeLine ("sta %s+1", lbuf);
                        if ((flags & CF_NOKEEP) == 0) {
                            AddCodeLine ("tax");
                            AddCodeLine ("lda %s", lbuf);
                        }
                    }
                }
            } else {
                AddCodeLine ("clc");
                AddCodeLine ("adc %s", lbuf);
                AddCodeLine ("sta %s", lbuf);
                AddCodeLine ("txa");
                AddCodeLine ("adc %s+1", lbuf);
                AddCodeLine ("sta %s+1", lbuf);
                if ((flags & CF_NOKEEP) == 0) {
                    AddCodeLine ("tax");
                    AddCodeLine ("lda %s", lbuf);
                }
            }
            break;

        case CF_LONG:
            if (flags & CF_CONST) {
                if (val < 0x100) {
                    AddCodeLine ("ldy #<(%s)", lbuf);
                    AddCodeLine ("sty ptr1");
                    AddCodeLine ("ldy #>(%s)", lbuf);
                    if (val == 1) {
                        AddCodeLine ("jsr laddeq1");
                    } else {
                        AddCodeLine ("lda #$%02X", (int)(val & 0xFF));
                        AddCodeLine ("jsr laddeqa");
                    }
                } else {
                    g_getstatic (flags, label, offs);
                    g_inc (flags, val);
                    g_putstatic (flags, label, offs);
                }
            } else {
                AddCodeLine ("ldy #<(%s)", lbuf);
                AddCodeLine ("sty ptr1");
                AddCodeLine ("ldy #>(%s)", lbuf);
                AddCodeLine ("jsr laddeq");
            }
            break;

        default:
            typeerror (flags);
    }
}



void g_addeqlocal (unsigned flags, int Offs, unsigned long val)
/* Emit += for a local variable */
{
    /* Calculate the true offset, check it, load it into Y */
    Offs -= StackPtr;
    CheckLocalOffs (Offs);

    /* Check the size and determine operation */
    switch (flags & CF_TYPEMASK) {

        case CF_CHAR:
            if (flags & CF_FORCECHAR) {
                AddCodeLine ("ldy #$%02X", Offs);
                AddCodeLine ("ldx #$00");
                if (flags & CF_CONST) {
                    AddCodeLine ("clc");
                    AddCodeLine ("lda #$%02X", (int)(val & 0xFF));
                    AddCodeLine ("adc (sp),y");
                    AddCodeLine ("sta (sp),y");
                } else {
                    AddCodeLine ("clc");
                    AddCodeLine ("adc (sp),y");
                    AddCodeLine ("sta (sp),y");
                }
                if ((flags & CF_UNSIGNED) == 0) {
                    unsigned L = GetLocalLabel();
                    AddCodeLine ("bpl %s", LocalLabelName (L));
                    AddCodeLine ("dex");
                    g_defcodelabel (L);
                }
                break;
            }
            /* FALLTHROUGH */

        case CF_INT:
            AddCodeLine ("ldy #$%02X", Offs);
            if (flags & CF_CONST) {
                if (IS_Get (&CodeSizeFactor) >= 400) {
                    AddCodeLine ("clc");
                    AddCodeLine ("lda #$%02X", (int)(val & 0xFF));
                    AddCodeLine ("adc (sp),y");
                    AddCodeLine ("sta (sp),y");
                    AddCodeLine ("iny");
                    AddCodeLine ("lda #$%02X", (int) ((val >> 8) & 0xFF));
                    AddCodeLine ("adc (sp),y");
                    AddCodeLine ("sta (sp),y");
                    if ((flags & CF_NOKEEP) == 0) {
                        AddCodeLine ("tax");
                        AddCodeLine ("dey");
                        AddCodeLine ("lda (sp),y");
                    }
                } else {
                    g_getimmed (flags, val, 0);
                    AddCodeLine ("jsr addeqysp");
                }
            } else {
                AddCodeLine ("jsr addeqysp");
            }
            break;

        case CF_LONG:
            if (flags & CF_CONST) {
                g_getimmed (flags, val, 0);
            }
            AddCodeLine ("ldy #$%02X", Offs);
            AddCodeLine ("jsr laddeqysp");
            break;

        default:
            typeerror (flags);
    }
}



void g_addeqind (unsigned flags, unsigned offs, unsigned long val)
/* Emit += for the location with address in ax */
{
    /* If the offset is too large for a byte register, add the high byte
    ** of the offset to the primary. Beware: We need a special correction
    ** if the offset in the low byte will overflow in the operation.
    */
    offs = MakeByteOffs (flags, offs);

    /* Check the size and determine operation */
    switch (flags & CF_TYPEMASK) {

        case CF_CHAR:
            AddCodeLine ("sta ptr1");
            AddCodeLine ("stx ptr1+1");
            AddCodeLine ("ldy #$%02X", offs);
            AddCodeLine ("ldx #$00");
            AddCodeLine ("lda #$%02X", (int)(val & 0xFF));
            AddCodeLine ("clc");
            AddCodeLine ("adc (ptr1),y");
            AddCodeLine ("sta (ptr1),y");
            break;

        case CF_INT:
        case CF_LONG:
            AddCodeLine ("jsr pushax");         /* Push the address */
            push (CF_PTR);                      /* Correct the internal sp */
            g_getind (flags, offs);             /* Fetch the value */
            g_inc (flags, val);                 /* Increment value in primary */
            g_putind (flags, offs);             /* Store the value back */
            break;

        default:
            typeerror (flags);
    }
}



void g_subeqstatic (unsigned flags, uintptr_t label, long offs,
                    unsigned long val)
/* Emit -= for a static variable */
{
    /* Create the correct label name */
    const char* lbuf = GetLabelName (flags, label, offs);

    /* Check the size and determine operation */
    switch (flags & CF_TYPEMASK) {

        case CF_CHAR:
            if (flags & CF_FORCECHAR) {
                AddCodeLine ("ldx #$00");
                if (flags & CF_CONST) {
                    if (val == 1) {
                        AddCodeLine ("dec %s", lbuf);
                        if ((flags & CF_NOKEEP) == 0) {
                            AddCodeLine ("lda %s", lbuf);
                        }
                    } else {
                        AddCodeLine ("lda %s", lbuf);
                        AddCodeLine ("sec");
                        AddCodeLine ("sbc #$%02X", (int)(val & 0xFF));
                        AddCodeLine ("sta %s", lbuf);
                    }
                } else {
                    AddCodeLine ("eor #$FF");
                    AddCodeLine ("sec");
                    AddCodeLine ("adc %s", lbuf);
                    AddCodeLine ("sta %s", lbuf);
                }
                if ((flags & CF_UNSIGNED) == 0) {
                    unsigned L = GetLocalLabel();
                    AddCodeLine ("bpl %s", LocalLabelName (L));
                    AddCodeLine ("dex");
                    g_defcodelabel (L);
                }
                break;
            }
            /* FALLTHROUGH */

        case CF_INT:
            if (flags & CF_CONST) {
                if (val == 1) {
                    unsigned L = GetLocalLabel();
                    if ((flags & CF_NOKEEP) == 0) {
                        if ((CPUIsets[CPU] & CPU_ISET_65SC02) != 0) {
                            AddCodeLine ("lda %s", lbuf);
                            AddCodeLine ("bne %s", LocalLabelName (L));
                            AddCodeLine ("dec %s+1", lbuf);
                            g_defcodelabel (L);
                            AddCodeLine ("dea");
                            AddCodeLine ("sta %s", lbuf);
                            AddCodeLine ("ldx %s+1", lbuf);
                        } else {
                            AddCodeLine ("ldx %s", lbuf);
                            AddCodeLine ("bne %s", LocalLabelName (L));
                            AddCodeLine ("dec %s+1", lbuf);
                            g_defcodelabel (L);
                            AddCodeLine ("dex");
                            AddCodeLine ("stx %s", lbuf);
                            AddCodeLine ("txa");
                            AddCodeLine ("ldx %s+1", lbuf);
                        }
                    } else {
                        AddCodeLine ("lda %s", lbuf);
                        AddCodeLine ("bne %s", LocalLabelName (L));
                        AddCodeLine ("dec %s+1", lbuf);
                        g_defcodelabel (L);
                        AddCodeLine ("dec %s", lbuf);
                    }
                } else {
                    AddCodeLine ("lda %s", lbuf);
                    AddCodeLine ("sec");
                    AddCodeLine ("sbc #$%02X", (unsigned char)val);
                    AddCodeLine ("sta %s", lbuf);
                    if (val < 0x100) {
                        unsigned L = GetLocalLabel ();
                        AddCodeLine ("bcs %s", LocalLabelName (L));
                        AddCodeLine ("dec %s+1", lbuf);
                        g_defcodelabel (L);
                        if ((flags & CF_NOKEEP) == 0) {
                            AddCodeLine ("ldx %s+1", lbuf);
                        }
                    } else {
                        AddCodeLine ("lda %s+1", lbuf);
                        AddCodeLine ("sbc #$%02X", (unsigned char)(val >> 8));
                        AddCodeLine ("sta %s+1", lbuf);
                        if ((flags & CF_NOKEEP) == 0) {
                            AddCodeLine ("tax");
                            AddCodeLine ("lda %s", lbuf);
                        }
                    }
                }
            } else {
                AddCodeLine ("eor #$FF");
                AddCodeLine ("sec");
                AddCodeLine ("adc %s", lbuf);
                AddCodeLine ("sta %s", lbuf);
                AddCodeLine ("txa");
                AddCodeLine ("eor #$FF");
                AddCodeLine ("adc %s+1", lbuf);
                AddCodeLine ("sta %s+1", lbuf);
                if ((flags & CF_NOKEEP) == 0) {
                    AddCodeLine ("tax");
                    AddCodeLine ("lda %s", lbuf);
                }
            }
            break;

        case CF_LONG:
            if (flags & CF_CONST) {
                if (val < 0x100) {
                    AddCodeLine ("ldy #<(%s)", lbuf);
                    AddCodeLine ("sty ptr1");
                    AddCodeLine ("ldy #>(%s)", lbuf);
                    AddCodeLine ("lda #$%02X", (unsigned char)val);
                    AddCodeLine ("jsr lsubeqa");
                } else {
                    g_getstatic (flags, label, offs);
                    g_dec (flags, val);
                    g_putstatic (flags, label, offs);
                }
            } else {
                AddCodeLine ("ldy #<(%s)", lbuf);
                AddCodeLine ("sty ptr1");
                AddCodeLine ("ldy #>(%s)", lbuf);
                AddCodeLine ("jsr lsubeq");
            }
            break;

        default:
            typeerror (flags);
    }
}



void g_subeqlocal (unsigned flags, int Offs, unsigned long val)
/* Emit -= for a local variable */
{
    /* Calculate the true offset, check it, load it into Y */
    Offs -= StackPtr;
    CheckLocalOffs (Offs);

    /* Check the size and determine operation */
    switch (flags & CF_TYPEMASK) {

        case CF_CHAR:
            if (flags & CF_FORCECHAR) {
                AddCodeLine ("ldy #$%02X", Offs);
                AddCodeLine ("ldx #$00");
                if (flags & CF_CONST) {
                    AddCodeLine ("lda (sp),y");
                    AddCodeLine ("sec");
                    AddCodeLine ("sbc #$%02X", (unsigned char)val);
                } else {
                    AddCodeLine ("eor #$FF");
                    AddCodeLine ("sec");
                    AddCodeLine ("adc (sp),y");
                }
                AddCodeLine ("sta (sp),y");
                if ((flags & CF_UNSIGNED) == 0) {
                    unsigned L = GetLocalLabel();
                    AddCodeLine ("bpl %s", LocalLabelName (L));
                    AddCodeLine ("dex");
                    g_defcodelabel (L);
                }
                break;
            }
            /* FALLTHROUGH */

        case CF_INT:
            if (flags & CF_CONST) {
                g_getimmed (flags, val, 0);
            }
            AddCodeLine ("ldy #$%02X", Offs);
            AddCodeLine ("jsr subeqysp");
            break;

        case CF_LONG:
            if (flags & CF_CONST) {
                g_getimmed (flags, val, 0);
            }
            AddCodeLine ("ldy #$%02X", Offs);
            AddCodeLine ("jsr lsubeqysp");
            break;

        default:
            typeerror (flags);
    }
}



void g_subeqind (unsigned flags, unsigned offs, unsigned long val)
/* Emit -= for the location with address in ax */
{
    /* If the offset is too large for a byte register, add the high byte
    ** of the offset to the primary. Beware: We need a special correction
    ** if the offset in the low byte will overflow in the operation.
    */
    offs = MakeByteOffs (flags, offs);

    /* Check the size and determine operation */
    switch (flags & CF_TYPEMASK) {

        case CF_CHAR:
            AddCodeLine ("sta ptr1");
            AddCodeLine ("stx ptr1+1");
            AddCodeLine ("ldy #$%02X", offs);
            AddCodeLine ("ldx #$00");
            AddCodeLine ("lda (ptr1),y");
            AddCodeLine ("sec");
            AddCodeLine ("sbc #$%02X", (unsigned char)val);
            AddCodeLine ("sta (ptr1),y");
            break;

        case CF_INT:
        case CF_LONG:
            AddCodeLine ("jsr pushax");         /* Push the address */
            push (CF_PTR);                      /* Correct the internal sp */
            g_getind (flags, offs);             /* Fetch the value */
            g_dec (flags, val);                 /* Increment value in primary */
            g_putind (flags, offs);             /* Store the value back */
            break;

        default:
            typeerror (flags);
    }
}



/*****************************************************************************/
/*                 Add a variable address to the value in ax                 */
/*****************************************************************************/



void g_addaddr_local (unsigned flags attribute ((unused)), int offs)
/* Add the address of a local variable to ax */
{
    unsigned L = 0;

    /* Add the offset */
    offs -= StackPtr;
    if (IS_Get (&CodeSizeFactor) <= 100) {
        if (offs != 0) {
            /* We cannot address more then 256 bytes of locals anyway */
            g_inc (CF_INT | CF_CONST, offs);
        }
        /* Add the current stackpointer value */
        AddCodeLine ("jsr leaaxsp");
    } else {
        if (offs != 0) {
            /* We cannot address more then 256 bytes of locals anyway */
            L = GetLocalLabel();
            CheckLocalOffs (offs);
            AddCodeLine ("clc");
            AddCodeLine ("adc #$%02X", offs & 0xFF);
            /* Do also skip the CLC insn below */
            AddCodeLine ("bcc %s", LocalLabelName (L));
            AddCodeLine ("inx");
        }

        /* Add the current stackpointer value */
        AddCodeLine ("clc");
        if (L != 0) {
            /* Label was used above */
            g_defcodelabel (L);
        }
        AddCodeLine ("adc sp");
        AddCodeLine ("tay");
        AddCodeLine ("txa");
        AddCodeLine ("adc sp+1");
        AddCodeLine ("tax");
        AddCodeLine ("tya");
    }
}



void g_addaddr_static (unsigned flags, uintptr_t label, long offs)
/* Add the address of a static variable to ax */
{
    /* Create the correct label name */
    const char* lbuf = GetLabelName (flags, label, offs);

    /* Add the address to the current ax value */
    AddCodeLine ("clc");
    AddCodeLine ("adc #<(%s)", lbuf);
    AddCodeLine ("tay");
    AddCodeLine ("txa");
    AddCodeLine ("adc #>(%s)", lbuf);
    AddCodeLine ("tax");
    AddCodeLine ("tya");
}



/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/



void g_save (unsigned flags)
/* Copy primary register to hold register. */
{
    /* Check the size and determine operation */
    switch (flags & CF_TYPEMASK) {

        case CF_CHAR:
            if (flags & CF_FORCECHAR) {
                AddCodeLine ("pha");
                break;
            }
            /* FALLTHROUGH */

        case CF_INT:
            AddCodeLine ("sta regsave");
            AddCodeLine ("stx regsave+1");
            break;

        case CF_LONG:
            AddCodeLine ("jsr saveeax");
            break;

        default:
            typeerror (flags);
    }
}



void g_restore (unsigned flags)
/* Copy hold register to primary. */
{
    /* Check the size and determine operation */
    switch (flags & CF_TYPEMASK) {

        case CF_CHAR:
            if (flags & CF_FORCECHAR) {
                AddCodeLine ("pla");
                break;
            }
            /* FALLTHROUGH */

        case CF_INT:
            AddCodeLine ("lda regsave");
            AddCodeLine ("ldx regsave+1");
            break;

        case CF_LONG:
            AddCodeLine ("jsr resteax");
            break;

        default:
            typeerror (flags);
    }
}



void g_cmp (unsigned flags, unsigned long val)
/* Immediate compare. The primary register will not be changed, Z flag
** will be set.
*/
{
    unsigned L;

    /* Check the size and determine operation */
    switch (flags & CF_TYPEMASK) {

        case CF_CHAR:
            if (flags & CF_FORCECHAR) {
                AddCodeLine ("cmp #$%02X", (unsigned char)val);
                break;
            }
            /* FALLTHROUGH */

        case CF_INT:
            L = GetLocalLabel();
            AddCodeLine ("cmp #$%02X", (unsigned char)val);
            AddCodeLine ("bne %s", LocalLabelName (L));
            AddCodeLine ("cpx #$%02X", (unsigned char)(val >> 8));
            g_defcodelabel (L);
            break;

        case CF_LONG:
            Internal ("g_cmp: Long compares not implemented");
            break;

        default:
            typeerror (flags);
    }
}



static void oper (unsigned Flags, unsigned long Val, const char* const* Subs)
/* Encode a binary operation. subs is a pointer to four strings:
**      0       --> Operate on ints
**      1       --> Operate on unsigneds
**      2       --> Operate on longs
**      3       --> Operate on unsigned longs
*/
{
    /* Determine the offset into the array */
    if (Flags & CF_UNSIGNED) {
        ++Subs;
    }
    if ((Flags & CF_TYPEMASK) == CF_LONG) {
        Subs += 2;
    }

    /* Load the value if it is not already in the primary */
    if (Flags & CF_CONST) {
        /* Load value */
        g_getimmed (Flags, Val, 0);
    }

    /* Output the operation */
    AddCodeLine ("jsr %s", *Subs);

    /* The operation will pop it's argument */
    pop (Flags);
}



void g_test (unsigned flags)
/* Test the value in the primary and set the condition codes */
{
    switch (flags & CF_TYPEMASK) {

        case CF_CHAR:
            if (flags & CF_FORCECHAR) {
                AddCodeLine ("tax");
                break;
            }
            /* FALLTHROUGH */

        case CF_INT:
            AddCodeLine ("stx tmp1");
            AddCodeLine ("ora tmp1");
            break;

        case CF_LONG:
            if (flags & CF_UNSIGNED) {
                AddCodeLine ("jsr utsteax");
            } else {
                AddCodeLine ("jsr tsteax");
            }
            break;

        default:
            typeerror (flags);

    }
}



void g_push (unsigned flags, unsigned long val)
/* Push the primary register or a constant value onto the stack */
{
    if (flags & CF_CONST && (flags & CF_TYPEMASK) != CF_LONG) {

        /* We have a constant 8 or 16 bit value */
        if ((flags & CF_TYPEMASK) == CF_CHAR && (flags & CF_FORCECHAR)) {

            /* Handle as 8 bit value */
            AddCodeLine ("lda #$%02X", (unsigned char) val);
            AddCodeLine ("jsr pusha");

        } else {

            /* Handle as 16 bit value */
            g_getimmed (flags, val, 0);
            AddCodeLine ("jsr pushax");
        }

    } else {

        /* Value is not 16 bit or not constant */
        if (flags & CF_CONST) {
            /* Constant 32 bit value, load into eax */
            g_getimmed (flags, val, 0);
        }

        /* Push the primary register */
        switch (flags & CF_TYPEMASK) {

            case CF_CHAR:
                if (flags & CF_FORCECHAR) {
                    /* Handle as char */
                    AddCodeLine ("jsr pusha");
                    break;
                }
                /* FALL THROUGH */
            case CF_INT:
                AddCodeLine ("jsr pushax");
                break;

            case CF_LONG:
                AddCodeLine ("jsr pusheax");
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
** of *both* values (must have same size).
*/
{
    switch (flags & CF_TYPEMASK) {

        case CF_CHAR:
        case CF_INT:
            AddCodeLine ("jsr swapstk");
            break;

        case CF_LONG:
            AddCodeLine ("jsr swapestk");
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
        AddCodeLine ("ldy #$%02X", ArgSize);
    }
    AddCodeLine ("jsr _%s", Label);
    StackPtr += ArgSize;                /* callee pops args */
}



void g_callind (unsigned Flags, unsigned ArgSize, int Offs)
/* Call subroutine indirect */
{
    if ((Flags & CF_ADDRMASK) != CF_STACK) {
        /* Address is in a/x */
        if ((Flags & CF_FIXARGC) == 0) {
            /* Pass arg count */
            AddCodeLine ("ldy #$%02X", ArgSize);
        }
        AddCodeLine ("jsr callax");
    } else {
        /* The address is on stack, offset is on Val */
        Offs -= StackPtr;
        CheckLocalOffs (Offs);
        AddCodeLine ("pha");
        AddCodeLine ("ldy #$%02X", Offs);
        AddCodeLine ("lda (sp),y");
        AddCodeLine ("sta jmpvec+1");
        AddCodeLine ("iny");
        AddCodeLine ("lda (sp),y");
        AddCodeLine ("sta jmpvec+2");
        AddCodeLine ("pla");
        AddCodeLine ("jsr jmpvec");
    }

    /* Callee pops args */
    StackPtr += ArgSize;
}



void g_jump (unsigned Label)
/* Jump to specified internal label number */
{
    AddCodeLine ("jmp %s", LocalLabelName (Label));
}



void g_truejump (unsigned flags attribute ((unused)), unsigned label)
/* Jump to label if zero flag clear */
{
    AddCodeLine ("jne %s", LocalLabelName (label));
}



void g_falsejump (unsigned flags attribute ((unused)), unsigned label)
/* Jump to label if zero flag set */
{
    AddCodeLine ("jeq %s", LocalLabelName (label));
}



void g_branch (unsigned Label)
/* Branch unconditionally to Label if the CPU has the BRA instruction.
** Otherwise, jump to Label.
** Use this function, instead of g_jump(), only where it is certain that
** the label cannot be farther away from the branch than -128/+127 bytes.
*/
{
    if ((CPUIsets[CPU] & (CPU_ISET_65SC02 | CPU_ISET_6502DTV)) != 0) {
        AddCodeLine ("bra %s", LocalLabelName (Label));
    } else {
        g_jump (Label);
    }
}



void g_lateadjustSP (unsigned label)
/* Adjust stack based on non-immediate data */
{
    AddCodeLine ("pha");
    AddCodeLine ("lda %s", LocalDataLabelName (label));
    AddCodeLine ("clc");
    AddCodeLine ("adc sp");
    AddCodeLine ("sta sp");
    AddCodeLine ("lda %s+1", LocalDataLabelName (label));
    AddCodeLine ("adc sp+1");
    AddCodeLine ("sta sp+1");
    AddCodeLine ("pla");
}

void g_drop (unsigned Space)
/* Drop space allocated on the stack */
{
    if (Space > 255) {
        /* Inline the code since calling addysp repeatedly is quite some
        ** overhead.
        */
        AddCodeLine ("pha");
        AddCodeLine ("lda #$%02X", (unsigned char) Space);
        AddCodeLine ("clc");
        AddCodeLine ("adc sp");
        AddCodeLine ("sta sp");
        AddCodeLine ("lda #$%02X", (unsigned char) (Space >> 8));
        AddCodeLine ("adc sp+1");
        AddCodeLine ("sta sp+1");
        AddCodeLine ("pla");
    } else if (Space > 8) {
        AddCodeLine ("ldy #$%02X", Space);
        AddCodeLine ("jsr addysp");
    } else if (Space != 0) {
        AddCodeLine ("jsr incsp%u", Space);
    }
}



void g_space (int Space)
/* Create or drop space on the stack */
{
    if (Space < 0) {
        /* This is actually a drop operation */
        g_drop (-Space);
    } else if (Space > 255) {
        /* Inline the code since calling subysp repeatedly is quite some
        ** overhead.
        */
        AddCodeLine ("pha");
        AddCodeLine ("lda sp");
        AddCodeLine ("sec");
        AddCodeLine ("sbc #$%02X", (unsigned char) Space);
        AddCodeLine ("sta sp");
        AddCodeLine ("lda sp+1");
        AddCodeLine ("sbc #$%02X", (unsigned char) (Space >> 8));
        AddCodeLine ("sta sp+1");
        AddCodeLine ("pla");
    } else if (Space > 8) {
        AddCodeLine ("ldy #$%02X", Space);
        AddCodeLine ("jsr subysp");
    } else if (Space != 0) {
        AddCodeLine ("jsr decsp%u", Space);
    }
}



void g_cstackcheck (void)
/* Check for a C stack overflow */
{
    AddCodeLine ("jsr cstkchk");
}



void g_stackcheck (void)
/* Check for a stack overflow */
{
    AddCodeLine ("jsr stkchk");
}



void g_add (unsigned flags, unsigned long val)
/* Primary = TOS + Primary */
{
    static const char* const ops[4] = {
        "tosaddax", "tosaddax", "tosaddeax", "tosaddeax"
    };

    if (flags & CF_CONST) {
        flags &= ~CF_FORCECHAR; /* Handle chars as ints */
        g_push (flags & ~CF_CONST, 0);
    }
    oper (flags, val, ops);
}



void g_sub (unsigned flags, unsigned long val)
/* Primary = TOS - Primary */
{
    static const char* const ops[4] = {
        "tossubax", "tossubax", "tossubeax", "tossubeax"
    };

    if (flags & CF_CONST) {
        flags &= ~CF_FORCECHAR; /* Handle chars as ints */
        g_push (flags & ~CF_CONST, 0);
    }
    oper (flags, val, ops);
}



void g_rsub (unsigned flags, unsigned long val)
/* Primary = Primary - TOS */
{
    static const char* const ops[4] = {
        "tosrsubax", "tosrsubax", "tosrsubeax", "tosrsubeax"
    };
    oper (flags, val, ops);
}



void g_mul (unsigned flags, unsigned long val)
/* Primary = TOS * Primary */
{
    static const char* const ops[4] = {
        "tosmulax", "tosumulax", "tosmuleax", "tosumuleax"
    };

    /* Do strength reduction if the value is constant and a power of two */
    if (flags & CF_CONST) {

        /* Deal with negative values if it's signed multiplication */
        int Negation = (flags & CF_UNSIGNED) == 0 && (long)val < 0;
        int p2 = PowerOf2 (Negation ? 0UL - val : val);

        /* Check if we can use shift instead of multiplication */
        if (p2 == 0 || (p2 > 0 && IS_Get (&CodeSizeFactor) >= (Negation ? 100 : 0))) {
            /* Generate a shift instead */
            g_asl (flags, p2);

            /* Negate the result if val is negative */
            if (Negation) {
                g_neg (flags);
            }

            /* Done */
            return;
        }
    }

    /* If the right hand side is const, the lhs is not on stack but still
    ** in the primary register.
    */
    if (flags & CF_CONST) {

        switch (flags & CF_TYPEMASK) {

            case CF_CHAR:
                if (flags & CF_FORCECHAR) {
                    /* Handle some special cases */
                    switch (val) {
                        case 0:
                            AddCodeLine ("lda #$00");
                            return;
                        case 1:
                            /* Nothing to do */
                            return;
                        case 3:
                            AddCodeLine ("sta tmp1");
                            AddCodeLine ("asl a");
                            AddCodeLine ("clc");
                            AddCodeLine ("adc tmp1");
                            return;

                        case 5:
                            AddCodeLine ("sta tmp1");
                            AddCodeLine ("asl a");
                            AddCodeLine ("asl a");
                            AddCodeLine ("clc");
                            AddCodeLine ("adc tmp1");
                            return;

                        case 6:
                            AddCodeLine ("sta tmp1");
                            AddCodeLine ("asl a");
                            AddCodeLine ("clc");
                            AddCodeLine ("adc tmp1");
                            AddCodeLine ("asl a");
                            return;

                        case 10:
                            AddCodeLine ("sta tmp1");
                            AddCodeLine ("asl a");
                            AddCodeLine ("asl a");
                            AddCodeLine ("clc");
                            AddCodeLine ("adc tmp1");
                            AddCodeLine ("asl a");
                            return;
                    }
                }
                /* FALLTHROUGH */

            case CF_INT:
                switch (val) {
                    case 0:
                        AddCodeLine ("lda #$00");
                        AddCodeLine ("tax");
                        return;
                    case 1:
                        /* Nothing to do */
                        return;
                    case 3:
                        AddCodeLine ("jsr mulax3");
                        return;
                    case 5:
                        AddCodeLine ("jsr mulax5");
                        return;
                    case 6:
                        AddCodeLine ("jsr mulax6");
                        return;
                    case 7:
                        AddCodeLine ("jsr mulax7");
                        return;
                    case 9:
                        AddCodeLine ("jsr mulax9");
                        return;
                    case 10:
                        AddCodeLine ("jsr mulax10");
                        return;
                }
                break;

            case CF_LONG:
                break;

            default:
                typeerror (flags);
        }

        /* If we go here, we didn't emit code. Push the lhs on stack and fall
        ** into the normal, non-optimized stuff.
        */
        flags &= ~CF_FORCECHAR; /* Handle chars as ints */
        g_push (flags & ~CF_CONST, 0);

    }

    /* Use long way over the stack */
    oper (flags, val, ops);
}



void g_div (unsigned flags, unsigned long val)
/* Primary = TOS / Primary */
{
    static const char* const ops[4] = {
        "tosdivax", "tosudivax", "tosdiveax", "tosudiveax"
    };

    /* Do strength reduction if the value is constant and a power of two */
    if (flags & CF_CONST) {

        /* Deal with negative values as well as different sizes */
        int           Negation   = (flags & CF_UNSIGNED) == 0 && (long)val < 0;
        unsigned long NegatedVal = 0UL - val;
        int           p2         = PowerOf2 (Negation ? NegatedVal : val);

        /* Generate a shift instead */
        if ((flags & CF_UNSIGNED) != 0 && p2 > 0) {
            g_asr (flags, p2);
            return;
        }

        /* Check if we can afford using shift instead of multiplication at the
        ** cost of code size */
        if (p2 == 0 || (p2 > 0 && IS_Get (&CodeSizeFactor) >= (Negation ? 200 : 170))) {
            /* Generate a conditional shift instead */
            if (p2 > 0) {
                unsigned int  DoShiftLabel = GetLocalLabel ();
                unsigned int  EndLabel     = GetLocalLabel ();
                unsigned long MaskedVal    = Negation ? val : NegatedVal;

                /* GitHub #169 - if abs(expr) < abs(val), the result is always 0.
                ** First, check whether expr >= 0 and skip to the shift if true.
                */
                switch (flags & CF_TYPEMASK) {
                case CF_CHAR:
                    if (flags & CF_FORCECHAR) {
                        MaskedVal &= 0xFF;
                        AddCodeLine ("cmp #$00");
                        AddCodeLine ("bpl %s", LocalLabelName (DoShiftLabel));
                        break;
                    }
                    /* FALLTHROUGH */

                case CF_INT:
                    MaskedVal &= 0xFFFF;
                    AddCodeLine ("cpx #$00");
                    AddCodeLine ("bpl %s", LocalLabelName (DoShiftLabel));
                    break;

                case CF_LONG:
                    MaskedVal &= 0xFFFFFFFF;
                    AddCodeLine ("ldy sreg+1");
                    AddCodeLine ("bpl %s", LocalLabelName (DoShiftLabel));
                    break;

                default:
                    typeerror (flags);
                    break;
                }
                /* Second, check whether expr <= -asb(val) and skip to the
                ** shift if true. The original content of expr has to be saved
                ** before the checking comparison and restored after that, as
                ** the content in Primary register will be destroyed.
                ** The result of the comparison is a boolean. We can store
                ** it in the Carry flag with a LSR and branch on it later.
                */
                g_save (flags);
                g_le (flags | CF_UNSIGNED, MaskedVal);
                AddCodeLine ("lsr a");
                g_restore (flags);
                AddCodeLine ("bcs %s", LocalLabelName (DoShiftLabel));

                /* The result is 0. We can just load 0 and skip the shifting. */
                g_getimmed (flags | CF_ABSOLUTE, 0, 0);

                /* TODO: replace with BEQ? Would it be optimized? */
                g_jump (EndLabel);

                /* Do the shift. The sign of the result may need to be corrected
                ** later.
                */
                g_defcodelabel (DoShiftLabel);
                g_asr (flags, p2);
                g_defcodelabel (EndLabel);
            }

            /* Negate the result as long as val < 0, even if val == -1 and no
            ** shift was generated.
            */
            if (Negation) {
                g_neg (flags);
            }

            /* Done */
            return;
        }

        /* If we go here, we didn't emit code. Push the lhs on stack and fall
        ** into the normal, non-optimized stuff.
        */
        flags &= ~CF_FORCECHAR; /* Handle chars as ints */
        g_push (flags & ~CF_CONST, 0);
    }

    /* Generate a division */
    oper (flags, val, ops);

}



void g_mod (unsigned flags, unsigned long val)
/* Primary = TOS % Primary */
{
    static const char* const ops[4] = {
        "tosmodax", "tosumodax", "tosmodeax", "tosumodeax"
    };
    int p2;

    /* Check if we can do some cost reduction */
    if ((flags & CF_CONST) && (flags & CF_UNSIGNED) && val != 0xFFFFFFFF && (p2 = PowerOf2 (val)) >= 0) {
        /* We can do that with an AND operation */
        g_and (flags, val - 1);
    } else {
        /* Do it the hard way... */
        if (flags & CF_CONST) {
            /* lhs is not on stack */
            flags &= ~CF_FORCECHAR;     /* Handle chars as ints */
            g_push (flags & ~CF_CONST, 0);
        }
        oper (flags, val, ops);
    }
}



void g_or (unsigned flags, unsigned long val)
/* Primary = TOS | Primary */
{
    static const char* const ops[4] = {
        "tosorax", "tosorax", "tosoreax", "tosoreax"
    };

    /* If the right hand side is const, the lhs is not on stack but still
    ** in the primary register.
    */
    if (flags & CF_CONST) {

        switch (flags & CF_TYPEMASK) {

            case CF_CHAR:
                if (flags & CF_FORCECHAR) {
                    if ((val & 0xFF) != 0) {
                        AddCodeLine ("ora #$%02X", (unsigned char)val);
                    }
                    return;
                }
                /* FALLTHROUGH */

            case CF_INT:
                if (val <= 0xFF) {
                    if ((val & 0xFF) != 0) {
                        AddCodeLine ("ora #$%02X", (unsigned char)val);
                    }
                } else if ((val & 0xFF00) == 0xFF00) {
                    if ((val & 0xFF) != 0) {
                        AddCodeLine ("ora #$%02X", (unsigned char)val);
                    }
                    AddCodeLine ("ldx #$FF");
                } else if (val != 0) {
                    AddCodeLine ("ora #$%02X", (unsigned char)val);
                    AddCodeLine ("pha");
                    AddCodeLine ("txa");
                    AddCodeLine ("ora #$%02X", (unsigned char)(val >> 8));
                    AddCodeLine ("tax");
                    AddCodeLine ("pla");
                }
                return;

            case CF_LONG:
                if (val <= 0xFF) {
                    if ((val & 0xFF) != 0) {
                        AddCodeLine ("ora #$%02X", (unsigned char)val);
                    }
                    return;
                }
                break;

            default:
                typeerror (flags);
        }

        /* If we go here, we didn't emit code. Push the lhs on stack and fall
        ** into the normal, non-optimized stuff. Note: The standard stuff will
        ** always work with ints.
        */
        flags &= ~CF_FORCECHAR;
        g_push (flags & ~CF_CONST, 0);
    }

    /* Use long way over the stack */
    oper (flags, val, ops);
}



void g_xor (unsigned flags, unsigned long val)
/* Primary = TOS ^ Primary */
{
    static const char* const ops[4] = {
        "tosxorax", "tosxorax", "tosxoreax", "tosxoreax"
    };


    /* If the right hand side is const, the lhs is not on stack but still
    ** in the primary register.
    */
    if (flags & CF_CONST) {

        switch (flags & CF_TYPEMASK) {

            case CF_CHAR:
                if (flags & CF_FORCECHAR) {
                    if ((val & 0xFF) != 0) {
                        AddCodeLine ("eor #$%02X", (unsigned char)val);
                    }
                    return;
                }
                /* FALLTHROUGH */

            case CF_INT:
                if (val <= 0xFF) {
                    if (val != 0) {
                        AddCodeLine ("eor #$%02X", (unsigned char)val);
                    }
                } else if (val != 0) {
                    if ((val & 0xFF) != 0) {
                        AddCodeLine ("eor #$%02X", (unsigned char)val);
                    }
                    AddCodeLine ("pha");
                    AddCodeLine ("txa");
                    AddCodeLine ("eor #$%02X", (unsigned char)(val >> 8));
                    AddCodeLine ("tax");
                    AddCodeLine ("pla");
                }
                return;

            case CF_LONG:
                if (val <= 0xFF) {
                    if (val != 0) {
                        AddCodeLine ("eor #$%02X", (unsigned char)val);
                    }
                    return;
                }
                break;

            default:
                typeerror (flags);
        }

        /* If we go here, we didn't emit code. Push the lhs on stack and fall
        ** into the normal, non-optimized stuff. Note: The standard stuff will
        ** always work with ints.
        */
        flags &= ~CF_FORCECHAR;
        g_push (flags & ~CF_CONST, 0);
    }

    /* Use long way over the stack */
    oper (flags, val, ops);
}



void g_and (unsigned Flags, unsigned long Val)
/* Primary = TOS & Primary */
{
    static const char* const ops[4] = {
        "tosandax", "tosandax", "tosandeax", "tosandeax"
    };

    /* If the right hand side is const, the lhs is not on stack but still
    ** in the primary register.
    */
    if (Flags & CF_CONST) {

        switch (Flags & CF_TYPEMASK) {

            case CF_CHAR:
                if (Flags & CF_FORCECHAR) {
                    if ((Val & 0xFF) == 0x00) {
                        AddCodeLine ("lda #$00");
                    } else if ((Val & 0xFF) != 0xFF) {
                        AddCodeLine ("and #$%02X", (unsigned char)Val);
                    }
                    return;
                }
                /* FALLTHROUGH */
            case CF_INT:
                if ((Val & 0xFFFF) != 0xFFFF) {
                    if (Val <= 0xFF) {
                        AddCodeLine ("ldx #$00");
                        if (Val == 0) {
                            AddCodeLine ("lda #$00");
                        } else if (Val != 0xFF) {
                            AddCodeLine ("and #$%02X", (unsigned char)Val);
                        }
                    } else if ((Val & 0xFFFF) == 0xFF00) {
                        AddCodeLine ("lda #$00");
                    } else if ((Val & 0xFF00) == 0xFF00) {
                        AddCodeLine ("and #$%02X", (unsigned char)Val);
                    } else if ((Val & 0x00FF) == 0x0000) {
                        AddCodeLine ("txa");
                        AddCodeLine ("and #$%02X", (unsigned char)(Val >> 8));
                        AddCodeLine ("tax");
                        AddCodeLine ("lda #$00");
                    } else {
                        AddCodeLine ("tay");
                        AddCodeLine ("txa");
                        AddCodeLine ("and #$%02X", (unsigned char)(Val >> 8));
                        AddCodeLine ("tax");
                        AddCodeLine ("tya");
                        if ((Val & 0x00FF) == 0x0000) {
                            AddCodeLine ("lda #$00");
                        } else if ((Val & 0x00FF) != 0x00FF) {
                            AddCodeLine ("and #$%02X", (unsigned char)Val);
                        }
                    }
                }
                return;

            case CF_LONG:
                if (Val <= 0xFF) {
                    AddCodeLine ("ldx #$00");
                    AddCodeLine ("stx sreg+1");
                    AddCodeLine ("stx sreg");
                    if ((Val & 0xFF) != 0xFF) {
                         AddCodeLine ("and #$%02X", (unsigned char)Val);
                    }
                    return;
                } else if (Val == 0xFF00) {
                    AddCodeLine ("lda #$00");
                    AddCodeLine ("sta sreg+1");
                    AddCodeLine ("sta sreg");
                    return;
                }
                break;

            default:
                typeerror (Flags);
        }

        /* If we go here, we didn't emit code. Push the lhs on stack and fall
        ** into the normal, non-optimized stuff. Note: The standard stuff will
        ** always work with ints.
        */
        Flags &= ~CF_FORCECHAR;
        g_push (Flags & ~CF_CONST, 0);
    }

    /* Use long way over the stack */
    oper (Flags, Val, ops);
}



void g_asr (unsigned flags, unsigned long val)
/* Primary = TOS >> Primary */
{
    static const char* const ops[4] = {
        "tosasrax", "tosshrax", "tosasreax", "tosshreax"
    };

    /* If the right hand side is const, the lhs is not on stack, but still
    ** in the primary register.
    */
    if (flags & CF_CONST) {
        switch (flags & CF_TYPEMASK) {
            case CF_CHAR:
                if (flags & CF_FORCECHAR) {
                    val &= 7;
                    if ((flags & CF_UNSIGNED) != 0) {
                        /* Instead of `val` right shifts, we can also do `9 - val` left rotates
                        ** and a mask.  This saves 3 bytes and 8 cycles for `val == 7` and
                        ** 1 byte and 4 cycles for `val == 6`.
                        */
                        if (val < 6) {
                            while (val--) {
                                AddCodeLine ("lsr a");  /* 1 byte, 2 cycles */
                            }
                        } else {
                            unsigned i;
                            /* The first ROL shifts in garbage and sets carry to the high bit.
                            ** The garbage is cleaned up by the mask.
                            */
                            for (i = val; i < 9; ++i) {
                                AddCodeLine ("rol a");  /* 1 byte,  2 cycles */
                            }
                            /* 2 bytes, 2 cycles */
                            AddCodeLine ("and #$%02X", 0xFF >> val);
                        }
                        return;
                    } else if (val <= 2) {
                        while (val--) {
                            AddCodeLine ("cmp #$80");
                            AddCodeLine ("ror a");
                        }
                        return;
                    }
                    AddCodeLine ("ldx #$00");
                    if ((flags & CF_UNSIGNED) == 0) {
                        unsigned L = GetLocalLabel ();

                        AddCodeLine ("cmp #$80");   /* Sign bit into carry */
                        AddCodeLine ("bcc %s", LocalLabelName (L));
                        AddCodeLine ("dex");        /* Make $FF */
                        g_defcodelabel (L);
                    }
                }
                /* FALLTHROUGH */

            case CF_INT:
                val &= 0x0F;
                if (val >= 8) {
                    AddCodeLine ("txa");
                    if (flags & CF_UNSIGNED) {
                        AddCodeLine ("ldx #$00");
                    } else {
                        unsigned L = GetLocalLabel ();

                        AddCodeLine ("cpx #$80");   /* Sign bit into carry */
                        AddCodeLine ("ldx #$00");
                        AddCodeLine ("bcc %s", LocalLabelName (L));
                        AddCodeLine ("dex");        /* Make $FF */
                        g_defcodelabel (L);
                    }
                    val -= 8;
                }
                if (val == 7) {
                    if (flags & CF_UNSIGNED) {
                        AddCodeLine ("jsr shrax7");
                    } else {
                        AddCodeLine ("jsr asrax7");
                    }
                    val = 0;
                }
                if (val >= 4) {
                    if (flags & CF_UNSIGNED) {
                        AddCodeLine ("jsr shrax4");
                    } else {
                        AddCodeLine ("jsr asrax4");
                    }
                    val -= 4;
                }
                if (val > 0) {
                    if (flags & CF_UNSIGNED) {
                        AddCodeLine ("jsr shrax%lu", val);
                    } else {
                        AddCodeLine ("jsr asrax%lu", val);
                    }
                }
                return;

            case CF_LONG:
                val &= 0x1F;
                if (val >= 24) {
                    AddCodeLine ("ldx #$00");
                    AddCodeLine ("lda sreg+1");
                    if ((flags & CF_UNSIGNED) == 0) {
                        unsigned L = GetLocalLabel ();

                        AddCodeLine ("bpl %s", LocalLabelName (L));
                        AddCodeLine ("dex");
                        g_defcodelabel (L);
                    }
                    AddCodeLine ("stx sreg");
                    AddCodeLine ("stx sreg+1");
                    val -= 24;
                }
                if (val >= 16) {
                    AddCodeLine ("ldy #$00");
                    AddCodeLine ("ldx sreg+1");
                    if ((flags & CF_UNSIGNED) == 0) {
                        unsigned L = GetLocalLabel ();

                        AddCodeLine ("bpl %s", LocalLabelName (L));
                        AddCodeLine ("dey");
                        g_defcodelabel (L);
                    }
                    AddCodeLine ("lda sreg");
                    AddCodeLine ("sty sreg+1");
                    AddCodeLine ("sty sreg");
                    val -= 16;
                }
                if (val >= 8) {
                    AddCodeLine ("txa");
                    AddCodeLine ("ldx sreg");
                    AddCodeLine ("ldy sreg+1");
                    AddCodeLine ("sty sreg");
                    if ((flags & CF_UNSIGNED) == 0) {
                        unsigned L = GetLocalLabel ();

                        AddCodeLine ("cpy #$80");
                        AddCodeLine ("ldy #$00");
                        AddCodeLine ("bcc %s", LocalLabelName (L));
                        AddCodeLine ("dey");
                        g_defcodelabel (L);
                    } else {
                        AddCodeLine ("ldy #$00");
                    }
                    AddCodeLine ("sty sreg+1");
                    val -= 8;
                }
                if (val >= 4) {
                    if (flags & CF_UNSIGNED) {
                        AddCodeLine ("jsr shreax4");
                    } else {
                        AddCodeLine ("jsr asreax4");
                    }
                    val -= 4;
                }
                if (val > 0) {
                    if (flags & CF_UNSIGNED) {
                        AddCodeLine ("jsr shreax%lu", val);
                    } else {
                        AddCodeLine ("jsr asreax%lu", val);
                    }
                }
                return;

            default:
                typeerror (flags);
        }

        /* If we go here, we didn't emit code. Push the lhs on stack and fall
        ** into the normal, non-optimized stuff. Note: The standard stuff will
        ** always work with ints.
        */
        flags &= ~CF_FORCECHAR;
        g_push (flags & ~CF_CONST, 0);
    }

    /* Use long way over the stack */
    oper (flags, val, ops);
}



void g_asl (unsigned flags, unsigned long val)
/* Primary = TOS << Primary */
{
    static const char* const ops[4] = {
        "tosaslax", "tosshlax", "tosasleax", "tosshleax"
    };

    /* If the right hand side is const, the lhs is not on stack, but still
    ** in the primary register.
    */
    if (flags & CF_CONST) {
        switch (flags & CF_TYPEMASK) {
            case CF_CHAR:
                if ((flags & CF_FORCECHAR) != 0) {
                    val &= 7;
                    /* Large shifts are faster and smaller with ROR.  See g_asr for detailed
                    ** byte and cycle counts.
                    */
                    if (val < 6) {
                        while (val--) {
                            AddCodeLine ("asl a");
                        }
                    } else {
                        unsigned i;
                        for (i = val; i < 9; ++i) {
                            AddCodeLine ("ror a");
                        }
                        AddCodeLine ("and #$%02X", (~0U << val) & 0xFF);
                    }
                    return;
                }
                /* FALLTHROUGH */

            case CF_INT:
                val &= 0x0F;
                if (val >= 8) {
                    AddCodeLine ("tax");
                    AddCodeLine ("lda #$00");
                    val -= 8;
                }
                if (val == 7) {
                    if (flags & CF_UNSIGNED) {
                        AddCodeLine ("jsr shlax7");
                    } else {
                        AddCodeLine ("jsr aslax7");
                    }
                    val = 0;
                }
                if (val >= 4) {
                    if (flags & CF_UNSIGNED) {
                        AddCodeLine ("jsr shlax4");
                    } else {
                        AddCodeLine ("jsr aslax4");
                    }
                    val -= 4;
                }
                if (val > 0) {
                    if (flags & CF_UNSIGNED) {
                        AddCodeLine ("jsr shlax%lu", val);
                    } else {
                        AddCodeLine ("jsr aslax%lu", val);
                    }
                }
                return;

            case CF_LONG:
                val &= 0x1F;
                if (val >= 24) {
                    AddCodeLine ("sta sreg+1");
                    AddCodeLine ("lda #$00");
                    AddCodeLine ("tax");
                    AddCodeLine ("sta sreg");
                    val -= 24;
                }
                if (val >= 16) {
                    AddCodeLine ("stx sreg+1");
                    AddCodeLine ("sta sreg");
                    AddCodeLine ("lda #$00");
                    AddCodeLine ("tax");
                    val -= 16;
                }
                if (val >= 8) {
                    AddCodeLine ("ldy sreg");
                    AddCodeLine ("sty sreg+1");
                    AddCodeLine ("stx sreg");
                    AddCodeLine ("tax");
                    AddCodeLine ("lda #$00");
                    val -= 8;
                }
                if (val > 4) {
                    if (flags & CF_UNSIGNED) {
                        AddCodeLine ("jsr shleax4");
                    } else {
                        AddCodeLine ("jsr asleax4");
                    }
                    val -= 4;
                }
                if (val > 0) {
                    if (flags & CF_UNSIGNED) {
                        AddCodeLine ("jsr shleax%lu", val);
                    } else {
                        AddCodeLine ("jsr asleax%lu", val);
                    }
                }
                return;

            default:
                typeerror (flags);
        }

        /* If we go here, we didn't emit code. Push the lhs on stack and fall
        ** into the normal, non-optimized stuff. Note: The standard stuff will
        ** always work with ints.
        */
        flags &= ~CF_FORCECHAR;
        g_push (flags & ~CF_CONST, 0);
    }

    /* Use long way over the stack */
    oper (flags, val, ops);
}



void g_neg (unsigned Flags)
/* Primary = -Primary */
{
    switch (Flags & CF_TYPEMASK) {

        case CF_CHAR:
            if (Flags & CF_FORCECHAR) {
                AddCodeLine ("eor #$FF");
                AddCodeLine ("clc");
                AddCodeLine ("adc #$01");
                return;
            }
            /* FALLTHROUGH */

        case CF_INT:
            AddCodeLine ("jsr negax");
            break;

        case CF_LONG:
            AddCodeLine ("jsr negeax");
            break;

        default:
            typeerror (Flags);
    }
}



void g_bneg (unsigned flags)
/* Primary = !Primary */
{
    switch (flags & CF_TYPEMASK) {

        case CF_CHAR:
            AddCodeLine ("jsr bnega");
            break;

        case CF_INT:
            AddCodeLine ("jsr bnegax");
            break;

        case CF_LONG:
            AddCodeLine ("jsr bnegeax");
            break;

        default:
            typeerror (flags);
    }
}



void g_com (unsigned Flags)
/* Primary = ~Primary */
{
    switch (Flags & CF_TYPEMASK) {

        case CF_CHAR:
            if (Flags & CF_FORCECHAR) {
                AddCodeLine ("eor #$FF");
                return;
            }
            /* FALLTHROUGH */

        case CF_INT:
            AddCodeLine ("jsr complax");
            break;

        case CF_LONG:
            AddCodeLine ("jsr compleax");
            break;

        default:
            typeerror (Flags);
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
    switch (flags & CF_TYPEMASK) {

        case CF_CHAR:
            if (flags & CF_FORCECHAR) {
                if ((CPUIsets[CPU] & CPU_ISET_65SC02) != 0 && val <= 2) {
                    while (val--) {
                        AddCodeLine ("ina");
                    }
                } else {
                    AddCodeLine ("clc");
                    AddCodeLine ("adc #$%02X", (unsigned char)val);
                }
                break;
            }
            /* FALLTHROUGH */

        case CF_INT:
            if ((CPUIsets[CPU] & CPU_ISET_65SC02) != 0 && val == 1) {
                unsigned L = GetLocalLabel();
                AddCodeLine ("ina");
                AddCodeLine ("bne %s", LocalLabelName (L));
                AddCodeLine ("inx");
                g_defcodelabel (L);
            } else if (IS_Get (&CodeSizeFactor) < 200) {
                /* Use jsr calls */
                if (val <= 8) {
                    AddCodeLine ("jsr incax%lu", val);
                } else if (val <= 255) {
                    AddCodeLine ("ldy #$%02X", (unsigned char) val);
                    AddCodeLine ("jsr incaxy");
                } else {
                    g_add (flags | CF_CONST, val);
                }
            } else {
                /* Inline the code */
                if (val <= 0x300) {
                    if ((val & 0xFF) != 0) {
                        unsigned L = GetLocalLabel();
                        AddCodeLine ("clc");
                        AddCodeLine ("adc #$%02X", (unsigned char) val);
                        AddCodeLine ("bcc %s", LocalLabelName (L));
                        AddCodeLine ("inx");
                        g_defcodelabel (L);
                    }
                    if (val >= 0x100) {
                        AddCodeLine ("inx");
                    }
                    if (val >= 0x200) {
                        AddCodeLine ("inx");
                    }
                    if (val >= 0x300) {
                        AddCodeLine ("inx");
                    }
                } else if ((val & 0xFF) != 0) {
                    AddCodeLine ("clc");
                    AddCodeLine ("adc #$%02X", (unsigned char) val);
                    AddCodeLine ("pha");
                    AddCodeLine ("txa");
                    AddCodeLine ("adc #$%02X", (unsigned char) (val >> 8));
                    AddCodeLine ("tax");
                    AddCodeLine ("pla");
                } else {
                    AddCodeLine ("pha");
                    AddCodeLine ("txa");
                    AddCodeLine ("clc");
                    AddCodeLine ("adc #$%02X", (unsigned char) (val >> 8));
                    AddCodeLine ("tax");
                    AddCodeLine ("pla");
                }
            }
            break;

        case CF_LONG:
            if (val <= 255) {
                AddCodeLine ("ldy #$%02X", (unsigned char) val);
                AddCodeLine ("jsr inceaxy");
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
    switch (flags & CF_TYPEMASK) {

        case CF_CHAR:
            if (flags & CF_FORCECHAR) {
                if ((CPUIsets[CPU] & CPU_ISET_65SC02) != 0 && val <= 2) {
                    while (val--) {
                        AddCodeLine ("dea");
                    }
                } else {
                    AddCodeLine ("sec");
                    AddCodeLine ("sbc #$%02X", (unsigned char)val);
                }
                break;
            }
            /* FALLTHROUGH */

        case CF_INT:
            if (IS_Get (&CodeSizeFactor) < 200) {
                /* Use subroutines */
                if (val <= 8) {
                    AddCodeLine ("jsr decax%d", (int) val);
                } else if (val <= 255) {
                    AddCodeLine ("ldy #$%02X", (unsigned char) val);
                    AddCodeLine ("jsr decaxy");
                } else {
                    g_sub (flags | CF_CONST, val);
                }
            } else {
                /* Inline the code */
                if (val < 0x300) {
                    if ((val & 0xFF) != 0) {
                        unsigned L = GetLocalLabel();
                        AddCodeLine ("sec");
                        AddCodeLine ("sbc #$%02X", (unsigned char) val);
                        AddCodeLine ("bcs %s", LocalLabelName (L));
                        AddCodeLine ("dex");
                        g_defcodelabel (L);
                    }
                    if (val >= 0x100) {
                        AddCodeLine ("dex");
                    }
                    if (val >= 0x200) {
                        AddCodeLine ("dex");
                    }
                } else {
                    if ((val & 0xFF) != 0) {
                        AddCodeLine ("sec");
                        AddCodeLine ("sbc #$%02X", (unsigned char) val);
                        AddCodeLine ("pha");
                        AddCodeLine ("txa");
                        AddCodeLine ("sbc #$%02X", (unsigned char) (val >> 8));
                        AddCodeLine ("tax");
                        AddCodeLine ("pla");
                    } else {
                        AddCodeLine ("pha");
                        AddCodeLine ("txa");
                        AddCodeLine ("sec");
                        AddCodeLine ("sbc #$%02X", (unsigned char) (val >> 8));
                        AddCodeLine ("tax");
                        AddCodeLine ("pla");
                    }
                }
            }
            break;

        case CF_LONG:
            if (val <= 255) {
                AddCodeLine ("ldy #$%02X", (unsigned char) val);
                AddCodeLine ("jsr deceaxy");
            } else {
                g_sub (flags | CF_CONST, val);
            }
            break;

        default:
            typeerror (flags);

    }
}



/*
** Following are the conditional operators. They compare the TOS against
** the primary and put a literal 1 in the primary if the condition is
** true, otherwise they clear the primary register
*/



void g_eq (unsigned flags, unsigned long val)
/* Test for equal */
{
    static const char* const ops[4] = {
        "toseqax", "toseqax", "toseqeax", "toseqeax"
    };

    unsigned L;

    /* If the right hand side is const, the lhs is not on stack but still
    ** in the primary register.
    */
    if (flags & CF_CONST) {

        switch (flags & CF_TYPEMASK) {

            case CF_CHAR:
                if (flags & CF_FORCECHAR) {
                    AddCodeLine ("cmp #$%02X", (unsigned char)val);
                    AddCodeLine ("jsr booleq");
                    return;
                }
                /* FALLTHROUGH */

            case CF_INT:
                L = GetLocalLabel();
                AddCodeLine ("cpx #$%02X", (unsigned char)(val >> 8));
                AddCodeLine ("bne %s", LocalLabelName (L));
                AddCodeLine ("cmp #$%02X", (unsigned char)val);
                g_defcodelabel (L);
                AddCodeLine ("jsr booleq");
                return;

            case CF_LONG:
                break;

            default:
                typeerror (flags);
        }

        /* If we go here, we didn't emit code. Push the lhs on stack and fall
        ** into the normal, non-optimized stuff. Note: The standard stuff will
        ** always work with ints.
        */
        flags &= ~CF_FORCECHAR;
        g_push (flags & ~CF_CONST, 0);
    }

    /* Use long way over the stack */
    oper (flags, val, ops);
}



void g_ne (unsigned flags, unsigned long val)
/* Test for not equal */
{
    static const char* const ops[4] = {
        "tosneax", "tosneax", "tosneeax", "tosneeax"
    };

    unsigned L;

    /* If the right hand side is const, the lhs is not on stack but still
    ** in the primary register.
    */
    if (flags & CF_CONST) {

        switch (flags & CF_TYPEMASK) {

            case CF_CHAR:
                if (flags & CF_FORCECHAR) {
                    AddCodeLine ("cmp #$%02X", (unsigned char)val);
                    AddCodeLine ("jsr boolne");
                    return;
                }
                /* FALLTHROUGH */

            case CF_INT:
                L = GetLocalLabel();
                AddCodeLine ("cpx #$%02X", (unsigned char)(val >> 8));
                AddCodeLine ("bne %s", LocalLabelName (L));
                AddCodeLine ("cmp #$%02X", (unsigned char)val);
                g_defcodelabel (L);
                AddCodeLine ("jsr boolne");
                return;

            case CF_LONG:
                break;

            default:
                typeerror (flags);
        }

        /* If we go here, we didn't emit code. Push the lhs on stack and fall
        ** into the normal, non-optimized stuff. Note: The standard stuff will
        ** always work with ints.
        */
        flags &= ~CF_FORCECHAR;
        g_push (flags & ~CF_CONST, 0);
    }

    /* Use long way over the stack */
    oper (flags, val, ops);
}



void g_lt (unsigned flags, unsigned long val)
/* Test for less than */
{
    static const char* const ops[4] = {
        "tosltax", "tosultax", "toslteax", "tosulteax"
    };

    unsigned Label;

    /* If the right hand side is const, the lhs is not on stack but still
    ** in the primary register.
    */
    if (flags & CF_CONST) {

        /* Because the handling of the overflow flag is too complex for
        ** inlining, we can handle only unsigned compares, and signed
        ** compares against zero here.
        */
        if (flags & CF_UNSIGNED) {

            /* Give a warning in some special cases */
            if (val == 0) {
                Warning ("Comparison of unsigned type < 0 is always false");
                AddCodeLine ("jsr return0");
                return;
            }

            /* Look at the type */
            switch (flags & CF_TYPEMASK) {

                case CF_CHAR:
                    if (flags & CF_FORCECHAR) {
                        AddCodeLine ("cmp #$%02X", (unsigned char)val);
                        AddCodeLine ("jsr boolult");
                        return;
                    }
                    /* FALLTHROUGH */

                case CF_INT:
                    /* If the low byte is zero, we must only test the high byte */
                    AddCodeLine ("cpx #$%02X", (unsigned char)(val >> 8));
                    if ((val & 0xFF) != 0) {
                        unsigned L = GetLocalLabel();
                        AddCodeLine ("bne %s", LocalLabelName (L));
                        AddCodeLine ("cmp #$%02X", (unsigned char)val);
                        g_defcodelabel (L);
                    }
                    AddCodeLine ("jsr boolult");
                    return;

                case CF_LONG:
                    /* Do a subtraction */
                    AddCodeLine ("cmp #$%02X", (unsigned char)val);
                    AddCodeLine ("txa");
                    AddCodeLine ("sbc #$%02X", (unsigned char)(val >> 8));
                    AddCodeLine ("lda sreg");
                    AddCodeLine ("sbc #$%02X", (unsigned char)(val >> 16));
                    AddCodeLine ("lda sreg+1");
                    AddCodeLine ("sbc #$%02X", (unsigned char)(val >> 24));
                    AddCodeLine ("jsr boolult");
                    return;

                default:
                    typeerror (flags);
            }

        } else if (val == 0) {

            /* A signed compare against zero must only look at the sign bit */
            switch (flags & CF_TYPEMASK) {

                case CF_CHAR:
                    if (flags & CF_FORCECHAR) {
                        AddCodeLine ("asl a");          /* Bit 7 -> carry */
                        AddCodeLine ("lda #$00");
                        AddCodeLine ("ldx #$00");
                        AddCodeLine ("rol a");
                        return;
                    }
                    /* FALLTHROUGH */

                case CF_INT:
                    /* Just check the high byte */
                    AddCodeLine ("cpx #$80");           /* Bit 7 -> carry */
                    AddCodeLine ("lda #$00");
                    AddCodeLine ("ldx #$00");
                    AddCodeLine ("rol a");
                    return;

                case CF_LONG:
                    /* Just check the high byte */
                    AddCodeLine ("lda sreg+1");
                    AddCodeLine ("asl a");              /* Bit 7 -> carry */
                    AddCodeLine ("lda #$00");
                    AddCodeLine ("ldx #$00");
                    AddCodeLine ("rol a");
                    return;

                default:
                    typeerror (flags);
            }

        } else {

            /* Signed compare against a constant != zero */
            switch (flags & CF_TYPEMASK) {

                case CF_CHAR:
                    if (flags & CF_FORCECHAR) {
                        Label = GetLocalLabel ();
                        AddCodeLine ("sec");
                        AddCodeLine ("sbc #$%02X", (unsigned char)val);
                        AddCodeLine ("bvc %s", LocalLabelName (Label));
                        AddCodeLine ("eor #$80");
                        g_defcodelabel (Label);
                        AddCodeLine ("asl a");          /* Bit 7 -> carry */
                        AddCodeLine ("lda #$00");
                        AddCodeLine ("ldx #$00");
                        AddCodeLine ("rol a");
                        return;
                    }
                    /* FALLTHROUGH */

                case CF_INT:
                    /* Do a subtraction */
                    Label = GetLocalLabel ();
                    AddCodeLine ("cmp #$%02X", (unsigned char)val);
                    AddCodeLine ("txa");
                    AddCodeLine ("sbc #$%02X", (unsigned char)(val >> 8));
                    AddCodeLine ("bvc %s", LocalLabelName (Label));
                    AddCodeLine ("eor #$80");
                    g_defcodelabel (Label);
                    AddCodeLine ("asl a");          /* Bit 7 -> carry */
                    AddCodeLine ("lda #$00");
                    AddCodeLine ("ldx #$00");
                    AddCodeLine ("rol a");
                    return;

                case CF_LONG:
                    /* This one is too costly */
                    break;

                default:
                    typeerror (flags);
            }

        }

        /* If we go here, we didn't emit code. Push the lhs on stack and fall
        ** into the normal, non-optimized stuff. Note: The standard stuff will
        ** always work with ints.
        */
        flags &= ~CF_FORCECHAR;
        g_push (flags & ~CF_CONST, 0);
    }

    /* Use long way over the stack */
    oper (flags, val, ops);
}



void g_le (unsigned flags, unsigned long val)
/* Test for less than or equal to */
{
    static const char* const ops[4] = {
        "tosleax", "tosuleax", "tosleeax", "tosuleeax"
    };


    /* If the right hand side is const, the lhs is not on stack but still
    ** in the primary register.
    */
    if (flags & CF_CONST) {

        /* Look at the type */
        switch (flags & CF_TYPEMASK) {

            case CF_CHAR:
                if (flags & CF_FORCECHAR) {
                    if (flags & CF_UNSIGNED) {
                        /* Unsigned compare */
                        if (val < 0xFF) {
                            /* Use < instead of <= because the former gives
                            ** better code on the 6502 than the latter.
                            */
                            g_lt (flags, val+1);
                        } else {
                            /* Always true */
                            Warning ("Condition is always true");
                            AddCodeLine ("jsr return1");
                        }
                    } else {
                        /* Signed compare */
                        if ((long) val < 0x7F) {
                            /* Use < instead of <= because the former gives
                            ** better code on the 6502 than the latter.
                            */
                            g_lt (flags, val+1);
                        } else {
                            /* Always true */
                            Warning ("Condition is always true");
                            AddCodeLine ("jsr return1");
                        }
                    }
                    return;
                }
                /* FALLTHROUGH */

            case CF_INT:
                if (flags & CF_UNSIGNED) {
                    /* Unsigned compare */
                    if (val < 0xFFFF) {
                        /* Use < instead of <= because the former gives
                        ** better code on the 6502 than the latter.
                        */
                        g_lt (flags, val+1);
                    } else {
                        /* Always true */
                        Warning ("Condition is always true");
                        AddCodeLine ("jsr return1");
                    }
                } else {
                    /* Signed compare */
                    if ((long) val < 0x7FFF) {
                        g_lt (flags, val+1);
                    } else {
                        /* Always true */
                        Warning ("Condition is always true");
                        AddCodeLine ("jsr return1");
                    }
                }
                return;

            case CF_LONG:
                if (flags & CF_UNSIGNED) {
                    /* Unsigned compare */
                    if (val < 0xFFFFFFFF) {
                        /* Use < instead of <= because the former gives
                        ** better code on the 6502 than the latter.
                        */
                        g_lt (flags, val+1);
                    } else {
                        /* Always true */
                        Warning ("Condition is always true");
                        AddCodeLine ("jsr return1");
                    }
                } else {
                    /* Signed compare */
                    if ((long) val < 0x7FFFFFFF) {
                        g_lt (flags, val+1);
                    } else {
                        /* Always true */
                        Warning ("Condition is always true");
                        AddCodeLine ("jsr return1");
                    }
                }
                return;

            default:
                typeerror (flags);
        }

        /* If we go here, we didn't emit code. Push the lhs on stack and fall
        ** into the normal, non-optimized stuff. Note: The standard stuff will
        ** always work with ints.
        */
        flags &= ~CF_FORCECHAR;
        g_push (flags & ~CF_CONST, 0);
    }

    /* Use long way over the stack */
    oper (flags, val, ops);
}



void g_gt (unsigned flags, unsigned long val)
/* Test for greater than */
{
    static const char* const ops[4] = {
        "tosgtax", "tosugtax", "tosgteax", "tosugteax"
    };


    /* If the right hand side is const, the lhs is not on stack but still
    ** in the primary register.
    */
    if (flags & CF_CONST) {

        /* Look at the type */
        switch (flags & CF_TYPEMASK) {

            case CF_CHAR:
                if (flags & CF_FORCECHAR) {
                    if (flags & CF_UNSIGNED) {
                        if (val == 0) {
                            /* If we have a compare > 0, we will replace it by
                            ** != 0 here, since both are identical but the
                            ** latter is easier to optimize.
                            */
                            g_ne (flags, val);
                        } else if (val < 0xFF) {
                            /* Use >= instead of > because the former gives
                            ** better code on the 6502 than the latter.
                            */
                            g_ge (flags, val+1);
                        } else {
                            /* Never true */
                            Warning ("Condition is never true");
                            AddCodeLine ("jsr return0");
                        }
                    } else {
                        if ((long) val < 0x7F) {
                            /* Use >= instead of > because the former gives
                            ** better code on the 6502 than the latter.
                            */
                            g_ge (flags, val+1);
                        } else {
                            /* Never true */
                            Warning ("Condition is never true");
                            AddCodeLine ("jsr return0");
                        }
                    }
                    return;
                }
                /* FALLTHROUGH */

            case CF_INT:
                if (flags & CF_UNSIGNED) {
                    /* Unsigned compare */
                    if (val == 0) {
                        /* If we have a compare > 0, we will replace it by
                        ** != 0 here, since both are identical but the latter
                        ** is easier to optimize.
                        */
                        g_ne (flags, val);
                    } else if (val < 0xFFFF) {
                        if (val == 0xFF) {
                            AddCodeLine ("cpx #$00");
                        } else {
                            /* Use >= instead of > because the former gives better
                            ** code on the 6502 than the latter.
                            */
                            g_ge (flags, val+1);
                        }
                    } else {
                        /* Never true */
                        Warning ("Condition is never true");
                        AddCodeLine ("jsr return0");
                    }
                } else {
                    /* Signed compare */
                    if ((long) val < 0x7FFF) {
                        g_ge (flags, val+1);
                    } else {
                        /* Never true */
                        Warning ("Condition is never true");
                        AddCodeLine ("jsr return0");
                    }
                }
                return;

            case CF_LONG:
                if (flags & CF_UNSIGNED) {
                    /* Unsigned compare */
                    if (val == 0) {
                        /* If we have a compare > 0, we will replace it by
                        ** != 0 here, since both are identical but the latter
                        ** is easier to optimize.
                        */
                        g_ne (flags, val);
                    } else if (val == 0xFF) {
                        AddCodeLine ("cpx #$00");
                    } else if (val < 0xFFFFFFFF) {
                        /* Use >= instead of > because the former gives better
                        ** code on the 6502 than the latter.
                        */
                        g_ge (flags, val+1);
                    } else {
                        /* Never true */
                        Warning ("Condition is never true");
                        AddCodeLine ("jsr return0");
                    }
                } else {
                    /* Signed compare */
                    if (val == 0xFF) {
                        AddCodeLine ("cpx #$00");
                    } else if ((long) val < 0x7FFFFFFF) {
                        g_ge (flags, val+1);
                    } else {
                        /* Never true */
                        Warning ("Condition is never true");
                        AddCodeLine ("jsr return0");
                    }
                }
                return;

            default:
                typeerror (flags);
        }

        /* If we go here, we didn't emit code. Push the lhs on stack and fall
        ** into the normal, non-optimized stuff. Note: The standard stuff will
        ** always work with ints.
        */
        flags &= ~CF_FORCECHAR;
        g_push (flags & ~CF_CONST, 0);
    }

    /* Use long way over the stack */
    oper (flags, val, ops);
}



void g_ge (unsigned flags, unsigned long val)
/* Test for greater than or equal to */
{
    static const char* const ops[4] = {
        "tosgeax", "tosugeax", "tosgeeax", "tosugeeax"
    };

    unsigned Label;


    /* If the right hand side is const, the lhs is not on stack but still
    ** in the primary register.
    */
    if (flags & CF_CONST) {

        /* Because the handling of the overflow flag is too complex for
        ** inlining, we can handle only unsigned compares, and signed
        ** compares against zero here.
        */
        if (flags & CF_UNSIGNED) {

            /* Give a warning in some special cases */
            if (val == 0) {
                Warning ("Condition is always true");
                AddCodeLine ("jsr return1");
                return;
            }

            /* Look at the type */
            switch (flags & CF_TYPEMASK) {

                case CF_CHAR:
                    if (flags & CF_FORCECHAR) {
                        /* Do a subtraction. Condition is true if carry set */
                        AddCodeLine ("cmp #$%02X", (unsigned char)val);
                        AddCodeLine ("lda #$00");
                        AddCodeLine ("ldx #$00");
                        AddCodeLine ("rol a");
                        return;
                    }
                    /* FALLTHROUGH */

                case CF_INT:
                    /* Do a subtraction. Condition is true if carry set */
                    AddCodeLine ("cmp #$%02X", (unsigned char)val);
                    AddCodeLine ("txa");
                    AddCodeLine ("sbc #$%02X", (unsigned char)(val >> 8));
                    AddCodeLine ("lda #$00");
                    AddCodeLine ("ldx #$00");
                    AddCodeLine ("rol a");
                    return;

                case CF_LONG:
                    /* Do a subtraction. Condition is true if carry set */
                    AddCodeLine ("cmp #$%02X", (unsigned char)val);
                    AddCodeLine ("txa");
                    AddCodeLine ("sbc #$%02X", (unsigned char)(val >> 8));
                    AddCodeLine ("lda sreg");
                    AddCodeLine ("sbc #$%02X", (unsigned char)(val >> 16));
                    AddCodeLine ("lda sreg+1");
                    AddCodeLine ("sbc #$%02X", (unsigned char)(val >> 24));
                    AddCodeLine ("lda #$00");
                    AddCodeLine ("ldx #$00");
                    AddCodeLine ("rol a");
                    return;

                default:
                    typeerror (flags);
            }

        } else if (val == 0) {

            /* A signed compare against zero must only look at the sign bit */
            switch (flags & CF_TYPEMASK) {

                case CF_CHAR:
                    if (flags & CF_FORCECHAR) {
                        AddCodeLine ("tax");
                        AddCodeLine ("jsr boolge");
                        return;
                    }
                    /* FALLTHROUGH */

                case CF_INT:
                    /* Just test the high byte */
                    AddCodeLine ("txa");
                    AddCodeLine ("jsr boolge");
                    return;

                case CF_LONG:
                    /* Just test the high byte */
                    AddCodeLine ("lda sreg+1");
                    AddCodeLine ("jsr boolge");
                    return;

                default:
                    typeerror (flags);
            }

        } else {

            /* Signed compare against a constant != zero */
            switch (flags & CF_TYPEMASK) {

                case CF_CHAR:
                    if (flags & CF_FORCECHAR) {
                        Label = GetLocalLabel ();
                        AddCodeLine ("sec");
                        AddCodeLine ("sbc #$%02X", (unsigned char)val);
                        AddCodeLine ("bvs %s", LocalLabelName (Label));
                        AddCodeLine ("eor #$80");
                        g_defcodelabel (Label);
                        AddCodeLine ("asl a");          /* Bit 7 -> carry */
                        AddCodeLine ("lda #$00");
                        AddCodeLine ("ldx #$00");
                        AddCodeLine ("rol a");
                        return;
                    }
                    /* FALLTHROUGH */

                case CF_INT:
                    /* Do a subtraction */
                    Label = GetLocalLabel ();
                    AddCodeLine ("cmp #$%02X", (unsigned char)val);
                    AddCodeLine ("txa");
                    AddCodeLine ("sbc #$%02X", (unsigned char)(val >> 8));
                    AddCodeLine ("bvs %s", LocalLabelName (Label));
                    AddCodeLine ("eor #$80");
                    g_defcodelabel (Label);
                    AddCodeLine ("asl a");          /* Bit 7 -> carry */
                    AddCodeLine ("lda #$00");
                    AddCodeLine ("ldx #$00");
                    AddCodeLine ("rol a");
                    return;

                case CF_LONG:
                    /* This one is too costly */
                    break;

                default:
                    typeerror (flags);
            }
        }

        /* If we go here, we didn't emit code. Push the lhs on stack and fall
        ** into the normal, non-optimized stuff. Note: The standard stuff will
        ** always work with ints.
        */
        flags &= ~CF_FORCECHAR;
        g_push (flags & ~CF_CONST, 0);
    }

    /* Use long way over the stack */
    oper (flags, val, ops);
}



/*****************************************************************************/
/*                         Allocating static storage                         */
/*****************************************************************************/



void g_res (unsigned n)
/* Reserve static storage, n bytes */
{
    AddDataLine ("\t.res\t%u,$00", n);
}



void g_defdata (unsigned flags, uintptr_t val, long offs)
/* Define data with the size given in flags */
{
    if (flags & CF_CONST) {

        /* Numeric constant */
        switch (flags & CF_TYPEMASK) {

            case CF_CHAR:
                AddDataLine ("\t.byte\t$%02"PRIXPTR, val & 0xFF);
                break;

            case CF_INT:
                AddDataLine ("\t.word\t$%04"PRIXPTR, val & 0xFFFF);
                break;

            case CF_LONG:
                AddDataLine ("\t.dword\t$%08"PRIXPTR, val & 0xFFFFFFFF);
                break;

            default:
                typeerror (flags);
                break;

        }

    } else {

        /* Create the correct label name */
        const char* Label = GetLabelName (flags, val, offs);

        /* Labels are always 16 bit */
        AddDataLine ("\t.addr\t%s", Label);

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
        AddDataLine ("%s", Buf);
    }
}



void g_zerobytes (unsigned Count)
/* Output Count bytes of data initialized with zero */
{
    if (Count > 0) {
        AddDataLine ("\t.res\t%u,$00", Count);
    }
}



void g_initregister (unsigned Label, unsigned Reg, unsigned Size)
/* Initialize a register variable from static initialization data */
{
    /* Register variables do always have less than 128 bytes */
    unsigned CodeLabel = GetLocalLabel ();
    AddCodeLine ("ldx #$%02X", (unsigned char) (Size - 1));
    g_defcodelabel (CodeLabel);
    AddCodeLine ("lda %s,x", GetLabelName (CF_STATIC, Label, 0));
    AddCodeLine ("sta %s,x", GetLabelName (CF_REGVAR, Reg, 0));
    AddCodeLine ("dex");
    AddCodeLine ("bpl %s", LocalLabelName (CodeLabel));
}



void g_initauto (unsigned Label, unsigned Size)
/* Initialize a local variable at stack offset zero from static data */
{
    unsigned CodeLabel = GetLocalLabel ();

    CheckLocalOffs (Size);
    if (Size <= 128) {
        AddCodeLine ("ldy #$%02X", Size-1);
        g_defcodelabel (CodeLabel);
        AddCodeLine ("lda %s,y", GetLabelName (CF_STATIC, Label, 0));
        AddCodeLine ("sta (sp),y");
        AddCodeLine ("dey");
        AddCodeLine ("bpl %s", LocalLabelName (CodeLabel));
    } else if (Size <= 256) {
        AddCodeLine ("ldy #$00");
        g_defcodelabel (CodeLabel);
        AddCodeLine ("lda %s,y", GetLabelName (CF_STATIC, Label, 0));
        AddCodeLine ("sta (sp),y");
        AddCodeLine ("iny");
        AddCmpCodeIfSizeNot256 ("cpy #$%02X", Size);
        AddCodeLine ("bne %s", LocalLabelName (CodeLabel));
    }
}



void g_initstatic (unsigned InitLabel, unsigned VarLabel, unsigned Size)
/* Initialize a static local variable from static initialization data */
{
    if (Size <= 128) {
        unsigned CodeLabel = GetLocalLabel ();
        AddCodeLine ("ldy #$%02X", Size-1);
        g_defcodelabel (CodeLabel);
        AddCodeLine ("lda %s,y", GetLabelName (CF_STATIC, InitLabel, 0));
        AddCodeLine ("sta %s,y", GetLabelName (CF_STATIC, VarLabel, 0));
        AddCodeLine ("dey");
        AddCodeLine ("bpl %s", LocalLabelName (CodeLabel));
    } else if (Size <= 256) {
        unsigned CodeLabel = GetLocalLabel ();
        AddCodeLine ("ldy #$00");
        g_defcodelabel (CodeLabel);
        AddCodeLine ("lda %s,y", GetLabelName (CF_STATIC, InitLabel, 0));
        AddCodeLine ("sta %s,y", GetLabelName (CF_STATIC, VarLabel, 0));
        AddCodeLine ("iny");
        AddCmpCodeIfSizeNot256 ("cpy #$%02X", Size);
        AddCodeLine ("bne %s", LocalLabelName (CodeLabel));
    } else {
        /* Use the easy way here: memcpy() */
        g_getimmed (CF_STATIC, VarLabel, 0);
        AddCodeLine ("jsr pushax");
        g_getimmed (CF_STATIC, InitLabel, 0);
        AddCodeLine ("jsr pushax");
        g_getimmed (CF_INT | CF_UNSIGNED | CF_CONST, Size, 0);
        AddCodeLine ("jsr %s", GetLabelName (CF_EXTERNAL, (uintptr_t) "memcpy", 0));
    }
}



/*****************************************************************************/
/*                                Bit-fields                                 */
/*****************************************************************************/



void g_testbitfield (ATTR_UNUSED(unsigned Flags), unsigned BitOffs, unsigned BitWidth)
/* Test bit-field in primary. */
{
    /* Since the end is inclusive and cannot be negative here, we subtract 1 from the sum */
    unsigned MSBit = BitOffs + BitWidth - 1U;
    unsigned Bytes = MSBit / CHAR_BITS + 1U - BitOffs / CHAR_BITS;
    unsigned HeadMask = (0xFF << (BitOffs % CHAR_BITS)) & 0xFF;
    unsigned TailMask = ((1U << (MSBit % CHAR_BITS + 1U)) - 1U) & 0xFF;
    unsigned UntestedBytes = ((1U << Bytes) - 1U) << (BitOffs / CHAR_BITS);

    /* We don't use these flags for now. Could CF_NOKEEP be potentially interesting? */
    Flags &= ~CF_STYPEMASK;

    /* If we need to do a test, then we avoid shifting (ASR only shifts one bit at a time,
    ** so is slow) and just AND the head and tail bytes with the appropriate mask, then
    ** OR the results with the rest bytes.
    */
    if (Bytes == 1) {
        HeadMask = TailMask = HeadMask & TailMask;
    }

    /* Get the head byte */
    switch (BitOffs / CHAR_BITS) {
    case 0:
        if (HeadMask == 0xFF && Bytes == 1) {
            AddCodeLine ("tax");
            UntestedBytes &= ~0x1;
        }
        break;
    case 1:
        if (HeadMask != 0xFF || TailMask == 0xFF) {
            AddCodeLine ("txa");
            UntestedBytes &= ~0x2;
        }
        break;
    case 2:
        if (HeadMask != 0xFF || TailMask == 0xFF) {
            AddCodeLine ("lda sreg");
            UntestedBytes &= ~0x4;
        }
        break;
    case 3:
        /* In this case we'd have HeadMask == TailMask and only 1 byte, but anyways... */
        if (HeadMask != 0xFF || TailMask == 0xFF) {
            AddCodeLine ("lda sreg+1");
            UntestedBytes &= ~0x8;
        }
        break;
    default:
        break;
    }

    /* Keep in mind that the head is NOT always "Byte 0" */
    if (HeadMask != 0xFF) {
        AddCodeLine ("and #$%02X", HeadMask);
        /* Abuse the "Byte 0" flag so that this head content will be saved by the routine */
        UntestedBytes |= 0x1;
    }

    /* If there is only 1 byte to test, we have done with it */
    if (Bytes == 1) {
        return;
    }

    /* Handle the tail byte */
    if (TailMask != 0xFF) {
        /* If we have to do any more masking operation, register A will be used for that,
        ** and its current content in it must be saved.
        */
        if (UntestedBytes & 0x1) {
            AddCodeLine ("sta tmp1");
        }

        /* Test the tail byte */
        switch (MSBit / CHAR_BITS) {
        case 1:
            AddCodeLine ("txa");
            UntestedBytes &= ~0x2;
            break;
        case 2:
            AddCodeLine ("lda sreg");
            UntestedBytes &= ~0x4;
            break;
        case 3:
            AddCodeLine ("lda sreg+1");
            UntestedBytes &= ~0x8;
            break;
        default:
            break;
        }
        AddCodeLine ("and #$%02X", TailMask);

        if (UntestedBytes & 0x1) {
            AddCodeLine ("ora tmp1");
        }
    }

    /* OR the rest bytes together, which could never need masking */
    if (UntestedBytes & 0x2) {
        AddCodeLine ("stx tmp1");
        AddCodeLine ("ora tmp1");
    }
    if (UntestedBytes & 0x4) {
        AddCodeLine ("ora sreg");
    }
    if (UntestedBytes & 0x8) {
        AddCodeLine ("ora sreg+1");
    }
}



void g_extractbitfield (unsigned Flags, unsigned FullWidthFlags, int IsSigned,
                        unsigned BitOffs, unsigned BitWidth)
/* Extract bits from bit-field in primary. */
{
    unsigned EndBit = BitOffs + BitWidth;
    unsigned long ZeroExtendMask = 0;  /* Zero if we don't need to zero-extend. */

    /* Shift right by the bit offset; no code is emitted if BitOffs is zero */
    g_asr (Flags | CF_CONST, BitOffs);

    /* To zero-extend, we will and by the width if the field doesn't end on a char or
    ** int boundary.  If it does end on a boundary, then zeros will have already been shifted in,
    ** but we need to clear the high byte for char.  g_and emits no code if the mask is all ones.
    ** This is here so the signed and unsigned branches can use it.
    */
    if (EndBit == CHAR_BITS) {
        /* We need to clear the high byte, since CF_FORCECHAR was set. */
        ZeroExtendMask = 0xFF;
    } else if (EndBit != INT_BITS && EndBit != LONG_BITS) {
        ZeroExtendMask = shl_l (1UL, BitWidth) - 1UL;
    }

    /* Handle signed bit-fields. */
    if (IsSigned) {
        unsigned SignBitPos = BitWidth - 1U;
        unsigned SignBitByte = SignBitPos / CHAR_BITS;
        unsigned SignBitPosInByte = SignBitPos % CHAR_BITS;

        if (ZeroExtendMask != 0) {
            /* The universal trick is:
            **   x = bits & bit_mask
            **   m = 1 << (bit_width - 1)
            **   r = (x ^ m) - m
            ** which works for long as well.
            */

            if (SignBitByte + 1U == sizeofarg (FullWidthFlags)) {
                /* We can just sign-extend on the high byte if it is the only affected one */
                unsigned char SignBitMask = (1UL << SignBitPosInByte) & 0xFF;
                unsigned char Mask = ((2UL << (SignBitPos % CHAR_BITS)) - 1UL) & 0xFF;

                /* Move the correct byte to .A */
                switch (SignBitByte) {
                  case 0:
                    break;
                  case 1:
                    AddCodeLine ("tay");
                    AddCodeLine ("txa");
                    break;
                  case 3:
                    AddCodeLine ("tay");
                    AddCodeLine ("lda sreg+1");
                    break;
                  default:
                    FAIL ("Invalid Byte for sign bit");
                }

                /* Use .A to do the ops on the correct byte */
                AddCodeLine ("and #$%02X", Mask);
                AddCodeLine ("eor #$%02X", SignBitMask);
                AddCodeLine ("sec");
                AddCodeLine ("sbc #$%02X", SignBitMask);

                /* Move the correct byte from .A */
                switch (SignBitByte) {
                  case 0:
                    break;
                  case 1:
                    AddCodeLine ("tax");
                    AddCodeLine ("tya");
                    break;
                  case 3:
                    AddCodeLine ("sta sreg+1");
                    AddCodeLine ("tya");
                    break;
                  default:
                    FAIL ("Invalid Byte for sign bit");
                }
            } else {
                unsigned long SignBitMask = 1UL << SignBitPos;
                unsigned long Mask = (2UL << SignBitPos) - 1UL;
                g_and (FullWidthFlags | CF_CONST, Mask);
                g_xor (FullWidthFlags | CF_CONST, SignBitMask);
                g_dec (FullWidthFlags | CF_CONST, SignBitMask);
            }
        } else {
            unsigned char SignBitMask = (1UL << SignBitPosInByte) & 0xFF;
            unsigned ZeroExtendLabel = GetLocalLabel ();

            /* Save .A because the sign-bit test will destroy it. */
            AddCodeLine ("tay");

            /* Move the correct byte to .A */
            switch (SignBitByte) {
              case 0:
                break;
              case 1:
                AddCodeLine ("txa");
                break;
              case 3:
                AddCodeLine ("lda sreg+1");
                break;
              default:
                FAIL ("Invalid Byte for sign bit");
            }

            /* Test the sign bit */
            AddCodeLine ("and #$%02X", SignBitMask);
            AddCodeLine ("beq %s", LocalLabelName (ZeroExtendLabel));

            if (SignBitByte + 1U == sizeofarg (FullWidthFlags)) {
                /* We can just sign-extend on the high byte if it is the only affected one */
                unsigned char Mask = ~((2UL << (SignBitPos % CHAR_BITS)) - 1UL) & 0xFF;

                /* Use .A to do the ops on the correct byte */
                switch (SignBitByte) {
                  case 0:
                    AddCodeLine ("tya");
                    AddCodeLine ("ora #$%02X", Mask);
                    /* We could jump over the following tya instead, but that wouldn't be faster
                    ** than taking this extra tay and then the tya.
                    */
                    AddCodeLine ("tay");
                    break;
                  case 1:
                    AddCodeLine ("txa");
                    AddCodeLine ("ora #$%02X", Mask);
                    AddCodeLine ("tax");
                    break;
                  case 3:
                    AddCodeLine ("lda sreg+1");
                    AddCodeLine ("ora #$%02X", Mask);
                    AddCodeLine ("sta sreg+1");
                    break;
                  default:
                    FAIL ("Invalid Byte for sign bit");
                }
            } else {
                /* Since we are going to get back .A later anyways, we may just do the op on the
                ** higher bytes with whatever content currently in it.
                */
                unsigned long Mask = ~((2UL << SignBitPos) - 1UL);
                g_or (FullWidthFlags | CF_CONST, Mask);
            }

            /* Get back .A. We need to duplicate the TYA, rather than move it before
            ** the branch to share with the other label, because TYA changes some condition codes.
            */
            g_defcodelabel (ZeroExtendLabel);
            AddCodeLine ("tya");
        }
    } else {
        /* Unsigned bit-field, needs only zero-extension. */
        if (ZeroExtendMask != 0) {
            g_and (FullWidthFlags | CF_CONST, ZeroExtendMask);
        }
    }
}



/*****************************************************************************/
/*                             Switch statement                              */
/*****************************************************************************/



void g_switch (Collection* Nodes, unsigned DefaultLabel, unsigned Depth)
/* Generate code for a switch statement */
{
    unsigned NextLabel = 0;
    unsigned I;

    /* Setup registers and determine which compare insn to use */
    const char* Compare;
    switch (Depth) {
        case 1:
            Compare = "cmp #$%02X";
            break;
        case 2:
            Compare = "cpx #$%02X";
            break;
        case 3:
            AddCodeLine ("ldy sreg");
            Compare = "cpy #$%02X";
            break;
        case 4:
            AddCodeLine ("ldy sreg+1");
            Compare = "cpy #$%02X";
            break;
        default:
            Internal ("Invalid depth in g_switch: %u", Depth);
    }

    /* Walk over all nodes */
    for (I = 0; I < CollCount (Nodes); ++I) {

        /* Get the next case node */
        CaseNode* N = CollAtUnchecked (Nodes, I);

        /* If we have a next label, define it */
        if (NextLabel) {
            g_defcodelabel (NextLabel);
            NextLabel = 0;
        }

        /* Do the compare */
        AddCodeLine (Compare, CN_GetValue (N));

        /* If this is the last level, jump directly to the case code if found */
        if (Depth == 1) {

            /* Branch if equal */
            g_falsejump (0, CN_GetLabel (N));

        } else {

            /* Determine the next label */
            if (I == CollCount (Nodes) - 1) {
                /* Last node means not found */
                g_truejump (0, DefaultLabel);
            } else {
                /* Jump to the next check */
                NextLabel = GetLocalLabel ();
                g_truejump (0, NextLabel);
            }

            /* Check the next level */
            g_switch (N->Nodes, DefaultLabel, Depth-1);

        }
    }

    /* If we go here, we haven't found the label */
    g_jump (DefaultLabel);
}



/*****************************************************************************/
/*                       User supplied assembler code                        */
/*****************************************************************************/



void g_asmcode (struct StrBuf* B)
/* Output one line of assembler code. */
{
    int len = (int) SB_GetLen(B);
    const char *buf = SB_GetConstBuf(B);

    /* remove whitespace at end of line */
    /* NOTE: This masks problems in ParseInsn(), which in some cases seems to
             rely on no whitespace being present at the end of a line in generated
             code (see issue #1252). However, it generally seems to be a good
             idea to remove trailing whitespace from (inline) assembly, so we
             do it anyway. */
    while (len) {
       switch (buf[len - 1]) {
       case '\n':
       case ' ':
       case '\t':
           --len;
           continue;
       }
       break;
    }

    AddCodeLine ("%.*s", len, buf);
}
