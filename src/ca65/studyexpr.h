/*****************************************************************************/
/*                                                                           */
/*                                studyexpr.h                                */
/*                                                                           */
/*                         Study an expression tree                          */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003-2012,  Ullrich von Bassewitz                                     */
/*                 Roemerstrasse 52                                          */
/*                 D-70794 Filderstadt                                       */
/* EMail:          uz@cc65.org                                               */
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



#ifndef STUDYEXPR_H
#define STUDYEXPR_H

                                            

/* common */
#include "exprdefs.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Flags */
#define ED_OK           0x00            /* Nothing special */
#define ED_TOO_COMPLEX  0x01            /* Expression is too complex */
#define ED_ERROR        0x02            /* Error evaluating the expression */

/* Symbol reference */
typedef struct ED_SymRef ED_SymRef;
struct ED_SymRef {
    long                Count;          /* Number of references */
    struct SymEntry*    Ref;            /* Actual reference */
};

/* Section reference */
typedef struct ED_SecRef ED_SecRef;
struct ED_SecRef {
    long                Count;          /* Number of references */
    unsigned            Ref;            /* Actual reference */
};

/* Structure for parsing expression trees */
typedef struct ExprDesc ExprDesc;
struct ExprDesc {
    unsigned short      Flags;          /* See ED_xxx */
    unsigned char       AddrSize;       /* Address size of the expression */
    long                Val;            /* The offset value */
    long                Right;          /* Right value for StudyBinaryExpr */

    /* Symbol reference management */
    unsigned            SymCount;       /* Number of symbols referenced */
    unsigned            SymLimit;       /* Memory allocated */
    ED_SymRef*          SymRef;         /* Symbol references */

    /* Section reference management */
    unsigned            SecCount;       /* Number of sections referenced */
    unsigned            SecLimit;       /* Memory allocated */
    ED_SecRef*          SecRef;         /* Section references */
};



/*****************************************************************************/
/*                              struct ExprDesc                              */
/*****************************************************************************/



ExprDesc* ED_Init (ExprDesc* ED);
/* Initialize an ExprDesc structure for use with StudyExpr */

void ED_Done (ExprDesc* ED);
/* Delete allocated memory for an ExprDesc. */

int ED_IsConst (const ExprDesc* ED);
/* Return true if the expression is constant */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void StudyExpr (ExprNode* Expr, ExprDesc* D);
/* Study an expression tree and place the contents into D */



/* End of studyexpr.h */

#endif
