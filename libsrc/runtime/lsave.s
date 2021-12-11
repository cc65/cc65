;
; Ullrich von Bassewitz, 08.08.1998
; Christian Krueger, 11-Mar-2017, optimization
;
; CC65 runtime: save eax into temp storage/restore eax from temp storage
;

        .export         saveeax, resteax
        .importzp       sreg, regsave

saveeax:
        sta     regsave
        stx     regsave+1
        ldy     sreg
        sty     regsave+2
        ldy     sreg+1
        sty     regsave+3
        rts

resteax:
        lda     regsave+3
        sta     sreg+1
        lda     regsave+2
        sta     sreg
        ldx     regsave+1
        lda     regsave
        rts
