;
; Ullrich von Bassewitz, 08.08.1998
;
; CC65 runtime: save ax into temp storage/restore ax from temp storage
;

        .export         saveeax, resteax
        .importzp       sreg, regsave

saveeax:
        sta     regsave
        stx     regsave+1
        lda     sreg
        sta     regsave+2
        lda     sreg+1
        sta     regsave+3
        lda     regsave
        rts

resteax:
        lda     regsave+3
        sta     sreg+1
        lda     regsave+2
        sta     sreg
        ldx     regsave+1
        lda     regsave
        rts

