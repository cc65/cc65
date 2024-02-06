;
; Ullrich von Bassewitz, 26.10.2000
;
; CC65 runtime: Push (char)2 onto the stack
;

        .export         pushc2
        .import         pusha

; Beware: The optimizer knows about this function!

.proc   pushc2

        lda     #2
        jmp     pusha

.endproc


