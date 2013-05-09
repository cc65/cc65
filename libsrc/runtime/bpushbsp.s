;
; Ullrich von Bassewitz, 31.08.1998
;
; CC65 runtime: Load a from stack slot and push as byte
;

        .export         bpushbsp, bpushbysp
        .import         pusha
        .importzp       sp

bpushbsp:
        ldy     #0
bpushbysp:
        lda     (sp),y
        jmp     pusha



