/*****************************************************************************/
/*                                                                           */
/*				   strbuf.h                                  */
/*                                                                           */
/*			 Variable sized string buffers                       */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001    	 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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



#include <string.h>

/* common */
#include "attrib.h"
#include "inline.h"



/*****************************************************************************/
/*				     Data                                    */
/*****************************************************************************/



typedef struct StrBuf StrBuf;
struct StrBuf {
    unsigned    Allocated;
    unsigned    Len;
    char*       Buf;
};

/* Initializer for static string bufs */
#define STATIC_STRBUF_INITIALIZER 	{ 0, 0, 0 }



/*****************************************************************************/
/*	      			     Code    	   	     		     */
/*****************************************************************************/



StrBuf* InitStrBuf (StrBuf* B);
/* Initialize a string buffer */

void DoneStrBuf (StrBuf* B);
/* Free the data of a string buffer (but not the struct itself) */

StrBuf* NewStrBuf (void);
/* Allocate, initialize and return a new StrBuf */

void FreeStrBuf (StrBuf* B);
/* Free a string buffer */

void SB_Realloc (StrBuf* B, unsigned NewSize);
/* Reallocate the string buffer space, make sure at least NewSize bytes are
 * available. THIS IS NOT A USER CALLABLE FUNCTION!
 */

#if defined(HAVE_INLINE)
INLINE unsigned SB_GetLen (StrBuf* B)
/* Return the length of the buffer contents */
{
    return B->Len;
}
#else
#  define SB_GetLen(B)  (B)->Len
#endif

#if defined(HAVE_INLINE)
INLINE const char* SB_GetConstBuf (const StrBuf* B)
/* Return a buffer pointer */
{
    return B->Buf;
}
#else
#  define SB_GetConstBuf(B)     (B)->Buf
#endif

#if defined(HAVE_INLINE)
INLINE char* SB_GetBuf (StrBuf* B)
/* Return a buffer pointer */
{
    return B->Buf;
}
#else
#  define SB_GetBuf(B)     (B)->Buf
#endif

#if defined(HAVE_INLINE)
INLINE void SB_Clear (StrBuf* B)
/* Clear the string buffer (make it empty) */
{
    B->Len = 0;
}
#else
#  define SB_Clear(B)   ((B)->Len = 0)
#endif

void SB_Terminate (StrBuf* B);
/* Zero terminate the given string buffer. NOTE: The terminating zero is not
 * accounted for in B->Len, if you want that, you have to use AppendChar!
 */

void SB_AppendChar (StrBuf* B, char C);
/* Append a character to a string buffer */

void SB_AppendBuf (StrBuf* B, const char* S, unsigned Size);
/* Append a character buffer to the end of the string buffer */

#if defined(HAVE_INLINE)
INLINE void SB_AppendStr (StrBuf* B, const char* S)
/* Append a string to the end of the string buffer */
{
    SB_AppendBuf (B, S, strlen (S));
}
#else
#  define SB_AppendStr(B, S)    SB_AppendBuf (B, S, strlen (S))
#endif

void SB_Copy (StrBuf* Target, const StrBuf* Source);
/* Copy Source to Target, discarding the old contents of Target */

#if defined(HAVE_INLINE)
INLINE void SB_Append (StrBuf* Target, const StrBuf* Source)
/* Append the contents of Source to Target */
{
    SB_AppendBuf (Target, Source->Buf, Source->Len);
}
#else
#  define SB_Append(Target, Source)     SB_AppendBuf (Target, (Source)->Buf, (Source)->Len)
#endif

#if defined(HAVE_INLINE)
INLINE void SB_Cut (StrBuf* B, unsigned Len)
/* Cut the contents of B at the given length. If the current length of the
 * buffer is smaller than Len, nothing will happen.
 */
{
    if (Len < B->Len) {
       	B->Len = Len;
    }
}
#else
#  define SB_Cut(B, L)        if ((L) < (B)->Len) { (B)->Len = (L); }
#endif

void SB_Slice (StrBuf* Target, const StrBuf* Source, unsigned Start, unsigned Len);
/* Copy a slice from Source into Target. The current contents of Target are
 * destroyed. If Start is greater than the length of Source, or if Len
 * characters aren't available, the result will be a buffer with less than Len
 * bytes.
 */



/* End of strbuf.h */

#endif



