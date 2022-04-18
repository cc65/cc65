;*****************************************************************************/
;*                                                                           */
;*                            tgi-vectorfont.inc                             */
;*                                                                           */
;*                        TGI vector font definitions                        */
;*                                                                           */
;*                                                                           */
;*                                                                           */
;* (C) 2009,      Ullrich von Bassewitz                                      */
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



;------------------------------------------------------------------------------
; Vectorfont constants

TGI_VF_VERSION          = $00                   ; File version number
TGI_VF_FIRSTCHAR        = $20                   ; First char in file
TGI_VF_LASTCHAR         = $7E                   ; Last char in file
TGI_VF_CCOUNT           = (TGI_VF_LASTCHAR - TGI_VF_FIRSTCHAR + 1)

;------------------------------------------------------------------------------
; TCH file header and font data structures

; TCH file header
.struct TGI_VF_HDR
        MAGIC           .byte   3               ; "TCH"
        VERSION         .byte   1               ; Version number
        SIZE            .word   1               ; Font data size
.endstruct

; Font data loaded directly from file
.struct TGI_VECTORFONT
        TOP             .byte                   ; Height of char
        BOTTOM          .byte                   ; Descender
        HEIGHT          .byte                   ; Maximum char height
        WIDTHS          .byte   ::TGI_VF_CCOUNT ; Char widths
        CHARS           .word   ::TGI_VF_CCOUNT ; Pointer to character defs
        OPS             .byte                   ; Actually dynamic
.endstruct

;------------------------------------------------------------------------------
; C callable functions

        .global _tgi_vectorchar



