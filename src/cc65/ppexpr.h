/*****************************************************************************/
/*                                                                           */
/*                                 ppexpr.h                                  */
/*                                                                           */
/*                      Expressions for C preprocessor                       */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2022  The cc65 Authors                                                */
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



#ifndef PPEXPR_H
#define PPEXPR_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* PPExpr data struct */
typedef struct PPExpr PPExpr;
struct PPExpr
{
    long IVal;
    unsigned Flags;
};

/* PPExpr initializers */
#define AUTO_PPEXPR_INITIALIZER     { 0, 0 }
#define STATIC_PPEXPR_INITIALIZER   { 0, 0 }

/* PPExpr flags */
#define PPEXPR_NONE         0U
#define PPEXPR_UNSIGNED     1U
#define PPEXPR_UNDEFINED    2U



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void ParsePPExpr (PPExpr* Expr);
/* Parse a line for PP expression */



/* End of ppexpr.h */

#endif
