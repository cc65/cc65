;
; Ullrich von Bassewitz, 11.04.1999
;
; CC65 runtime: Load an unsigned char indirect from pointer somewhere in stack
;

        .export         ldaui0sp, ldauiysp
        .importzp       sp, ptr1

ldaui0sp:
        ldy     #1
ldauiysp:
        lda     (sp),y
        sta     ptr1+1
        dey
        lda     (sp),y
        sta     ptr1
        txa
        tay
        ldx     #0
        lda     (ptr1),y
        rts

                                   
