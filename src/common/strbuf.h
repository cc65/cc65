/*****************************************************************************/
/*                                                                           */
/*                                 strbuf.h                                  */
/*                                                                           */
/*                       Variable sized string buffers                       */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2009, Ullrich von Bassewitz                                      */
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



#ifndef STRBUF_H
#define STRBUF_H



#include <stdarg.h>
#include <string.h>

/* common */
#include "attrib.h"
#include "check.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/


/* We want to track whether a character is "raw" or not. */
/* "raw" characters should NOT be translated when translating a string. */
/* We do this by keeping a second array parallel to "Buf" called "Cooked". */
/* Think of "cooked" as the inverse of "raw". */
/* If Cooked[n] is 0, then the character is raw and should not be translated. */
/* This was done to keep LIT_STR_BUFFER sane. */

typedef struct StrBuf StrBuf;
struct StrBuf {
    char*       Buf;                    /* Pointer to buffer */
    char*       Cooked;                 /* Pointer to cooked buffer */
    unsigned    Len;                    /* Length of the string */
    unsigned    Index;                  /* Used for reading (Get and friends) */
    unsigned    Allocated;              /* Size of allocated memory */
};

/* An empty string buf */
extern const StrBuf EmptyStrBuf;

/* Initializer for static string bufs */
#define STATIC_STRBUF_INITIALIZER       { 0, 0, 0, 0, 0 }

/* Initializer for auto string bufs */
#define AUTO_STRBUF_INITIALIZER         { 0, 0, 0, 0, 0 }

/* Initialize with a string literal (beware: evaluates str twice!) */
#define LIT_STRBUF_INITIALIZER(str)     { (char*)str, (char *)str, sizeof(str)-1, 0, 0 }



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static inline StrBuf* SB_Init (StrBuf* B)
/* Initialize a string buffer */
{
    *B = EmptyStrBuf;
    return B;
}

StrBuf* SB_InitFromString (StrBuf* B, const char* S);
/* Initialize a string buffer from a literal string. Beware: The buffer won't
** store a copy but a pointer to the actual string. A buffer initialized with
** this routine may be "forgotten" without calling SB_Done, since no memory
** has been allocated.
*/

void SB_Done (StrBuf* B);
/* Free the data of a string buffer (but not the struct itself) */

StrBuf* NewStrBuf (void);
/* Allocate, initialize and return a new StrBuf */

void FreeStrBuf (StrBuf* B);
/* Free a string buffer */

void SB_Realloc (StrBuf* B, unsigned NewSize);
/* Reallocate the string buffer space, make sure at least NewSize bytes are
** available.
*/

static inline unsigned SB_GetLen (const StrBuf* B)
/* Return the length of the buffer contents */
{
    return B->Len;
}

static inline unsigned SB_GetIndex (const StrBuf* B)
/* Return the user index of the string buffer */
{
    return B->Index;
}

static inline void SB_SetIndex (StrBuf* B, unsigned Index)
/* Set the user index of the string buffer */
{
    B->Index = Index;
}

static inline const char* SB_GetConstBuf (const StrBuf* B)
/* Return a buffer pointer */
{
    return B->Buf;
}

static inline char* SB_GetBuf (StrBuf* B)
/* Return a buffer pointer */
{
    return B->Buf;
}

static inline char* SB_GetCooked (StrBuf* B)
/* Return a cooked pointer */
{
    return B->Cooked;
}

static inline char SB_At (const StrBuf* B, unsigned Index)
/* Get a character from the buffer */
{
    PRECONDITION (Index < B->Len);
    return B->Buf[Index];
}

static inline char SB_AtUnchecked (const StrBuf* B, unsigned Index)
/* Get a character from the buffer */
{
    return B->Buf[Index];
}

static inline int SB_IsEmpty (const StrBuf* B)
/* Return true if the string buffer is empty */
{
    return (B->Len == 0);
}

static inline int SB_NotEmpty (const StrBuf* B)
/* Return true if the string buffer is not empty */
{
    return (B->Len > 0);
}

static inline void SB_Clear (StrBuf* B)
/* Clear the string buffer (make it empty) */
{
    B->Len = B->Index = 0;
}

static inline void SB_Reset (StrBuf* B)
/* Reset the string buffer index to zero */
{
    B->Index = 0;
}

static inline char SB_Get (StrBuf* B)
/* Return the next character from the string incrementing Index. Returns NUL
** if the end of the string is reached.
*/
{
    return (B->Index < B->Len)? B->Buf[B->Index++] : '\0';
}

static inline char SB_Peek (const StrBuf* B)
/* Look at the next character from the string without incrementing Index.
** Returns NUL if the end of the string is reached.
*/
{
    return (B->Index < B->Len)? B->Buf[B->Index] : '\0';
}

