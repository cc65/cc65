/*****************************************************************************/
/*                                                                           */
/*				    zlib.h                                   */
/*                                                                           */
/*              Decompression routines for the 'deflate' format              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2001 Piotr Fusik                                                 */
/*               a.k.a. Fox/Taquart                                          */
/* EMail:        fox@scene.pl                                                */
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



#ifndef _ZLIB_H
#define _ZLIB_H



void* inflatemem (void* dest, void* src);
/* Read the deflate compressed data starting from src and store
 * the uncompressed data starting from dest.
 * Return pointer to a byte after the decompressed data. That is, the result
 * minus dest is the size of the decompressed data.
 */



/* end of zlib.h */
#endif



