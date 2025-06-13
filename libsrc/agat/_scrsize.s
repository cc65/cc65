;
; Ullrich von Bassewitz, 26.10.2000
; Konstantin Fedorov, 12.06.2025
;
; Screen size variables
;

        .export         screensize

        .include        "agat.inc"

screensize:
        lda    WNDWDTH
        bit    TATTR
        bmi    t64
        lsr
t64:
        tax
        lda    WNDBTM
        sec
        sbc    WNDTOP
        tay
        rts
