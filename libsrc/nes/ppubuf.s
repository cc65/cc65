;
; Written by Groepaz/Hitmen <groepaz@gmx.net>
; Cleanup by Ullrich von Bassewitz <uz@cc65.org>
;


        .export         ppubuf_waitempty
        .export         ppubuf_wait
        .export         ppubuf_put
        .export         ppubuf_flush
        .include        "nes.inc"

.code

; ------------------------------------------------------------------------
; ppubuf_waitempty
; Wait until buffer is empty

.proc   ppubuf_waitempty


@wait:  lda     ringcount
        bne     @wait
        rts

.endproc


; ------------------------------------------------------------------------
; ppubuf_wait
; Wait until buffer is full

.proc   ppubuf_wait

        lda     #$ff            ; (($0100/3)*1)
@wait:  cmp     ringcount
        beq     @wait
        rts

.endproc

; ------------------------------------------------------------------------
; Put a PPU-Memory write to buffer
; called from main program (not necessary when in vblank irq)

.proc   ppubuf_put

        sta     ppuval
        sty     ppuhi
        stx     ppulo

        jsr     ppubuf_wait             ; wait if buffer is full

        ldy     ringwrite
        lda     ppuhi
        sta     ringbuff,y
        lda     ppulo
        sta     ringbuff+$0100,y
        lda     ppuval
        sta     ringbuff+$0200,y

        iny
        sty     ringwrite
        inc     ringcount
        rts

.endproc

; ------------------------------------------------------------------------
; Flush PPU-Memory write buffer
; called from vblank interupt

.proc   ppubuf_flush

        ldy     ringcount
        bne     @doloop
        rts

@doloop:
        ldx     ringread
        lda     #$0e
        sta     temp

@loop:
.repeat 5
        lda     ringbuff,x
        sta     $2006
        lda     ringbuff+$0100,x
        sta     $2006
        lda     ringbuff+$0200,x
        sta     $2007
        inx

        dey
        beq     @end
.endrepeat

        dec     temp
        bne     @loop

@end:   stx     ringread
        sty     ringcount

        rts     

.endproc

; ------------------------------------------------------------------------
; Data

.bss

temp:   .res    1


