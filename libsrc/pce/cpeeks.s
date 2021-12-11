;
; 2020-07-14, Groepaz
;
; void cpeeks (char* s, unsigned length);
;
; get string from current position, do NOT advance cursor

        .export         _cpeeks

        .import         popax
        .importzp       ptr1, ptr2, tmp1, tmp2

        .macpack        generic

        .include        "pce.inc"
        .include        "extzp.inc"

_cpeeks:
        eor     #<$FFFF         ; counting a word upward is faster
        sta     ptr2            ; so, we use -(length + 1)
        txa
        eor     #>$FFFF
        sta     ptr2+1

        st0     #VDC_MARR       ; Memory-Address Read
        ldy     SCREEN_PTR
        ldx     SCREEN_PTR+1
        sty     VDC_DATA_LO
        stx     VDC_DATA_HI

        st0     #VDC_VRR        ; VRAM Read Register

        jsr     popax
        sta     tmp1            ; (will be a .Y index)
        stx     ptr1+1

        ldx     #<$0000
        stx     ptr1
        beq     L2              ; branch always

L3:     ldy     tmp2
        lda     VDC_DATA_LO     ; get character
        bit     VDC_DATA_HI     ; we need to "read" the highbyte to advance the address
        iny
        sty     tmp2
        and     #<~$80          ; remove reverse bit

        ldy     tmp1
        sta     (ptr1),y
        iny
        bne     L1
        inc     ptr1+1
L1:     sty     tmp1

L2:     inc     ptr2            ; count length
        bne     L3
        inc     ptr2+1
        bne     L3

        txa                     ; terminate the string
        ldy     tmp1
        sta     (ptr1),y
        rts
