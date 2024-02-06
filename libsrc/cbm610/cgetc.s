;
; Ullrich von Bassewitz, 06.08.1998
;
; char cgetc (void);
;

        .export         _cgetc
        .import         plot, write_crtc
        .import         cursor

        .import         keyidx: zp, keybuf: zp, config: zp



_cgetc: lda     keyidx          ; Get number of characters
        bne     L2              ; Jump if there are already chars waiting

; Switch on the cursor if needed

        lda     cursor
        beq     L1              ; Jump if no cursor

        jsr     plot            ; Set the current cursor position
        ldy     #10
        lda     config          ; Cursor format
        jsr     write_crtc      ; Set the cursor formar

L1:     lda     keyidx
        beq     L1

        ldy     #10
        lda     #$20            ; Cursor off
        jsr     write_crtc

L2:     ldx     #$00            ; Get index
        ldy     keybuf          ; Get first character in the buffer
        sei
L3:     lda     keybuf+1,x      ; Move up the remaining chars
        sta     keybuf,x
        inx
        cpx     keyidx
        bne     L3
        dec     keyidx
        cli

        ldx     #$00            ; High byte
        tya                     ; First char from buffer
        rts


