;
; Christian Groessler, June 2004
;
; this file provides a color functions similar to BASIC
;
; void __fastcall__ _setcolor     (unsigned char color_reg, unsigned char hue, unsigned char luminace);
; void __fastcall__ _setcolor_low (unsigned char color_reg, unsigned char color_value);
; unsigned char __fastcall__ _getcolor (unsigned char color_reg);
;


        .export         __setcolor, __setcolor_low, __getcolor
        .import         popa
        .include        "atari.inc"

.proc   __getcolor

        tax
        lda     COLOR0,x        ; get current value
        ldx     #0
        rts

.endproc

.proc   __setcolor

;       asl     a               ; not -> BASIC compatible
        sta     lum             ; remember luminance
        jsr     popa            ; get hue
        asl     a
        asl     a
        asl     a
        asl     a
        ora     lum
;       jmp     __setcolor_low

.endproc

.proc   __setcolor_low

        pha
        jsr     popa
        tax
        pla
        sta     COLOR0,x
        rts

.endproc

        .bss
lum:    .res    1
