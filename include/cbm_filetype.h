/*****************************************************************************/
/*                                                                           */
/*                              cbm_filetype.h                               */
/*                                                                           */
/*                      Definitions for CBM file types                       */
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



#ifndef _CBM_FILETYPE_H
#define _CBM_FILETYPE_H



/* Check for errors */
#if !defined(__CBM__)
#  error This module may be used only when compiling for CBM machines!
#endif



/*****************************************************************************/
/*                                Definitions                                */
/*****************************************************************************/



/* CBM FILE TYPES. The definitions are used within standard headers, so we
** be careful with identifiers in the user name space.
** "Regular" files have a special bit set so it's easier to pick them out.
*/
#define _CBM_T_REG      0x10U   /* Bit set for regular files */
#define _CBM_T_SEQ      0x10U
#define _CBM_T_PRG      0x11U
#define _CBM_T_USR      0x12U
#define _CBM_T_REL      0x13U
#define _CBM_T_VRP      0x14U   /* Vorpal fast-loadable format */
#define _CBM_T_DEL      0x00U
#define _CBM_T_CBM      0x01U   /* 1581 sub-partition */
#define _CBM_T_DIR      0x02U   /* IDE64 and CMD sub-directory */
#define _CBM_T_LNK      0x03U   /* IDE64 soft-link */
#define _CBM_T_OTHER    0x04U   /* File-type not recognized */
#define _CBM_T_HEADER   0x05U   /* Disk header / title */

#if __CC65_STD__ == __CC65_STD_CC65__
/* Allow for names without leading underscores */
#define CBM_T_DEL       _CBM_T_DEL
#define CBM_T_SEQ       _CBM_T_SEQ
#define CBM_T_PRG       _CBM_T_PRG
#define CBM_T_USR       _CBM_T_USR
#define CBM_T_REL       _CBM_T_REL
#define CBM_T_CBM       _CBM_T_CBM
#define CBM_T_DIR       _CBM_T_DIR
#define CBM_T_LNK       _CBM_T_LNK
#define CBM_T_VRP       _CBM_T_VRP
#define CBM_T_OTHER     _CBM_T_OTHER
#define CBM_T_HEADER    _CBM_T_HEADER
#endif



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



unsigned char __fastcall__ _cbm_filetype (unsigned char c);
/* Map the start character for a file type to one of the file types above.
** Note: 'd' will always mapped to CBM_T_DEL. The calling function has to
** look at the following character to determine if the file type is actually
** CBM_T_DIR.
** This is a function used by the implementation. There is usually no need
** to call it from user code.
*/



/* End of cbm_filetype.h */
#endif


