/*****************************************************************************/
/*                                                                           */
/*                                  stmt.h                                   */
/*                                                                           */
/*                             Parse a statement                             */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2008 Ullrich von Bassewitz                                       */
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



#ifndef STMT_H
#define STMT_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/


/* Enumeration for the AnyStatement() return flags. It is used for simple flow
** analysis and tracks occurance of "return", "break", "goto" and "continue"
** (anything that jumps). The flags for the distinct statements are only set
** if they are always executed. So for example in case of an if/else statement,
** SF_RETURN is set only if both branches contain a "return". The SF_ANY_xxx
** flags are set if any of the statements occurred. So for an if/else
** statement, if one branch contains a "return" and the other a "continue"
** statement, neither SF_RETURN, nor SF_CONTINUE is set, but SF_ANY_RETURN and
** SF_ANY_CONTINUE.
** There are some additional flags that tell if the statement parsed was
** preceeded by at least one label.
*/
enum {
    SF_NONE             = 0x0000,

    /* Flags for special statements that cause the next statement to be
    ** unreachable. They are only return as long as the condition is true.
    ** Please note that a statement function can return more than one of
    ** these flags set. For example for an "if" that has "return" in one
    ** branch and "continue" in another.
    */
    SF_RETURN           = 0x00001,      /* Code definitely returns */
    SF_BREAK            = 0x00002,      /* Code definitely breaks */
    SF_GOTO             = 0x00004,      /* Code definitely jumps */
    SF_CONTINUE         = 0x00008,      /* Code definitely continues */
    SF_OTHER            = 0x00010,      /* Endless loop, terminating func */
    SF_MASK_UNREACH     = 0x000FF,      /* Following code is unreachable */

    /* Flags for the occurance of any of the conditions from above. Statements
    ** will clear whatever is no longer valid when exiting. A while loop, for
    ** example will never return SF_ANY_BREAK or SF_ANY_CONTINUE since that was
    ** handled inside the loop.
    */
    SF_ANY_RETURN       = 0x00100,      /* Code contains a return statement */
    SF_ANY_BREAK        = 0x00200,      /* Code contains a break statement */
    SF_ANY_GOTO         = 0x00400,      /* Code contains a goto statement */
    SF_ANY_CONTINUE     = 0x00800,      /* Code contains a continue statement */
    SF_ANY_OTHER        = 0x01000,      /* Code contains endless loop etc. */
    SF_MASK_ANY         = 0x0FF00,      /* Code contains any of the above */

    /* Flags for labels */
    SF_LABEL_GOTO       = 0x10000,      /* Statement preceeded by goto label */
    SF_LABEL_CASE       = 0x20000,      /* Statement preceeded by case label */
    SF_LABEL_DEFAULT    = 0x40000,      /* Statement preceeded by default label */
    SF_MASK_LABEL       = 0x70000,      /* Mask for any label */
};

/* Forward */
struct SwitchCtrl;



/*****************************************************************************/
/*                Functions to handle the flow control flags                 */
/*****************************************************************************/



static inline int SF_Break (int F)
/* Return just the "break" flag in F */
{
    return (F & SF_BREAK);
}

static inline int SF_Continue (int F)
/* Return just the "continue" flag in F */
{
    return (F & SF_CONTINUE);
}

static inline int SF_Unreach (int F)
/* Return just the "unreachable" part of the given flags */
{
    return (F & SF_MASK_UNREACH);
}

static inline int SF_Any_Break (int F)
/* Check if there was any "break" statement */
{
    return (F & SF_ANY_BREAK);
}

static inline int SF_Any (int F)
/* Return just the "any" part of the given flags */
{
    return (F & SF_MASK_ANY);
}

static inline int SF_Label (int F)
/* Return just the "label" part of the given flags */
{
    return (F & SF_MASK_LABEL);
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



int StatementBlock (struct SwitchCtrl* Switch);
/* Parse multiple statements within curly braces checking for unreachable
** code. Returns the SF_xxx flags for the last statement.
*/

int AnyStatement (int* PendingToken, struct SwitchCtrl* Switch);
/* Statement parser. Returns one of the SF_xxx flags describing if the
** statement does a return/break. If the PendingToken pointer is not NULL,
** the function will not skip the terminating token of the statement (closing
** brace or semicolon), but store true if there is a pending token, and false
** if there is none. The token is always checked, so there is no need for the
** caller to check this token, it must be skipped, however. If the argument
** pointer is NULL, the function will skip the token. When called to parse a
** switch body, the switch control structure must be passed via the Switch
** argument. Otherwise it must be NULL.
*/



/* End of stmt.h */

#endif
