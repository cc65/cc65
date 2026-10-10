;
; Written by Groepaz/Hitmen <groepaz@gmx.net>
; Cleanup by Ullrich von Bassewitz <uz@cc65.org>
; Optimized by Brandon Woodward
;

        .export         ppubuf_put
        .include        "nes.inc"
        .include        "zeropage.inc"

.code

; ------------------------------------------------------------------------
; Put a PPU-Memory write to buffer
; called from main program (not necessary when in vblank irq)

.proc   ppubuf_put
        stx     tmp4

; If buffer is full, wait for it to free up

@wait:  ldx     ppust_count
; $45 is the largest number that didn't cause glitches, $44 gives a bit more
; breathing room. see issue #1703
        cpx     #$44
        beq     @wait

        sta     ppust_buff+$00,x        ; A = value
        tya
        sta     ppust_buff+$50,x        ; Y = low byte
        lda     tmp4
        sta     ppust_buff+$A0,x        ; X = high byte

        inc     ppust_count
; Failsafe: did ppust_count change due to VBLANK?
        cpx     ppust_count
        bcs     @wait

        rts

.endproc
