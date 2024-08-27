;
; Helper functions to save/restore the cursor position when switching the screen memory
;
; void __fastcall__ savecursorto(uint16_t *ptr);
; void __fastcall__ restorecursorfrom(uint16_t *ptr);

        .export         _savecursorto, _restorecursorfrom
        .import         CURS_X: zp, CURS_Y: zp


.proc   _savecursorto
        jsr     pushax
        lda     CURS_X
        ldx     CURS_Y
        ldy     #0
        jmp     staxspidx
.endproc


.proc   _restorecursorfrom
        jsr ldaxi
        sta CURS_X
        stx CURS_Y
        rts
.endproc
