;
; int ria_execl (const char* path, ...);
;
; The strings are pushed before the table, so the offset of each string is
; stored in place of the pointer to that string until the table is pushed.
;

        .export         _ria_execl

        .import         _strlen, addysp

        .importzp       c_sp, ptr2, ptr4, tmp1, tmp2, tmp3

        .include        "rp6502.inc"
        .include        "errno.inc"

_ria_execl:
        sty     tmp1            ; parameter bytes
        lda     #16             ; argument limit
        sta     tmp3
        lda     #2              ; the 0 that ends the table
        sta     ptr2
        stz     ptr2+1
@scan:  dey
        lda     (c_sp),y
        tax
        dey
        ora     (c_sp),y
        beq     @push
        dec     tmp3
        bmi     @inval
        lda     (c_sp),y
        sty     tmp2
        jsr     _strlen
        clc
        adc     #3              ; the NUL and the table entry
        bcc     :+
        inx
        clc
:       adc     ptr2
        sta     ptr2
        txa
        adc     ptr2+1
        sta     ptr2+1
        ldx     ptr2
        cpx     #<513
        sbc     #>513
        bcs     @inval          ; more than 512 bytes
        ldy     tmp2
        bra     @scan
@inval: ldy     tmp1
        jsr     addysp
        lda     #EINVAL
        jmp     ___directerrno

@push:  sty     tmp3            ; the NULL slot
        iny
@str:   iny
        cpy     tmp1
        beq     @table
        sty     tmp2
        iny
        lda     (c_sp),y
        tax
        dey
        lda     (c_sp),y
        jsr     _strlen         ; X:Y = length, and (ptr4),y is the NUL
        eor     #$FF            ; ptr2 -= length + 1
        clc
        adc     ptr2
        sta     ptr2
        txa
        eor     #$FF
        adc     ptr2+1
        sta     ptr2+1
@byte:  lda     (ptr4),y
        sta     RIA_XSTACK
        dey
        cpy     #$FF
        bne     @byte
        dec     ptr4+1
        dex
        bpl     @byte
        ldy     tmp2
        lda     ptr2
        sta     (c_sp),y
        iny
        lda     ptr2+1
        sta     (c_sp),y
        bra     @str

@table: ldy     tmp3            ; the 0 in the NULL slot ends the table
@entry: iny
        lda     (c_sp),y
        sta     RIA_XSTACK
        dey
        lda     (c_sp),y
        sta     RIA_XSTACK
        iny
        iny
        cpy     tmp1
        bne     @entry
        jsr     addysp
        lda     #RIA_OP_EXEC
        sta     RIA_OP
        jmp     RIA_SPIN
