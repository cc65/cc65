;
; Ullrich von Bassewitz, 22.06.1998
;
; CC65 runtime: Push a long from somewhere in the stack
;


        .export         pushlysp
        .import         pusheax
        .importzp       sreg, spc

.proc   pushlysp

        iny
        iny
        lda     (spc),y
        iny
        sta     sreg
        lda     (spc),y
        sta     sreg+1
        dey
        dey
        lda     (spc),y
        dey
        tax
        lda     (spc),y
        jmp     pusheax

.endproc


