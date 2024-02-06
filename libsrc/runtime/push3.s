;
; Ullrich von Bassewitz, 26.10.2000
;
; CC65 runtime: Push (int)3 onto the stack
;

        .export         push3
        .import         pusha0

; Beware: The optimizer knows about this function!

.proc   push3

        lda     #3
        jmp     pusha0

.endproc


