; Maciej 'YTM/Elysium' Witkowiak
; 06.06.2002

; This is the source for the loadable VLIR-structured program part

    ; include some GEOS defines

            .include "../../../libsrc/geos-common/const.inc"
            .include "../../../libsrc/geos-cbm/jumptab.inc"
            .include "../../../libsrc/geos-cbm/geossym.inc"
            .include "../../../libsrc/geos-common/geosmac.inc"

    ; export names of functions that will be used in the main program

            .export OVERLAY1_Function1
            .export OVERLAY1_Function2


    ; go into OVERLAY1 segment - everything that is here will go into
    ; VLIR chain #1

                .segment "OVERLAY1"

OVERLAY1_Function1:             jmp Function1   ; jump table, not really necessary
OVERLAY1_Function2:             jmp Function2
                ; etc.

    ; rodata - if this is defined in .segment "RODATA"
    ; it will end up in the VLIR0 part, you don't want that
paramString:
                .byte DEF_DB_POS | 1
                .byte DBTXTSTR, TXT_LN_X, TXT_LN_2_Y
                .word line1
                .byte DBTXTSTR, TXT_LN_X, TXT_LN_3_Y
                .word line2
                .byte OK, DBI_X_0, DBI_Y_2
                .byte NULL

line1:          .byte "This is in module 1",0
line2:          .byte "This is in module 1",0

    ; code
Function1:      LoadW r0, paramString
                jsr DoDlgBox
Function2:      rts
