;
; Expose include-file symbol names to C code.
;

        .export         _atmos_ping, _atmos_shoot, _atmos_explode
        .export         _atmos_zap, _atmos_tick, _atmos_tock

        .include        "atmos.inc"

.proc   _atmos_ping
        bit     $31
        bvs     L1      ; Atmos?
        jmp     PING
L1:     jmp     PING1
.endproc

.proc   _atmos_shoot
        lda     $31
        bvs     L1      ; Atmos?
        jmp SHOOT
L1:     jmp SHOOT1
.endproc

.proc   _atmos_explode
        lda     $31
        bvs     L1      ; Atmos?
        jmp EXPLODE
L1:     jmp EXPLODE1
.endproc

.proc   _atmos_zap
        lda     $31
        bvs     L1      ; Atmos?
        jmp ZAP
L1:     jmp ZAP1
.endproc

.proc   _atmos_tick
        lda     $31
        bvs     L1      ; Atmos?
        jmp TICK
L1:     jmp TICK1
.endproc

.proc   _atmos_tock
        lda     $31
        bvs     L1      ; Atmos?
        jmp TOCK
L1:     jmp TOCK1
.endproc
