;
; Oliver Schmidt, 03.03.2007
;
; VTABZ routine
;

        .export         VTABZ

        .include        "apple2.inc"

        .segment        "LOWCODE"

VTABZ:
        ; Switch in ROM and call VTABZ
        bit     $C082
        jsr     $FC24           ; Generate text base address

        ; Switch in LC bank 2 and return
        bit     $C080
        rts
