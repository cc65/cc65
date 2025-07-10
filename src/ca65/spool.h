/*****************************************************************************/
/*                                                                           */
/*                                  spool.h                                  */
/*                                                                           */
/*              Id and message pool for the ca65 macroassembler              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003-2011, Ullrich von Bassewitz                                      */
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



#ifndef SPOOL_H
#define SPOOL_H



/* common */
#include "strpool.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* The index for an empty string */
#define EMPTY_STRING_ID         0U

/* The string pool */
extern StringPool* StrPool;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static inline unsigned GetStrBufId (const StrBuf* S)
/* Return the id of the given string buffer */
{
    return SP_Add (StrPool, S);
}

static inline unsigned GetStringId (const char* S)
/* Return the id of the given string */
{
    return SP_AddStr (StrPool, S);
}

static inline const StrBuf* GetStrBuf (unsigned Index)
/* Convert a string index into a string */
{
    return SP_Get (StrPool, Index);
}

static inline const char* GetString (unsigned Index)
/* Convert a string index into a string */
{
    return SB_GetConstBuf (SP_Get (StrPool, Index));
}

void WriteStrPool (void);
/* Write the string pool to the object file */

void InitStrPool (void);
/* Initialize the string pool */



/* End of spool.h */

#endif
