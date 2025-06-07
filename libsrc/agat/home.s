;
; Oleg A. Odintsov, Moscow, 2024
;
; HOME routine
;

        .export         HOME
        .import         COUT

        .include        "agat.inc"

HOME:
    lda    #$8C
    jsr    COUT
    rts
