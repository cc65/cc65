;
; mainargs.s
;

; Lower priority than initheap so argv_mem() can use malloc().
.constructor initmainargs, 23
.import __argc, __argv, _argv_mem
.import incax2
.importzp ptr1, ptr2
.include "rp6502.inc"

.segment "ONCE"

.proc initmainargs

    ; Ask the RIA for argv data; returns total byte count in AX.
    lda     #RIA_OP_ARGV
    sta     RIA_OP
    jsr     RIA_SPIN

    ; Bail if argv size <= 0.
    sta     ptr2
    txa
    bmi     zxstack    ; count < 0
    sta     ptr2+1
    ora     ptr2
    beq     zxstack    ; count == 0

    ; Request memory; _argv_mem may clobber.
    lda     ptr2
    ldx     ptr2+1
    pha
    phx
    jsr     _argv_mem
    ply
    sty     ptr2+1
    ply
    sty     ptr2

    ; Bail if no memory.
    sta     ptr1
    stx     ptr1+1
    sta     __argv
    stx     __argv+1
    ora     ptr1+1
    beq     zxstack

    ; Pop ptr2 bytes from RIA_XSTACK into memory.
    ldy     #0
fillloop:
    lda     RIA_XSTACK
    sta     (ptr1),y
    inc     ptr1
    bne     :+
    inc     ptr1+1
:   lda     ptr2
    bne     :+
    dec     ptr2+1
:   dec     ptr2
    lda     ptr2
    ora     ptr2+1
    bne     fillloop

    ; Walk the pointer table: relocate each offset to an absolute address
    ; and count argc. The RIA stores offsets relative to the buffer start;
    ; adding __argv turns them into usable pointers.
relocate:
    lda     __argv
    sta     ptr1
    lda     __argv+1
    sta     ptr1+1

walkloop:
    lda     (ptr1)          ; 65C02 ZP-indirect: low byte of entry
    ldy     #1
    ora     (ptr1),y        ; OR with high byte
    beq     done            ; null entry = end of table

    ; Add buffer base to the stored offset.  Y=1 from above.
    lda     (ptr1)          ; low byte
    clc
    adc     __argv
    sta     (ptr1)
    lda     (ptr1),y        ; high byte
    adc     __argv+1
    sta     (ptr1),y

    inc     __argc
    bne     :+
    inc     __argc+1
:
    lda     ptr1
    ldx     ptr1+1
    jsr     incax2
    sta     ptr1
    stx     ptr1+1
    bra     walkloop

zxstack:
    lda     #RIA_OP_ZXSTACK
    sta     RIA_OP

done:
    rts

.endproc
