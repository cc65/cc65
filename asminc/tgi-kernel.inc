;*****************************************************************************/
;*                                                                           */
;*                                tgi-kernel.inc                             */
;*                                                                           */
;*                             TGI kernel interface                          */
;*                                                                           */
;*                                                                           */
;*                                                                           */
;* (C) 2002-2004 Ullrich von Bassewitz                                       */
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



;------------------------------------------------------------------------------
; The driver header

.struct TGI_HDR
        ID                  .byte   3   ; Contains 0x74, 0x67, 0x69 ("tgi")
        VERSION             .byte   1   ; Interface version
        VARS                .struct
            XRES            .word   1   ; X resolution
            YRES            .word   1   ; Y resolution
            COLORCOUNT      .byte   1   ; Number of available colors
            PAGECOUNT       .byte   1   ; Number of screens available
            FONTSIZE_X      .byte   1   ; System font size in X direction
            FONTSIZE_Y      .byte   1   ; System font size in Y direction
        .endstruct
        RESERVED            .byte   4   ; Reserved for extensions
        JUMPTAB             .struct
            INSTALL         .addr       ; INSTALL routine
            UNINSTALL       .addr       ; UNINSTALL routine
            INIT            .addr       ; INIT routine
            DONE            .addr       ; DONE routine
            GETERROR   	    .addr       ; GETERROR routine
            CONTROL         .addr       ; CONTROL routine
            CLEAR           .addr       ; CLEAR routine
            SETVIEWPAGE     .addr       ; SETVIEWPAGE routine
            SETDRAWPAGE     .addr       ; SETDRAWPAGE routine
            SETCOLOR        .addr       ; SETCOLOR routine
            SETPALETTE      .addr       ; SETPALETTE routine
            GETPALETTE      .addr       ; GETPALETTE routine
            GETDEFPALETTE   .addr       ; GETDEFPALETTE routine
            SETPIXEL        .addr       ; SETPIXEL routine
            GETPIXEL        .addr       ; GETPIXEL routine
            LINE            .addr       ; LINE routine
            BAR             .addr       ; BAR routine
            CIRCLE          .addr       ; CIRCLE routine
            TEXTSTYLE       .addr       ; TEXTSTYLE routine
            OUTTEXT         .addr       ; OUTTEXT routine
            IRQ             .addr       ; IRQ routine
        .endstruct
.endstruct

;------------------------------------------------------------------------------
; The TGI API version, stored at TGI_HDR_VERSION

TGI_API_VERSION         = $02

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
        .global tgi_line
        .global tgi_bar
        .global tgi_circle
        .global tgi_textstyle
        .global tgi_outtext

;------------------------------------------------------------------------------
; ASM functions

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
        .global _tgi_ioctl
        .global _tgi_done
        .global _tgi_geterror
        .global _tgi_geterrormsg
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


