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



;------------------------------------------------------------------------------
; Offsets into the driver header

TGI_HDR_ID              = 0             ; Contains 0x74, 0x67, 0x69 ("tgi")
TGI_HDR_VERSION         = 3             ; Interface version
TGI_HDR_XRES            = 4             ; X resolution
TGI_HDR_YRES            = 6             ; Y resolution
TGI_HDR_COLORCOUNT      = 8             ; Number of available colors
TGI_HDR_PAGECOUNT       = 9             ; Number of screens available
TGI_HDR_ERROR		= 10		; Error code
TGI_HDR_RES             = 11            ; Reserved for extensions

TGI_HDR_JUMPTAB         = 16
TGI_HDR_INSTALL         = TGI_HDR_JUMPTAB+0     ; INSTALL routine
TGI_HDR_DEINSTALL       = TGI_HDR_JUMPTAB+2     ; DEINSTALL routine
TGI_HDR_INIT            = TGI_HDR_JUMPTAB+4     ; INIT routine
TGI_HDR_DONE            = TGI_HDR_JUMPTAB+6     ; DONE routine
TGI_HDR_CONTROL         = TGI_HDR_JUMPTAB+8     ; CONTROL routine
TGI_HDR_CLEAR           = TGI_HDR_JUMPTAB+10    ; CLEAR routine
TGI_HDR_SETVIEWPAGE     = TGI_HDR_JUMPTAB+12    ; SETVIEWPAGE routine
TGI_HDR_SETDRAWPAGE     = TGI_HDR_JUMPTAB+14    ; SETDRAWPAGE routine
TGI_HDR_SETCOLOR        = TGI_HDR_JUMPTAB+16    ; SETCOLOR routine
TGI_HDR_SETPIXEL        = TGI_HDR_JUMPTAB+18    ; SETPIXEL routine
TGI_HDR_GETPIXEL        = TGI_HDR_JUMPTAB+20    ; GETPIXEL routine
TGI_HDR_LINE            = TGI_HDR_JUMPTAB+22    ; LINE routine
TGI_HDR_BAR             = TGI_HDR_JUMPTAB+24    ; BAR routine
TGI_HDR_CIRCLE          = TGI_HDR_JUMPTAB+26    ; CIRCLE routine

TGI_HDR_JUMPCOUNT       = 14            ; Number of jump vectors

;------------------------------------------------------------------------------
; Variables

	.global	_tgi_drv      		; Pointer to driver
	.global	_tgi_error    		; Last error code
        .global _tgi_mode               ; Graphics mode or zero
        .global _tgi_curx               ; Current drawing cursor X
        .global _tgi_cury               ; Current drawing cursor Y
        .global _tgi_color              ; Current drawing color
        .global _tgi_xres               ; X resolution of the current mode
        .global _tgi_yres               ; Y resolution of the current mode
        .global _tgi_colorcount         ; Number of available colors
        .global _tgi_pagecount          ; Number of available screen pages

;------------------------------------------------------------------------------
; Driver entry points

        .global tgi_install
        .global tgi_deinstall
        .global tgi_init
        .global tgi_done
        .global tgi_control
        .global tgi_clear
        .global tgi_setviewpage
        .global tgi_setdrawpage
        .global tgi_setcolor
        .global tgi_setpixel
        .global tgi_getpixel
        .global tgi_line
        .global tgi_bar
        .global tgi_circle


;------------------------------------------------------------------------------
; ASM functions

        .global tgi_emu_bar
        .global tgi_fetch_error
        .global tgi_getset
        .global tgi_inv_arg
        .global tgi_linepop
        .global tgi_set_ptr

