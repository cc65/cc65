;
; Ullrich von Bassewitz, 31.08.1998
;
; CC65 runtime: Load a indirect from address in ax
;

        .export         ldaidx
        .importzp       ptr1

.proc   ldaidx
        sta     ptr1
        stx     ptr1+1
        ldx     #$00
        lda     (ptr1),y
        bpl     @L1
        dex
@L1:    rts
.endproc

