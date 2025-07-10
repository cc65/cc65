/*****************************************************************************/
/*                                                                           */
/*                                xsprintf.c                                 */
/*                                                                           */
/*                       Replacement sprintf function                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2004 Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
/*               D-70794 Filderstadt                                         */
/* EMail:        uz@cc65.org                                                 */
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
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <limits.h>

/* common */
#include "chartype.h"
#include "check.h"
#include "strbuf.h"
#include "va_copy.h"
#include "xsprintf.h"



/*****************************************************************************/
/*                                  vsnprintf                                */
/*****************************************************************************/



/* The following is a very basic vsnprintf like function called xvsnprintf. It
** features only the basic format specifiers (especially the floating point
** stuff is missing), but may be extended if required. Reason for supplying
** my own implementation is that vsnprintf is standard but not implemented by
** older compilers, and some that implement it, don't adhere to the standard
** (for example Microsoft with its _vsnprintf).
*/

typedef struct {

    /* Variable argument list pointer */
    va_list     ap;

    /* Output buffer */
    char*       Buf;
    size_t      BufSize;
    size_t      BufFill;

    /* Argument string buffer and string buffer pointer. The string buffer
    ** must be big enough to hold a converted integer of the largest type
    ** including an optional sign and terminating zero.
    */
    char        ArgBuf[256];
    int         ArgLen;

    /* Flags */
    enum {
        fNone     = 0x0000,
        fMinus    = 0x0001,
        fPlus     = 0x0002,
        fSpace    = 0x0004,
        fHash     = 0x0008,
        fZero     = 0x0010,
        fWidth    = 0x0020,
        fPrec     = 0x0040,
        fUnsigned = 0x0080,
        fUpcase   = 0x0100
    } Flags;

    /* Conversion base and table */
    unsigned    Base;
    const char* CharTable;

    /* Field width */
    int         Width;

    /* Precision */
    int         Prec;

    /* Length modifier */
    enum {
        lmChar,
        lmShort,
        lmInt,
        lmLong,
        lmIntMax,
        lmSizeT,
        lmPtrDiffT,
        lmLongDouble,

        /* Unsupported modifiers */
        lmLongLong = lmLong,

        /* Default length is integer */
        lmDefault = lmInt
    } LengthMod;

} PrintfCtrl;



static void AddChar (PrintfCtrl* P, char C)
/* Store one character in the output buffer if there's enough room. */
{
    if (++P->BufFill <= P->BufSize) {
        *P->Buf++ = C;
    }
}



static void AddPadding (PrintfCtrl* P, char C, unsigned Count)
/* Add some amount of padding */
{
    while (Count--) {
        AddChar (P, C);
    }
}



static intmax_t NextIVal (PrintfCtrl*P)
/* Read the next integer value from the variable argument list */
{
    switch (P->LengthMod) {
        case lmChar:        return (char) va_arg (P->ap, int);
        case lmShort:       return (short) va_arg (P->ap, int);
        case lmInt:         return (int) va_arg (P->ap, int);
        case lmLong:        return (long) va_arg (P->ap, long);
        case lmIntMax:      return va_arg (P->ap, intmax_t);
        case lmSizeT:       return (uintmax_t) va_arg (P->ap, size_t);
        case lmPtrDiffT:    return (long) va_arg (P->ap, ptrdiff_t);
        default:
            FAIL ("Invalid type size in NextIVal");
            return 0;
    }
}



static uintmax_t NextUVal (PrintfCtrl*P)
/* Read the next unsigned integer value from the variable argument list */
{
    switch (P->LengthMod) {
        case lmChar:        return (unsigned char) va_arg (P->ap, unsigned);
        case lmShort:       return (unsigned short) va_arg (P->ap, unsigned);
        case lmInt:         return (unsigned int) va_arg (P->ap, unsigned int);
        case lmLong:        return (unsigned long) va_arg (P->ap, unsigned long);
        case lmIntMax:      return va_arg (P->ap, uintmax_t);
        case lmSizeT:       return va_arg (P->ap, size_t);
        case lmPtrDiffT:    return (intmax_t) va_arg (P->ap, ptrdiff_t);
        default:
            FAIL ("Invalid type size in NextUVal");
            return 0;
    }
}



static void ToStr (PrintfCtrl* P, uintmax_t Val)
/* Convert the given value to a (reversed) string */
{
    char* S = P->ArgBuf;
    while (Val) {
        *S++ = P->CharTable[Val % P->Base];
        Val /= P->Base;
    }
    P->ArgLen = S - P->ArgBuf;
}



