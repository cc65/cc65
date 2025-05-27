;
; Colin Leroy-Mira, 27/05/2025
;
; Verify the presence of a 80 columns card in slot 3,
; and publish a flag accordingly.
;
        .export         aux80col

        .ifndef __APPLE2ENH__
        .import         machinetype
        .endif

        .constructor    detect80cols

        .include        "apple2.inc"

        .data

aux80col:        .byte 0

        .segment        "ONCE"

IdOfsTable:                     ; Table of bytes positions, used to check four
                                ; specific bytes on the slot's firmware to make
                                ; sure this is a serial card.
        .byte   $05             ; Pascal 1.0 ID byte
        .byte   $07             ; Pascal 1.0 ID byte
        .byte   $0B             ; Pascal 1.1 generic signature byte
        .byte   $0C             ; Device signature byte

IdValTable:                     ; Table of expected values for the four checked
                                ; bytes
        .byte   $38             ; ID Byte 0 (from Pascal 1.0), fixed
        .byte   $18             ; ID Byte 1 (from Pascal 1.0), fixed
        .byte   $01             ; Generic signature for Pascal 1.1, fixed
        .byte   $88             ; Device signature byte (80 columns card)

IdTableLen      = * - IdValTable

detect80cols:
        .ifndef __APPLE2ENH__
        lda     machinetype     ; Check we're on a //e at least, otherwise we
        bpl     NoDev           ; handle no 80cols hardware (like Videx)
        .endif

        ldx     #IdTableLen-1
:       ldy     IdOfsTable,x    ; Check Pascal 1.1 Firmware Protocol ID bytes
        lda     IdValTable,x
        cmp     $C300,y
        bne     NoDev
        dex
        bpl     :-

        dec     aux80col   ; We have an 80-columns card! Set flag to $FF

NoDev:  rts
