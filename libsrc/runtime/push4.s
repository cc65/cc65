;
; Ullrich von Bassewitz, 26.10.2000
;
; CC65 runtime: Push (int)4 onto the stack
;

        .export         push4
        .import         pusha0

; Beware: The optimizer knows about this function!

.proc   push4

        lda     #4
        jmp     pusha0

.endproc


