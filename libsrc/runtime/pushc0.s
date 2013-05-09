;
; Ullrich von Bassewitz, 26.10.2000
;
; CC65 runtime: Push (char)0 onto the stack
;

        .export         pushc0
        .import         pusha

; Beware: The optimizer knows about this function!

.proc   pushc0

        lda     #0
        jmp     pusha

.endproc


