/*****************************************************************************/
/*                                                                           */
/*				   cfgdata.c				     */
/*                                                                           */
/*	     		     Config data structure			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2003 Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
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



#include <string.h>

/* common */
#include "strutil.h"
#include "xmalloc.h"

/* sim65 */
#include "error.h"
#include "scanner.h"
#include "cfgdata.h"



/*****************************************************************************/
/*                                     Code                                  */
/*****************************************************************************/



CfgData* NewCfgData (void)
/* Create and intialize a new CfgData struct, then return it. The function
 * uses the current output of the config scanner.
 */
{
    /* Get the length of the identifier */
    unsigned AttrLen = strlen (CfgSVal);

    /* Allocate memory */
    CfgData* D = xmalloc (sizeof (CfgData) + AttrLen);

    /* Initialize the fields */
    D->Type = CfgDataInvalid;
    D->Line = CfgErrorLine;
    D->Col  = CfgErrorCol;
    memcpy (D->Attr, CfgSVal, AttrLen+1);

    /* Return the new struct */
    return D;
}



void FreeCfgData (CfgData* D)
/* Free a config data structure */
{
    if (D->Type == CfgDataString) {
        /* Free the string value */
        xfree (D->V.SVal);
    }
    /* Free the structure */
    xfree (D);
}



void CfgDataCheckType (const CfgData* D, unsigned Type)
/* Check the config data type and print an error message if it has the wrong
 * type.
 */
{
    if (D->Type != Type) {
        Error ("%s(%u): Attribute `%s' has invalid type",
               CfgGetName (), D->Line, D->Attr);
    }
}



int CfgDataFind (const Collection* Attributes, const char* AttrName)
/* Find the attribute with the given name and return its index. Return -1 if
 * the attribute was not found.
 */
{
    unsigned I;

    /* Walk through the attributes checking for a "mirror" attribute */
    for (I = 0; I < CollCount (Attributes); ++I) {

        /* Get the next attribute */
        const CfgData* D = CollConstAt (Attributes, I);

        /* Compare the name */
        if (StrCaseCmp (D->Attr, AttrName) == 0) {
            /* Found */
            return I;
        }
    }

    /* Not found */
    return -1;
}



CfgData* CfgDataGetTyped (Collection* Attributes, const char* Name, unsigned Type)
/* Find the attribute with the given name and type. If found, remove it from
 * Attributes and return it. If not found or wrong type, return NULL.
 */
{
    CfgData* D;

    /* Search for the attribute */
    int I = CfgDataFind (Attributes, Name);
    if (I < 0) {
        /* Not found */
        return 0;
    }

    /* Get the attribute */
    D = CollAtUnchecked (Attributes, I);

    /* Check the type */
    if (D->Type != Type) {
        /* Wrong type. ### Warn here? */
        return 0;
    }

    /* Remove the attribute and return it */
    CollDelete (Attributes, I);
    return D;
}



int CfgDataGetId (Collection* Attributes, const char* Name, char** Id)
/* Search CfgInfo for an attribute with the given name and type "id". If
 * found, remove it from the configuration, copy it into Buf and return
 * true. If not found, return false.
 */
{
    CfgData* D = CfgDataGetTyped (Attributes, Name, CfgDataId);
    if (D == 0) {
        /* Not found or wrong type */
        return 0;
    }

    /* Use the string value and invalidate the type, so FreeCfgData won't
     * delete the string.
     */
    *Id = D->V.SVal;
    D->Type = CfgDataInvalid;

    /* Delete the config data struct */
    FreeCfgData (D);

    /* Success */
    return 1;
}



int CfgDataGetStr (Collection* Attributes, const char* Name, char** S)
/* Search CfgInfo for an attribute with the given name and type "string".
 * If found, remove it from the configuration, copy it into Buf and return
 * true. If not found, return false.
 */
{
    CfgData* D = CfgDataGetTyped (Attributes, Name, CfgDataString);
    if (D == 0) {
        /* Not found or wrong type */
        return 0;
    }

    /* Use the string value and invalidate the type, so FreeCfgData won't
     * delete the string.
     */
    *S = D->V.SVal;
    D->Type = CfgDataInvalid;

    /* Delete the config data struct */
    FreeCfgData (D);

    /* Success */
    return 1;
}



int CfgDataGetNum (Collection* Attributes, const char* Name, long* Val)
/* Search CfgInfo for an attribute with the given name and type "number".
 * If found, remove it from the configuration, copy it into Val and return
 * true. If not found, return false.
 */
{
    CfgData* D = CfgDataGetTyped (Attributes, Name, CfgDataString);
    if (D == 0) {
        /* Not found or wrong type */
        return 0;
    }

    /* Return the value to the caller */
    *Val = D->V.IVal;

    /* Delete the config data struct */
    FreeCfgData (D);

    /* Success */
    return 1;
}



