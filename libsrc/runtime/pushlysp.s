;
; Ullrich von Bassewitz, 22.06.1998
;
; CC65 runtime: Push a long from somewhere in the stack
;


        .export         pushlysp
        .import         pusheax
        .importzp       sreg, sp

.proc   pushlysp

        iny
        iny
        lda     (sp),y
        iny
        sta     sreg
        lda     (sp),y
        sta     sreg+1
        dey
        dey
        lda     (sp),y
        dey
        tax
        lda     (sp),y
        jmp     pusheax

.endproc


