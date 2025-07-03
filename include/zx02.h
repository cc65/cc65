/*****************************************************************************/
/*                                                                           */
/*                                 zx02.h                                    */
/*                                                                           */
/*              Decompression routine for the 'zx02' format                  */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2022 DMSC                                                             */
/*                                                                           */
/*                                                                           */
/* MIT license:                                                              */
/* Permission is hereby granted, free of charge, to any person obtaining a   */
/* copy of this software and associated documentation files (the “Software”),*/
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense,  */
/* and/or sell copies of the Software, and to permit persons to whom the     */
/* Software is furnished to do so, subject to the following conditions:      */
/*                                                                           */
/* The above copyright notice and this permission notice shall be included   */
/* in all copies or substantial portions of the Software.                    */
/*                                                                           */
/* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS   */
/* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF                */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN */
/* NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,  */
/* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR     */
/* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE */
/* USE OR OTHER DEALINGS IN THE SOFTWARE.                                    */
/*****************************************************************************/



#ifndef _ZX02_H
#define _ZX02_H

void __fastcall__ decompress_zx02 (const unsigned char* src, unsigned char* const dst);
/* Decompresses the source buffer into the destination buffer.
** compress with zx02 input.bin output.zx02
 */


/* end of zx02.h */
#endif
