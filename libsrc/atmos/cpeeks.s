;
; 2017-06-20, Greg King
;
; void cpeeks (char* s, unsigned length);
;

        .export         _cpeeks

        .import         setscrptr, popax
        .importzp       ptr1, ptr2, ptr3, tmp1, tmp2

        .macpack        generic


_cpeeks:
        eor     #<$FFFF         ; counting a word upward is faster
        sta     ptr3            ; so, we use -(length + 1)
        txa
        eor     #>$FFFF
        sta     ptr3+1

        jsr     setscrptr       ; Set ptr2 and .Y to the cursor's address
        sty     tmp2

        jsr     popax
        sta     tmp1            ; (will be a .Y index)
        stx     ptr1+1
        ldx     #<$0000
        stx     ptr1
        bze     L3              ; branch always

L4:     ldy     tmp2
        lda     (ptr2),y        ; Get char
        iny
        bnz     L2
        inc     ptr2+1
L2:     sty     tmp2
        and     #<~$80          ; Remove reverse bit
        ldy     tmp1
        sta     (ptr1),y
        iny
        bnz     L1
        inc     ptr1+1
L1:     sty     tmp1

L3:     inc     ptr3            ; count length
        bnz     L4
        inc     ptr3+1
        bnz     L4

        txa                     ; terminate the string
        ldy     tmp1
        sta     (ptr1),y
        rts
