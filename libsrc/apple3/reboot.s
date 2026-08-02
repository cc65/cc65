;
; Oliver Schmidt, 14.09.2009
; Robert Justice, 2026
;
; void rebootafterexit (void);
;

        .include        "apple3.inc"

        .constructor    initreboot, 11
        .export         _rebootafterexit
        .import         done, return

_rebootafterexit := return

        .segment        "ONCE"

initreboot:
        lda     E_REG      ; get the current invironment reg
        ora     #$43       ; turn on Cxxx I/O space and Primary ROM
        sta     E_REG
        ; Quit to RESET Vector
        lda     #<$F4EE
        ldx     #>$F4EE
        sta     done+1
        stx     done+2
        rts
