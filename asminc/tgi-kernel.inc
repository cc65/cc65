;*****************************************************************************/
;*                                                                           */
;*                                tgi-kernel.inc                             */
;*                                                                           */
;*                             TGI kernel interface                          */
;*                                                                           */
;*                                                                           */
;*                                                                           */
;* (C) 2002      Ullrich von Bassewitz                                       */
;*               Wacholderweg 14                                             */
;*               D-70597 Stuttgart                                           */
;* EMail:        uz@musoftware.de                                            */
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



; Offsets into the driver header
TGI_HDR_ID              = 0             ; Contains 0x74, 0x67, 0x69 ("tgi")
TGI_HDR_VERSION         = 3             ; Interface version
TGI_HDR_XRES            = 4             ; X resolution
TGI_HDR_YRES            = 6             ; Y resolution
TGI_HDR_COLORS          = 8             ; Number of available colors
TGI_HDR_RES             = 9             ; Reserved for extensions

TGI_HDR_INSTALL         = 16            ; INSTALL routine
TGI_HDR_DEINSTALL       = 18            ; DEINSTALL routine
TGI_HDR_INIT            = 20            ; INIT routine
TGI_HDR_POST            = 22            ; POST routine
TGI_HDR_CONTROL         = 24            ; CONTROL routine
TGI_HDR_CLEAR           = 26            ; CLEAR routine
TGI_HDR_SETCOLOR        = 28            ; SETCOLOR routine
TGI_HDR_SETPIXEL        = 30            ; SETPIXEL routine
TGI_HDR_GETPIXEL        = 32            ; GETPIXEL routine
TGI_HDR_LINE            = 34            ; LINE routine
TGI_HDR_BAR             = 36            ; BAR routine
TGI_HDR_CIRCLE          = 38            ; CIRCLE routine


