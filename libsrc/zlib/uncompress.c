/*
** uncompress.c
**
** Piotr Fusik, 18.11.2001
*/

#include <zlib.h>

int __fastcall__ uncompress (char* dest, unsigned* destLen,
                             const char* source, unsigned sourceLen)
{
        unsigned len;
        unsigned char* ptr;
        unsigned long csum;
        /* source[0]: Compression method and flags
            bits 0 to 3: Compression method (must be Z_DEFLATED)
            bits 4 to 7: Compression info (must be <= 7)
           source[1]: Flags
            bits 0 to 4: Check bits
            bit 5:       Preset dictionary (not supported, sorry)
            bits 6 to 7: Compression level
        */
        if ((source[0] & 0x8f) != Z_DEFLATED || source[1] & 0x20)
                return Z_DATA_ERROR;
        if ((((unsigned) source[0] << 8) | (unsigned char) source[1]) % 31)
                return Z_DATA_ERROR;
        *destLen = len = inflatemem(dest, source + 2);
        ptr = (unsigned char*) source + sourceLen - 4;
        csum = adler32(adler32(0L, Z_NULL, 0), dest, len);
        if ((unsigned char) csum != ptr[3]
         || (unsigned char) (csum >> 8) != ptr[2]
         || (unsigned char) (csum >> 16) != ptr[1]
         || (unsigned char) (csum >> 24) != ptr[0])
                return Z_DATA_ERROR;
        return Z_OK;
}
