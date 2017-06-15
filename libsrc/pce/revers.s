
        .include        "pce.inc"
        .include        "extzp.inc"

        .export _revers

.proc   _revers

        ldx     #$00            ; Assume revers off
        tay                     ; Test onoff
        beq     L1              ; Jump if off
        ldx     #$80            ; Load on value
        ldy     #$00            ; Assume old value is zero
L1:     lda     RVS             ; Load old value
        stx     RVS             ; Set new value
        beq     L2              ; Jump if old value zero
        iny                     ; Make old value = 1
L2:     ldx     #$00            ; Load high byte of result
        tya                     ; Load low byte, set CC
        rts

.endproc

;-------------------------------------------------------------------------------
; force the init constructor to be imported

        .import         initconio
conio_init      = initconio
