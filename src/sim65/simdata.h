/*****************************************************************************/
/*                                                                           */
/*				   simdata.h				     */
/*                                                                           */
/*		   Simulator data passed to the chip plugins		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2003 Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
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



#ifndef SIMDATA_H
#define SIMDATA_H



/*****************************************************************************/
/*                                     Data                                  */
/*****************************************************************************/



/* SimData structure */
typedef struct SimData SimData;
struct SimData {
    unsigned	MajorVersion;
    unsigned	MinorVersion;

    /* -- Callback functions -- */

    void* (*Malloc) (size_t Size);
    /* Allocate a memory block of the given size */

    void (*Free) (void* Block);
    /* Free an allocated memory block */

    void (*Warning) (const char* Format, ...);
    /* Print a warning */

    void (*Error) (const char* Format, ...);
    /* Print an error and terminate the program */

    void (*Internal) (const char* Format, ...);
    /* Print an internal program error and terminate */

    void (*Break) (const char* Format, ...);
    /* Stop the CPU and display the given message */

    int (*GetCfgId) (void* CfgInfo, const char* Name, char** Id);
    /* Search CfgInfo for an attribute with the given name and type "id". If
     * found, remove it from the configuration, pass a pointer to a dynamically
     * allocated string containing the value to Id, and return true. If not
     * found, return false. The memory passed in Id must be free by a call to
     * Free();
     */

    int (*GetCfgStr) (void* CfgInfo, const char* Name, char** S);
    /* Search CfgInfo for an attribute with the given name and type "string".
     * If found, remove it from the configuration, pass a pointer to a
     * dynamically allocated string containing the value to S, and return
     * true. If not found, return false. The memory passed in S must be free
     * by a call to Free();
     */

    int (*GetCfgNum) (void* CfgInfo, const char* Name, long* Val);
    /* Search CfgInfo for an attribute with the given name and type "number".
     * If found, remove it from the configuration, copy it into Val and return
     * true. If not found, return false.
     */
};



/* End of simdata.h */

#endif



