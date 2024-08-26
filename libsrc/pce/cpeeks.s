;
; 2020-07-14, Groepaz
; 2020-07-15, Greg King
;
; void cpeeks (char* s, unsigned length);
;
; get string from current position, do NOT advance cursor

        .export         _cpeeks

        .import         popax
        .importzp       ptr1, ptr2

        .include        "pce.inc"
        .include        "extzp.inc"

_cpeeks:
        eor     #<$FFFF         ; counting a word upward is faster
        sta     ptr2            ; so, we use -(length + 1)
        txa
        eor     #>$FFFF
        sta     ptr2+1

        st0     #VDC_CR         ; Control Register
        st2     #>$0088         ; make VRAM address increment by one

        st0     #VDC_MARR       ; Memory-Address Read
        ldy     SCREEN_PTR
        ldx     SCREEN_PTR+1
        sty     a:VDC_DATA_LO
        stx     a:VDC_DATA_HI

        st0     #VDC_VRR        ; VRAM Read Register

        jsr     popax
        tay                     ; low byte of address will be used as index
        stx     ptr1+1
        ldx     #<$0000
        stx     ptr1
        beq     L2              ; branch always

L3:     lda     a:VDC_DATA_LO   ; get character
        bit     a:VDC_DATA_HI   ; need to read high byte to advance VDC address
        and     #<~$80          ; remove reverse bit
        sta     (ptr1),y
        iny
        bne     L2
        inc     ptr1+1

L2:     inc     ptr2            ; count length
        bne     L3
        inc     ptr2+1
        bne     L3

        txa                     ; terminate the string
        sta     (ptr1),y
        rts
