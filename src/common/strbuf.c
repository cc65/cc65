/*****************************************************************************/
/*                                                                           */
/*		     		   strbuf.c                                  */
/*                                                                           */
/*			 Variable sized string buffers                       */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2008 Ullrich von Bassewitz                                       */
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



#include <string.h>
#include <ctype.h>

/* common */
#include "chartype.h"
#include "strbuf.h"
#include "va_copy.h"
#include "xmalloc.h"
#include "xsprintf.h"



/*****************************************************************************/
/*		     		     Data                                    */
/*****************************************************************************/



/* An empty string buf */
const StrBuf EmptyStrBuf = STATIC_STRBUF_INITIALIZER;



/*****************************************************************************/
/*	      			     Code	   	     		     */
/*****************************************************************************/



StrBuf* InitStrBuf (StrBuf* B)
/* Initialize a string buffer */
{
    B->Allocated = 0;
    B->Len       = 0;
    B->Index     = 0;
    B->Buf       = 0;
    return B;
}



void DoneStrBuf (StrBuf* B)
/* Free the data of a string buffer (but not the struct itself) */
{
    xfree (B->Buf);
}



StrBuf* NewStrBuf (void)
/* Allocate, initialize and return a new StrBuf */
{
    /* Allocate a new string buffer */
    StrBuf* B = xmalloc (sizeof (StrBuf));

    /* Initialize the struct... */
    InitStrBuf (B);

    /* ...and return it */
    return B;
}



void FreeStrBuf (StrBuf* B)
/* Free a string buffer */
{
    DoneStrBuf (B);
    xfree (B);
}



void SB_Realloc (StrBuf* B, unsigned NewSize)
/* Reallocate the string buffer space, make sure at least NewSize bytes are
 * available.
 */
{
    /* Get the current size, use a minimum of 8 bytes */
    unsigned NewAllocated = B->Allocated;
    if (NewAllocated == 0) {
     	NewAllocated = 8;
    }

    /* Round up to the next power of two */
    while (NewAllocated < NewSize) {
     	NewAllocated *= 2;
    }

    /* Reallocate the buffer */
    B->Buf       = xrealloc (B->Buf, NewAllocated);
    B->Allocated = NewAllocated;
}



#if !defined(HAVE_INLINE)
char SB_At (const StrBuf* B, unsigned Index)
/* Get a character from the buffer */
{
    PRECONDITION (Index < B->Len);
    return B->Buf[Index];
}
#endif



void SB_Drop (StrBuf* B, unsigned Count)
/* Drop characters from the end of the string. */
{
    PRECONDITION (Count <= B->Len);
    B->Len -= Count;
    if (B->Index > B->Len) {
        B->Index = B->Len;
    }
}



void SB_Terminate (StrBuf* B)
/* Zero terminate the given string buffer. NOTE: The terminating zero is not
 * accounted for in B->Len, if you want that, you have to use AppendChar!
 */
{
    unsigned NewLen = B->Len + 1;
    if (NewLen > B->Allocated) {
	SB_Realloc (B, NewLen);
    }
    B->Buf[B->Len] = '\0';
}



void SB_CopyBuf (StrBuf* Target, const char* Buf, unsigned Size)
/* Copy Buf to Target, discarding the old contents of Target */
{
    if (Target->Allocated < Size) {
	SB_Realloc (Target, Size);
    }
    memcpy (Target->Buf, Buf, Size);
    Target->Len = Size;
}



#if !defined(HAVE_INLINE)
void SB_CopyStr (StrBuf* Target, const char* S)
/* Copy S to Target, discarding the old contents of Target */
{
    SB_CopyBuf (Target, S, strlen (S));
}
#endif



#if !defined(HAVE_INLINE)
void SB_Copy (StrBuf* Target, const StrBuf* Source)
/* Copy Source to Target, discarding the old contents of Target */
{
    SB_CopyBuf (Target, Source->Buf, Source->Len);
    Target->Index = Source->Index;
}
#endif



void SB_AppendChar (StrBuf* B, int C)
/* Append a character to a string buffer */
{
    unsigned NewLen = B->Len + 1;
    if (NewLen > B->Allocated) {
	SB_Realloc (B, NewLen);
    }
    B->Buf[B->Len] = (char) C;
    B->Len = NewLen;
}



void SB_AppendBuf (StrBuf* B, const char* S, unsigned Size)
/* Append a character buffer to the end of the string buffer */
{
    unsigned NewLen = B->Len + Size;
    if (NewLen > B->Allocated) {
	SB_Realloc (B, NewLen);
    }
    memcpy (B->Buf + B->Len, S, Size);
    B->Len = NewLen;
}



#if !defined(HAVE_INLINE)
void SB_AppendStr (StrBuf* B, const char* S)
/* Append a string to the end of the string buffer */
{
    SB_AppendBuf (B, S, strlen (S));
}
#endif



