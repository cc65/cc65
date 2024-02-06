;
; Marc 'BlackJack' Rintsch, 11.06.1999
;
; unsigned char __fastcall__ cbm_k_save(unsigned int start, unsigned int end);
;

        .include        "cbm.inc"

        .export         _cbm_k_save
        .import         popptr1
        .importzp       ptr1, tmp1


_cbm_k_save:
        sta     tmp1            ; store end address
        stx     tmp1+1
        jsr     popptr1         ; pop start address
        lda     #ptr1
        ldx     tmp1
        ldy     tmp1+1
        jsr     SAVE
        ldx     #0              ; Clear high byte
        bcs     @NotOk
        txa
@NotOk: rts
