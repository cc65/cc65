;/*****************************************************************************/
;/*                                                                           */
;/* cbm_screen_charmap.inc                                                    */
;/*                                                                           */
;/* c Copyright 2019, Gerhard W. Gruber (sparhawk@gmx.at)                     */
;/*                                                                           */
;/* When using CBM mode, this include converts character literals             */
;/* from ASCII to screen-code mapping, so you can write directly              */
;/* to the screen memory.                                                     */
;/*                                                                           */
;/* If this include is used, no additional macros are needed.                 */
;/*                                                                           */
;/*****************************************************************************/

; Char $00 -> c + 128
.charmap $00, $80

; Char $01 ... $1A -> c + 128 + 64 control alphabet
.charmap $01, $C1
.charmap $02, $C2
.charmap $03, $C3
.charmap $04, $C4
.charmap $05, $C5
.charmap $06, $C6
.charmap $07, $C7
.charmap $08, $C8
.charmap $09, $C9
.charmap $0A, $CA
.charmap $0B, $CB
.charmap $0C, $CC
.charmap $0D, $CD
.charmap $0E, $CE
.charmap $0F, $CF
.charmap $10, $D0
.charmap $11, $D1
.charmap $12, $D2
.charmap $13, $D3
.charmap $14, $D4
.charmap $15, $D5
.charmap $16, $D6
.charmap $17, $D7
.charmap $18, $D8
.charmap $19, $D9
.charmap $1A, $DA

; Char $1B ... $1F -> c + 128
.charmap $1B, $9B
.charmap $1C, $9C
.charmap $1D, $9D
.charmap $1E, $9E
.charmap $1F, $9F

; Char $20 ... $3F -> c
.charmap $20, $20
.charmap $21, $21
.charmap $22, $22
.charmap $23, $23
.charmap $24, $24
.charmap $25, $25
.charmap $26, $26
.charmap $27, $27
.charmap $28, $28
.charmap $29, $29
.charmap $2A, $2A
.charmap $2B, $2B
.charmap $2C, $2C
.charmap $2D, $2D
.charmap $2E, $2E
.charmap $2F, $2F
.charmap $30, $30
.charmap $31, $31
.charmap $32, $32
.charmap $33, $33
.charmap $34, $34
.charmap $35, $35
.charmap $36, $36
.charmap $37, $37
.charmap $38, $38
.charmap $39, $39
.charmap $3A, $3A
.charmap $3B, $3B
.charmap $3C, $3C
.charmap $3D, $3D
.charmap $3E, $3E
.charmap $3F, $3F

; Char $40 -> c - 64
.charmap $40, $00

; Char $41 ... $5A -> c upper-case alphabet
.charmap $41, $41
.charmap $42, $42
.charmap $43, $43
.charmap $44, $44
.charmap $45, $45
.charmap $46, $46
.charmap $47, $47
.charmap $48, $48
.charmap $49, $49
.charmap $4A, $4A
.charmap $4B, $4B
.charmap $4C, $4C
.charmap $4D, $4D
.charmap $4E, $4E
.charmap $4F, $4F
.charmap $50, $50
.charmap $51, $51
.charmap $52, $52
.charmap $53, $53
.charmap $54, $54
.charmap $55, $55
.charmap $56, $56
.charmap $57, $57
.charmap $58, $58
.charmap $59, $59
.charmap $5A, $5A

; Char $5B ... $5F -> c - 64
.charmap $5B, $1B
.charmap $5C, $1C
.charmap $5D, $1D
.charmap $5E, $1E
.charmap $5F, $1F

; Char $60 -> c - 32
.charmap $60, $40

; Char $61 ... $7A -> c - 32 - 64 lower-case alphabet
.charmap $61, $01
.charmap $62, $02
.charmap $63, $03
.charmap $64, $04
.charmap $65, $05
.charmap $66, $06
.charmap $67, $07
.charmap $68, $08
.charmap $69, $09
.charmap $6A, $0A
.charmap $6B, $0B
.charmap $6C, $0C
.charmap $6D, $0D
.charmap $6E, $0E
.charmap $6F, $0F
.charmap $70, $10
.charmap $71, $11
.charmap $72, $12
.charmap $73, $13
.charmap $74, $14
.charmap $75, $15
.charmap $76, $16
.charmap $77, $17
.charmap $78, $18
.charmap $79, $19
.charmap $7A, $1A

