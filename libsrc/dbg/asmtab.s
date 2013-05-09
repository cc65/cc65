;
; Ullrich von Bassewitz, 07.08.1998
;
; Tables needed for the line assembler/disassembler.
;

        .export         OffsetTab
        .export         AdrFlagTab
        .export         SymbolTab1, SymbolTab2
        .export         MnemoTab1, MnemoTab2

; -------------------------------------------------------------------------
; Assembler tables

.rodata


OffsetTab:
        .byte   $40,$02,$45,$03,$D0,$08,$40,$09
        .byte   $30,$22,$45,$33,$D0,$08,$40,$09
        .byte   $40,$02,$45,$33,$D0,$08,$40,$09
        .byte   $40,$02,$45,$B3,$D0,$08,$40,$09
        .byte   $00,$22,$44,$33,$D0,$8C,$44,$00
        .byte   $11,$22,$44,$33,$D0,$8C,$44,$9A
        .byte   $10,$22,$44,$33,$D0,$08,$40,$09
        .byte   $10,$22,$44,$33,$D0,$08,$40,$09
        .byte   $62,$13,$78,$A9

AdrFlagTab:
        .byte   $00,$21,$81,$82,$00,$00,$59,$4D
        .byte   $91,$92,$86,$4A,$85,$9D

SymbolTab1:
        .byte   $2C,$29,$2C,$23,$28,$24

SymbolTab2:
        .byte   $59,$00,$58,$24,$24,$00

MnemoTab1:
        .byte   $1C,$8A,$1C,$23,$5D,$8B,$1B,$A1
        .byte   $9D,$8A,$1D,$23,$9D,$8B,$1D,$A1
        .byte   $00,$29,$19,$AE,$69,$A8,$19,$23
        .byte   $24,$53,$1B,$23,$24,$53,$19,$A1
        .byte   $00,$1A,$5B,$5B,$A5,$69,$24,$24
        .byte   $AE,$AE,$A8,$AD,$29,$00,$7C,$00
        .byte   $15,$9C,$6D,$9C,$A5,$69,$29,$53
        .byte   $84,$13,$34,$11,$A5,$69,$23,$A0

MnemoTab2:
        .byte   $D8,$62,$5A,$48,$26,$62,$94,$88
        .byte   $54,$44,$C8,$54,$68,$44,$E8,$94
        .byte   $00,$B4,$08,$84,$74,$B4,$28,$6E
        .byte   $74,$F4,$CC,$4A,$72,$F2,$A4,$8A
        .byte   $00,$AA,$A2,$A2,$74,$74,$74,$72
        .byte   $44,$68,$B2,$32,$B2,$00,$22,$00
        .byte   $1A,$1A,$26,$26,$72,$72,$88,$C8
        .byte   $C4,$CA,$26,$48,$44,$44,$A2,$C8


   

