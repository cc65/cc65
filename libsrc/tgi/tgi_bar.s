;
; Ullrich von Bassewitz, 21.06.2002
;
; void __fastcall__ tgi_bar (int x1, int y1, int x2, int y2);
; /* Draw a bar (a filled rectangle) using the current color */


        .include        "tgi-kernel.inc"

        .importzp       ptr1, ptr2, ptr3, ptr4
        .import         popax


.proc   _tgi_bar

        sta     ptr4            ; Y2
        stx     ptr4+1

        jsr     popax
        sta     ptr3            ; X2
        stx     ptr3+1

        jsr     popax
        sta     ptr2            ; Y1
        stx     ptr2+1

        jsr     popax
        sta     ptr1            ; X1
        stx     ptr1+1

; Make sure X1 is less than X2. Swap both if not.

        lda     ptr3
        cmp     ptr1
        lda     ptr3+1
        sbc     ptr1+1
        bpl     @L1
        lda     ptr3
        ldy     ptr1
        sta     ptr1
        sty     ptr3
        lda     ptr3+1
        ldy     ptr1+1
        sta     ptr1+1
        sty     ptr3+1

; Make sure Y1 is less than Y2. Swap both if not.

@L1:    lda     ptr4
        cmp     ptr2
        lda     ptr4+1
        sbc     ptr2+1
        bpl     @L2
        lda     ptr4
        ldy     ptr2
        sta     ptr2
        sty     ptr4
        lda     ptr4+1
        ldy     ptr2+1
        sta     ptr2+1
        sty     ptr4+1

; Check if X2 or Y2 are negative. If so, the bar is completely out of screen.

@L2:    lda     ptr4+1
        ora     ptr3+1
        bmi     @L9             ; Bail out

; Check if X1 is negative. If so, clip it to the left border (zero).

        bit     ptr1+1
        bpl     @L3
        lda     #$00
        sta     ptr1
        sta     ptr1+1
        beq     @L4             ; Branch always, skip following test

; Check if X1 is beyond the right border. If so, the bar is invisible.

@L3:    lda     ptr1
        cmp     _tgi_xres
        lda     ptr1+1
        sbc     _tgi_xres
        bcs     @L9             ; Bail out if invisible

; Check if Y1 is negative. If so, clip it to the top border (zero).

@L4:    bit     ptr2+1
        bpl     @L5
        lda     #$00
        sta     ptr2
        sta     ptr2+1
        beq     @L6             ; Branch always, skip following test

; Check if Y1 is beyond the bottom border. If so, the bar is invisible.

@L5:    lda     ptr2
        cmp     _tgi_yres
        lda     ptr2+1
        sbc     _tgi_yres
        bcs     @L9             ; Bail out if invisible

; Check if X2 is larger than the maximum x coord. If so, clip it.

@L6:    lda     ptr3
        cmp     _tgi_xres
        lda     ptr3+1
        sbc     _tgi_xres+1
        bcc     @L7
        jsr     _tgi_getmaxx
        sta     ptr3
        stx     ptr3+1

; Check if Y2 is larger than the maximum y coord. If so, clip it.

@L7:    lda     ptr4
        cmp     _tgi_yres
        lda     ptr4+1
        sbc     _tgi_yres+1
        bcc     @L8
        jsr     _tgi_getmaxy
        sta     ptr4
        stx     ptr4+1

; The coordinates are now valid. Call the driver.

@L8:    jmp     tgi_bar

; Error exit

@L9:    rts

.endproc
