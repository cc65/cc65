/*****************************************************************************/
/*                                                                           */
/*                                 global.h                                  */
/*                                                                           */
/*                   Global variables for the ld65 linker                    */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2010, Ullrich von Bassewitz                                      */
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



extern const char*      OutputName;     /* Name of output file */
extern unsigned         OutputNameUsed; /* Output name was used by %O */

extern unsigned         ModuleId;       /* Id for o65 module */

extern unsigned char    HaveStartAddr;  /* True if start address was given */
extern unsigned long    StartAddr;      /* Start address */

extern unsigned char    VerboseMap;     /* Verbose map file */
extern const char*      MapFileName;    /* Name of the map file */
extern const char*      LabelFileName;  /* Name of the label file */
extern const char*      DbgFileName;    /* Name of the debug file */



/* End of global.h */

#endif
