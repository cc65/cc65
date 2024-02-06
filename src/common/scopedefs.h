/*****************************************************************************/
/*                                                                           */
/*                                 symdefs.h                                 */
/*                                                                           */
/*               Scope definitions for the bin65 binary utils                */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2011,      Ullrich von Bassewitz                                      */
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



#ifndef SCOPEDEFS_H
#define SCOPEDEFS_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Size of scope available? */
#define SCOPE_SIZELESS          0x00U   /* No scope size available */
#define SCOPE_SIZE              0x01U   /* Scope has a size */
#define SCOPE_MASK_SIZE         0x01U   /* Size mask */

#define SCOPE_HAS_SIZE(x)       (((x) & SCOPE_MASK_SIZE) == SCOPE_SIZE)

/* Does the scope has a label? */
#define SCOPE_UNLABELED         0x00U   /* Scope has no label */
#define SCOPE_LABELED           0x02U   /* Scope has a label */
#define SCOPE_MASK_LABEL        0x02U   /* Label size */

#define SCOPE_HAS_LABEL(x)      (((x) & SCOPE_MASK_LABEL) == SCOPE_LABELED)



/* Scope types */
enum {
    SCOPE_GLOBAL,                       /* Global level */
    SCOPE_FILE,                         /* File level */
    SCOPE_SCOPE,                        /* .SCOPE/.PROC */
    SCOPE_HAS_DATA = SCOPE_SCOPE,       /* Last scope that contains data */
    SCOPE_STRUCT,                       /* .STRUCT/.UNION */
    SCOPE_ENUM,                         /* .ENUM */
    SCOPE_UNDEF    = 0xFF
};



/* End of scopedefs.h */

#endif
