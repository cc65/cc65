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

/* Error codes */
#define EM_ERR_OK               0       /* No error */
#define EM_ERR_NO_DRIVER        1       /* No driver available */
#define EM_ERR_CANNOT_LOAD      2       /* Error loading driver */
#define EM_ERR_INV_DRIVER       3       /* Invalid driver */
#define EM_ERR_NO_DEVICE        4       /* Device (hardware) not found */

/* Parameters for the em_copy_... functions. NOTE: The first seven bytes
 * have the same order and alignment as needed for the Commodore REU, so
 * don't change the order without changing the assembler file that defines
 * the struct offsets and the code in the REU driver.
 */
struct em_copy {
    void*           buf;        /* Memory buffer to copy from or to */
    unsigned char   offs;       /* Offset into page */
    unsigned        page;       /* Starting page to copy from or to */
    unsigned        count;      /* Number of bytes to copy */
    unsigned char   unused;     /* Make the size 8 bytes */
};



/*****************************************************************************/
/* 	      	       	      	   Functions	     			     */
/*****************************************************************************/



unsigned char __fastcall__ em_load_driver (const char* driver);
/* Load an extended memory driver and return an error code */

unsigned char __fastcall__ em_unload (void);
/* Unload the currently loaded driver. */

unsigned __fastcall__ em_pagecount (void);
/* Return the total number of 256 byte pages available in extended memory. */

void* __fastcall__ em_map (unsigned page);
/* Unmap the current page from memory and map a new one. The function returns
 * a pointer to the location of the page in memory. Note: Without calling
 * em_commit, the old contents of the memory window may be lost!
 */

void __fastcall__ em_commit (void);
/* Commit changes in the memory window to extended storage. If the contents
 * of the memory window have been changed, these changes may be lost if
 * em_map, em_copyfrom or em_copyto are called without calling em_commit
 * first. Note: Not calling em_commit does not mean that the changes are
 * discarded, it does just mean that some drivers will discard the changes.
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



