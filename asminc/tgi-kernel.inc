;*****************************************************************************/
;*                                                                           */
;*                                tgi-kernel.inc                             */
;*                                                                           */
;*                             TGI kernel interface                          */
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



;------------------------------------------------------------------------------
; The driver header

.struct TGI_HDR
        ID                  .byte   3   ; Contains 0x74, 0x67, 0x69 ("tgi")
        VERSION             .byte   1   ; Interface version
        LIBREF              .addr       ; Library reference
        VARS                .struct
            XRES            .word   1   ; X resolution
            YRES            .word   1   ; Y resolution
            COLORCOUNT      .byte   1   ; Number of available colors
            PAGECOUNT       .byte   1   ; Number of screens available
            FONTWIDTH       .byte   1   ; System font width in pixel
            FONTHEIGHT      .byte   1   ; System font height in pixel
            ASPECTRATIO     .word   1   ; Fixed point 8.8 format
            FLAGS           .byte   1   ; TGI driver flags
        .endstruct
        JUMPTAB             .struct
            INSTALL         .addr       ; INSTALL routine
            UNINSTALL       .addr       ; UNINSTALL routine
            INIT            .addr       ; INIT routine
            DONE            .addr       ; DONE routine
            GETERROR        .addr       ; GETERROR routine
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
            TEXTSTYLE       .addr       ; TEXTSTYLE routine
            OUTTEXT         .addr       ; OUTTEXT routine
        .endstruct
.endstruct

;------------------------------------------------------------------------------
; The TGI API version, stored at TGI_HDR_VERSION

TGI_API_VERSION         = $06

;------------------------------------------------------------------------------
; Bitmapped tgi driver flags, stored in TGI_HDR::VARS::FLAGS.
; Beware: Some of the bits are tested using the BIT instruction, so do not
; change the values without checking the code!

TGI_BM_FONT_FINESCALE   = $80           ; Bitmap fonts are fine grained scalable

;------------------------------------------------------------------------------
; Text constants

TGI_FONT_BITMAP         = 0
TGI_FONT_VECTOR         = 1

TGI_TEXT_HORIZONTAL     = 0
TGI_TEXT_VERTICAL       = 1

;----------------------------------------------------------------------------
; Results of tgi_outcode

TGI_CLIP_NONE           = $00
TGI_CLIP_LEFT           = $01
TGI_CLIP_RIGHT          = $02
TGI_CLIP_BOTTOM         = $04
TGI_CLIP_TOP            = $08

;------------------------------------------------------------------------------
; ASM accessible color constants

        .global tgi_color_black:zp      ; Target-specific value for black
        .global tgi_color_white:zp      ; Target-specific value for white

;------------------------------------------------------------------------------
; C accessible variables

        .global _tgi_drv                ; Pointer to driver
        .global _tgi_error              ; Last error code
        .global _tgi_gmode              ; Flag: graphics mode active
        .global _tgi_curx               ; Current drawing cursor X
        .global _tgi_cury               ; Current drawing cursor Y
        .global _tgi_color              ; Current drawing color
        .global _tgi_font               ; Which font to use
        .global _tgi_textdir            ; Current text direction
        .global _tgi_vectorfont         ; Pointer to vector font
        .global _tgi_textscalew         ; Text magnification for the width
        .global _tgi_textscaleh         ; Text magnification for the height
        .global _tgi_charwidth          ; Width of scaled system font char
        .global _tgi_charheight         ; Height of scaled system font char
        .global _tgi_xres               ; X resolution of the current mode
        .global _tgi_yres               ; Y resolution of the current mode
        .global _tgi_xmax               ; Maximum X coordinate
        .global _tgi_ymax               ; Maximum Y coordinate
        .global _tgi_colorcount         ; Number of available colors
        .global _tgi_pagecount          ; Number of available screen pages
        .global _tgi_fontwidth          ; System font width
        .global _tgi_fontheight         ; System font height
        .global _tgi_aspectratio        ; Aspect ratio, fixed point 8.8
        .global _tgi_flags              ; TGI driver flags

;------------------------------------------------------------------------------
; ASM accessible variables

        .global tgi_clip_x1             ; Coordinate for line clipper
        .global tgi_clip_y1             ; Coordinate for line clipper
        .global tgi_clip_x2             ; Coordinate for line clipper
        .global tgi_clip_y2             ; Coordinate for line clipper

;------------------------------------------------------------------------------
; Driver entry points

        .global tgi_install
        .global tgi_uninstall
        .global tgi_init
        .global tgi_done
        .global tgi_geterror
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
        .global tgi_textstyle
        .global tgi_outtext

;------------------------------------------------------------------------------
; ASM functions

        .global tgi_clear_ptr
        .global tgi_clippedline
        .global tgi_curtoxy
        .global tgi_getset
        .global tgi_imulround
        .global tgi_inv_arg
        .global tgi_inv_drv
        .global tgi_linepop
        .global tgi_outcode
        .global tgi_popxy
        .global tgi_popxy2
        .global tgi_set_ptr

;------------------------------------------------------------------------------
; C callable functions

        .global _tgi_arc
        .global _tgi_bar
        .global _tgi_circle
        .global _tgi_clear
        .global _tgi_done
        .global _tgi_ellipse
        .global _tgi_getaspectratio
        .global _tgi_getcolor
        .global _tgi_getcolorcount
        .global _tgi_getdefpalette
        .global _tgi_geterror
        .global _tgi_geterrormsg
        .global _tgi_getmaxcolor
        .global _tgi_getmaxx
        .global _tgi_getmaxy
        .global _tgi_getpagecount
        .global _tgi_getpalette
        .global _tgi_getpixel
        .global _tgi_gettextheight
        .global _tgi_gettextwidth
        .global _tgi_getxres
        .global _tgi_getyres
        .global _tgi_gotoxy
        .global _tgi_imulround
        .global _tgi_init
        .global _tgi_install
        .global _tgi_install_vectorfont
        .global _tgi_ioctl
        .global _tgi_line
        .global _tgi_lineto
        .global _tgi_load_driver
        .global _tgi_outtext
        .global _tgi_outtextxy
        .global _tgi_pieslice
        .global _tgi_setaspectratio
        .global _tgi_setcolor
        .global _tgi_setdrawpage
        .global _tgi_setpalette
        .global _tgi_setpixel
        .global _tgi_settextdir
        .global _tgi_settextscale
        .global _tgi_settextstyle
        .global _tgi_setviewpage
        .global _tgi_uninstall
        .global _tgi_unload
