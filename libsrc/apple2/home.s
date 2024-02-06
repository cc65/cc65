;
; Oliver Schmidt, 03.03.2007
;
; HOME routine
;

        .export         HOME

        .include        "apple2.inc"

        .segment        "LOWCODE"

HOME:
        ; Switch in ROM and call HOME
        bit     $C082
        jsr     $FC58           ; Clear current text screen

        ; Switch in LC bank 2 for R/O and return
        bit     $C080
        rts
