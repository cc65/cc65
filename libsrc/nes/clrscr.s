;
; Written by Groepaz/Hitmen <groepaz@gmx.net>
; Cleanup by Ullrich von Bassewitz <uz@cc65.org>
; Some optimizations by Brandon Woodward
;
; void clrscr (void);
;

        .export         _clrscr, clrscr_skipvsync

        .include        "nes.inc"
        .import         _waitvsync


.proc   _clrscr

; wait for vblank

        jsr     _waitvsync

.endproc
.proc   clrscr_skipvsync

; switch screen off

;       ldx     #%00000000      ; X already 0
        stx     PPU_CTRL2

; Set VRAM address to Nametable #1

        lda     #>$2000
        sta     PPU_VRAM_ADDR2
;       ldx     #<$0000         ; X already 0
        stx     PPU_VRAM_ADDR2

; Clear Nametable & Attribute table #1 (TODO test)

;       ldx     #$00            ; X already 0
.assert ' ' = >$2000, error, "' ' is not $20"
;       lda     #' '            ;Coincidentally, A already happens to have the right value
ntloop: dex
        sta     PPU_VRAM_IO     ;write $3C0 chars to the nametable
        sta     PPU_VRAM_IO
        sta     PPU_VRAM_IO
        sta     PPU_VRAM_IO
        cpx     #$11
        bcs     ntloop
        lda     #0              ; For the last 64 bytes, write 0 to the attribute table
        dex
        bpl     ntloop+1        ; Skip the dex since we just did one

        lda     #$80
        sta     sprdma_en       ;OAM contents have decayed by now so it needs to be refreshed

; switch screen on again during VBLANK

        lda     #PPU_RENDER_EN
        sta     ppuctrl2_buf
        rts

.endproc
