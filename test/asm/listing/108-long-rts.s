; test of long-rts promotion

.p816
.feature long_jsr_jmp_rts
.smart +
.proc farproc : far
    rts ; should be $6B (RTL) and not $60 (RTS)
.endproc
