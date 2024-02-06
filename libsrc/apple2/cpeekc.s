;
; 2020-07-12, Oliver Schmidt
;
; char cpeekc (void);
;

        .export         _cpeekc

        .include        "apple2.inc"

_cpeekc:
        ldy     CH
        .ifdef  __APPLE2ENH__
        bit     RD80VID         ; In 80 column mode?
        bpl     peek            ; No, just go ahead
        tya
        lsr                     ; Div by 2
        tay
        bcs     peek            ; Odd cols are in main memory
        bit     HISCR           ; Assume SET80COL
        .endif
peek:   lda     (BASL),Y        ; Get character
        .ifdef  __APPLE2ENH__
        bit     LOWSCR          ; Doesn't hurt in 40 column mode
        .endif
        eor     #$80            ; Invert high bit
        ldx     #$00
        rts
