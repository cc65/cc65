;
; Marc 'BlackJack' Rintsch, 11.06.1999
;
; unsigned char __fastcall__ cbm_k_save(unsigned int start, unsigned int end);
;

        .export         _cbm_k_save
        .import         SAVE
        .import         popax
        .importzp       ptr1, tmp1


_cbm_k_save:
        sta     tmp1            ; store end address
        stx     tmp1+1
        jsr     popax           ; pop start address
        sta     ptr1
        stx     ptr1+1
        lda     #ptr1
        ldx     tmp1
        ldy     tmp1+1
        jsr     SAVE
        ldx     #0              ; Clear high byte
        bcs     @NotOk
        txa
@NotOk: rts