static inline char SB_LookAt (const StrBuf* B, unsigned Index)
/* Look at a specific character from the string. Returns NUL if the given
** index is greater than the size of the string.
*/
{
    return (Index < B->Len)? B->Buf[Index] : '\0';
}

static inline char SB_LookAtLast (const StrBuf* B)
/* Look at the last character from the string. Returns NUL if the string buffer
** is empty.
*/
{
    return (B->Len > 0)? B->Buf[B->Len-1] : '\0';
}

static inline void SB_Skip (StrBuf* B)
/* Skip the next character in the string buffer if this is possible. */
{
    if (B->Index < B->Len) {
        ++B->Index;
    }
}

static inline void SB_SkipMultiple (StrBuf* B, unsigned Count)
/* Skip a number of characters in the string buffer if this is possible. */
{
    if ((B->Index += Count) > B->Len) {
        B->Index = B->Len;
    }
}

void SB_Drop (StrBuf* B, unsigned Count);
/* Drop characters from the end of the string. */

void SB_Terminate (StrBuf* B);
/* Zero terminate the given string buffer. NOTE: The terminating zero is not
** accounted for in B->Len, if you want that, you have to use AppendChar!
*/

void SB_CopyBuf (StrBuf* Target, const char* Buf, unsigned Size);
/* Copy Buf to Target, discarding the old contents of Target */

void SB_CopyBufCooked (StrBuf* Target, const char* Buf, const char *Cooked, unsigned Size);
/* Copy Buf and Cooked to Target, discarding the old contents of Target */

static inline void SB_CopyStr (StrBuf* Target, const char* S)
/* Copy S to Target, discarding the old contents of Target */
{
    SB_CopyBuf (Target, S, (unsigned)strlen (S));
}

static inline void SB_Copy (StrBuf* Target, const StrBuf* Source)
/* Copy Source to Target, discarding the old contents of Target */
{
    SB_CopyBufCooked (Target, Source->Buf, Source->Cooked, Source->Len);
    Target->Index = Source->Index;
}

void SB_AppendChar (StrBuf* B, int C);
/* Append a character to a string buffer */

void SB_AppendCharCooked (StrBuf* B, int C, int Cooked);
/* Append a character to a string buffer, raw if Cooked == 0 */

void SB_AppendBuf (StrBuf* B, const char* S, unsigned Size);
/* Append a character buffer to the end of the string buffer */

static inline void SB_AppendStr (StrBuf* B, const char* S)
/* Append a string to the end of the string buffer */
{
    SB_AppendBuf (B, S, (unsigned)strlen (S));
}

static inline void SB_Append (StrBuf* Target, const StrBuf* Source)
/* Append the contents of Source to Target */
{
    unsigned NewLen = Target->Len + Source->Len;
    if (NewLen > Target->Allocated) {
        SB_Realloc (Target, NewLen);
    }
    memcpy (Target->Buf + Target->Len, Source->Buf, Source->Len);
    memcpy (Target->Cooked + Target->Len, Source->Cooked, Source->Len);
    Target->Len = NewLen;
}

static inline void SB_Cut (StrBuf* B, unsigned Len)
/* Cut the contents of B at the given length. If the current length of the
** buffer is smaller than Len, nothing will happen.
*/
{
    if (Len < B->Len) {
        B->Len = Len;
    }
}

void SB_Slice (StrBuf* Target, const StrBuf* Source, unsigned Start, unsigned Len);
/* Copy a slice from Source into Target. The current contents of Target are
** destroyed. If Start is greater than the length of Source, or if Len
** characters aren't available, the result will be a buffer with less than Len
** bytes.
*/

void SB_Move (StrBuf* Target, StrBuf* Source);
/* Move the complete contents of Source to target. This will delete the old
** contents of Target, and Source will be empty after the call.
*/

void SB_ToLower (StrBuf* S);
/* Convert all characters in S to lower case */

void SB_ToUpper (StrBuf* S);
/* Convert all characters in S to upper case */

int SB_Compare (const StrBuf* S1, const StrBuf* S2);
/* Do a lexical compare of S1 and S2. See strcmp for result codes. */

int SB_CompareStr (const StrBuf* S1, const char* S2);
/* Do a lexical compare of S1 and S2. See strcmp for result codes. */

void SB_VPrintf (StrBuf* S, const char* Format, va_list ap) attribute ((format (printf, 2, 0)));
/* printf function with S as target. The function is safe, which means that
** the current contents of S are discarded, and are allocated again with
** a matching size for the output. The function will call FAIL when problems
** are detected (anything that let xsnprintf return -1).
*/

void SB_Printf (StrBuf* S, const char* Format, ...) attribute ((format (printf, 2, 3)));
/* vprintf function with S as target. The function is safe, which means that
** the current contents of S are discarded, and are allocated again with
** a matching size for the output. The function will call FAIL when problems
** are detected (anything that let xsnprintf return -1).
*/



/* End of strbuf.h */

#endif
