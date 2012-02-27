/*****************************************************************************/
/*                                                                           */
/*                                 output.h                                  */
/*                                                                           */
/*   Output format/file definitions for the sp65 sprite and bitmap utility   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2012,      Ullrich von Bassewitz                                      */
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



#ifndef OUTPUT_H
#define OUTPUT_H



/* common */
#include "strbuf.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



enum OutputFormat {
    ofAuto      = -1,           /* Auto detect */
    ofAsm,                      /* Output assembler source */
    ofBin,                      /* Output raw binary format */

    ofCount                     /* Number of output formats without ofAuto */
};
typedef enum OutputFormat OutputFormat;




/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void WriteOutputFile (const char* Name, const StrBuf* Data, OutputFormat Format);
/* Write the contents of Data to the given file in the format specified. If
 * the format is ofAuto, it is determined by the file extension.
 */



/* End of output.h */

#endif



