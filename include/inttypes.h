/*****************************************************************************/
/*                                                                           */
/*                                inttypes.h                                 */
/*                                                                           */
/*                    Format conversion of integer types                     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2011, Ullrich von Bassewitz                                      */
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



/* Note: This file is not fully ISO 9899-1999 compliant because cc65 lacks
** a 64 bit data types and is not able to return structs > 4 bytes. The
** declarations have been adjusted accordingly or left out.
*/



#ifndef _INTTYPES_H
#define _INTTYPES_H



/* inttypes.h must always include stdint.h */
#ifndef _STDINT_H
#include <stdint.h>
#endif



/* Standard functions */
intmax_t __fastcall__ imaxabs (intmax_t val);
intmax_t __fastcall__ strtoimax (const char* nptr, char** endptr, int base);
uintmax_t __fastcall__ strtoumax (const char* nptr, char** endptr, int base);



/* printf() macros for signed integers */
#define PRId8                   "d"
#define PRId16                  "d"
#define PRId32                  "ld"

#define PRIdLEAST8              "d"
#define PRIdLEAST16             "d"
#define PRIdLEAST32             "ld"

#define PRIdFAST8               "d"
#define PRIdFAST16              "d"
#define PRIdFAST32              "ld"

#define PRIdMAX                 "ld"
#define PRIdPTR                 "d"

#define PRIi8                   "i"
#define PRIi16                  "i"
#define PRIi32                  "li"

#define PRIiLEAST8              "i"
#define PRIiLEAST16             "i"
#define PRIiLEAST32             "li"

#define PRIiFAST8               "i"
#define PRIiFAST16              "i"
#define PRIiFAST32              "li"

#define PRIiMAX                 "li"
#define PRIiPTR                 "i"

/* fprintf() macros for unsigned integers */
#define PRIo8                   "o"
#define PRIo16                  "o"
#define PRIo32                  "lo"

#define PRIoLEAST8              "o"
#define PRIoLEAST16             "o"
#define PRIoLEAST32             "lo"

#define PRIoFAST8               "o"
#define PRIoFAST16              "o"
#define PRIoFAST32              "lo"

#define PRIoMAX                 "lo"
#define PRIoPTR                 "o"

#define PRIu8                   "u"
#define PRIu16                  "u"
#define PRIu32                  "lu"

#define PRIuLEAST8              "u"
#define PRIuLEAST16             "u"
#define PRIuLEAST32             "lu"

#define PRIuFAST8               "u"
#define PRIuFAST16              "u"
#define PRIuFAST32              "lu"

#define PRIuMAX                 "lu"
#define PRIuPTR                 "u"

#define PRIx8                   "x"
#define PRIx16                  "x"
#define PRIx32                  "lx"

#define PRIxLEAST8              "x"
#define PRIxLEAST16             "x"
#define PRIxLEAST32             "lx"

#define PRIxFAST8               "x"
#define PRIxFAST16              "x"
#define PRIxFAST32              "lx"

#define PRIxMAX                 "lx"
#define PRIxPTR                 "x"

#define PRIX8                   "X"
#define PRIX16                  "X"
#define PRIX32                  "lX"

#define PRIXLEAST8              "X"
#define PRIXLEAST16             "X"
#define PRIXLEAST32             "lX"

#define PRIXFAST8               "X"
#define PRIXFAST16              "X"
#define PRIXFAST32              "lX"

#define PRIXMAX                 "lX"
#define PRIXPTR                 "X"

/* fscanf() macros for signed integers */
#define SCNd8                   "hd"
#define SCNd16                  "d"
#define SCNd32                  "ld"

#define SCNdLEAST8              "hd"
#define SCNdLEAST16             "d"
#define SCNdLEAST32             "ld"

#define SCNdFAST8               "hd"
#define SCNdFAST16              "d"
#define SCNdFAST32              "ld"

#define SCNdMAX                 "ld"
#define SCNdPTR                 "d"

#define SCNi8                   "hi"
#define SCNi16                  "i"
#define SCNi32                  "li"

#define SCNiLEAST8              "hi"
#define SCNiLEAST16             "i"
#define SCNiLEAST32             "li"

#define SCNiFAST8               "hi"
#define SCNiFAST16              "i"
#define SCNiFAST32              "li"

#define SCNiMAX                 "li"
#define SCNiPTR                 "i"

/* fscanf() macros for unsigned integers */
#define SCNo8                   "ho"
#define SCNo16                  "o"
#define SCNo32                  "lo"

#define SCNoLEAST8              "ho"
#define SCNoLEAST16             "o"
#define SCNoLEAST32             "lo"

#define SCNoFAST8               "ho"
#define SCNoFAST16              "o"
#define SCNoFAST32              "lo"

#define SCNoMAX                 "lo"
#define SCNoPTR                 "o"

#define SCNu8                   "hu"
#define SCNu16                  "u"
#define SCNu32                  "lu"

#define SCNuLEAST8              "hu"
#define SCNuLEAST16             "u"
#define SCNuLEAST32             "lu"

#define SCNuFAST8               "hu"
#define SCNuFAST16              "u"
#define SCNuFAST32              "lu"

#define SCNuMAX                 "lu"
#define SCNuPTR                 "u"

#define SCNx8                   "hx"
#define SCNx16                  "x"
#define SCNx32                  "lx"

#define SCNxLEAST8              "hx"
#define SCNxLEAST16             "x"
#define SCNxLEAST32             "lx"

#define SCNxFAST8               "hx"
#define SCNxFAST16              "x"
#define SCNxFAST32              "lx"

#define SCNxMAX                 "lx"
#define SCNxPTR                 "x"



/* End of inttypes.h */
#endif



