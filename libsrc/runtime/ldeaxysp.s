;
; Ullrich von Bassewitz, 22.06.1998
;
; CC65 runtime: Load a long from somewhere in the stack
;
; Beware: The optimizer knows about the register contents after calling
;         this function!
;


        .export         ldeax0sp, ldeaxysp
        .importzp       sreg, sp

ldeax0sp:
        ldy     #3
ldeaxysp:
        lda     (sp),y
        sta     sreg+1
        dey
        lda     (sp),y
        sta     sreg
        dey
        lda     (sp),y
        tax
        dey
        lda     (sp),y
        rts

