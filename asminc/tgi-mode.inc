;*****************************************************************************/
;*                                                                           */
;*                                 tgi-mode.inc                              */
;*                                                                           */
;*                             TGI mode definitions                          */
;*                                                                           */
;*                                                                           */
;*                                                                           */
;* (C) 2002-2003 Ullrich von Bassewitz                                       */
;*               Römerstraße 52                                              */
;*               D-70794 Filderstadt                                         */
;* EMail:        uz@cc65.org                                                 */
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



; Graphics modes. Zero may not be used as a mode number.
.enum
        TGI_MODE_INVALID
        TGI_MODE_320_200_2      ; 320x200,  2 colors (b/w)
        TGI_MODE_160_200_4      ; 160x200,  4 colors
        TGI_MODE_640_200_2	; 640x200,  2 colors (b/w)
        TGI_MODE_640_480_2	; 640x480,  2 colors (b/w)
        TGI_MODE_280_192_8      ; 280x192,  8 colors
        TGI_MODE_40_48_16       ; 40x48,   16 colors
.endenum


                                
