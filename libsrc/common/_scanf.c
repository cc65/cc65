/*
** _scanf.c
**
** (c) Copyright 2001-2005, Ullrich von Bassewitz <uz@cc65.org>
** 2014-09-10, Greg King <greg.king5@verizon.net>
**
** This is the basic layer for all scanf-type functions.  It should be
** rewritten in assembly, at some time in the future.  So, some of the code
** is not as elegant as it could be.
*/



#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <limits.h>
#include <errno.h>

#include <ctype.h>
/* _scanf() can give EOF to these functions.  But, the macroes can't
** understand it; so, they are removed.
*/
#undef isspace
#undef isxdigit

#include "_scanf.h"

#pragma static-locals(on)



/*****************************************************************************/
/*                            SetJmp return codes                            */
/*****************************************************************************/



enum {
    RC_OK,                              /* setjmp() call */
    RC_NOCONV,                          /* No conversion possible */
    RC_EOF                              /* EOF reached */
};



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



static const char*    format;           /* Copy of function argument */
static const struct scanfdata* D_;      /* Copy of function argument */
static va_list        ap;               /* Copy of function argument */
static jmp_buf        JumpBuf;          /* "Label" that is used for failures */
static char           F;                /* Character from format string */
static unsigned       CharCount;        /* Characters read so far */
static int            C;                /* Character from input */
static unsigned       Width;            /* Maximum field width */
static long           IntVal;           /* Converted int value */
static int            Assignments;      /* Number of assignments */
static unsigned char  IntBytes;         /* Number of bytes-1 for int conversions */

/* Flags */
static bool           Converted;        /* Some object was converted */
static bool           Positive;         /* Flag for positive value */
static bool           NoAssign;         /* Suppress assignment */
static bool           Invert;           /* Do we need to invert the charset? */
static unsigned char  CharSet[(1+UCHAR_MAX)/CHAR_BIT];
static const unsigned char Bits[CHAR_BIT] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80
};

/* We need C to be 16 bits since we cannot check for EOF otherwise.
** Unfortunately, this causes the code to be quite larger, even if for most
** purposes, checking the low byte would be enough, since if C is EOF, the
** low byte will not match any useful character anyway (at least for the
** supported platforms - I know that this is not portable). So the following
** macro is used to access just the low byte of C.
*/
#define CHAR(c)         (*((unsigned char*)&(c)))



/*****************************************************************************/
/*                              Character sets                               */
/*****************************************************************************/



/* We don't want the optimizer to ruin our "perfect" ;-)
** assembly code!
*/
#pragma optimize (push, off)

static unsigned FindBit (void)
/* Locate the character's bit in the charset array.
** < .A - Argument character
** > .X - Offset of the byte in the character-set mask
** > .A - Bit-mask
*/
{
    asm ("pha");
    asm ("lsr a");              /* Divide by CHAR_BIT */
    asm ("lsr a");
    asm ("lsr a");
    asm ("tax");                /* Byte's offset */
    asm ("pla");
    asm ("and #%b", CHAR_BIT-1);
    asm ("tay");                /* Bit's offset */
    asm ("lda %v,y", Bits);
    return (unsigned) __AX__;
}

#pragma optimize (pop)


static void __fastcall__ AddCharToSet (unsigned char /* C */)
/* Set the given bit in the character set */
{
    FindBit();
    asm ("ora %v,x", CharSet);
    asm ("sta %v,x", CharSet);
}



#pragma optimize (push, off)

static unsigned char IsCharInSet (void)
/* Check if the char. is part of the character set. */
{
    /* Get the character from C. */
    asm ("lda #$00");
    asm ("ldx %v+1", C);
    asm ("bne L1");                     /* EOF never is in the set */
    asm ("lda %v", C);
    FindBit();
    asm ("and %v,x", CharSet);
    asm ("L1:");
    asm ("ldx #$00");
    return (unsigned char) __AX__;
}

#pragma optimize (pop)



