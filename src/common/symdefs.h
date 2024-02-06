/*****************************************************************************/
/*                                                                           */
/*                                 symdefs.h                                 */
/*                                                                           */
/*               Symbol definitions for the bin65 binary utils               */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2011, Ullrich von Bassewitz                                      */
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



/* Object file tags for imports and exports */



#ifndef SYMDEFS_H
#define SYMDEFS_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Number of module constructor/destructor declarations for an export */
#define SYM_CONDES_MASK         0x0007U

#define SYM_IS_CONDES(x)        (((x) & SYM_CONDES_MASK) != 0)
#define SYM_GET_CONDES_COUNT(x) ((x) & SYM_CONDES_MASK)
#define SYM_INC_CONDES_COUNT(x) ((x)++)

/* Size of symbol available? */
#define SYM_SIZELESS            0x0000U /* No symbol size available */
#define SYM_SIZE                0x0008U /* Symbol has a size */
#define SYM_MASK_SIZE           0x0008U /* Size mask */

#define SYM_HAS_SIZE(x)         (((x) & SYM_MASK_SIZE) == SYM_SIZE)

/* Symbol value type */
#define SYM_CONST               0x0000U /* Mask bit for const values */
#define SYM_EXPR                0x0010U /* Mask bit for expr values */
#define SYM_MASK_VAL            0x0010U /* Value mask */

#define SYM_IS_CONST(x)         (((x) & SYM_MASK_VAL) == SYM_CONST)
#define SYM_IS_EXPR(x)          (((x) & SYM_MASK_VAL) == SYM_EXPR)

/* Symbol usage */
#define SYM_EQUATE              0x0000U /* Mask bit for an equate */
#define SYM_LABEL               0x0020U /* Mask bit for a label */
#define SYM_MASK_LABEL          0x0020U /* Value mask */

#define SYM_IS_EQUATE(x)        (((x) & SYM_MASK_LABEL) == SYM_EQUATE)
#define SYM_IS_LABEL(x)         (((x) & SYM_MASK_LABEL) == SYM_LABEL)

/* Symbol type */
#define SYM_STD                 0x0000U /* Standard symbol */
#define SYM_CHEAP_LOCAL         0x0040U /* Cheap local symbol */
#define SYM_MASK_TYPE           0x0040U /* Value mask */

#define SYM_IS_STD(x)           (((x) & SYM_MASK_TYPE) == SYM_STD)
#define SYM_IS_CHEAP_LOCAL      (((x) & SYM_MASK_TYPE) == SYM_CHEAP_LOCAL)

/* Export */
#define SYM_EXPORT              0x0080U /* Export */
#define SYM_MASK_EXPORT         0x0080U /* Value mask */

#define SYM_IS_EXPORT(x)        (((x) & SYM_MASK_EXPORT) == SYM_EXPORT)

/* Import */
#define SYM_IMPORT              0x0100U /* Import */
#define SYM_MASK_IMPORT         0x0100U /* Value mask */

#define SYM_IS_IMPORT(x)        (((x) & SYM_MASK_IMPORT) == SYM_IMPORT)



/* End of symdefs.h */

#endif
