;
; Ullrich von Bassewitz, 08.08.1998
;
; void screensize (unsigned char* x, unsigned char* y);
;

        .export         _screensize

        .import         popptr1
        .import         screensize
        .importzp       ptr1, ptr2

.proc   _screensize

        sta     ptr2            ; Store the y pointer
        stx     ptr2+1
        jsr     popptr1         ; Get the x pointer into ptr1
        jsr     screensize      ; Get screensize into X/Y
        tya                     ; Get Y size into A

.if .cap(CPU_HAS_ZPIND)
        sta     (ptr2)
        txa
        sta     (ptr1)
.else
        ldy     #0
        sta     (ptr2),y
        txa
        sta     (ptr1),y
.endif
        rts

.endproc