#if !defined(HAVE_INLINE)
void SB_Append (StrBuf* Target, const StrBuf* Source)
/* Append the contents of Source to Target */
{
    SB_AppendBuf (Target, Source->Buf, Source->Len);
}
#endif



#if !defined(HAVE_INLINE)
void SB_Cut (StrBuf* B, unsigned Len)
/* Cut the contents of B at the given length. If the current length of the
 * buffer is smaller than Len, nothing will happen.
 */
{
    if (Len < B->Len) {
       	B->Len = Len;
    }
}
#endif



void SB_Slice (StrBuf* Target, const StrBuf* Source, unsigned Start, unsigned Len)
/* Copy a slice from Source into Target. The current contents of Target are
 * destroyed. If Start is greater than the length of Source, or if Len
 * characters aren't available, the result will be a buffer with less than Len
 * bytes.
 */
{
    /* Calculate the length of the resulting buffer */
    if (Start >= Source->Len) {
       	/* Target will be empty */
	SB_Clear (Target);
	return;
    }
    if (Start + Len > Source->Len) {
       	Len = Source->Len - Start;
    }

    /* Make sure we have enough room in the target string buffer */
    if (Len > Target->Allocated) {
	SB_Realloc (Target, Len);
    }

    /* Copy the slice */
    memcpy (Target->Buf, Source->Buf + Start, Len);
    Target->Len = Len;
}



void SB_Move (StrBuf* Target, StrBuf* Source)
/* Move the complete contents of Source to target. This will delete the old
 * contents of Target, and Source will be empty after the call.
 */
{
    /* Free the target string */
    if (Target->Buf) {
        xfree (Target->Buf);
    }

    /* Move all data from Source to Target */
    *Target = *Source;

    /* Clear Source */
    InitStrBuf (Source);
}



void SB_ToLower (StrBuf* S)
/* Convert all characters in S to lower case */
{
    unsigned I;
    char* B = S->Buf;
    for (I = 0; I < S->Len; ++I, ++B) {
        if (IsUpper (*B)) {
            *B = tolower (*B);
        }
    }
}



void SB_ToUpper (StrBuf* S)
/* Convert all characters in S to upper case */
{
    unsigned I;
    char* B = S->Buf;
    for (I = 0; I < S->Len; ++I, ++B) {
        if (IsLower (*B)) {
            *B = toupper (*B);
        }
    }
}



int SB_Compare (const StrBuf* S1, const StrBuf* S2)
/* Do a lexical compare of S1 and S2. See strcmp for result codes. */
{
    int Result;
    if (S1->Len < S2->Len) {
        Result = memcmp (S1->Buf, S2->Buf, S1->Len);
        if (Result == 0) {
            /* S1 considered lesser because it's shorter */
            Result = -1;
        }
    } else if (S1->Len > S2->Len) {
        Result = memcmp (S1->Buf, S2->Buf, S2->Len);
        if (Result == 0) {
            /* S2 considered lesser because it's shorter */
            Result = 1;
        }
    } else {
        Result = memcmp (S1->Buf, S2->Buf, S1->Len);
    }
    return Result;
}



void SB_VPrintf (StrBuf* S, const char* Format, va_list ap)
/* printf function with S as target. The function is safe, which means that
 * the current contents of S are discarded, and are allocated again with
 * a matching size for the output. The function will call FAIL when problems
 * are detected (anything that let xsnprintf return -1).
 */
{
    va_list tmp;
    int SizeNeeded;

    /* Since we must determine the space needed anyway, we will try with
     * the currently allocated memory. If the call succeeds, we've saved
     * an allocation. If not, we have to reallocate and try again.
     */
    va_copy (tmp, ap);
    SizeNeeded = xvsnprintf (S->Buf, S->Allocated, Format, tmp);
    va_end (tmp);

    /* Check the result, the xvsnprintf function should not fail */
    CHECK (SizeNeeded >= 0);

    /* Check if we must reallocate */
    if ((unsigned) SizeNeeded >= S->Allocated) {
        /* Must retry. Don't use Realloc to avoid copying */
        xfree (S->Buf);
        S->Allocated = SizeNeeded + 1;          /* Account for '\0' */
        S->Buf = xmalloc (S->Allocated);
        (void) xvsnprintf (S->Buf, S->Allocated, Format, ap);
    }

    /* Update string buffer variables */
    S->Len = SizeNeeded;
    S->Index = 0;
}



void SB_Printf (StrBuf* S, const char* Format, ...)
/* vprintf function with S as target. The function is safe, which means that
 * the current contents of S are discarded, and are allocated again with
 * a matching size for the output. The function will call FAIL when problems
 * are detected (anything that let xsnprintf return -1).
 */
{
    va_list ap;
    va_start (ap, Format);
    SB_VPrintf (S, Format, ap);
    va_end (ap);
}



