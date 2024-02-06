;
; Ullrich von Bassewitz, 26.10.2000
;
; CC65 runtime: Push (char)1 onto the stack
;

        .export         pushc1
        .import         pusha

; Beware: The optimizer knows about this function!

.proc   pushc1

        lda     #1
        jmp     pusha

.endproc


