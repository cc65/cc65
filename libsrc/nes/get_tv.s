;
; Stefan Haubenthal, 2007-01-21
;
; unsigned char get_tv (void);
; /* Return the video mode the machine is using */
;

        .include        "nes.inc"
        .include        "get_tv.inc"
        .importzp       tmp1, tmp2

;--------------------------------------------------------------------------
; _get_tv

.proc   _get_tv

        lda     #TV::NTSC       ; assume NTSC
        tax                     ; clear
        sta     tmp1
        sta     tmp2
        ldy     tickcount
wait:   cpy     tickcount
        beq     wait
        iny                     ; next tick
loop:   cpy     tickcount
        bne     over
        inc     tmp1
        bne     loop
        inc     tmp2
        bne     loop
over:   ldy     tmp2
        cpy     #9
        bcc     nopal
        lda     #TV::PAL        ; choose PAL
nopal:  rts

.endproc
