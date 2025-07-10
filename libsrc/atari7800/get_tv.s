;
; Karri Kaksonen, 2022-03-25
;
; unsigned char get_tv (void)
;
    .include    "atari7800.inc"
    .include    "get_tv.inc"
    .export     _get_tv
    .export     _paldetected

.segment    "DATA"

_paldetected:
    .byte   $FF

; ---------------------------------------------------------------
; unsigned char get_tv (void)
; ---------------------------------------------------------------

.segment    "CODE"

.proc   _get_tv: near

.segment    "CODE"

    ldx     #$00
    lda     #$FF
    cmp     _paldetected
    bne     L8
L1: lda     MSTAT
    and     #$80
    bne     L1
L2: lda     MSTAT
    and     #$80
    beq     L2
L3: lda     MSTAT
    and     #$80
    bne     L3
    lda     #$00
    sta     M0001
    jmp     L5
L4: sta     MWSYNC
    sta     MWSYNC
    dec     M0001
L5: lda     MSTAT
    and     #$80
    beq     L4
    lda     M0001
    cmp     #$78
    bcc     L6
    lda     #TV::NTSC
    jmp     L7
L6: lda     #TV::PAL
L7: sta     _paldetected
    ldx     #$00
L8: lda     _paldetected
    rts

.segment    "BSS"

M0001:
    .res    1,$00

.endproc

