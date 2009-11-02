;
; Ullrich von Bassewitz, 2009-10-30
;


        .include        "tgi-kernel.inc"

        .import         popa, popax


;-----------------------------------------------------------------------------
; Calculate either the total height or the total width of a bitmapped
; character, depending on the value in Y. On entry, X contains the scaling
; factor. Since it is usually small, we multiplicate by doing repeated adds.
; The function returns zero in X and the calculated value in A.

.proc   charsize_helper

        lda     _tgi_fontwidth,y
        jmp     @L2
@L1:    clc
        adc     _tgi_fontwidth,y
@L2:    dex
        bne     @L1
        sta     _tgi_charwidth,y
        rts

.endproc

;-----------------------------------------------------------------------------
; void __fastcall__ tgi_textstyle (unsigned width, unsigned height,
;                                  unsigned char dir, unsigned char font);
; /* Set the style for text output. The scaling factors for width and height
;  * are 8.8 fixed point values. This means that $100 = 1 $200 = 2 etc.
;  * dir is one of the TGI_TEXT_XXX constants. font is one of the TGI_FONT_XXX
;  * constants.
;  */
;

.proc   _tgi_textstyle

        sta     _tgi_font               ; Remember the font to use
        jsr     popa
        sta     _tgi_textdir            ; Remember the direction

; Pop the height and run directly into tgi_textscale

        jsr     popax

.endproc

;-----------------------------------------------------------------------------
; void __fastcall__ tgi_textscale (unsigned width, unsigned height);
; /* Set the scaling for text output. The scaling factors for width and height
;  * are 8.8 fixed point values. This means that $100 = 1 $200 = 2 etc.
;  */

.proc   _tgi_textscale

; The height value is in 8.8 fixed point. Store it and calculate a rounded
; value for scaling the bitmapped system font in the driver.

        sta     _tgi_textscaleh+0
        stx     _tgi_textscaleh+1
        asl     a                       ; Check value behind comma
        bcc     @L1
        inx                             ; Round
@L1:    stx     _tgi_textscaleh+2       ; Store rounded value

; Calculate the total height of the bitmapped font and remember it.

        ldy     #1
        jsr     charsize_helper

; The width value is in 8.8 fixed point. Store it and calculate a rounded
; value for scaling the bitmapped system font in the driver.

        jsr     popax                   ; height
        sta     _tgi_textscalew+0
        stx     _tgi_textscalew+1
        asl     a                       ; Check value behind comma
        bcc     @L2
        inx                             ; Round
@L2:    stx     _tgi_textscalew+2       ; Store rounded value

; Calculate the total width of the bitmapped font and remember it.

        ldy     #0
        jsr     charsize_helper

; Load values and call the driver, parameters are passed in registers

        ldx     _tgi_textscalew+2
        ldy     _tgi_textscaleh+2
        lda     _tgi_textdir
        jmp     tgi_textstyle

.endproc


