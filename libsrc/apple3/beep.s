;
; Robert Justice, 2026
;
; void beep(void)
;

        .export         _beep

        .include        "apple3.inc"

_beep:
        lda     E_REG      ; get the current invironment reg
        ora     #$40       ; turn on Cxxx I/O space
        sta     E_REG
        bit     SPKRIII
        and     #$BF       ; turn off Cxxx I/O space
        sta     E_REG
        rts
