;
; 2026, Rumbledethumps
;
; XRAM block memory access, optimally unrolled.

.include "rp6502.inc"

.importzp c_sp, ptr1, ptr2, ptr3
.import incsp1, incsp2, incsp3, incsp4

; void __fastcall__ xramN_read (void* dest, unsigned src, unsigned count);

.macro xram_read rw, addr, step
        sta     ptr3            ; count arrives in A/X
        stx     ptr3+1
        lda     (c_sp)          ; src is the XRAM address
        sta     addr
        ldy     #1
        lda     (c_sp),y
        sta     addr+1
        lda     #1
        sta     step
        iny
        lda     (c_sp),y        ; dest is the 6502 RAM address
        sta     ptr2
        iny
        lda     (c_sp),y
        sta     ptr2+1
        jsr     incsp4
        ldx     ptr3+1
        inx                     ; pages, plus one for the partial page
        lda     ptr3
        and     #$F8
        tay
        beq     @next
        clc                     ; bias dest so the partial page ends as Y wraps
        adc     ptr2
        sta     ptr2
        bcs     :+
        dec     ptr2+1
:       tya
        eor     #$FF
        tay
        iny
@page:  .repeat 8
        lda     rw
        sta     (ptr2),y
        iny
        .endrepeat
        bne     @page
        inc     ptr2+1
@next:  dex
        bne     @page
        lda     ptr3
        and     #7
        beq     @done
        tax
@rest:  lda     rw
        sta     (ptr2),y
        iny
        dex
        bne     @rest
@done:  rts
.endmacro

; void __fastcall__ xramN_write (unsigned dest, const void* src, unsigned count);

.macro xram_write rw, addr, step
        sta     ptr3            ; count arrives in A/X
        stx     ptr3+1
        lda     (c_sp)          ; src is the 6502 RAM address
        sta     ptr2
        ldy     #1
        lda     (c_sp),y
        sta     ptr2+1
        iny
        lda     (c_sp),y        ; dest is the XRAM address
        sta     addr
        iny
        lda     (c_sp),y
        sta     addr+1
        jsr     incsp4
        lda     #1
        sta     step
        ldx     ptr3+1
        inx                     ; pages, plus one for the partial page
        lda     ptr3
        and     #$F8
        tay
        beq     @next
        clc                     ; bias src so the partial page ends as Y wraps
        adc     ptr2
        sta     ptr2
        bcs     :+
        dec     ptr2+1
:       tya
        eor     #$FF
        tay
        iny
@page:  .repeat 8
        lda     (ptr2),y
        sta     rw
        iny
        .endrepeat
        bne     @page
        inc     ptr2+1
@next:  dex
        bne     @page
        lda     ptr3
        and     #7
        beq     @done
        tax
@rest:  lda     (ptr2),y
        sta     rw
        iny
        dex
        bne     @rest
@done:  rts
.endmacro

; void __fastcall__ xramN_set (unsigned dest, unsigned char val, unsigned count);

.macro xram_set rw, addr, step
        sta     ptr3            ; count arrives in A/X
        stx     ptr3+1
        ldy     #1              ; dest is the XRAM address
        lda     (c_sp),y
        sta     addr
        iny
        lda     (c_sp),y
        sta     addr+1
        lda     #1
        sta     step
        lda     (c_sp)          ; value
        jsr     incsp3
        tay                     ; the value stays in Y across the stores
        ldx     ptr3+1
        inx                     ; pages, plus one for the partial page
        lda     ptr3
        and     #15
        beq     @blocks
@rest:  sty     rw
        dec     a
        bne     @rest
@blocks:
        lda     ptr3
        lsr     a
        lsr     a
        lsr     a
        lsr     a
        beq     @next
@page:  .repeat 16
        sty     rw
        .endrepeat
        dec     a
        bne     @page
@next:  lda     #16
        dex
        bne     @page
        rts
.endmacro

; unsigned char __fastcall__ xramN_peek8 (unsigned addr);

.macro xram_peek8 rw, addr
        sta     addr
        stx     addr+1
        lda     rw
        ldx     #0
        rts
.endmacro

; unsigned __fastcall__ xramN_peek16 (unsigned addr);

.macro xram_peek16 rw, addr, step
        sta     addr
        stx     addr+1
        lda     #1
        sta     step
        lda     rw
        ldx     rw
        rts
.endmacro

; void __fastcall__ _xramN_poke8 (unsigned char val, unsigned addr);

.macro xram_poke8 rw, addr
        sta     addr
        stx     addr+1
        lda     (c_sp)
        sta     rw
        jmp     incsp1
.endmacro

; void __fastcall__ _xramN_poke16 (unsigned val, unsigned addr);

.macro xram_poke16 rw, addr, step
        sta     addr
        stx     addr+1
        lda     #1
        sta     step
        lda     (c_sp)
        sta     rw
        ldy     #1
        lda     (c_sp),y
        sta     rw
        jmp     incsp2
.endmacro
