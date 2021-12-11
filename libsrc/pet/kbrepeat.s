;
; unsigned char __fastcall__ kbrepeat (unsigned char mode);
;
; 2017-06-16, Groepaz
; 2017-09-05, Greg King
;

        .export         _kbrepeat

        .include        "pet.inc"

_kbrepeat:
        ldx     #>$0000
        ldy     SCR_LINELEN
        cpy     #40 + 1
        bcc     L1              ; branch if screen is 40 columns wide

        ldy     KBDREPEAT80     ; get old value
        sta     KBDREPEAT80     ; store new value
        tya                     ; return old value
        rts

L1:     tay
        lda     KBDREPEAT40B    ; get REPEAT-key flag (used by some editor ROMs)
        lsr     a               ; move bit 0 into bit 7
        ror     a
        ora     KBDREPEAT40     ; combine with old key-REPEAT flags
        sty     KBDREPEAT40
        rts
