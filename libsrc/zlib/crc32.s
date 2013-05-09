;
; Piotr Fusik, 14.11.2001
;
; unsigned long __fastcall__ crc32 (unsigned long crc, unsigned char* buf,
;                                   unsigned len);
;

        .export _crc32

        .import         compleax, incsp2, incsp4, popax, popeax
        .importzp       sreg, ptr1, ptr2, tmp1, tmp2

POLYNOMIAL      =       $EDB88320

make_table:
        ldx     #0
@L1:    lda     #0
        sta     tmp2
        sta     sreg
        sta     sreg+1
        ldy     #8
        txa
@L2:    sta     tmp1
        lsr     a
        bcc     @L3
        lda     sreg+1
        lsr     a
        eor     #(POLYNOMIAL>>24)&$FF
        sta     sreg+1
        lda     sreg
        ror     a
        eor     #(POLYNOMIAL>>16)&$FF
        sta     sreg
        lda     tmp2
        ror     a
        eor     #(POLYNOMIAL>>8)&$FF
        sta     tmp2
        lda     tmp1
        ror     a
        eor     #POLYNOMIAL&$FF
        bcs     @L4     ; branch always
@L3:    rol     a
        lsr     sreg+1
        ror     sreg
        ror     tmp2
        ror     a
@L4:    dey
        bne     @L2
        sta     table_0,x
        lda     tmp2
        sta     table_1,x
        lda     sreg
        sta     table_2,x
        lda     sreg+1
        sta     table_3,x
        inx
        bne     @L1
        inc     table_initialised
RET:
        rts

_crc32:
; ptr2 = (len & 0xff) == 0 ? len : len + 0x100;
        tay
        beq     @L1
        inx
@L1:    sta     ptr2
        stx     ptr2+1
; ptr1 = buf
        jsr     popax
        sta     ptr1
        stx     ptr1+1
; if (buf == NULL) return 0;
        ora     ptr1+1
        beq     @L0
; if (!tables_initialised) make_tables();
        lda     table_initialised
        bne     @dont_make
        jsr     make_table
@dont_make:
; eax = crc
        jsr     popeax
; if (len == 0) return crc;
        ldy     ptr2
        bne     @L2
        ldy     ptr2+1
        beq     RET
@L2:
; eax = ~crc
        jsr     compleax
        stx     tmp2
        ldy     #0
; crc = (crc >> 8) ^ table[(crc & 0xff) ^ *p++];
@L3:    eor     (ptr1),y
        tax
        lda     table_0,x
        eor     tmp2
        sta     tmp1
        lda     table_1,x
        eor     sreg
        sta     tmp2
        lda     table_2,x
        eor     sreg+1
        sta     sreg
        lda     table_3,x
        sta     sreg+1
        lda     tmp1
        iny
        bne     @L4
        inc     ptr1+1
@L4:    dec     ptr2
        bne     @L3
        dec     ptr2+1
        bne     @L3
        ldx     tmp2
        jmp     compleax

; return 0L
@L0:    sta     sreg
        sta     sreg+1
; ignore crc
        jmp     incsp4

                .data
table_initialised:
                .byte   0

                .bss
table_0:        .res    256
table_1:        .res    256
table_2:        .res    256
table_3:        .res    256


