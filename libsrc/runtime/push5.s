;
; Ullrich von Bassewitz, 26.10.2000
;
; CC65 runtime: Push (int)5 onto the stack
;

        .export         push5
        .import         pusha0

; Beware: The optimizer knows about this function!

.proc   push5

        lda     #5
        jmp     pusha0

.endproc


