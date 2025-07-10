;
; Lauri Kasanen, 6 Jun 2017
; (C) Mega Cat Studios
; An optimized LZ4 decompressor
;
; Almost 7 times faster, uses no RAM (vs 14 bytes BSS), and takes 1/4 the space
; vs the official C source.
;
;
; C implementation was:

; void decompress_lz4 (unsigned char *in, unsigned char *out, const int outlen) {
;   unsigned char token, tmp;
;   unsigned int offset;
;   unsigned char *end = out+outlen;
;   unsigned char *copysrc;
;
;   while (out < end) {
;     token = *in++;
;     offset = token >> 4;
;
;     token &= 0x0f;
;     token += 4; // Minmatch
;
;     if (offset == 15) {
; moreliterals:
;       tmp = *in++;
;       offset += tmp;
;       if (tmp == 255)
;         goto moreliterals;
;     }
;
;     if (offset) {
;       memcpy(out, in, offset);
;       out += offset;
;       in += offset;
;     }
;
;     if (out >= end) {
;       return;
;     }
;
;     offset = (*in);
;     in++;
;     offset += (*in)<<8;
;     in++;
;
;     copysrc = out - offset;
;     offset = token;
;
;     if (token == 19) {
; morematches:
;       tmp = *in++;
;       offset += tmp;
;       if (tmp == 255)
;         goto morematches;
;     }
;
;     memcpy(out, copysrc, offset);
;     out += offset;
;   }
; }

        .importzp       c_sp, sreg, regsave, regbank
        .importzp       tmp1, tmp2, tmp3, tmp4, ptr1, ptr2, ptr3, ptr4
        .macpack        longbranch
        .import         memcpy_upwards,pushax,popax
        .export         _decompress_lz4

out = regsave
end = regsave + 2
tmp = tmp1
token = tmp2
offset = ptr3
in = sreg

; ---------------------------------------------------------------
; void decompress_lz4 (const u8 *in, u8 * const out, const u16 outlen)
; ---------------------------------------------------------------

.segment        "CODE"

.proc   _decompress_lz4: near

        sta     tmp
        stx     tmp+1

;
; end = out + outlen;
;
        jsr     popax
        sta     out
        clc
        adc     tmp
        sta     end
        txa
        sta     out+1
        adc     tmp+1
        sta     end+1

        jsr     popax
        sta     in
        stx     in+1

;
; while (out < end) {
;
        jmp     check_len
;
; token = *in++;
;
get_token:
        ldy     #$00
        lda     (in),y
        tay                   ; Backup token to Y

        inc     in
        bne     :+
        inc     in+1

:
;
; offset = token >> 4;
;
        ldx     #$00
        lsr     a
        lsr     a
        lsr     a
        lsr     a
        sta     offset
        stx     offset+1
;
; token &= 0xf;
; token += 4; // Minmatch
;
        tya                   ; Get token back from Y
        and     #$0F
        clc
        adc     #$04
        sta     token
;
; if (offset == 15) {
;
        lda     offset
        cmp     #$0F
moreliterals:
        bne     check_offset_not_zero
;
; tmp = *in++;
;
        ldy     #$00
        lda     (in),y
        sta     tmp

        inc     in
        bne     :+
        inc     in+1

:
;
; offset += tmp;
;
        clc
        adc     offset
        sta     offset
        lda     #$00
        adc     offset+1
        sta     offset+1
;
; if (tmp == 255)
;
        lda     tmp
        cmp     #$FF
;
; goto moreliterals;
;
        jmp     moreliterals
;
; if (offset) {
;
check_offset_not_zero:
        lda     offset
        ora     offset+1
        beq     check_end
;
; memcpy(out, in, offset);
;
        lda     out
        sta     ptr2
        ldx     out+1
        stx     ptr2+1
        jsr     pushax
        lda     in
        ldx     in+1
        sta     ptr1
        stx     ptr1+1
;        ldy     #$00 - not needed as pushax zeroes Y
        jsr     memcpy_upwards
;
; out += offset;
; memcpy returned a pointer to out
;
        clc
        adc     offset
        sta     out
        txa
        adc     offset+1
        sta     out+1
;
; in += offset;
;
        lda     offset
        clc
        adc     in
        sta     in
        lda     offset+1
        adc     in+1
        sta     in+1
;
; if (out >= end)
;
check_end:
        lda     out
        cmp     end
        lda     out+1
        sbc     end+1
;
; return;
;
        bcc     end_not_reached
        rts
;
; memcpy(&offset, in, 2);
;
end_not_reached:
        ldy     #$00
        lda     (in),y
        sta     offset
        iny
        lda     (in),y
        sta     offset+1
;
; in += 2;
;
        lda     #$02
        clc
        adc     in
        sta     in
        bcc     :+
        inc     in+1

:
;
; copysrc = out - offset;
;
        lda     out
        sec
        sbc     offset
        sta     ptr1
        lda     out+1
        sbc     offset+1
        sta     ptr1+1
;
; offset = token;
;
        lda     #$00
        sta     offset+1
        lda     token
        sta     offset
;
; if (token == 19) {
;
        cmp     #$13
morematches:
        bne     token_not_19
;
; tmp = *in++;
;
        ldy     #$00
        lda     (in),y
        sta     tmp

        inc     in
        bne     :+
        inc     in+1
:
;
; offset += tmp;
;
        clc
        adc     offset
        sta     offset
        tya
        adc     offset+1
        sta     offset+1
;
; if (tmp == 255)
;
        lda     tmp
        cmp     #$FF
;
; goto morematches;
;
        jmp     morematches
;
; memcpy(out, copysrc, offset);
;
token_not_19:
        lda     out
        sta     ptr2
        ldx     out+1
        stx     ptr2+1
        jsr     pushax
        jsr     memcpy_upwards
;
; out += offset;
;
        clc
        adc     offset
        sta     out
        txa
        adc     offset+1
        sta     out+1     ; 0 on the first loop iteration
check_len:
;
; while (out < end) {
;
        lda     out
        cmp     end
        lda     out+1
        sbc     end+1
        jcc     get_token

        rts

.endproc
