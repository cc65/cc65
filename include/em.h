/*****************************************************************************/
/*                                                                           */
/*                                   em.h                                    */
/*                                                                           */
/*                      API for extended memory access                       */
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



#ifndef _EM_H
#define _EM_H



/*****************************************************************************/
/*                                  Definitions                              */
/*****************************************************************************/



/* Size of an extended memory page */
#define EM_PAGE_SIZE          256

/* Driver constants */
#define EM_DRV_DETECT           0

/* Error codes */
#define EM_ERR_OK               0       /* No error */
#define EM_ERR_NO_DRIVER        1       /* No driver available */
#define EM_ERR_CANNOT_LOAD      2       /* Error loading driver */
#define EM_ERR_INV_DRIVER       3       /* Invalid driver */
#define EM_ERR_NO_DEVICE        4       /* Device (hardware) not found */

/* Parameters for the em_copy_... functions */
struct em_copy {
    unsigned        page;       /* Starting page to copy from or to */
    unsigned char   offs;       /* Offset into page */
    void*           buf;        /* Memory buffer to copy from or to */
    unsigned        count;      /* Number of bytes to copy */
    unsigned char   unused;     /* Make the size 8 bytes */
};



/*****************************************************************************/
/* 	      	       	      	   Functions	     			     */
/*****************************************************************************/



unsigned char __fastcall__ em_load (unsigned char driver);
/* Load the extended memory driver and return an error code. */

unsigned char __fastcall__ em_load_driver (const char* driver);
/* Load an extended memory driver and return an error code */

unsigned char em_unload (void);
/* Unload the currently loaded driver. */

unsigned em_pagecount (void);
/* Return the total number of 256 byte pages available in extended memory. */

void* __fastcall__ em_map (unsigned page);
/* Unmap the current page from memory and map a new one. The function returns
 * a pointer to the location of the page in memory.
 */

void* __fastcall__ em_mapclean (unsigned page);
/* Unmap the current page from memory and map a new one. The function returns
 * a pointer to the location of the page in memory. This function differs from
 * em_map_page() in that it will discard the contents of the currently mapped
 * page, assuming that the page has not been modified or that the modifications
 * are no longer needed, if this leads to better performance. NOTE: This does
 * NOT mean that the contents of currently mapped page are always discarded!
 */

void __fastcall__ em_copyfrom (const struct em_copy* copy_data);
/* Copy from extended into linear memory. Note: This may invalidate the
 * currently mapped page.
 */

void __fastcall__ em_copyto (const struct em_copy* copy_data);
/* Copy from linear into extended memory. Note: This may invalidate the
 * currently mapped page.
 */



/* End of em.h */
#endif



