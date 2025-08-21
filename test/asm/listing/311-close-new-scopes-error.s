.scope foo
start:
        jmp bar::start
.endscope

.scope bar
start:
        rts
.endscope
