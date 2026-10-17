;
; int __fastcall__ ria_execv (const char* path, char* const argv[]);
;

        .export         _ria_execv

        .import         _strlen, incsp2

        .importzp       c_sp, ptr1, ptr2, ptr4, tmp2, tmp3

        .include        "rp6502.inc"
        .include        "errno.inc"

_ria_execv:
        sta     ptr1
        stx     ptr1+1
        lda     #2              ; the 0 that ends the table
        sta     ptr2
        stz     ptr2+1
        ldy     #$FE            ; path, then argv from index 0
@scan:  sty     tmp2
        jsr     arglen
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
        iny
        iny
        iny
        lda     (ptr1),y
        dey
        ora     (ptr1),y
        beq     @push
        cpy     #30             ; argv[15] would be argument 17
        bcc     @scan
@inval: jsr     incsp2
        lda     #EINVAL
        jmp     ___directerrno

@push:  sty     tmp3            ; index of the NULL in argv
@str:   dey
        dey
        sty     tmp2
        jsr     arglen          ; X:Y = length, and (ptr4),y is the NUL
@byte:  lda     (ptr4),y
        sta     RIA_XSTACK
        dey
        cpy     #$FF
        bne     @byte
        dec     ptr4+1
        dex
        bpl     @byte
        ldy     tmp2
        bpl     @str

        stz     RIA_XSTACK      ; the 0 that ends the table
        stz     RIA_XSTACK
        ldy     tmp3
@entry: dey
        dey
        sty     tmp2
        jsr     arglen
        eor     #$FF            ; ptr2 -= length + 1
        clc
        adc     ptr2
        sta     ptr2
        tay
        txa
        eor     #$FF
        adc     ptr2+1
        sta     ptr2+1
        sta     RIA_XSTACK
        sty     RIA_XSTACK
        ldy     tmp2
        bpl     @entry
        jsr     incsp2
        lda     #RIA_OP_EXEC
        sta     RIA_OP
        jmp     RIA_SPIN

; A/X = strlen of argv[Y/2], or of path when Y is negative
arglen: tya
        bmi     @path
        iny
        lda     (ptr1),y
        tax
        dey
        lda     (ptr1),y
        jmp     _strlen
@path:  ldy     #1
        lda     (c_sp),y
        tax
        lda     (c_sp)
        jmp     _strlen
