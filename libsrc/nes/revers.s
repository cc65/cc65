;
; Ullrich von Bassewitz, 07.08.1998
; Rewritten by Brandon Woodward, 16.9.2026
;
; unsigned char revers (unsigned char onoff);
;

        .export         _revers

        .include        "nes.inc"

.proc   _revers
        ldx     RVS
        cpx     #$80            ; Old revers in carry
        tax
        beq     @z
        lda     #$80
@z:     sta     RVS
        rol     a               ; Load old revers
        rts

.endproc
