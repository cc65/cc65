;
; int __fastcall__ read (int fildes, void* buf, unsigned count);
; int __fastcall__ write (int fildes, const void* buf, unsigned count);
;

        .export         _read, _write

        .import         __ria_read_xstack, __ria_write_xstack
        .import         incsp2, popptr1

        .importzp       c_sp, ptr1, ptr2, ptr3, sreg, tmp2

        .include        "rp6502.inc"

; The OS moves at most 512 bytes per call, so the count is split into chunks.
; The chunk calls use only ptr1 and tmp1, so the loop keeps the count left in
; ptr2, the total in ptr3, buf in sreg and the direction in tmp2. ptr4 is not
; used because fgets keeps it across its calls to fgetc.

_write: ldy     #$80
        .byte   $2C             ; bit abs, skips the ldy below
_read:  ldy     #0
        sty     tmp2
        sta     ptr2
        stx     ptr2+1
        jsr     popptr1
        lda     ptr1
        sta     sreg
        lda     ptr1+1
        sta     sreg+1
        stz     ptr3
        stz     ptr3+1
@loop:  lda     ptr2
        ora     ptr2+1
        beq     @done
        lda     (c_sp)          ; fildes
        sta     RIA_A
        clc                     ; buf + total
        lda     sreg
        adc     ptr3
        sta     ptr1
        lda     sreg+1
        adc     ptr3+1
        sta     ptr1+1
        ldy     ptr2            ; min(count, 512)
        ldx     ptr2+1
        cpx     #2
        bcc     @call
        ldy     #0
        ldx     #2
@call:  bit     tmp2
        bmi     @write
        stx     RIA_XSTACK
        sty     RIA_XSTACK
        ldy     #0
        jsr     __ria_read_xstack
        bra     @moved
@write: clc
        jsr     __ria_write_xstack
@moved: cpx     #$80
        bcs     @error
        pha
        clc
        adc     ptr3
        sta     ptr3
        txa
        adc     ptr3+1
        sta     ptr3+1
        pla
        bne     @done           ; a short chunk ends the transfer
        cpx     #2
        bne     @done
        dec     ptr2+1
        dec     ptr2+1
        bra     @loop
@error: lda     ptr3            ; data already moved is still reported
        ora     ptr3+1
        bne     @done
        dec     a               ; -1
        tax
        jmp     incsp2
@done:  lda     ptr3
        ldx     ptr3+1
        jmp     incsp2
