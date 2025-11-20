/*****************************************************************************/
/*                                                                           */
/*                               mngdstrbuf.h                                */
/*                                                                           */
/*                          Managed string buffers                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2025,     Kugelfuhr                                                   */
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



#ifndef MNGDSTRBUF_H
#define MNGDSTRBUF_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Forward */
struct StrBuf;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void DoneMngdStrBufs (void);
/* Free all managed string buffers */

struct StrBuf* NewMngdStrBuf (void);
/* Get a new managed string buffer */



/* End of mngdstrbuf.h */

#endif
