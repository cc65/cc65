;
; Lauri Kasanen, 6 Jun 2017
; (C) Mega Cat Studios
; An optimized LZ4 decompressor
;
; Almost 7 times faster, uses no RAM (vs 14 bytes BSS), and takes 1/4 the space
; vs the official C source.
;

        .importzp       sp, sreg, regsave, regbank
        .importzp       tmp1, tmp2, tmp3, tmp4, ptr1, ptr2, ptr3, ptr4
        .macpack        longbranch
        .import         memcpy_upwards,pushax,popax
        .export         _decompress_lz4

out = regsave
written = regsave + 2
tmp = tmp1
token = tmp2
offset = ptr3
in = sreg
outlen = ptr4

; ---------------------------------------------------------------
; void decompress_lz4 (const u8 *in, u8 * const out, const u16 outlen)
; ---------------------------------------------------------------

.segment        "CODE"

.proc   _decompress_lz4: near

        sta     outlen
        stx     outlen+1

        jsr     popax
        sta     out
        stx     out+1

        jsr     popax
        sta     in
        stx     in+1

;
; written = 0;
;
        lda     #$00
        sta     written
;
; while (written < outlen) {
;
        jmp     L0046
;
; token = *in++;
;
L0004:  ldy     #$00
        lda     (in),y
        sta     token

        inc     in
        bne     L000A
        inc     in+1
L000A:
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
        lda     token
        and     #$0F
        clc
        adc     #$04
        sta     token
;
; if (offset == 15) {
;
        lda     offset
        cmp     #$0F
L0013:  bne     L001A
;
; tmp = *in++;
;
        ldy     #$00
        lda     (in),y
        sta     tmp

        inc     in
        bne     L0017
        inc     in+1
L0017:
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
        jmp     L0013
;
; if (offset) {
;
L001A:  lda     offset
        ora     offset+1
        beq     L001C
;
; memcpy(&out[written], in, offset);
;
        lda     out
        clc
        adc     written
        sta     ptr2
        lda     out+1
        adc     written+1
        tax
        lda     ptr2
        stx     ptr2+1
        jsr     pushax
        lda     in
        ldx     in+1
        sta     ptr1
        stx     ptr1+1
;        ldy     #$00 - not needed as pushax zeroes Y
        jsr     memcpy_upwards
;
; written += offset;
;
        lda     offset
        clc
        adc     written
        sta     written
        lda     offset+1
        adc     written+1
        sta     written+1
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
; if (written >= outlen)
;
L001C:  lda     written
        cmp     outlen
        lda     written+1
        sbc     outlen+1
;
; return;
;
        bcc     L0047
        rts
;
; memcpy(&offset, in, 2);
;
L0047:  ldy     #$00
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
        bcc     L002F
        inc     in+1
;
; copysrc = out + written - offset;
;
L002F:  lda     out
        clc
        adc     written
        tay
        lda     out+1
        adc     written+1
        tax
        tya
        sec
        sbc     offset
        sta     ptr1
        txa
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
L0045:  bne     L003C
;
; tmp = *in++;
;
        ldy     #$00
        lda     (in),y
        sta     tmp

        inc     in
        bne     L0039
        inc     in+1
L0039:
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
        jmp     L0045
;
; memcpy(&out[written], copysrc, offset);
;
L003C:  lda     out
        clc
        adc     written
        sta     ptr2
        lda     out+1
        adc     written+1
        tax
        lda     ptr2
        stx     ptr2+1
        jsr     pushax
        jsr     memcpy_upwards
;
; written += offset;
;
        lda     offset
        clc
        adc     written
        sta     written
        lda     offset+1
        adc     written+1
L0046:  sta     written+1
;
; while (written < outlen) {
;
        lda     written
        cmp     outlen
        lda     written+1
        sbc     outlen+1
        jcc     L0004

        rts

.endproc

