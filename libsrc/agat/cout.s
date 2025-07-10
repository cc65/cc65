;
; Oleg A. Odintsov, Moscow, 2024
;
; COUT routine
;

    .export     COUT
    .include    "agat.inc"

COUT:
        cmp    #$10
        bpl    out
        ora    #$80
out:    jmp    (VCOUT)
