;
; Ullrich von Bassewitz, 26.10.2000
;
; CC65 runtime: Push (int)1 onto the stack
;

        .export         push1
        .import         pusha0

; Beware: The optimizer knows about this function!

.proc   push1

        lda     #1
        jmp     pusha0

.endproc


