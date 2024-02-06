;
; Ullrich von Bassewitz, 1998-12-09, 2004-11-30
;
; void __fastcall__ __swap (void* p, void* q, size_t size);
;

        .export         ___swap
        .import         popax, popptr1
        .importzp       ptr1, ptr2, ptr3


___swap: eor     #$FF
        sta     ptr3
        txa
        eor     #$FF
        sta     ptr3+1          ; Save -(size+1) into ptr3

        jsr     popax           ; Get q
        sta     ptr2
        stx     ptr2+1

        jsr     popptr1         ; Get p

; Prepare for swap

        ; ldy     #$00          is guaranteed by popptr1

; Swap loop

@L1:    inc     ptr3            ; Bump counter low byte
        beq     @L3             ; Branch on overflow

@L2:    lda     (ptr1),y
        tax
        lda     (ptr2),y
        sta     (ptr1),y
        txa
        sta     (ptr2),y
        iny
        bne     @L1
        inc     ptr1+1
        inc     ptr2+1
        bne     @L1             ; Branch always (hopefully)

; Bump the high counter byte

@L3:    inc     ptr3+1
        bne     @L2

; Done

        rts