static void InvertCharSet (void)
/* Invert the character set */
{
    asm ("ldy #%b", sizeof (CharSet) - 1);
    asm ("L1:");
    asm ("lda %v,y", CharSet);
    asm ("eor #$FF");
    asm ("sta %v,y", CharSet);
    asm ("dey");
    asm ("bpl L1");
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static void PushBack (void)
/* Push back the last (unused) character, provided it is not EOF. */
{
    /* Get the character from C. */
    /* Only the high-byte needs to be checked for EOF. */
    asm ("ldx %v+1", C);
    asm ("bne %g", Done);
    asm ("lda %v", C);

    /* Put unget()'s first argument on the stack. */
    asm ("jsr pushax");

    /* Copy D into the zero-page. */
    (const struct scanfdata*) __AX__ = D_;
    asm ("sta ptr1");
    asm ("stx ptr1+1");

    /* Copy the unget vector to jmpvec. */
    asm ("ldy #%b", offsetof (struct scanfdata, unget));
    asm ("lda (ptr1),y");
    asm ("sta jmpvec+1");
    asm ("iny");
    asm ("lda (ptr1),y");
    asm ("sta jmpvec+2");

    /* Load D->data into __AX__. */
    asm ("ldy #%b", offsetof (struct scanfdata, data) + 1);
    asm ("lda (ptr1),y");
    asm ("tax");
    asm ("dey");
    asm ("lda (ptr1),y");

    /* Call the unget routine. */
    asm ("jsr jmpvec");

    /* Take back that character's count. */
    asm ("lda %v", CharCount);
    asm ("bne %g", Yank);
    asm ("dec %v+1", CharCount);
Yank:
    asm ("dec %v", CharCount);

Done:
    ;
}



static void ReadChar (void)
/* Get an input character, count characters */
{
    /* Move D to ptr1 */
    asm ("lda %v", D_);
    asm ("ldx %v+1", D_);
    asm ("sta ptr1");
    asm ("stx ptr1+1");

    /* Copy the get vector to jmpvec */
    asm ("ldy #%b", offsetof (struct scanfdata, get));
    asm ("lda (ptr1),y");
    asm ("sta jmpvec+1");
    asm ("iny");
    asm ("lda (ptr1),y");
    asm ("sta jmpvec+2");

    /* Load D->data into __AX__ */
    asm ("ldy #%b", offsetof (struct scanfdata, data) + 1);
    asm ("lda (ptr1),y");
    asm ("tax");
    asm ("dey");
    asm ("lda (ptr1),y");

    /* Call the get routine */
    asm ("jsr jmpvec");

    /* Assign the result to C */
    asm ("sta %v", C);
    asm ("stx %v+1", C);

    /* If C is EOF, don't bump the character counter.
    ** Only the high-byte needs to be checked.
    */
    asm ("inx");
    asm ("beq %g", Done);

    /* Must bump CharCount. */
    asm ("inc %v", CharCount);
    asm ("bne %g", Done);
    asm ("inc %v+1", CharCount);

Done:
    ;
}



#pragma optimize (push, off)

static void __fastcall__ Error (unsigned char /* Code */)
/* Does a longjmp using the given code */
{
    asm ("pha");
    (char*) __AX__ = JumpBuf;
    asm ("jsr pushax");
    asm ("pla");
    asm ("ldx #>$0000");
    asm ("jmp %v", longjmp);
}

#pragma optimize (pop)



static void CheckEnd (void)
/* Stop a scan if it prematurely reaches the end of a string or a file. */
{
    /* Only the high-byte needs to be checked for EOF. */
    asm ("ldx %v+1", C);
    asm ("beq %g", Done);

        Error (RC_EOF);
Done:
    ;
}



static void SkipWhite (void)
/* Skip white space in the input and return the first non white character */
{
    while ((bool) isspace (C)) {
        ReadChar ();
    }
}



#pragma optimize (push, off)

static void ReadSign (void)
/* Read an optional sign and skip it. Store 1 in Positive if the value is
** positive, store 0 otherwise.
*/
{
    /* We can ignore the high byte of C here, since if it is EOF, the lower
    ** byte won't match anyway.
    */
    asm ("lda %v", C);
    asm ("cmp #'-'");
    asm ("bne %g", NotNeg);

    /* Negative value */
    asm ("sta %v", Converted);
    asm ("jsr %v", ReadChar);
    asm ("lda #$00");           /* Flag as negative */
    asm ("beq %g", Store);

    /* Positive value */
NotNeg:
    asm ("cmp #'+'");
    asm ("bne %g", Pos);
    asm ("sta %v", Converted);
    asm ("jsr %v", ReadChar);   /* Skip the + sign */
Pos:
    asm ("lda #$01");           /* Flag as positive */
Store:
    asm ("sta %v", Positive);
}

#pragma optimize (pop)



static unsigned char __fastcall__ HexVal (char C)
/* Convert a digit to a value */
{
    return (bool) isdigit (C) ?
        C - '0' :
        (char) tolower ((int) C) - ('a' - 10);
}



static void __fastcall__ ReadInt (unsigned char Base)
/* Read an integer, and store it into IntVal. */
{
    unsigned char Val, CharCount = 0;

    /* Read the integer value */
    IntVal = 0L;
    while ((bool) isxdigit (C) && ++Width != 0
           && (Val = HexVal ((char) C)) < Base) {
        ++CharCount;
        IntVal = IntVal * (long) Base + (long) Val;
        ReadChar ();
    }

    /* If we didn't convert anything, it's a failure. */
    if (CharCount == 0) {
        Error (RC_NOCONV);
    }

    /* Another conversion */
    Converted = true;
}



static void AssignInt (void)
/* Assign the integer value in Val to the next argument. The function makes
** several non-portable assumptions, to reduce code size:
**   - signed and unsigned types have the same representation.
**   - short and int have the same representation.
**   - all pointer types have the same representation.
*/
{
    if (NoAssign == false) {

        /* Get the next argument pointer */
        (void*) __AX__ = va_arg (ap, void*);

        /* Put the argument pointer into the zero-page. */
        asm ("sta ptr1");
        asm ("stx ptr1+1");

        /* Get the number of bytes-1 to copy */
        asm ("ldy %v", IntBytes);

        /* Assign the integer value */
Loop:   asm ("lda %v,y", IntVal);
        asm ("sta (ptr1),y");
        asm ("dey");
        asm ("bpl %g", Loop);

        /* Another assignment */
        asm ("inc %v", Assignments);
        asm ("bne %g", Done);
        asm ("inc %v+1", Assignments);
Done:   ;
    }
}



static void __fastcall__ ScanInt (unsigned char Base)
/* Scan an integer including white space, sign and optional base spec,
** and store it into IntVal.
*/
{
    /* Skip whitespace */
    SkipWhite ();

    /* Read an optional sign */
    ReadSign ();

    /* If Base is unknown (zero), figure it out */
    if (Base == 0) {
        if (CHAR (C) == '0') {
            ReadChar ();
            switch (CHAR (C)) {
                case 'x':
                case 'X':
                    Base = 16;
                    Converted = true;
                    ReadChar ();
                    break;
                default:
                    Base = 8;

                    /* Restart at the beginning of the number because it might
                    ** be only a single zero digit (which already was read).
                    */
                    PushBack ();
                    C = '0';
            }
        } else {
            Base = 10;
        }
    }

    /* Read the integer value */
    ReadInt (Base);

    /* Apply the sign */
    if (Positive == false) {
        IntVal = -IntVal;
    }

    /* Assign the value to the next argument unless suppressed */
    AssignInt ();
}



static char GetFormat (void)
/* Pick up the next character from the format string. */
{
/*  return (F = *format++); */
    (const char*) __AX__ = format;
    asm ("sta regsave");
    asm ("stx regsave+1");
    ++format;
    asm ("ldy #0");
    asm ("lda (regsave),y");
    asm ("ldx #>$0000");
    return (F = (char) __AX__);
}



int __fastcall__ _scanf (const struct scanfdata* D,
                         const char* format_, va_list ap_)
/* This is the routine used to do the actual work. It is called from several
** types of wrappers to implement the actual ISO xxscanf functions.
*/
{
    register char* S;
             bool  HaveWidth;   /* True if a width was given */
             bool  Match;       /* True if a character-set has any matches */
             char  Start;       /* Walks over a range */

    /* Place copies of the arguments into global variables. This is not very
    ** nice, but on a 6502 platform it gives better code, since the values
    ** do not have to be passed as parameters.
    */
    D_     = D;
    format = format_;
    ap     = ap_;

    /* Initialize variables */
    Converted   = false;
    Assignments = 0;
    CharCount   = 0;

    /* Set up the jump "label".  CheckEnd() will use that label when EOF
    ** is reached.  ReadInt() will use it when number-conversion fails.
    */
    if ((unsigned char) setjmp (JumpBuf) == RC_OK) {
Again:

        /* Get the next input character */
        ReadChar ();

        /* Walk over the format string */
        while (GetFormat ()) {

            /* Check for a conversion */
            if (F != '%') {

                /* Check for a match */
                if ((bool) isspace ((int) F)) {

                    /* Special white space handling: Any whitespace in the
                    ** format string matches any amount of whitespace including
                    ** none(!). So this match will never fail.
                    */
                    SkipWhite ();
                    continue;
                }

Percent:
                /* ### Note:  The opposite test (C == F)
                ** would be optimized into buggy code!
                */
                if (C != (int) F) {

                    /* A mismatch -- we will stop scanning the input,
                    ** and return the number of assigned conversions.
                    */
                    goto NoConv;
                }

                /* A match -- get the next input character, and continue. */
                goto Again;

            } else {

                /* A conversion. Skip the percent sign. */
                /* 0. Check for %% */
                if (GetFormat () == '%') {
                    goto Percent;
                }

                /* 1. Assignment suppression */
                NoAssign = (F == '*');
                if (NoAssign) {
                    GetFormat ();
                }

                /* 2. Maximum field width */
                Width     = UINT_MAX;
                HaveWidth = (bool) isdigit (F);
                if (HaveWidth) {
                    Width = 0;
                    do {
                        /* ### Non portable ### */
                        Width = Width * 10 + (F & 0x0F);
                    } while ((bool) isdigit (GetFormat ()));
                }
                if (Width == 0) {
                    /* Invalid specification */
                    /* Note:  This method of leaving the function might seem
                    ** to be crude, but it optimizes very well because
                    ** the four exits can share this code.
                    */
                    _seterrno (EINVAL);
                    Assignments = EOF;
                    PushBack ();
                    return Assignments;
                }
                /* Increment-and-test makes better code than test-and-decrement
                ** does.  So, change the width into a form that can be used in
                ** that way.
                */
                Width = ~Width;

                /* 3. Length modifier */
                IntBytes = sizeof(int) - 1;
                switch (F) {
                    case 'h':
                        if (*format == 'h') {
                            IntBytes = sizeof(char) - 1;
                            ++format;
                        }
                        GetFormat ();
                        break;

                    case 'l':
                        if (*format == 'l') {
                            /* Treat long long as long */
                            ++format;
                        }
                        /* FALLTHROUGH */
                    case 'j':   /* intmax_t */
                        IntBytes = sizeof(long) - 1;
                        /* FALLTHROUGH */

                    case 'z':   /* size_t */
                    case 't':   /* ptrdiff_t */
                        /* Same size as int */

                    case 'L':   /* long double - ignore this one */
                        GetFormat ();
                }

                /* 4. Conversion specifier */
                switch (F) {
                    /* 'd' and 'u' conversions are actually the same, since the
                    ** standard says that even the 'u' modifier allows an
                    ** optionally signed integer.
                    */
                    case 'd':   /* Optionally signed decimal integer */
                    case 'u':
                        ScanInt (10);
                        break;

                    case 'i':
                        /* Optionally signed integer with a base */
                        ScanInt (0);
                        break;

                    case 'o':
                        /* Optionally signed octal integer */
                        ScanInt (8);
                        break;

                    case 'x':
                    case 'X':
                        /* Optionally signed hexadecimal integer */
                        ScanInt (16);
                        break;

                    case 's':
                        /* Whitespace-terminated string */
                        SkipWhite ();
                        CheckEnd ();       /* Is it an input failure? */
                        Converted = true;  /* No, conversion will succeed */
                        if (NoAssign == false) {
                            S = va_arg (ap, char*);
                        }
                        while (C != EOF
                               && (bool) isspace (C) == false
                               && ++Width) {
                            if (NoAssign == false) {
                                *S++ = C;
                            }
                            ReadChar ();
                        }
                        /* Terminate the string just read */
                        if (NoAssign == false) {
                            *S = '\0';
                            ++Assignments;
                        }
                        break;

                    case 'c':
                        /* Fixed-length string, NOT zero-terminated */
                        if (HaveWidth == false) {
                            /* No width given, default is 1 */
                            Width = ~1u;
                        }
                        CheckEnd ();       /* Is it an input failure? */
                        Converted = true;  /* No, at least 1 char. available */
                        if (NoAssign == false) {
                            S = va_arg (ap, char*);
                            /* ## This loop is convenient for us, but it isn't
                            ** standard C.  The standard implies that a failure
                            ** shouldn't put anything into the array argument.
                            */
                            while (++Width) {
                                CheckEnd ();  /* Is it a matching failure? */
                                *S++ = C;
                                ReadChar ();
                            }
                            ++Assignments;
                        } else {
                            /* Just skip as many chars as given */
                            while (++Width) {
                                CheckEnd ();  /* Is it a matching failure? */
                                ReadChar ();
                            }
                        }
                        break;

                    case '[':
                        /* String using characters from a set */
                        /* Clear the set */
                        memset (CharSet, 0, sizeof (CharSet));
                        /* Skip the left-bracket, and test for inversion. */
                        Invert = (GetFormat () == '^');
                        if (Invert) {
                            GetFormat ();
                        }
                        if (F == ']') {
                            /* Empty sets aren't allowed; so, a right-bracket
                            ** at the beginning must be a member of the set.
                            */
                            AddCharToSet (F);
                            GetFormat ();
                        }
                        /* Read the characters that are part of the set */
                        while (F != '\0' && F != ']') {
                            if (*format == '-') {  /* Look ahead at next char. */
                                /* A range. Get start and end, skip the '-' */
                                Start = F;
                                ++format;
                                switch (GetFormat ()) {
                                    case '\0':
                                    case ']':
                                        /* '-' as last char means:  include '-' */
                                        AddCharToSet (Start);
                                        AddCharToSet ('-');
                                        break;
                                    default:
                                        /* Include all characters
                                        ** that are in the range.
                                        */
                                        while (1) {
                                            AddCharToSet (Start);
                                            if (Start == F) {
                                                break;
                                            }
                                            ++Start;
                                        }
                                        /* Get next char after range */
                                        GetFormat ();
                                }
                            } else {
                                /* Just a character */
                                AddCharToSet (F);
                                /* Get next char */
                                GetFormat ();
                            }
                        }
                        /* Don't go beyond the end of the format string. */
                        /* (Maybe, this should mean an invalid specification.) */
                        if (F == '\0') {
                            --format;
                        }

                        /* Invert the set if requested */
                        if (Invert) {
                            InvertCharSet ();
                        }

                        /* We have the set in CharSet. Read characters and
                        ** store them into a string while they are part of
                        ** the set.
                        */
                        Match = false;
                        if (NoAssign == false) {
                            S = va_arg (ap, char*);
                        }
                        while (IsCharInSet () && ++Width) {
                            if (NoAssign == false) {
                                *S++ = C;
                            }
                            Match = Converted = true;
                            ReadChar ();
                        }
                        /* At least one character must match the set. */
                        if (Match == false) {
                            goto NoConv;
                        }
                        if (NoAssign == false) {
                            *S = '\0';
                            ++Assignments;
                        }
                        break;

                    case 'p':
                        /* Pointer, general format is 0xABCD.
                        ** %hhp --> zero-page pointer
                        ** %hp --> near pointer
                        ** %lp --> far pointer
                        */
                        SkipWhite ();
                        if (CHAR (C) != '0') {
                            goto NoConv;
                        }
                        Converted = true;
                        ReadChar ();
                        switch (CHAR (C)) {
                            case 'x':
                            case 'X':
                                break;
                            default:
                                goto NoConv;
                        }
                        ReadChar ();
                        ReadInt (16);
                        AssignInt ();
                        break;

                    case 'n':
                        /* Store the number of characters consumed so far
                        ** (the read-ahead character hasn't been consumed).
                        */
                        IntVal = (long) (CharCount - (C == EOF ? 0u : 1u));
                        AssignInt ();
                        /* Don't count it. */
                        if (NoAssign == false) {
                            --Assignments;
                        }
                        break;

                    case 'S':
                    case 'C':
                        /* Wide characters */

                    case 'a':
                    case 'A':
                    case 'e':
                    case 'E':
                    case 'f':
                    case 'F':
                    case 'g':
                    case 'G':
                        /* Optionally signed float */

                        /* Those 2 groups aren't implemented. */
                        _seterrno (ENOSYS);
                        Assignments = EOF;
                        PushBack ();
                        return Assignments;

                    default:
                        /* Invalid specification */
                        _seterrno (EINVAL);
                        Assignments = EOF;
                        PushBack ();
                        return Assignments;
                }
            }
        }
    } else {
NoConv:

        /* Coming here means a failure. If that happens at EOF, with no
        ** conversion attempts, then it is considered an error; otherwise,
        ** the number of assignments is returned (the default behaviour).
        */
        if (C == EOF && Converted == false) {
            Assignments = EOF;  /* Special case:  error */
        }
    }

    /* Put the read-ahead character back into the input stream. */
    PushBack ();

    /* Return the number of conversion-and-assignments. */
    return Assignments;
}



