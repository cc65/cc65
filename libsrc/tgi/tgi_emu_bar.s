;
; Ullrich von Bassewitz, 22.06.2002
;
; Emulation for tgi_bar.
;

        .include        "tgi-kernel.inc"

        .importzp       ptr1, ptr2, ptr3, ptr4
        .export         tgi_emu_bar

tgi_emu_bar:
        lda     ptr4
        sta     Y2
        lda     ptr4+1
        sta     Y2+1

        lda     ptr3
        sta     X2
        lda     ptr3+1
        sta     X2+1

        lda     ptr2
        sta     ptr4
        sta     Y1
        lda     ptr2+1
        sta     ptr4+1
        sta     Y1+1

        lda     ptr1
        sta     X1
        lda     ptr1+1
        sta     X1+1

@L1:    jsr     tgi_line

        lda     Y1
        cmp     Y2
        bne     @L2
        lda     Y1
        cmp     Y2
        beq     @L4

@L2:    inc     Y1
        bne     @L3
        inc     Y1+1

@L3:    lda     Y1
        sta     ptr2
        sta     ptr4
        lda     Y1+1
        sta     ptr2+1
        sta     ptr4+1

        lda     X1
        sta     ptr1
        lda     X1+1
        sta     ptr1+1

        lda     X2
        sta     ptr3
        lda     X2+1
        sta     ptr3+1
        jmp     @L1

@L4:    rts

;-----------------------------------------------------------------------------
; Data

.bss

DY:     .res    2
X1:     .res    2
X2:     .res    2
Y1:     .res    2
Y2:     .res    2



