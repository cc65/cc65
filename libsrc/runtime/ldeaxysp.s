;
; Ullrich von Bassewitz, 22.06.1998
;
; CC65 runtime: Load a long from somewhere in the stack
;
; Beware: The optimizer knows about the register contents after calling
;         this function!
;


        .export         ldeax0sp, ldeaxysp
        .importzp       sreg, c_sp

ldeax0sp:
        ldy     #3
ldeaxysp:
        lda     (c_sp),y
        sta     sreg+1
        dey
        lda     (c_sp),y
        sta     sreg
        dey
        lda     (c_sp),y
        tax
        dey
        lda     (c_sp),y
        rts

