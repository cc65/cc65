;*
;* void cclearxy (unsigned char x, unsigned char y, unsigned char length);
;* void cclear (unsigned char length);
;*

        .export     _cclearxy, _cclear
        .import     popa, _gotoxy, cputdirect
        .importzp   tmp1

_cclearxy:
        pha             ; Save length
        jsr     popa    ; get Y
        jsr     _gotoxy
        pla

_cclear:
        cmp     #0      ; Zero length?
        beq     L2
        sta     tmp1

L1:     lda     #$20    ; Space
        jsr     cputdirect
        dec     tmp1
        bne     L1

L2:     rts
