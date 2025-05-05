;
; 2020-07-12, Oliver Schmidt
;
; char cpeekc (void);
;

        .ifndef __APPLE2ENH__
        .import         has_80cols_card
        .endif

        .export         _cpeekc

        .include        "apple2.inc"

_cpeekc:
        ldy     CH

        sec                     ; Assume main memory

        .ifndef __APPLE2ENH__
        bit     has_80cols_card
        bpl     peek
        .endif

        bit     RD80VID         ; In 80 column mode?
        bpl     peek            ; No, just go ahead
        lda     OURCH
        lsr                     ; Div by 2
        tay
        bcs     peek            ; Odd cols are in main memory
        php
        sei                     ; No valid MSLOT et al. in aux memory
        bit     HISCR           ; Assume SET80COL

peek:   lda     (BASL),Y        ; Get character
        bcs     :+              ; In main memory
        bit     LOWSCR
        plp

:       eor     #$80            ; Invert high bit
        ldx     #>$0000
        rts
