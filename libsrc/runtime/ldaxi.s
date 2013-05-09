;
; Ullrich von Bassewitz, 31.08.1998
;
; CC65 runtime: Load ax indirect from address in ax
;

        .export         ldaxi, ldaxidx
        .importzp       ptr1

ldaxi:  ldy     #1
ldaxidx:
        sta     ptr1
        stx     ptr1+1
        lda     (ptr1),y
        tax
        dey
        lda     (ptr1),y
        rts