static void FormatInt (PrintfCtrl* P, uintmax_t Val)
/* Convert the integer value */
{
    char Lead[5];
    unsigned LeadCount = 0;
    unsigned PrecPadding;
    unsigned WidthPadding;
    unsigned I;


    /* Determine the translation table */
    P->CharTable = (P->Flags & fUpcase)? "0123456789ABCDEF" : "0123456789abcdef";

    /* Check if the value is negative */
    if ((P->Flags & fUnsigned) == 0 && ((intmax_t) Val) < 0) {
        Val = -((intmax_t) Val);
        Lead[LeadCount++] = '-';
    } else if ((P->Flags & fPlus) != 0) {
        Lead[LeadCount++] = '+';
    } else if ((P->Flags & fSpace) != 0) {
        Lead[LeadCount++] = ' ';
    }

    /* Convert the value into a (reversed string). */
    ToStr (P, Val);

    /* The default precision for all integer conversions is one. This means
    ** that the fPrec flag is always set and does not need to be checked
    ** later on.
    */
    if ((P->Flags & fPrec) == 0) {
        P->Flags |= fPrec;
        P->Prec = 1;
    }

    /* Determine the leaders for alternative forms */
    if ((P->Flags & fHash) != 0) {
        if (P->Base == 16) {
            /* Start with 0x */
            Lead[LeadCount++] = '0';
            Lead[LeadCount++] = (P->Flags & fUpcase)? 'X' : 'x';
        } else if (P->Base == 8) {
            /* Alternative form for 'o': always add a leading zero. */
            if (P->Prec <= P->ArgLen) {
                Lead[LeadCount++] = '0';
            }
        }
    }

    /* Determine the amount of precision padding needed */
    if (P->ArgLen < P->Prec) {
        PrecPadding = P->Prec - P->ArgLen;
    } else {
        PrecPadding = 0;
    }

    /* Determine the width padding needed */
    if ((P->Flags & fWidth) != 0) {
        int CurWidth = LeadCount + PrecPadding + P->ArgLen;
        if (CurWidth < P->Width) {
            WidthPadding = P->Width - CurWidth;
        } else {
            WidthPadding = 0;
        }
    } else {
        WidthPadding = 0;
    }

    /* Output left space padding if any */
    if ((P->Flags & (fMinus | fZero)) == 0 && WidthPadding > 0) {
        AddPadding (P, ' ', WidthPadding);
        WidthPadding = 0;
    }

    /* Leader */
    for (I = 0; I < LeadCount; ++I) {
        AddChar (P, Lead[I]);
    }

    /* Left zero padding if any */
    if ((P->Flags & fZero) != 0 && WidthPadding > 0) {
        AddPadding (P, '0', WidthPadding);
        WidthPadding = 0;
    }

    /* Precision padding */
    if (PrecPadding > 0) {
        AddPadding (P, '0', PrecPadding);
    }

    /* The number itself. Beware: It's reversed! */
    while (P->ArgLen > 0) {
        AddChar (P, P->ArgBuf[--P->ArgLen]);
    }

    /* Right width padding if any */
    if (WidthPadding > 0) {
        AddPadding (P, ' ', WidthPadding);
    }
}



static void FormatStr (PrintfCtrl* P, const char* Val)
/* Convert the string */
{
    unsigned WidthPadding;

    /* Get the string length limited to the precision. Beware: We cannot use
    ** strlen here, because if a precision is given, the string may not be
    ** zero terminated.
    */
    int Len;
    if ((P->Flags & fPrec) != 0) {
        const char* S = memchr (Val, '\0', P->Prec);
        if (S == 0) {
            /* Not zero terminated */
            Len = P->Prec;
        } else {
            /* Terminating zero found */
            Len = S - Val;
        }
    } else {
        Len = strlen (Val);
    }

    /* Determine the width padding needed */
    if ((P->Flags & fWidth) != 0 && P->Width > Len) {
        WidthPadding = P->Width - Len;
    } else {
        WidthPadding = 0;
    }

    /* Output left padding */
    if ((P->Flags & fMinus) != 0 && WidthPadding > 0) {
        AddPadding (P, ' ', WidthPadding);
        WidthPadding = 0;
    }

    /* Output the string */
    while (Len--) {
        AddChar (P, *Val++);
    }

    /* Output right padding if any */
    if (WidthPadding > 0) {
        AddPadding (P, ' ', WidthPadding);
    }
}



