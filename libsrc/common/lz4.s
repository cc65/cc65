;
; Lauri Kasanen, 6 Jun 2017
; (C) Mega Cat Studios
; An optimized LZ4 decompressor
;

        .importzp       sp, sreg, regsave, regbank
        .importzp       tmp1, tmp2, tmp3, tmp4, ptr1, ptr2, ptr3, ptr4
        .macpack        longbranch
        .import         memcpy_upwards,pushax,popax
        .export         _decompress_lz4

_out = regsave
_written = regsave + 2
_tmp = tmp1
_token = tmp2
_offset = ptr3
_in = sreg
_outlen = ptr4

; ---------------------------------------------------------------
; void decompress_lz4 (const u8 *in, u8 * const out, const u16 outlen)
; ---------------------------------------------------------------

.segment        "CODE"

.proc   _decompress_lz4: near

        sta     _outlen
        stx     _outlen+1

        jsr     popax
        sta     _out
        stx     _out+1

        jsr     popax
        sta     _in
        stx     _in+1

;
; written = 0;
;
        lda     #$00
        sta     _written
;
; while (written < outlen) {
;
        jmp     L0046
;
; token = *in++;
;
L0004:  ldy     #0
        lda     (_in),y
        sta     _token

        inc     _in
        bne     L000A
        inc     _in+1
L000A:
;
; offset = token >> 4;
;
        ldx     #$00
        lsr     a
        lsr     a
        lsr     a
        lsr     a
        sta     _offset
        stx     _offset+1
;
; token &= 0xf;
; token += 4; // Minmatch
;
        lda     _token
        and     #$0F
        clc
        adc     #4
        sta     _token
;
; if (offset == 15) {
;
        lda     _offset
        cmp     #$0F
L0013:  bne     L001A
;
; tmp = *in++;
;
        ldy     #0
        lda     (_in),y
        sta     _tmp

        inc     _in
        bne     L0017
        inc     _in+1
L0017:
;
; offset += tmp;
;
        clc
        adc     _offset
        sta     _offset
        lda     #$00
        adc     _offset+1
        sta     _offset+1
;
; if (tmp == 255)
;
        lda     _tmp
        cmp     #$FF
;
; goto moreliterals;
;
        jmp     L0013
;
; if (offset) {
;
L001A:  lda     _offset
        ora     _offset+1
        beq     L001C
;
; memcpy(&out[written], in, offset);
;
        lda     _out
        clc
        adc     _written
        sta     ptr2
        lda     _out+1
        adc     _written+1
        tax
        lda     ptr2
        stx     ptr2+1
        jsr     pushax
        lda     _in
        ldx     _in+1
        sta     ptr1
        stx     ptr1+1
        ldy     #0
        jsr     memcpy_upwards
;
; written += offset;
;
        lda     _offset
        clc
        adc     _written
        sta     _written
        lda     _offset+1
        adc     _written+1
        sta     _written+1
;
; in += offset;
;
        lda     _offset
        clc
        adc     _in
        sta     _in
        lda     _offset+1
        adc     _in+1
        sta     _in+1
;
; if (written >= outlen)
;
L001C:  lda     _written
        cmp     _outlen
        lda     _written+1
        sbc     _outlen+1
;
; return;
;
        bcc     L0047
        rts
;
; memcpy(&offset, in, 2);
;
L0047:  ldy     #0
        lda     (_in),y
        sta     _offset
        iny
        lda     (_in),y
        sta     _offset+1
;
; in += 2;
;
        lda     #$02
        clc
        adc     _in
        sta     _in
        bcc     L002F
        inc     _in+1
;
; copysrc = out + written - offset;
;
L002F:  lda     _out
        clc
        adc     _written
        pha
        lda     _out+1
        adc     _written+1
        tax
        pla
        sec
        sbc     _offset
        sta     ptr1
        txa
        sbc     _offset+1
        sta     ptr1+1
;
; offset = token;
;
        lda     #$00
        sta     _offset+1
        lda     _token
        sta     _offset
;
; if (token == 19) {
;
        cmp     #$13
L0045:  bne     L003C
;
; tmp = *in++;
;
        ldy     #0
        lda     (_in),y
        sta     _tmp

        inc     _in
        bne     L0039
        inc     _in+1
L0039:
;
; offset += tmp;
;
        clc
        adc     _offset
        sta     _offset
        tya
        adc     _offset+1
        sta     _offset+1
;
; if (tmp == 255)
;
        lda     _tmp
        cmp     #$FF
;
; goto morematches;
;
        jmp     L0045
;
; memcpy(&out[written], copysrc, offset);
;
L003C:  lda     _out
        clc
        adc     _written
        sta     ptr2
        lda     _out+1
        adc     _written+1
        tax
        lda     ptr2
        stx     ptr2+1
        jsr     pushax
        jsr     memcpy_upwards
;
; written += offset;
;
        lda     _offset
        clc
        adc     _written
        sta     _written
        lda     _offset+1
        adc     _written+1
L0046:  sta     _written+1
;
; while (written < outlen) {
;
        lda     _written
        cmp     _outlen
        lda     _written+1
        sbc     _outlen+1
        jcc     L0004

        rts

.endproc

