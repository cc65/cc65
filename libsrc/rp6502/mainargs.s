;
; mainargs.s
;

; Runs after crt0 clears BSS, so __argv_mem() may return static or heap
; memory over the reclaimed ONCE.
.constructor initmainargs, 5
.import __argc, __argv, ___argv_mem
.importzp ptr1, ptr2
.include "rp6502.inc"

.code

.proc initmainargs

    ; Ask the RIA for argv data; returns total byte count in AX.
    lda     #RIA_OP_ARGV
    sta     RIA_OP
    jsr     RIA_SPIN

    ; Request memory; __argv_mem may clobber.
    phx
    pha
    jsr     ___argv_mem
    sta     __argv
    stx     __argv+1
    sta     ptr1
    stx     ptr1+1
    sta     ptr2
    stx     ptr2+1
    ply
    plx

    ; Bail if no memory.
    ora     ptr1+1
    beq     bail

    ; Pop X:Y bytes from RIA_XSTACK into memory.
    tya
    beq     fillloop
    inx
fillloop:
    lda     RIA_XSTACK
    sta     (ptr2)
    inc     ptr2
    bne     :+
    inc     ptr2+1
:   dey
    bne     fillloop
    dex
    bne     fillloop

    ; Walk the pointer table: relocate each offset to an absolute address
    ; and count argc. The RIA stores offsets relative to the buffer start;
    ; adding __argv turns them into usable pointers.
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
    lda     ptr1            ; carry is clear, the buffer ends below $10000
    adc     #2
    sta     ptr1
    bcc     walkloop
    inc     ptr1+1
    bra     walkloop

bail:
    lda     #RIA_OP_DROP_XSTACK
    sta     RIA_OP

done:
    rts

.endproc
