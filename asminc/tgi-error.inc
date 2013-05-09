;*****************************************************************************/
;*                                                                           */
;*                                 tgi-error.inc                             */
;*                                                                           */
;*                                TGI error codes                            */
;*                                                                           */
;*                                                                           */
;*                                                                           */
;* (C) 2002-2012, Ullrich von Bassewitz                                      */
;*                Roemerstrasse 52                                           */
;*                D-70794 Filderstadt                                        */
;* EMail:         uz@cc65.org                                                */
;*                                                                           */
;*                                                                           */
;* This software is provided 'as-is', without any expressed or implied       */
;* warranty.  In no event will the authors be held liable for any damages    */
;* arising from the use of this software.                                    */
;*                                                                           */
;* Permission is granted to anyone to use this software for any purpose,     */
;* including commercial applications, and to alter it and redistribute it    */
;* freely, subject to the following restrictions:                            */
;*                                                                           */
;* 1. The origin of this software must not be misrepresented; you must not   */
;*    claim that you wrote the original software. If you use this software   */
;*    in a product, an acknowledgment in the product documentation would be  */
;*    appreciated but is not required.                                       */
;* 2. Altered source versions must be plainly marked as such, and must not   */
;*    be misrepresented as being the original software.                      */
;* 3. This notice may not be removed or altered from any source              */
;*    distribution.                                                          */
;*                                                                           */
;*****************************************************************************/



; Error constants
.enum
        TGI_ERR_OK              ; No error
        TGI_ERR_NO_DRIVER       ; No driver available
        TGI_ERR_CANNOT_LOAD     ; Error loading driver or font
        TGI_ERR_INV_DRIVER      ; Invalid driver
        TGI_ERR_INV_MODE        ; Mode not supported by driver
        TGI_ERR_INV_ARG         ; Invalid function argument
        TGI_ERR_INV_FUNC        ; Function not supported
        TGI_ERR_INV_FONT        ; Font file is invalid
        TGI_ERR_NO_RES          ; Out of resources (memory, handles, ...)
        TGI_ERR_UNKNOWN         ; Unknown error
        TGI_ERR_INSTALLED       ; A driver is already installed

        TGI_ERR_COUNT           ; Special: Number of error messages
.endenum

