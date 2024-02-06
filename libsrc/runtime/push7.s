;
; Ullrich von Bassewitz, 26.10.2000
;
; CC65 runtime: Push (int)7 onto the stack
;

        .export         push7
        .import         pusha0

; Beware: The optimizer knows about this function!

.proc   push7

        lda     #7
        jmp     pusha0

.endproc