static void StoreOffset (PrintfCtrl* P)
/* Store the current output offset (%n format spec) */
{
    switch (P->LengthMod) {
        case lmChar:     *va_arg (P->ap, char*)      = (char)P->BufFill; break;
        case lmShort:    *va_arg (P->ap, short*)     = (short)P->BufFill; break;
        case lmInt:      *va_arg (P->ap, int*)       = P->BufFill; break;
        case lmLong:     *va_arg (P->ap, long*)      = P->BufFill; break;
        case lmIntMax:   *va_arg (P->ap, intmax_t*)  = P->BufFill; break;
        case lmSizeT:    *va_arg (P->ap, size_t*)    = P->BufFill; break;
        case lmPtrDiffT: *va_arg (P->ap, ptrdiff_t*) = P->BufFill; break;
        default: FAIL ("Invalid size modifier for %n format spec. in xvsnprintf()");
    }
}



int xvsnprintf (char* Buf, size_t Size, const char* Format, va_list ap)
/* A basic vsnprintf implementation. Does currently only support integer
** formats.
*/
{
    PrintfCtrl P;
    int Done;
    char F;
    char SBuf[2];
    const char* SPtr;
    int UseStrBuf = 0;


    /* Initialize the control structure */
    va_copy (P.ap, ap);
    P.Buf       = Buf;
    P.BufSize   = Size;
    P.BufFill   = 0;

    /* Parse the format string */
    while ((F = *Format++) != '\0') {

        if (F != '%') {
            /* Not a format specifier, just copy */
            AddChar (&P, F);
            continue;
        }

        /* Check for %% */
        if (*Format == '%') {
            ++Format;
            AddChar (&P, '%');
            continue;
        }

        /* It's a format specifier. Check for flags. */
        F = *Format++;
        P.Flags = fNone;
        Done = 0;
        while (F != '\0' && !Done) {
            switch (F) {
                case '-': P.Flags |= fMinus; F = *Format++; break;
                case '+': P.Flags |= fPlus;  F = *Format++; break;
                case ' ': P.Flags |= fSpace; F = *Format++; break;
                case '#': P.Flags |= fHash;  F = *Format++; break;
                case '0': P.Flags |= fZero;  F = *Format++; break;
                default:  Done     = 1;                     break;
            }
        }
        /* Optional field width */
        if (F == '*') {
            P.Width = va_arg (P.ap, int);
            /* A negative field width argument is taken as a - flag followed
            ** by a positive field width.
            */
            if (P.Width < 0) {
                P.Flags |= fMinus;
                P.Width = -P.Width;
            }
            F = *Format++;
            P.Flags |= fWidth;
        } else if (IsDigit (F)) {
            P.Width = F - '0';
            while (1) {
                F = *Format++;
                if (!IsDigit (F)) {
                    break;
                }
                P.Width = P.Width * 10 + (F - '0');
            }
            P.Flags |= fWidth;
        }

        /* Optional precision */
        if (F == '.') {
            F = *Format++;
            P.Flags |= fPrec;
            if (F == '*') {
                P.Prec = va_arg (P.ap, int);
                /* A negative precision argument is taken as if the precision
                ** were omitted.
                */
                if (P.Prec < 0) {
                    P.Flags &= ~fPrec;
                }
                F = *Format++;          /* Skip the '*' */
            } else if (IsDigit (F)) {
                P.Prec = F - '0';
                while (1) {
                    F = *Format++;
                    if (!IsDigit (F)) {
                        break;
                    }
                    P.Prec = P.Prec * 10 + (F - '0');
                }
            } else if (F == '-') {
                /* A negative precision argument is taken as if the precision
                ** were omitted.
                */
                F = *Format++;          /* Skip the minus */
                while (IsDigit (F = *Format++)) ;
                P.Flags &= ~fPrec;
            } else {
                P.Prec = 0;
            }
        }

        /* Optional length modifier */
        P.LengthMod = lmDefault;
        switch (F) {

            case 'h':
                F = *Format++;
                if (F == 'h') {
                    F = *Format++;
                    P.LengthMod = lmChar;
                } else {
                    P.LengthMod = lmShort;
                }
                break;

            /* support the MSVC specific I64 for long long */
            case 'I':
                F = *Format++;
                if (F == '6') {
                    F = *Format++;
                    if (F == '4') {
                        F = *Format++;
                        P.LengthMod = lmLongLong;
                    }
                }
                break;

            case 'l':
                F = *Format++;
                if (F == 'l') {
                    F = *Format++;
                    P.LengthMod = lmLongLong;
                } else {
                    P.LengthMod = lmLong;
                }
                break;

            case 'j':
                P.LengthMod = lmIntMax;
                F = *Format++;
                break;

            case 'z':
                P.LengthMod = lmSizeT;
                F = *Format++;
                break;

            case 't':
                P.LengthMod = lmPtrDiffT;
                F = *Format++;
                break;

            case 'L':
                P.LengthMod = lmLongDouble;
                F = *Format++;
                break;

        }

        /* If the space and + flags both appear, the space flag is ignored */
        if ((P.Flags & (fSpace | fPlus)) == (fSpace | fPlus)) {
            P.Flags &= ~fSpace;
        }
        /* If the 0 and - flags both appear, the 0 flag is ignored */
        if ((P.Flags & (fZero | fMinus)) == (fZero | fMinus)) {
            P.Flags &= ~fZero;
        }
        /* If a precision is specified, the 0 flag is ignored */
        if (P.Flags & fPrec) {
            P.Flags &= ~fZero;
        }

        /* Conversion specifier */
        switch (F) {

            case 'd':
            case 'i':
                P.Base = 10;
                FormatInt (&P, NextIVal (&P));
                break;

            case 'o':
                P.Flags |= fUnsigned;
                P.Base = 8;
                FormatInt (&P, NextUVal (&P));
                break;

            case 'u':
                P.Flags |= fUnsigned;
                P.Base = 10;
                FormatInt (&P, NextUVal (&P));
                break;

            case 'X':
                P.Flags |= (fUnsigned | fUpcase);
                /* FALLTHROUGH */
            case 'x':
                P.Base = 16;
                FormatInt (&P, NextUVal (&P));
                break;

            case 'c':
                SBuf[0] = (char) va_arg (P.ap, int);
                SBuf[1] = '\0';
                FormatStr (&P, SBuf);
                break;

            case 's':
                SPtr = va_arg (P.ap, const char*);
                CHECK (SPtr != 0);
                FormatStr (&P, SPtr);
                break;

            case 'p':
                /* See comment at top of header file */
                if (UseStrBuf) {
                    /* Argument is StrBuf */
                    const StrBuf* S = va_arg (P.ap, const StrBuf*);
                    CHECK (S != 0);
                    /* Handle the length by using a precision */
                    if ((P.Flags & fPrec) != 0) {
                        /* Precision already specified, use length of string
                        ** if less.
                        */
                        if ((unsigned) P.Prec > SB_GetLen (S)) {
                            P.Prec = SB_GetLen (S);
                        }
                    } else {
                        /* No precision, add it */
                        P.Flags |= fPrec;
                        P.Prec  = SB_GetLen (S);
                    }
                    FormatStr (&P, SB_GetConstBuf (S));
                    UseStrBuf = 0;              /* Reset flag */
                } else {
                    /* Use hex format for pointers */
                    P.Flags |= (fUnsigned | fPrec);
                    P.Prec = ((sizeof (void*) * CHAR_BIT) + 3) / 4;
                    P.Base = 16;
                    FormatInt (&P, (uintptr_t) va_arg (P.ap, void*));
                }
                break;

            case 'm':
                /* See comment at top of header file */
                UseStrBuf = 1;
                break;

            case 'n':
                StoreOffset (&P);
                break;

            default:
                /* Invalid format spec */
                FAIL ("Invalid format specifier in xvsnprintf");

        }
    }

    /* We don't need P.ap any longer */
    va_end (P.ap);

    /* Terminate the output string and return the number of chars that had
    ** been written if the buffer was large enough.
    ** Beware: The terminating zero is not counted for the function result!
    */
    AddChar (&P, '\0');
    return P.BufFill - 1;
}



int xsnprintf (char* Buf, size_t Size, const char* Format, ...)
/* A basic snprintf implementation. Does currently only support integer
** formats.
*/
{
    int Res;
    va_list ap;

    va_start (ap, Format);
    Res = xvsnprintf (Buf, Size, Format, ap);
    va_end (ap);

    return Res;
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



int xsprintf (char* Buf, size_t BufSize, const char* Format, ...)
/* Replacement function for sprintf */
{
    int Res;
    va_list ap;

    va_start (ap, Format);
    Res = xvsprintf (Buf, BufSize, Format, ap);
    va_end (ap);

    return Res;
}



int xvsprintf (char* Buf, size_t BufSize, const char* Format, va_list ap)
/* Replacement function for sprintf */
{
    int Res = xvsnprintf (Buf, BufSize, Format, ap);
    CHECK (Res >= 0 && (unsigned) (Res+1) < BufSize);
    return Res;
}
