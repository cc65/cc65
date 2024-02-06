;
; Ullrich von Bassewitz, 31.08.1998
;
; CC65 runtime: Load a from stack slot and push as word
;

        .export         pushbsp, pushbysp
        .import         pusha0
        .importzp       sp

pushbsp:
        ldy     #0
pushbysp:
        lda     (sp),y          ; get lo byte
        jmp     pusha0          ; promote to unsigned and push


