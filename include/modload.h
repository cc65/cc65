/*****************************************************************************/
/*                                                                           */
/*                                   modload.h                               */
/*                                                                           */
/*                     o65 module loader interface for cc65                  */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002      Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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



/* Exports structures and functions to load relocatable o65 modules at
** runtime.
*/



#ifndef _MODLOAD_H
#define _MODLOAD_H



/* The following struct is passed to the module loader. It contains stuff,
** the loader needs to work, and another area where the loader will place
** informational data if it was successful. You will have to check the return
** code of mod_load before accessing any of these additional struct members.
*/
struct mod_ctrl {
    /* Parameters passed into the loader routine. The member callerdata
    ** is an opaque 16 bit datatype that may be used by the caller to
    ** pass data through to the read routine. The read routine is used by the
    ** loader to load any required data. There are several calls where the
    ** read routine is passed a count of 1, so you may choose to make this
    ** a special case when implementing read(). The read() should return the
    ** number of bytes actually read. If the return value differs from the
    ** passed count, this is considered an error.
    ** NOTE: read() is designed so that the POSIX read() routine can be used
    ** for this vector, if you're loading from disk.
    */
    int __fastcall__  (*read) (int callerdata, void* buffer, unsigned count);
    int               callerdata;

    /* Parameters set by the loader routine */
    void*             module;           /* Pointer to module data */
    unsigned          module_size;      /* Total size of loaded module */
    unsigned          module_id;        /* Module id */
};



unsigned char __fastcall__ mod_load (struct mod_ctrl* ctrl);
/* Load a module into memory and relocate it. The function will return an
** error code (see below). If MLOAD_OK is returned, the outgoing fields in
** the passed mod_ctrl struct contain information about the module just
** loaded.
*/

void __fastcall__ mod_free (void* module);
/* Free a loaded module. Note: The given pointer is the pointer to the
** module memory, not a pointer to a control structure.
*/



/* Errors */
#define MLOAD_OK                0       /* Module load successful */
#define MLOAD_ERR_READ          1       /* Read error */
#define MLOAD_ERR_HDR           2       /* Header error */
#define MLOAD_ERR_OS            3       /* Wrong OS */
#define MLOAD_ERR_FMT           4       /* Data format error */
#define MLOAD_ERR_MEM           5       /* Not enough memory */



/* End of modload.h */
#endif



