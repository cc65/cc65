;*****************************************************************************/
;*                                                                           */
;*                                tgi-kernel.inc                             */
;*                                                                           */
;*                             TGI kernel interface                          */
;*                                                                           */
;*                                                                           */
;*                                                                           */
;* (C) 2002-2003 Ullrich von Bassewitz                                       */
;*               Römerstrasse 52                                             */
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



;------------------------------------------------------------------------------
; Offsets into the driver header

TGI_HDR_ID              = 0             ; Contains 0x74, 0x67, 0x69 ("tgi")
TGI_HDR_VERSION         = 3             ; Interface version
TGI_HDR_XRES            = 4             ; X resolution
TGI_HDR_YRES            = 6             ; Y resolution
TGI_HDR_COLORCOUNT      = 8             ; Number of available colors
TGI_HDR_PAGECOUNT       = 9             ; Number of screens available
TGI_HDR_FONTSIZE_X      = 10            ; System font size in X direction
TGI_HDR_FONTSIZE_Y      = 11            ; System font size in Y direction
TGI_HDR_RES             = 12            ; Reserved for extensions

TGI_HDR_JUMPTAB         = 16
TGI_HDR_INSTALL         = TGI_HDR_JUMPTAB+0     ; INSTALL routine
TGI_HDR_UNINSTALL       = TGI_HDR_JUMPTAB+2     ; UNINSTALL routine
TGI_HDR_INIT            = TGI_HDR_JUMPTAB+4     ; INIT routine
TGI_HDR_DONE            = TGI_HDR_JUMPTAB+6     ; DONE routine
TGI_HDR_GETERROR	= TGI_HDR_JUMPTAB+8 	; GETERROR routine
TGI_HDR_CONTROL         = TGI_HDR_JUMPTAB+10    ; CONTROL routine
TGI_HDR_CLEAR           = TGI_HDR_JUMPTAB+12    ; CLEAR routine
TGI_HDR_SETVIEWPAGE     = TGI_HDR_JUMPTAB+14    ; SETVIEWPAGE routine
TGI_HDR_SETDRAWPAGE     = TGI_HDR_JUMPTAB+16    ; SETDRAWPAGE routine
TGI_HDR_SETCOLOR        = TGI_HDR_JUMPTAB+18    ; SETCOLOR routine
TGI_HDR_SETPALETTE      = TGI_HDR_JUMPTAB+20    ; SETPALETTE routine
TGI_HDR_GETPALETTE      = TGI_HDR_JUMPTAB+22    ; GETPALETTE routine
TGI_HDR_GETDEFPALETTE   = TGI_HDR_JUMPTAB+24    ; GETDEFPALETTE routine
TGI_HDR_SETPIXEL        = TGI_HDR_JUMPTAB+26    ; SETPIXEL routine
TGI_HDR_GETPIXEL        = TGI_HDR_JUMPTAB+28    ; GETPIXEL routine
TGI_HDR_HORLINE         = TGI_HDR_JUMPTAB+30    ; HORLINE routine
TGI_HDR_LINE            = TGI_HDR_JUMPTAB+32    ; LINE routine
TGI_HDR_BAR             = TGI_HDR_JUMPTAB+34    ; BAR routine
TGI_HDR_CIRCLE          = TGI_HDR_JUMPTAB+36    ; CIRCLE routine
TGI_HDR_TEXTSTYLE       = TGI_HDR_JUMPTAB+39    ; TEXTSTYLE routine
TGI_HDR_OUTTEXT         = TGI_HDR_JUMPTAB+42    ; OUTTEXT routine

TGI_HDR_JUMPCOUNT       = 21            ; Number of jump vectors

;------------------------------------------------------------------------------
; The TGI API version, stored at TGI_HDR_VERSION

TGI_API_VERSION         = $01

;------------------------------------------------------------------------------
; Text style constants

TGI_TEXT_HORIZONTAL     = 0
TGI_TEXT_VERTICAL       = 1

;------------------------------------------------------------------------------
; Variables

	.global	_tgi_drv      		; Pointer to driver
	.global	_tgi_error    		; Last error code
        .global _tgi_gmode              ; Flag: graphics mode active
        .global _tgi_curx               ; Current drawing cursor X
        .global _tgi_cury               ; Current drawing cursor Y
        .global _tgi_color              ; Current drawing color
        .global _tgi_textdir            ; Current text direction
        .global _tgi_textmagx           ; Text magnification in X dir
        .global _tgi_textmagy           ; Text magnification in Y dir
        .global _tgi_xres               ; X resolution of the current mode
        .global _tgi_yres               ; Y resolution of the current mode
        .global _tgi_colorcount         ; Number of available colors
        .global _tgi_pagecount          ; Number of available screen pages
        .global _tgi_fontsizex          ; System font X size
        .global _tgi_fontsizey          ; System font Y size

;------------------------------------------------------------------------------
; Driver entry points

        .global tgi_install
        .global tgi_uninstall
        .global tgi_init
        .global tgi_done
	.global	tgi_geterror
        .global tgi_control
        .global tgi_clear
        .global tgi_setviewpage
        .global tgi_setdrawpage
        .global tgi_setcolor
        .global tgi_setpalette
        .global tgi_getpalette
        .global tgi_getdefpalette
        .global tgi_setpixel
        .global tgi_getpixel
        .global tgi_horline
        .global tgi_line
        .global tgi_bar
        .global tgi_circle
        .global tgi_textstyle
        .global tgi_outtext

;------------------------------------------------------------------------------
; ASM functions

        .global tgi_emu_bar
        .global tgi_getset
        .global tgi_inv_arg
        .global tgi_inv_drv
        .global tgi_linepop
        .global tgi_set_ptr
        .global tgi_popxy
        .global tgi_popxy2
        .global tgi_curtoxy

;------------------------------------------------------------------------------
; C callable functions

        .global _tgi_load
        .global _tgi_load_driver
        .global _tgi_unload
        .global _tgi_install
        .global _tgi_uninstall
        .global _tgi_init
        .global _tgi_done
        .global _tgi_geterror
        .global _tgi_clear
        .global _tgi_getpagecount
        .global _tgi_setviewpage
        .global _tgi_setdrawpage
        .global _tgi_getcolorcount
        .global _tgi_getmaxcolor
        .global _tgi_setcolor
        .global _tgi_getcolor
        .global _tgi_setpalette
        .global _tgi_getpalette
        .global _tgi_getdefpalette
        .global _tgi_getxres
        .global _tgi_getmaxx
        .global _tgi_getyres
        .global _tgi_getmaxy
        .global _tgi_getpixel
        .global _tgi_setpixel
        .global _tgi_gotoxy
        .global _tgi_line
        .global _tgi_lineto
        .global _tgi_circle
        .global _tgi_bar
        .global _tgi_textstyle
        .global _tgi_textwidth
        .global _tgi_textheight
        .global _tgi_outtext
        .global _tgi_outtextxy



