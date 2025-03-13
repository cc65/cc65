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
        sec                     ; Assume main memory
        bit     RD80VID         ; In 80 column mode?
        bpl     peek            ; No, just go ahead
        lda     OURCH
        lsr                     ; Div by 2
        tay
        bcs     peek            ; Odd cols are in main memory
        php
        sei                     ; No valid MSLOT et al. in aux memory
        bit     HISCR           ; Assume SET80COL
        .endif
peek:   lda     (BASL),Y        ; Get character
        .ifdef  __APPLE2ENH__
        bcs     :+              ; In main memory
        bit     LOWSCR
        plp
:       .endif
        eor     #$80            ; Invert high bit
        ldx     #>$0000
        rts
