;
; Written by Groepaz/Hitmen <groepaz@gmx.net>
; Cleanup by Ullrich von Bassewitz <uz@cc65.org>
;
; void clrscr (void);
;

        .export         _clrscr
        .import         ppubuf_waitempty

        .include        "nes.inc"


.proc   _clrscr

; wait until all console data has been written

        jsr     ppubuf_waitempty

; wait for vblank

        lda     #0
        sta     VBLANK_FLAG
@w2:    lda     VBLANK_FLAG
        beq     @w2

; switch screen off

        lda     #%00000000
        sta     PPU_CTRL2

; Set start address to Name Table #1

        lda     #$20
        sta     PPU_VRAM_ADDR2
        lda     #$00
        sta     PPU_VRAM_ADDR2

; Clear Name Table #1

        lda     #' '
        ldx     #$f0            ; 4*$f0=$03c0

beg:    sta     PPU_VRAM_IO
        sta     PPU_VRAM_IO
        sta     PPU_VRAM_IO
        sta     PPU_VRAM_IO
        dex
        bne     beg

        lda     #$23            ;
        sta     PPU_VRAM_ADDR2  ; Set start address to PPU address $23C0
        lda     #$C0            ; (1st attribute table)
        sta     PPU_VRAM_ADDR2

        ldx     #$00

lll:    lda     #$00            ; Write attribute table value and auto increment
        sta     PPU_VRAM_IO     ; to next address
        inx
        cpx     #$40
        bne     lll

; switch screen on again

        lda     #%00011110
        sta     PPU_CTRL2
        rts

.endproc


