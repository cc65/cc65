;
; Ullrich von Bassewitz, 2009-20-30
;
; void __fastcall__ tgi_textstyle (unsigned magwidth, unsigned magheight,
;                                  unsigned char dir);
; /* Set the style for text output. The scaling factors for width and height
;  * are 8.8 fixed point values. This means that $100 = 1 $200 = 2 etc.
;  */


        .include        "tgi-kernel.inc"

        .import         popax


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
;

.proc   _tgi_textstyle

        sta     _tgi_textdir            ; Remember the direction

; The magheight value is in 8.8 fixed point. Store it and calculate a rounded
; value for scaling the bitmapped system font in the driver.

        jsr     popax                   ; magheight
        sta     _tgi_textmagh+0
        stx     _tgi_textmagh+1
        asl     a                       ; Check value behind comma
        bcc     @L1
        inx                             ; Round
@L1:    stx     _tgi_textmagh+2         ; Store rounded value

; Calculate the total height of the bitmapped font and remember it.

        ldy     #1
        jsr     charsize_helper

; The magwidth value is in 8.8 fixed point. Store it and calculate a rounded
; value for scaling the bitmapped system font in the driver.

        jsr     popax                   ; magheight
        sta     _tgi_textmagw+0
        stx     _tgi_textmagw+1
        asl     a                       ; Check value behind comma
        bcc     @L2
        inx                             ; Round
@L2:    stx     _tgi_textmagw+2         ; Store rounded value

; Calculate the total width of the bitmapped font and remember it.

        ldy     #0
        jsr     charsize_helper

; Load values and call the driver, parameters are passed in registers

        ldx     _tgi_textmagw+2
        ldy     _tgi_textmagh+2
        lda     _tgi_textdir
        jmp     tgi_textstyle

.endproc

