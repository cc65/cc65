;
; Ullrich von Bassewitz, 14.09.2001
;

        .export         _peekbsys, _peekwsys
        .importzp       ptr1

        .include        "cbm610.inc"


; ------------------------------------------------------------------------
; unsigned char __fastcall__ peekbsys (unsigned addr);

.proc   _peekbsys

        sta     ptr1            ; Store argument pointer
        stx     ptr1+1
        ldx     IndReg
        lda     #$0F
        sta     IndReg
        ldy     #$00
        lda     (ptr1),y
        stx     IndReg
        ldx     #$00            ; Extend to word
        rts

.endproc

; ------------------------------------------------------------------------
; unsigned __fastcall__ peekwsys (unsigned addr);

.proc   _peekwsys

        sta     ptr1            ; Store argument pointer
        stx     ptr1+1
        ldx     IndReg
        lda     #$0F
        sta     IndReg
        ldy     #$00
        lda     (ptr1),y        ; Get low byte
        pha
        iny
        lda     (ptr1),y        ; Get high byte
        stx     IndReg
        tax                     ; High byte -> X
        pla                     ; Low byte -> A
        rts

.endproc

