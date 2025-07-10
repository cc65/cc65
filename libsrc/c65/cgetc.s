

        .include    "cbm_kernal.inc"
        .import     cursor
        .export     _cgetc
_cgetc:

        lda cursor
        beq nocursor

        ; enable the cursor
        clc
        jsr CURSOR

nocursor:
        ; wait for a key
       ; FIXME: is $d610 mega65 specific?
:
        lda $d610
        beq :-

        jsr KBDREAD

        pha
        ; disable the cursor
        sec
        jsr CURSOR

        pla
        ldx #0
        rts

