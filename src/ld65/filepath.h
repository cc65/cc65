/*****************************************************************************/
/*                                                                           */
/*                                 filepath.h                                */
/*                                                                           */
/*                    File search path handling for ld65                     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003-2013, Ullrich von Bassewitz                                      */
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



#ifndef FILEPATH_H
#define FILEPATH_H



/* common */
#include "searchpath.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



extern SearchPaths*     LibSearchPath;          /* Library path */
extern SearchPaths*     ObjSearchPath;          /* Object file path */
extern SearchPaths*     CfgSearchPath;          /* Config file path */

extern SearchPaths*     LibDefaultPath;         /* Default Library path */
extern SearchPaths*     ObjDefaultPath;         /* Default Object file path */
extern SearchPaths*     CfgDefaultPath;         /* Default Config file path */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void InitSearchPaths (void);
/* Initialize the path search list */



/* End of filepath.h */

#endif
