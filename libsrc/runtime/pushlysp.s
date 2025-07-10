;
; Ullrich von Bassewitz, 22.06.1998
;
; CC65 runtime: Push a long from somewhere in the stack
;


        .export         pushlysp
        .import         pusheax
        .importzp       sreg, c_sp

.proc   pushlysp

        iny
        iny
        lda     (c_sp),y
        iny
        sta     sreg
        lda     (c_sp),y
        sta     sreg+1
        dey
        dey
        lda     (c_sp),y
        dey
        tax
        lda     (c_sp),y
        jmp     pusheax

.endproc