; Char $7B ... $7F -> c - 32
.charmap $7B, $5B
.charmap $7C, $5C
.charmap $7D, $5D
.charmap $7E, $5E
.charmap $7F, $5F

; Char $80 -> c + 64
.charmap $80, $C0

; Char $81 ... $9A -> c control alphabet
.charmap $81, $81
.charmap $82, $82
.charmap $83, $83
.charmap $84, $84
.charmap $85, $85
.charmap $86, $86
.charmap $87, $87
.charmap $88, $88
.charmap $89, $89
.charmap $8A, $8A
.charmap $8B, $8B
.charmap $8C, $8C
.charmap $8D, $8D
.charmap $8E, $8E
.charmap $8F, $8F
.charmap $90, $90
.charmap $91, $91
.charmap $92, $92
.charmap $93, $93
.charmap $94, $94
.charmap $95, $95
.charmap $96, $96
.charmap $97, $97
.charmap $98, $98
.charmap $99, $99
.charmap $9A, $9A

; Char $9B ... $9F -> c + 64
.charmap $9B, $DB
.charmap $9C, $DC
.charmap $9D, $DD
.charmap $9E, $DE
.charmap $9F, $DF

; Char $A0 ... $BF -> c - 64
.charmap $A0, $60
.charmap $A1, $61
.charmap $A2, $62
.charmap $A3, $63
.charmap $A4, $64
.charmap $A5, $65
.charmap $A6, $66
.charmap $A7, $67
.charmap $A8, $68
.charmap $A9, $69
.charmap $AA, $6A
.charmap $AB, $6B
.charmap $AC, $6C
.charmap $AD, $6D
.charmap $AE, $6E
.charmap $AF, $6F
.charmap $B0, $70
.charmap $B1, $71
.charmap $B2, $72
.charmap $B3, $73
.charmap $B4, $74
.charmap $B5, $75
.charmap $B6, $76
.charmap $B7, $77
.charmap $B8, $78
.charmap $B9, $79
.charmap $BA, $7A
.charmap $BB, $7B
.charmap $BC, $7C
.charmap $BD, $7D
.charmap $BE, $7E
.charmap $BF, $7F

; Char $C0 ... $DF -> c - 128
.charmap $C0, $40

; Char $C1 ... $DA -> c - 128 - 64 lower-case alphabet
.charmap $C1, $01
.charmap $C2, $02
.charmap $C3, $03
.charmap $C4, $04
.charmap $C5, $05
.charmap $C6, $06
.charmap $C7, $07
.charmap $C8, $08
.charmap $C9, $09
.charmap $CA, $0A
.charmap $CB, $0B
.charmap $CC, $0C
.charmap $CD, $0D
.charmap $CE, $0E
.charmap $CF, $0F
.charmap $D0, $10
.charmap $D1, $11
.charmap $D2, $12
.charmap $D3, $13
.charmap $D4, $14
.charmap $D5, $15
.charmap $D6, $16
.charmap $D7, $17
.charmap $D8, $18
.charmap $D9, $19
.charmap $DA, $1A

; Char $DB ... $DF -> c - 128
.charmap $DB, $5B
.charmap $DC, $5C
.charmap $DD, $5D
.charmap $DE, $5E
.charmap $DF, $5F

; Char $E0 ... $FF -> c - 128
.charmap $E0, $60
.charmap $E1, $61
.charmap $E2, $62
.charmap $E3, $63
.charmap $E4, $64
.charmap $E5, $65
.charmap $E6, $66
.charmap $E7, $67
.charmap $E8, $68
.charmap $E9, $69
.charmap $EA, $6A
.charmap $EB, $6B
.charmap $EC, $6C
.charmap $ED, $6D
.charmap $EE, $6E
.charmap $EF, $6F
.charmap $F0, $70
.charmap $F1, $71
.charmap $F2, $72
.charmap $F3, $73
.charmap $F4, $74
.charmap $F5, $75
.charmap $F6, $76
.charmap $F7, $77
.charmap $F8, $78
.charmap $F9, $79
.charmap $FA, $7A
.charmap $FB, $7B
.charmap $FC, $7C
.charmap $FD, $7D
.charmap $FE, $7E
.charmap $FF, $7F
