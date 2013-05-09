;
; Ullrich von Bassewitz, 08.08.1998
;
; void screensize (unsigned char* x, unsigned char* y);
;

        .export         _screensize

        .import         popsreg
        .import         screensize
        .importzp       ptr1, sreg

.proc   _screensize

        sta     ptr1            ; Store the y pointer
        stx     ptr1+1
        jsr     popsreg         ; Get the x pointer into sreg
        jsr     screensize      ; Get screensize into X/Y
        tya                     ; Get Y size into A

.IFP02
        ldy     #0
        sta     (ptr1),y
        txa
        sta     (sreg),y
.ELSE
        sta     (ptr1)
        txa
        sta     (sreg)
.ENDIF

        rts

.endproc

