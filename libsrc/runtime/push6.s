;
; Ullrich von Bassewitz, 26.10.2000
;
; CC65 runtime: Push (int)6 onto the stack
;

        .export         push6
        .import         pusha0

; Beware: The optimizer knows about this function!

.proc   push6

        lda     #6
        jmp     pusha0

.endproc


