/*****************************************************************************/
/*                                                                           */
/*                                  lz4.h                                    */
/*                                                                           */
/*              Decompression routine for the 'lz4' format                   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2017 Mega Cat Studios                                                 */
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



#ifndef _LZ4_H
#define _LZ4_H

void __fastcall__ decompress_lz4 (const unsigned char* src, unsigned char* const dst,
                                  const unsigned short uncompressed_size);
/* Decompresses the source buffer into the destination buffer.
** The size of the decompressed data must be known in advance, LZ4
** does not include any terminator in-stream.
*/

/* end of lz4.h */
#endif
