/*****************************************************************************/
/*                                                                           */
/*                                 declare.h                                 */
/*                                                                           */
/*                 Parse variable and function declarations                  */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2009, Ullrich von Bassewitz                                      */
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



#ifndef DECLARE_H
#define DECLARE_H



/* common */
#include "coll.h"

/* cc65 */
#include "scanner.h"
#include "symtab.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Type specifier parser flags */
typedef enum typespec_t typespec_t;
enum typespec_t {
    TS_NONE                 = 0x00,

    /* Default type */
    TS_MASK_DEFAULT_TYPE    = 0x03,
    TS_DEFAULT_TYPE_NONE    = 0x00,     /* No default type */
    TS_DEFAULT_TYPE_INT     = 0x01,     /* Good old int */
    TS_DEFAULT_TYPE_AUTO    = 0x02,     /* C23 type inference with auto */

    /* Whether to allow certain kinds of specifiers */
    TS_STORAGE_CLASS_SPEC   = 0x04,     /* Allow storage class specifiers */
    TS_FUNCTION_SPEC        = 0x08,     /* Allow function specifiers */
};

/* Masks for the Flags field in DeclSpec */
#define DS_NONE                 0x0000U /* Nothing specified or used    */
#define DS_DEF_STORAGE          0x0001U /* Default storage class used   */
#define DS_EXPLICIT_TYPE        0x0002U /* Type specified               */
#define DS_DEF_TYPE             0x0004U /* Implicit type used           */
#define DS_AUTO_TYPE            0x0006U /* C23 auto type used           */
#define DS_TYPE_MASK            0x0006U /* Mask for type of spec decl   */
#define DS_EXTRA_TYPE           0x0008U /* ESU type in declaration      */
#define DS_NEW_TYPE_DECL        0x0010U /* New type declared            */
#define DS_NEW_TYPE_DEF         0x0020U /* New type defined             */
#define DS_NEW_TYPE             (DS_NEW_TYPE_DECL | DS_NEW_TYPE_DEF)
#define DS_EXPLICIT_SIGNEDNESS  0x0040U /* Signedness specified         */
#define DS_NO_EMPTY_DECL        0x0100U /* Disallow empty declaration   */
#define DS_ALLOW_BITFIELD       0x0200U /* Allow anonymous bit-fields   */



/* Result of ParseDeclSpec */
typedef struct DeclSpec DeclSpec;
struct DeclSpec {
    unsigned    StorageClass;           /* One of the SC_xxx flags      */
    Type        Type[MAXTYPELEN];       /* Type of the declaration spec */
    unsigned    Flags;                  /* Bitmapped flags              */
};

/* Result of ParseDecl */
typedef struct Declarator Declarator;
struct Declarator {
    unsigned    StorageClass;           /* A set of SC_xxx flags */
    Type        Type[MAXTYPELEN];       /* The type */
    ident       Ident;                  /* The identifier, if any*/
    Collection* Attributes;             /* Attributes if any */

    /* Working variables */
    unsigned    Index;              /* Used to build Type */
};

/* Modes for ParseDecl:
**  - DM_IDENT_OR_EMPTY means:
**      we *may* have an identifier, or none. If it is the latter case,
**      the type specifier must be used for an empty declaration,
**      or it is an error.
**  - DM_NO_IDENT means:
**      we must have a type but no variable identifer
**      (if there is one, it's not read).
**      Note: this is used for type names.
**  - DM_ACCEPT_PARAM_IDENT means:
**      we *may* have an identifier. If there is an identifier,
**      it is read, but it is no error, if there is none.
**      Note: this is used for function parameter type lists.
*/
typedef enum {
    DM_IDENT_OR_EMPTY,
    DM_NO_IDENT,
    DM_ACCEPT_PARAM_IDENT,
} declmode_t;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



Type* ParseType (Type* Type);
/* Parse a complete type specification in parentheses */

int ParseDecl (DeclSpec* Spec, Declarator* D, declmode_t Mode);
/* Parse a variable, type or function declarator. Return -1 if this stops at
** an unpaired right parenthesis/bracket/curly brace. Return 0 if this stops
** after consuming a semicolon or closing curly brace, or reaching an EOF.
** Return 1 otherwise.
*/

void ParseDeclSpec (DeclSpec* Spec, typespec_t TSFlags, unsigned DefStorage);
/* Parse a declaration specification */

void CheckEmptyDecl (const DeclSpec* Spec);
/* Called after an empty type declaration (that is, a type declaration without
** a variable). Checks if the declaration does really make sense and issues a
** warning if not.
*/



/* End of declare.h */

#endif
