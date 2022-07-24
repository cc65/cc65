;*
;* clrscr
;*
;* NB: All screen functions assume Graphics Mode 1 in a default configuration.
;* Therefore, this is hard coded to use $1000-$12FF as screen VRAM.

        .export         _clrscr
        .include        "creativision.inc"

_clrscr:

        sei             ; Disable interrupts. Default INT handler reads VDP_STATUS
                        ; and would lose any setup done here.

        lda     #$00    ; VRAM offset low
        sta     VDP_CONTROL_W

        lda     #$50    ; VRAM offset high ($10 OR $40)
        sta     VDP_CONTROL_W

        lda     #$40    ; Space char from ROM setup

        ldx     #0
        ldy     #3

L1:     sta     VDP_DATA_W
        inx
        bne     L1
        dey
        bne     L1

        cli             ; Let interrupts go again

        lda     #0
        sta     CURSOR_X
        sta     CURSOR_Y
        sta     SCREEN_PTR
        lda     #$10
        sta     SCREEN_PTR+1

        rts
