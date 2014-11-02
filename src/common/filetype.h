/*****************************************************************************/
/*                                                                           */
/*                                filetype.h                                 */
/*                                                                           */
/*                       Determine the type of a file                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003      Ullrich von Bassewitz                                       */
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



#ifndef FILETYPE_H
#define FILETYPE_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* File types */
typedef enum {
    FILETYPE_UNKNOWN = -1,      /* Unknown file type */
    FILETYPE_C,                 /* C source file */
    FILETYPE_ASM,               /* Assembler file */
    FILETYPE_OBJ,               /* Object file */
    FILETYPE_LIB,               /* Library file */
    FILETYPE_GR,                /* GEOS resource file */
    FILETYPE_O65                /* O65 object file */
} FILETYPE;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



FILETYPE GetFileType (const char* Name);
/* Determine the type of the given file by looking at the name. If the file
** type could not be determined, the function returns FILETYPE_UNKOWN.
*/



/* End of filetype.h */

#endif
