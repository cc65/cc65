/*****************************************************************************/
/*                                                                           */
/*                                alignment.c                                */
/*                                                                           */
/*                             Address aligment                              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2011,      Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
/*                70794 Filderstadt                                          */
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



/* common */
#include "alignment.h"
#include "check.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* To factorize an alignment, we will use the following prime table. It lists
** all primes up to 256, which means we're able to factorize alignments up to
** 0x10000. This is checked in the code.
*/
static const unsigned char Primes[] = {
      2,   3,   5,   7,  11,  13,  17,  19,  23,  29,
     31,  37,  41,  43,  47,  53,  59,  61,  67,  71,
     73,  79,  83,  89,  97, 101, 103, 107, 109, 113,
    127, 131, 137, 139, 149, 151, 157, 163, 167, 173,
    179, 181, 191, 193, 197, 199, 211, 223, 227, 229,
    233, 239, 241, 251
};
#define PRIME_COUNT     (sizeof (Primes) / sizeof (Primes[0]))
#define LAST_PRIME      ((unsigned long)Primes[PRIME_COUNT-1])



/* A number together with its prime factors */
typedef struct FactorizedNumber FactorizedNumber;
struct FactorizedNumber {
    unsigned long       Value;                  /* The actual number */
    unsigned long       Remainder;              /* Remaining prime */
    unsigned char       Powers[PRIME_COUNT];    /* Powers of the factors */
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static void Initialize (FactorizedNumber* F, unsigned long Value)
/* Initialize a FactorizedNumber structure */
{
    unsigned I;

    F->Value = Value;
    F->Remainder = 1;
    for (I = 0; I < PRIME_COUNT; ++I) {
        F->Powers[I] = 0;
    }
}



static void Factorize (unsigned long Value, FactorizedNumber* F)
/* Factorize a value between 1 and 0x10000 that is in F */
{
    unsigned I;

    /* Initialize F */
    Initialize (F, Value);

    /* If the value is 1 we're already done */
    if (Value == 1) {
        return;
    }

    /* Be sure we can factorize */
    CHECK (Value <= MAX_ALIGNMENT && Value != 0);

    /* Handle factor 2 separately for speed */
    while ((Value & 0x01UL) == 0UL) {
        ++F->Powers[0];
        Value >>= 1;
    }

    /* Factorize. */
    I = 1;      /* Skip 2 because it was handled above */
    while (Value > 1) {
        unsigned long Tmp = Value / Primes[I];
        if (Tmp * Primes[I] == Value) {
            /* This is a factor */
            ++F->Powers[I];
            Value = Tmp;
        } else {
            /* This is not a factor, try next one */
            if (++I >= PRIME_COUNT) {
                break;
            }
        }
    }

    /* If something is left, it must be a remaining prime */
    F->Remainder = Value;
}



unsigned long LeastCommonMultiple (unsigned long Left, unsigned long Right)
/* Calculate the least common multiple of two numbers and return
** the result.
*/
{
    unsigned I;
    FactorizedNumber L, R;
    unsigned long Res;

    /* Factorize the two numbers */
    Factorize (Left, &L);
    Factorize (Right, &R);

    /* Generate the result from the factors.
    ** Some thoughts on range problems: Since the largest numbers we can
    ** factorize are 2^16 (0x10000), the only numbers that could produce an
    ** overflow when using 32 bits are exactly these. But the LCM for 2^16
    ** and 2^16 is 2^16 so this will never happen and we're safe.
    */
    Res = L.Remainder * R.Remainder;
    for (I = 0; I < PRIME_COUNT; ++I) {
        unsigned P = (L.Powers[I] > R.Powers[I])? L.Powers[I] : R.Powers[I];
        while (P--) {
            Res *= Primes[I];
        }
    }

    /* Return the calculated lcm */
    return Res;
}



unsigned long AlignAddr (unsigned long Addr, unsigned long Alignment)
/* Align an address to the given alignment */
{
    return ((Addr + Alignment - 1) / Alignment) * Alignment;
}



unsigned long AlignCount (unsigned long Addr, unsigned long Alignment)
/* Calculate how many bytes must be inserted to align Addr to Alignment */
{
    return AlignAddr (Addr, Alignment) - Addr;
}
