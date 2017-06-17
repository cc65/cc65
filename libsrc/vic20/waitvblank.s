        .export         _waitvblank

        .include        "vic20.inc"

; FIXME
; this flag doesnt work on vic20!!!
; it will have to be filled by a get_tv() constructor or so
PALFLAG		= $2A6		; $01 = PAL, $00 = NTSC

_waitvblank:
        lda     PALFLAG
        beq     @ntsc
        ldx     #(312-8)/2
        .byte $2c
@ntsc:
        ldx     #(262-8)/2
@l2:
        cpx     VIC_HLINE
        bcs     @l2
        rts

