; Maciej 'YTM/Elysium' Witkowiak
; 06.06.2002

; This is the source for the loadable VLIR-structured program part

; similar to vlir1.s except the fact that this is chain #2

            .include "../../../libsrc/geos-common/const.inc"
            .include "../../../libsrc/geos-cbm/jumptab.inc"
            .include "../../../libsrc/geos-cbm/geossym.inc"
            .include "../../../libsrc/geos-common/geosmac.inc"

            .export OVERLAY2_Function1
            .export OVERLAY2_Function2

                .segment "OVERLAY2"

OVERLAY2_Function1:             jmp Function1
OVERLAY2_Function2:             jmp Function2
                ; etc.

paramString:
                .byte DEF_DB_POS | 1
                .byte DBTXTSTR, TXT_LN_X, TXT_LN_2_Y
                .word line1
                .byte DBTXTSTR, TXT_LN_X, TXT_LN_3_Y
                .word line2
                .byte OK, DBI_X_0, DBI_Y_2
                .byte NULL

Function2:      LoadW r0, paramString
                jsr DoDlgBox
Function1:      rts

line1:          .byte "This is in module 2",0
line2:          .byte "This is in module 2",0
