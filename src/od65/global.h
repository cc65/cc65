/*****************************************************************************/
/*                                                                           */
/*                                 global.h                                  */
/*                                                                           */
/*          Global variables for the od65 object file dump utility           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2011, Ullrich von Bassewitz                                      */
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



#ifndef GLOBAL_H
#define GLOBAL_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



#define D_HEADER        0x0001U         /* Dump the header */
#define D_OPTIONS       0x0002U         /* Dump the options */
#define D_FILES         0x0004U         /* Dump source file info */
#define D_SEGMENTS      0x0008U         /* Dump segment info */
#define D_IMPORTS       0x0010U         /* Dump imported symbols */
#define D_EXPORTS       0x0020U         /* Dump exported symbols */
#define D_DBGSYMS       0x0040U         /* Dump debug symbols */
#define D_LINEINFO      0x0080U         /* Dump line infos */
#define D_SCOPES        0x0100U         /* Dump scopes */
#define D_SEGSIZE       0x0200U         /* Dump segment sizes */
#define D_ALL           0xFFFFU         /* Dump anything */



extern unsigned         What;           /* What should get dumped? */



/* End of global.h */

#endif
