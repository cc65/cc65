;*****************************************************************************/
;*                                                                           */
;*                                   modfree.s                               */
;*                                                                           */
;*			    Free loaded o65 modules			     */
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



        .include        "modload.inc"

        .import         _free
        .importzp       ptr1

;------------------------------------------------------------------------------
; mod_free: Free a loaded module

.code
_mod_free:
        sta     ptr1
        stx     ptr1+1                          ; Save parameter

        ldy     #MODCTRL_MODULE + 1
        lda     (ptr1),y
        tax
        dey
        lda     (ptr1),y                        ; Load module pointer

        jmp     _free                           ; Free the module memory


