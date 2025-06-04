;
; Ullrich von Bassewitz, 22.06.1998
;
; CC65 runtime: Load a long from somewhere in the stack
;
; Beware: The optimizer knows about the register contents after calling
;         this function!
;


        .export         ldeax0sp, ldeaxysp
        .importzp       sreg, spc

ldeax0sp:
        ldy     #3
ldeaxysp:
        lda     (spc),y
        sta     sreg+1
        dey
        lda     (spc),y
        sta     sreg
        dey
        lda     (spc),y
        tax
        dey
        lda     (spc),y
        rts

