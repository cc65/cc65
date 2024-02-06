; Maciej 'YTM/Elysium' Witkowiak
; 06.06.2002

; This is the source for the main VLIR-structured program part

    ; include some GEOS defines

            .include "../../../libsrc/geos-common/const.inc"
            .include "../../../libsrc/geos-cbm/jumptab.inc"
            .include "../../../libsrc/geos-cbm/geossym.inc"
            .include "../../../libsrc/geos-common/geosmac.inc"

    ; import load addresses for all VLIR chains
    ; these labels are defined upon linking with ld65

            .import __OVERLAYADDR__
            .import __OVERLAYSIZE__

    ; import names of functions defined (and exported) in each VLIR part
    ; of your application
    ; here I used an OVERLAYx_ prefix to prevent name clashes

            .import OVERLAY1_Function1
            .import OVERLAY2_Function1

    ; segments "STARTUP", "CODE", "DATA", "RODATA" and "BSS" all go to VLIR0 chain

            .segment "STARTUP"
            ; code segment for VLIR 0 chain
ProgExec:
                LoadW r0, paramString           ; show something
                jsr DoDlgBox

                MoveW dirEntryBuf+OFF_DE_TR_SC, r1
                LoadW r4, fileHeader
                jsr GetBlock                    ; load back VLIR t&s table
                bnex error

                lda #1
                jsr PointRecord                 ; we want next module (#1)
                LoadW r2, __OVERLAYSIZE__       ; length - as many bytes as we have room for
                LoadW r7, __OVERLAYADDR__       ; all VLIR segments have the same load address
                jsr ReadRecord                  ; load it
                bnex error
                jsr OVERLAY1_Function1          ; execute something

                lda #2
                jsr PointRecord                 ; next module
                LoadW r2, __OVERLAYSIZE__
                LoadW r7, __OVERLAYADDR__
                jsr ReadRecord                  ; load it
                bnex error
                jsr OVERLAY2_Function1          ; execute something

error:          jmp EnterDeskTop                ; end of application

            .segment "RODATA"
            ; read-only data segment
paramString:
            .byte DEF_DB_POS | 1
            .byte DBTXTSTR, TXT_LN_X, TXT_LN_2_Y
            .word line1
            .byte DBTXTSTR, TXT_LN_X, TXT_LN_3_Y
            .word line2
            .byte OK, DBI_X_0, DBI_Y_2
            .byte NULL

line1:      .byte BOLDON, "Hello World!",0
line2:      .byte OUTLINEON,"Hello",PLAINTEXT," world!",0

            .segment "DATA"
            ; read/write initialized data segment
counter:    .word 0

            .segment "BSS"
            ; read/write uninitialized data segment
            ; this space doesn't go into output file, only its size and
            ; position is remembered
