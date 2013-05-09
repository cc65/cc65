;
; Ullrich von Bassewitz, 03.06.1999
;
; void __fastcall__ cbm_k_setnam (const char* Name);
;

        .export         _cbm_k_setnam
        .import         SETNAM
        .importzp       ptr1
                              

_cbm_k_setnam:
        sta     ptr1            ; Store pointer to file name
        stx     ptr1+1
        ldy     #$FF
@Loop:  iny                     ; Get length of name
        lda     (ptr1),y
        bne     @Loop

        tya                     ; Length
        ldx     ptr1
        ldy     ptr1+1
        jmp     SETNAM

