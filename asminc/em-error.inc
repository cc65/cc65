;/*****************************************************************************/
;/*                                                                           */
;/*                               em-error.inc                                */
;/*                                                                           */
;/*                              EM error codes                               */
;/*                                                                           */
;/*                                                                           */
;/*                                                                           */
;/* (C) 2002-2012, Ullrich von Bassewitz                                      */
;/*                Roemerstrasse 52                                           */
;/*                D-70794 Filderstadt                                        */
;/* EMail:         uz@cc65.org                                                */
;/*                                                                           */
;/*                                                                           */
;/* This software is provided 'as-is', without any expressed or implied       */
;/* warranty.  In no event will the authors be held liable for any damages    */
;/* arising from the use of this software.                                    */
;/*                                                                           */
;/* Permission is granted to anyone to use this software for any purpose,     */
;/* including commercial applications, and to alter it and redistribute it    */
;/* freely, subject to the following restrictions:                            */
;/*                                                                           */
;/* 1. The origin of this software must not be misrepresented; you must not   */
;/*    claim that you wrote the original software. If you use this software   */
;/*    in a product, an acknowledgment in the product documentation would be  */
;/*    appreciated but is not required.                                       */
;/* 2. Altered source versions must be plainly marked as such, and must not   */
;/*    be misrepresented as being the original software.                      */
;/* 3. This notice may not be removed or altered from any source              */
;/*    distribution.                                                          */
;/*                                                                           */
;/*****************************************************************************/



; Error constants
.enum
        EM_ERR_OK                       ; No error
        EM_ERR_NO_DRIVER                ; No driver available
        EM_ERR_CANNOT_LOAD              ; Error loading driver
        EM_ERR_INV_DRIVER               ; Invalid driver
        EM_ERR_NO_DEVICE                ; Device (hardware) not found
        EM_ERR_INSTALLED                ; A driver is already installed

        EM_ERR_COUNT                    ; Special: Number of error messages
.endenum



