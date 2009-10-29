;
; Sinus table
;
; Ullrich von Bassewitz, 2009-10-29
;

        .export         _cc65_sintab


; ---------------------------------------------------------------------------
;

.rodata

_cc65_sintab:
        .byte   $00, $04, $09, $0D, $12, $16, $1B, $1F, $24, $28
        .byte   $2C, $31, $35, $3A, $3E, $42, $47, $4B, $4F, $53
        .byte   $58, $5C, $60, $64, $68, $6C, $70, $74, $78, $7C
        .byte   $80, $84, $88, $8B, $8F, $93, $96, $9A, $9E, $A1
        .byte   $A5, $A8, $AB, $AF, $B2, $B5, $B8, $BB, $BE, $C1
        .byte   $C4, $C7, $CA, $CC, $CF, $D2, $D4, $D7, $D9, $DB
        .byte   $DE, $E0, $E2, $E4, $E6, $E8, $EA, $EC, $ED, $EF
        .byte   $F1, $F2, $F3, $F5, $F6, $F7, $F8, $F9, $FA, $FB
        .byte   $FC, $FD, $FE, $FE, $FF, $FF, $FF
