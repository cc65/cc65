;
; Ullrich von Bassewitz, 26.10.2000
;
; CC65 runtime: Push (int)2 onto the stack
;

        .export         push2
        .import         pusha0

; Beware: The optimizer knows about this function!

.proc   push2

        lda     #2
        jmp     pusha0

.endproc


