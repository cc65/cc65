/*****************************************************************************/
/*                                                                           */
/*                                  attr.c                                   */
/*                                                                           */
/*                          Command line attributes                          */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2012,      Ullrich von Bassewitz                                      */
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



#include <stdio.h>
#include <string.h>

/* common */
#include "chartype.h"
#include "strbuf.h"
#include "xmalloc.h"

/* sp65 */
#include "attr.h"
#include "error.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static int IsNumber (const char* Value)
/* Check if Value is an integer number */
{
    if (*Value == '-' || *Value == '+') {
        ++Value;
    }
    while (IsDigit (*Value)) {
        ++Value;
    }
    return (*Value == '\0');
}



Attr* NewAttr (const char* Name, const char* Value)
/* Create a new attribute */
{
    /* Determine the length of Value */
    unsigned Len = strlen (Value);

    /* Allocate memory */
    Attr* A = xmalloc (sizeof (Attr) + Len);

    /* Initialize the fields */
    A->Flags = IsNumber (Value)? afInt : afNone;
    A->Name  = xstrdup (Name);
    memcpy (A->Value, Value, Len + 1);

    /* Return the new struct */
    return A;
}



void DumpAttrColl (const Collection* C)
/* Dump a collection of attribute/value pairs for debugging */
{
    unsigned I;
    for (I = 0; I < CollCount (C); ++I) {
        const Attr* A = CollConstAt (C, I);
        printf ("%s=%s\n", A->Name, A->Value);
    }
}



int FindAttr (const Collection* C, const char* Name, unsigned* Index)
/* Search for an attribute with the given name in the collection. If it is
 * found, the function returns true and Index contains the index of the
 * entry. If Name isn't found, the function returns false and Index
 * will contain the insert position.
 */
{
    /* Do a binary search */
    int Lo = 0;
    int Hi = (int) CollCount (C) - 1;
    while (Lo <= Hi) {

        /* Mid of range */
        int Cur = (Lo + Hi) / 2;

        /* Get item */
        const Attr* A = CollAt (C, Cur);

        /* Compare */
        int Res = strcmp (A->Name, Name);

        /* Found? */
        if (Res < 0) {
            Lo = Cur + 1;
        } else if (Res > 0) {
            Hi = Cur - 1;
        } else {
            /* Found! */
            *Index = Cur;
            return 1;
        }
    }

    /* Pass back the insert position */
    *Index = Lo;
    return 0;
}



const Attr* GetAttr (const Collection* C, const char* Name)
/* Search for an attribute with the given name and return it. The function
 * returns NULL if the attribute wasn't found.
 */
{
    /* Search for the attribute and return it */
    unsigned Index;
    if (FindAttr (C, Name, &Index)) {
        return CollConstAt (C, Index);
    } else {
        /* Not found */
        return 0;
    }
}



const Attr* NeedAttr (const Collection* C, const char* Name, const char* Context)
/* Search for an attribute with the given name and return it. If the attribute
 * is not found, the function terminates with an error using Context as
 * additional context in the error message.
 */
{
    /* Search for the attribute and return it */
    unsigned Index;
    if (!FindAttr (C, Name, &Index)) {
        Error ("Found no attribute named `%s' in %s", Name, Context);
    }
    return CollConstAt (C, Index);
}



const char* GetAttrVal (const Collection* C, const char* Name)
/* Search for an attribute with the given name and return its value. The
 * function returns NULL if the attribute wasn't found.
 */
{
    const Attr* A = GetAttr (C, Name);
    return (A == 0)? 0 : A->Value;
}



const char* NeedAttrVal (const Collection* C, const char* Name, const char* Context)
/* Search for an attribute with the given name and return its value. If the
 * attribute wasn't not found, the function terminates with an error using
 * Context as additional context in the error message.
 */
{
    const Attr* A = NeedAttr (C, Name, Context);
    return (A == 0)? 0 : A->Value;
}



void AddAttr (Collection* C, const char* Name, const char* Value)
/* Add an attribute to an alphabetically sorted attribute collection */
{
    /* Create a new attribute entry */
    Attr* A = NewAttr (Name, Value);

    /* Search for the attribute. If it is there, we have a duplicate, otherwise
     * we have the insert position.
     */
    unsigned Index;
    if (FindAttr (C, Name, &Index)) {
        Error ("Duplicate command line attribute `%s'", Name);
    }

    /* Insert the attribute */
    CollInsert (C, A, Index);
}



void SplitAddAttr (Collection* C, const char* Combined, const char* Name)
/* Split a combined name/value pair and add it as an attribute to C. Some
 * attributes may not need a name. If the name is missing, use Name. If
 * Name is NULL, terminate with an error.
 */
{
    /* Name and value are separated by an equal sign */
    const char* Pos = strchr (Combined, '=');
    if (Pos == 0) {
        /* Combined is actually a value */
        if (Name == 0) {
            Error ("Command line attribute `%s' doesn't contain a name", Combined);
        }
        AddAttr (C, Name, Combined);
    } else {
        /* Must split name and value */
        StrBuf N = AUTO_STRBUF_INITIALIZER;
        SB_CopyBuf (&N, Combined, Pos - Combined);
        SB_Terminate (&N);

        /* Add the attribute */
        AddAttr (C, SB_GetConstBuf (&N), Pos+1);

        /* Release memory */
        SB_Done (&N);
    }
}



Collection* ParseAttrList (const char* List, const char** NameList, unsigned NameCount)
/* Parse a list containing name/value pairs into a sorted collection. Some
 * attributes may not need a name, so NameList contains these names. If there
 * were no errors, the function returns a alphabetically sorted collection
 * containing Attr entries.
 */
{
    const char* Name;

    /* Create a new collection */
    Collection* C = NewCollection ();

    /* Name/value pairs are separated by commas */
    const char* L = List;
    StrBuf B = AUTO_STRBUF_INITIALIZER;
    while (1) {
        if (*L == ',' || *L == '\0') {

            /* Terminate the string */
            SB_Terminate (&B);

            /* Determine the default name */
            if (CollCount (C) >= NameCount) {
                Name = 0;
            } else {
                Name = NameList[CollCount (C)];
            }

            /* Split and add this attribute/value pair */
            SplitAddAttr (C, SB_GetConstBuf (&B), Name);

            /* Done, clear the buffer. */
            SB_Clear (&B);
            if (*L == '\0') {
                break;
            }
        } else {
            SB_AppendChar (&B, *L);
        }
        ++L;
    }

    /* Free memory */
    SB_Done (&B);

    /* Return the collection with the attributes */
    return C;
}



