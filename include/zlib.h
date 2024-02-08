/*****************************************************************************/
/*                                                                           */
/*                                  zlib.h                                   */
/*                                                                           */
/*              Decompression routines for the 'deflate' format              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2015 Piotr Fusik <fox@scene.pl>                                  */
/*                                                                           */
/* This file is based on the zlib.h from 'zlib' general purpose compression  */
/* library, version 1.1.3, (C) 1995-1998 Jean-loup Gailly and Mark Adler.    */
/*                                                                           */
/*  Jean-loup Gailly        Mark Adler                                       */
/*  jloup@gzip.org          madler@alumni.caltech.edu                        */
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

#define Z_OK         0
#define Z_DATA_ERROR (-3)
/* Return codes for uncompress() */

#define Z_DEFLATED   8
/* The deflate compression method (the only one supported) */

#define Z_NULL       0


unsigned __fastcall__ inflatemem (unsigned char* dest,
                                  const unsigned char* source);
/*
     Decompresses the source buffer into the destination buffer.
   Returns the size of the uncompressed data (number of bytes written starting
   from dest).

     This function expects data in the DEFLATE format, described in RFC
   (Request for Comments) 1951 in the file
   ftp://ds.internic.net/rfc/rfc1951.txt.

     This function does not exist in the original zlib. Its implementation
   using original zlib might be following:

   unsigned inflatemem (char* dest, const char* source)
   {
     z_stream stream;

     stream.next_in = (Bytef*) source;
     stream.avail_in = 65535;

     stream.next_out = dest;
     stream.avail_out = 65535;

     stream.zalloc = (alloc_func) 0;
     stream.zfree = (free_func) 0;

     inflateInit2(&stream, -MAX_WBITS);
     inflate(&stream, Z_FINISH);
     inflateEnd(&stream);

     return stream.total_out;
   }
*/


int __fastcall__ uncompress (unsigned char* dest, unsigned* destLen,
                             const unsigned char* source, unsigned sourceLen);
/*
   Original zlib description:

     Decompresses the source buffer into the destination buffer.  sourceLen is
   the byte length of the source buffer. Upon entry, destLen is the total
   size of the destination buffer, which must be large enough to hold the
   entire uncompressed data. (The size of the uncompressed data must have
   been saved previously by the compressor and transmitted to the decompressor
   by some mechanism outside the scope of this compression library.)
   Upon exit, destLen is the actual size of the compressed buffer.
     This function can be used to decompress a whole file at once if the
   input file is mmap'ed.

     uncompress returns Z_OK if success, Z_MEM_ERROR if there was not
   enough memory, Z_BUF_ERROR if there was not enough room in the output
   buffer, or Z_DATA_ERROR if the input data was corrupted.

   Implementation notes:

     This function expects data in the ZLIB format, described in RFC 1950
   in the file ftp://ds.internic.net/rfc/rfc1950.txt. The ZLIB format is
   essentially the DEFLATE format plus a very small header and Adler-32
   checksum.

     Z_MEM_ERROR and Z_BUF_ERROR are never returned in this implementation.
*/


unsigned long __fastcall__ adler32 (unsigned long adler,
                                    const unsigned char* buf,
                                    unsigned len);

/*
   Original zlib description:

     Update a running Adler-32 checksum with the bytes buf[0..len-1] and
   return the updated checksum. If buf is NULL, this function returns
   the required initial value for the checksum.
   An Adler-32 checksum is almost as reliable as a CRC32 but can be computed
   much faster. Usage example:

     unsigned long adler = adler32(0L, Z_NULL, 0);

     while (read_buffer(buffer, length) != EOF) {
       adler = adler32(adler, buffer, length);
     }
     if (adler != original_adler) error();

   Implementation notes:

     This function isn't actually much faster than crc32(), but it is smaller
   and does not use any lookup tables.
*/


unsigned long __fastcall__ crc32 (unsigned long crc,
                                  const unsigned char* buf,
                                  unsigned len);
/*
   Original zlib description:

     Update a running crc with the bytes buf[0..len-1] and return the updated
   crc. If buf is NULL, this function returns the required initial value
   for the crc. Pre- and post-conditioning (one's complement) is performed
   within this function so it shouldn't be done by the application.
   Usage example:

     unsigned long crc = crc32(0L, Z_NULL, 0);

     while (read_buffer(buffer, length) != EOF) {
       crc = crc32(crc, buffer, length);
     }
     if (crc != original_crc) error();

   Implementation notes:

     This function uses statically allocated 1 KB lookup table. The table is
   initialised before it is used for the first time (that is, if buffer is
   NULL or length is zero, then the lookup table isn't initialised).
*/


/* end of zlib.h */
#endif



